/*
 * util-HSV.h
 *
 *  	Date: 	27/04/2013
 *      Author: Jonathan Poltak Samosir
 *
 * Extensively modifies algorithms originally written by Jef Poskanzer for libppm functions.
 * More details and permission notice in util-HSV.c.
 */

struct hsv
{
    double h;   // hue (degrees) (0..360)
    double s;   // saturation (0..1)
    double v;   // value (0..1)
};

struct hsv rgb_to_hsv(Uint32, SDL_PixelFormat *);
Uint32 hsv_to_rgb(struct hsv, SDL_PixelFormat *);
