#ifndef _DITHER_HPP_
#define _DITHER_HPP_

#include <png++/png.hpp>
#include <map>
#include <tuple>


// Using the "JJN" dithering matrix. Source:
// https://tannerhelland.com/2012/12/28/dithering-eleven-algorithms-source-code.html
const std::map<std::pair<int, int>, double> dither_matrix = {
    {{ 1,  0}, (double) 7 / 48},
    {{ 2,  0}, (double) 5 / 48},
    {{-2,  1}, (double) 3 / 48},
    {{-1,  1}, (double) 5 / 48},
    {{ 0,  1}, (double) 7 / 48},
    {{ 1,  1}, (double) 5 / 48},
    {{ 2,  1}, (double) 3 / 48},
    {{-2,  2}, (double) 1 / 48},
    {{-1,  2}, (double) 3 / 48},
    {{ 0,  2}, (double) 5 / 48},
    {{ 1,  2}, (double) 3 / 48},
    {{ 2,  2}, (double) 1 / 48},
};


void dither (png::image<png::gray_pixel> &image);


#endif