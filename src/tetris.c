#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const int WIDTH = 10;
const int HEIGHT = 20;

char *game_board[12][22];

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
