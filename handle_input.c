#include "handle_input.h"
#include "cell.h"
#include "datatypes.h"
#include <stdio.h>
#include <stdlib.h> // for the atoi-function
#include <string.h> // for the strcpy-function

/*
The program can be started in two ways. It is both possible to load a saved playfield and continue with it, and to simply start from a new field with given dimensions. 
If the state of the game is stored in a file called "state.txt", then the player can load the game using the following command: ./MineSweeper -f state.txt
On the other hand, the player can also start a new playfield with width 20 and height 10 that contains 15 mines, by executing the following command: ./MineSweeper -w 20 -h 10 -m 15.
These three arguments can be entered in any order.

This function returns 1 if everything went well, otherwise it returns 0 so that the game process is stopped.
*/
int handle_initial_arguments(int argc, const char *argv[], struct game_board *game_board, int *total_bombs, char *filename, enum Boolean *file_flag) {
    enum Boolean can_continue = TRUE;
    int provided_options = 0;
    char opt;
    while ((--argc > 0) && (**++argv == '-')) {
        if ((opt = *++*argv) == '\0') {
            printf("Please provide one of the following flags after '-'; f: file, h: height, w: width, m: mines.\n");
            printf("Usage examples: ./MineSweeper -w 5 -h 7 -m 2\n                ./MineSweeper -f game_state.txt\n");
            return 0;
        }
        switch (opt) {
        case 'w':
            ++argv;
            --argc;
            ++provided_options;
            if ((argc < 1) || ((game_board->columns = atoi(*argv)) == 0)) {
                printf("Please provide a valid width.\n");
                printf("You can do this by entering the desired number after the '-w' flag.\n");
                can_continue = FALSE;
            }
            break;
        case 'h':
            argv++;
            --argc;
            ++provided_options;
            if ((argc < 1) || ((game_board->rows = atoi(*argv)) == 0)) {
                printf("Please provide a valid height.\n");
                printf("You can do this by entering the desired number after the '-h' flag.\n");
                can_continue = FALSE;
            }
            break;
        case 'm':
            argv++;
            --argc;
            ++provided_options;
            if ((argc < 1) || ((*total_bombs = atoi(*argv)) == 0)) {
                printf("Please provide a valid number of mines.\n");
                printf("You can do this by entering the desired number after the '-m' flag.\n");
                can_continue = FALSE;
            }
            if (*total_bombs >= (game_board->rows * game_board->columns)) {
                printf("Too many mines, please provide less mines.\n");
                can_continue = FALSE;
            }
            break;
        case 'f':
            argv++;
            --argc;
            FILE *fp;
            ++provided_options;
            if ((fp = fopen(*argv, "r")) != NULL) {
                // we have to read the rows and columns here
                // so that the dynamic allocation can be done after this function
                fscanf(fp, "%d ", &game_board->rows);    // read the number of rows
                fscanf(fp, "%d ", &game_board->columns); // read the number of columns
                fclose(fp);
                strcpy(filename, *argv);
                *file_flag = TRUE;
            } else {
                printf("Please provide a valid file name.\n");
                printf("You can do this by entering the correct file name after the '-f' flag.\n");
                return 0;
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

void process_input(struct game_board *game_board, int total_bombs, struct flags_info *flags_info, struct user_input *user_input) {
    if (user_input->command == REVEAL) {
        reveal(game_board, user_input->row, user_input->column, flags_info);
    } else if (user_input->command == FLAG) {
        place_flag(&game_board->playing_field[user_input->row][user_input->column], flags_info, total_bombs);
    } else if (user_input->command == PRINT) { // In the case of PRINT, we don't have to do anything here, but this piece is left here to emphasize that
    }
}
