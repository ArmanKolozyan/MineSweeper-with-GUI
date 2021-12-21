/*
The state of the game is saved in a file named "state.txt" at every iteration.
The game state can also be installed given a filename in which it is stored.
The state of the game is written and expected to be read in the following format: 
first four lines represent the width, height, number of placed flags and number of correctly placed flags respectively.
Next are all the lines that represent the state of each cell, where each line consists of two characters, viz:
first character represents the number of neighbours that are bombs, or 'B' if the cell is a bomb
second character indicates whether the cell is revealed (R), flagged (F), or hidden (H).
*/

#include "encoder_decoder.h"
#include <stdio.h>
#include <stdlib.h> // for the atoi-function

#define ENCODE_FILENAME "gamestate.txt"

/*
Decodes the state of the game given the filename.
*/
void decode(char *filename, int *total_bombs, int *rows, int *columns, struct cell playing_field[rows][columns], int *placed_flags, int *correct_placed_flags) {

    /*
    This buffer will not be cleared after every call to fgets because
    at every write to this buffer, a '\0' character is put at the end
    and every read/call to atoi stops at the '\0' character.
    It is of size 4 to provide enough space for: first & second character, newline character & the terminating null character.
    */
    char input_buffer[4];

    FILE *fp;
    fp = fopen(filename, "r");

    fscanf(fp, "%d ", rows);                     // read the number of rows
    fscanf(fp, "%d ", columns);                  // read the number of columns
    fgets(input_buffer, sizeof input_buffer, fp); // reads the number of placed flags
    *placed_flags = atoi(input_buffer);
    fgets(input_buffer, sizeof input_buffer, fp); // reads the number of correctly placed flags
    *correct_placed_flags = atoi(input_buffer);

    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *columns; j++) {
            struct cell *current_cell = &playing_field[i][j];
            fgets(input_buffer, sizeof input_buffer, fp);
            char curr = input_buffer[0];
            if (curr == 'B') {
                (*total_bombs)++;
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
Encodes the game state into a file named "gamestate.txt".
*/
void encode(int rows, int columns, struct cell playing_field[rows][columns], int placed_flags, int correct_placed_flags) {
    FILE *fp;
    fp = fopen(ENCODE_FILENAME, "w");
    fprintf(fp, "%d", rows);
    fputc('\n', fp);
    fprintf(fp, "%d", columns);
    fputc('\n', fp);
    fprintf(fp, "%d", placed_flags);
    fputc('\n', fp);
    fprintf(fp, "%d", correct_placed_flags);
    fputc('\n', fp);
    fclose(fp);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
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