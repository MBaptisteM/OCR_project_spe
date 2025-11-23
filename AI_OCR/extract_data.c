#include "extract_data.h"

//The goal of this code is to extract all the datas of an image and return an array


char* extract(char* path){
    IMG_Init(IMG_INIT_PNG);

    char stock[150];
    snprintf(stock, sizeof(stock), "%s", path);
    SDL_Surface *img = IMG_Load(stock);
    if (img == NULL)
        errx(EXIT_FAILURE, "fail surface with img");

    int w = img->w; //get picture dimensions
    int h = img->h;

    //creation and initialization of grayscale based on img pixels

    unsigned char **gray = calloc(h, sizeof(unsigned char*));
    if (gray == NULL)
        errx(EXIT_FAILURE, "fail calloc gray");
    for (int y = 0; y < h; y++)
    {
        gray[y] = calloc(w, sizeof(unsigned char));
        if (gray[y] == NULL)
            errx(EXIT_FAILURE, "fail calloc gray[y]");
    }

    

    Uint8 *p = (Uint8 *)img->pixels;
    int bpp = img->format->BytesPerPixel; //bytes per pixels

    char* tab = calloc(42 * 49, sizeof(char));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Uint8 r, g, b;
            Uint32 pixel;

            //access to the pixels depending on the bpp
            Uint8 *p_px = p + y * img->pitch + x * bpp;

            switch (bpp) {
                case 1:
                    pixel = *p_px;
                    SDL_GetRGB(pixel, img->format, &r, &g, &b);
                    break;

                case 2:
                    pixel = *(Uint16 *)p_px;
                    SDL_GetRGB(pixel, img->format, &r, &g, &b);
                    break;

                case 3:
                    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                        pixel = p_px[0] << 16 | p_px[1] << 8 | p_px[2];
                    else
                        pixel = p_px[0] | p_px[1] << 8 | p_px[2] << 16;
                    SDL_GetRGB(pixel, img->format, &r, &g, &b);
                    break;

                case 4:
                    pixel = *(Uint32 *)p_px;
                    SDL_GetRGB(pixel, img->format, &r, &g, &b);
                    break;
            }

            gray[y][x] = (Uint8)(0.299*r + 0.587*g + 0.114*b);
            if (gray[y][x] < 160)
                tab[y * 42 + x] = 1;
        }
    }

    free(gray);
    return tab;
}