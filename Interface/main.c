#include "menu.h"

int main() 
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Rect button;
    menu_initialize(&window, &renderer, &button);

    int running = 1;
    while (running == 1) 
    {
      running = menu_event_handler(&window, &button);
      SDL_RenderPresent(renderer);
    }
    if (running == 2)
    {
      clean_everything(window, renderer);
    }
      
    else if (running == 0)
      clean_everything(window, renderer);
    return EXIT_SUCCESS;
}