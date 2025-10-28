#include <SDL2/SDL.h>
#include "Pixel.h"

static Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y) {
	// Récupère le nombre d'octets utilisés pour stocker un pixel (1, 2, 3 ou 4)
	int bpp = surf->format->BytesPerPixel;

	// Calcule l'adresse mémoire du pixel (x, y)
	//  -> (Uint8*)surf->pixels : pointeur vers le début des pixels dans la surface
	//  -> y * surf->pitch : saute les lignes précédentes (chaque ligne a "pitch" octets)
	//  -> x * bpp : avance de x pixels dans la ligne courante
	// Le résultat est donc l'adresse exacte du pixel demandé
	return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}


Uint32 getPixel(SDL_Surface *img, unsigned x, unsigned y) {
	Uint8 *p = pixel_ref(img, x, y);

	// Sélectionne la méthode de lecture selon la profondeur de couleur (en octets par pixel)
	switch (img->format->BytesPerPixel) {
		//La couleur est stockée sur un seul octet
		case 1:
			return *p;
		//la couleur est sur deux octets
		case 2:
			return *(Uint16 *)p;
		//Traitement spécial car pas d’alignement direct en mémoire
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				return p[0] << 16 | p[1] << 8 | p[2];
			}
			else {
				return p[0] | p[1] << 8 | p[2] << 16;
			}
		//La couleur est sur quatre octets (Uint32)
		case 4:
			return *(Uint32 *)p;
	}

	return 0;
}

void setPixel(SDL_Surface *img, unsigned x, unsigned y, Uint32 pixel) {
	Uint8 *p = pixel_ref(img, x, y);

	// Choisit la méthode d’écriture selon la profondeur de couleur
	switch(img->format->BytesPerPixel) {
		//Ecriture directe
		case 1:
			*p = pixel;
			break;
		//Ecriture sur deux octets
		case 2:
			*(Uint16 *)p = pixel;
			break;
		//Ecriture manuelle selon l’ordre des octets
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
		//Ecriture directe sur quatre octets
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}