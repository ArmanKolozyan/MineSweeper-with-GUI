#ifndef encoder_decoder
#define encoder_decoder

#include "datatypes.h"
#include <stdio.h>

extern int ROWS;
extern int COLUMNS;

void encode(struct cell playing_field[ROWS][COLUMNS], int placed_flags, int correct_placed_flags);
void decode(char *filename, struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags);

#endif