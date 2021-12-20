#ifndef handle_input
#define handle_input

#include "datatypes.h"
#include "gui.h"
#include <stdio.h>

void get_initial_arguments(int argc, const char *argv[], char *filename, enum Boolean *file_flag);
void process_input(struct cell (*playing_field)[], int *placed_flags, int *correct_placed_flags);
enum Boolean player_wants_to_replay();

#endif