#include "init/Init.h"
#include "get_letters/get_letters.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
        errx(EXIT_FAILURE, "not enough arguments");
    
    SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    const char *filename = argv[1];
	SDL_Surface *img = IMG_Load(filename);

	if (!img) 
    {
		printf("Error while loading picture\n");
		return 1;
	}

	SDL_Surface *converted = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(img);
	img = converted;

	binarize(img);
    if (argc == 3)
        rotate_image(&img, atoi(argv[2]));

    char instruction[150];
    snprintf(instruction, sizeof(instruction), "./ocr %s", filename);
    
    system(instruction);
    
    return EXIT_SUCCESS;
}