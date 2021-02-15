#include <CLI/CLI.hpp>
#include <cstdio>
#include <png++/png.hpp>
#include <string>
#include <exception>
#include "reverse_byte.hpp"
#include "dither.hpp"


int main (int argc, char *argv[])
{
    // Set up command-line options
    CLI::App app {"Convert a png image to monochrome bitmap data for LCD / OLED controllers"};

    std::string image_path = "";
    app.add_option ("image", image_path, "Path to a png image to be converted") -> check (CLI::ExistingFile);

    std::string dithered_image_path = "";
    app.add_option ("dithered_image", dithered_image_path, "If given, save dithered image to the path specified");
    
    bool invert = false;
    app.add_flag ("-i, --invert", invert, "Invert output, i.e. swap black and white");
    
    bool bytes_vertical = false;
    app.add_flag ("-v, --bytes-vertical", bytes_vertical, "Group every 8 pixels in a column (instead of in a row) as a byte");
    
    bool lsb_first = false;
    app.add_flag ("-l, --lsb-first", lsb_first, "Output bytes with LSB towards the origin");

    bool columns_first = false;
    app.add_flag ("-c, --columns-first", columns_first, "Go column by column insetad of row by row");

    bool do_dither = false;
    app.add_flag ("-d, --dither", do_dither, "Perform dithering before converting");

    CLI11_PARSE(app, argc, argv);


    // Load image, quit on any error
    png::image<png::gray_pixel> image;
    try {
        image.read(image_path);
    } catch (const std::exception &e) {
        fprintf (stderr, "Couldn't load image: %s\n", e.what());
        return 1;
    }
    int width = image.get_width ();
    int height = image.get_height ();


    // Make sure image size fits a whole number of bytes, if not then quit
    if (bytes_vertical && height % 8 != 0) {
        fprintf (stderr, "The height of the image needs to be a multiple of 8\n");
        return 2;
    } else if (width % 8 != 0) {
        fprintf (stderr, "The width of the image needs to be a multiple of 8\n");
        return 2;
    }


    if (do_dither) {
        dither(image);

        if (dithered_image_path != "") {
            try {
                image.write(dithered_image_path);
            } catch (const std::exception &e) {
                fprintf (stderr, "Couldn't save dithered image: %s\n", e.what());
                return 3;
            }
        }
    }


    // Fill output bytes into an array in the sequence that they will be printed
    unsigned char *bytes = new unsigned char[width * height / 8];

    if (bytes_vertical) {
        // There are 'width' columns and 'height / 8' rows of bytes
        for (int row = 0; row < height / 8; row++) {
            for (int col = 0; col < width; col++) {

                int x = col;
                int y = row * 8;
                unsigned char byte = (image[y    ][x] ? 0b10000000 : 0)
                                   | (image[y + 1][x] ? 0b01000000 : 0)
                                   | (image[y + 2][x] ? 0b00100000 : 0)
                                   | (image[y + 3][x] ? 0b00010000 : 0)
                                   | (image[y + 4][x] ? 0b00001000 : 0)
                                   | (image[y + 5][x] ? 0b00000100 : 0)
                                   | (image[y + 6][x] ? 0b00000010 : 0)
                                   | (image[y + 7][x] ? 0b00000001 : 0);

                if (columns_first) {
                    bytes[col * (height / 8) + row] = byte;
                } else {
                    bytes[row * width + col] = byte;
                }
            }
        }
    } else {
        // There are 'width / 8' columns and 'height' rows of bytes
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width / 8; col++) {

                int x = col * 8;
                int y = row;
                unsigned char byte = (image[y][x    ] ? 0b10000000 : 0)
                                   | (image[y][x + 1] ? 0b01000000 : 0)
                                   | (image[y][x + 2] ? 0b00100000 : 0)
                                   | (image[y][x + 3] ? 0b00010000 : 0)
                                   | (image[y][x + 4] ? 0b00001000 : 0)
                                   | (image[y][x + 5] ? 0b00000100 : 0)
                                   | (image[y][x + 6] ? 0b00000010 : 0)
                                   | (image[y][x + 7] ? 0b00000001 : 0);

                if (columns_first) {
                    bytes[col * height + row] = byte;
                } else {
                    bytes[row * (width / 8) + col] = byte;
                }
            }
        }
    }


    // Apply the "LSB first" and "invert" options
    if (lsb_first) {
        for (int i = 0; i < width * height / 8; i++) {
            bytes[i] = reverse_byte(bytes[i]);
        }
    }
    if (invert) {
        for (int i = 0; i < width * height / 8; i++) {
            bytes[i] = ~bytes[i];
        }
    }


    // Print data
    for (int i = 0; i < width * height / 8; i++) {
        // Don't put a comma in front of the first value
        if (i != 0) {
            printf (", ");
        }
        printf ("0x%02X", bytes[i]);
    }
    printf ("\n");


    // Tidy up
    delete [] bytes;

    
    return 0;
}
