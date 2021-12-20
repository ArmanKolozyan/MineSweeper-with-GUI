#include "handle_input.h"
#include <stdio.h>
#include <stdlib.h> // for the atoi-function
#include <string.h> // for the strcpy-function
#include "datatypes.h"
#include "cell.h" 

extern enum Command USER_COMMAND;
extern int USER_ROW;
extern int USER_COLUMN; // later miss beter in een struct
extern int TOTAL_BOMBS;
extern int ROWS;
extern int COLUMNS;


/*
The program can be started in two ways. It is both possible to load a saved playfield and continue with it, and to simply start from a new field with given dimensions. 
If the state of the game is stored in a file called "state.txt", then the player can load the game using the following command: ./MineSweeper -f state.txt
On the other hand, the player can also start a new playfield with width 20 and height 10 that contains 15 mines, by executing the following command: ./MineSweeper -w 20 -h 10 -m 15.
These three arguments can be entered in any order.
*/
void get_initial_arguments(int argc, const char *argv[], char *filename, enum Boolean *file_flag) {
    int can_continue = 1, opt;
    while ((--argc > 0) && (**++argv == '-') && can_continue)
        while ((opt = *++*argv) != '\0') {
            switch (opt) {
            case 'w':
                argv++;
                --argc;
                ROWS = atoi(*argv);
                break;
            case 'h':
                argv++;
                --argc;
                COLUMNS = atoi(*argv);
                break;
            case 'm':
                argv++;
                --argc;
                TOTAL_BOMBS = atoi(*argv);
                break;
            case 'f':
                argv++;
                --argc;
                FILE *fp;
                fp = fopen(*argv, "r");
                char rows_input[5];
                char columns_input[5];
                fgets(rows_input, sizeof rows_input, fp);
                fgets(columns_input, sizeof columns_input, fp);
                fclose(fp);
                ROWS = atoi(rows_input);
                COLUMNS = atoi(columns_input);
                strcpy(filename, *argv);
                can_continue = 0;
                *file_flag = TRUE;
                break;
            default:
                printf("illegal option %c\n", opt);
                can_continue = 0;
                break;
            }
            if ((ROWS == -2) || (COLUMNS == -2) || (TOTAL_BOMBS == -2)) {
                printf("Usage: find -x -n pattern\n");
                can_continue = 0;
            }
            break;
        }
}

void process_input(struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags) {
    if (USER_COMMAND == REVEAL) {
        reveal(playing_field, USER_ROW, USER_COLUMN, placed_flags, correct_placed_flags);
    } else if (USER_COMMAND == FLAG) {
       place_flag(&playing_field[USER_ROW][USER_COLUMN], placed_flags, correct_placed_flags);
    } else if (USER_COMMAND == PRINT) { // In the case of PRINT, we don't have to do anything here, but this piece is left here to emphasize that,
    }                               //  because when calling "call_the_printer" in Minesweeper.c, it checks whether the entire revealed field should be printed.
}


 enum Boolean player_wants_to_replay() {
     printf("Press ENTER if you want to replay!\n");
     getchar() == '\n';
 }



