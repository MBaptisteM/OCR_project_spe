#include "menu.h"

void menu_initialize(SDL_Window **window, SDL_Renderer **renderer,
                     SDL_Rect *training_button, SDL_Rect *resolve_button)
{
    (void)window; 
    if (!renderer || !*renderer)
        errx(EXIT_FAILURE, "renderer is NULL in menu_initialize");

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_RenderClear(*renderer);

    // ---------- BACKGROUND ----------
    SDL_Texture *background =
        IMG_LoadTexture(*renderer, "elements/background.png");
    if (!background)
        errx(EXIT_FAILURE, "fail load background");

    SDL_RenderCopy(*renderer, background, NULL, NULL);

    // ---------- TITRE ----------
    TTF_Font *title_font = TTF_OpenFont("elements/tittle.TTF", 100);
    if (!title_font)
        errx(EXIT_FAILURE, "fail open tittle.TTF");

    SDL_Color blue = {11, 36, 189, 255};

    SDL_Surface *wordSurf = TTF_RenderText_Solid(title_font, "WORD", blue);
    SDL_Texture *wordTex = SDL_CreateTextureFromSurface(*renderer, wordSurf);
    SDL_Rect wordRect = {
        (WIDTH - wordSurf->w) / 2,
        15,
        wordSurf->w,
        wordSurf->h
    };
    SDL_RenderCopy(*renderer, wordTex, NULL, &wordRect);

    SDL_Surface *solverSurf = TTF_RenderText_Solid(title_font, "SOLVER", blue);
    SDL_Texture *solverTex = SDL_CreateTextureFromSurface(*renderer, solverSurf);
    SDL_Rect solverRect = {
        (WIDTH - solverSurf->w) / 2,
        45 + solverSurf->h,
        solverSurf->w,
        solverSurf->h
    };
    SDL_RenderCopy(*renderer, solverTex, NULL, &solverRect);

    SDL_FreeSurface(wordSurf);
    SDL_DestroyTexture(wordTex);
    SDL_FreeSurface(solverSurf);
    SDL_DestroyTexture(solverTex);
    TTF_CloseFont(title_font);

    // ---------- TUTO ----------
    TTF_Font *tuto_font = TTF_OpenFont("elements/text.TTF", 20);
    if (!tuto_font)
        errx(EXIT_FAILURE, "fail open text.TTF");

    SDL_Surface *howSurf =
        TTF_RenderText_Solid(tuto_font, "How to resolve a grid :", blue);
    SDL_Texture *howTex =
        SDL_CreateTextureFromSurface(*renderer, howSurf);
    SDL_Rect howRect = {
        (WIDTH - howSurf->w) / 2,
        430,
        howSurf->w,
        howSurf->h
    };
    SDL_RenderCopy(*renderer, howTex, NULL, &howRect);

    SDL_Surface *line1Surf =
        TTF_RenderText_Solid(tuto_font,
                             "1 - Click on a button to continue.",
                             blue);
    SDL_Texture *line1Tex =
        SDL_CreateTextureFromSurface(*renderer, line1Surf);
    SDL_Rect line1Rect = {
        (WIDTH - line1Surf->w) / 2,
        465,
        line1Surf->w,
        line1Surf->h
    };
    SDL_RenderCopy(*renderer, line1Tex, NULL, &line1Rect);

    SDL_Surface *line2Surf =
        TTF_RenderText_Solid(tuto_font,
                             "2 - Select TRAINING or RESOLVE.",
                             blue);
    SDL_Texture *line2Tex =
        SDL_CreateTextureFromSurface(*renderer, line2Surf);
    SDL_Rect line2Rect = {
        (WIDTH - line2Surf->w) / 2,
        495,
        line2Surf->w,
        line2Surf->h
    };
    SDL_RenderCopy(*renderer, line2Tex, NULL, &line2Rect);

    SDL_Surface *line3Surf =
        TTF_RenderText_Solid(tuto_font,
                             "3 - Follow instructions.",
                             blue);
    SDL_Texture *line3Tex =
        SDL_CreateTextureFromSurface(*renderer, line3Surf);
    SDL_Rect line3Rect = {
        (WIDTH - line3Surf->w) / 2,
        525,
        line3Surf->w,
        line3Surf->h
    };
    SDL_RenderCopy(*renderer, line3Tex, NULL, &line3Rect);

    SDL_FreeSurface(howSurf);
    SDL_FreeSurface(line1Surf);
    SDL_FreeSurface(line2Surf);
    SDL_FreeSurface(line3Surf);
    SDL_DestroyTexture(howTex);
    SDL_DestroyTexture(line1Tex);
    SDL_DestroyTexture(line2Tex);
    SDL_DestroyTexture(line3Tex);
    TTF_CloseFont(tuto_font);

    // ---------- BOUTONS ----------
    SDL_Texture *buttonTex =
        IMG_LoadTexture(*renderer, "elements/button.png");
    if (!buttonTex)
        errx(EXIT_FAILURE, "fail load button.png");

    TTF_Font *btnFont = TTF_OpenFont("elements/tittle.TTF", 40);
    if (!btnFont)
        errx(EXIT_FAILURE, "fail open tittle.TTF for buttons");

    SDL_Color white = {255, 255, 255, 255};

    // TRAINING
    SDL_Surface *trainSurf =
        TTF_RenderText_Solid(btnFont, "TRAINING", white);
    SDL_Texture *trainTex =
        SDL_CreateTextureFromSurface(*renderer, trainSurf);

    SDL_Rect trainRect = {
        (WIDTH - (int)(trainSurf->w * 1.6)) / 2.1,
        220,
        (int)(trainSurf->w * 1.7),
        (int)(trainSurf->h * 4)
    };
    SDL_RenderCopy(*renderer, buttonTex, NULL, &trainRect);

    SDL_Rect trainTextRect = {
        (WIDTH - trainSurf->w) / 2 + 10,
        trainRect.y + (trainRect.h - trainSurf->h) / 2,
        trainSurf->w,
        trainSurf->h
    };
    SDL_RenderCopy(*renderer, trainTex, NULL, &trainTextRect);

    // RESOLVE
    SDL_Surface *resolveSurf =
        TTF_RenderText_Solid(btnFont, "RESOLVE", white);
    SDL_Texture *resolveTex =
        SDL_CreateTextureFromSurface(*renderer, resolveSurf);

    SDL_Rect resolveRect = {
        (WIDTH - (int)(resolveSurf->w * 1.6)) / 2.1,
        310,
        (int)(resolveSurf->w * 1.7),
        (int)(resolveSurf->h * 4)
    };
    SDL_RenderCopy(*renderer, buttonTex, NULL, &resolveRect);

    SDL_Rect resolveTextRect = {
        (WIDTH - resolveSurf->w) / 2 + 10,
        resolveRect.y + (resolveRect.h - resolveSurf->h) / 2,
        resolveSurf->w,
        resolveSurf->h
    };
    SDL_RenderCopy(*renderer, resolveTex, NULL, &resolveTextRect);

    // Renvoi des zones cliquables
    *training_button = trainRect;
    *resolve_button = resolveRect;

    // On affiche TOUT
    SDL_RenderPresent(*renderer);

    // Clean des ressources locales
    SDL_FreeSurface(trainSurf);
    SDL_FreeSurface(resolveSurf);
    SDL_DestroyTexture(trainTex);
    SDL_DestroyTexture(resolveTex);
    SDL_DestroyTexture(buttonTex);
    SDL_DestroyTexture(background);
    TTF_CloseFont(btnFont);
}

void clean_everything(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int menu_event_handler(SDL_Window **window,
                       SDL_Rect *training_button, SDL_Rect *resolve_button)
{
    SDL_Event event;
    (void)window;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return 0;

        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT)
        {
            int mx = event.button.x;
            int my = event.button.y;

            if (isInside(*training_button, mx, my))
                return 3;

            if (isInside(*resolve_button, mx, my))
                return 2;
        }
    }
    return 1;
}

int isInside(SDL_Rect rect, int x, int y)
{
    return (x >= rect.x &&
            x <= rect.x + rect.w &&
            y >= rect.y &&
            y <= rect.y + rect.h);
}