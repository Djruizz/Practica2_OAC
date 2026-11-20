#include "ripes_system.h"

#define PIXEL_SIZE 2
#define DELTATIME  10000
#define MATRIX_WIDTH  (LED_MATRIX_0_WIDTH / PIXEL_SIZE)
#define MATRIX_HEIGHT (LED_MATRIX_0_HEIGHT / PIXEL_SIZE)
#define SNAKE_LENGTH 5

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

void initGameMatrix(int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH],
                    int snakeRow[SNAKE_LENGTH],
                    int snakeColumn[SNAKE_LENGTH]) {
    for (int row = 0; row < MATRIX_HEIGHT; row++) {
        for (int column = 0; column < MATRIX_WIDTH; column++) {
            gameMatrix[row][column] = EMPTY;
        }
    }

    snakeRow[0] = 5;
    snakeColumn[0] = 5;
    snakeRow[1] = 5;
    snakeColumn[1] = 4;
    snakeRow[2] = 5;
    snakeColumn[2] = 3;

    for (int i = 0; i < SNAKE_LENGTH; i++) {
        gameMatrix[snakeRow[i]][snakeColumn[i]] = SNAKE;
    }

    gameMatrix[7][7] = APPLE;
}

void moveSnake(int gameMatrix[MATRIX_HEIGHT][MATRIX_WIDTH],
               int snakeRow[SNAKE_LENGTH],
               int snakeColumn[SNAKE_LENGTH],
               int direction) {
    int nextRow = snakeRow[0];
    int nextColumn = snakeColumn[0];

    if (direction == 0) nextRow--;
    if (direction == 1) nextColumn++;
    if (direction == 2) nextRow++;
    if (direction == 3) nextColumn--;

    if (nextRow < 0) nextRow = MATRIX_HEIGHT - 1;
    if (nextRow >= MATRIX_HEIGHT) nextRow = 0;
    if (nextColumn < 0) nextColumn = MATRIX_WIDTH - 1;
    if (nextColumn >= MATRIX_WIDTH) nextColumn = 0;

    gameMatrix[snakeRow[SNAKE_LENGTH - 1]][snakeColumn[SNAKE_LENGTH - 1]] = EMPTY;

    for (int i = SNAKE_LENGTH - 1; i > 0; i--) {
        snakeRow[i] = snakeRow[i - 1];
        snakeColumn[i] = snakeColumn[i - 1];
    }

    snakeRow[0] = nextRow;
    snakeColumn[0] = nextColumn;
    gameMatrix[nextRow][nextColumn] = SNAKE;
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
    int snakeRow[SNAKE_LENGTH];
    int snakeColumn[SNAKE_LENGTH];

    initGameMatrix(gameMatrix, snakeRow, snakeColumn);

    int direction = 1;

    while (*switches & 1) {
        direction = currentDirection(direction);
        moveSnake(gameMatrix, snakeRow, snakeColumn, direction);
        refreshScreen(gameMatrix);
        waitTime();
    }

    offPixel(led_base);
    return 0;
}