#include "gui.h"
#include <stdlib.h>

enum Command command;
int user_row;    // will be overwritten if command is 'r' or 'f'
int user_column; // will be overwritten if command is 'r' or 'f'
extern int ROWS;
extern int COLUMNS;

/*
This renderer is used to draw figures in the window. 
The renderer is initialized in the initialize_window function.
 */
static SDL_Renderer *renderer;

SDL_Texture *textures_array[12];

/*
static SDL_Texture *digit_0_texture = NULL;
static SDL_Texture *digit_1_texture = NULL;
static SDL_Texture *digit_2_texture = NULL;
static SDL_Texture *digit_3_texture = NULL;
static SDL_Texture *digit_4_texture = NULL;
static SDL_Texture *digit_5_texture = NULL;
static SDL_Texture *digit_6_texture = NULL;
static SDL_Texture *digit_7_texture = NULL;
static SDL_Texture *digit_8_texture = NULL;
static SDL_Texture *covered_texture = NULL;
static SDL_Texture *flagged_texture = NULL;
static SDL_Texture *bomb_texture = NULL;
*/
SDL_Texture *Textures_array[12];

int is_relevant_event(SDL_Event *event) {
    if (event == NULL) {
        return 0;
    }
    return (event->type == SDL_MOUSEBUTTONDOWN) ||
           (event->type == SDL_KEYDOWN) ||
           (event->type == SDL_QUIT);
}

/*
 * Onderstaande twee lijnen maken deel uit van de minimalistische voorbeeldapplicatie:
 * ze houden de laatste positie bij waar de gebruiker geklikt heeft.
 */
int mouse_x = 0;
int mouse_y = 0;

/*
 * Geeft aan of de applicatie moet verdergaan.
 * Dit is waar zolang de gebruiker de applicatie niet wilt afsluiten door op het kruisje te klikken.
 */
int should_continue = 1;

/*
 * Dit is het venster dat getoond zal worden en waarin het speelveld weergegeven wordt.
 * Dit venster wordt aangemaakt bij het initialiseren van de GUI en wordt weer afgebroken
 * wanneer het spel ten einde komt.
 */
static SDL_Window *window;

/*
 * Vangt de input uit de GUI op. Deze functie is al deels geïmplementeerd, maar je moet die zelf
 * nog afwerken. Je mag natuurlijk alles aanpassen aan deze functie, inclusies return-type en argumenten.
 */
void read_input() {
    SDL_Event event;
    if (command != PRINT) {
        command = NOTHING;
    }

    /*
	 * Handelt alle input uit de GUI af.
	 * Telkens de speler een input in de GUI geeft (bv. een muisklik, muis bewegen, toets indrukken enz.)
	 * wordt er een 'event' (van het type SDL_Event) gegenereerd dat hier wordt afgehandeld.
	 *
	 * Zie ook https://wiki.libsdl.org/SDL_PollEvent en http://www.parallelrealities.co.uk/2011_09_01_archive.html
	 */
    while (!SDL_PollEvent(&event) || !is_relevant_event(&event)) {
    }

    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_p) {
            if (command == PRINT) {
                command = NOTHING;
            } else {
                command = PRINT;
            }
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        user_column = event.button.x / 50;
        user_row = event.button.y / 50;
        if (event.button.button == SDL_BUTTON_LEFT) {
            command = REVEAL;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
            command = FLAG;
        }
        break;
    case SDL_QUIT:
        /* De gebruiker heeft op het kruisje van het venster geklikt om de applicatie te stoppen. */
        should_continue = 0;
        break;

        // case SDL_MOUSEBUTTONDOWN:
        //     /*
        // 	 * De speler heeft met de muis geklikt: met de onderstaande lijn worden de coördinaten in het
        // 	 * het speelveld waar de speler geklikt heeft bewaard in de variabelen mouse_x en mouse_y.
        // 	 */
        //     mouse_x = event.button.x;
        //     mouse_y = event.button.y;
        //     printf("Clicked at (%i,%i)\n", mouse_x, mouse_y);
        //     break;
    }
}

/* 
Calls the "print_field" function to print the whole field. The second paramater indicates whether the 
field has to be revealed. 
*/
void call_the_drawer(struct cell playing_field[ROWS][COLUMNS]) {
    if (command == PRINT) {
        draw_field(playing_field, TRUE);
    } else {
        draw_field(playing_field, FALSE);
    }
}

void draw_field(struct cell playing_field[ROWS][COLUMNS], enum Boolean reveal_all) { // ook flags tonen?

    SDL_RenderClear(renderer);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
            SDL_Rect rectangle = {j * IMAGE_WIDTH, i * IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT};
            if (current_cell->revealed || current_cell->flagged || reveal_all) {
                if (current_cell->flagged && !reveal_all) { // sequence of conidition checks plays a crucial role, because otherwise a flagged cell would contain the neighbours_count on the screen
                    SDL_RenderCopy(renderer, Textures_array[10], NULL, &rectangle);
                } else if (!current_cell->bomb) {
                    SDL_RenderCopy(renderer, Textures_array[current_cell->neighbours_count], NULL, &rectangle);
                } else {
                    SDL_RenderCopy(renderer, Textures_array[11], NULL, &rectangle);
                }
            } else {
                SDL_RenderCopy(renderer, Textures_array[9], NULL, &rectangle);
            };
        }
    }
    SDL_RenderPresent(renderer);
}

void draw_window() {
    /*
	 * Maakt het venster blanco.
	 */
    SDL_RenderClear(renderer);

    //  /*
    //  * Bereken de plaats (t.t.z., de rechthoek) waar een afbeelding moet getekend worden.
    //  * Dit is op de plaats waar de gebruiker het laatst geklikt heeft.
    //  */
    //  SDL_Rect rectangle = {mouse_x, mouse_y, IMAGE_WIDTH, IMAGE_HEIGHT};
    //  /* Tekent de afbeelding op die plaats. */
    //  SDL_RenderCopy(renderer, digit_1_texture, NULL, &rectangle);
    //
    //  SDL_Rect rectanglee = {mouse_x + IMAGE_WIDTH, mouse_y + IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT};
    //  /* Tekent de afbeelding op die plaats. */
    //  SDL_RenderCopy(renderer, digit_1_texture, NULL, &rectanglee);

    /*
	 * Onderstaande code moet zeker worden uitgevoerd op het einde van deze functie.
	 * Wanneer je iets tekent in het venster wordt dit venster nl. niet meteen aangepast.
	 * Via de SDL_RenderPresent functie wordt het venster correct geüpdatet.
	 */
    SDL_RenderPresent(renderer);
}

/*
 * Initialiseert het venster en alle extra structuren die nodig zijn om het venster te manipuleren.
 */
void initialize_window(const char *title) {
    /*
	 * Code o.a. gebaseerd op:
	 * http://lazyfoo.net/tutorials/SDL/02_getting_an_image_on_the_screen/index.php
	 */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Maak het venster aan met de gegeven dimensies en de gegeven titel. */
    window = SDL_CreateWindow(title, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        /* Er ging iets verkeerd bij het initialiseren. */
        printf("Couldn't set screen mode to required dimensions: %s\n", SDL_GetError());
        exit(1);
    }

    /* Initialiseert de renderer. */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    /* Laat de default-kleur die de renderer in het venster tekent wit zijn. */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

/*
 * Dealloceert alle SDL structuren die geïnitialiseerd werden.
 */
void free_gui() {
    /* Dealloceert de SDL_Textures die werden aangemaakt. */
    for (int i = 0; i < 12; i++) {
        SDL_DestroyTexture(Textures_array[i]);
    }
    /* Dealloceert het venster. */
    SDL_DestroyWindow(window);
    /* Dealloceert de renderer. */
    SDL_DestroyRenderer(renderer);

    /* Sluit SDL af. */
    SDL_Quit();
}

/*
 * Laadt alle afbeeldingen die getoond moeten worden in.
 */
void initialize_textures() {
    /*
	 * Laadt de afbeeldingen in. In deze minimalistische applicatie laden we slechts 1 afbeelding in.
	 * Indien de afbeelding niet kon geladen worden (bv. omdat het pad naar de afbeelding verkeerd is),
	 * geeft SDL_LoadBMP een NULL-pointer terug.
	 */
    /*
    SDL_Surface *digit_0_surface = SDL_LoadBMP("Images/0.bmp");
    SDL_Surface *digit_1_surface = SDL_LoadBMP("Images/1.bmp");
    SDL_Surface *digit_2_surface = SDL_LoadBMP("Images/2.bmp");
    SDL_Surface *digit_3_surface = SDL_LoadBMP("Images/3.bmp");
    SDL_Surface *digit_4_surface = SDL_LoadBMP("Images/4.bmp");
    SDL_Surface *digit_5_surface = SDL_LoadBMP("Images/5.bmp");
    SDL_Surface *digit_6_surface = SDL_LoadBMP("Images/6.bmp");
    SDL_Surface *digit_7_surface = SDL_LoadBMP("Images/7.bmp");
    SDL_Surface *digit_8_surface = SDL_LoadBMP("Images/8.bmp");
    SDL_Surface *covered_surface = SDL_LoadBMP("Images/covered.bmp");
    SDL_Surface *flagged_surface = SDL_LoadBMP("Images/flagged.bmp");
    SDL_Surface *bomb_surface = SDL_LoadBMP("Images/mine.bmp");
    */

    SDL_Surface *Surface_array[] = {
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
    /*
	 * Zet deze afbeelding om naar een texture die getoond kan worden in het venster.
	 * Indien de texture niet kon omgezet worden, geeft de functie een NULL-pointer terug.
	 */
    for (int i = 0; i < 12; i++) {
        Textures_array[i] = SDL_CreateTextureFromSurface(renderer, Surface_array[i]);
        SDL_FreeSurface(Surface_array[i]);
    }
}

/*
 * Initialiseert onder het venster waarin het speelveld getoond zal worden, en de texture van de afbeelding die getoond zal worden.
 * Deze functie moet aangeroepen worden aan het begin van het spel, vooraleer je de spelwereld begint te tekenen.
 */
void initialize_gui() {
    initialize_window("Minesweeper");
    initialize_textures();
}

// int main(int argc, char *argv[]) {
//     initialize_gui();
//     draw_field();
//     while (should_continue) {
//         read_input();
//         //		draw_window()
//     }
//     /* Dealloceer al het geheugen dat werd aangemaakt door SDL zelf. */
//     free_gui();
//     return 0;
// }
