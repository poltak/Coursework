/*
 * util-SDL.h
 *
 *      Date:   24/04/2013
 *      Author: Jonathan Poltak Samosir
 *
 * Prototypes for utility functions that require SDL libraries.
 */

Uint8 **alloc_channels(unsigned int, unsigned int);
void free_channels(Uint8 **, unsigned int);
