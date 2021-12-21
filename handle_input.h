#ifndef handle_input
#define handle_input

#include "datatypes.h"

int handle_initial_arguments(int *rows, int *columns, int *total_bombs, int argc, const char *argv[], char *filename, enum Boolean *file_flag);
void process_input(int rows, int columns, struct cell playing_field[rows][columns], int *placed_flags, int *correct_placed_flags);
void handle_game_end();

#endif