#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "dither_kernel.h"
#include "quantize.h"


int main(int argc, char* argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <quantize_to_n_bits> <input_image> <output.png>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int quantized_num_bits = atoi(argv[1]);
    if (!quantized_num_bits || quantized_num_bits < 0 || quantized_num_bits > 8) {
        fprintf(stderr, "Cannot produce output quantized to '%s' bits\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char* img_in_path = argv[2];
    int width, height, chs;
    uint8_t* img_in = stbi_load(img_in_path, &width, &height, &chs, STBI_grey);
    if (!img_in) {
        fprintf(stderr, "Unable to load image file '%s'\n", img_in_path);
        exit(EXIT_FAILURE);
    }

    int num_pixels = width * height;
    uint8_t* img_out = malloc(num_pixels * sizeof(uint8_t));
    double* errs = malloc(num_pixels * sizeof(double));

    memset(errs, 0, num_pixels * sizeof(double));

    // Dithering --------------------------------------------------------------

    int shift_dist = 8 - quantized_num_bits;
    int scale_to_255 = 1 / (pow(2, quantized_num_bits) - 1) * 255.0f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double p_in = img_in[y * width + x] - errs[y * width + x];
            uint8_t p_out = quantize_to_n_bits(p_in, quantized_num_bits);
            double p_err = p_out - p_in;

            img_out[y * width + x] = p_out;

            for (int i = 0; i < KERNEL_SIZE; i++) {
                dither_kernel_element_t portion = dither_kernel[i];
                int to_x = x + portion.x_offset;
                int to_y = y + portion.y_offset;

                // Don't diffuse error out of bounds
                if (to_x < 0 || to_x >= width || to_y < 0 || to_y >= height) {
                    continue;
                }

                errs[to_y * width + to_x] += p_err * portion.fraction;
            }
        }
    }

    // Write out result -------------------------------------------------------

    char* img_out_path = argv[3];
    if (!stbi_write_png(img_out_path, width, height, STBI_grey, img_out, width)) {
        fprintf(stderr, "Dithering finished but failed to save result to '%s'\n", img_out_path);
        
        stbi_image_free(img_in);
        free(img_out);
        free(errs);

        exit(EXIT_FAILURE);
    }

    // Clean up ---------------------------------------------------------------

    stbi_image_free(img_in);
    free(img_out);
    free(errs);

    exit(EXIT_SUCCESS);
}