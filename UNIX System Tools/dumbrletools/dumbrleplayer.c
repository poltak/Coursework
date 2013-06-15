/*
 * dumbrleplayer.c
 *
 *  	Date: 	24/04/2013
 *      Author: Jonathan Poltak Samosir
 *
 * Plays a DUMBRLEv1 file, providing options for specifying a frame delay.
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include "dumbrleplayer.h"
#include "util.h"
#include "util-dumbrle.h"

static void process_file(FILE *, unsigned int);

/* Handles processing of the DUMBRLEv1 file pointed to. Handles header data (dimensions) and
 * iterates through characters in the file to process frames. Once a frame is found (from the
 * character being START_OF_FRAME), the file pointer and control is passed to process_frame().
 * Also handles initialising and setting up of the SDL_Surface which will be used to display
 * images gotten from the dumbrle file.
 */
static void process_file(FILE *file, unsigned int frame_delay)
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
			SDL_Delay(frame_delay);     // Delay for specified amount of time.
		}
	}

	SDL_Delay(2000);			// Waits on last frame for 2 seconds before exiting.
	SDL_FreeSurface(screen);
	SDL_Quit();
}

/* Provides basic arg checking in the way of arg count and checking for a valid DUMBRLEv1 file.
 * Also checks frame delay arg to make sure it's non-negative.
 */
int main(int argc, char *argv[])
{
    if (argc != VALID_ARGS_LEN)
        print_and_die(NULL, USAGE_3);

    int frame_delay = atoi(argv[2]);
    if (frame_delay < 0)
    	print_and_die(NULL, USAGE_3);

    // Get pointer to valid dumbrle file (and handle non dumbrle files).
    FILE *file = handle_file(argv[1]);

    // Decode and play file.
    process_file(file, frame_delay);
    fclose(file);

    return EXIT_SUCCESS;
}
