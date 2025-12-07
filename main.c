#include "INTERFACE/menu.h"
#include "INTERFACE/training.h"
#include "INTERFACE/resolve_window.h"
#include "READ_GRID/init/Init.h"
#include "READ_GRID/get_letters/get_letters.h"
#include "SOLVER/Solver.h"
#include "AI_OCR/training.h"
#include "AI_OCR/extract_data.h"
#include "AI_OCR/dict.h"
#include <unistd.h>

//ce code la a un probleme avec la premiere grid ou il lui manque 10 lettres et une fois sur 2 i fait de la merde autre part
int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "SDL_Init failed");
    if (TTF_Init() < 0)
        errx(EXIT_FAILURE, "TTF_Init failed");

    //-------------------------window's creation-------------------------------

    SDL_Window *window = SDL_CreateWindow(
        "",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window)
        errx(EXIT_FAILURE, "SDL_CreateWindow failed");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!renderer)
        errx(EXIT_FAILURE, "SDL_CreateRenderer failed");

    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    //-------------------------menu and training-------------------------------

    SDL_Rect training_button;
    SDL_Rect resolve_button;

    int state = 1;   // 1 = menu, 3 = training, 0 = quit

    while (state != 0)
    {
        if (state == 1)
        {
            menu_initialize(&window, &renderer, &training_button, 
                &resolve_button);
            state = menu_event_handler(&window, &training_button, 
                &resolve_button);
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
        
    //----------------------------resolve window-------------------------------

    SDL_Texture *texture = NULL;

    ImageItem *images = calloc(6, sizeof(ImageItem));
    start_initialize(window, renderer, &texture, images);

    int running = 7;
    int show_image = 0;   
    int selected = -1;

    SDL_Surface *img = NULL;
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
                texture = loadChoice(images[selected].name, window, renderer,
                     &img);
                SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255);
                SDL_RenderClear(renderer);

                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);

                break;
            }
        }
        /*else if (show_image == 1)
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
        }*/
    }

    if (running == -1)
    {
        start_clear(window, renderer, texture);
        return EXIT_SUCCESS;
    }

    //sleep(2);

    //--------------------------start pretreatement---------------------------- 

    char pict_path[200];
    sprintf(pict_path, "../READ_GRID/images/%s.png", (images[selected].name));

    SDL_Surface *converted = SDL_ConvertSurfaceFormat(img, 
        SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(img);
	img = converted;

    binarize(img);
    double angle = detect_rotation_angle_projection(img, 1);
	rotate_image(&img, angle);

	//-------------------display picture after pretreatement-------------------

    // SDL_Texture *texture_target = SDL_CreateTexture(renderer, 
    //     SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, img->w, img->h);
    // if (!texture_target)
    //     errx(EXIT_FAILURE, "failed to create targetable texture");
    
    // SDL_Texture *tmp_tex = SDL_CreateTextureFromSurface(renderer, img);
    // SDL_SetRenderTarget(renderer, texture_target);
    // SDL_RenderCopy(renderer, tmp_tex, NULL, NULL);
    // SDL_SetRenderTarget(renderer, NULL);
    
    // SDL_DestroyTexture(tmp_tex);
    
    // texture = texture_target;
    // SDL_RenderCopy(renderer, texture, NULL, NULL);
    // SDL_RenderPresent(renderer);

    texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, img->w, img->h);
    if (!texture) errx(EXIT_FAILURE, "failed to create targetable texture");

    SDL_Texture *tmp_tex = SDL_CreateTextureFromSurface(renderer, img);


    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderCopy(renderer, tmp_tex, NULL, NULL);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_DestroyTexture(tmp_tex);
    
    //sleep(5);
    SDL_Event ev;
    int done = 0;
    int wait = 1;
    struct grid *grid_struct = NULL;
    struct words *words = NULL;
    while (wait)
    {
        
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
                wait = 0;
        }
        SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        
        if (!done)
        {
            //--------------------------image splitting--------------------------------

            

            image_splitting(img, &grid_struct, &words);

            //printf("grid size : %zu\n", grid_struct->size);

            //--------------------------call of AI-------------------------------------

            //creation of the solver's grid

            FILE *fp = fopen("solver_grid.txt", "w");
            if (fp == NULL)
                errx(EXIT_FAILURE, "fail open file for grid");

            load("AI_OCR/weights.txt");
            for(size_t i = 0; i < grid_struct->size/grid_struct->cols; i++)
            {
                for (size_t j = 0; j < grid_struct->cols; j++)
                { 
                    char c = get_character(grid_struct->
                        letters[i*grid_struct->cols + j].path);
                    fprintf(fp, "%c", c);
                }
                fprintf(fp,"\n");
            }
            fclose(fp);

            //creation of array of words


            for(size_t i = 0; i < words->words_count; i++)
            {
                for(size_t j = 0; j < words->words_sizes[i];  j++)
                {
                    char c = get_character(words->paths[i][j]);
                    words->words[i][j] = c;
                }
                words->words[i][words->words_sizes[i]] = 0;
            }

            //display words
            /*for(size_t i = 0; i < words->words_count; i ++)
                printf("word n%zu : %s\n", i, words->words[i]);*/

            //-------------------------call of solver----------------------------------

            //initialize struct result 

            struct solverResult *results = calloc(words->words_count, 
                sizeof(struct solverResult));
            if (results == NULL)
                errx(EXIT_FAILURE, "fail calloc rresults");
            for(size_t i = 0; i < words->words_count; i++)
            {
                results[i].x0 = calloc(1, sizeof(int));
                results[i].y0 = calloc(1, sizeof(int));
                results[i].x1 = calloc(1, sizeof(int));
                results[i].y1 = calloc(1, sizeof(int));
            }

            //call solver's functions 

            if (!loadGrid("solver_grid.txt")) 
                errx(EXIT_FAILURE, "cannot load grid for solver");

            for(size_t i = 0;i < words->words_count; i++)
            {
                struct solverResult curr = results[i];
                if(!searchWord(words->words[i], curr.x0, curr.y0, curr.x1, curr.y1))
                {
                    printf("word %s not found\n", words->words[i]);
                    *curr.x0 = -1;
                }
                else
                    printf("curr.x0 : %i, curr.y0 %i, curr.x1 %i, curr.y1 %i\n", *curr.x0, *curr.y0, *curr.x1, *curr.y1);
            }
            for(size_t i = 0; i < words->words_count; i++)
            {
                if(*(results[i].x0) == -1)
                    continue;
                int i1 = (*(results[i].y0)*grid_struct->cols)+(*(results[i].x0));
                int i2 = (*(results[i].y1)*grid_struct->cols)+(*(results[i].x1));
                
                int xi = grid_struct->letters[i1].center.center_x;
                int yi = grid_struct->letters[i1].center.center_y;
                int xf = grid_struct->letters[i2].center.center_x;
                int yf = grid_struct->letters[i2].center.center_y;
                
                if(SDL_SetRenderTarget(renderer, texture) < 0)
                    errx(EXIT_FAILURE, "echec renderer_target");
                
                if(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255) < 0)
                    errx(EXIT_FAILURE, "echec color");
                
                if (SDL_RenderDrawLine(renderer,xi, yi, xf, yf) < 0)
                    errx(EXIT_FAILURE, "echec drawing");
                if (SDL_RenderDrawLine(renderer,xi+1, yi+1, xf+1, yf+1) < 0)
                    errx(EXIT_FAILURE, "echec drawing");
                
                if(SDL_SetRenderTarget(renderer, NULL) < 0)
                    errx(EXIT_FAILURE, "echec targetr null");
                
                if(SDL_RenderCopy(renderer, texture, NULL, NULL)< 0)
                    errx(EXIT_FAILURE, "echec renderer copy");
            }
            done = 1;
            // if (SDL_SaveBMP(img, "output.bmp") < 0)
            //     errx(EXIT_FAILURE, "echec save");
            int w, h;
            SDL_GetRendererOutputSize(renderer, &w, &h);
            SDL_Surface *final = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32,
                 SDL_PIXELFORMAT_RGBA32);
            SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32,
                final->pixels, final->pitch);
            IMG_SavePNG(final, "result.png");
            SDL_FreeSurface(final);
            for(size_t i = 0; i < words->words_count; i++)
            {
                free(results[i].x0);
                free(results[i].y0);
                free(results[i].x1);
                free(results[i].y1);
            }
            free(results);

        }
    }


    //free
    for (size_t i = 0; i < grid_struct->size; i++)
        free(grid_struct->letters[i].path);
    free(grid_struct->letters);
    free(grid_struct);
    for(size_t i = 0; i < words->words_count; i++)
    {
        for (size_t j = 0; j < words->words_sizes[i]; j++)
            free(words->paths[i][j]);
        free(words->paths[i]);
        free(words->words[i]);
    }
    free(words->words);
    free(words->paths);
    free(words->words_sizes);
    free(words);
    start_clear(window, renderer, texture);
    return EXIT_SUCCESS;





    //to keep displaying the picture loaded

    /*SDL_Event ev;
    int wait = 1;
    while (wait)
    {
        while (SDL_PollEvent(&ev))
        {
            SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255);
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            if (ev.type == SDL_QUIT)
                wait = 0;
            
        }
    }*/


    
}

