#include <stdio.h>
#include <stdint.h>

int main() {
    int x = 15;
    int y = 7;
    uint8_t byte;
    byte = (uint8_t) x;
    byte = (byte & 0x0F) | (y << 4);
    printf("byte: %u\n", byte);

    int x_new = (byte & 0x0F);
    int y_new = (byte >> 4) & 0x0F;
    printf("x: %d, y: %d\n", x_new, y_new);
}