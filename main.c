#include "ripes_system.h"
#include "stdio.h"
volatile unsigned int * led_base = (int*) LED_MATRIX_0_BASE;
volatile unsigned int *btn_up = (volatile unsigned int *)D_PAD_0_UP;
volatile unsigned int *btn_down = (volatile unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *btn_left = (volatile unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *btn_right = (volatile unsigned int *)D_PAD_0_RIGHT;

#define PIXEL_SIZE 2
#define DELTATIME 10000

void printPixel(volatile unsigned int *base, unsigned int color) {
    for (int i = 0; i < PIXEL_SIZE; i++) {
        for (int j = 0; j < PIXEL_SIZE; j++) {
            base[i * LED_MATRIX_0_WIDTH + j] = color;
        }
    }
}
void offPixel(volatile unsigned int *base){
    for (int i = 0; i < PIXEL_SIZE; i++) {
        for (int j = 0; j < PIXEL_SIZE; j++) {
            base[i * LED_MATRIX_0_WIDTH + j] = 0x0;
        }
    }
}
int currentDirection(int lastDirection){
    unsigned int up    = (*btn_up) & 1;
    unsigned int left  = (*btn_left) & 1;
    unsigned int down  = (*btn_down) & 1;
    unsigned int right = (*btn_right) & 1;
    if(up) return 0;
    else if(right) return 1;
    else if(down) return 2;
    else if(left) return 3;
    else return lastDirection;
}

int main() {
    int gameMatrix[LED_MATRIX_0_HEIGHT][LED_MATRIX_0_WIDTH];
    int direction;
    while(SWITCHES_0_BASE){
        direction = currentDirection(direction);
        if(direction == 0){
            printPixel(led_base, 0xFF0000);
        }
        if(direction == 1){
            printPixel(led_base, 0x00FF00);
        }
        if(direction == 2){
            printPixel(led_base, 0x0000FF);
        }
        if(direction == 3){
            printPixel(led_base, 0x00FFFF);
        }
    }
    return 0;
}