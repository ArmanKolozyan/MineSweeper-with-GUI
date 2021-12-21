#ifndef encoder_decoder
#define encoder_decoder

#include "datatypes.h"

void encode(int rows, int columns, struct cell playing_field[rows][columns], int placed_flags, int correct_placed_flags);
void decode(int *rows, int *columns, struct cell playing_field[*rows][*columns], int *total_bombs, char *filename, int *placed_flags, int *correct_placed_flags);

#endif