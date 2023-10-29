#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ncurses.h>

#define WIDTH 10
#define HEIGHT 20

#define I_COLOR 1
#define O_COLOR 2
#define T_COLOR 3
#define J_COLOR 4
#define L_COLOR 5
#define S_COLOR 6
#define Z_COLOR 7
#define B_COLOR 8

#define ZERO 0
#define NINETY 1
#define ONE_EIGHTY 2
#define TWO_SEVENTY 3

int I_SHAPE[4][4] = {
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {0,0,0,0}
};
int O_SHAPE[4][4] = {
    {0,1,1,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
int T_SHAPE[4][4] = {
    {0,1,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
int J_SHAPE[4][4] = {
    {1,0,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
int L_SHAPE[4][4] = {
    {0,0,1,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
int S_SHAPE[4][4] = {
    {0,1,1,0},
    {1,1,0,0},
    {0,0,0,0},
    {0,0,0,0}
};
int Z_SHAPE[4][4] = {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};

typedef struct {
    int shape[4][4];
    int color;
    int orientation;
}Tetromino;

Tetromino *make_tetromino(int shape[4][4], int color) {
    Tetromino *tetromino = (Tetromino*)malloc(sizeof(Tetromino));
    if (tetromino == NULL) {
        fprintf(stderr, "Memory Allocation Failed.\n");
        return NULL;
    }
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            tetromino->shape[i][j] = shape[i][j];
        }
    }
    tetromino->color = color;
    tetromino->orientation = ZERO;
    return tetromino;
}

int display_game(char *game_board[HEIGHT+2][WIDTH+2], int game_color[HEIGHT+2][WIDTH+2]) {
    for (int row=0; row < HEIGHT+2; row++) {
        for (int col=0; col < WIDTH+2; col++) {
            attron(COLOR_PAIR(game_color[row][col]));
            printw("%s", game_board[row][col]);
            attroff(COLOR_PAIR(game_color[row][col]));
        }
        printw("\n");
    }
    return 0;
}

volatile sig_atomic_t quit = 0;
void handle_sigint(int signum) {
    quit = 1;
}

int loop(char *game_board[HEIGHT+2][WIDTH+2], int game_color[HEIGHT+2][WIDTH+2]) {
    int running = TRUE;
    int delay_us = 1000000;
    Tetromino *s_tetromino = make_tetromino(S_SHAPE, S_COLOR);
    Tetromino *t_tetromino = make_tetromino(T_SHAPE, T_COLOR);
    int row = -1;
    while (!quit && running) {
        display_game(game_board, game_color);
        refresh();
        usleep(delay_us);
        clear();
        row++;
        if (row == 20) {
            running = FALSE;
        }
    }
    free(s_tetromino);
    free(t_tetromino);
    return 0;
}

int init_color_pairs() {
    init_pair(I_COLOR, 5, -1);
    init_pair(O_COLOR, 6, -1);
    init_pair(T_COLOR, 4, -1);
    init_pair(J_COLOR, 2, -1);
    init_pair(L_COLOR, 1, -1);
    init_pair(S_COLOR, 3, -1);
    init_pair(Z_COLOR, 20, -1);
    init_pair(B_COLOR, 0, -1);
    return 0;
}

int main() {
    char *game_board[HEIGHT+2][WIDTH+2];
    int game_color[HEIGHT+2][WIDTH+2];
    for (int row=0; row<HEIGHT+2; row++) {
        for (int col=0; col<WIDTH+2; col++) {
            game_board[row][col] = "[]";
            game_color[row][col] = 8;
        }
    }

    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support colors.\n");
        exit(1);
    }
    start_color();
    use_default_colors();
    assume_default_colors(-1, -1);
    init_color_pairs();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    signal(SIGINT, handle_sigint);
    loop(game_board, game_color);

    endwin();
    return 0; 
}
