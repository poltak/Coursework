/*
 * framefirstview.c
 *
 *  	Date: 	24/04/2013
 *      Author: Jonathan Poltak Samosir
 *      ID:		2271 3603
 *
 * Displays the first frame of a specified DUMBRLEv1 file for specified amount of seconds.
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include "firstframeview.h"
#include "util.h"
#include "util-dumbrle.h"

static void process_file(FILE *, unsigned int);

/* Handles processing of the DUMBRLEv1 file pointed to. Handles header data (dimensions) and
 * iterates through characters in the file to process frames. Once a frame is found (from the
 * character being START_OF_FRAME), the file pointer and control is passed to process_frame().
 * Also handles initialising and setting up of the SDL_Surface which will be used to display
 * images gotten from the dumbrle file.
 * As per the assignment specifications, only the first frame will be processed by this function.
 */
static void process_file(FILE *file, unsigned int display_seconds)
{
	SDL_Surface *screen;
	int width, height;

	SDL_Init(SDL_INIT_VIDEO);
	get_dimension_properties(file, &width, &height);
	screen = SDL_SetVideoMode(width, height, BITS_PER_PIXEL, SDL_SWSURFACE);

	int c;
	while ((c = fgetc(file)) != END_OF_FRAMES)		// If final 'E' value found.
	{
		if (c == START_OF_FRAME)
		{
			process_frame(file, screen);
			SDL_Flip(screen);
			break;              // Don't process anymore frames after first frame.
		}
	}

	SDL_Delay(display_seconds * 1000);      // Delay for specified amount of time before closing.
	SDL_FreeSurface(screen);
	SDL_Quit();
}

/* Provides basic arg checking in the way of arg count and checking for a valid DUMBRLEv1 file.
 * Passes control to process_file() function which does most of the file work.
 */
int main(int argc, char *argv[])
{
    if (argc != VALID_ARGS_LEN)
        print_and_die(NULL, USAGE_2);

    int display_seconds = atoi(argv[2]);
    if (display_seconds < 0)
        print_and_die(NULL, USAGE_2);

    // Get pointer to valid dumbrle file (and handle non dumbrle files).
    FILE *file = handle_file(argv[1]);

    // Decode and display first frame of file.
    process_file(file, display_seconds);
    fclose(file);

    return EXIT_SUCCESS;
}
