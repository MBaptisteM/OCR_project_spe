#ifndef RESOLVE_WINDOW
#define RESOLVE_WINDOW

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h> 

struct displayInfo
{
    size_t n;
    int cols;
    int rows;
    char imp;
};
typedef struct {
    SDL_Texture* texture;
    SDL_Rect rect;
    char* name;
} ImageItem;

size_t count_files();
void start_initialize(SDL_Window *window, SDL_Renderer *renderer, 
    SDL_Texture **texture, ImageItem *images);
void start_redraw(SDL_Renderer *renderer, ImageItem *images, size_t n);
void start_clear(SDL_Window *window, SDL_Renderer *renderer, 
    SDL_Texture *texture, size_t n, ImageItem *images);
int start_event_handler(SDL_Window *window, ImageItem *images, size_t n);
SDL_Texture *loadChoice(const char* name, SDL_Window *window, 
    SDL_Renderer *renderer, SDL_Surface **image) ;
size_t isInsidePicture(ImageItem *images, int x, int y, size_t n); 

#endif
