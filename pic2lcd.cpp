#include <cstdio>
#include <cmath>
#include <cstring>
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
    CmdLine cmd("pic2lcd: convert images into hex strings to display on monochrome LCD screens like those driven by SSD1306, ST7525, ST7920, etc.", ' ', "Beta " __TIME__ " " __DATE__);

    UnlabeledValueArg<string> param_image_file("image", "The image to be processed", true, "", "path_to_image");
    cmd.add(param_image_file);

    ValueArg<string> param_dither_algorithm("", "dither-alg", "The dithering algorithm to use. Available values are jjn, floyd_steinberg, stucki, atikinson, burkes, sierra, sierra_2_row, sierra_lite. The default value is jjn.", false, "jjn", "name_of_dither_alg");
    cmd.add(param_dither_algorithm);

    ValueArg<string> param_delimiter("", "delimiter", "A string separating values in the output. The default is \", \", which outputs \"0x55, 0xAA, ...\".", false, ", ", "delimiter");
    cmd.add(param_delimiter);

    SwitchArg param_decimal("", "decimal", "Use decimal values instead of hex.", false);
    cmd.add(param_decimal);

    cmd.parse(argc, argv);

    // Read the image 
    BMP image;
    if(!image.ReadFromFile(param_image_file.getValue().c_str())) {
        printf("The image file could not be read.\n");
        return 1;
    }
    unsigned int width = image.TellWidth();
    unsigned int height = image.TellHeight();

    // Select the specified diterhing algorithm
    if(dither_matrices.find(param_dither_algorithm.getValue().c_str()) == dither_matrices.end()) {
        printf("The dithering algorithm you specified is not one of those available.\n");
        return 1;
    }
    vector<tuple<unsigned int, unsigned int, double>> dither_matrix = dither_matrices[param_dither_algorithm.getValue().c_str()];

    // Convert the image into greyscale
    int *image_greyscale = new int[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image_greyscale[y * width + x] = image(x, y)->Red * 0.3 +
                                             image(x, y)->Green * 0.59 +
                                             image(x, y)->Blue * 0.11;
        }
    }

    // Dither the image to black-and-white
    int *errors = new int[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int position = y * width + x;
            int luminance = min(image_greyscale[position] - errors[position], 255);
            int error;

            if (luminance >= 127) {
				image_greyscale[position] = 255;
                error = 255 - luminance;
            } else {
				image_greyscale[position] = 0;
                error = 0 - luminance;
            }

            // Distribute the error according to the dither_matrix
            for (auto &spread : dither_matrix) {
                int x_offset,  y_offset;
                double how_much;
                tie(x_offset, y_offset, how_much) = spread;
				int position = (y + y_offset) * width + (x + x_offset);
                errors[position] += error * how_much;
            }
        }
    }

	// Convert the dithered image to a hex string
	// vertical bytes, LSB up
	string delimiter = param_delimiter.getValue();
    bool use_decimal = param_decimal.getValue();
    bool first = true;
	for (int page = 0; page < height / 8; page++) {
		for (int col = 0; col < width; col++) {

			unsigned char this_byte = 0x00;
			for (int bit = 0; bit < 8; bit++) {
				int position = width * (page * 8 + bit) + col;
                if (!image_greyscale[position]) {
					this_byte |= 0x01 << bit;
				}
			}

			if(!first) {
                printf ("%s", delimiter.c_str());
            }

            if(use_decimal) { 
                printf ("%d", this_byte);
            } else {
                printf ("0x%02X", this_byte);
            }
            
            first = false;
		}
	}

    printf("\n");

    return 0;
}
