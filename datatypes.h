#ifndef datatypes
#define datatypes

enum Boolean {
    FALSE = 0, // enums always start at 0, but I gave FALSE the value 0 to emphasize that
    TRUE
};

enum Command {
    REVEAL,
    PRINT,
    FLAG,
    NOTHING
};

struct cell {
    enum Boolean bomb;
    enum Boolean revealed;
    enum Boolean flagged;
    int neighbours_count;
};

/*
should_continue indicates whether the gui-application should continue. 
This is true as long as the user does not want to close the application by clicking the x button on the screen.
*/
struct user_input {
    enum Command command;
    int row;
    int column;
    enum Boolean should_continue;
};

struct game_board {
    struct cell **playing_field;
    int rows;
    int columns;
};

struct flags_info {
    int placed_flags;
    int correct_placed_flags; // if correct_placed_flags == total_bombs => the player has won
};


#endif