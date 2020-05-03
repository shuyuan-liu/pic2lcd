#pragma once

#include <tuple>
#include <vector>
#include <map>

using namespace std;

// These vectors of tuples defines how errors during dithering are pushed to
// neighbouring pixels. Errors are always pushed to pixels that have NOT been
// processed, and never to those that have (pushing the error to processed
// pixels makes no difference as we will not touch those pixels ever again.
//
// The following website lists several dithering algorithms 
// https://tannerhelland.com/2012/12/28/dithering-eleven-algorithms-source-code.html
// The default dithering algorithm used by pic2lcd is JJN (Jarvis, Judice, and
// Ninke), as follows:
//     _ _ X 7 5
//     3 5 7 5 3    (1/48)
//     1 3 5 3 1
// Where _ is a processed pixel, X is the pixel currently being processed,
// and the numbers represent how much of the error is pushed to a particular
// pixel. For instance 7/48 of the error is subtracted from the pixel
// immediately to the right of X.
//
// The format is {x_offset, y_offset, how_much}, where:
//     x_offset, y_offset: the coordinates of the pixel to distribute the error to,
//         relative to the current pixel
//     how_much: what fraction of the error is ditributed to the pixel specified by
//         x_offset and y_offset.

map<string, vector<tuple<unsigned int, unsigned int, double>>> dither_matrices = {

    {"jjn",{
        { 1, 0, 7 / 48.0},
        { 2, 0, 5 / 48.0},
        {-2, 1, 3 / 48.0},
        {-1, 1, 5 / 48.0},
        { 0, 1, 7 / 48.0},
        { 1, 1, 5 / 48.0},
        { 2, 1, 3 / 48.0},
        {-2, 2, 1 / 48.0},
        {-1, 2, 3 / 48.0},
        { 0, 2, 5 / 48.0},
        { 1, 2, 3 / 48.0},
        { 2, 2, 1 / 48.0},
    }},

    {"floyd_steinberg", {
        { 1, 0, 7 / 16.0},
        {-1, 1, 3 / 16.0},
        { 0, 1, 5 / 16.0},
        { 1, 1, 1 / 16.0},
    }},

    {"stucki", {
        { 1, 0, 8 / 42.0},
        { 2, 0, 4 / 42.0},
        {-2, 1, 2 / 42.0},
        {-1, 1, 4 / 42.0},
        { 0, 1, 8 / 42.0},
        { 1, 1, 4 / 42.0},
        { 2, 1, 2 / 42.0},
        {-2, 2, 1 / 42.0},
        {-1, 2, 2 / 42.0},
        { 0, 2, 4 / 42.0},
        { 1, 2, 2 / 42.0},
        { 2, 2, 1 / 42.0},
    }},

    {"atkinson", {
        { 1, 0, 1 / 8.0},
        { 2, 0, 1 / 8.0},
        {-1, 1, 1 / 8.0},
        { 0, 1, 1 / 8.0},
        { 1, 1, 1 / 8.0},
        { 0, 2, 1 / 8.0},
    }},

    {"burkes", {
        { 1, 0, 8 / 32.0},
        { 2, 0, 4 / 32.0},
        {-2, 1, 2 / 32.0},
        {-1, 1, 4 / 32.0},
        { 0, 1, 8 / 32.0},
        { 1, 1, 4 / 32.0},
        { 2, 1, 2 / 32.0},
    }},

    {"sierra", {
        { 1, 0, 5 / 32.0},
        { 2, 0, 3 / 32.0},
        {-2, 1, 2 / 32.0},
        {-1, 1, 4 / 32.0},
        { 0, 1, 5 / 32.0},
        { 1, 1, 4 / 32.0},
        { 2, 1, 2 / 32.0},
        {-1, 2, 2 / 32.0},
        { 0, 2, 3 / 32.0},
        { 1, 2, 2 / 32.0},
    }},

    {"sierra_2_row", {
        { 1, 0, 4 / 16.0},
        { 2, 0, 3 / 16.0},
        {-2, 1, 1 / 16.0},
        {-1, 1, 2 / 16.0},
        { 0, 1, 3 / 16.0},
        { 1, 1, 2 / 16.0},
        { 2, 1, 1 / 16.0},
    }},

    {"sierra_lite", {
        { 1, 0, 2 / 4.0},
        {-1, 1, 1 / 4.0},
        { 0, 1, 1 / 4.0},
    }}

};