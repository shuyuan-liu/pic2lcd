# Pic2lcd

Converts images to bitmap data that can be fed to monochrome OLED / LCD panels driven by SSD1306, SH1106, ST7920 and the like. 

## Compiling

I use the [Tup](https://github.com/gittup/tup) build system because it’s nice and simple.

1. Install [Tup](https://github.com/gittup/tup).
2. Install the following headers:
    - [CLI11](https://github.com/CLIUtils/CLI11)
    - [fmt](https://github.com/fmtlib/fmt)
    - [png++](https://www.nongnu.org/pngpp/)
3. `git clone <this repo>`
4. `cd pic2lcd`
5. `tup`
