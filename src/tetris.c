#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ncurses.h>

#define WIDTH 12
#define HEIGHT 22

#define I_COLOR 1
#define O_COLOR 2
#define T_COLOR 3
#define J_COLOR 4
#define L_COLOR 5
#define S_COLOR 6
#define Z_COLOR 7
#define B_COLOR 8
#define W_COLOR 9

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

// factory for a dynamically allocated tetromino
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

// colors the game_color according to the tetromino color and the squares in occupies
int paint_tetromino(Tetromino *tetromino, int row, int col, int game_color[HEIGHT][WIDTH]) {
    for (int rr=0; rr < 4; rr++) {
        for (int cc=0; cc < 4; cc++) {
            if (
                    tetromino->shape[rr][cc] == 1
                    && row+rr > 0
                    && row+rr < HEIGHT
                    && col+cc > 0
                    && col+cc < WIDTH
               ) {
                game_color[row+rr][col+cc] = tetromino->color;
            }
        }
    }
    return 0;
}

// resets all game_color to background
int clear_paint(int game_color[HEIGHT][WIDTH]) {
    for (int row=0; row<HEIGHT; row++) {
        for (int col=0; col<WIDTH; col++) {
            if (
                    row == 0
                    || row == HEIGHT-1
                    || col == 0
                    || col == WIDTH-1
               ) {
                game_color[row][col] = W_COLOR;
            } else {
                game_color[row][col] = B_COLOR;
            }
        }
    }
    return 0;
}

// individually write a single square of the game to the window buffer
int display_square(int row, int col, char *game_board[HEIGHT][WIDTH], int game_color[HEIGHT][WIDTH]) {
    attron(COLOR_PAIR(game_color[row][col]));
    printw("%s", game_board[row][col]);
    attroff(COLOR_PAIR(game_color[row][col]));
    return 0;
}

// write the current visual game state to the window buffer
int display_game(char *game_board[HEIGHT][WIDTH], int game_color[HEIGHT][WIDTH]) {
    for (int row=0; row < HEIGHT; row++) {
        for (int col=0; col < WIDTH; col++) {
            display_square(row, col, game_board, game_color);
        }
        printw("\n");
    }
    return 0;
}

volatile sig_atomic_t quit = 0;
void handle_sigint(int signum) {
    quit = 1;
}

int loop(char *game_board[HEIGHT][WIDTH], int game_color[HEIGHT][WIDTH]) {
    int running = TRUE;
    int delay_us = 1000000;
    Tetromino *active_tetromino = make_tetromino(S_SHAPE, S_COLOR);
    int row = -1;
    while (!quit && running) {
        clear_paint(game_color);
        paint_tetromino(active_tetromino, row, 5, game_color);
        display_game(game_board, game_color);
        refresh();
        usleep(delay_us);
        clear();
        row++;
        if (row == 20) {
            running = FALSE;
        }
    }
    free(active_tetromino);
    return 0;
}

int init_color_pairs() {
    init_pair(I_COLOR, 5, 0);
    init_pair(O_COLOR, 6, 0);
    init_pair(T_COLOR, 4, 0);
    init_pair(J_COLOR, 2, 0);
    init_pair(L_COLOR, 1, 0);
    init_pair(S_COLOR, 3, 0);
    init_pair(Z_COLOR, 20, 0);
    init_pair(B_COLOR, 8, -1);
    init_pair(W_COLOR, 7, 0);
    return 0;
}

int main() {
    char *game_board[HEIGHT][WIDTH];
    for (int row=0; row<HEIGHT; row++) {
        for (int col=0; col<WIDTH; col++) {
            game_board[row][col] = "[]";
        }
    }
    int game_color[HEIGHT][WIDTH];

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
