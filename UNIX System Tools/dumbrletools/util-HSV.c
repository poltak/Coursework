/*
 * util-HSV.c
 *
 *  	Date: 	27/04/2013
 *      ID:		2271 3603
 *
 * Extensively modifies algorithms originally written by Jef Poskanzer for libppm functions:
 *
 * pixel ppm_color_from_hsv(struct hsv const hsv,
 *                    pixval     const maxval);
 *
 * struct hsv ppm_hsv_from_color(pixel const color,
 *                    pixval const maxval);
 *
 * I have re-used the underlying algorithms for the functions to allow them to convert from an SDL
 * 32 bit RGB pixel (as opposed to the original libppm pixel type) to an HSV pixel and vice-versa.
 * The original unaltered copyright permission notice allowing me to modify this code appears below.
 *
 * Apart from the underlying algorithms for these functions, all code and comments are my own.
 */

/*
** Copyright (C) 1989 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <math.h>
#include <SDL/SDL.h>
#include "util.h"
#include "util-HSV.h"

/* Takes in SDL 32 bit RGB pixel and extracts each 8 bit channel. From these channels, performs
 * relevant comparisons to get which hue sector we are in (specifies Value), then attempts to find
 * the Saturation level by comparing to epsilon. Finally the hue degree is found from getting each
 * channel's chrominace and performing calculations depending on which hue sector we are calculated
 * to be in.
 */
struct hsv rgb_to_hsv(Uint32 pixel, SDL_PixelFormat *format)
{
    struct hsv hsv;
    double epsilon = 1e-5;
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);

    double R = (double) r / PIXEL_MAXVAL;
    double G = (double) g / PIXEL_MAXVAL;
    double B = (double) b / PIXEL_MAXVAL;

    enum hue_sector {SECTOR_RED, SECTOR_GRN, SECTOR_BLU};
    enum hue_sector hue_sector;

    if (R >= G)
    {
        if (R >= B)
        {
            hue_sector = SECTOR_RED;
            hsv.v = R;
        }
        else
        {
            hue_sector = SECTOR_BLU;
            hsv.v = B;
        }
    }
    else
    {
        if (G >= B)
        {
            hue_sector = SECTOR_GRN;
            hsv.v = G;
        }
        else
        {
            hue_sector = SECTOR_BLU;
            hsv.v = B;
        }
    }

    double range = hsv.v - fmin(R, fmin(G, B));

    if (hsv.v < epsilon)
        hsv.s = 0.0;
    else
        hsv.s = range / hsv.v;

    if (range < epsilon)
        hsv.h = 0.0;        // It's gray so arbitrarily pick 0
    else
    {
        double cr = (hsv.v - R) / range;
        double cg = (hsv.v - G) / range;
        double cb = (hsv.v - B) / range;

        double angle;   // Hue angle (-30 to +330)

        switch (hue_sector)
        {
            case SECTOR_RED:
                angle = 0.0 + 60.0 * (cb - cg);
                break;
            case SECTOR_GRN:
                angle = 120.0 + 60.0 * (cr - cb);
                break;
            case SECTOR_BLU:
                angle = 240.0 + 60.0 * (cg - cr);
                break;
        }
        hsv.h = angle >= 0.0 ? angle : 360 + angle;
    }

    return hsv;
}

/* Given pixel in HSV colour space, returns an SDL 32 bit RGB pixel. This is calculated by finding
 * which hue sector our colour resides in, and getting a fraction f of the way our colour is going
 * from one side of the sector to the other, in a clockwise direction. Depending on the sector we
 * are in, we set the RGB values to calculated m, n, k or the HSV Value.
 * Finally, we use SDL_MapRGB to map the calculated R G and B values (cast into 8 bit ints from
 * doubles, by multiplying by their maxvals) and return a 32 bit RGB pixel.
 */
Uint32 hsv_to_rgb(struct hsv pixel, SDL_PixelFormat *format)
{
    double R, G, B;

    if (pixel.s == 0)
        R = G = B = pixel.v;
    else
    {
        // Colour wheel divided into six 60 degree sectors.
        unsigned int sector_size = 60;
        // Sector in which our colour resides. (0..5)
        unsigned int sector = (pixel.h / sector_size);
        // Fraction of the way the colour is from each side of sector going clockwise. (0..1)
        double f = (pixel.h - sector * sector_size)/60;

        double m = (pixel.v * (1 - pixel.s));
        double n = (pixel.v * (1 - (pixel.s * f)));
        double k = (pixel.v * (1 - (pixel.s * (1 - f))));

        switch (sector)
        {
            case 0:
                R = pixel.v;
                G = k;
                B = m;
                break;
            case 1:
                R = n;
                G = pixel.v;
                B = m;
                break;
            case 2:
                R = m;
                G = pixel.v;
                B = k;
                break;
            case 3:
                R = m;
                G = n;
                B = pixel.v;
                break;
            case 4:
                R = k;
                G = m;
                B = pixel.v;
                break;
            case 5:
                R = pixel.v;
                G = m;
                B = n;
                break;
            default:
                print_and_die("Invalid H value passed to hsv_to_rgb", BAD_HSV);
        }
    }

    return SDL_MapRGB(format, (Uint8) (R * PIXEL_MAXVAL), (Uint8) (G * PIXEL_MAXVAL), (Uint8) (B * PIXEL_MAXVAL));
}
