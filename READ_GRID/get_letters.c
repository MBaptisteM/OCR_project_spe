#include "get_letters.h"
#define BLACK_THRESHOLD 120 //under 120 we assume it's a dark gray pixel

void dfs(unsigned char **img, int **labels, int w, int h, int x, int y, int label, struct Cell *cell)
{
    //end of recursion
    if (x < 0 || x >= w || y < 0|| y >= h)
        return;
    if (img[y][x] >= BLACK_THRESHOLD)
        return;
    if (labels[y][x] != 0)
        return;
    
    //change info of the cell

    labels[y][x] = label;
    //cell->area++;

    if (x < cell->x_min)
        cell->x_min = x;
    if (x > cell->x_max)
        cell->x_max = x;
    if (y < cell->y_min)
        cell->y_min = y;
    if (y > cell->y_max)
        cell->y_max = y;

    //check if pixels around are part of our cell

    dfs(img, labels, w, h, x+1, y, label, cell);
    dfs(img, labels, w, h, x-1, y, label, cell);
    dfs(img, labels, w, h, x, y+1, label, cell);
    dfs(img, labels, w, h, x, y-1, label, cell); 
}

int label_image_dfs(unsigned char **img, int **labels, int w, int h, struct Cell **out)
{
    int label = 0;
    int alloc_size = 100; //first estimation of 100 cells

    struct Cell *cells = calloc(alloc_size, sizeof(struct Cell));
    if (cells == NULL)
        errx(EXIT_FAILURE, "fail calloc cells");

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if(img[y][x] < BLACK_THRESHOLD && labels[y][x] == 0)
            {
                label++; //we found a new possible letter
                if (label >= alloc_size)
                {
                    alloc_size *=2;
                    cells = realloc(cells, alloc_size*sizeof(struct Cell));
                    if (cells == NULL)
                        errx(EXIT_FAILURE, "fail realloc cells");
                }

                cells[label-1].label = label;
                cells[label-1].x_min = x;
                cells[label-1].x_max = x;
                cells[label-1].y_min = y;
                cells[label-1].y_max = y;
                //cells[label-1].area = 0;


                dfs(img, labels, w, h, x, y, label, &cells[label-1]);
                //check if another cell contain this cell
            
                for (int i = 0; i < label-1; i++)
                {
                    struct Cell *c = &cells[i];
                    if (c->family != 'r' &&
                            cells[label-1].x_min > c->x_min &&
                            cells[label-1].x_max < c->x_max &&
                            cells[label-1].y_min > c->y_min &&
                            cells[label-1].y_max < c->y_max)
                        c->family = 'r';
                }
            }
        }
    }
    *out = cells;
    return label;
}

//void sort_by_families(struct Cell* cells, size_t n, struct Cell** grid, size_t *s_grid, struct Cell** words, size_t* s_words)
void sort_by_families(struct Cell* cells, size_t n, struct Same_family*** families_sorted)
{
    //initialize temporary variables
    
    /*size_t size1 = 1;
    size_t size2 = 1;
    struct Cell* temp1 = calloc(size1, sizeof(struct Cell));
    if (temp1 == NULL)
        errx(EXIT_FAILURE, "fail calloc temp1");
    struct Cell* temp2 = calloc(size2, sizeof(struct Cell));
    if (temp2 == NULL)
    errx(EXIT_FAILURE, "fail calloc temp2");*/

    //initialize an array that will contain center of our cells

    struct Center centers[n];
    for (size_t i = 0; i < n; i++)
    {
        centers[i].center_x = (cells[i].x_min + cells[i].x_max) /2;
        centers[i].center_y = (cells[i].y_min + cells[i].y_max) /2;
        centers[i].size_x = cells[i].x_max - cells[i].x_min;
        centers[i].size_y = cells[i].y_max - cells[i].y_min;
    }

    //array with distance betwen all centers
    struct Same_family **distances = calloc(n, sizeof(struct Same_family*));
    if (distances == NULL)
        errx(EXIT_FAILURE, "fail in calloc distances");
    for (size_t i = 0; i < n; i++)
    {
        struct Same_family *tab = calloc(n-1, sizeof(struct Same_family));
        if (tab == NULL)
            errx(EXIT_FAILURE, "fail calloc distances[i]");
        char offset = 0;
        for (size_t j = 0; j < n; j++)
        {
            if (j!= i)
            {
                tab[j-offset].dist = distance(centers[i], centers[j]);
                tab[j-offset].index = j;
            }
            else
                offset = 1;
        }
        mergeSort(tab, 0, n-2);
        
        distances[i] = tab;
        /*for (size_t j = 0; j < n-1; j++)
            printf("i = %zu j = %zu dist = %f index = %zu\n", i, j,  distances[i][j].dist,  distances[i][j].index);*/
        free(tab);
        printf("\n\n");
    }
    /**/for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n-1; j++)
        {
            printf("i = %zu j = %zu dist = %f index = %zu\n", i, j,  distances[i][j].dist,  distances[i][j].index);
            //printf("Cell[%u] is at %f from cell[%zu]\n", i, distances[i][j].dist, distances[i][j].index);
        }
    }
    *families_sorted  = distances;
    /*size_t **families = calloc(n, sizeof(size_t*));
    if (families == NULL)
        errx(EXIT_FAILURE, "fail in calloc families");
    for (size_t i = 0; i < n; i++)
    {
        
    }*/
    
}

double distance(struct Center c1, struct Center c2)
{
    double dist = sqrt((c2.center_x - c1.center_x)*(c2.center_x - c1.center_x) + (c2.center_y - c1.center_y)*(c2.center_y - c1.center_y));
    double diff_x = (double)abs(c1.size_x - c2.size_x);
    double diff_y = (double)abs(c1.size_y - c2.size_y);
    return dist*1.5 + diff_x*0.75 + diff_y*0.75;
}

void merge(struct Same_family arr[], size_t l, size_t m, size_t r)
{
    
    size_t i, j, k;
    size_t n1 = m - l + 1;
    size_t n2 = r - m;

    struct Same_family L[n1], R[n2];
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i].dist <= R[j].dist) 
        {
            arr[k] = L[i];
            i++;
        }
        else 
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) 
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(struct Same_family arr[], size_t l, size_t r)
{
    
    if (l < r) 
    {
        size_t m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        errx(EXIT_FAILURE, "not enough arguments");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errx(EXIT_FAILURE, "fail initialization sdl");

    //initialize the surface with the grid

    IMG_Init(IMG_INIT_PNG);
    SDL_Surface *img = IMG_Load(argv[1]); 
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

    /*struct Cell *grid;
    size_t s_grid;
    struct Cell *words;
    size_t s_words;
    sort_by_families(cells, (size_t) n, &grid, &s_grid, &words, &s_words);*/

    struct Same_family **families;
    sort_by_families(cells, (size_t) n, &families);
    /*for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n-1; j++)
        {
            printf("Cell[%u] is at %f from cell[%zu]\n", i, families[i][j].dist, families[i][j].index);
        }
    }*/
    //save letters into images
    printf("debut\n");
    int offset = 0;
    for (int i = 0; i < n; i++)
    {
        struct Cell c = cells[i];
        if (c.family != 'r')
        {
        printf("cell : %i\n", i);
        int width = c.x_max - c.x_min + 1;
        int height = c.y_max - c.y_min + 1;

        SDL_Rect srcRect = { c.x_min, c.y_min, width, height };

        SDL_Surface *letter = SDL_CreateRGBSurfaceWithFormat(0, width, height, img->format->BitsPerPixel, SDL_PIXELFORMAT_RGBA8888);
        if (letter == NULL)
            errx(EXIT_FAILURE, "fail letter");
            
        SDL_BlitSurface(img, &srcRect, letter, NULL); //copy letter in surface

        char filename[64];
        snprintf(filename, sizeof(filename), "letters/letter_%d.png", c.label-offset);

        IMG_SavePNG(letter, filename);
        printf(" → Sauvegardé dans %s\n", filename);

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

    free(gray);
    free(labels);
    free(cells);
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;

}
