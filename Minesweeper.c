// -------------------------------------------------------------------
// game:     MineSweeper (Command Line Version)
// author:   Arman Kolozyan
// year:     2021-2022
// course:   Structuur van Computerprogramma's II
// -------------------------------------------------------------------

#include "cell.h"
#include "datatypes.h"
#include "gui.h"
#include "handle_input.h"
#include "encoder_decoder.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h> // for the rand- & srand-function
#include <string.h> // for the strcpy-function
#include <time.h>   // for the time-function (needed for seeding the random-generator)

int ROWS;
int COLUMNS;
int TOTAL_BOMBS;
extern int USER_ROW;
extern int USER_COLUMN;
extern int SHOULD_CONTINUE;

/*
2 global variables that are modified by external functions (see cell.c) when the player has won/lost.
*/
enum Boolean GAME_OVER;
enum Boolean GAME_WON;

/* 
Fills the 2D array (defined in the main function) with structs and gives default values to the struct members.
This function is only called when the user wants to start a new session from scratch and doesn't want to load the game from a file.
*/
void initialize_field(struct cell playing_field[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
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
void install_bombs(struct cell playing_field[ROWS][COLUMNS]) {
    srand(time(0)); //to seed rand with a starting value
    int placed_bombs = 0;
    while (placed_bombs != TOTAL_BOMBS) {
        int bomb_row = rand() % ROWS;
        int bomb_column = rand() % COLUMNS;
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
void initialize(struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags) {
    initialize_field(playing_field);
    initialize_gui();
    draw_field(playing_field, FALSE);
    read_input();
    install_bombs(playing_field);
    calculate_neighbours_bombs(playing_field);
    process_input(playing_field, placed_flags, correct_placed_flags);
    call_the_drawer(playing_field);
}

int main(int argc, const char *argv[]) {
    GAME_WON = FALSE;
    GAME_OVER = FALSE;
    TOTAL_BOMBS = 0;    //To initialize the variables when starting a new game-session.
    int placed_flags = 0;
    int correct_placed_flags = 0; // if correct_placed_flags == TOTAL_BOMBS => the player has won

    const int filaneme_size = 50;
    char filename[filaneme_size];
    enum Boolean file_flag = FALSE;

    get_initial_arguments(argc, argv, filename, &file_flag);

    USER_ROW = rand() % ROWS;
    USER_COLUMN = rand() % COLUMNS;

    struct cell(*playing_field)[COLUMNS] = malloc(sizeof(struct cell[ROWS][COLUMNS]));

    if (file_flag) {
        decode(filename, playing_field, &placed_flags, &correct_placed_flags);
        initialize_gui();
        call_the_drawer(playing_field);
    } else {
        initialize(playing_field, &placed_flags, &correct_placed_flags);
    }

    while (!GAME_OVER && !GAME_WON && SHOULD_CONTINUE) {
        read_input();
        process_input(playing_field, &placed_flags, &correct_placed_flags);
        call_the_drawer(playing_field);
    }
    // after winning/losing the game, the whole revealed field is printed
    if (GAME_WON) {
        printf("\nYOU WON! GOOD JOB!\n");
        draw_field(playing_field, TRUE);
        read_input();
    }
    if (GAME_OVER) {
        printf("\nGAME OVER!\n");
        draw_field(playing_field, TRUE);
        read_input();
    }
}