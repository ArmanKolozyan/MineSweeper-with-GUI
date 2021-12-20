#include "handle_input.h"
#include "cell.h"
#include "macros.h"
#include <stdio.h>

extern enum Command USER_COMMAND;
extern int USER_ROW;
extern int USER_COLUMN; // later miss beter in een struct
extern const int ROWS;
extern const int COLUMNS;

/*
When the user types its command and presses Enter, it will sometimes leave some input (like '\n' voor example) in the input buffer. 
In such cases, it is necessary to clear the input buffer for the next input. 
*/
void clear_input() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF) { //getchar() returns EOF in the event of a read error, so the loop should test for EOF also
        ;
    }
}

/*
Checks if the given column/row is not outisde the field.
*/
enum Boolean check_boundaries(int USER_ROW, int USER_COLUMN) {
    return ((USER_ROW >= 0) && (USER_ROW < ROWS) && (USER_COLUMN >= 0) && (USER_COLUMN < COLUMNS));
}

/*
"get_arguments" returns TRUE if everything went right (i.e. two numbers between the boundaries were provided), otherwise it returns FALSE.
*/
enum Boolean get_arguments(int *USER_ROW, int *USER_COLUMN) {
    const int expecting_arguments = 2; // if the players provides more than 2 arguments, the first 2 arguments are taken
    if (scanf("%i %i", USER_ROW, USER_COLUMN) != expecting_arguments) {
        printf("Please provide numbers as arguments.\n"); // because scanf() returns the number of fields that were successfully converted and assigned
        clear_input();
        sleep(2);
        return FALSE;
    };
    if (check_boundaries(*USER_ROW, *USER_COLUMN)) {
        clear_input();
        return TRUE;
    } else {
        printf("Input is out of bounds! Try again.\n");
        clear_input();
        sleep(2);
        return FALSE;
    }
}

/*
"get_input" makes sure that "command", "USER_ROW" and "USER_COLUMN" contain (correct) values given by the player.
// */
// void get_input(struct cell playing_field[ROWS][COLUMNS]) {
//     printf("Write your command: \n");
//     char after_command;
//     *command = getchar();
//     after_command = getchar(); // is necessary to recognize erroneous input
// 
//     if (*command == PRINT && after_command == '\n') {
//         ;
//     } else if ((*command == REVEAL || *command == FLAG) && after_command == ' ') {
//         if (!get_arguments(USER_ROW, USER_COLUMN)) {
//             get_input(playing_field, command, USER_ROW, USER_COLUMN);
//         }
//     } else if ((*command == REVEAL || *command == FLAG) && after_command == '\n') {
//         printf("Please provide arguments after the command!\n");
//         sleep(2);
//         get_input(playing_field, command, USER_ROW, USER_COLUMN);
//     } else {
//         printf("Wrong command! Try again.\n");
//         sleep(2);
//         clear_input();
//         get_input(playing_field, command, USER_ROW, USER_COLUMN);
//     }
// }
// 
/*
"process_input" makes sure that, depending on the command, the correct action is performed.
*/
void process_input(struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags) {
    if (USER_COMMAND == REVEAL) {
        reveal(playing_field, USER_ROW, USER_COLUMN, placed_flags, correct_placed_flags);
    } else if (USER_COMMAND == FLAG) {
       place_flag(&playing_field[USER_ROW][USER_COLUMN], placed_flags, correct_placed_flags);
    } else if (USER_COMMAND == PRINT) { // In the case of PRINT, we don't have to do anything here, but this piece is left here to emphasize that,
    }                               //  because when calling "call_the_printer" in Minesweeper.c, it checks whether the entire revealed field should be printed.
}

// enum Boolean handle_replay() {
//     printf("Press ENTER if you want to replay!\n");
//     getchar() == '\n';
// }



