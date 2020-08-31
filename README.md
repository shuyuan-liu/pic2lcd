**ABOUT TO BE DEPRECATED**

A rewrite of this program in C is being worked on and this repo will be archived once the rewrite is ready for use.

# pic2lcd

A small utility that converts images to data that can be displayed on monochrome OLED / LCD panels like those driven by SSD1306, SH1106 and ST7567.

## Features

- Performs dithering on the image. The default algorithm is Floyd-Steinberg, with a number of other options.
- Can output either hex values or decimal values. Python users that read the output with the *csv* library might find the decimal format easier to work with.
- Customisable value delimiter. The default is a comma plus a space, which produces `0x95, 0x40, ...`. You can change this with the `--delimiter` option to produce `0x95,0x40,...`, `0x95 0x40 ...` etc.

## Compiling

Pic2lcd has only been compiled on Linux, but it should be compatible with Windows build environments with the proper headers set up.

### Prerequisites



1. Install the *tclap* library headers. On Arch Linux use `sudo pacman -S tclap`. For other distributions search for the corresponding package and install.
2. Clone this repo.
3. `cd pic2lcd`
4. `c++ pic2lcd.cpp EasyBMP/EasyBMP.cpp -o pic2lcd` (sorry no Makefile yet, will add in the future)

## Options

| Option                                       | Description                                                  |
| -------------------------------------------- | ------------------------------------------------------------ |
| `-o <v/h>, --byte-orientation <v/h>`         | Whether bytes are v (vertical) or h (horizontal) in the display's RAM. Default is vertical, which works for SSD1306 and SH1106. ST7920 needs to use horizontal. |
| `-b <10/16>, --base <10/16>`                 | Which numerical base to use in the output. Can be 10 (decimal) or 16 (hex). Default is hex. |
| `-d <delimiter>, --delimiter <delimiter>`    | What goes between values in the output. Default: ', ', which outputs '0x55, 0xAA, ...'. |
| `-a <dither_alg>, --dither-alg <dither_alg>` | Select the dithering algorithm to dither the image with. Default: `floyd_steinberg`. Available: `jjn`, `floyd_steinberg`, `stucki`, `atikinson`, `burkes`, `sierra`, `sierra_2_row`, `sierra_lite`. |

