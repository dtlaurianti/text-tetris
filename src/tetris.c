#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>

#define WIDTH 12
#define HEIGHT 22

#define ZERO 0
#define NINETY 1
#define ONE_EIGHTY 2
#define TWO_SEVENTY 3

#define I_COLOR 1
#define O_COLOR 2
#define T_COLOR 3
#define J_COLOR 4
#define L_COLOR 5
#define S_COLOR 6
#define Z_COLOR 7
#define B_COLOR 8
#define W_COLOR 9

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

// write the current visual game state to the window buffer
int display_game(int game_color[HEIGHT][WIDTH]) {
    for (int row=0; row < HEIGHT; row++) {
        for (int col=0; col < WIDTH; col++) {
            attron(COLOR_PAIR(game_color[row][col]));
            printw("[]");
            attroff(COLOR_PAIR(game_color[row][col]));
        }
        printw("\n");
    }
    return 0;
}

int loop(int game_color[HEIGHT][WIDTH]) {
    int running = TRUE;
    struct timespec tick_time;
    struct timespec curr_time;
    int tps = 60;
    int fall_period = 1*tps;
    int fall_counter = 0;
    float delay_s = 1.0/tps;
    Tetromino *active_tetromino = make_tetromino(S_SHAPE, S_COLOR);
    int row = -1;
    int col = 6;
    int ch;

    while (running) {
        clear_paint(game_color);
        paint_tetromino(active_tetromino, row, col, game_color);
        display_game(game_color);

        refresh();

        clock_gettime(CLOCK_MONOTONIC, &tick_time);
        clock_gettime(CLOCK_MONOTONIC, &curr_time);
        while ((curr_time.tv_sec - tick_time.tv_sec) + (curr_time.tv_nsec - tick_time.tv_nsec)/1e9 < delay_s) {
            ch = getch();
            if (ch == KEY_LEFT && col>1) {
                col--;
            } else if (ch == KEY_RIGHT && col<WIDTH-2) {
                col++;
            }
            clock_gettime(CLOCK_MONOTONIC, &curr_time);
        }

        clear();

        fall_counter++;
        if (fall_counter >= fall_period) {
            fall_counter = 0;
            row++;
        }
        if (row == 20) {
            running = FALSE;
        }
    }
    free(active_tetromino);
    return 0;
}

int main() {
    int game_color[HEIGHT][WIDTH];
    clear_paint(game_color);

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
    timeout(10);
    keypad(stdscr, TRUE);
    curs_set(0);

    loop(game_color);

    endwin();
    return 0; 
}
