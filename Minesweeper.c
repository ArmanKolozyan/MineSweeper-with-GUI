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
Fills the 2D array (defined in the main function) with structs and gives default values to the struct members.
This function is only called when the user wants to start a new session from scratch and doesn't want to load the game from a file.
*/
void initialize_field(struct game_board *game_board) {
    for (int i = 0; i < game_board->rows; i++) {
        for (int j = 0; j < game_board->columns; j++) {
            struct cell *current_cell = &game_board->playing_field[i][j];
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
Dynamically allocates the playing field.
*/
struct cell **make_field(int rows, int columns) {
    struct cell **array2d = (struct cell **)calloc(rows, sizeof(struct cell *)); // struct *cell werkt blijkbaar niet
    for (int i = 0; i < rows; i++) {
        array2d[i] = (struct cell *)calloc(columns, sizeof(struct cell));
    }
    return array2d;
}

/*
Deallocates the dynamically allocated playing field.
*/
void deallocate_field(struct game_board *game_board) {
    for (int i = 0; i < game_board->rows; i++) {
        free(game_board->playing_field[i]);
    }
    free(game_board->playing_field);
}

/*
Deallocates all dynamically allocated memory.
*/
void deallocate_memory(struct game_board *game_board) {
    // GUI
    free_gui();
    //playing_vield
    deallocate_field(game_board);
}

/*
To prevent the player from immediately, in the first turn, stepping on a mine and losing the game, 
the first command is handled separately to ensure that the first given row and column of the player does not contain a mine.
*/
void initialize(struct game_board *game_board, int total_bombs, struct flags_info *flags_info, struct user_input *user_input) {
    initialize_field(game_board);
    initialize_gui(game_board->rows, game_board->columns);
    call_the_drawer(game_board);
    read_input();
    install_bombs(game_board, total_bombs, user_input);
    calculate_neighbours_bombs(game_board);
    process_input(game_board, total_bombs, flags_info, user_input);
}

int main(int argc, const char *argv[]) {
    const int max_filename_size = 50;
    char filename[max_filename_size];
    enum Boolean file_option = FALSE;
    int total_bombs = 0;
    struct game_board game_board;
    struct flags_info flags_info;
    struct user_input *user_input = provide_user_input();

    if (!handle_initial_arguments(argc, argv, &game_board, &total_bombs, filename, &file_option)) { //something went wrong when reading the user-input
        exit(EXIT_FAILURE);
    }

    // dynamic allocation of the playing field
    game_board.playing_field = make_field(game_board.rows, game_board.columns);

    /* The row and column chosen by the player are initialized by a random number. 
       Since when installing the mines we prevent the player from immediately, 
       in the first turn, stepping on a mine and losing the game, 
       this ensures that the function runs correctly even though the player's 
       first command is the print command. */
    user_input->row = rand() % game_board.rows;
    user_input->column = rand() % game_board.columns;
    flags_info.placed_flags = 0;
    flags_info.correct_placed_flags = 0;

    if (file_option) { // user wants to start game session from own file
        decode(&game_board, &total_bombs, filename, &flags_info);
        initialize_gui(game_board.rows, game_board.columns);
    } else {
        initialize(&game_board, total_bombs, &flags_info, user_input);
    }

    printf("Remaining flags: %i\n", total_bombs - flags_info.placed_flags);

    // game-loop
    while (!has_player_lost() && !has_player_won() && user_input->should_continue) {
        encode(&game_board, &flags_info);
        call_the_drawer(&game_board);
        read_input();
        process_input(&game_board, total_bombs, &flags_info, user_input);
    }

    // after winning/losing the game, the whole revealed field is drawn
    if (has_player_won()) {
        printf("\nYOU WON! GOOD JOB!\n");
        draw_field(&game_board, TRUE);
    }
    if (has_player_lost()) {
        printf("\nGAME OVER!\n");
        draw_field(&game_board, TRUE);
    }

    while (user_input->should_continue)
        read_input();

    // memory deallocation
    deallocate_memory(&game_board);
}