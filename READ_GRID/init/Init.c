#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_rotozoom.h>
#include <stdio.h>
#include "Pixel.h"


//binarize function
void binarize(SDL_Surface *img, Uint8 threshold) {
	Uint32 pixel;
	Uint8 r, g, b;

    // Parcourt chaque pixel de l'image
	for (int y = 0; y < img->h; y++) {
		for (int x = 0; x < img->w; x++) {
            // Récupère la couleur du pixel actuel
			pixel = getPixel(img, x, y);
            // Extrait les composantes r,g,b du pixel
			SDL_GetRGB(pixel, img->format, &r, &g, &b);
            // Calcule la couleur approximative du pixel et applique du noir ou blanc
			Uint8 val = (0.3*r + 0.59*g + 0.11*b > threshold) ? 255 : 0;
            // Remplace le pixel original par sa version noir/blanc
			pixel = SDL_MapRGB(img->format, val, val, val);
			setPixel(img, x, y, pixel);
		}
	}
}

void rotate_image(SDL_Surface **img, double angle) {
	if (!img || !*img) return;

	SDL_Surface *rotated = rotozoomSurface(*img, -angle, 1.0, 1);
	if (!rotated) {
		printf("Erreur de rotation.\n");
		return;
	}

	SDL_FreeSurface(*img);
	*img = rotated;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Erreur args>\n");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_Surface *img = IMG_Load(argv[1]);
	if (!img) {
		printf("Erreur de chargement\n");
		return 1;
	}

	printf("Image chargée: %dx%d pixels\n", img->w, img->h);

	SDL_Surface *converted = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(img);
	img = converted;

	binarize(img, 128);
	rotate_image(&img, 45.0);

	IMG_SavePNG(img, "output.png");


	SDL_Window *window = SDL_CreateWindow("Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, img->w, img->h, 0);
	SDL_Surface *window_surface = SDL_GetWindowSurface(window);
	SDL_BlitSurface(img, NULL, window_surface, NULL);
	SDL_UpdateWindowSurface(window);
	SDL_Delay(5000);

	SDL_DestroyWindow(window);

	SDL_FreeSurface(img);
	IMG_Quit();
	SDL_Quit();

	return 0;
}