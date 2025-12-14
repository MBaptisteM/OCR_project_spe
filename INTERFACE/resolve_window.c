#include "resolve_window.h"

#define WIDTH 1000
#define HEIGHT 600

size_t count_files()
{
    size_t res = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir("READ_GRID/images");
    if (d == NULL) 
        errx(EXIT_FAILURE, "fail open dir");
    while ((dir = readdir(d)) != NULL)
    {
        if (dir->d_name[0] != '.')
            res++;
    } 
    closedir(d);
    return res;
}
void start_initialize(SDL_Window *window, SDL_Renderer *renderer, 
    SDL_Texture **texture, ImageItem *images)
{
    (void)window;
    (void)texture;

    //------------------------clean past renderer------------------------------
    
    SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255); //bg color
    SDL_RenderClear(renderer);

    //------------------------get all pictures data----------------------------

    size_t n = count_files();
    if (n > 10)
        n = 10;

    int cols = 0;
    int rows = 0;
    int imp = 0;
    int imgW = 250;
    int imgH = 200;
    switch(n)
    {
        case 1:
            rows = 1;
            cols = 1;
            imgW = 450;
            imgH = 450;
            break;
        case 2:
            rows = 1;
            cols = 2;
            imgW = 350;
            imgH = 300;
            break;
        case 3:
            rows = 1;
            cols = 3;
            break;
        case 4:
            rows = 2;
            cols = 2;
            break;
        case 5:
            rows = 2;
            cols = 3;
            imp = 1;
            break;
        case 6:
            rows = 2;
            cols = 3;
            break;
        case 7:
            rows = 2;
            cols = 4;
            imp = 1;
            imgW = 200;
            imgH = 150;
            break;
        case 8:
            rows = 2;
            cols = 4;
            imgW = 200;
            imgH = 150;
            break;
        case 9:
            rows = 2;
            cols = 5;
            imp = 1;
            imgW = 150;
            imgH = 100;
            break;
        default:
            rows = 2;
            cols = 5;
            imgW = 150;
            imgH = 100;
            break;
    }

    char **imgNames = malloc(n * sizeof(char *));
    char **imgPaths = malloc(n * sizeof(char *));
    if (!imgNames || !imgPaths)
        errx(EXIT_FAILURE, "malloc failed");

    DIR *d;
    struct dirent *dir;
    d = opendir("READ_GRID/images");
    if (d == NULL) 
        errx(EXIT_FAILURE, "fail open dir");
    size_t i = 0;
    while ((dir = readdir(d)) != NULL && i < n)
    {
        if (dir->d_name[0] == '.')
            continue;
        char path[300];
        char *name = dir->d_name;
        snprintf(path, sizeof(path), "READ_GRID/images/%s", name);
        imgNames[i] = malloc(strlen(name) + 1);
        imgPaths[i] = malloc(strlen(path) + 1);
        if (!imgNames[i] || !imgPaths[i])
            errx(EXIT_FAILURE, "malloc img i failed");
        strcpy(imgPaths[i], path);
        strcpy(imgNames[i], name);
        //printf("path %zu : %s, name %s\n", i, imgPaths[i], imgNames[i]);
        i++;
    } 
    closedir(d);

    int margin = 50;
    int spacing = 0;
    if (cols > 1)
    {
        spacing = (WIDTH - 2 * margin - cols * imgW) / (cols - 1);
        if (spacing < 0)
            spacing = 0;
    }
    for (size_t i = 0; i < n; i++) 
    {
        SDL_Surface* surf = IMG_Load(imgPaths[i]);
        if (surf == NULL)
            errx(EXIT_FAILURE, "fail load image of a grid");
        images[i].texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        int curr_row = i/cols;
        int curr_col = i % cols;
        if (rows == 1)
        {
            if (cols != 1)
                images[i].rect.y = 180;
            else
                {
                    images[i].rect.y = 100;
                    images[i].rect.x = 300;
                }
        }  
        else 
            images[i].rect.y = 120 + curr_row * (imgH + 50);
        if ((int)i >= cols && imp)
            images[i].rect.x = imgW/2 +margin + curr_col * (imgW + spacing);
        else if (cols != 1)
            images[i].rect.x = margin + curr_col * (imgW + spacing);
        
        images[i].rect.w = imgW;
        images[i].rect.h = imgH;
        images[i].name = imgNames[i];
    }

    for (size_t i = 0; i < n; i++)
        free(imgPaths[i]);
    free(imgPaths);

}
void start_redraw(SDL_Renderer *renderer, ImageItem *images, size_t n)
{
    size_t nred = n;
    if (n > 10)
        nred = 10;
    //------------------------clean past renderer------------------------------
    
    SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255); //bg color
    SDL_RenderClear(renderer);

    //--------------------------create tittle----------------------------------
    
    TTF_Font* tittle = TTF_OpenFont("INTERFACE/elements/tittle.TTF", 50);
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
    //TTF_CloseFont(tittle);
    //SDL_RenderPresent(renderer);

    //----------------------------add warning----------------------------------
    if (n > 10)
    {
        TTF_Font* fontWarning = TTF_OpenFont("INTERFACE/elements/text.TTF", 20);
        if (fontWarning  == NULL)
            errx(EXIT_FAILURE, "fail find tittle ttf");
        text_color.r = 11;
        text_color.g = 36;
        text_color.b = 189;

        SDL_Surface* warningSurf = TTF_RenderText_Solid(fontWarning, 
            "Be ecareful ! Only the first ten pictures are displayed", text_color); 
        SDL_Texture* warning = 
            SDL_CreateTextureFromSurface(renderer, warningSurf);
        
        SDL_Rect warningRect = {(WIDTH-warningSurf->w)/2, 450, 
            warningSurf->w, warningSurf->h};
        SDL_RenderCopy(renderer, warning, NULL, &warningRect);
        TTF_CloseFont(fontWarning);
    }
    //-------------------------display pictures names--------------------------

    SDL_FreeSurface(instructionSurf);
    SDL_DestroyTexture(instruction);
    TTF_CloseFont(tittle);

    TTF_Font* nameFont = TTF_OpenFont("INTERFACE/elements/text.TTF", 15);
    if (nameFont  == NULL)
        errx(EXIT_FAILURE, "fail find tittle ttf");
    text_color.r = 11;
    text_color.g = 36;
    text_color.b = 189;
    for (size_t i = 0; i < nred; i++)
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
    SDL_Texture *texture, size_t n, ImageItem *images)
{
    (void)n;
    (void)images;
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

int start_event_handler(SDL_Window *window, ImageItem *images, size_t n)
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
                    size_t picture = isInsidePicture(images, mx, my, n);
                    if (picture != n+1) 
                    {
                        return picture;
                    }
                }
                break;
        }
    }
    return n+1;
}
SDL_Texture *loadChoice(const char* name, SDL_Window *window, 
    SDL_Renderer *renderer, SDL_Surface **image) 
{
    (void)window;
    char path[200];
    sprintf(path, "READ_GRID/images/%s", name);

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
size_t isInsidePicture(ImageItem *images, int x, int y, size_t n) 
{
    for (size_t i = 0; i < n; i++)
    {
        if(x >= images[i].rect.x &&
            x <= images[i].rect.x + images[i].rect.w &&
            y >= images[i].rect.y &&
            y <= images[i].rect.y + images[i].rect.h)
            return i+1;
    }
    return n+1;
}