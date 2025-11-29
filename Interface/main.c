#include "menu.h"
#include "training.h"
#include <stdlib.h>

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "SDL_Init failed");
    if (TTF_Init() < 0)
        errx(EXIT_FAILURE, "TTF_Init failed");

    // Création UNIQUE de la fenêtre
    SDL_Window *window = SDL_CreateWindow(
        "interface menu",
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
            // DESSIN DU MENU (pas de nouvelle fenêtre)
            menu_initialize(&window, &renderer, &training_button, &resolve_button);
            state = menu_event_handler(&window, &training_button, &resolve_button);
        }
        else if (state == 3)
        {
            // PAGE TRAINING dans la MÊME fenêtre
            training_screen(window, renderer);
            state = 1;     // retour au menu
        }
        else
        {
            // autres états, au cas où
            state = 0;
        }
    }

    clean_everything(window, renderer);
    return EXIT_SUCCESS;
}