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

/*
2 global variables that are modified by external functions (see cell.c) when the player has won/lost.
*/
enum Boolean GAME_OVER;
enum Boolean GAME_WON;

/* 
Fills the 2D array (defined in the main function) with structs and gives default values to the struct members.
This function is only called when the user wants to start a new session from scratch and doesn't want to load the game from a file.
*/
void initialize_field(int rows, int columns, struct cell **playing_field) {
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
void install_bombs(struct game_board *game_board, int total_bombs, struct user_input *user_input) {
    srand(time(0)); //to seed rand with a starting value
    int placed_bombs = 0;
    while (placed_bombs != total_bombs) {
        int bomb_row = rand() % game_board->rows;
        int bomb_column = rand() % game_board->columns;
        if ((bomb_row != user_input->row) || (bomb_column != user_input->column)) {
            struct cell *bomb_cell = &game_board->playing_field[bomb_row][bomb_column];
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
void initialize(struct game_board *game_board, int total_bombs, int *placed_flags, int *correct_placed_flags, struct user_input *user_input) {
    initialize_field(game_board);
    initialize_gui(game_board->rows, game_board->columns);
    call_the_drawer(game_board);
    read_input();
    install_bombs(game_board, total_bombs, user_input);
    calculate_neighbours_bombs(game_board);
    process_input(game_board, total_bombs, placed_flags, correct_placed_flags, user_input);
}

int main(int argc, const char *argv[]) {
    int placed_flags = 0;
    int correct_placed_flags = 0; // if correct_placed_flags == total_bombs => the player has won
    const int max_filename_size = 50;
    char filename[max_filename_size];
    enum Boolean file_option = FALSE;
    int total_bombs;
    struct game_board game_board;
    struct flags_info flags_info;
    struct user_input *user_input;


    if (!handle_initial_arguments(argc, argv, &game_board, &total_bombs, filename, &file_option)) { //something went wrong when reading the user-input
        exit(EXIT_FAILURE);
    }

    // dynamic allocation of the playing field
    ////// struct cell(*playing_field)[columns] = malloc(sizeof(struct cell[rows][columns])); // see: https://stackoverflow.com/a/12805980/14043571

    provide_user_input(&user_input);

    
    /* The row and column chosen by the player are initialized by a random number. 
       Since when installing the mines we prevent the player from immediately, 
       in the first turn, stepping on a mine and losing the game, 
       this ensures that the function runs correctly even though the player's 
       first command is the print command. */
    user_input.row = rand() % game_board.rows;
    user_input.column = rand() % game_board.columns;

    
    flags_info.placed_flags = 0;
    flags_info.correct_placed_flags = 0;
    game_board.playing_field = playing_field;

    if (file_option) { // user wants to start game session from own file
        decode(&rows, &columns, playing_field, &total_bombs, filename, &placed_flags, &correct_placed_flags);
        initialize_gui(rows, columns);
    } else {
        initialize(&game_board, total_bombs, &flags_info, &user_input);
    }

    printf("Remaining flags: %i\n", total_bombs - placed_flags);

    // game-loop
    while (!GAME_OVER && !GAME_WON && user_input.should_continue) {
        encode(&game_board, &flags_info);
        call_the_drawer(&game_board);
        read_input();
        process_input(rows, columns, playing_field, total_bombs, &placed_flags, &correct_placed_flags);
    }

    // after winning/losing the game, the whole revealed field is drawn
    if (GAME_WON) {
        printf("\nYOU WON! GOOD JOB!\n");
        draw_field(rows, columns, playing_field, TRUE);
    }
    if (GAME_OVER) {
        printf("\nGAME OVER!\n");
        draw_field(rows, columns, playing_field, TRUE);
    }

    while (user_input.should_continue)
        read_input();

    // memory deallocation
    free_gui();
    for (int i = 0; i < rows; i++) {
        free(playing_field[i]);
    }
    free(playing_field);
}