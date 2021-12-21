#ifndef encoder_decoder
#define encoder_decoder

#include "datatypes.h"

void encode(int rows, int columns, struct cell playing_field[rows][columns], int placed_flags, int correct_placed_flags);
void decode(char *filename, int *total_bombs, int *rows, int *columns, struct cell playing_field[rows][columns], int *placed_flags, int *correct_placed_flags);

#endif