#ifndef GUI_H_
#define GUI_H_


#include "datatypes.h"

void call_the_drawer(int rows, int columns, struct cell **playing_field);
void draw_field(int rows, int columns, struct cell **playing_field, enum Boolean reveal_all);
void initialize_gui(int rows, int columns);
void read_input();
void free_gui();
extern struct user_input USER_INPUT;


#endif
