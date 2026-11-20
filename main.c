#include <stdint.h>
#include "ripes_system.h"

#define PIXEL_SIZE 2
#define DELTATIME  10000

static volatile unsigned int * const led_base = (volatile unsigned int *)LED_MATRIX_0_BASE;
static volatile unsigned int * const dpad     = (volatile unsigned int *)D_PAD_0_BASE;
static volatile unsigned int * const switches = (volatile unsigned int *)SWITCHES_0_BASE;

static inline void printPixel(volatile unsigned int *base, unsigned int color) {
    for (int i = 0; i < PIXEL_SIZE; i++) {
        for (int j = 0; j < PIXEL_SIZE; j++) {
            base[i * LED_MATRIX_0_WIDTH + j] = color;
        }
    }
}

static inline void offPixel(volatile unsigned int *base) {
    printPixel(base, 0x000000);
}

int currentDirection(int lastDirection) {
    // Se leen los 4 registros para limpiar sus flags de captura
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

int main(void) {
    static const uint32_t colors[4] = {
        0xFF0000, // 0: UP
        0x00FF00, // 1: RIGHT
        0x0000FF, // 2: DOWN
        0x00FFFF  // 3: LEFT
    };

    int direction = 1;
    int prevDirection = -1; 

    while (*switches & 1) {
        direction = currentDirection(direction);

        if (direction != prevDirection) {
            printPixel(led_base, colors[direction]);
            prevDirection = direction;
        }
    }

    offPixel(led_base);
    return 0;
}