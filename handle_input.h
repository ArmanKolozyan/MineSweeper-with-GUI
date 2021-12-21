#ifndef handle_input
#define handle_input

#include "datatypes.h"
#include "gui.h"

int handle_initial_arguments(int argc, const char *argv[], int *rows, int *columns, int *total_bombs, char *filename, enum Boolean *file_flag);
void process_input(int rows, int columns, struct cell **playing_field, int total_bombs, int *placed_flags, int *correct_placed_flags);

#endif