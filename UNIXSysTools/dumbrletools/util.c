/*
 * util.c
 *
 *      Date:   24/04/2013
 *      Author: Jonathan Poltak Samosir
 *      ID:     2271 3603
 *
 * Contains numerous utility functions used by all programs in this assignment.
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>     // Used for checking filename extensions.
#include <SDL/SDL.h>
#include "util.h"       // Header file with stdlib function prototypes.
#include "util-SDL.h"   // Header file with functions prototypes using SDL library.


// Function only used in this file.
static bool is_valid_file(FILE *, char *);


/* Attempts to open file at file_path and check if it's a valid DUMBRLEv1 file. If so, it returns a
 * pointer to the FILE, else calls print_and_die() with NOT_DUMBRLE error.
 */
FILE *handle_file(char *file_path)
{
    FILE *file;

    file = fopen(file_path, "r");
    if (!file)                                  // If cannot open file.
        print_and_die("Cannot open file.", IO);

    if (!is_valid_file(file, file_path))        // If not a valid DUMBRLEv1 file.
    {
        fclose(file);
        print_and_die(NULL, NOT_DUMBRLE);
    }

    return file;
}

/* Checks the first line of the file header to make sure it matches the DUMBRLEv1 type.
 * Also checks that the file extension is ".dumbrle".
 * Returns true if above conditions are satisfied, otherwise false.
 */
static bool is_valid_file(FILE *file, char *file_path)
{
	// Short circuit calculations and return false if file ext. is not valid DUMBRLEv1 extension.
	char *file_ext = strrchr(file_path, '.');
	if (!file_ext || strcmp(file_ext, VALID_FILE_EXT))
	{
		return false;
	}

	bool return_val = false;
	char *header = (char *) malloc(HEADER_TYPE_LEN + 1);
	if (!header)
	{
		fclose(file);
		print_and_die("Cannot allocate memory.", MEM);
	}

	// Get first line of file and compare to valid DUMBRLEv1 header.
	if (fgets(header, HEADER_TYPE_LEN, file)
			&& !strcmp(header, VALID_HEADER_TYPE))
		return_val = true;

    //free(file_ext);
	free(header);
	return return_val;
}

/* Checks the header of the file to get the dimension properties as specified in the DUMBRLEv1 file
 * format description.
 * Stores property values in addresses pointed to by width and height arguments.
 */
void get_dimension_properties(FILE *file, int *width, int *height)
{
	char *header = (char *) malloc(DIMENSIONS_MAX_LEN +1);
	if (!header)
	{
		print_and_die("Cannot allocate memory.", MEM);
	}

	if (!fgets(header, DIMENSIONS_MAX_LEN, file))
    {
        fclose(file);
        free(header);
        print_and_die("Cannot get dimension properties", IO);
    }

	sscanf(header, "%d %d", width, height);
	free(header);
}

/* Allocates enough memory to make num_of_channels channels of num_of_pixels 8 bit values.
 * Returns pointer to start address of allocated channels.
 */
Uint8 **alloc_channels(unsigned int num_of_channels, unsigned int num_of_pixels)
{
	int i;
	Uint8 **channels = (Uint8 **) malloc(num_of_channels);
	if (!channels)
		print_and_die("Cannot allocate memory for screen.", MEM);

	for (i = 0; i < num_of_channels; i++)
	{
		channels[i] = (Uint8 *) malloc(num_of_pixels);
		if (!channels[i])
			print_and_die("Cannot allocate memory for screen.", MEM);
	}

	return channels;
}

/* Frees memory for all num_of_channels in the channels vector passed in.
 */
void free_channels(Uint8** channels, unsigned int num_of_channels)
{
	int i;
	for (i = 0; i < num_of_channels; i++)
		free(channels[i]);
	free(channels);
}

/* Prints commonly used error message, with optional detail through arguments and exits with an
 * EXIT_FAILURE signal.
 * Error message specified by enum type Error_t.
 */
void print_and_die(char *message, Error_t type)
{
    switch (type)
    {
        case IO:
            fprintf(stderr, "Fatal IO error encountered:\n%s\n", message);
            break;
        case MEM:
            fprintf(stderr, "Fatal memory error occurred:\n%s\n", message);
            break;
        case NOT_DUMBRLE:
            fprintf(stderr, "File is not a standard DUMBRLEv1 file.\n");
            break;
        case USAGE_1:
            fprintf(stderr, "usage: dumbrletoppm <dumbrle file> <PPM output prefix>\n");
            break;
        case USAGE_2:
            fprintf(stderr, "usage: firstframeview <dumbrle file> <display time 0-*>\n");
            break;
        case USAGE_3:
            fprintf(stderr, "usage: dumbrleplayer <dumbrle file> <frame delay 0-*>\n");
            break;
        case USAGE_4:
            fprintf(stderr, "usage: smartrleplayer <dumbrle file> <frame delay 0-*> <brightness 0-100> <contrast 0-100> <saturation 0-100>\n");
            break;
        case BAD_HSV:
            fprintf(stderr, "%s\n", message);
            break;
        default:
            break;
    }
    exit(EXIT_FAILURE);
}
