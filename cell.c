#include "cell.h"
#include <stdio.h>

static int remaining_nonbomb_cells;
extern enum Boolean GAME_WON;
extern enum Boolean GAME_OVER;

void calculate_neighbours_bombs(int rows, int columns, struct cell playing_field[rows][columns]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            struct cell *current_cell = &playing_field[i][j];
            if (current_cell->bomb) { // we don't have to count the neighbours of bombs
                continue;
            }
            {
                int neighbour_bombs = 0;
                for (int off_i = -1; off_i <= 1; off_i++) {
                    for (int off_j = -1; off_j <= 1; off_j++) { // checking all the neighbours of current_cell
                        int neighbour_i = i + off_i;
                        int neighbour_j = j + off_j;
                        if ((neighbour_i < 0) || (neighbour_i >= rows)) { // checking if we are not off the field
                            continue;
                        }
                        if ((neighbour_j < 0) || (neighbour_j >= columns)) { // checking if we are not off the field
                            continue;
                        }
                        {
                            struct cell *neighbour = &playing_field[neighbour_i][neighbour_j];
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


void remove_flag(struct cell *the_cell, int *placed_flags, int *correct_placed_flags) {
    the_cell->flagged = FALSE;
    (*placed_flags)--;
    if (the_cell->bomb) {
        (*correct_placed_flags)--;
    }
}

void place_flag(struct cell *the_cell, int *placed_flags, int *correct_placed_flags, int total_bombs) {
    if (!the_cell->flagged) {
        if (the_cell->revealed) {
            printf("Action cannot be done. Cell is revealed.\n");
        } else if (*placed_flags < total_bombs) {
            the_cell->flagged = TRUE;
            (*placed_flags)++; // brackets are necessary since "++"" has higher precedence than "*"
            printf("Remaining flags: %i\n", total_bombs - *placed_flags);
            if (the_cell->bomb) {
                (*correct_placed_flags)++;
                if (*correct_placed_flags == total_bombs) {
                    GAME_WON = 1;
                }
            }
        } else {
            printf("No flags left!\n");
        }
    } else {
        remove_flag(the_cell, placed_flags, correct_placed_flags); // if the cell was already flagged, the player gets his flag back
    }
}


/*
If a cell is revealed that does not contain a mine in any of its neighbouring cells, all those neighbouring cells are revealed too. This process also repeats for each of these neighbours 
whose neighbours_count equals zero.
*/
void reveal_neighbours(int rows, int columns, struct cell playing_field[rows][columns], int row, int column, int *placed_flags, int *correct_placed_flags) {
    for (int off_i = -1; off_i <= 1; off_i++) {
        for (int off_j = -1; off_j <= 1; off_j++) { // checking all the neighbours
            int neighbour_i = row + off_i;
            int neighbour_j = column + off_j;
            if ((neighbour_i < 0) || (neighbour_i >= rows)) { // checking if we are not off the field
                continue;
            }
            if ((neighbour_j < 0) || (neighbour_j >= columns)) { // checking if we are not off the field
                continue;
            }
            {
                struct cell *neighbour = &playing_field[neighbour_i][neighbour_j];
                if (!neighbour->revealed) // otherwise infinite loop (constantly asking each other to reveal themselves)
                {
                    reveal(rows, columns, playing_field, neighbour_i, neighbour_j, placed_flags, correct_placed_flags);
                }
            }
        }
    }
}

void reveal(int rows, int columns, struct cell playing_field[rows][columns], int row, int column, int *placed_flags, int *correct_placed_flags) {
    struct cell *the_cell = &playing_field[row][column];
    if (the_cell->flagged) {
        remove_flag(the_cell, placed_flags, correct_placed_flags); // personal choice: if the player wants to reveal a flagged cell, the flag is given back to the player before the reveal
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
            reveal_neighbours(rows, columns, playing_field, row, column, placed_flags, correct_placed_flags);
        }
    }
}