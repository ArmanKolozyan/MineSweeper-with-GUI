#include "gui.h"
#include <SDL2/SDL.h> // for the SDL2-functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for the strcat-function

/*
The height and width (in pixels) of the cell-images that are displayed in the playing field.
 */
#define IMAGE_HEIGHT 50
#define IMAGE_WIDTH 50

/*
The controls of the game that you can easily customize by changing these macros.
*/
#define PRINT_KEY SDLK_p
#define REVEAL_KEY SDL_BUTTON_LEFT
#define FLAG_KEY SDL_BUTTON_RIGHT

/*
Textures-array contains all the textures. 
They are placed in an array to easily grab the texture depending on the number of neighbours of a cell.
*/
#define AMOUNT_OF_TEXTURES 12
#define HIDDEN_TEXTURE_INDEX 9
#define FLAG_TEXTURE_INDEX 10
#define MINE_TEXTURE_INDEX 11
SDL_Texture *TEXTURES_ARRAY[AMOUNT_OF_TEXTURES];

/*
The height and width of the window (in pixels).
*/
static int WINDOW_HEIGHT;
static int WINDOW_WIDTH;

/*
The variable below represent the player's events.
The struct members are: row, column and command of the user.
*/
static struct user_input USER_INPUT;

/*
This renderer is used to draw figures in the window. 
The renderer is initialized in the "initialize_window" function.
 */
static SDL_Renderer *RENDERER;

/*
The two variables below keep track of the last position where the user clicked.
 */
static int MOUSE_X = 0;
static int MOUSE_Y = 0;

/*
This is the window that will be shown and in which the playing field is displayed. 
This window is created when initializing the GUI and is aborted when the game ends.
 */
static SDL_Window *WINDOW;

/*
This auxiliary procedure is used in read_input to take actions only on relevant input from the user.
Relevant events are:
mouse-clicks
keyboard-input
pressing on x button on the screen
*/
int is_relevant_event(SDL_Event *event) {
    if (event == NULL) {
        return 0;
    }
    return (event->type == SDL_MOUSEBUTTONDOWN) ||
           (event->type == SDL_KEYDOWN) ||
           (event->type == SDL_QUIT);
}

struct user_input *provide_user_input(void) {
    return &USER_INPUT;
}

/*
This auxiliary procedure is used in read_input to ensure that the command is reset to "nothing". 
Only if the previous command was the print command we do not need to reset it because read_input 
needs to know that the user has already asked for a print so that this time the playing field is reset to its original display.
*/
void reset_command(void) {
    if (USER_INPUT.command != PRINT) {
        USER_INPUT.command = NOTHING;
    }
}

void provide_user_input(struct user_input *input_struct) {
    input_struct = &USER_INPUT;
}

/*
Receives input from the GUI.
 */
void read_input() {
    SDL_Event event;

    reset_command();

    while (!SDL_PollEvent(&event) || !is_relevant_event(&event)) {
        // do nothing
    }

    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == PRINT_KEY) {
            if (USER_INPUT.command == PRINT) {
                USER_INPUT.command = NOTHING;
            } else {
                USER_INPUT.command = PRINT;
            }
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        USER_INPUT.column = event.button.x / IMAGE_WIDTH;
        USER_INPUT.row = event.button.y / IMAGE_HEIGHT;
        if (event.button.button == REVEAL_KEY) {
            USER_INPUT.command = REVEAL;
        } else if (event.button.button == FLAG_KEY) {
            USER_INPUT.command = FLAG;
        }
        break;

    case SDL_QUIT:
        USER_INPUT.should_continue = FALSE;
        break;
    }
}

/* 
Calls the "draw_field" function to draw the field. 
*/
void call_the_drawer(struct game_board *game_board) {
    if (USER_INPUT.command == PRINT) {
        draw_field(game_board, TRUE);
    } else {
        draw_field(game_board, FALSE);
    }
}

/*
Draws the playing field.
*/
void draw_field(struct game_board *game_board, enum Boolean reveal_all) {

    SDL_RenderClear(RENDERER);
    for (int i = 0; i < game_board->rows; i++) {
        for (int j = 0; j < game_board->columns; j++) {
            struct cell *current_cell = &game_board->playing_field[i][j];
            SDL_Rect rectangle = {j * IMAGE_WIDTH, i * IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT};
            if (current_cell->revealed || current_cell->flagged || reveal_all) {
                if (current_cell->flagged && !reveal_all) { // sequence of condition checks plays a crucial role, because otherwise a flagged cell would contain the neighbours_count on the screen
                    SDL_RenderCopy(RENDERER, TEXTURES_ARRAY[FLAG_TEXTURE_INDEX], NULL, &rectangle);
                } else if (!current_cell->bomb) {
                    SDL_RenderCopy(RENDERER, TEXTURES_ARRAY[current_cell->neighbours_count], NULL, &rectangle);
                } else {
                    SDL_RenderCopy(RENDERER, TEXTURES_ARRAY[MINE_TEXTURE_INDEX], NULL, &rectangle);
                }
            } else {
                SDL_RenderCopy(RENDERER, TEXTURES_ARRAY[HIDDEN_TEXTURE_INDEX], NULL, &rectangle);
            };
        }
    }
    SDL_RenderPresent(RENDERER);
}

/*
Clears the screen and updates it to the new version.
*/
void draw_window() {

    SDL_RenderClear(RENDERER);
    SDL_RenderPresent(RENDERER);
}

/*
Initializes the window and any additional structures needed to manipulate the window.
 */
void initialize_window(const char *title) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    WINDOW = SDL_CreateWindow(title, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN); // creates the window

    if (WINDOW == NULL) {
        printf("Couldn't set screen mode to required dimensions: %s\n", SDL_GetError());
        exit(1);
    }

    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(RENDERER, 255, 255, 255, 255); // default color of the window (white)
}

/*
Deallocates all SDL-structures that were initialized.
 */
void free_gui() {

    for (int i = 0; i < AMOUNT_OF_TEXTURES; i++) {
        SDL_DestroyTexture(TEXTURES_ARRAY[i]); // textures
    }

    SDL_DestroyWindow(WINDOW); // the window

    SDL_DestroyRenderer(RENDERER); // the renderer

    SDL_Quit(); // closes SDL
}

/*
Loads all the images and converts them to textures.
 */
void initialize_textures() {

    SDL_Surface *images_array[AMOUNT_OF_TEXTURES] = {
        SDL_LoadBMP("Images/0.bmp"),
        SDL_LoadBMP("Images/1.bmp"),
        SDL_LoadBMP("Images/2.bmp"),
        SDL_LoadBMP("Images/3.bmp"),
        SDL_LoadBMP("Images/4.bmp"),
        SDL_LoadBMP("Images/5.bmp"),
        SDL_LoadBMP("Images/6.bmp"),
        SDL_LoadBMP("Images/7.bmp"),
        SDL_LoadBMP("Images/8.bmp"),
        SDL_LoadBMP("Images/covered.bmp"),
        SDL_LoadBMP("Images/flagged.bmp"),
        SDL_LoadBMP("Images/mine.bmp")};

    for (int i = 0; i < AMOUNT_OF_TEXTURES; i++) {
        if ((TEXTURES_ARRAY[i] = SDL_CreateTextureFromSurface(RENDERER, images_array[i])) == NULL) {
            printf("Image with index %d could not be converted to a texture.", i);
        }
        SDL_FreeSurface(images_array[i]);
    }
}

/*
Initializes the window and the textures of the images that will be displayed. 
This function is called at the beginning of the game, before the playing field is drawn.
 */
void initialize_gui(int rows, int columns) {
    USER_INPUT.should_continue = TRUE;
    WINDOW_HEIGHT = IMAGE_HEIGHT * rows;
    WINDOW_WIDTH = IMAGE_WIDTH * columns;
    initialize_window("MineSweeper");
    initialize_textures();
}
