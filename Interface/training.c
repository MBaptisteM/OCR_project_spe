#include "training.h"

#define WIDTH 1000
#define HEIGHT 600

void training_screen(SDL_Window *window, SDL_Renderer *renderer)
{
    int running = 1;
    SDL_Event event;

    // ---------- charger background ----------
    SDL_Texture *background =
        IMG_LoadTexture(renderer, "elements/background.png");
    if (!background)
        errx(EXIT_FAILURE, "fail load background");

    // ---------- Fonts ----------
    TTF_Font *big = TTF_OpenFont("elements/tittle.TTF", 80);
    if (!big)
        errx(EXIT_FAILURE, "fail load tittle");

    SDL_Color color = {11, 36, 189};

    SDL_Surface *txtSurf =
        TTF_RenderText_Solid(big, "TRAINING IN PROGRESS", color);
    SDL_Texture *txt =
        SDL_CreateTextureFromSurface(renderer, txtSurf);

    SDL_Rect txtRect = {
        (WIDTH - txtSurf->w) / 2,
        80,
        txtSurf->w,
        txtSurf->h
    };

    // ---------- bouton STOP ----------
    SDL_Texture *buttonTex =
        IMG_LoadTexture(renderer, "elements/button.png");
    if (!buttonTex)
        errx(EXIT_FAILURE, "fail load stop button");

    SDL_Surface *stopSurf =
        TTF_RenderText_Solid(big, "STOP", (SDL_Color){255, 255, 255});
    SDL_Texture *stopTxt =
        SDL_CreateTextureFromSurface(renderer, stopSurf);

    SDL_Rect stopRect = {
        (WIDTH - stopSurf->w * 2) / 2,
        350,
        stopSurf->w * 2,
        stopSurf->h * 5
    };

    SDL_Rect stopTxtRect = {
        (WIDTH - stopSurf->w) / 2 + 10,
        stopRect.y + (stopRect.h - stopSurf->h) / 2,
        stopSurf->w,
        stopSurf->h
    };

    // ---------- Boucle principale ----------
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        int mx = event.button.x;
                        int my = event.button.y;

                        if (mx >= stopRect.x && mx <= stopRect.x + stopRect.w &&
                            my >= stopRect.y && my <= stopRect.y + stopRect.h)
                        {
                            running = 0;  // on ferme la page training
                        }
                    }
                    break;
            }
        }

        // ---------- rendering ----------
        SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_RenderCopy(renderer, txt, NULL, &txtRect);
        SDL_RenderCopy(renderer, buttonTex, NULL, &stopRect);
        SDL_RenderCopy(renderer, stopTxt, NULL, &stopTxtRect);

        SDL_RenderPresent(renderer);
    }

    // ---------- clean ----------
    SDL_FreeSurface(txtSurf);
    SDL_DestroyTexture(txt);
    SDL_FreeSurface(stopSurf);
    SDL_DestroyTexture(stopTxt);
    SDL_DestroyTexture(buttonTex);
    SDL_DestroyTexture(background);
    TTF_CloseFont(big);
}