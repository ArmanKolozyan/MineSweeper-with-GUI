#ifndef handle_input
#define handle_input

#include "datatypes.h"

int get_initial_arguments(int argc, const char *argv[], char *filename, enum Boolean *file_flag);
void process_input(struct cell (*playing_field)[], int *placed_flags, int *correct_placed_flags);
void handle_game_end();

#endif