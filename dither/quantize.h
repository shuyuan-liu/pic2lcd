#pragma once

#include <stdint.h>
#include <math.h>

// Requires 1 <= n_bits <= 8
uint8_t quantize_to_n_bits(double in, int n_bits)
{
    double result = floor(in / pow(2, 8 - n_bits)) / (pow(2, n_bits) - 1) * 255.0f;
    
    // Remember to clamp values! Otherwise output may contain stark lines
    if (result < 0) return 0;
    if (result > 255) return 255;
    return (uint8_t)round(result);
}