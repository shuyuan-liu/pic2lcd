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
    CmdLine cmd("pic2lcd: Convert images into raw data for monochrome LCD screens like those driven by SSD1306, SH1106, ST7525, ST7920, etc.", ' ', "Build " __TIME__ " " __DATE__);

    UnlabeledValueArg<string> param_image_file("image", "The image to convert", true, "", "path_to_image");
    cmd.add(param_image_file);

    ValueArg<string> param_dither_algorithm("a", "dither-alg", "Select the dithering algorithm to dither the image with. Default: floyd_steinberg. Available: jjn, floyd_steinberg, stucki, atikinson, burkes, sierra, sierra_2_row, sierra_lite.", false, "floyd_steinberg", "dither_alg");
    cmd.add(param_dither_algorithm);

    ValueArg<string> param_delimiter("d", "delimiter", "What goes between values in the output. Default: ', ', which outputs '0x55, 0xAA, ...'.", false, ", ", "delimiter");
    cmd.add(param_delimiter);

    ValueArg<int> param_base("b", "base", "Which numerical base to use in the output. Can be 10 (decimal) or 16 (hex). Default is hex.", false, 16, "10|16");
    cmd.add(param_base);
    
    ValueArg<char> param_byte_orientation("o", "byte-orientation", "Whether bytes are v (vertical) or h (horizontal) in the display's RAM. Default is vertical, which worked for SSD1306 and SH1106. ST7920 needs to use horizontal.", false, 'v', "v|h");
    cmd.add(param_byte_orientation);

    cmd.parse(argc, argv);


    // Read the image
    BMP image;
    if(!image.ReadFromFile(param_image_file.getValue().c_str())) {
        fprintf(stderr, "Error opening the image file.\n");
        return 1;
    }
    size_t width = image.TellWidth();
    size_t height = image.TellHeight();

    // Select the specified diterhing algorithm
    if(dither_matrices.find(param_dither_algorithm.getValue().c_str()) == dither_matrices.end()) {
        fprintf(stderr, "The dithering algorithm specified is not one of those available.\n");
        fprintf(stderr, "Available algorithms: jjn, floyd_steinberg, stucki, atikinson, burkes, sierra, sierra_2_row, sierra_lite.\n");
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
    //
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
    //         instead of the luminance itself.
    // 
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
            //
            for (const auto& spread : dither_matrix) {
                int x_offset,  y_offset;
                double proportion;
                tie(x_offset, y_offset, proportion) = spread;

                int position = width * (y + y_offset) + (x + x_offset);
                errors[position] += error * proportion;
            }
        }
    }

    // Convert the dithered image to a list of values, each representing
    // 8 pixels (1 byte of data).
    //
    // Byte arrangement: vertical bytes, LSB up
    //
    string delimiter = param_delimiter.getValue();
    string format_string;
    switch (param_base.getValue()) {
        case 10:
            format_string = "%d";
            break;
        case 16:
            format_string = "0x%02X";
            break;
        default:
            fprintf(stderr, "The base has to be either 10 or 16.\n");
            return 1;
    }
    format_string += delimiter;
    
    char byte_orientation = param_byte_orientation.getValue();
    if (byte_orientation == 'v') {
        for (size_t page = 0; page < height / 8; page++) {
            for (size_t col = 0; col < width; col++) {

                uint_fast8_t this_byte = 0x00;
                for (uint_fast8_t bit = 0; bit < 8; bit++) {
                    size_t position = width * (page * 8 + bit) + col;
                    if (image_greyscale[position]) {
                        this_byte |= 1 << bit;
                    }
                }

                printf(format_string.c_str(), this_byte);
            }
        }
    } else if (byte_orientation == 'h') {
        for (size_t row = 0; row < height; row++) {
            for (size_t byte = 0; byte < width / 8; byte++) {

                uint_fast8_t this_byte = 0x00;
                for (uint_fast8_t bit = 0; bit < 8; bit++) {
                    size_t position = width * row + byte * 8;
                    if (image_greyscale[position]) {
                        this_byte |= 1 << bit;
                    }
                }

                printf(format_string.c_str(), this_byte);
            }
        }
    } else {
        fprintf(stderr, "Byte orientation must be either v or h.\n");
        return 1;
    }

    printf("\n");

    return 0;
}
