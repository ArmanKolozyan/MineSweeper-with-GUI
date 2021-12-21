#include "cell.h"
#include <stdio.h>

static int remaining_nonbomb_cells;
static enum Boolean GAME_WON = FALSE;
static enum Boolean GAME_OVER = FALSE;

void calculate_neighbours_bombs(struct game_board *game_board) {
    for (int i = 0; i < game_board->rows; i++) {
        for (int j = 0; j < game_board->columns; j++) {
            struct cell *current_cell = &game_board->playing_field[i][j];
            if (current_cell->bomb) { // we don't have to count the neighbours of bombs
                continue;
            }
            {
                int neighbour_bombs = 0;
                for (int off_i = -1; off_i <= 1; off_i++) {
                    for (int off_j = -1; off_j <= 1; off_j++) { // checking all the neighbours of current_cell
                        int neighbour_i = i + off_i;
                        int neighbour_j = j + off_j;
                        if ((neighbour_i < 0) || (neighbour_i >= game_board->rows)) { // checking if we are not off the field
                            continue;
                        }
                        if ((neighbour_j < 0) || (neighbour_j >= game_board->columns)) { // checking if we are not off the field
                            continue;
                        }
                        {
                            struct cell *neighbour = &game_board->playing_field[neighbour_i][neighbour_j];
                            if (neighbour->bomb) {
                                neighbour_bombs++;
                            }
                        }
                    }
                }
                current_cell->neighbours_count = neighbour_bombs;
            }
        }
    }
}

/*
The two procedures below indicate whether the game is done by looking at the (static) global variables GAME_WON and GAME_OVER.
*/
enum Boolean has_player_won() {
    return GAME_WON;
}
enum Boolean has_player_lost() {
    return GAME_OVER;
}

void remove_flag(struct cell *the_cell, struct flags_info *flags_info) {
    the_cell->flagged = FALSE;
    (flags_info->placed_flags)--;
    if (the_cell->bomb) {
        (flags_info->correct_placed_flags)--;
    }
}

void place_flag(struct cell *the_cell, struct flags_info *flags_info, int total_bombs) {
    if (!the_cell->flagged) {
        if (the_cell->revealed) {
            printf("Action cannot be done. Cell is revealed.\n");
        } else if (flags_info->placed_flags < total_bombs) {
            the_cell->flagged = TRUE;
            (flags_info->placed_flags)++; // brackets are necessary since "++"" has higher precedence than "*"
            printf("Remaining flags: %i\n", total_bombs - flags_info->placed_flags);
            if (the_cell->bomb) {
                (flags_info->placed_flags)++;
                if (flags_info->correct_placed_flags == total_bombs) {
                    GAME_WON = 1;
                }
            }
        } else {
            printf("No flags left!\n");
        }
    } else {
        remove_flag(the_cell, flags_info); // if the cell was already flagged, the player gets his flag back
    }
}


/*
If a cell is revealed that does not contain a mine in any of its neighbouring cells, all those neighbouring cells are revealed too. This process also repeats for each of these neighbours 
whose neighbours_count equals zero.
*/
void reveal_neighbours(struct game_board *game_board, int row, int column, struct flags_info *flags_info) {
    for (int off_i = -1; off_i <= 1; off_i++) {
        for (int off_j = -1; off_j <= 1; off_j++) { // checking all the neighbours
            int neighbour_i = row + off_i;
            int neighbour_j = column + off_j;
            if ((neighbour_i < 0) || (neighbour_i >= game_board->rows)) { // checking if we are not off the field
                continue;
            }
            if ((neighbour_j < 0) || (neighbour_j >= game_board->columns)) { // checking if we are not off the field
                continue;
            }
            {
                struct cell *neighbour = &game_board->playing_field[neighbour_i][neighbour_j];
                if (!neighbour->revealed) // otherwise infinite loop (constantly asking each other to reveal themselves)
                {
                    reveal(game_board, neighbour_i, neighbour_j, flags_info);
                }
            }
        }
    }
}

void reveal(struct game_board *game_board, int row, int column, struct flags_info *flags_info) {
    struct cell *the_cell = &game_board->playing_field[row][column];
    if (the_cell->flagged) {
        remove_flag(the_cell, flags_info); // personal choice: if the player wants to reveal a flagged cell, the flag is given back to the player before the reveal
    }
    if (the_cell->bomb) {
        GAME_OVER = 1;
    } else if (the_cell->revealed) {
        printf("Cell is already revealed!\n");
    } else {
        the_cell->revealed = TRUE;
        remaining_nonbomb_cells--;
        if (remaining_nonbomb_cells == 0) {
            GAME_WON = 1;
        }
        if (the_cell->neighbours_count == 0) {
            reveal_neighbours(game_board, row, column, flags_info);
        }
    }
}