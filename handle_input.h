#ifndef handle_input
#define handle_input

#include "datatypes.h"
#include "gui.h"

//void get_input(struct cell (*playing_field)[], enum Command *command, int user_row, int *user_column);
void process_input(struct cell (*playing_field)[], int *placed_flags, int *correct_placed_flags);
enum Boolean handle_replay();

#endif