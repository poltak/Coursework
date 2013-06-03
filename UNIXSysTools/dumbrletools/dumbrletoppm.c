/*
 * dumbrletoppm.c
 *
 *  	Date: 	24/04/2013
 *      Author: Jonathan Poltak Samosir
 *      ID:		2271 3603
 *
 * Converts DUMBRLEv1 files into number of PPM files for each frame contained in the dumbrle.
 */

#include <stdio.h>
#include <string.h>		// Used for specifying the PPM output file names.
#include <ppm.h>
#include "util.h"
#include "dumbrletoppm.h"

// All static function prototypes.
static void process_file(FILE *, char *);
static void process_frame(FILE *, PPMMeta_t *, char *);
static int process_channel(FILE *, pixel **, Channel_t, unsigned int);
static void set_pixval(pixel *, pixval, Channel_t);
static PPMMeta_t *init_ppmmeta(int, int, pixval);
static char *get_filename(char *, unsigned int *);

/* Type PPMMeta_t defined in dumbrle2ppm.h.
 */
struct PPMMeta
{
	int width;
	int height;
	unsigned int ppm_count;
	pixval maxval;
};

/* Handles processing of the DUMBRLEv1 file pointed to. Handles header data (dimensions) and
 * iterates through characters in the file to process frames. Once a frame is found (from the
 * character being START_OF_FRAME), the file pointer and control is passed to process_frame().
 */
static void process_file(FILE *file, char *ppm_prefix)
{
	// Get width and height from file header.
	int width, height;
	get_dimension_properties(file, &width, &height);
	PPMMeta_t *ppm_data = init_ppmmeta(width, height, PIXEL_MAXVAL);

	// Process actual frame data.
	int c;
	while ((c = fgetc(file)) != END_OF_FRAMES)	// If final 'E' value found.
	{
		if (c == START_OF_FRAME)
		{
			process_frame(file, ppm_data, ppm_prefix);
		}
	}

	free(ppm_data);
}

/* Processes each channel of the frame and results are placed in pixel_array. Finishes by
 * calculating the filename for the frame before writing the pixel_array to the new PPM file.
 *
 * ppm_filename_end is a pointer to the end of a string containing the prefix for all PPM filenames.
 * It is used in this function to concatenate the frame_number and PPM extension.
 */
static void process_frame(FILE *file, PPMMeta_t *ppm_data, char *ppm_prefix)
{
	// Allocate new array of pixels to make PPM for our frame.
	pixel **pixel_array = ppm_allocarray(ppm_data->width, ppm_data->height);

	// Used for counting the pixels handled by process_channel(), so we continue to process same
	// 	channel until all pixels for frame have been processed.
	int pixel_counter = 0;
	int num_of_pixels = ppm_data->width * ppm_data->height;

	// For red, green and blue packbit-data channels.
	Channel_t channel;
	for (channel = RED; channel <= BLUE; channel++, pixel_counter = 0)
	{
		while (pixel_counter < num_of_pixels)	// Continue to process channel until all pixels processed.
			pixel_counter += process_channel(file, pixel_array, channel, pixel_counter);
	}

	// Write frame data in pixel_array to new PPM file.
    char *filename = get_filename(ppm_prefix, &ppm_data->ppm_count);
	FILE *ppm_output = fopen(filename, "w");
	if (!ppm_output)
		print_and_die("Cannot write to PPM file.", IO);

	ppm_writeppm(ppm_output, pixel_array, ppm_data->width, ppm_data->height, ppm_data->maxval, 0);

	free(filename);
	ppm_freearray(pixel_array, ppm_data->height);
	fclose(ppm_output);
}

/* Sets up filename for current frame output PPM and returns a pointer to the first char.
 */
static char *get_filename(char *ppm_prefix, unsigned int *ppm_count)
{
	// Sets up the file_name prefix for PPM output file.
	char *filename = (char *) malloc(MAX_FILENAME_LEN +1);
	strcpy(filename, ppm_prefix);
	strcat(filename, "-");

	// Concat filename with frame number and PPM extension.
	char *filename_end = &filename[strlen(ppm_prefix)] +1;	// Add 1 to address because of hyphen concat.
	sprintf(filename_end, "%04u", ++(*ppm_count));          // Incrememnt ppm_count in the process.
	strcat(filename_end, VALID_PPM_EXT);                    // Concat PPM extension.

    return filename;
}

/* Using passed FILE pointer, does the basic packbits variant algorithm, getting the next symbol
 * (packbits header) in the file before switching on it depending on it's value. Changes calculated
 * pixel in pixel_array on the specifed channel by calling to set_pixval().
 * offset arg specifies the amount of pixels in pixel_array to start setting from (so we don't
 * overwrite previously set pixvals).
 *
 * Returns the number of pixels processed in this call.
 */
static int process_channel(FILE *file, pixel **pixel_array, Channel_t channel,
		unsigned int offset)
{
	pixval symbol;
	int header, i;

	header = (char) fgetc(file);

	if (header < 0)
	{	// Repeat next symbol 2-header times and put into next 2-header pixels.
		header = 2 - header;
		symbol = fgetc(file);
		for (i = offset; i < header + offset; i++)
		{
			set_pixval(*pixel_array + i, symbol, channel);
		}
	}
	else
	{	// Set next header+1 pixels to the next header+1 values.
		header++;
		for (i = offset; i < header + offset; i++)
		{
			symbol = fgetc(file);
			set_pixval(*pixel_array + i, symbol, channel);
		}
	}
	return header;	// Amount of pixels processed in this call.
}

/* Handles setting the given channel for the pixel to the pixval gotten in process_channel().
 */
static void set_pixval(pixel *pixel, pixval symbol, Channel_t channel)
{
	switch (channel)
	{
	case RED:
		pixel->r = symbol;
		break;
	case GREEN:
		pixel->g = symbol;
		break;
	case BLUE:
		pixel->b = symbol;
		break;
	}
}

/* Allocates memory to new PPMMeta struct and initialises values as specified.
 * Returns pointer to initialised struct.
 */
static PPMMeta_t *init_ppmmeta(int init_width, int init_height, pixval init_maxval)
{
	PPMMeta_t *ppm_data = (PPMMeta_t *) malloc(sizeof(PPMMeta_t));
	if (!ppm_data)
		print_and_die("Cannot allocate memory.", MEM);

	ppm_data->width = init_width;
	ppm_data->height = init_height;
	ppm_data->maxval = init_maxval;
	ppm_data->ppm_count = 0;

	return ppm_data;
}

/* Basic argument checking in the form of arg count and checking for valid DUMBRLEv1 input file.
 * If file is valid DUMBRLEv1, it gets processed by process_file().
 */
int main(int argc, char *argv[])
{
	if (argc != 3)
		print_and_die(NULL, USAGE_1);

	FILE *file = handle_file(argv[1]);
	process_file(file, argv[2]);
	fclose(file);

	return EXIT_SUCCESS;
}
