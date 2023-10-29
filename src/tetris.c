#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <termios.h>

#define WIDTH 10
#define HEIGHT 20

typedef struct {
    int r;
    int g;
    int b;
}RGB;

const RGB I_COLOR = { 0x38, 0x40, 0x9B };
const RGB O_COLOR = { 0xDB, 0xDB, 0x4C };
const RGB T_COLOR = { 0xDC, 0x4C, 0x9E };
const RGB J_COLOR = { 0xDB, 0x8B, 0x45 };
const RGB L_COLOR = { 0x3E, 0xB5, 0xC9};
const RGB S_COLOR = { 0x40, 0xAE, 0x7D };
const RGB Z_COLOR = { 0xDA, 0x3C, 0x40 };

typedef enum {
    ZERO,
    NINETY,
    ONE_EIGHTY,
    TWO_SEVENTY
} Orientation;

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
    const RGB *color;
    Orientation orientation;
}Tetromino;

char *game_board[HEIGHT+2][WIDTH+2];

char *reset_escape() {
    return "\x1b[0m";
}

char *text_color_escape(int r, int g, int b) {
    int length = snprintf(NULL, 0, "\x1b[38;2;%d;%d;%dm", r, g, b); 
    if (length <= 0) {
        return NULL;
    }
    char *esc = (char*)malloc(length+1);
    if (esc == NULL) {
        return NULL;
    }
    sprintf(esc, "\x1b[38;2;%d;%d;%dm", r, g, b);
    return esc;
}

Tetromino *make_tetromino(int shape[4][4], const RGB *color) {
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

int draw_tetromino(int row, int col, Tetromino *tetromino) {
    for (int rr = 0; rr < 4; rr++) {
        for (int cc = 0; cc < 4; cc++) {
            if (
                    tetromino->shape[rr][cc]
                    && row+rr > 0
                    && row+rr < HEIGHT+1
                    && col+cc > 0
                    && col+cc < WIDTH+1
               ) {
                char *tc_esc = text_color_escape(
                        tetromino->color->r,
                        tetromino->color->g,
                        tetromino->color->b
                        );
                char *r_esc = reset_escape();
                int length = snprintf(NULL, 0, "%s[]%s", tc_esc, r_esc);
                char *tile = (char*)malloc(length+1);
                sprintf(tile, "%s[]%s", tc_esc, r_esc);
                game_board[row+rr][col+cc] = tile;
            }
        }
    }
    return 0;
}

// WARN: If there is ever a case where all squares of the board are tetrominos
// WARN: Then this code will memory leak the tile
int draw_board() {
    char *tile = strdup("[]");
    if (tile == NULL) {
        fprintf(stderr, "Memory Allocation Failed.\n");
        return 1;
    }
    for (int row=0; row < HEIGHT+2; row++) {
        for (int col=0; col < WIDTH+2; col++) {
            game_board[row][col] = tile;
        }
    }
    return 0;
}

int clear_board() {
    for (int row=0; row < HEIGHT+2; row++) {
        for (int col=0; col < WIDTH+2; col++) {
            if (game_board[row][col] != NULL) {
                free(game_board[row][col]);
            }
        }
    }
    return 0;
}

int display_game() {
    for (int row=0; row < HEIGHT+2; row++) {
        for (int col=0; col < WIDTH+2; col++) {
            printf("%s", game_board[row][col]);
        }
        printf("\n");
    }
    return 0;
}

int clear_game() {
    for (int row=0; row < HEIGHT+2; row++) {
        printf("\033[A\033[K");
    }
    return 0;
}

int loop() {
    int running = 1;
    int delay_us = 1000000;
    Tetromino *s_tetromino = make_tetromino(S_SHAPE, &S_COLOR);
    Tetromino *t_tetromino = make_tetromino(T_SHAPE, &T_COLOR);
    int row = -1;
    while (running) {
        draw_board();
        draw_tetromino(row, 5, s_tetromino);
        draw_tetromino(19, 1, t_tetromino);
        display_game();
        usleep(delay_us);
        clear_game();
        row++;
    }
    free(s_tetromino);
    free(t_tetromino);
    return 0;
}

int disable_echo() {
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), 0, &term);
    return 0;
}

int main() {
    disable_echo();
    loop();

    return 0; 
}
