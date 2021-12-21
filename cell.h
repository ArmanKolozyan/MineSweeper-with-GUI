#ifndef field_cell
#define field_cell

#include "datatypes.h"

void calculate_neighbours_bombs(int rows, int columns, struct cell playing_field[rows][columns]);
void place_flag(struct cell *the_cell, int *placed_flags, int *correct_placed_flags, int total_bombs);
void reveal(int rows, int columns, struct cell playing_field[rows][columns], int row, int column, int *placed_flags, int *correct_placed_flags);

#endif