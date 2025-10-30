#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "Pixel.h"


//binarize function
void binarize(SDL_Surface *img) {
    Uint32 pixel;
    Uint8 r, g, b;

    // Loop through each row of the image
    for (int y = 0; y < img->h; y++) {
        // Loop through each column of the image
        for (int x = 0; x < img->w; x++) {
            // Get the pixel value at position (x, y)
            pixel = getPixel(img, x, y);
            // Extract the r,g,b from the pixel
            SDL_GetRGB(pixel, img->format, &r, &g, &b);

            // Convert the pixel to grayscale using a luminance formula
            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

            // General darkening step:
            // Reduce brightness slightly before further processing to make the image overall darker
            int darkened = gray - 25; // decrease overall brightness
            if (darkened < 0)
                darkened = 0; // clamp to minimum 0 to avoid error
            gray = (Uint8)darkened;

            // Enhance contrast and apply gentle darkening
            if (gray < 40)
                gray = 0; // deep black for very dark pixels
            else if (gray > 230)
                gray = 255; // pure white for very bright pixels
            else {
                // Moderate contrast adjustment:
                // Scale the difference from mid-gray (128) by a contrast factor
                float contrast = 3.0f;
                float new_gray = (gray - 128) * contrast + 128;

                // Slightly lower brightness to darken midtones further
                new_gray -= 20;

                // Clamp the adjusted grayscale value to valid range [0, 255] to avoid errors
                if (new_gray < 0) new_gray = 0;
                if (new_gray > 255) new_gray = 255;

                // Assign the adjusted grayscale value back to gray
                gray = (Uint8)new_gray;
            }
            if (r > 213 && g > 213 && b > 213) {
                pixel = SDL_MapRGB(img->format, gray, gray, gray);
                setPixel(img, x, y, pixel);
                continue;
            }

            int darkened2 = gray - 200; // decrease overall brightness
            if (darkened2 < 0)
                darkened2 = 0; // clamp to minimum 0 to avoid error
            gray = (Uint8)darkened2;

            // Set to a real pixel
            pixel = SDL_MapRGB(img->format, gray, gray, gray);
            // Set the modified pixel back into the image at position (x, y)
            setPixel(img, x, y, pixel);

        }
    }
}
// rotate_image function
void rotate_image(SDL_Surface **img, double angle_degrees) {
    if (!img || !*img) return;

    SDL_Surface *src = *img;

    // Convert degrees to radians for trigonometric functions
    double angle = angle_degrees * M_PI / 180.0;

    int w = src->w;
    int h = src->h;

    // Calculate new width and height after rotation
    int new_w = (int)(fabs(w * cos(angle)) + fabs(h * sin(angle)));
    int new_h = (int)(fabs(w * sin(angle)) + fabs(h * cos(angle)));

    // Create a new surface to store the rotated image
    SDL_Surface *rotated = SDL_CreateRGBSurface(
        0, new_w, new_h, src->format->BitsPerPixel,
        src->format->Rmask, src->format->Gmask,
        src->format->Bmask, src->format->Amask
    );

    if (!rotated) {
        printf("Error creating rotated surface\n");
        return;
    }

    // Centers of the original and rotated images
    double cx = w / 2.0;
    double cy = h / 2.0;
    double ncx = new_w / 2.0;
    double ncy = new_h / 2.0;

    SDL_LockSurface(src);
    SDL_LockSurface(rotated);

    // Loop through each pixel in the new (rotated) image
    for (int y = 0; y < new_h; y++) {
        for (int x = 0; x < new_w; x++) {
            // Calculate inverse mapping coordinates
            double dx = x - ncx;
            double dy = y - ncy;

            // Apply inverse rotation to find the corresponding source pixel
            double src_x = cos(angle) * dx + sin(angle) * dy + cx;
            double src_y = -sin(angle) * dx + cos(angle) * dy + cy;

            // If the mapped source coordinates are inside the original image
            if (src_x >= 0 && src_x < w && src_y >= 0 && src_y < h) {
                // Get the pixel color from the source image
                Uint32 pixel = getPixel(src, (int)src_x, (int)src_y);

                // Write the pixel to the new (rotated) surface
                setPixel(rotated, x, y, pixel);
            } else {
                // Outside the original image → fill with black
                Uint32 black = SDL_MapRGB(rotated->format, 0, 0, 0);
                setPixel(rotated, x, y, black);
            }
        }
    }

    SDL_UnlockSurface(src);
    SDL_UnlockSurface(rotated);

    // Replace the original surface with the rotated one
    SDL_FreeSurface(*img);
    *img = rotated;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Error args>\n");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_Surface *img = IMG_Load(argv[1]);

	if (!img) {
		printf("Error while loading picture\n");
		return 1;
	}


	SDL_Surface *converted = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGB888, 0);
	SDL_FreeSurface(img);
	img = converted;

	binarize(img);
	rotate_image(&img, atoi(argv[2]));

	//IMG_SavePNG(img, "output.png");
	// SDL_Window *window = SDL_CreateWindow("Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, img->w, img->h, 0);
	// SDL_Surface *window_surface = SDL_GetWindowSurface(window);
	// SDL_BlitSurface(img, NULL, window_surface, NULL);
	// SDL_UpdateWindowSurface(window);
	//
	// SDL_DestroyWindow(window);
	//
	// SDL_FreeSurface(img);
	// IMG_Quit();
	// SDL_Quit();

	return 0;
}