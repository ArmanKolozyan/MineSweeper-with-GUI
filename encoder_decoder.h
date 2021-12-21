#ifndef encoder_decoder
#define encoder_decoder

#include "datatypes.h"

void encode(int rows, int columns, struct cell **playing_field, int placed_flags, int correct_placed_flags);
void encode(struct game_board *game_board, struct flags_info *flags_info);

#endif