#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

struct Cell
{
    int label;
    char family;
    //int area;
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};

struct Center
{
    int center_x;
    int center_y;
    int size_x;
    int size_y;
};

struct Same_family
{
    double dist;
    size_t index;
};

void dfs(unsigned char **img, int **labels, int w, int h, int x, int y, int label, struct Cell *cell);
int label_image_dfs(unsigned char **img, int **labels, int w, int h, struct Cell **out);
void sort_by_families(struct Cell* cells, size_t n, struct Same_family*** families_sorted);
//void sort_by_families(struct Cell* cells, size_t n, struct Cell** grid, size_t *s_grid, struct Cell** words, size_t* s_words);
double distance(struct Center c1, struct Center c2);
void merge(struct Same_family arr[], size_t l, size_t m, size_t r);
void mergeSort(struct Same_family arr[], size_t l, size_t r);
