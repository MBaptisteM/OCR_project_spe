#include "training.h"
#include "menu.h"

#define WIDTH 1000
#define HEIGHT 600

void end_training(struct call* c){
    pthread_mutex_lock(c->mutex);
    *c->run = 0;
    save("../AI_OCR/weights.txt");
    generate_network();
    pthread_mutex_unlock(c->mutex);
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
        400,
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
    if (c == NULL)
        errx(EXIT_FAILURE, "issue when creating the call");

    float* accuracy = malloc(sizeof(float));
    if (accuracy == NULL)
        errx(EXIT_FAILURE, "issue when creating the accuracy");
    *accuracy = 0;

    char* run = malloc(sizeof(char));
    if (run == NULL)
        errx(EXIT_FAILURE, "issue when creating the run");
    *run = 1;

    pthread_mutex_t *mtx = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mtx, NULL);

    c->mutex = mtx;
    c->accuracy = accuracy;
    c->run = run;
    
    pthread_t thr;
    if (pthread_create(&thr, NULL, start_training, (void*)c))
        errx(EXIT_FAILURE, "issue when creating the thread");

    SDL_Color blue = {11, 36, 189, 255};
    TTF_Font *acc_font = TTF_OpenFont("INTERFACE/elements/text.TTF", 40);
        if (!acc_font)
            errx(EXIT_FAILURE, "fail open text.TTF");
    SDL_Surface *acc_fixed =
    TTF_RenderText_Solid(acc_font, "Accuracy : ", blue);
    SDL_Texture *acc_fixedTex =
        SDL_CreateTextureFromSurface(*renderer, acc_fixed);
    SDL_Rect acc_fixedRect = {
        250,
        HEIGHT/2,
        acc_fixed->w,
        acc_fixed->h
    };

    while (running){
        //SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_SetRenderDrawColor(*renderer, 236, 224, 197, 255);
        SDL_RenderClear(*renderer);
        
        //--------------------------display accuracy---------------------------
        SDL_RenderCopy(*renderer, acc_fixedTex, NULL, &acc_fixedRect);

        char acc_value[20];
        pthread_mutex_lock(c->mutex);
        float acc_copy = *c->accuracy;
        pthread_mutex_unlock(c->mutex);
        sprintf(acc_value, "%.1f / 100", acc_copy);
        SDL_Surface *accSurf =
            TTF_RenderText_Solid(acc_font,
                                acc_value,
                                blue);
        if(!accSurf)
            continue;
        SDL_Texture *accTex =
            SDL_CreateTextureFromSurface(*renderer, accSurf);
        if(!accTex)
            continue;
        SDL_Rect accRect = {
            550,
            HEIGHT/2,
            accSurf->w,
            accSurf->h
        };
        SDL_RenderCopy(*renderer, accTex, NULL, &accRect);
        SDL_RenderCopy(*renderer, txt, NULL, &txtRect);
        SDL_RenderCopy(*renderer, buttonTex, NULL, &stopRect);
        SDL_RenderCopy(*renderer, stopTxt, NULL, &stopTxtRect);
        SDL_RenderPresent(*renderer);
        SDL_FreeSurface(accSurf);
        SDL_DestroyTexture(accTex);
        SDL_Delay(16);

        while (SDL_PollEvent(&event)){
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

        
    }
    pthread_join(thr, NULL);

    pthread_mutex_destroy(c->mutex);
    free(c->mutex);
    free(c->accuracy);
    free(c->run);
    free(c);

    SDL_FreeSurface(txtSurf);
    SDL_FreeSurface(stopSurf);
    SDL_DestroyTexture(txt);
    SDL_DestroyTexture(stopTxt);
    SDL_DestroyTexture(buttonTex);
    SDL_FreeSurface(acc_fixed);
    SDL_DestroyTexture(acc_fixedTex);
    //SDL_DestroyTexture(background);
    TTF_CloseFont(big);
    TTF_CloseFont(acc_font);

    return EXIT_SUCCESS;
}