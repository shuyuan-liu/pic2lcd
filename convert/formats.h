#pragma once

#include <unistd.h> // for NULL

typedef enum {
    MSB_FIRST,
    LSB_FIRST
} bit_order_t;

typedef enum {
    HORIZONTAL,
    VERTICAL
} orientation_t;

typedef struct {
    const char* name;
    const int bitdepth;
    const bit_order_t bit_order;
    const orientation_t byte_orientation;
} output_format_t;

extern const output_format_t formats_avail[];