#ifndef field_cell
#define field_cell

#include "datatypes.h"

void calculate_neighbours_bombs(struct cell (*playing_field)[]);
void place_flag(struct cell *the_cell, int *placed_flags, int *correct_placed_flags);
void reveal(struct cell (*playing_field)[], int row, int column, int *placed_flags, int *correct_placed_flags);

#endif