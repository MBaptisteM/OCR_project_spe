#include "resolve_window.h"

#define WIDTH 1000
#define HEIGHT 600


void start_initialize(SDL_Window *window, SDL_Renderer *renderer, 
    SDL_Texture **texture, ImageItem *images)
{
    (void)window;
    (void)texture;

    //------------------------clean past renderer------------------------------
    
    SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255); //bg color
    SDL_RenderClear(renderer);

    //----------------------------load images----------------------------------

    const char* imgPaths[6] = {
        "../READ_GRID/images/lvl1_1.png", "../READ_GRID/images/lvl1_2.png", 
        "../READ_GRID/images/lvl2_1.png", "../READ_GRID/images/lvl2_2.png", 
        "../READ_GRID/images/lvl3_1.png", "../READ_GRID/images/lvl3_2.png"
    };

    const char* imgNames[6] = { 
        "lvl1_1", "lvl1_2", "lvl2_1", "lvl2_2", "lvl3_1", "lvl3_2"
    };

    int imgW = 250; 
    int imgH = 200; 
    int spacing = WIDTH/2 - (imgW/2 + imgW + 50);
    for (size_t i = 0; i < 6; i++) 
    {
        SDL_Surface* surf = IMG_Load(imgPaths[i]);
        if (surf == NULL)
            errx(EXIT_FAILURE, "fail load image of a grid");
        images[i].texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        int row = i / 3;
        int col = i % 3;
        images[i].rect.x = 50 + col * (imgW + spacing);
        images[i].rect.y = 120 + row * (imgH + 50);
        images[i].rect.w = imgW;
        images[i].rect.h = imgH;
        images[i].name = imgNames[i];
    }
}
void start_redraw(SDL_Renderer *renderer, ImageItem *images)
{
    //------------------------clean past renderer------------------------------
    
    SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255); //bg color
    SDL_RenderClear(renderer);

    //--------------------------create tittle----------------------------------
    
    TTF_Font* tittle = TTF_OpenFont("elements/tittle.TTF", 50);
    if (tittle  == NULL)
        errx(EXIT_FAILURE, "fail find tittle ttf");

    SDL_Color text_color = {74, 90, 238, 255};
    SDL_Surface* instructionSurf = 
        TTF_RenderText_Solid(tittle, "Choose your picture", text_color); 
    SDL_Texture* instruction = 
        SDL_CreateTextureFromSurface(renderer, instructionSurf);
    
    SDL_Rect instructionRect = {(WIDTH-instructionSurf->w)/2, 30, 
        instructionSurf->w, instructionSurf->h};
    SDL_RenderCopy(renderer, instruction, NULL, &instructionRect);
    //SDL_RenderPresent(renderer);

    //-------------------------display pictures names--------------------------

    SDL_FreeSurface(instructionSurf);
    SDL_DestroyTexture(instruction);
    TTF_CloseFont(tittle);

    TTF_Font* nameFont = TTF_OpenFont("elements/text.TTF", 15);
    if (nameFont  == NULL)
        errx(EXIT_FAILURE, "fail find tittle ttf");
    text_color.r = 11;
    text_color.g = 36;
    text_color.b = 189;
    for (size_t i = 0; i < 6; i++)
    {
        SDL_RenderCopy(renderer, images[i].texture, NULL, &images[i].rect);
        SDL_Surface* nameSurf = 
            TTF_RenderText_Solid(nameFont, images[i].name, text_color);
        SDL_Texture* nameTex = 
            SDL_CreateTextureFromSurface(renderer, nameSurf);
        SDL_Rect nameRect = {images[i].rect.x+(images[i].rect.w-nameSurf->w)/2,
                                images[i].rect.y + images[i].rect.h + 5,
                                nameSurf->w, nameSurf->h};
        SDL_FreeSurface(nameSurf);
        SDL_RenderCopy(renderer, nameTex, NULL, &nameRect);
        SDL_DestroyTexture(nameTex);
    }
    TTF_CloseFont(nameFont);
    SDL_RenderPresent(renderer);
}

void start_clear(SDL_Window *window, SDL_Renderer *renderer, 
    SDL_Texture *texture)
{
    if(texture)
        SDL_DestroyTexture(texture);
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int start_event_handler(SDL_Window *window, ImageItem *images)
{
    (void)window;
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT :
                return -1;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mx = event.button.x;
                    int my = event.button.y;
                    size_t picture = isInsidePicture(images, mx, my);
                    if (picture != 7) 
                    {
                        return picture;
                    }
                }
                break;
        }
    }
    return 7;
}
SDL_Texture *loadChoice(const char* name, SDL_Window *window, 
    SDL_Renderer *renderer, SDL_Surface **image) 
{
    (void)window;
    char path[200];
    sprintf(path, "../READ_GRID/images/%s.png", name);

    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "fail load selected picture in surface");
    *image = surface;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
        errx(EXIT_FAILURE, "fail create texture from surface");
    //SDL_FreeSurface(surface);
    
    return texture;

}
size_t isInsidePicture(ImageItem *images, int x, int y) 
{
    for (size_t i = 0; i < 6; i++)
    {
        if(x >= images[i].rect.x &&
            x <= images[i].rect.x + images[i].rect.w &&
            y >= images[i].rect.y &&
            y <= images[i].rect.y + images[i].rect.h)
            return i+1;
    }
    return 7;
}