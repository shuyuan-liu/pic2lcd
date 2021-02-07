#include <CLI/CLI.hpp>
#include <fmt/core.h>
#include <png++/png.hpp>
#include <string>
#include <exception>
#include "reverse_byte.hpp"

int main (int argc, char *argv[])
{
    // Set up command-line options
    CLI::App app {"Convert an image to monochrome bitmap data"};

    std::string image_file = "";
    app.add_option ("image", image_file, "Path to the image to be converted (png only)") -> check (CLI::ExistingFile);
    
    bool invert = false;
    app.add_option ("-i, --invert", invert, "Invert the output (swap black and white)");
    
    bool bytes_vertical = false;
    app.add_option ("-v, --bytes-vertical", bytes_vertical, "Treat every 8 pixels in a column (instead of in a row) as one byte");
    
    bool lsb_first = false;
    app.add_option ("-l, --lsb-first", lsb_first, "Output bytes with LSB closer to the origin");

    bool columns_first = false;
    app.add_option ("-c, --columns-first", columns_first, "Go up-to-down then left-to-right");

    CLI11_PARSE(app, argc, argv);


    // Load image, quit on any error
    png::image<png::gray_pixel> image;
    try {
        image.read(image_file);
    } catch (const std::exception &e) {
        fmt::print(stderr, "Failed to open image file: {}\n", e.what());
        return 1;
    }
    int width = image.get_width ();
    int height = image.get_height ();


    // Make sure image size fits a whole number of bytes, if not then quit
    if (bytes_vertical && height % 8 != 0) {
        fmt::print (stderr, "The height of the image must be a multiple of 8\n");
        return 2;
    } else if (width % 8 != 0) {
        fmt::print (stderr, "The width of the image must be a multiple of 8\n");
        return 2;
    }


    // Fill output bytes into an array in the sequence that they will be output
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


    // Output data
    for (int i = 0; i < width * height / 8; i++) {
        // Don't put a comma in front of the first value
        if (i != 0) {
            fmt::print (", ");
        }
        fmt::print ("{:#04x}", bytes[i]);
    }
    fmt::print ("\n");


    // Tidy up
    delete [] bytes;

    
    return 0;
}
