#include <cstdio>
#include <cmath>
#include <cstdint>
#include <string>
#include <tclap/CmdLine.h>
#include "EasyBMP/EasyBMP.h"

#include "dither_matrix.h"
#include "black_and_white.h"

using namespace std;
using namespace TCLAP;


int main(int argc, char** argv)
{
    // Set up command line options
    CmdLine cmd("pic2lcd: convert images into hex strings to display on monochrome LCD screens like those driven by SSD1306, SH1106, ST7525, ST7920, etc.", ' ', "Beta " __TIME__ " " __DATE__);

    UnlabeledValueArg<string> param_image_file("image", "The image to be processed", true, "", "path_to_image");
    cmd.add(param_image_file);

    ValueArg<string> param_dither_algorithm("", "dither-alg", "The dithering algorithm to use. Available values are jjn, floyd_steinberg, stucki, atikinson, burkes, sierra, sierra_2_row, sierra_lite. The default value is floyd_steinberg.", false, "floyd_steinberg", "name_of_dither_alg");
    cmd.add(param_dither_algorithm);

    ValueArg<string> param_delimiter("", "delimiter", "A string separating values in the output. The default is \", \", which outputs \"0x55, 0xAA, ...\".", false, ", ", "delimiter");
    cmd.add(param_delimiter);

    SwitchArg param_decimal("", "decimal", "Use decimal values instead of hex.", false);
    cmd.add(param_decimal);

    cmd.parse(argc, argv);

    // Read the image
    BMP image;
    if(!image.ReadFromFile(param_image_file.getValue().c_str())) {
        fprintf(stderr, "The image file could not be read.\n");
        return 1;
    }
    size_t width = image.TellWidth();
    size_t height = image.TellHeight();

    // Select the specified diterhing algorithm
    if(dither_matrices.find(param_dither_algorithm.getValue().c_str()) == dither_matrices.end()) {
        fprintf(stderr, "The dithering algorithm specified is not one of those available.\n");
        return 1;
    }
    auto dither_matrix = dither_matrices[param_dither_algorithm.getValue().c_str()];

    // Convert the image into greyscale
    // 
    // image_greyscale: an array with the same size as the input image, storing
    //                  the luminance (brightness) values of the pixels. Each
    //                  pixel in the original image will have one luminance
    //                  value here. Therefore an input image of size 128x64
    //                  will give an image_greyscale with 128*64 elements.
    uint_fast8_t *image_greyscale = new uint_fast8_t[width * height];
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            image_greyscale[y * width + x] = image(x, y)->Red * 0.3 +
                                            image(x, y)->Green * 0.59 +
                                            image(x, y)->Blue * 0.11;
        }
    }

    // Dither the image to black-and-white
    // 
    // errors: an array with the same size as the image storing the errors
    //         added to it by its beighbouring pixels. When determining
    //         whether a pixel will be white / black in the final image, 
    //         the value of (the pixel's luminance + added error) is used
    //         instead of just the luminance itself,  in order to compensate
    //         for 
    int *errors = new int[width * height];
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            size_t position = y * width + x;
            int luminance = image_greyscale[position] - errors[position];
            int error;

            if (luminance > 127) {
                image_greyscale[position] = 255;
                error = 255 - luminance;
            } else {
                image_greyscale[position] = 0;
                error = 0 - luminance;
            }

            // Distribute the error according to the dither_matrix
            // 
            // spread: an element in the dither matrix that specifies how
            //         much of the error is added to one of the neighbouring
            //         pixels.
            // 
            // dither_matrix: the collection of "spread"s that defines how the
            //                error on the current pixel is divided and added
            //                to its neighbouring pixels.
            for (const auto& spread : dither_matrix) {
                int x_offset,  y_offset; 
                double proportion;
                tie(x_offset, y_offset, proportion) = spread;

                int position = width * (y + y_offset) + (x + x_offset);
                errors[position] += error * proportion;
            }
        }
    }

    // Convert the dithered image to a list of values,  each representing
    // 8 pixels (1 byte of data).
    // 
    // Byte arrangement: vertical bytes, LSB up
    string delimiter = param_delimiter.getValue();
    bool decimal = param_decimal.getValue();
    bool first_value = true;

    for (size_t page = 0; page < height / 8; page++) {
        for (size_t col = 0; col < width; col++) {
            uint_fast8_t this_byte = 0x00;
            for (uint_fast8_t bit = 0; bit < 8; bit++) {
                size_t position = width * (page * 8 + bit) + col;
                if (image_greyscale[position]) {
                    this_byte |= 0x01 << bit;
                }
            }

            if(!first_value) { printf ("%s", delimiter.c_str()); }

            if(decimal) { printf ("%d", this_byte); }
            else { printf ("0x%02X", this_byte); }

            first_value = false;
        }
    }

    return 0;
}
