#ifndef TRAINING_H
#define TRAINING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include "../AI_OCR/training.h"

void end_training(struct call* c);
int training_screen(SDL_Window *window, SDL_Renderer **renderer);

#endif