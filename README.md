# pic2lcd

A small utility that converts images to data that can be displayed on monochrome OLED / LCD panels like those driven by SSD1306, SH1106 and ST7567.

## Features

- Performs dithering on the image. The default algorithm is Floyd-Steinberg, with a number of other options.
- Can output either hex values or decimal values. Python users that read the output with the *csv* library might find the decimal format easier to work with.
- Customisable value delimiter. The default is a comma plus a space, which produces `0x95, 0x40, ...`. You can change this with the `--delimiter` option to produce `0x95,0x40,...`, `0x95 0x40 ...` etc.

## Compiling

1. Install the *tclap* library headers. On Arch Linux use `sudo pacman -S tclap`. For other distributions search for the corresponding package and install.
2. Clone this repo.
3. `cd pic2lcd`
4. `c++ pic2lcd.cpp EasyBMP/EasyBMP.cpp -o pic2lcd` (sorry no Makefile yet, will add in the future)
