#ifndef GUI_H_
#define GUI_H_


#include "datatypes.h"

void call_the_drawer(struct game_board *game_board);
void draw_field(struct game_board *game_board, enum Boolean reveal_all);
void initialize_gui(int rows, int columns);
void read_input();
void free_gui();
extern struct user_input USER_INPUT;


#endif
