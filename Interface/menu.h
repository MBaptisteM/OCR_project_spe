#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>


void menu_initialize(SDL_Window **window, SDL_Renderer **renderer,
                     SDL_Rect *training_button, SDL_Rect *resolve_button);


void clean_everything(SDL_Window *window, SDL_Renderer *renderer);


int menu_event_handler(SDL_Window **window,
                       SDL_Rect *training_button, SDL_Rect *resolve_button);


int isInside(SDL_Rect rect, int x, int y);

#endif 