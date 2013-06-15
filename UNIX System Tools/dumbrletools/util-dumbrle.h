/*
 * util-dumbrle.h
 *
 *  	Date: 	24/04/2013
 *      Author: Jonathan Poltak Samosir
 */

#define BITS_PER_PIXEL  32

void process_frame(FILE *, SDL_Surface *);
int process_channel(FILE *, Uint8 *, unsigned int);
