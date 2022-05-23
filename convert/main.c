#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "formats.h"
#include "byte_utils.h"


const output_format_t formats_avail[] = {
    {"st7920", 1, MSB_FIRST, HORIZONTAL},
    {"ssd1306", 1, LSB_FIRST, VERTICAL},
    {"sh1106", 1, LSB_FIRST, VERTICAL},
    {"st75256", 2, MSB_FIRST, VERTICAL},
    {"ssd1327", 4, MSB_FIRST, HORIZONTAL},
    {NULL, 0, MSB_FIRST, HORIZONTAL}
};


int main(int argc, char* argv[])
{
    // Prepare ----------------------------------------------------------------
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_image> <controller_model>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* img_in_path = argv[1];
    int width, height, chs;
    uint8_t* img_in = stbi_load(img_in_path, &width, &height, &chs, STBI_grey);
    if (!img_in) {
        fprintf(stderr, "Unable to load image file '%s'\n", img_in_path);
        exit(EXIT_FAILURE);
    }

    char* format_req = argv[2];
    const output_format_t* format = NULL;
    for (int i = 0; formats_avail[i].name != NULL; i++) {
        if (strcmp(format_req, formats_avail[i].name) == 0) {
            format = &formats_avail[i];
            break;
        }
    }
    if (!format) {
        fprintf(stderr, "The controller model '%s' is not recognised\n", format_req);
        exit(EXIT_FAILURE);
    }

    // Check image dimensions -------------------------------------------------

    int x_step = (format->byte_orientation == VERTICAL) ? (1) : (8 / format->bitdepth);
    int y_step = (format->byte_orientation == HORIZONTAL) ? (1) : (8 / format->bitdepth);
    
    if (width % x_step != 0) {
        fprintf(stderr, "The width of '%s' needs to be a multiple of %d.\n", img_in_path, x_step);
        exit(EXIT_FAILURE);
    }
    if (height % y_step != 0) {
        fprintf(stderr, "The height of '%s' needs to be a multiple of %d.\n", img_in_path, y_step);
        exit(EXIT_FAILURE);
    }

    // Main processing --------------------------------------------------------

    for (int y = 0; y < height; y += y_step) {
        for (int x = 0; x < width; x += x_step) {
            uint8_t byte_out = 0x00;
            if (format->byte_orientation == VERTICAL) {
                for (int i = 0; i < y_step; i++) {
                    byte_out |= (img_in[(y + i) * width + x] & n_msb(format->bitdepth)) >> (i * format->bitdepth);
                }
            } else {
                for (int i = 0; i < x_step; i++) {
                    byte_out |= (img_in[y * width + (x + i)] & n_msb(format->bitdepth)) >> (i * format->bitdepth);
                }
            }

            printf("0x%02X, ", (format->bit_order == MSB_FIRST) ? byte_out : rev_byte(byte_out));
        }
    }

    printf("\n");


    stbi_image_free(img_in);

    exit(EXIT_SUCCESS);
}