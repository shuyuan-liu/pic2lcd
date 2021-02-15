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
Usage: pic2lcd [OPTIONS] [image] [dithered_image]

Positionals:
  image TEXT:FILE             Path to a png image to be converted
  dithered_image TEXT         If given, save dithered image to the path specified

Options:
  -h,--help                   Print this help message and exit
  -i,--invert                 Invert output, i.e. swap black and white
  -v,--bytes-vertical         Group every 8 pixels in a column (instead of in a row) as a byte
  -l,--lsb-first              Output bytes with LSB towards the origin
  -c,--columns-first          Go column by column insetad of row by row
  -d,--dither                 Perform dithering before converting
```

## About dithering

I had initially removed dithering from pic2lcd because programs like ImageMagick could already do the job. However, for the following reasons I have now added back dithering:

- ImageMagick is slow for dithering. On my laptop, dithering a 256x128 image 100 times *and* saving the hex output takes pic2lcd only 1.4s, while ImageMagick needs 3.4s just for dithering alone.
- ImageMagick automatically adjusts the image's contrast, which I don't want it to do.
- It's programming practice for me.
