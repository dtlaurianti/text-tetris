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

char *game_board[WIDTH+2][HEIGHT+2];

int draw_game() {
    for (int row=0; row < HEIGHT+2; row++) {
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
    display_game();
    return 0; 
}
