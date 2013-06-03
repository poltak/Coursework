/*
 * smartrleplayer.c
 *
 *  	Date: 	26/04/2013
 *      Author: Jonathan Poltak Samosir
 *
 * Plays a DUMBRLEv1 file, providing support for frame delay, brightness, contrast and saturation
 * options.
 * All options should work!
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include "smartrleplayer.h"
#include "util.h"
#include "util-HSV.h"
#include "util-dumbrle.h"

static void process_file(FILE *, unsigned int, unsigned int, unsigned int, unsigned int);
static void set_display_levels(SDL_Surface *, double, double, double);

/* Handles processing of the DUMBRLEv1 file pointed to. Handles header data (dimensions) and
 * iterates through characters in the file to process frames. Once a frame is found (from the
 * character being START_OF_FRAME), the file pointer and control is passed to process_frame().
 * Also handles initialising and setting up of the SDL_Surface which will be used to display
 * images gotten from the dumbrle file.
 */
static void process_file(FILE *file, unsigned int frame_delay, unsigned int brightness,
        unsigned int contrast, unsigned int saturation)
{
    SDL_Surface *screen;
    int width, height;

    SDL_Init(SDL_INIT_VIDEO);
    get_dimension_properties(file, &width, &height);
    screen = SDL_SetVideoMode(width, height, BITS_PER_PIXEL, SDL_SWSURFACE);

    int c;
    while ((c = fgetc(file)) != END_OF_FRAMES)      // If final 'E' value found.
    {
        if (c == START_OF_FRAME)
        {
            process_frame(file, screen);
            // Set specified display options before displaying frame.
            set_display_levels(screen, (double) saturation/100.0, (double) brightness/100.0,
                    (double) contrast/100.0);
            SDL_Flip(screen);
            SDL_Delay(frame_delay);
        }
    }

    SDL_Delay(2000);    // Wait on last frame for 2 seconds before exiting.
    SDL_FreeSurface(screen);
    SDL_Quit();
}

/* Sets the display levels for each pixel on the SDL_Surface to calculated values.
 * Essentially works by converting each RGB pixel into it's HSV equivalent, then altering the
 * relevant HSV fields. After pixel is dealt with in HSV color model, it is converted back into
 * it's SDL 32 bit RGB format and placed back into the SDL_Surface's pixel vector.
 * fields.
 */
static void set_display_levels(SDL_Surface *screen, double saturation, double brightness,
        double contrast)
{
    int i, num_of_pixels = screen->w * screen->h;
    Uint32 *pixels = (Uint32 *) screen->pixels;
    struct hsv pixel_hsv;

    if (SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);
    for (i = 0; i < num_of_pixels; i++)
    {
        pixel_hsv = rgb_to_hsv(*(pixels + i), screen->format);  // Convert to HSV.

        // -0.5 on each of below values so to make 50 be the default "unchanged" arg value.
        pixel_hsv.s += (saturation - .5);                           // Set saturation.
        pixel_hsv.v += (brightness - .5);                           // Set brightness.
        pixel_hsv.v = (pixel_hsv.v - .5) * (contrast + .5) + .5;    // Calculate and set contrast.
        /* Contrast algorithm inspired by advice found online:
         * First a small algorithm based on how Photoshop calculates contrast in RGB colour space:
         * http://www.gamedev.net/topic/313181-adjust-contrast-rgb-image/#entry3004503
         *
         * And putting that together with advice for calculating contrast in HVL colour space:
         * http://stackoverflow.com/a/2976426/1973813
         *
         * I found some other, very much more complicated, algorithms (perhaps with better results),
         * but for the most part I was happy with how this one I put together works. Even works okay
         * (for the most part...) with different brightness settings, which share the same HSV
         * channel: value.
         */

        // Make sure no values have over/underflowed in range 0..1.
        if (pixel_hsv.v < 0)        pixel_hsv.v = 0;
        else if (pixel_hsv.v > 1)   pixel_hsv.v = 1;
        if (pixel_hsv.s < 0)        pixel_hsv.s = 0;
        else if (pixel_hsv.s > 1)   pixel_hsv.s = 1;

        *(pixels + i) = hsv_to_rgb(pixel_hsv, screen->format);  // Convert back to RGB.
    }
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
}

/* Provides basic arg checking the the way of arg count and checking for a valid DUMBRLEv1 file.
 * Also checks each supplementary arg to make sure they're non-negative.
 */
int main(int argc, char *argv[])
{
    if (argc != VALID_ARGS_LEN)
        print_and_die(NULL, USAGE_4);

    // Checks all supp. args to make sure they're non-negative.
    int i;
    int argstoi[] =
    {
        atoi(argv[2]),
        atoi(argv[3]),
        atoi(argv[4]),
        atoi(argv[5])
    };

    // Make sure supplementary args are within range.
    for (i = 0; i < VALID_SUPP_ARGS_LEN; i++)
        if (argstoi[i] < 0 || (argstoi[i] > 100 && i > 0))
            print_and_die(NULL, USAGE_4);

    // Get pointer to valid dumbrle file (and handle non dumbrle files).
    FILE *file = handle_file(argv[1]);

    // Decode and play file.
    process_file(file, argstoi[0], argstoi[1], argstoi[2], argstoi[3]);
    fclose(file);

    return EXIT_SUCCESS;
}
