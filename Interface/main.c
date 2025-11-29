#include "menu.h"
#include "training.h"   
#include <stdlib.h>

int main()
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    SDL_Rect training_button;
    SDL_Rect resolve_button;

    menu_initialize(&window, &renderer, &training_button, &resolve_button);

    int running = 1;
    while (running == 1)
    {
        running = menu_event_handler(&window, &training_button, &resolve_button);
        SDL_RenderPresent(renderer);
    }

    if (running == 3)
    {
        training_screen(window, renderer);

        menu_initialize(&window, &renderer, &training_button, &resolve_button);
        running = 1;

        while (running == 1)
        {
            running = menu_event_handler(&window, &training_button, &resolve_button);
            SDL_RenderPresent(renderer);
        }
    }

    else if (running == 2)
    {x
        //mettre la fonction qui resouds 
    }

    clean_everything(window, renderer);
    return EXIT_SUCCESS;
}