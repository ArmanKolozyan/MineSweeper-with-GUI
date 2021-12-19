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
#include "printing.h"
#include <stdio.h>
#include <stdlib.h> // for the rand- & srand-functions
#include <time.h>   // for the time-function
#include <unistd.h> //for the sleep-function

extern int user_row;
extern int user_column;
extern int should_continue;
int ROWS = 10;
int COLUMNS = 10;
int TOTAL_BOMBS = 10;
char filename[50];

/*
2 global variables that are modified by external functions (see cell.c) when the player has won/lost.
*/
enum Boolean GAME_OVER;
enum Boolean GAME_WON;

/* 
Fills the 2D array (defined in the main function) with structs and gives default values to the struct members.
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
Places the number of bombs specified in "macros.h".
To prevent the player from immediately, in the first turn, stepping on a mine and losing the game, this function receives the 
row and column of the first call of reveal/flag. If the first call is a print, then it receives a random row and column.
*/
void install_bombs(struct cell playing_field[ROWS][COLUMNS]) {
    srand(time(0)); //to seed rand with a starting value
    int placed_bombs = 0;
    while (placed_bombs != TOTAL_BOMBS) {
        int bomb_row = rand() % ROWS;
        int bomb_column = rand() % COLUMNS;
        if ((bomb_row != user_row) || (bomb_column != user_column)) {
            struct cell *bomb_cell = &playing_field[bomb_row][bomb_column];
            if (!bomb_cell->bomb) { // to ensure that all bombs are placed in different cells
                bomb_cell->bomb = TRUE;
                placed_bombs++;
            }
        }
    }
}

void decode(FILE *fp, struct cell playing_field[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            struct cell *current_cell = &playing_field[i][j];
            char buf[4];
            fgets(buf, sizeof buf, fp);
            char curr = buf[0];
            if (curr == 'B') {
                current_cell->neighbours_count = 0;
                current_cell->bomb = TRUE;
            } else {
                current_cell->neighbours_count = curr - '0';
                current_cell->bomb = FALSE;
            }
            curr = buf[1];
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
}

struct cell *make_cell(void) {
    struct cell *cell = malloc(sizeof(struct cell));
    cell->bomb = FALSE;
    cell->revealed = FALSE;
    cell->flagged = FALSE;
    cell->neighbours_count = 0;
    return cell;
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

void encode(struct cell playing_field[ROWS][COLUMNS]) {
    FILE *fp;
    fp = fopen("state.txt", "w");
    fprintf(fp, "%d", 10);
    fputc('\n', fp);
    fprintf(fp, "%d", 10);
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
    draw_field(playing_field, FALSE);
    read_input();
    install_bombs(playing_field);
    calculate_neighbours_bombs(playing_field);
    process_input(playing_field, placed_flags, correct_placed_flags);
    call_the_drawer(playing_field);
}

void get_initial_arguments(int argc, const char *argv[]) {
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
                break;
            default:
                printf("illegal option %c\n", opt);
                argc = 0;
                break;
            }
            if ((ROWS == -2) || (COLUMNS == -2) || (TOTAL_BOMBS == -2)) {
                // hier error message printen
                can_continue = 0;
            }
            break;
        }
}

//  printf("  Enter the number of rows: ");
//   scanf("%u", &ROWS);
//   printf("Enter the number of colums: ");
//   scanf("%u", &COLUMNS);
//   while (ROWS < 1 || COLUMNS < 1) {
//       printf("Number of rows or columns cannot be below 1.\nPlease try again...\n");
//      printf("  Enter the number of rows: ");
//      scanf("%u", &ROWS);
//      printf("Enter the number of colums: ");
//     scanf("%u", &COLUMNS);

int main(int argc, const char *argv[]) {
#define MAXLINE 1000
    int getline2(char s[], int lim);
    char get_char();
    char line[MAXLINE];
    long lineno = 0;
    GAME_WON = FALSE;
    GAME_OVER = FALSE;
    get_initial_arguments(argc, argv);
    struct cell(*playing_field)[ROWS] = malloc(sizeof(struct cell[ROWS][COLUMNS]));
    int placed_flags = 0;         // dit ook bijhouden waarschijnlijk!!!!!!
    int correct_placed_flags = 0; // if correct_placed_flags == TOTAL_BOMBS => the player has won
   /* FILE *fp;
    fp = fopen(filename, "r");
    char rows_input[5];
    char columns_input[5];
    fgets(rows_input, sizeof rows_input, fp);
    fgets(columns_input, sizeof columns_input, fp);
    ROWS = atoi(rows_input);
    COLUMNS = atoi(columns_input);
    decode(fp, playing_field);
    fclose(fp);
    */
    initialize_gui();
  //  call_the_drawer(playing_field);
    initialize(playing_field, &placed_flags, &correct_placed_flags);
    while (!GAME_OVER && !GAME_WON && should_continue) {
        read_input();
        process_input(playing_field, &placed_flags, &correct_placed_flags);
        if (!GAME_OVER && !GAME_WON) { // to ensure that in case of a win or loss (determined by the above "process_input"), the field is not printed twice
            call_the_drawer(playing_field);
        }
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