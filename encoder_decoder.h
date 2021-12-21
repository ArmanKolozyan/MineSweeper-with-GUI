#ifndef encoder_decoder
#define encoder_decoder

#include "datatypes.h"

void encode(struct game_board *game_board, struct flags_info *flags_info);
void decode(struct game_board *game_board, int *total_bombs, char *filename, struct flags_info *flags_info);

#endif