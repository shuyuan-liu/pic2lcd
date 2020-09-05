#pragma once

namespace pic2lcd {
	const char DESCRIPTION[] = "A small utility for converting images to raw data compatible with simple LCD controllers, e.g. ST7920, SSD1306.";
	const char OPTION_VALUE_DELIMITER = ' ';
	const char VERSION[] = "2.0";
	const char ERR_DITHER_ALGORITHM[] = "The dithering algorithm specified is not available.\n";
	const char ERR_CANT_OPEN_IMAGE[] = "Failed to open the input image.\n";
	const char ERR_WIDTH_NOT_MULTIPLE_OF_8[] = "The width of the image has to be a multiple of 8.\n";
	const char ERR_HEIGHT_NOT_MULTIPLE_OF_8[] = "The height of the image has to be a multiple of 8.\n";
}
