# pic2lcd

A small utility that converts images to data that can be displayed on monochrome OLED / LCD panels like those driven by SSD1306, SH1106 and ST7567.

## Features

- Performs dithering on the image. The default algorithm is Jarvis, Judice, and Ninke (jjn), with a number of other options.
- Can output either hex values or decimal values. Python users that read the output with the *csv* library might find the decimal format easier to work with.
- Customisable value delimiter. The default is a comma plus a space, which produces `0x95, 0x40, ...`. You can change this with the `--delimiter` option.

## Compiling

Pic2lcd has only been compiled on Linux, but it should be compatible with Windows build environments with the proper headers set up.

### Prerequisites

1. Install the *tclap* library headers. This should be available from your distro's repositories.
2. Clone this repo.
3. `cd pic2lcd`
4. `c++ pic2lcd.cpp`

## Options



