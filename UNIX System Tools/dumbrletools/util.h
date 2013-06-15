/*
 * util.h
 *
 *      Date:   24/04/2013
 *      Author: Jonathan Poltak Samosir
 *
 * Prototypes for utility functions that do not require third-party libraries.
 */

#define DIMENSIONS_MAX_LEN      10
#define HEADER_TYPE_LEN     	11
#define PIXEL_MAXVAL			255
#define MAX_EXT_LEN			    64
#define VALID_FILE_EXT      	".dumbrle"
#define VALID_HEADER_TYPE   	"DUMBRLEv1\n"
#define START_OF_FRAME         'K'
#define END_OF_FRAMES          'E'

typedef enum
{
	IO, MEM, USAGE_1, USAGE_2, USAGE_3, USAGE_4, USAGE_5, NOT_DUMBRLE, BAD_HSV
} Error_t;

typedef enum
{
	RED, GREEN, BLUE
} Channel_t;

FILE *handle_file(char *);
void get_dimension_properties(FILE *, int *, int *);
void print_and_die(char *, Error_t);
