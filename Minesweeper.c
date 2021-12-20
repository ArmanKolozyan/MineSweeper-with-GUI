// -------------------------------------------------------------------
// game:     MineSweeper (Command Line Version)
// author:   Arman Kolozyan
// year:     2021-2022
// course:   Structuur van Computerprogramma's II
// -------------------------------------------------------------------

#include "cell.h"
#include "datatypes.h"
#include "gui.h"
#include "handle_input.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h> // for the rand- & srand-function
#include <string.h> // for the strcpy-function
#include <time.h>   // for the time-function (needed for seeding the random-generator)

int ROWS;
int COLUMNS;
int TOTAL_BOMBS;
extern int USER_ROW;
extern int USER_COLUMN;
extern int SHOULD_CONTINUE;

/*
2 global variables that are modified by external functions (see cell.c) when the player has won/lost.
*/
enum Boolean GAME_OVER;
enum Boolean GAME_WON;

/* 
Fills the 2D array (defined in the main function) with structs and gives default values to the struct members.
This function is only called when the user wants to start a new session from scratch and doesn't want to load the game from a file.
*/
void initialize_field(struct cell playing_field[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
            current_cell->bomb = FALSE;
            current_cell->revealed = FALSE;
            current_cell->flagged = FALSE;
            current_cell->neighbours_count = 0;
        }
    }
}

/* 
Places the number of bombs specified by the user.
To prevent the player from immediately, in the first turn, stepping on a mine and losing the game, this function receives the 
row and column of the first call of reveal/flag. If the first call is a print, then it receives a random row and column.
*/
void install_bombs(struct cell playing_field[ROWS][COLUMNS]) {
    srand(time(0)); //to seed rand with a starting value
    int placed_bombs = 0;
    while (placed_bombs != TOTAL_BOMBS) {
        int bomb_row = rand() % ROWS;
        int bomb_column = rand() % COLUMNS;
        if ((bomb_row != USER_ROW) || (bomb_column != USER_COLUMN)) {
            struct cell *bomb_cell = &playing_field[bomb_row][bomb_column];
            if (!bomb_cell->bomb) { // to ensure that all bombs are placed in different cells
                bomb_cell->bomb = TRUE;
                placed_bombs++;
            }
        }
    }
}

/*
Decodes and installs the state of the game that is stored in the given file.
The state of the game is expected to be in the following format: 
first four lines represent the width, height, number of placed flags and number of correctly placed flags respectively.
Next are all the lines that represent the state of each cell, where each line consists of two characters, viz:
first character represents the number of neighbours that are bombs, or 'B' if the cell is a bomb
second character indicates whether the cell is revealed (R), flagged (F), or hidden (H).
*/
void decode(char *filename, struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags) {

    /*
    This buffer will not be cleared after every call to fgets because
    at every write to this buffer, a '\0' character is put at the end
    and every read/call to atoi stops at the '\0' character.
    It is of size 4 to provide enough space for: first & second character, newline character & the terminating null character.
    */
    char input_buffer[4];

    FILE *fp;
    fp = fopen(filename, "r");

    fscanf(fp, "%d ", &ROWS);                     // read the number of rows
    fscanf(fp, "%d ", &COLUMNS);                  // read the number of columns
    fgets(input_buffer, sizeof input_buffer, fp); // reads the number of placed flags
    *placed_flags = atoi(input_buffer);
    fgets(input_buffer, sizeof input_buffer, fp); // reads the number of correctly placed flags
    *correct_placed_flags = atoi(input_buffer);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
            fgets(input_buffer, sizeof input_buffer, fp);
            char curr = input_buffer[0];
            if (curr == 'B') {
                current_cell->neighbours_count = 0;
                current_cell->bomb = TRUE;
            } else {
                current_cell->neighbours_count = curr - '0'; // to get the actual number that represents the neighbours-count
                current_cell->bomb = FALSE;
            }
            curr = input_buffer[1];
            switch (curr) {
            case 'R':
                current_cell->revealed = TRUE;
                current_cell->flagged = FALSE;
                break;
            case 'F':
                current_cell->flagged = TRUE;
                current_cell->revealed = FALSE;
                break;
            case 'H':
                current_cell->revealed = FALSE;
                current_cell->flagged = FALSE;
            }
        }
    }
    fclose(fp);
}

/*
for an array of int of N * M the syntax would be "int (*arr)[M] = malloc(sizeof(int) * N * M);"
This is a weird syntax, but it applied for a lot of stuff in more complicated C, like function pointers
you start with the variable name, arr, and you go from right to left
(*arr) mean this is a pointer,
[M] mean this is pointer to an array of size M
then int, this mean this a pointer to an array of int of size M
so if you understand how pointers works, if you dereference arr, you obtain an array of size M.
this have a lot of advantages, everything is contigus in memory, so its faster, you don't need to have multiples pointers, you only need 1 malloc and 1 free.
and if you make a function that have a prototype like "void foo(int arr[][M]);" it will accept static 2D array but also 2D dynamic array.
this work for the number of dimension you want, N * M * 0 would be "int (*arr)[N][M] = malloc(sizeof(int) * N * M * 0));"
wich is a lot easier to read than an array of array of array, with a nested allocation.
*/
void make_field(struct cell (**aptr)[ROWS][COLUMNS]) {
    *aptr = malloc(sizeof(struct cell[ROWS][COLUMNS]));
}

void deallocate_field(struct cell playing_field[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        free(playing_field[i]);
    }
    free(playing_field);
}

/*
Encodes the game state into a file named "state.txt".
The state of the game is written in the following format: 
first four lines represent the width, height, number of placed flags and number of correctly placed flags respectively.
Next are all the lines that represent the state of each cell, where each line consists of two characters, viz:
first character represents the number of neighbours that are bombs, or 'B' if the cell is a bomb
second character indicates whether the cell is revealed (R), flagged (F), or hidden (H).
*/
void encode(struct cell playing_field[ROWS][COLUMNS], int placed_flags, int correct_placed_flags) {
    FILE *fp;
    fp = fopen("state.txt", "w");
    fprintf(fp, "%d", ROWS);
    fputc('\n', fp);
    fprintf(fp, "%d", COLUMNS);
    fputc('\n', fp);
    fprintf(fp, "%d", placed_flags);
    fputc('\n', fp);
    fprintf(fp, "%d", correct_placed_flags);
    fputc('\n', fp);
    fclose(fp);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
            if (!current_cell->bomb) {
                int neighbours_count = current_cell->neighbours_count;
                fprintf(fp, "%d", neighbours_count);
            } else {
                char status = 'B';
                fputc(status, fp);
            }
            if (current_cell->revealed) {
                char status = 'R';
                fputc(status, fp);
            } else if (current_cell->flagged) {
                char status = 'F';
                fputc(status, fp);
            } else {
                char status = 'H';
                fputc(status, fp);
            }
            fputc('\n', fp);
        }
    }
}

/*
To prevent the player from immediately, in the first turn, stepping on a mine and losing the game, 
the first command is handled separately to ensure that the first given row and column of the player does not contain a mine.
*/
void initialize(struct cell playing_field[ROWS][COLUMNS], int *placed_flags, int *correct_placed_flags) {
    initialize_field(playing_field);
    initialize_gui();
    draw_field(playing_field, FALSE);
    read_input();
    install_bombs(playing_field);
    calculate_neighbours_bombs(playing_field);
    process_input(playing_field, placed_flags, correct_placed_flags);
    call_the_drawer(playing_field);
}

/*
The program can be started in two ways. It is both possible to load a saved playfield and continue with it, and to simply start from a new field with given dimensions. 
If the state of the game is stored in a file called "state.txt", then the player can load the game using the following command: ./MineSweeper -f state.txt
On the other hand, the player can also start a new playfield with width 20 and height 10 that contains 15 mines, by executing the following command: ./MineSweeper -w 20 -h 10 -m 15.
These three arguments can be entered in any order.
*/
void get_initial_arguments(int argc, const char *argv[], char *filename, enum Boolean *file_flag) {
    int can_continue, opt;
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

int main(int argc, const char *argv[]) {
    GAME_WON = FALSE;
    GAME_OVER = FALSE; //To initialize the variables when starting a new game-session.

    const int filaneme_size = 50;
    char filename[filaneme_size];
    enum Boolean file_flag = FALSE;

    get_initial_arguments(argc, argv, filename, &file_flag);

    struct cell(*playing_field)[COLUMNS] = malloc(sizeof(struct cell[ROWS][COLUMNS]));
    int placed_flags = 0;
    int correct_placed_flags = 0; // if correct_placed_flags == TOTAL_BOMBS => the player has won

    if (file_flag) {
        decode(filename, playing_field, placed_flags, correct_placed_flags);
        initialize_gui();
        call_the_drawer(playing_field);
    } else {
        initialize(playing_field, &placed_flags, &correct_placed_flags);
    }

    while (!GAME_OVER && !GAME_WON && SHOULD_CONTINUE) {
        read_input();
        process_input(playing_field, &placed_flags, &correct_placed_flags);
        call_the_drawer(playing_field);
    }
    // after winning/losing the game, the whole revealed field is printed
    if (GAME_WON) {
        printf("\nYOU WON! GOOD JOB!\n");
        draw_field(playing_field, TRUE);
        read_input();
    }
    if (GAME_OVER) {
        printf("\nGAME OVER!\n");
        draw_field(playing_field, TRUE);
        read_input();
    }
}