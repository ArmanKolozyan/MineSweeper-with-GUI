#include "gui.h"
#include <SDL2/SDL.h> // for the SDL2-functions
#include <stdio.h>
#include <stdlib.h> // for the rand-function
#include <stdlib.h>

/*
The height and width of the window (in pixels).
*/
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500

/*
The height and width (in pixels) of the cell-images that are displayed in the playing field.
 */
#define IMAGE_HEIGHT 50
#define IMAGE_WIDTH 50

/* 
The number of rows and columns of the playing field determined by the player.
*/
extern int ROWS;
extern int COLUMNS;

/*
The variables below represent the player's events.
The row and column chosen by the player is initially a random number. Since when installing the mines we prevent the player from immediately, 
in the first turn, stepping on a mine and losing the game, this ensures that the function runs correctly even though the player's first command is the print command.
*/
enum Command USER_COMMAND;
int USER_ROW = rand() % ROWS;
int USER_COLUMN = rand() % COLUMNS;

/*
This renderer is used to draw figures in the window. 
The renderer is initialized in the "initialize_window" function.
 */
static SDL_Renderer *RENDERER;

/*
The two variables below keep track of the last position where the user clicked.
 */
int MOUSE_X = 0;
int MOUSE_Y = 0;

/*
Indicates whether the application should continue. 
This is true as long as the user does not want to close the application by clicking the x button on the screen.
 */
enum Boolean SHOULD_CONTINUE = TRUE;

/*
This is the window that will be shown and in which the playing field is displayed. 
This window is created when initializing the GUI and is aborted when the game ends.
 */
static SDL_Window *WINDOW;

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

/*
This auxiliary procedure is used in read_input to ensure that the command is reset to "nothing". 
Only if the previous command was the print command we do not need to reset it because read_input 
needs to know that the user has already asked for a print so that this time the playing field is reset to its original display.
*/
void reset_command(void) {
    if (USER_COMMAND != PRINT) {
        USER_COMMAND = NOTHING;
    }
}

/*
The three functions below provide abstractions to aid code readability. 
In addition, it is possible to customize the controls of the game below.
*/
int is_print_input(SDL_Event event) {
    event.key.keysym.sym == SDLK_p;
}
int is_reveal_input(SDL_Event event) {
    event.button.button == SDL_BUTTON_LEFT;
}
int is_flag_input(SDL_Event event) {
    event.button.button == SDL_BUTTON_RIGHT;
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
        if (is_print_input(event)) {
            if (USER_COMMAND == PRINT) {
                USER_COMMAND = NOTHING;
            } else {
                USER_COMMAND = PRINT;
            }
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        USER_COLUMN = event.button.x / IMAGE_WIDTH;
        USER_ROW = event.button.y / IMAGE_HEIGHT;
        if (is_reveal_input(event)) {
            USER_COMMAND = REVEAL;
        } else if (is_flag_input(event)) {
            USER_COMMAND = FLAG;
        }
        break;

    case SDL_QUIT:
        SHOULD_CONTINUE = FALSE;
        break;
    }
}


/* 
Calls the "draw_field" function to draw the field. 
*/
void call_the_drawer(struct cell playing_field[ROWS][COLUMNS]) {
    if (USER_COMMAND == PRINT) {
        draw_field(playing_field, TRUE);
    } else {
        draw_field(playing_field, FALSE);
    }
}

/*
Draws the playing field.
*/
void draw_field(struct cell playing_field[ROWS][COLUMNS], enum Boolean reveal_all) {

    SDL_RenderClear(RENDERER);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
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
void initialize_gui() {
    initialize_window("MineSweeper");
    initialize_textures();
}
