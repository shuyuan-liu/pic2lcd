#include <cstdio>
#include <cstdint>
#include <string>
#include <tclap/CmdLine.h>
#include <png++/png.hpp>

#include "strings.h"
#include "dither_algorithms.h"

using namespace std;

void print_value (uint8_t value, bool inverse, bool decimal) {
	if (inverse) {
		value = ~value;
	}

	if (decimal) {
		printf ("%d", value);
	} else {
		printf ("0x%02X", value);
	}
}


int main (int argc, char* argv[])
{
	// Specify and parse command line options
	TCLAP::CmdLine cmd (pic2lcd::DESCRIPTION, pic2lcd::OPTION_VALUE_DELIMITER, pic2lcd::VERSION);
	TCLAP::SwitchArg arg_horizontal_bytes ("b", "horizonal-bytes", "Make bytes horizontal as in ST7920; default is vertical as in SSD1306", cmd, false);
	TCLAP::SwitchArg arg_inverse ("i", "inverse", "Treat light pixels as 0 and dark pixels as 1", cmd, false);
	TCLAP::SwitchArg arg_decimal ("d", "decimal", "Output decimal values instead of hexadecimal", cmd, false);
	TCLAP::SwitchArg arg_lsb ("l", "lsb", "Place LSB towards the origin when outputing data", cmd, false);
	TCLAP::ValueArg<string> arg_delimiter ("s", "delimiter", "Set the string that separates each value", false, ", ", "string", cmd);
	TCLAP::ValueArg<int> arg_threshold ("t", "threshold", "The brightness value above which a pixel will be regarded as white", false, 127, "int", cmd);
	TCLAP::ValueArg<string> arg_algorithm ("a", "algorithm", "The dithering algorithm to use.", false, "jjn", "string", cmd);
	TCLAP::UnlabeledValueArg<string> arg_image_file ("image-file", "Path to the image file to be processed", true, "", "image_file", cmd);
	TCLAP::SwitchArg arg_save_dithered ("r", "save-dithered", "Save the dithered image as dithered.png", cmd, false);
	cmd.parse (argc, argv);

	// Extract options into varaibles for easier use later
	auto horizontal_bytes = arg_horizontal_bytes.getValue();
	auto inverse = arg_inverse.getValue();
	auto decimal = arg_decimal.getValue();
	auto lsb = arg_lsb.getValue();
	auto delimiter = arg_delimiter.getValue();
	auto threshold = arg_threshold.getValue();
	auto algorithm = arg_algorithm.getValue();
	auto image_file = arg_image_file.getValue();
	auto save_dithered = arg_save_dithered.getValue();


	// Load the image
	//
	// We specify the numbers of channels as 1 so that the image will be
	// converted to greyscale on load
	//
	// depth: Unused, just required by the load function
	//
	png::image<png::gray_pixel> image (image_file);
	int width = image.get_width();
	int height = image.get_height();

	// The width or height of the image must be multiples of 8, depending on
	// the byte orientation selected
	//
	if (horizontal_bytes) {
		if (width % 8 != 0) {
			fprintf (stderr, pic2lcd::ERR_WIDTH_NOT_MULTIPLE_OF_8);
			exit (2);
		}
	} else {
		if (height % 8 != 0) {
			fprintf (stderr, pic2lcd::ERR_HEIGHT_NOT_MULTIPLE_OF_8);
			exit (2);
		}
	}


	// Load the specified dithering algorithm
	//
	if (dither_algorithms.find(algorithm) == dither_algorithms.end()) {
		fprintf (stderr, pic2lcd::ERR_DITHER_ALGORITHM);
		exit (1);
	}
	auto dither_matrix = dither_algorithms[algorithm];


	// Dither the image to black-and-white
	//
	// errors: Stores the error value (produced when processing neibouring
	//         pixels) to be taken into account for each pixel. Has the same
	//         size as the image.
	//
	double* errors = new double[width * height]();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			
			// We need to subtract the error spread to this pixel when
			// considering its brightness, to compensate for its neighbouring
			// pixels being "forced" to 0 or 255 from their original colours
			//
			// The reason for subtraction instead of addition is that an error
			// needs to be compensated by adjusting other pixels in the
			// opposite direction. For example, when a pixel is forced from 200
			// to 255, there is a positive error because 255 is brighter than
			// its real value. By subtracting from (darkening) its neighbouring
			// pixels, this error can be compensated for.
			//
			int pos = y * width + x;
			double value = image[y][x] - errors[pos];
			double err;

			// !!! The brightness value used when calculating the error is the
			// brightness value AFTER errors have been added, not before !!!
			//
			if (value > threshold) {
				image[y][x] = 255;
				err = 255 - value;
			} else {
				image[y][x] = 0;
				err = 0 - value;
			}

			// Spread this pixel's error to neighbouring pixels
			//
			// dither_matrix_element: An element in the dithering matrix
			//                        specifying one neighbouring pixel that
			//                        the error should be spread to, and how
			//                        much
			//
			for (auto dither_matrix_element : dither_matrix) {
				int dx, dy;
				double how_much;
				tie(dx, dy, how_much) = dither_matrix_element;

				// !!! Make sure our target pixel is still inside the image
				// otherwise we get sigmentation faults
				//
				if (x + dx >= 0 &&
					y + dy >= 0 &&
					x + dx < width &&
					y + dy < height) 
				{
					int pos = (y + dy) * width + (x + dx);
					errors[pos] += err * how_much;
				}
			}
		}
	}

	if (save_dithered) {
		image.write ("dithered.png");
	}


	// Output the processed image data
	//
	if (horizontal_bytes) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x += 8) {
				
				unsigned char this_byte = 0;
				for (int bit = 0; bit < 8; bit++) {
					int pos = y * width + x + bit;
					if (image[y][x+bit]) {
						this_byte |= (lsb ? (1 << bit) : (0x80 >> bit));
					}
				}

				// Print a delimiter except for the first byte
				if (x || y) {
					printf ("%s", delimiter.c_str());
				}

				print_value (this_byte, inverse, decimal);
			}
		}
	} else {
		for (int y = 0; y < height; y += 8) {
			for (int x = 0; x < width; x++) {
				
				unsigned char this_byte = 0;
				for (int bit = 0; bit < 8; bit++) {
					int pos = (y + bit) * width + x;
					if (image[y+bit][x]) {
						this_byte |= (lsb ? (1 << bit) : (0x80 >> bit));
					}
				}

				// Print a delimiter except for the first byte
				if (x || y) {
					printf ("%s", delimiter.c_str());
				}

				print_value (this_byte, inverse, decimal);
			}
		}
	}

	printf ("\n");

	exit (0);
}

