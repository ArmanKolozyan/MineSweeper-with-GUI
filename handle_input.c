#include "handle_input.h"
#include "cell.h"
#include "datatypes.h"
#include <stdio.h>
#include <stdlib.h> // for the atoi-function
#include <string.h> // for the strcpy-function

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

This function returns 1 if everything went well, otherwise it returns 0 so that the game process is stopped.
*/
int get_initial_arguments(int argc, const char *argv[], char *filename, enum Boolean *file_flag) {
    enum Boolean can_continue = TRUE;
    int provided_options = 0; 
    char opt;
    while ((--argc > 0) && (**++argv == '-')) {
        if ((opt = *++*argv) == '\0') {
            printf("Please provide one of the following flags; f: file, h: height, w: width, m: mines.\n");
            printf("Usage examples: ./MineSweeper -w 5 -h 7 -m 2\n                ./MineSweeper -f game_state.txt\n");
            return 0;
        }
        switch (opt) {
        case 'w':
            ++argv;
            --argc;
            ++provided_options;
            if ((argc < 1) || ((COLUMNS = atoi(*argv)) == 0)) {
                printf("Please provide a valid width.\n");
                printf("You can do this by entering the desired number after the '-w' flag.\n");
                can_continue = FALSE;
            }
            break;
        case 'h':
            argv++;
            --argc;
            ++provided_options;
            if ((argc < 1) || ((ROWS = atoi(*argv)) == 0)) {
                printf("Please provide a valid height.\n");
                printf("You can do this by entering the desired number after the '-h' flag.\n");
                can_continue = FALSE;
            }
            break;
        case 'm':
            argv++;
            --argc;
            ++provided_options;
            if ((argc < 1) || ((TOTAL_BOMBS = atoi(*argv)) == 0)) {
                printf("Please provide a valid number of mines.\n");
                printf("You can do this by entering the desired number after the '-m' flag.\n");
                can_continue = FALSE;
            }
            break;
        case 'f':
            argv++;
            --argc;
            FILE *fp;
            ++provided_options;
            if ((fp = fopen(*argv, "r")) != NULL) {
                char rows_input[5];
                char columns_input[5];
                fgets(rows_input, sizeof rows_input, fp);
                fgets(columns_input, sizeof columns_input, fp);
                fclose(fp);
                ROWS = atoi(rows_input);
                COLUMNS = atoi(columns_input);
                strcpy(filename, *argv);
                *file_flag = TRUE;
            } else {
                fclose(fp);
                can_continue = FALSE;
                printf("Please provide a valid file name.\n");
                printf("You can do this by entering the correct file name after the '-f' flag.\n");
            }
            break;
        default:
            printf("Illegal option %c\n", opt);
            can_continue = FALSE;
            break;
        }
    }
    if (!*file_flag && (provided_options < 3)) {
        if (!can_continue) {
            printf("Also: ");
        }
        printf("Please provide enough arguments.\n");
        printf("Usage examples: ./MineSweeper -w 5 -h 7 -m 2\n                ./MineSweeper -f game_state.txt\n");
        return 0;
    } else if (!can_continue) {
        return 0;
    } else {
        return 1;
    }
}

void process_input(struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags) {
    if (USER_COMMAND == REVEAL) {
        reveal(playing_field, USER_ROW, USER_COLUMN, placed_flags, correct_placed_flags);
    } else if (USER_COMMAND == FLAG) {
        place_flag(&playing_field[USER_ROW][USER_COLUMN], placed_flags, correct_placed_flags);
    } else if (USER_COMMAND == PRINT) { // In the case of PRINT, we don't have to do anything here, but this piece is left here to emphasize that,
    }                                   //  because when calling "call_the_printer" in Minesweeper.c, it checks whether the entire revealed field should be printed.
}

enum Boolean player_wants_to_replay() {
    printf("Press ENTER if you want to replay!\n");
    getchar() == '\n';
}
