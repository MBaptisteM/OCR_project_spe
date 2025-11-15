#include "menu.h"

#define WIDTH 1000
#define HEIGHT 600

void menu_initialize(SDL_Window **window, SDL_Renderer **renderer, SDL_Rect *button)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "fail initialization sdl");

    if (TTF_Init() < 0)
        errx(EXIT_FAILURE, "fail initialization TTF");

    *window = SDL_CreateWindow("interface menu", SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (*window == NULL)
        clean_everything(*window, *renderer);

    *renderer = SDL_CreateRenderer(*window,-1, SDL_RENDERER_ACCELERATED | 
        SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL)
        clean_everything(*window, *renderer);

    //------------------------clean past renderer------------------------------
    
    SDL_SetRenderDrawColor(*renderer, 236, 224, 197, 255); //bg color
    SDL_RenderClear(*renderer);

    //--------------------------create tittle----------------------------------
    
    TTF_Font* tittle = TTF_OpenFont("elements/tittle.TTF", 70);
    if (tittle  == NULL)
        errx(EXIT_FAILURE, "fail find tittle ttf");

    SDL_Color text_color = {74, 90, 238, 255};
    SDL_Surface* wordMessage = 
        TTF_RenderText_Solid(tittle, "WORD", text_color); 
    SDL_Texture* word = 
        SDL_CreateTextureFromSurface(*renderer, wordMessage);
    
    SDL_Rect word_rect = {(WIDTH - wordMessage->w) / 2, 40, wordMessage->w,
         wordMessage->h};
    SDL_RenderCopy(*renderer, word, NULL, &word_rect);
    SDL_RenderPresent(*renderer);
    SDL_Surface* solverMessage = 
        TTF_RenderText_Solid(tittle, "SOLVER", text_color); 
    SDL_Texture* solver = 
        SDL_CreateTextureFromSurface(*renderer, solverMessage);
    
    SDL_Rect solver_rect = {(WIDTH - solverMessage->w)/2, 45+solverMessage->h,
         solverMessage->w, solverMessage->h};
    SDL_RenderCopy(*renderer, solver, NULL, &solver_rect);
    SDL_RenderPresent(*renderer);


    SDL_FreeSurface(wordMessage);
    SDL_DestroyTexture(word);
    SDL_FreeSurface(solverMessage);
    SDL_DestroyTexture(solver);
    TTF_CloseFont(tittle);

    //----------------------------create tuto----------------------------------

    TTF_Font* tuto = TTF_OpenFont("elements/text.TTF", 20);
    if (tuto  == NULL)
        errx(EXIT_FAILURE, "fail find tuto ttf");
    text_color.r = 11;
    text_color.g = 36;
    text_color.b = 189;
    
    SDL_Surface* line1Text = 
        TTF_RenderText_Solid(tuto, 
            "1 - Click on the start button to go to the next page.", 
            text_color); 
    SDL_Texture* line1 = 
        SDL_CreateTextureFromSurface(*renderer, line1Text);
    
    SDL_Rect line1_rect = {(WIDTH - line1Text->w) / 2, 470, line1Text->w,
         line1Text->h};
    SDL_RenderCopy(*renderer, line1, NULL, &line1_rect);
    SDL_RenderPresent(*renderer);

    SDL_Surface* line2Text = 
        TTF_RenderText_Solid(tuto, 
            "2 - Select the image you want to resolve out of all the pictures.", 
            text_color); 
    SDL_Texture* line2 = 
        SDL_CreateTextureFromSurface(*renderer, line2Text);
    
    SDL_Rect line2_rect = {(WIDTH - line2Text->w) / 2, 475 + line1Text->h, 
        line2Text->w, line2Text->h};
    SDL_RenderCopy(*renderer, line2, NULL, &line2_rect);

    SDL_Surface* line3Text = 
        TTF_RenderText_Solid(tuto, 
            "3 - Let the program do the rest and see the result ! ", 
            text_color); 
    SDL_Texture* line3 = 
        SDL_CreateTextureFromSurface(*renderer, line3Text);
    
    SDL_Rect line3_rect = {(WIDTH - line3Text->w) / 2, 480 + line1Text->h + 
        line2Text->h, line3Text->w, line3Text->h};
    SDL_RenderCopy(*renderer, line3, NULL, &line3_rect);
    SDL_RenderPresent(*renderer);

    

    SDL_FreeSurface(line1Text);
    SDL_DestroyTexture(line1);
    SDL_FreeSurface(line2Text);
    SDL_DestroyTexture(line2);
    SDL_FreeSurface(line3Text);
    SDL_DestroyTexture(line3);
    TTF_CloseFont(tuto);

    //-----------------------create start button-------------------------------

    SDL_Texture* buttonTexture = IMG_LoadTexture(*renderer, 
        "elements/button.png");
    if (buttonTexture == NULL)
        errx(EXIT_FAILURE, "fail load button");

    TTF_Font* start_font = TTF_OpenFont("elements/tittle.TTF", 60);
    if (start_font  == NULL)
        errx(EXIT_FAILURE, "fail find start ttf");
    text_color.r = 255;
    text_color.g = 255;
    text_color.b = 255;
    
    SDL_Surface* startSurf = TTF_RenderText_Solid(start_font, "START", 
        text_color); 
    SDL_Texture* startText = 
        SDL_CreateTextureFromSurface(*renderer, startSurf);

    SDL_Rect buttonRect = {(WIDTH - startSurf->w*2)/2 , 160, startSurf->w *2, 
        startSurf->h *5};
    SDL_RenderCopy(*renderer, buttonTexture, NULL, &buttonRect);
    SDL_Rect textRect = {
        (WIDTH - startSurf->w) / 2 + 10,
        buttonRect.y + (buttonRect.h - startSurf->h) / 2,
        startSurf->w,
        startSurf->h
    };
    SDL_RenderCopy(*renderer, startText, NULL, &textRect);
    SDL_RenderPresent(*renderer);

    *button = buttonRect;

    SDL_DestroyTexture(buttonTexture);
    SDL_DestroyTexture(startText);
    TTF_CloseFont(start_font);

}

void clean_everything(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int menu_event_handler(SDL_Window **window, SDL_Rect *button)
{
    SDL_Event event;
    char windowed = 0;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT :
                return 0;
            case SDL_WINDOWEVENT :
                if (windowed)
                    break;
                switch (event.window.event) 
                {
                    /*case SDL_WINDOWEVENT_MAXIMIZED :
                        windowed = 1;
                        SDL_SetWindowFullscreen(*window, 
                            SDL_WINDOW_FULLSCREEN_DESKTOP);
                        windowed = 0;
                        break;
                    case SDL_WINDOWEVENT_RESTORED :
                        windowed = 1; 
                        SDL_SetWindowFullscreen(*window, 0);  
                        windowed = 0;
                        break;*/
                }
                break;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mx = event.button.x;
                    int my = event.button.y;
                    if (isInside(*button, mx, my)) 
                        return 2;
                }
                break;
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