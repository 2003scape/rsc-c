#include "arrow_yuv.h"

int arrow_yuv_width = 12;
int arrow_yuv_height = 20;

int arrow_yuv_lines[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                         9, 7, 8, 8, 4, 4, 4, 4, 2, 0};

int arrow_yuv_offsets[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 5, 5, 6, 6, 7};

uint8_t arrow_yuv[] = {
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80,
    0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80,
    0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80,
    0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xeb, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0xeb, 0x80,
    0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0xeb, 0x80,
    0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80};
