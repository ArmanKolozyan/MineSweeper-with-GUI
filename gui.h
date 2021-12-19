#ifndef GUI_H_
#define GUI_H_

#include <stdio.h>
#include <stdlib.h>

/*
 * Importeer de benodigde functies uit SDL2.
 */
#include <SDL2/SDL.h>

/*
 * De hoogte en breedte van het venster (in pixels).
 * Deze dimensies zijn arbitrair gekozen. Deze dimensies hangen mogelijk af van de grootte van het speelveld.
 */
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500

#include "datatypes.h"
#include "macros.h"

void call_the_drawer(struct cell (*playing_field)[]);
void draw_field(struct cell (*playing_field)[], enum Boolean reveal_all);
void initialize_gui();
void read_input();

/*
 * De hoogte en breedte (in pixels) van de afbeeldingen voor de vakjes in het speelveld die getoond worden.
 * Als je andere afbeelding wil gebruiken in je GUI, zorg er dan voor dat deze
 * dimensies ook aangepast worden.
 */
#define IMAGE_HEIGHT 50
#define IMAGE_WIDTH 50

#endif /* GUI_H_ */
