#include "Init.h"

#define PI 3.14159265358979323846



//check the average light
float light(SDL_Surface *img) {
    Uint32 pixel;
    Uint8 r, g, b;
    double total = 0.0;

    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            pixel = getPixel(img, x, y);
            SDL_GetRGB(pixel, img->format, &r, &g, &b);

            total += 0.299 * r + 0.587 * g + 0.114 * b;
        }
    }
    return total / (img->w * img->h);
}

//check if the image is colored (to do not edit it too much)
int is_colored(SDL_Surface *img){
    Uint32 pixel;
    Uint8 r, g, b;
    int sample_count = 0;
    int color_pixels = 0;

    for (int y = 0; y < img->h; y += 5) {
        for (int x = 0; x < img->w; x += 5) {
            pixel = getPixel(img, x, y);
            SDL_GetRGB(pixel, img->format, &r, &g, &b);

            if (abs(r - g) > 10 || abs(r - b) > 10 || abs(g - b) > 10) {
                color_pixels++;
            }
            sample_count++;
        }
    }

    float ratio = (float)color_pixels / (float)sample_count;
    //better by tests
    return ratio > 0.02;
}

//clean noise (a bit)
void remove_isolated_black_pixels(SDL_Surface *img){
    SDL_Surface *temp = SDL_ConvertSurface(img, img->format, 0);
    Uint32 pixel;
    Uint8 r, g, b;

    for (int y = 1; y < img->h - 1; y++) {
        for (int x = 1; x < img->w - 1; x++) {
            pixel = getPixel(temp, x, y);
            SDL_GetRGB(pixel, temp->format, &r, &g, &b);

            if (r < 40 && g < 40 && b < 40) {
                int neighbor_count = 0;

                //check the dark pixels aroud
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) 
                            continue;

                        Uint32 npix = getPixel(temp, x + dx, y + dy);
                        Uint8 nr, ng, nb;
                        SDL_GetRGB(npix, temp->format, &nr, &ng, &nb);

                        if (nr < 40 && ng < 40 && nb < 40) 
                            neighbor_count++;
                    }
                }

                //if the pixel is alone, remove it
                if (neighbor_count == 0) {
                    Uint32 white = SDL_MapRGB(img->format, 255, 255, 255);
                    setPixel(img, x, y, white);
                }
            }
        }
    }

    SDL_FreeSurface(temp);
}

//expand dark
void dark_exp(SDL_Surface *img){
    SDL_Surface *temp = SDL_ConvertSurface(img, img->format, 0);
    Uint32 pixel;
    Uint8 r, g, b;

    for (int y = 0; y < temp->h; y++) {
        for (int x = 0; x < temp->w; x++) {
            pixel = getPixel(temp, x, y);
            SDL_GetRGB(pixel, temp->format, &r, &g, &b);

            if (r < 60 && g < 60 && b < 60) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx >= 0 && nx < img->w && ny >= 0 && ny < img->h) {
                            Uint32 black = SDL_MapRGB(img->format, 0, 0, 0);
                            setPixel(img, nx, ny, black);
                        }
                    }
                }
            }
        }
    }

    SDL_FreeSurface(temp);
}

//erode to have a cleaner image
void erode(SDL_Surface *img){
    SDL_Surface *temp = SDL_ConvertSurface(img, img->format, 0);
    Uint32 pixel;
    Uint8 r, g, b;

    for (int y = 1; y < temp->h - 1; y++) {
        for (int x = 1; x < temp->w - 1; x++) {
            pixel = getPixel(temp, x, y);
            SDL_GetRGB(pixel, temp->format, &r, &g, &b);

            if (r < 60 && g < 60 && b < 60) {
                int tot_b_n = 0;
                int ho_b_n = 0;
                int ve_b_n = 0;

                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        Uint32 npix = getPixel(temp, x + dx, y + dy);
                        Uint8 nr, ng, nb;
                        SDL_GetRGB(npix, temp->format, &nr, &ng, &nb);
                        if (nr < 60 && ng < 60 && nb < 60) {
                            tot_b_n++;
                            if (dx != 0 && dy == 0) ho_b_n++;
                            if (dx == 0 && dy != 0) ve_b_n++;
                        }
                    }
                }

                if ((ho_b_n == 0) ||
                    (ho_b_n == 1)) {
                    Uint32 white = SDL_MapRGB(img->format, 255, 255, 255);
                    setPixel(img, x, y, white);
                }
            }
        }
    }

    SDL_FreeSurface(temp);
}

//black an white
void binarize(SDL_Surface *img){
    Uint32 pixel;
    Uint8 r, g, b;

    //check if the image is colored
    int colored = is_colored(img);

    //check the light of the image
    float avgLum = light(img);

    //change the parameters depending on the light
    float contrast_factor;
    int darken_base;

    if (avgLum < 60) {
        contrast_factor = 1.0f;
        darken_base = 5;
    } else if (avgLum < 140) {
        contrast_factor = 2.0f;
        darken_base = 20;
    } else {
        contrast_factor = 3.0f;
        darken_base = 30;
    }

    //iterate throught each pixel
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            pixel = getPixel(img, x, y);
            SDL_GetRGB(pixel, img->format, &r, &g, &b);

            //convert to gray
            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);

            //case image is colored
            if (colored) {
                pixel = SDL_MapRGB(img->format, gray, gray, gray);
                setPixel(img, x, y, pixel);
                continue;
            }

            //case if the image need to be edited
            int darkened = gray - darken_base;
            if (darkened < 0) darkened = 0;
            gray = (Uint8)darkened;

            if (gray < 40)
                gray = 0;
            else if (gray > 230)
                gray = 255;
            else {
                float new_gray = (gray - 128) * contrast_factor + 128;
                new_gray -= 20;
                if (new_gray < 0) 
                    new_gray = 0;
                if (new_gray > 255) 
                    new_gray = 255;

                gray = (Uint8)new_gray;
            }

            if (r > 213 && g > 213 && b > 213) {
                pixel = SDL_MapRGB(img->format, gray, gray, gray);
                setPixel(img, x, y, pixel);
                continue;
            }

            int darkened2 = gray - 200;
            if (darkened2 < 0) darkened2 = 0;
            gray = (Uint8)darkened2;

            pixel = SDL_MapRGB(img->format, gray, gray, gray);
            setPixel(img, x, y, pixel);
        }
    }

    //clean
    if (colored == 0) {
        remove_isolated_black_pixels(img); // Remove noise
        dark_exp(img);
        erode(img);
    }
}


//old func
/*
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
            // Reduce brightness slightly before further processing to make the 
            // image overall darker
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
                //Scale the difference from mid-gray (128) by a contrast factor
                float contrast = 3.0f;
                float new_gray = (gray - 128) * contrast + 128;

                // Slightly lower brightness to darken midtones further
                new_gray -= 20;

                // Clamp the adjusted grayscale value to valid range [0, 255] 
                // to avoid errors
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
*/

// detect_rotation_angle_projection function
double detect_rotation_angle_projection(SDL_Surface *img, int step_deg) {
    // Return 0 if image is NULL
    if (!img) return 0.0;

    // Conversion factor from degrees to radians
    const double deg_to_rad = PI / 180.0;
    const int ds = 4;

    // Compute downsampled width and height
    int w = img->w / ds, h = img->h / ds;

    // Return 0 if image is too small
    if (w < 10 || h < 10) return 0.0;

    Uint8 r, g, b;
    Uint8 *bw = malloc(w * h);
    if (!bw) return 0.0;

    SDL_LockSurface(img);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Uint32 p = getPixel(img, x * ds, y * ds);
            SDL_GetRGB(p, img->format, &r, &g, &b);
            int gray = (r + g + b) / 3;
            bw[y * w + x] = (gray < 128) ? 1 : 0;
        }
    }
    SDL_UnlockSurface(img);

    double best_angle = 0.0;
    double best_score = -1.0;

    // Loop over angles from -45 to 45 degrees
    for (int a = -45; a <= 45; a += step_deg) {
        double rad = a * deg_to_rad;
        double sin_a = sin(rad), cos_a = cos(rad);

        // Loop over angles from -45 to 45 degrees
        int nbins = w + h;
        int *proj = calloc(nbins, sizeof(int));
        if (!proj) { free(bw); return 0.0; }

        // Loop over angles from -45 to 45 degrees
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (!bw[y * w + x]) continue;
                int p = (int)(x * sin_a + y * cos_a + nbins / 2);
                if (p >= 0 && p < nbins) proj[p]++;
            }
        }

        // Compute projection score (sum of squares)
        double score = 0.0;
        for (int i = 0; i < nbins; i++)
            score += proj[i] * proj[i];

        // Update best angle if score is higher
        if (score > best_score) {
            best_score = score;
            best_angle = a;
        }
        free(proj);
    }

    free(bw);
    return best_angle;
}


// rotate_image function
void rotate_image(SDL_Surface **img, double angle_degrees) {
    if (img == 0 || *img == 0) 
        return;

    SDL_Surface *src = *img;

    // Convert degrees to radians for trigonometric functions
    double angle = angle_degrees * PI / 180.0;

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

    if (rotated == 0) {
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
                // Outside the original image, fill with black
                Uint32 black = SDL_MapRGB(rotated->format, 255, 255, 255);
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

