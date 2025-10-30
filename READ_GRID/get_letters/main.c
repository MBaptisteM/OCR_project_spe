#include "get_letters.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
        errx(EXIT_FAILURE, "not enough arguments");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "fail initialization sdl");

    //initialize the surface with the grid

    IMG_Init(IMG_INIT_PNG);

    char stock[150];
    snprintf(stock, sizeof(stock), "../images/%s", argv[1]);
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

    Uint32 *pixels = (Uint32*) img->pixels;
    int pitch = img->pitch/4; //number of pixels per line

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * pitch + x], img->format, &r, &g, &b);
            gray[y][x] = (unsigned char)(0.299*r + 0.587*g + 0.114*b); 
        }
    }

    //creation of the stock of labels

    int **labels = calloc(h, sizeof(int*));
    if (labels == NULL)
        errx(EXIT_FAILURE, "fail calloc labels");
    for (int y = 0; y < h; y++)
    {
        labels[y] = calloc(w, sizeof(int));
        if (labels[y] == NULL)
            errx(EXIT_FAILURE, "fail calloc labels[y]");
    }

    //initialization labels

    struct Cell *cells;
    int n = label_image_dfs(gray, labels, w, h, &cells);
    
    //sort cells

    struct Center *centers;
    struct Dist_with **families;
    sort_by_families(cells, (size_t) n, &families, &centers, 0);

    
    //Initialize the families
    struct family* all_families = calloc(n, sizeof(struct family));
    for (size_t i = 0 ;i < n; i++){
        struct family fam = {0,calloc(n, sizeof(struct fam_elt)),1,0};
        struct fam_elt first = {0,i};
        fam.tab[0] = first;
        all_families[i] = fam;
    }

    //Add closests elements together
    int iter = 0;
    while (Add_next_element(&all_families, families, n, 0, cells)){
        //only for the three first because too slow
        if (iter < 3){
            iter++;
            Remove_same_families(&all_families, n);
        }
    }

    
    
    

    //Define what are the spies elements (grid and elements containing other elements)
    int* to_remove = calloc(n,sizeof(int));
    size_t sizeRemove = 0;
    for (size_t i = 0; i < n; i++){
        if (cells[i].family == 2){
            to_remove[sizeRemove] = i;
            sizeRemove++;
        }
    }
    
    //Order 66 (use the spies as traitors) (you can only trust yoursefl and other letters bastards)
    for (size_t i = 0; i < n; i++){
        size_t k = 0;
        
        while (k < sizeRemove && all_families[i].completed != -1){
            if (contains(all_families[i], to_remove[k]))
                all_families[i].completed = -1;
            k++;
        }
            
    }

    Remove_same_families(&all_families, n);



    //Define the biggest family
    size_t max_ind = 0;
    double max_size = 0;
    double coef = 0.001;
    for (size_t i = 0; i < n; i ++){
        if (all_families[i].completed != -1 && all_families[i].size / (all_families[i].max_dist * coef) > max_size){
            max_size = all_families[i].size / (all_families[i].max_dist * coef);
            max_ind = i;
        }
    }


    //Remove every cells that are inside of the gid
    all_families[max_ind].completed = -1;
    for (size_t i = 0; i < all_families[max_ind].size; i++ ){
        cells[all_families[max_ind].tab[i].ind].family = 1;
    }


    //Remove every families that contains an element of the grid
    for (size_t i = 0; i < n; i++ ){
        size_t j = 0;
        while (j < all_families[i].size && all_families[i].completed != -1){
            if (cells[all_families[i].tab[j].ind].family == 1)
                all_families[i].completed = -1;
            j++;
        }
    }



    //Group the elements by family
    int k = 3;
    for (size_t i = 0; i < n; i ++){
        if (all_families[i].completed != -1){
            for (size_t j = 0; j < all_families[i].size; j++ ){
                cells[all_families[i].tab[j].ind].family = k;
            }
            k++;
        }
    }


    for (size_t i = 0; i < n; i++){
        if (cells[i].family == 0)
            cells[i].family = 1;
    }

    
    //Remove elements that are too far from the mediane distance
    Remove_too_far_mediane(&cells, (size_t)n, centers);



    
    //to make the selection more specific
    /*
    size_t new_size = 0;
    for (size_t i = 0 ;i < n; i++){
        if (cells[i].family != 1 && cells[i].family != 2){
            new_size ++;
            cells[i].family = 0;
        }
    }
        */

    struct family* all_families2 = calloc(n, sizeof(struct family));
    for (size_t i = 0 ;i < n; i++){
        if (cells[i].family != 1 && cells[i].family != 2){
            struct family fam = {0,calloc(n, sizeof(struct fam_elt)),1,0};
            struct fam_elt first = {0,i};
            fam.tab[0] = first;
            all_families2[i] = fam;
        }
        else{
            struct family fam = {0,calloc(n, sizeof(struct fam_elt)),0,-1};
            all_families2[i] = fam;
        }
    }

    sort_by_families(cells, (size_t) n, &families, &centers, 0);


    iter = 0;
    while (Add_next_element(&all_families2, families, n, 1, cells)){
        //only for the three first because too slow
        if (iter < 3){
            iter++;
            Remove_same_families(&all_families2, n);
        }
    } 



    Remove_same_families(&all_families2, n);


    k = 3;
    for (size_t i = 0; i < n; i ++){
        if (all_families2[i].completed != -1){
            int num = 0;
            for (size_t j = 0; j < all_families2[i].size; j++ ){
                if (cells[all_families2[i].tab[j].ind].family != 2){
                    cells[all_families2[i].tab[j].ind].family = k;
                    num ++;
                }
            }
            if (num != 0)
                k++;
        }
    }


    int actual = -1;
    int tab[n];
    int num = 0;
    for (size_t i = 0; i < n; i++){
        tab[i] = 0;
    }
    for (size_t i = 0; i < n; i++){
        if (cells[i].family != 1 && cells[i].family != 2)
            tab[cells[i].family]++;
    }
    for (size_t i = 3; i < n; i++){
        if (tab[i] < 3){
            for (size_t j = 0; j < n; j++){
                if (cells[j].family == i){
                    cells[j].family = 2;
                }
            }
        }
    }


    k = 3;
    for (size_t i = 0; i < n; i ++){
        if (all_families2[i].completed != -1){
            int num = 0;
            for (size_t j = 0; j < all_families2[i].size; j++ ){
                if (cells[all_families2[i].tab[j].ind].family != 2){
                    cells[all_families2[i].tab[j].ind].family = k;
                    num ++;
                }
            }
            if (num != 0)
                k++;
        }
    }


    


    




    
    





 
    //

    //clear the folders
    system("rm -r ../grid");
    system("rm -r ../letters");
    system("mkdir ../grid");
    system("mkdir ../letters");

    

    //save letters into images
    int offset = 0;
    for (int i = 0; i < n; i++)
    {
        struct Cell c = cells[i];
        if (c.family != 2)
        {
            
            //printf("cell : %i\n", i);
            int width = c.x_max - c.x_min + 1;
            int height = c.y_max - c.y_min + 1;

            SDL_Rect srcRect = { c.x_min, c.y_min, width, height};


            SDL_Surface *letter = SDL_CreateRGBSurfaceWithFormat(0, width, height, img->format->BitsPerPixel, SDL_PIXELFORMAT_RGBA8888);
            if (letter == NULL)
                errx(EXIT_FAILURE, "fail letter");
            
                
            SDL_BlitSurface(img, &srcRect, letter, NULL); //copy letter in surface
            

            char filename[64];
            if (c.family == 1)
                {
                    //printf("grid/letter_%i x_min : %i y_min : %i\n", c.label-offset, c.x_min, c.y_min);
                    snprintf(filename, sizeof(filename), "../grid/letter_%i.png", c.label-offset);
                }
            else
                {
                    //printf("letters/word_%i_letter_%i x_min : %i y_min : %i\n", c.family - 2, c.label-offset, c.x_min, c.y_min);
                    snprintf(filename, sizeof(filename), "../letters/word_%i_letter_%i.png", c.family - 2, c.label-offset);
                }

            IMG_SavePNG(letter, filename);
            //printf(" -> Saved in %s\n", filename);

            SDL_FreeSurface(letter);
            
        }
        else
            offset++;
    }

    //clean everything
    for (int y = 0; y < h; y++)
    {
        free(gray[y]);
        free(labels[y]);
    }
    for (size_t i = 0; i < n; i++) {
        free(families[i]);
    }
    for (size_t i = 0; i < n; i++) {
        free(all_families[i].tab);
    }
    free(all_families);
    for (size_t i = 0; i < n; i++) {
        free(all_families2[i].tab);
    }
    free(all_families2);

    free(families);
    free(centers);
    free(gray);
    free(labels);
    free(cells);
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;

}