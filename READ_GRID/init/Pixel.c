#include <SDL2/SDL.h>
#include "Pixel.h"

static Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y) {
	// Retrieves the number of bytes used to store a pixel (1, 2, 3, or 4)
	int bpp = surf->format->BytesPerPixel;

	// Calculates the memory address of the pixel (x, y)
	//  -> (Uint8*)surf->pixels : pointer to the beginning of pixels in the surface
	//  -> y * surf->pitch : skips previous lines (each line has "pitch" bytes)
	//  -> x * bpp : advances x pixels in the current line
	// The result is therefore the exact address of the requested pixel
	return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}


Uint32 getPixel(SDL_Surface *img, unsigned x, unsigned y) {
	Uint8 *p = pixel_ref(img, x, y);

	// Selects the reading method according to the color depth (in bytes per pixel)
	switch (img->format->BytesPerPixel) {
		//The color is stored on a single byte
		case 1:
			return *p;
		//the color is on two bytes
		case 2:
			return *(Uint16 *)p;
		//Special handling because there is no direct memory alignment
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				return p[0] << 16 | p[1] << 8 | p[2];
			}
			else {
				return p[0] | p[1] << 8 | p[2] << 16;
			}
		//The color is on four bytes (Uint32)
		case 4:
			return *(Uint32 *)p;
	}

	return 0;
}

void setPixel(SDL_Surface *img, unsigned x, unsigned y, Uint32 pixel) {
	Uint8 *p = pixel_ref(img, x, y);

	// Chooses the writing method according to the color depth
	switch(img->format->BytesPerPixel) {
		//Direct writing
		case 1:
			*p = pixel;
			break;
		//Writing on two bytes
		case 2:
			*(Uint16 *)p = pixel;
			break;
		//Manual writing according to the byte order
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		//Direct writing on four bytes
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}