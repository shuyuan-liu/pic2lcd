#include "dither.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>


int w = 0;
int h = 0;
int trash_index = 0;


int index_of (int x, int y, int x_offset, int y_offset)
{
    if (x + x_offset < 0 || x + x_offset >= w || y + y_offset < 0 || y + y_offset >= h) {
        return trash_index;
    }

    return y_offset * w + x + x_offset;
}


void dither (png::image<png::gray_pixel> &image)
{
    // This function implements the JJN dithering algorithm.
    // The algorithm is taken from:
    // https://tannerhelland.com/2012/12/28/dithering-eleven-algorithms-source-code.html

    w = image.get_width();
    h = image.get_height();

    double *errors = new double[3 * w + 1](); // One extra space for out-of-bounds writes to go to
    trash_index = sizeof(errors) - 1;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            int original_val = image[y][x];
            int final_val = ((original_val - errors[x]) > 127) ? 255 : 0;
            double error = final_val - original_val;
            image[y][x] = final_val;

            double error_7 = error * (7.0 / 48.0);
            double error_5 = error * (5.0 / 48.0);
            double error_3 = error * (3.0 / 48.0);
            double error_1 = error * (1.0 / 48.0);
            std::cerr << "(" << x << ", " << y << "): " << error << ", " << error_7 << ", " << error_5 <<", " << error_3 <<", " << error_1 << std::endl;
            
            errors[index_of(x, y,  1, 0)] += error_7;
            errors[index_of(x, y,  2, 0)] += error_5;
            errors[index_of(x, y, -2, 1)] += error_3;
            errors[index_of(x, y, -1, 1)] += error_5;
            errors[index_of(x, y,  0, 1)] += error_7;
            errors[index_of(x, y,  1, 1)] += error_5;
            errors[index_of(x, y,  2, 1)] += error_3;
            errors[index_of(x, y, -2, 2)] += error_1;
            errors[index_of(x, y, -1, 2)] += error_3;
            errors[index_of(x, y,  0, 2)] += error_5;
            errors[index_of(x, y,  1, 2)] += error_3;
            errors[index_of(x, y,  2, 2)] += error_1;
        }

        // We are progressing to the next line so shift the next line's errors
        // into the current line's space.
        std::memmove (errors, errors + w, w * sizeof(double));
        std::memmove (errors + w, errors + 2 * w, w * sizeof(double));
        

        // Clear the space for the next line so the previously next-line error
        // values won't interfere with new ones.
        std::memset (errors + 2 * w, 0, w * sizeof(double));
    }

    delete [] errors;
}