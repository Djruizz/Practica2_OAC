#include "ripes_system.h"
#include <stdlib.h>

#define PIXEL_SIZE 2
#define DELTATIME  1000
#define MATRIX_WIDTH  (LED_MATRIX_0_WIDTH / PIXEL_SIZE)
#define MATRIX_HEIGHT (LED_MATRIX_0_HEIGHT / PIXEL_SIZE)

#define EMPTY 0
#define SNAKE 1
#define APPLE 2

static volatile unsigned int * const led_base = (volatile unsigned int *)LED_MATRIX_0_BASE;
static volatile unsigned int * const dpad = (volatile unsigned int *)D_PAD_0_BASE;
static volatile unsigned int * const switches = (volatile unsigned int *)SWITCHES_0_BASE;

void printPixel(volatile unsigned int *base, unsigned int color) {
    for (int i = 0; i < PIXEL_SIZE; i++) {
        for (int j = 0; j < PIXEL_SIZE; j++) {
            base[i * LED_MATRIX_0_WIDTH + j] = color;
        }
    }
}

void offPixel(volatile unsigned int *base) {
    printPixel(base, 0x000000);
}

int currentDirection(int lastDirection) {
    unsigned int up    = dpad[0] & 1; // offset 0x0
    unsigned int down  = dpad[1] & 1; // offset 0x4
    unsigned int left  = dpad[2] & 1; // offset 0x8
    unsigned int right = dpad[3] & 1; // offset 0xc

    if (up) return 0;
    if (right) return 1;
    if (down) return 2;
    if (left) return 3;
    return lastDirection;
}

void waitTime(void) {
    for (volatile int i = 0; i < DELTATIME; i++);
}

int isSnakeCell(int snakeRow[MATRIX_WIDTH * MATRIX_HEIGHT],
                int snakeColumn[MATRIX_WIDTH * MATRIX_HEIGHT],
                int snakeLength,
                int row,
                int column) {
    for (int i = 0; i < snakeLength; i++) {
        if (snakeRow[i] == row && snakeColumn[i] == column) {
            return 1;
        }
    }
    return 0;
}

int placeApple(int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH],
               int snakeRow[MATRIX_WIDTH * MATRIX_HEIGHT],
               int snakeColumn[MATRIX_WIDTH * MATRIX_HEIGHT],
               int snakeLength) {
    int row;
    int column;

    do {
        row = rand() % MATRIX_HEIGHT;
        column = rand() % MATRIX_WIDTH;
    } while (gameMatrix[row][column] != EMPTY ||
             isSnakeCell(snakeRow, snakeColumn, snakeLength, row, column));

    gameMatrix[row][column] = APPLE;
    return 1;
}

void initGameMatrix(int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH],
                    int snakeRow[MATRIX_WIDTH * MATRIX_HEIGHT],
                    int snakeColumn[MATRIX_WIDTH * MATRIX_HEIGHT],
                    int *snakeLength) {
    for (int row = 0; row < MATRIX_HEIGHT; row++) {
        for (int column = 0; column < MATRIX_WIDTH; column++) {
            gameMatrix[row][column] = EMPTY;
        }
    }

    *snakeLength = 1;
    snakeRow[0] = 5;
    snakeColumn[0] = 5;
   

    for (int i = 0; i < *snakeLength; i++) {
        gameMatrix[snakeRow[i]][snakeColumn[i]] = SNAKE;
    }

    srand(*switches);
    placeApple(gameMatrix, snakeRow, snakeColumn, *snakeLength);
}

int moveSnake(int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH],
              int snakeRow[MATRIX_WIDTH * MATRIX_HEIGHT],
              int snakeColumn[MATRIX_WIDTH * MATRIX_HEIGHT],
              int *snakeLength,
              int direction) {
    int nextRow = snakeRow[0];
    int nextColumn = snakeColumn[0];
    int ateApple;

    if (direction == 0) nextRow--;
    if (direction == 1) nextColumn++;
    if (direction == 2) nextRow++;
    if (direction == 3) nextColumn--;

    if (nextRow < 0 || nextRow >= MATRIX_HEIGHT ||
        nextColumn < 0 || nextColumn >= MATRIX_WIDTH) {
        return 0;
    }

    ateApple = gameMatrix[nextRow][nextColumn] == APPLE;
    if (isSnakeCell(snakeRow, snakeColumn, *snakeLength, nextRow, nextColumn) &&
        !(nextRow == snakeRow[*snakeLength - 1] &&
          nextColumn == snakeColumn[*snakeLength - 1] &&
          !ateApple)) {
        return 0;
    }

    if (!ateApple) {
        gameMatrix[snakeRow[*snakeLength - 1]][snakeColumn[*snakeLength - 1]] = EMPTY;
    } else {
        (*snakeLength)++;
    }

    for (int i = *snakeLength - 1; i > 0; i--) {
        snakeRow[i] = snakeRow[i - 1];
        snakeColumn[i] = snakeColumn[i - 1];
    }

    snakeRow[0] = nextRow;
    snakeColumn[0] = nextColumn;
    gameMatrix[nextRow][nextColumn] = SNAKE;

    if (ateApple) {
        placeApple(gameMatrix, snakeRow, snakeColumn, *snakeLength);
    }

    return 1;
}

void refreshScreen(int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH]) {
    for (int row = 0; row < MATRIX_HEIGHT; row++) {
        for (int column = 0; column < MATRIX_WIDTH; column++) {
            volatile unsigned int *pixel =
                led_base + row * PIXEL_SIZE * LED_MATRIX_0_WIDTH
                         + column * PIXEL_SIZE;

            if (gameMatrix[row][column] == SNAKE) {
                printPixel(pixel, 0x00FF00);
            } else if (gameMatrix[row][column] == APPLE) {
                printPixel(pixel, 0xFF0000);
            } else {
                offPixel(pixel);
            }
        }
    }
}

int main(void) {
    int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH];
    int snakeRow[MATRIX_WIDTH * MATRIX_HEIGHT];
    int snakeColumn[MATRIX_WIDTH * MATRIX_HEIGHT];
    int snakeLength;

    while (1) {
        initGameMatrix(gameMatrix, snakeRow, snakeColumn, &snakeLength);

        int direction = 1;
        refreshScreen(gameMatrix);

        while (*switches & 1) {
            direction = currentDirection(direction);
            if (!moveSnake(gameMatrix, snakeRow, snakeColumn, &snakeLength, direction)) {
                break;
            }
            refreshScreen(gameMatrix);
            waitTime();
        }

        offPixel(led_base);

        while (*switches & 1) {
            waitTime();
        }
        while (!(*switches & 1)) {
            waitTime();
        }
    }
}
