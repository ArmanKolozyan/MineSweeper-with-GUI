#ifndef handle_input
#define handle_input

#include "datatypes.h"
#include "gui.h"

int handle_initial_arguments(int argc, const char *argv[], struct game_board *game_board, int *total_bombs, char *filename, enum Boolean *file_flag);
void process_input(struct game_board *game_board, int total_bombs, struct flags_info *flags_info);
#endif