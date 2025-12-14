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


void restart_self(char **argv)
{
    execv(argv[0], argv);
    _exit(1);
}

int main(int argc, char **argv)
{
    (void)argc;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "SDL_Init failed");
    if (TTF_Init() < 0)
        errx(EXIT_FAILURE, "TTF_Init failed");

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

    SDL_Rect training_button;
    SDL_Rect resolve_button;

    int state = 1;

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
            int r = training_screen(window, &renderer);
            if (r == -1)
                state = 0;
            else
                state = 1;
        }
        else if (state == 2)
            break;
    }

    if (state == 0)
    {
        clean_everything(window, renderer);
        return EXIT_SUCCESS;
    }

    SDL_Texture *texture = NULL;

    size_t nImages = count_files();
    size_t nred = nImages;
    if (nImages > 10)
        nred = 10;
    ImageItem *images = calloc(nred, sizeof(ImageItem));
    start_initialize(window, renderer, &texture, images);

    int running = nImages + 1;
    int show_image = 0;
    int selected = -1;

    SDL_Surface *img = NULL;
    while (running != -1)
    {
        if (show_image == 0)
        {
            start_redraw(renderer, images, nImages);
            running = start_event_handler(window, images, nred);

            if (running >= 1 && running <= (int)nred)
            {
                selected = running - 1;
                show_image = 1;
                texture = loadChoice(images[selected].name, window, renderer, &img);
                SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
                break;
            }
        }
    }

    if (running == -1)
    {
        start_clear(window, renderer, texture, nImages, images);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        restart_self(argv);
    }

    SDL_Surface *converted = SDL_ConvertSurfaceFormat(img,
        SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(img);
    img = converted;

    binarize(img);
    double angle = detect_rotation_angle_projection(img, 1);
    rotate_image(&img, angle);

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, img->w, img->h);
    if (!texture)
        errx(EXIT_FAILURE, "failed to create targetable texture");

    SDL_Texture *tmp_tex = SDL_CreateTextureFromSurface(renderer, img);
    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderCopy(renderer, tmp_tex, NULL, NULL);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_DestroyTexture(tmp_tex);

    TTF_Font *small = TTF_OpenFont("INTERFACE/elements/tittle.TTF", 15);

    SDL_Event ev;
    int done = 0;
    int wait = 1;
    int back_to_menu = 0;
    struct grid *grid_struct = NULL;
    struct words *words = NULL;

    while (wait)
    {
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
                wait = 0;

            if (done && ev.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = ev.button.x;
                int y = ev.button.y;
                if (x >= 20 && x <= 140 &&
                    y >= HEIGHT - 80 && y <= HEIGHT - 5)
                {
                    back_to_menu = 1;
                    wait = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 236, 224, 197, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        if (!done)
        {
            image_splitting(img, &grid_struct, &words);

            FILE *fp = fopen("solver_grid.txt", "w");
            if (!fp)
                errx(EXIT_FAILURE, "fail open file for grid");

            load("AI_OCR/weights.txt");
            for (size_t i = 0; i < grid_struct->size / grid_struct->cols; i++)
            {
                for (size_t j = 0; j < grid_struct->cols; j++)
                {
                    char c = get_character(
                        grid_struct->letters[i * grid_struct->cols + j].path);
                    fprintf(fp, "%c", c);
                }
                fprintf(fp, "\n");
            }
            fclose(fp);

            for (size_t i = 0; i < words->words_count; i++)
            {
                for (size_t j = 0; j < words->words_sizes[i]; j++)
                {
                    char c = get_character(words->paths[i][j]);
                    words->words[i][j] = c;
                }
                words->words[i][words->words_sizes[i]] = 0;
            }

            struct solverResult *results =
                calloc(words->words_count, sizeof(struct solverResult));
            for (size_t i = 0; i < words->words_count; i++)
            {
                results[i].x0 = calloc(1, sizeof(int));
                results[i].y0 = calloc(1, sizeof(int));
                results[i].x1 = calloc(1, sizeof(int));
                results[i].y1 = calloc(1, sizeof(int));
            }

            loadGrid("solver_grid.txt");

            for (size_t i = 0; i < words->words_count; i++)
            {
                struct solverResult curr = results[i];
                if (!searchWord(words->words[i],
                        curr.x0, curr.y0, curr.x1, curr.y1))
                    *curr.x0 = -1;
            }

            for (size_t i = 0; i < words->words_count; i++)
            {
                if (*(results[i].x0) == -1)
                    continue;

                int i1 = (*(results[i].y0) * grid_struct->cols)
                         + (*(results[i].x0));
                int i2 = (*(results[i].y1) * grid_struct->cols)
                         + (*(results[i].x1));

                int xi = grid_struct->letters[i1].center.center_x;
                int yi = grid_struct->letters[i1].center.center_y;
                int xf = grid_struct->letters[i2].center.center_x;
                int yf = grid_struct->letters[i2].center.center_y;

                SDL_SetRenderTarget(renderer, texture);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLine(renderer, xi, yi, xf, yf);
                SDL_RenderDrawLine(renderer, xi + 1, yi + 1, xf + 1, yf + 1);
                SDL_SetRenderTarget(renderer, NULL);
            }

            done = 1;
        }
        else
        {
            SDL_Texture *buttonTex =
                IMG_LoadTexture(renderer, "INTERFACE/elements/button.png");
            SDL_Surface *stopSurf =
                TTF_RenderText_Solid(small, "BACK",
                    (SDL_Color){255, 255, 255, 255});
            SDL_Texture *stopTxt =
                SDL_CreateTextureFromSurface(renderer, stopSurf);

            SDL_Rect stopRect = {20, HEIGHT - 80, 120, 75};
            SDL_Rect stopTxtRect = {
                stopRect.x + (stopRect.w - stopSurf->w) / 2 + 1,
                stopRect.y + (stopRect.h - stopSurf->h) / 2 - 4,
                stopSurf->w,
                stopSurf->h};

            SDL_RenderCopy(renderer, buttonTex, NULL, &stopRect);
            SDL_RenderCopy(renderer, stopTxt, NULL, &stopTxtRect);
            SDL_RenderPresent(renderer);

            SDL_DestroyTexture(buttonTex);
            SDL_DestroyTexture(stopTxt);
            SDL_FreeSurface(stopSurf);
        }
    }

    if (back_to_menu)
    {
        start_clear(window, renderer, texture, nImages, images);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        restart_self(argv);
    }

    clean_everything(window, renderer);
    return EXIT_SUCCESS;
}
