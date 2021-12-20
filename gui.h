#ifndef GUI_H_
#define GUI_H_


#include "datatypes.h"

void call_the_drawer(struct cell (*playing_field)[]);
void draw_field(struct cell (*playing_field)[], enum Boolean reveal_all);
void initialize_gui();
void read_input();


#endif
