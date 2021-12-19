#include <stdio.h>
#include <stdlib.h> // for the rand- & srand-functions

#define MAXLINE 1000
int getline2(char s[], int lim);
char get_char();

int main(int argc, const char *argv[]) {
    int getline2(char s[], int lim);
    char get_char();
    char line[MAXLINE];
    long lineno = 0;
    int opt, width = -1, height = -1, mines = -1;
    int can_continue = 1;
    while ((--argc > 0) && (**++argv == '-') && can_continue)
        while ((opt = *++*argv) != '\0') {
            switch (opt) {
            case 'w':
                argv++;
                --argc;
                width = atoi(*argv);
                break;
            case 'h':
                argv++;
                --argc;
                height = atoi(*argv);
                break;
            case 'm':
                argv++;
                --argc;
                mines = atoi(*argv);
                break;
            }
            if ((width == -2) || (height == -2) || (mines == -2)) {
                can_continue = 0;
            }
            break;
            //  case 'f':
            //      break;
            //  }
            //  }
        }
    printf("%i\n", width);
    printf("%i\n", height);
    printf("%i\n", mines);
}