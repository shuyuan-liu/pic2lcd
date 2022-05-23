#pragma once

typedef struct {
    int x_offset, y_offset;
    double fraction;
} dither_kernel_element_t;

const int KERNEL_SIZE = 12;
dither_kernel_element_t dither_kernel[] = {
    { 1, 0, 7 / 48.0f},
    { 2, 0, 5 / 48.0f},
    {-2, 1, 3 / 48.0f},
    {-1, 1, 5 / 48.0f},
    { 0, 1, 7 / 48.0f},
    { 1, 1, 5 / 48.0f},
    { 2, 1, 3 / 48.0f},
    {-2, 2, 1 / 48.0f},
    {-1, 2, 3 / 48.0f},
    { 0, 2, 5 / 48.0f},
    { 1, 2, 3 / 48.0f},
    { 2, 2, 1 / 48.0f}
};
