/*
 * util-dumbrle.c
 *
 *  	Date: 	24/04/2013
 *      Author: Jonathan Poltak Samosir
 *
 * Contains utility functions for processing DUMBRLEv1 file frames and channels for display with
 * an SDL_Surface.
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include "util.h"
#include "util-SDL.h"
#include "util-dumbrle.h"

/* Sets up three main channels (RGB) and attempts to use them to store the pixel values from the
 * dumbrle file. This channel processing is done in function process_channel(). After each channel
 * has been set up with proper pixel values, they are mapped into the main 32 bit channel that makes
 * up the actual pixel values for the SDL_Surface.
 */
void process_frame(FILE *file, SDL_Surface *screen)
{
	int i;
	unsigned int num_of_pixels = screen->w * screen->h;
	unsigned int pixel_counter = 0;
	Uint32 *master_channel = (Uint32 *) screen->pixels;
	Uint8 **channels = alloc_channels(3, num_of_pixels);
	Channel_t channel;

	for (channel = RED; channel <= BLUE; channel++)	// Process each channel (RGB).
	{
		while (pixel_counter < num_of_pixels)
			pixel_counter += process_channel(file, channels[channel], pixel_counter);
		pixel_counter = 0;	// Reset pixel_counter for next channel.
	}

	// Map each of the 8 bit R,G,B channel values to the 32 bit RGB master_channel.
    if (SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);
	for (i = 0; i < num_of_pixels; i++)
	{
		master_channel[i] = SDL_MapRGB(screen->format, channels[RED][i], channels[GREEN][i],
				channels[BLUE][i]);
	}
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);

	free_channels(channels, 3);
}

/* Processes individual channel passed which a pointer to is passed in. It does this through applying
 * the Packbits Variant algorithm on the file to decode the contents.
 * offset arg specifies the amount of pixels in channel to start setting from (so we don't
 * overwrite previously set pixels).
 * Returns the number of pixels processed in this call.
 *
 * NOTE: No need to lock surface as we're not messing with the actual pixels in the surface yet.
 */
int process_channel(FILE *file, Uint8 *channel, unsigned int offset)
{
	int header, i;
	unsigned char colour;

    // Cast to char to account for negative values (8 bit two's complement).
	header = (char) fgetc(file);

	if (header < 0)
	{	// Repeat next colour 2-header times and put into next 2-header pixels.
		header = 2 - header;
		colour = fgetc(file);

		for (i = offset; i < header + offset; i++)
		{
			*(channel +i) = colour;
		}
	} else
	{	// Set next header+1 pixels to the next header+1 colour values.
		header++;
		for (i = offset; i < header + offset; i++)
		{
			colour = fgetc(file);
			*(channel+i) = colour;
		}
	}

	return header;		// The amount of pixels dealt with in this call.
}
