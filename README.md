# Pic2lcd

Converts images to bitmap data that can be fed to monochrome OLED / LCD panels driven by SSD1306, SH1106, ST7920 and the like. 

## Compiling

I use the [Tup](https://github.com/gittup/tup) build system because it’s simple but useful enough for this small project.

1. Install [Tup](https://github.com/gittup/tup).
2. Install the following headers:
    - [CLI11](https://github.com/CLIUtils/CLI11)
    - [fmt](https://github.com/fmtlib/fmt)
    - [png++](https://www.nongnu.org/pngpp/)
3. `git clone <this repo>`
4. `cd pic2lcd`
5. `tup`

## Command-line options

```
Usage: pic2lcd [OPTIONS] [image]

Positionals:
  image                        Path to a png image to be converted

Options:
  -h, --help                   Print this help message and exit
  -i, --invert BOOLEAN         Invert the output (swap black and white)
  -v, --bytes-vertical BOOLEAN Treat each 8 vertically adjacent pixels as one byte
  -l, --lsb-first BOOLEAN      Output bytes with LSB closer to the origin
  -c, --columns-first BOOLEAN  Go up-to-down then left-to-right
```

“BOOLEAN” means either `true` or `false`.

