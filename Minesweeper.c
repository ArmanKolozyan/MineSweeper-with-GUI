// -------------------------------------------------------------------
// game:     MineSweeper (Command Line Version)
// author:   Arman Kolozyan
// year:     2021-2022
// course:   Structuur van Computerprogramma's II
// -------------------------------------------------------------------

#include "cell.h"
#include "datatypes.h"
#include "encoder_decoder.h"
#include "gui.h"
#include "handle_input.h"
#include <stdio.h>
#include <stdlib.h> // for the rand-function & srand-function
#include <time.h>   // for the time-function (needed for seeding the random-generator)

extern int USER_ROW;
extern int USER_COLUMN;

/*
2 global variables that are modified by external functions (see cell.c) when the player has won/lost.
*/
enum Boolean GAME_OVER;
enum Boolean GAME_WON;

/* 
Fills the 2D array (defined in the main function) with structs and gives default values to the struct members.
This function is only called when the user wants to start a new session from scratch and doesn't want to load the game from a file.
*/
void initialize_field(int rows, int columns, struct cell playing_field[rows][columns]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            struct cell *current_cell = &playing_field[i][j];
            current_cell->bomb = FALSE;
            current_cell->revealed = FALSE;
            current_cell->flagged = FALSE;
            current_cell->neighbours_count = 0;
        }
    }
}

/* 
Places the number of bombs specified by the user.
To prevent the player from immediately, in the first turn, stepping on a mine and losing the game, this function receives the 
row and column of the first call of reveal/flag. If the first call is a print, then it receives a random row and column.
*/
void install_bombs(int total_bombs, int rows, int columns, struct cell playing_field[rows][columns]) {
    srand(time(0)); //to seed rand with a starting value
    int placed_bombs = 0;
    while (placed_bombs != total_bombs) {
        int bomb_row = rand() % rows;
        int bomb_column = rand() % columns;
        if ((bomb_row != USER_ROW) || (bomb_column != USER_COLUMN)) {
            struct cell *bomb_cell = &playing_field[bomb_row][bomb_column];
            if (!bomb_cell->bomb) { // to ensure that all bombs are placed in different cells
                bomb_cell->bomb = TRUE;
                placed_bombs++;
            }
        }
    }
}

/*
To prevent the player from immediately, in the first turn, stepping on a mine and losing the game, 
the first command is handled separately to ensure that the first given row and column of the player does not contain a mine.
*/
void initialize(int total_bombs, int rows, int columns, struct cell playing_field[rows][columns], int *placed_flags, int *correct_placed_flags) {
    initialize_field(playing_field);
    initialize_gui();
    draw_field(playing_field, FALSE);
    read_input();
    install_bombs(playing_field);
    calculate_neighbours_bombs(playing_field);
    process_input(playing_field, placed_flags, correct_placed_flags);
}

int main(int argc, const char *argv[]) {
    int placed_flags = 0;
    int correct_placed_flags = 0; // if correct_placed_flags == total_bombs => the player has won
    const int max_filename_size = 50;
    char filename[max_filename_size];
    enum Boolean file_option = FALSE;
    int total_bombs;
    extern int should_continue;
    int rows;
    int columns;

    if (!handle_initial_arguments(argc, argv, filename, &file_option)) { //something went wrong when reading the user-input
        exit(EXIT_FAILURE);
    }

    /* The row and column chosen by the player will be initialized by a random number. 
       Since when installing the mines we prevent the player from immediately, 
       in the first turn, stepping on a mine and losing the game, 
       this ensures that the function runs correctly even though the player's 
       first command is the print command. */
    USER_ROW = rand() % rows;
    USER_COLUMN = rand() % columns;

    // dynamic allocation of the playing field
    struct cell(*playing_field)[columns] = malloc(sizeof(struct cell[rows][columns])); // see: https://stackoverflow.com/a/12805980/14043571

    if (file_option) { // user wants to start game session from own file
        decode(filename, playing_field, &placed_flags, &correct_placed_flags);
        initialize_gui();
    } else {
        initialize(playing_field, &placed_flags, &correct_placed_flags);
    }

    // game-loop
    while (!GAME_OVER && !GAME_WON && should_continue) {
        encode(playing_field, placed_flags, correct_placed_flags);
        call_the_drawer(playing_field);
        read_input();
        process_input(playing_field, &placed_flags, &correct_placed_flags);
    }

    // after winning/losing the game, the whole revealed field is drawn
    if (GAME_WON) {
        printf("\nYOU WON! GOOD JOB!\n");
        draw_field(playing_field, TRUE);
    }
    if (GAME_OVER) {
        printf("\nGAME OVER!\n");
        draw_field(playing_field, TRUE);
    }

    if (should_continue)
        handle_game_end();

    // memory deallocation
    free_gui();
    free(playing_field);
}