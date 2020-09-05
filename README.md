# Pic2lcd

A small utility that converts images to data that can be displayed on monochrome OLED / LCD panels like those driven by SSD1306, SH1106 and ST7567.

## Features

- Performs dithering on the image. The default algorithm is Jarvis, Judice, and Ninke (jjn), with a number of other options.
- Can output either hex values or decimal values. Python users that read the output with the *csv* library might find the decimal format easier to work with.
- Customisable value delimiter. The default is a comma plus a space, which produces `0x95, 0x40, ...`. You can change this with the `--delimiter` option.
- Can provide data for screens with both vertical bytes in memory (e.g. SSD1306) and horizontal bytes (e.g. ST7920).
- Bit order (LSB / MSB) can be changed

## Compiling

Pic2lcd has only been compiled on Linux, but it should be compatible with Windows build environments with the proper headers and compiler set up.

### Prerequisites

1. Install the *tclap* and *png++* library headers. This should be available from your distro's repositories.
2. Clone this repo.
3. `cd pic2lcd`
4. `make`

## Options



