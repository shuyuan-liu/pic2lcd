#include "dither.hpp"
#include <cstddef>
#include <cstring>


// I made these variables global because I don't want to be passing them to
// index_of every time I use it
int w = 0;
int h = 0;
int trash_index = 0;


int index_of (int x, int y, int x_offset, int y_offset)
{
    if ((x + x_offset) < 0 || (x + x_offset) >= w || (y + y_offset) < 0 || (y + y_offset) >= h) {
        return trash_index;
    } else {
        return (y + y_offset) * w + x + x_offset;
    }
}


void dither (png::image<png::gray_pixel> &image)
{
    w = image.get_width();
    h = image.get_height();

    double *errors = new double[w * h + 1](); // One extra space for out-of-bounds writes to go to
    trash_index = sizeof(errors) - 1;
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            int original_val = image[y][x];
            int corrected_val = original_val - errors[index_of(x, y, 0, 0)];
            int final_val = (corrected_val > 127) ? 255 : 0;
            int error = final_val - corrected_val;
            image[y][x] = final_val;

            for (auto k : dither_matrix) {
                int x_offset = k.first.first;
                int y_offset = k.first.second;
                double proportion = k.second;
                errors[index_of(x, y, x_offset, y_offset)] += error * proportion;
            }
        }
    }

    delete [] errors;
}