#pragma once

typedef struct {
    int x_offset, y_offset;
    double fraction;
} dither_kernel_element_t;

const int KERNEL_SIZE = 10;
dither_kernel_element_t dither_kernel[] = {
    { 1, 0, 5 / 32.0f},
    { 2, 0, 3 / 32.0f},
    {-2, 1, 2 / 32.0f},
    {-1, 1, 4 / 32.0f},
    { 0, 1, 5 / 32.0f},
    { 1, 1, 4 / 32.0f},
    { 2, 1, 2 / 32.0f},
    {-1, 2, 2 / 32.0f},
    { 0, 2, 3 / 32.0f},
    { 1, 2, 2 / 32.0f},
};
