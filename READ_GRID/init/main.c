#include "Init.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Error args>\n");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_Surface *img = IMG_Load(argv[1]);

	if (!img) {
		printf("Error while loading picture\n");
		return 1;
	}


	SDL_Surface *converted = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(img);
	img = converted;

	binarize(img);
	double angle = detect_rotation_angle_projection(img, 1);
	rotate_image(&img, angle);

	IMG_SavePNG(img, "output.png");

	// SDL_Window *window = SDL_CreateWindow("Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, img->w, img->h, 0);
	// SDL_Surface *window_surface = SDL_GetWindowSurface(window);
	// SDL_BlitSurface(img, NULL, window_surface, NULL);
	// SDL_UpdateWindowSurface(window);
	//
	// SDL_DestroyWindow(window);
	//
	// SDL_FreeSurface(img);
	// IMG_Quit();
	// SDL_Quit();

	return 0;
}
