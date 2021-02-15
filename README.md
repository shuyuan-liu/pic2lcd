# Pic2lcd

Converts images to bitmap data that can be fed to monochrome OLED / LCD panels driven by SSD1306, SH1106, ST7920 and the like. See below for configuration options available.

## Compiling

I use the [Tup](https://github.com/gittup/tup) build system because it’s simple but useful enough for this small project.

1. Install [Tup](https://github.com/gittup/tup).
2. Install the following headers:
    - [CLI11](https://github.com/CLIUtils/CLI11)
    - [png++](https://www.nongnu.org/pngpp/)
3. `git clone <this repo>`
4. `cd pic2lcd`
5. `tup`

## Command-line options

```
Usage: pic2lcd [OPTIONS] [image]

Positionals:
  image TEXT:FILE             Path to a png image to be converted

Options:
  -h,--help                   Print this help message and exit
  -i,--invert                 Invert the output (swap black and white)
  -v,--bytes-vertical         Treat every 8 pixels in a column, instead of in a row, as one byte
  -l,--lsb-first              Output bytes with LSB closer to the origin
  -c,--columns-first          Go up-to-down then left-to-right
  -d,--dither                 Perform dithering on the image before converting
```

## About dithering

I had initially removed dithering from pic2lcd because other programs like ImageMagick could already do the job. However, for the following reasons I have added back dithering:

- ImageMagick is slow at dithering. On my laptop, dithering a 256x128 image 100 times *and* saving the hex data takes pic2lcd 1.4s, while ImageMagick needs 3.4s just for dithering alone.
- ImageMagick by default adjusts the image's contrast automatically, which I don't want it to do.
- It's programming practice for me
