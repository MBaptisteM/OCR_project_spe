#include "training.h"
#include "menu.h"

#define WIDTH 1000
#define HEIGHT 600

void training_screen(SDL_Window *window, SDL_Renderer *renderer)
{
    (void)window;
    int running = 1;
    SDL_Event event;

    SDL_Texture *background =
        IMG_LoadTexture(renderer, "elements/background.png");

    TTF_Font *big = TTF_OpenFont("elements/tittle.TTF", 50);
    SDL_Color color = {11, 36, 189, 255};

    SDL_Surface *txtSurf =
        TTF_RenderText_Solid(big, "TRAINING IN PROGRESS", color);
    SDL_Texture *txt =
        SDL_CreateTextureFromSurface(renderer, txtSurf);

    SDL_Rect txtRect = {
        (WIDTH - txtSurf->w) / 2,
        100,
        txtSurf->w,
        txtSurf->h
    };

    SDL_Texture *buttonTex = IMG_LoadTexture(renderer, "elements/button.png");

    SDL_Surface *stopSurf =
        TTF_RenderText_Solid(big, "STOP", (SDL_Color){255, 255, 255,255});
    SDL_Texture *stopTxt =
        SDL_CreateTextureFromSurface(renderer, stopSurf);

    SDL_Rect stopRect = {
        (WIDTH - stopSurf->w * 1.6) / 2.16,
        220,
        stopSurf->w * 1.9,
        stopSurf->h * 4.1
    };

    SDL_Rect stopTxtRect = {
        (WIDTH - stopSurf->w) / 2 + 10,
        stopRect.y + (stopRect.h - stopSurf->h) / 2,
        stopSurf->w,
        stopSurf->h
    };

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                int mx = event.button.x;
                int my = event.button.y;

                if (isInside(stopRect, mx, my))
                    running = 0;
            }
        }

        SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_RenderCopy(renderer, txt, NULL, &txtRect);
        SDL_RenderCopy(renderer, buttonTex, NULL, &stopRect);
        SDL_RenderCopy(renderer, stopTxt, NULL, &stopTxtRect);
        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(txtSurf);
    SDL_FreeSurface(stopSurf);
    SDL_DestroyTexture(txt);
    SDL_DestroyTexture(stopTxt);
    SDL_DestroyTexture(buttonTex);
    SDL_DestroyTexture(background);
    TTF_CloseFont(big);
}