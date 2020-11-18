# Pic2lcd

A small utility that converts images to data that can be displayed on monochrome OLED / LCD panels like those driven by SSD1306, SH1106 and ST7920. Currently, this program can only take png images, and is compatible with only monochrome displays.

## Features

- Performs dithering on the image. The default algorithm is Jarvis, Judice, and Ninke (jjn), with a number of other options.
- Outputs either hex values or decimal values. Python users that read the output with the *csv* library might find the decimal format easier to work with.
- Customisable value delimiter. The default is a comma plus a space, which produces `0x95, 0x40, ...`. You can change this with the `--delimiter` option.
- Compatible with screens with both vertical bytes in memory (e.g. SSD1306) and horizontal bytes (e.g. ST7920).
- Bit order (LSB / MSB) can be changed.

## Compiling

Pic2lcd has only been compiled on Linux, but it should be compatible with other build environments given the proper headers and compiler are set up.

1. Install the *tclap* and *png++* library headers. This should be available from your distro's repositories.
2. Clone this repo: `git clone https://gitee.com/lsquared618/pic2lcd.git`
3. `cd pic2lcd`
4. `make`

In case you want to remake, use `make remake`.

## Options

### Dithering options

- `-a, --algorithm`: Select a dithering algorithm to use. Valid values are `jjn`, `floyd_steinberg`, `stucki`, `atkinson`, `burkes`, `sierra`, `sierra_2_row`, and `sierra_lite`. You can see the algorithms in the file *dither_algorithms.h*.
- `-t, --threshold`: Set the value above which a pixel should be considered white when dithering. The value of the pixel’s brightness minus the errors distributed to it will be compared with the threshold, and the pixel will be white if that value is above the threshold.

### Data output options

- `-s, --delimiter`: Set the string that separates each value when output. Default is a comma and a space, i.e. `", "`.
- `-l, --lsb`: Output values LSB first. Useful for screens where the LSB of a byte is oriented towards the origin.
- `-d, --decimal`: Output decimal values instead of hex.
- `-i, --inverse`: Swap 1s and 0s so that a bright pixel is a 0 and a dark pixel is a 1. Useful for screens where pixels are dark when turned on.
- `-b, --horizontal-bytes`: Output every 8 pixels in a row as a byte, instead of every 8 pixels in a column. FYI, SSD1306 uses vertical bytes and ST7920 uses horizontal bytes.

### Other options

- `-r, --save-dithered`: Save the dithered image as *dithered.png*.
- `--version` and `-h, --help`: They do what they say.