#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

struct Cell
{
    int label;
    int family;
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

//elements in a family
struct fam_elt{
	size_t actual;
	size_t ind;
};

//a family
struct family
{
	double max_dist;
	struct fam_elt* tab;
	size_t size;
	char completed; // 0 if not completed, 1 if completed, -1 if deleted
};

void dfs(unsigned char **img, int **labels, int w, int h, int x, int y, int label, struct Cell *cell);
int label_image_dfs(unsigned char **img, int **labels, int w, int h, struct Cell **out);
void mergeCells(struct Cell arr[], size_t l, size_t m, size_t r);
void mergeSortCells(struct Cell arr[], size_t l, size_t r);

void sort_by_families(struct Cell* cells, size_t n, struct Same_family*** families_sorted);
double distance(struct Center c1, struct Center c2);
void merge(struct Same_family arr[], size_t l, size_t m, size_t r);
void mergeSort(struct Same_family arr[], size_t l, size_t r);
char Same_families(struct family f1, struct family f2);
double Max_possible_dist(struct family f);
double Max_possible_dist_two(struct family f);
char contains(struct family f, int elt);
char Add_next_element(struct family** all_families, struct Same_family **families, int n, char second_call);