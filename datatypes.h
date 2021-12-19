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

#endif