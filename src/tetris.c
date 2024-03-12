#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>
#include "constants.h"

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

typedef struct {
    int id;
    int row;
    int col;
    int shape[4][4][4];
    int orientation;
}Tetromino;

// factory for a dynamically allocated tetromino
Tetromino *make_tetromino(int id) {
    Tetromino *tetromino = (Tetromino*)malloc(sizeof(Tetromino));
    if (tetromino == NULL) {
        fprintf(stderr, "Memory Allocation Failed.\n");
        return NULL;
    }
    switch (id) {
        case 1:
            memcpy(tetromino->shape, I_SHAPE, sizeof(tetromino->shape));
            break;
        case 2:
            memcpy(tetromino->shape, O_SHAPE, sizeof(tetromino->shape));
            break;
        case 3:
            memcpy(tetromino->shape, T_SHAPE, sizeof(tetromino->shape));
            break;
        case 4:
            memcpy(tetromino->shape, J_SHAPE, sizeof(tetromino->shape));
            break;
        case 5:
            memcpy(tetromino->shape, L_SHAPE, sizeof(tetromino->shape));
            break;
        case 6:
            memcpy(tetromino->shape, S_SHAPE, sizeof(tetromino->shape));
            break;
        case 7:
            memcpy(tetromino->shape, Z_SHAPE, sizeof(tetromino->shape));
            break;
    }
    tetromino->id = id;
    tetromino->orientation = ZERO;
    tetromino->row = 1;
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
            if (tetromino->shape[tetromino->orientation][rr][cc] == 1) {
                if (
                        (
                            !(tetromino->row+rr > 0)
                            || !(tetromino->row+rr < HEIGHT-1)
                            || !(tetromino->col+cc > 0)
                            || !(tetromino->col+cc < WIDTH-1)
                            || !(game_board[tetromino->row+rr][tetromino->col+cc] 
                                == B_SQUARE)
                        ) && (
                            !(tetromino->row+rr == 0)
                            || !(game_board[tetromino->row+rr][tetromino->col+cc] 
                                == W_SQUARE)
                            || !(tetromino->col+cc > 0)
                            || !(tetromino->col+cc < WIDTH-1)
                        )
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
            if (tetromino->shape[tetromino->orientation][rr][cc] == 1
                    && tetromino->row+rr > 0) {
                game_board[tetromino->row+rr][tetromino->col+cc] 
                    = tetromino->id;
            }
        }
    }
    return 0;
}

int can_unplace_tetromino(Tetromino *tetromino, int game_board[HEIGHT][WIDTH]) {
    for (int rr=0; rr < 4; rr++) {
        for (int cc=0; cc < 4; cc++) {
            if (tetromino->shape[tetromino->orientation][rr][cc] == 1) {
                if ((!(tetromino->row + rr > 0) ||
                     !(tetromino->row + rr < HEIGHT - 1) ||
                     !(tetromino->col + cc > 0) ||
                     !(tetromino->col + cc < WIDTH - 1) ||
                     !(game_board[tetromino->row + rr][tetromino->col + cc] ==
                       tetromino->id)) &&
                    (!(tetromino->row + rr == 0) ||
                     !(game_board[tetromino->row + rr][tetromino->col + cc] ==
                       W_SQUARE) ||
                     !(tetromino->col + cc > 0) ||
                     !(tetromino->col + cc < WIDTH - 1))) {
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
            if (tetromino->shape[tetromino->orientation][rr][cc] == 1
                    && tetromino->row+rr > 0) {
                game_board[tetromino->row+rr][tetromino->col+cc] = B_SQUARE;
            }
        }
    }
    return 0;
}

int compute_kick_index(int orientation, int prev_orientation) {
    if (prev_orientation == ZERO && orientation == RIGHT) {
        return 0;
    } if (prev_orientation == RIGHT && orientation == ZERO) {
        return 1;
    } if (prev_orientation == RIGHT && orientation == TWO) {
        return 2;
    } if (prev_orientation == TWO && orientation == RIGHT) {
        return 3;
    } if (prev_orientation == TWO && orientation == LEFT) {
        return 4;
    } if (prev_orientation == LEFT && orientation == TWO) {
        return 5;
    } if (prev_orientation == LEFT && orientation == ZERO) {
        return 6;
    } if (prev_orientation == ZERO && orientation == LEFT) {
        return 7;
    } else {
        return 8;
    }
}

// moves the tetromino according to the wall kick rules of the current attempt
int wall_kick(Tetromino *tetromino, int game_board[HEIGHT][WIDTH],
        int prev_orientation, int attempt) {
    if (tetromino->id == O_SQUARE) {
        return 0;
    } else if (tetromino->id == I_SQUARE) {
        int kick_index =
            compute_kick_index(tetromino->orientation, prev_orientation);
        int dx = I_KICK[kick_index][attempt][0];
        int dy = I_KICK[kick_index][attempt][1];
        tetromino->col = tetromino->col + dx;
        tetromino->row = tetromino->row - dy;
        return 0;
    } else {
        int kick_index =
            compute_kick_index(tetromino->orientation, prev_orientation);
        int dx = X_KICK[kick_index][attempt][0];
        int dy = X_KICK[kick_index][attempt][1];
        tetromino->col = tetromino->col + dx;
        tetromino->row = tetromino->row - dy;
        return 0;
    }
}

// attempts to rotate and wall kick the tetromino
int rotate_tetromino(Tetromino *tetromino, int game_board[HEIGHT][WIDTH],
        int direction) {
    if (direction != CW && direction != CCW) {
        return 1;
    }
    int prev_row = tetromino->row;
    int prev_col = tetromino->col;
    int prev_orientation = tetromino->orientation;
    tetromino->orientation = (tetromino->orientation + 4 + direction) % 4;

    for (int attempt = 0; attempt < 4; attempt++) {
        if (can_place_tetromino(tetromino, game_board)) {
            return 0;
        } else {
            tetromino->row = prev_row;
            tetromino->col = prev_col;
            wall_kick(tetromino, game_board, prev_orientation, attempt);
        }
    }
    tetromino->row = prev_row;
    tetromino->col = prev_col;
    tetromino->orientation = prev_orientation;
    return 0;
}

// deletes a given row from the game board, dropping down the above squares
int clear_line(int row, int game_board[HEIGHT][WIDTH]) {
    if (row < 1 || row >= HEIGHT - 1) {
        return 1;
    }
    for (int i = row; i > 1; i--) {
        memcpy(game_board[i], game_board[i - 1], WIDTH * sizeof(int));
    }

    for (int col = 0; col < WIDTH; col++) {
        if (col == 0 || col == WIDTH - 1) {
            game_board[1][col] = W_SQUARE;
        } else {
            game_board[1][col] = B_SQUARE;
        }
    }
    return 0;
}

// checks for rows filled completely by tetromino squares and deletes them,
// dropping down the tetromino squares above
int clear_filled_lines(int game_board[HEIGHT][WIDTH]) {
    int count = 0;
    for (int row = 1; row < HEIGHT - 1; row++) {
        int full = TRUE;
        for (int col = 0; col < WIDTH; col++) {
            if (game_board[row][col] == B_SQUARE) {
                full = FALSE;
            }
        }
        if (full) {
            count++;
            clear_line(row, game_board);
        }
    }
    return count;
}

int compute_score(int lines_cleared, int level, char log_msg[2*WIDTH]) {
    switch (lines_cleared) {
        case 4: 
            strncpy(log_msg, "Tetris!!!", 2*WIDTH);
            return 1200*(level+1);
        case 3: 
            strncpy(log_msg, "Triple Clear!!", 2*WIDTH);
            return 300*(level+1);
        case 2: 
            strncpy(log_msg, "Double Clear!", 2*WIDTH);
            return 100*(level+1);
        case 1: 
            strncpy(log_msg, "Line Cleared", 2*WIDTH);
            return 40*(level+1);
    }
    return 0;
}

// resets the game board to empty
int clear_board(int game_board[HEIGHT][WIDTH]) {
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            if (row == 0 || row == HEIGHT - 1 || col == 0 ||
                    col == WIDTH - 1) {
                game_board[row][col] = W_SQUARE;
            } else {
                game_board[row][col] = B_SQUARE;
            }
        }
    }
    return 0;
}

// write the current visual game state to the window buffer
int display_game(int game_board[HEIGHT][WIDTH], WINDOW *board_window) {
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            wattron(board_window, COLOR_PAIR(game_board[row][col]));
            wprintw(board_window, "[]");
            wattroff(board_window, COLOR_PAIR(game_board[row][col]));
        }
        wprintw(board_window, "\n");
    }
    return 0;
}

int count_digits(int num) {
    if (num == 0) {
        return 1;
    }
    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    return count;
}

int loop(
        int game_board[HEIGHT][WIDTH],
        WINDOW *board_window,
        WINDOW *score_window,
        WINDOW *level_window,
        WINDOW *log_window
        ) {
    int running = TRUE;
    int score = 0;
    int lines_cleared;
    int total_lines_cleared = 0;
    int level = 1;

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    struct timespec tick_time;
    struct timespec curr_time;

    int tps = 60;
    int fall_period = tps / level;
    int fall_counter = 0;
    float delay_s = 1.0 / tps;

    char log_msg[2*WIDTH] = "Starting new game...";

    int next_tetromino_id = (rand() % 7) + 1;
    Tetromino *active_tetromino = make_tetromino(next_tetromino_id);
    int ch;

    while (running) {
        wmove(board_window, 0, 0);
        lines_cleared = clear_filled_lines(game_board);
        if (lines_cleared > 0) {
            total_lines_cleared += lines_cleared;
            score += compute_score(lines_cleared, level, log_msg);
            level = 1 + total_lines_cleared / 10;
            fall_period = tps / level;
        }
        place_tetromino(active_tetromino, game_board);
        display_game(game_board, board_window);
        wrefresh(board_window);

        wmove(level_window, 0, 0);
        wprintw(level_window, "Level:");
        for (int i=0; i<2*WIDTH-6-count_digits(level); i++) {
            wprintw(level_window, " ");
        }
        wprintw(level_window, "%d", level);
        wrefresh(level_window);

        wmove(score_window, 0, 0);
        clock_gettime(CLOCK_MONOTONIC, &curr_time);
        wprintw(score_window, "%02d:%02d",
                (curr_time.tv_sec-start_time.tv_sec)/60,
                (curr_time.tv_sec-start_time.tv_sec)%60);
        for (int i=0; i<2*WIDTH-5-count_digits(score); i++) {
            wprintw(score_window, " ");
        }
        wprintw(score_window, "%d", score);
        wrefresh(score_window);

        wmove(log_window, 0, 0);
        wprintw(log_window, "%s", log_msg);
        wrefresh(log_window);

        unplace_tetromino(active_tetromino, game_board);

        clock_gettime(CLOCK_MONOTONIC, &tick_time);
        clock_gettime(CLOCK_MONOTONIC, &curr_time);
        while ((curr_time.tv_sec - tick_time.tv_sec) +
                (curr_time.tv_nsec - tick_time.tv_nsec) / 1e9 <
                delay_s) {
            ch = getch();
            if (ch == KEY_LEFT) {
                active_tetromino->col--;
                if (!can_place_tetromino(active_tetromino,
                            game_board)) {
                    active_tetromino->col++;
                }
            } else if (ch == KEY_RIGHT) {
                active_tetromino->col++;
                if (!can_place_tetromino(active_tetromino,
                            game_board)) {
                    active_tetromino->col--;
                }
            } else if (ch == KEY_DOWN) {
                active_tetromino->row++;
                if (!can_place_tetromino(active_tetromino,
                            game_board)) {
                    active_tetromino->row--;
                }
            } else if (ch == ' ') {
                while (can_place_tetromino(active_tetromino,
                            game_board)) {
                    active_tetromino->row++;
                }
                active_tetromino->row--;
                // don't let any more inputs register after hard drop
                // and immediately end the current fall period
                fall_counter = fall_period;
                break;
            } else if (ch == 'z' || ch == 'Z') {
                rotate_tetromino(active_tetromino, game_board, CCW);
            } else if (ch == 'x' || ch == 'X') {
                rotate_tetromino(active_tetromino, game_board, CW);
            }
            clock_gettime(CLOCK_MONOTONIC, &curr_time);
        }
        fall_counter++;
        if (fall_counter >= fall_period) {
            fall_counter = 0;
            active_tetromino->row++;
            if (!can_place_tetromino(active_tetromino, game_board)) {
                active_tetromino->row--;
                place_tetromino(active_tetromino, game_board);
                free(active_tetromino);
                next_tetromino_id = (rand() % 7) + 1;
                active_tetromino = make_tetromino(next_tetromino_id);
                if (!can_place_tetromino(active_tetromino, game_board)) {
                   running = false;
                }
            }
            strncpy(log_msg, "\n", sizeof(log_msg)-1);
        }
    }
    free(active_tetromino);
    return 0;
}

int menu_loop(
        WINDOW *board_window,
        WINDOW *score_window,
        WINDOW *level_window,
        WINDOW *log_window
        ) {
    int quit = 0;
    while (!quit) {
        int game_board[HEIGHT][WIDTH];
        clear_board(game_board);
        loop(game_board, board_window, score_window, level_window, log_window);
    }
    return 0;
}

int main() {
    srand(time(NULL));

    initscr();
    // make getch non-blocking to reduce text flashing
    nodelay(stdscr, TRUE);
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

    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);
    int window_origin_y = (term_height - (2*WIDTH+1))/2;
    int window_origin_x = (term_width - HEIGHT)/2;

    WINDOW *level_window = newwin(1, 2*WIDTH+1, window_origin_y-1, window_origin_x);
    WINDOW *board_window = newwin(HEIGHT, 2*WIDTH+1, window_origin_y, window_origin_x);
    WINDOW *score_window = newwin(1, 2*WIDTH+1, window_origin_y+22, window_origin_x);
    WINDOW *log_window = newwin(1, 2*WIDTH+1, window_origin_y+24, window_origin_x);

    wattron(board_window, A_STANDOUT);

    menu_loop(board_window, score_window, level_window, log_window);

    delwin(board_window);
    delwin(score_window);
    endwin();
    return 0;
}
