#include "training.h"
#include "menu.h"

#define WIDTH 1000
#define HEIGHT 600

void end_training(struct call* c){
    pthread_mutex_lock(&c->mutex);
    c->run = 0;
    save("weights.txt");
    pthread_mutex_unlock(&c->mutex);
}
int training_screen(SDL_Window *window, SDL_Renderer **renderer)
{
    (void)window;
    int running = 1;
    SDL_Event event;

    /*SDL_Texture *background =
        IMG_LoadTexture(renderer, "elements/background.png");*/

    SDL_SetRenderDrawColor(*renderer, 236, 224, 197, 255); //bg color
    SDL_RenderClear(*renderer);

    TTF_Font *big = TTF_OpenFont("INTERFACE/elements/tittle.TTF", 50);
    SDL_Color color = {11, 36, 189, 255};

    SDL_Surface *txtSurf =
        TTF_RenderText_Solid(big, "TRAINING IN PROGRESS", color);
    SDL_Texture *txt =
        SDL_CreateTextureFromSurface(*renderer, txtSurf);

    SDL_Rect txtRect = {
        (WIDTH - txtSurf->w) / 2,
        100,
        txtSurf->w,
        txtSurf->h
    };

    SDL_Texture *buttonTex = IMG_LoadTexture(*renderer, 
        "INTERFACE/elements/button.png");

    SDL_Surface *stopSurf =
        TTF_RenderText_Solid(big, "STOP", (SDL_Color){255, 255, 255,255});
    SDL_Texture *stopTxt =
        SDL_CreateTextureFromSurface(*renderer, stopSurf);

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

    

    struct call* c = malloc(sizeof(struct call));
    

    float* accuracy = malloc(sizeof(float));
    *accuracy = 0;

    char* run = malloc(sizeof(char));
    *run = 1;

    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, NULL);

    c->mutex = mtx;
    c->accuracy = accuracy;
    c->run = run;

    load("/home/baptiste/OCR_project_spe/AI_OCR/weights.txt");
    pthread_t thr;
    pthread_create(&thr, NULL, start_training, (void*)c);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT){
                end_training(c);
                return -1;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                
                int mx = event.button.x;
                int my = event.button.y;

                if (isInside(stopRect, mx, my)){
                    end_training(c);
                    running = 0;
                }
            }
        }

        printf("acc = %.2f\n", *c->accuracy);

        //SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_SetRenderDrawColor(*renderer, 236, 224, 197, 255);
        SDL_RenderClear(*renderer);
        SDL_RenderCopy(*renderer, txt, NULL, &txtRect);
        SDL_RenderCopy(*renderer, buttonTex, NULL, &stopRect);
        SDL_RenderCopy(*renderer, stopTxt, NULL, &stopTxtRect);
        SDL_RenderPresent(*renderer);
    }

    pthread_mutex_destroy(&c->mutex);
    free(c->accuracy);
    free(c->run);
    free(c);

    SDL_FreeSurface(txtSurf);
    SDL_FreeSurface(stopSurf);
    SDL_DestroyTexture(txt);
    SDL_DestroyTexture(stopTxt);
    SDL_DestroyTexture(buttonTex);
    //SDL_DestroyTexture(background);
    TTF_CloseFont(big);

    return EXIT_SUCCESS;
}