#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "pixel.h"


void binarize(SDL_Surface *img);
int is_colored(SDL_Surface *img);
void dark_exp(SDL_Surface *img);
void erode(SDL_Surface *img);
void remove_isolated_black_pixels(SDL_Surface *img);
double detect_rotation_angle_projection(SDL_Surface *img, int step_deg);
void rotate_image(SDL_Surface **img, double angle_degrees);
