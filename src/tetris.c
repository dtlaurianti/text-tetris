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

#define I_SQUARE 1
#define O_SQUARE 2
#define T_SQUARE 3
#define J_SQUARE 4
#define L_SQUARE 5
#define S_SQUARE 6
#define Z_SQUARE 7
#define B_SQUARE 8
#define W_SQUARE 9

int init_color_pairs() {
    init_pair(I_SQUARE, 5, 0);
    init_pair(O_SQUARE, 6, 0);
    init_pair(T_SQUARE, 4, 0);
    init_pair(J_SQUARE, 2, 0);
    init_pair(L_SQUARE, 1, 0);
    init_pair(S_SQUARE, 3, 0);
    init_pair(Z_SQUARE, 20, 0);
    init_pair(B_SQUARE, 8, -1);
    init_pair(W_SQUARE, 7, 0);
    return 0;
}

const int I_SHAPE[4][4] = {
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {0,0,0,0}
};
const int O_SHAPE[4][4] = {
    {0,1,1,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
const int T_SHAPE[4][4] = {
    {0,1,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
const int J_SHAPE[4][4] = {
    {1,0,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
const int L_SHAPE[4][4] = {
    {0,0,1,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};
const int S_SHAPE[4][4] = {
    {0,1,1,0},
    {1,1,0,0},
    {0,0,0,0},
    {0,0,0,0}
};
const int Z_SHAPE[4][4] = {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
};

typedef struct {
    int id;
    int row;
    int col;
    int shape[4][4];
    int orientation;
}Tetromino;

// factory for a dynamically allocated tetromino
Tetromino *make_tetromino(int shape[4][4], int id) {
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
    tetromino->id = id;
    tetromino->orientation = ZERO;
    tetromino->row = 0;
    if (id == 1 || id == 2) {
        tetromino->col = 4;
    } else {
        tetromino->col = 5;
    }
    return tetromino;
}

int can_place_tetromino(Tetromino *tetromino, int game_board[HEIGHT][WIDTH]) {
    for (int rr=0; rr < 4; rr++) {
        for (int cc=0; cc < 4; cc++) {
            if (tetromino->shape[rr][cc] == 1) {
                if (
                        !(tetromino->row+rr > 0)
                        || !(tetromino->row+rr < HEIGHT-1)
                        || !(tetromino->col+cc > 0)
                        || !(tetromino->col+cc < WIDTH-1)
                        || !(game_board[tetromino->row+rr][tetromino->col+cc] == B_SQUARE)
                    ) {
                        return FALSE;
                    }
            }
        }
    }
    return TRUE;
}

int place_tetromino(Tetromino *tetromino, int game_board[HEIGHT][WIDTH]) {
    if (!can_place_tetromino(tetromino, game_board)) {
        return 1;
    }
    for (int rr=0; rr < 4; rr++) {
        for (int cc=0; cc < 4; cc++) {
            if (tetromino->shape[rr][cc] == 1) {
                game_board[tetromino->row+rr][tetromino->col+cc] = tetromino->id;
            }
        }
    }
    return 0;
}

int can_unplace_tetromino(Tetromino *tetromino, int game_board[HEIGHT][WIDTH]) {
    for (int rr=0; rr < 4; rr++) {
        for (int cc=0; cc < 4; cc++) {
            if (tetromino->shape[rr][cc] == 1) {
                if (
                        !(tetromino->row+rr > 0)
                        || !(tetromino->row+rr < HEIGHT-1)
                        || !(tetromino->col+cc > 0)
                        || !(tetromino->col+cc < WIDTH-1)
                        || !(game_board[tetromino->row+rr][tetromino->col+cc] == tetromino->id)
                    ) {
                        return FALSE;
                    }
            }
        }
    }
    return TRUE;
}

int unplace_tetromino(Tetromino *tetromino, int game_board[HEIGHT][WIDTH]) {
    if (!can_unplace_tetromino(tetromino, game_board)) {
        return 1;
    }
    for (int rr=0; rr < 4; rr++) {
        for (int cc=0; cc < 4; cc++) {
            if (tetromino->shape[rr][cc] == 1) {
                game_board[tetromino->row+rr][tetromino->col+cc] = B_SQUARE;
            }
        }
    }
    return 0;
}

// resets the game board to empty
int clear_board(int game_board[HEIGHT][WIDTH]) {
    for (int row=0; row<HEIGHT; row++) {
        for (int col=0; col<WIDTH; col++) {
            if (
                    row == 0
                    || row == HEIGHT-1
                    || col == 0
                    || col == WIDTH-1
               ) {
                game_board[row][col] = W_SQUARE;
            } else {
                game_board[row][col] = B_SQUARE;
            }
        }
    }
    return 0;
}

// write the current visual game state to the window buffer
int display_game(int game_board[HEIGHT][WIDTH]) {
    for (int row=0; row < HEIGHT; row++) {
        for (int col=0; col < WIDTH; col++) {
            attron(COLOR_PAIR(game_board[row][col]));
            printw("[]");
            attroff(COLOR_PAIR(game_board[row][col]));
        }
        printw("\n");
    }
    return 0;
}

int loop(int game_board[HEIGHT][WIDTH]) {
    int running = TRUE;
    struct timespec tick_time;
    struct timespec curr_time;
    int tps = 60;
    int fall_period = 1*tps;
    int fall_counter = 0;
    float delay_s = 1.0/tps;
    Tetromino *active_tetromino = make_tetromino(S_SHAPE, S_SQUARE);
    int ch;

    while (running) {
        place_tetromino(active_tetromino, game_board);

        // paint_tetromino(active_tetromino, game_board);
        display_game(game_board);
        refresh();
        unplace_tetromino(active_tetromino, game_board);

        clock_gettime(CLOCK_MONOTONIC, &tick_time);
        clock_gettime(CLOCK_MONOTONIC, &curr_time);
        while ((curr_time.tv_sec - tick_time.tv_sec) + (curr_time.tv_nsec - tick_time.tv_nsec)/1e9 < delay_s) {
            ch = getch();
            if (ch == KEY_LEFT) {
                active_tetromino->col--;
                if (!can_place_tetromino(active_tetromino, game_board)) {
                    active_tetromino->col++;
                }
            } else if (ch == KEY_RIGHT) {
                active_tetromino->col++;
                if (!can_place_tetromino(active_tetromino, game_board)) {
                    active_tetromino->col--;
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &curr_time);
        }

        clear();

        fall_counter++;
        if (fall_counter >= fall_period) {
            fall_counter = 0;
            active_tetromino->row++;
        }
        if (active_tetromino->row == 20) {
            running = FALSE;
        }
    }
    free(active_tetromino);
    return 0;
}

int main() {
    int game_board[HEIGHT][WIDTH];
    clear_board(game_board);

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

    loop(game_board);

    endwin();
    return 0; 
}
