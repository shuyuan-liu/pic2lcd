#ifndef _DITHER_HPP_
#define _DITHER_HPP_

#include <png++/png.hpp>
#include <map>
#include <tuple>


// 
const std::map<std::pair<int, int>, double> dither_matrix = {
    {{ 1,  0}, (double) 5 / 32},
    {{ 2,  0}, (double) 3 / 32},
    {{-2,  1}, (double) 2 / 32},
    {{-1,  1}, (double) 4 / 32},
    {{ 0,  1}, (double) 5 / 32},
    {{ 1,  1}, (double) 4 / 32},
    {{ 2,  1}, (double) 2 / 32},
    {{-1,  2}, (double) 2 / 32},
    {{ 0,  2}, (double) 3 / 32},
    {{ 1,  2}, (double) 2 / 32},
};


void dither (png::image<png::gray_pixel> &image);


#endif