#include "printing.h"
#include "macros.h"
#include <stdio.h>

extern int ROWS;
extern int COLUMNS;
extern int TOTAL_BOMBS;

void print_column_numbers() {
    printf("      ");
    for (int i = 0; i < COLUMNS; i++) {
        printf("%i   ", i);
        if (i < 10) { // makes code more general: columns consisting of 2 digits are also printed nicely
            printf(" ");
        }
    }
    printf("\n");
}

/*
If the entire revealed field is to be printed(at the end of the game, or after the PRINT command), 
then this function is given a boolean TRUE as its second argument, in all other cases it is given the boolean FALSE.
*/
void print_field(struct cell playing_field[ROWS][COLUMNS], enum Boolean reveal_all, int placed_flags) {
    printf("Remaining flags: %i\n", TOTAL_BOMBS - placed_flags);
    print_column_numbers();
    for (int i = 0; i < ROWS; i++) {
        if (i < 10) {
            printf(" "); // makes code more general: rows consisting of 2 digits are also printed nicely
        }
        printf("  %i|", i);
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
            if (current_cell->revealed || current_cell->flagged || reveal_all) {
                if (current_cell->flagged && !reveal_all) { // sequence of conidition checks plays a crucial role, because otherwise a flagged cell would contain the neighbours_count on the screen
                    printf(" F ||");
                } else if (!current_cell->bomb) {
                    printf(" %i ||", current_cell->neighbours_count);
                } else {
                    printf(" B ||");
                }
            } else {
                printf("   ||");
            };
        }
        printf("\n");
    }
}