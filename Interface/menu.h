#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

void menu_initialize(SDL_Window **window, SDL_Renderer **renderer, SDL_Rect *button);
void clean_everything(SDL_Window *window, SDL_Renderer *renderer);
int menu_event_handler(SDL_Window **window, SDL_Rect *button);
int isInside(SDL_Rect rect, int x, int y);