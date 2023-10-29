#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
    RGB color;
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
    // printf("\x1b[38;2;%d;%d;%dm", r, g, b);
    sprintf(esc, "\x1b[38;2;%d;%d;%dm", r, g, b);
    return esc;
}

Tetromino *make_tetromino(int shape[4][4], RGB color) {
    Tetromino *tetromino = (Tetromino*)malloc(sizeof(Tetromino));
    if (tetromino == NULL) {
        fprintf(stderr, "Memory Allocation Failed.\n");
        return NULL;
    }
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            printf("shape: %d\n", shape[i][j]);
            tetromino->shape[i][j] = shape[i][j];
            printf("tetromino->shape: %d\n", tetromino->shape[i][j]);
        }
    }
    tetromino->color = color;
    tetromino->orientation = ZERO;
    return tetromino;
}

int draw_tetromino(int row, int col, Tetromino *tetromino) {
    for (int rr = 0; rr < 4; rr++) {
        for (int cc = 0; cc < 4; cc++) {
            printf("%d\n", tetromino->shape[rr][cc]);
            if (tetromino->shape[rr][cc]) {
                char *tile;
                sprintf(tile,
                        "%s[]%s",
                        text_color_escape(
                            tetromino->color.r,
                            tetromino->color.g,
                            tetromino->color.b
                            ),
                        reset_escape()
                       );
                game_board[row+rr][col+cc] = tile;
            }
        }
    }
    return 0;
}

int draw_game() {
    for (int row=0; row< HEIGHT+2; row++) {
        for (int col=0; col < WIDTH+2; col++) {
            game_board[row][col] = "[]";
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

int main() {
    draw_game();

    Tetromino *s_tetromino = make_tetromino(S_SHAPE, S_COLOR);
    draw_tetromino(5, 5, s_tetromino);
    free(s_tetromino);
    Tetromino *t_tetromino = make_tetromino(T_SHAPE, T_COLOR);
    draw_tetromino(10, 5, s_tetromino);
    free(s_tetromino);

    display_game();
    return 0; 
}
