#include "menu.h"
#include "training.h"
#include "resolve_window.h"
#include <stdlib.h>

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "SDL_Init failed");
    if (TTF_Init() < 0)
        errx(EXIT_FAILURE, "TTF_Init failed");

    // window's creation
    SDL_Window *window = SDL_CreateWindow(
        "",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window)
        errx(EXIT_FAILURE, "SDL_CreateWindow failed");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        errx(EXIT_FAILURE, "SDL_CreateRenderer failed");

    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    SDL_Rect training_button;
    SDL_Rect resolve_button;

    int state = 1;   // 1 = menu, 3 = training, 0 = quit

    while (state != 0)
    {
        if (state == 1)
        {
            menu_initialize(&window, &renderer, &training_button, &resolve_button);
            state = menu_event_handler(&window, &training_button, &resolve_button);
        }
        else if (state == 3)
        {
            // training window
            int r = training_screen(window, &renderer);
            if(r == -1)
                state = 0;
            else
                state = 1;     // back to menu
        }
        else if (state == 2)
        {
            break;
        }
    }
    
    if (state == 0)
    {
        clean_everything(window, renderer);
        return EXIT_SUCCESS;
    }
        

    SDL_Texture *texture = NULL;
    ImageItem *images = calloc(6, sizeof(ImageItem));
    start_initialize(window, renderer, &texture, images);

    int running = 7;
    int show_image = 0;   
    int selected = -1;

    while (running != -1)
    {
        if (show_image == 0)
        {
            start_redraw(renderer, images);
            running = start_event_handler(window, images);

            if (running >= 1 && running <= 6)
            {
                selected = running - 1;
                show_image = 1; 
                texture = loadChoice(images[selected].name, window, renderer);
            }
        }
        else if (show_image == 1)
        {
            SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255);
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Event ev;
            while (SDL_PollEvent(&ev))
            {
                if (ev.type == SDL_QUIT)
                    running = -1;
            }
        }
    }


    
    if (running == -1)
    {
        start_clear(window, renderer, texture);
    }
    return EXIT_SUCCESS;
}