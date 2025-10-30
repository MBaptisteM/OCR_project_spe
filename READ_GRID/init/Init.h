#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "Pixel.h"


void binarize(SDL_Surface *img);
void rotate_image(SDL_Surface **img, double angle_degrees);
