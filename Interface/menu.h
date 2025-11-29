#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <err.h>

#define WIDTH 1000
#define HEIGHT 600

void menu_initialize(SDL_Window **window, SDL_Renderer **renderer,
                     SDL_Rect *training_button, SDL_Rect *resolve_button);

int menu_event_handler(SDL_Window **window,
                       SDL_Rect *training_button, SDL_Rect *resolve_button);

void clean_everything(SDL_Window *window, SDL_Renderer *renderer);

int isInside(SDL_Rect rect, int x, int y);

#endif