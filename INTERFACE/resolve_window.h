#ifndef RESOLVE_WINDOW
#define RESOLVE_WINDOW

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

//gcc -o start start_window.c test.c menu.c -Wall -Wextra `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image
typedef struct {
    SDL_Texture* texture;
    SDL_Rect rect;
    const char* name;
} ImageItem;

void start_initialize(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture **texture, ImageItem *allImages);
void start_redraw(SDL_Renderer *renderer, ImageItem *images);
void start_clear(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);
int start_event_handler(SDL_Window *window, ImageItem *images);
SDL_Texture *loadChoice(const char* name, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface **image);
size_t isInsidePicture(ImageItem *images, int x, int y);

#endif
