#ifndef field_cell
#define field_cell

#include "datatypes.h"

void calculate_neighbours_bombs(struct game_board *game_board);
void place_flag(struct cell *the_cell, struct flags_info *flags_info, int total_bombs);
void reveal(struct game_board *game_board, int row, int column, struct flags_info *flags_info, int total_bombs);
enum Boolean has_player_won();
enum Boolean has_player_lost();

#endif