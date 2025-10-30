#include "get_letters.h"
#define BLACK_THRESHOLD 160 //under 120 we assume it's a dark gray pixel

void dfs(unsigned char **img, int **labels, int w, int h, int x, int y, int label, struct Cell *cell, int black_TH)
{
    //end of recursion
    if (x < 0 || x >= w || y < 0|| y >= h){
        return;
    }
        
    if (img[y][x] >= black_TH){
        return;
    }
    if (labels[y][x] != 0){
        return;
    }
    
    //change info of the cell

    labels[y][x] = label;

    if (x < cell->x_min)
        cell->x_min = x;
    if (x > cell->x_max)
        cell->x_max = x;
    if (y < cell->y_min)
        cell->y_min = y;
    if (y > cell->y_max)
        cell->y_max = y;

    //check if pixels around are part of our cell

    int edit[] = {1,0, -1,0, 0,1, 0,-1, 1,1, -1,1, 1,-1, -1,-1};
    for (size_t i = 0; i < 16; i+=2){
        dfs(img, labels, w, h, x + edit[i], y + edit[i + 1], label, cell, black_TH);
    }
    return;
}

int label_image_dfs(unsigned char **img, int **labels, int w, int h, struct Cell **out)
{
    int label = 0;
    int alloc_size = 100; //first estimation of 100 cells
    int black_TH = BLACK_THRESHOLD;

    struct Cell *cells = calloc(alloc_size, sizeof(struct Cell));
    if (cells == NULL)
        errx(EXIT_FAILURE, "fail calloc cells");

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if(img[y][x] < black_TH && labels[y][x] == 0)
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
                dfs(img, labels, w, h, x, y, label, &cells[label-1], black_TH);
                
                /*
                size_t i = 0;
                for (size_t i = 0; i < size; i++){
                    if (img[tab[i].y][tab[i].x] < BLACK_THRESHOLD + 40 && tab[i].y < cells[label - 1].y_min + abs(cells[label - 1].y_max - cells[label - 1].y_min) * 0.9 && labels[tab[i].y][tab[i].x] == 0){
                        printf("x = %i, y = %i, num = %i\n",tab[i].x, tab[i].y, label);
                        //we want to link
                        labels[tab[i].y][tab[i].x] = label;
                        if (cells[label - 1].x_max < tab[i].x)
                            cells[label - 1].x_max = tab[i].x;

                        if (cells[label - 1].y_max < tab[i].y)
                            cells[label - 1].y_max = tab[i].y;

                        if (cells[label - 1].x_min > tab[i].x)
                            cells[label - 1].x_min = tab[i].x;

                        if (cells[label - 1].y_min > tab[i].y)
                            cells[label - 1].y_min = tab[i].y;
                        
                        
                        int edit[] = {1,0, -1,0, 0,1, 0,-1, 1,1, -1,1, 1,-1, -1,-1};
                        for (size_t i = 0; i < 16; i+=2){
                            size_t k = 0;
                            size_t s = 0;
                            struct XY* tab2 = dfs(img, labels, w, h, tab[i].x + edit[i], tab[i].y + edit[i + 1], label, &cells[label-1], &s);

                            for (size_t k = 0; k < s; k++){
                                tab[size] = tab2[k];
                                size ++;
                            }
                        }
                    }

                    i++;
                }
                    */
                //remove the cell if too small
                if (cells[label-1].y_max - cells[label-1].y_min < 10)
                {
                    label--;
                }
                else if ((cells[label - 1].y_max - cells[label - 1].y_min + 1) >= 15 && (double)(cells[label - 1].x_max - cells[label - 1].x_min + 1) / (double)(cells[label - 1].y_max - cells[label - 1].y_min + 1) > 2.17){
                    printf("passe\n");
                    int white_min = -1;
                    size_t x_final = 0;
                    int white_min2 = -1;
                    size_t x_final2 = 0;
                    for (size_t x = cells[label - 1].x_min; x < cells[label - 1].x_max; x++){
                        double sum = 0;
                        for (size_t y = cells[label - 1].y_min + 2; y <= cells[label - 1].y_max - 2; y++){
                            sum += img[y][x];
                        }
                        if (white_min == -1 || sum > white_min){
                            white_min2 = white_min;
                            x_final2 = x_final;

                            white_min = sum;
                            x_final = x;
                            x+=1;
                        }
                    }


 
                    if (white_min2 != -1){
                        label++;
                        if (label >= alloc_size)
                        {
                            alloc_size *=2;
                            cells = realloc(cells, alloc_size*sizeof(struct Cell));
                            if (cells == NULL)
                                errx(EXIT_FAILURE, "fail realloc cells");
                        }

                        cells[label-1].label = label;
                        cells[label-1].x_min = x_final2 + 1;
                        cells[label-1].x_max = x_final;
                        cells[label-1].y_min = cells[label - 2].y_min;
                        cells[label-1].y_max = cells[label - 2].y_max;

                        for (size_t x = cells[label-1].x_min; x < cells[label-1].x_max; x++){
                            for (size_t y = cells[label-1].y_min; y < cells[label-1].y_max; y++){
                                if (labels[y][x] == label - 1)
                                    labels[y][x] = label;
                            }
                        }

                        label++;
                        if (label >= alloc_size)
                        {
                            alloc_size *=2;
                            cells = realloc(cells, alloc_size*sizeof(struct Cell));
                            if (cells == NULL)
                                errx(EXIT_FAILURE, "fail realloc cells");
                        }

                        cells[label-1].label = label;
                        cells[label-1].x_min = x_final + 1;
                        cells[label-1].x_max = cells[label - 3].x_max;
                        cells[label-1].y_min = cells[label - 2].y_min;
                        cells[label-1].y_max = cells[label - 2].y_max;

                        for (size_t x = cells[label-1].x_min; x < cells[label-1].x_max; x++){
                            for (size_t y = cells[label-1].y_min; y < cells[label-1].y_max; y++){
                                if (labels[y][x] == label - 1)
                                    labels[y][x] = label;
                            }
                        }

                        if (cells[label-2].y_max - cells[label-2].y_min < 10)
                        {
                            label--;
                        }

                        cells[label-3].x_max = x_final2;

                        if (cells[label-3].y_max - cells[label-3].y_min < 10)
                        {
                            label--;
                        }
                        
                    }
                    if (cells[label-1].y_max - cells[label-1].y_min < 10)
                    {
                        label--;
                    }

                }
                else if ((cells[label - 1].x_max - cells[label - 1].x_min + 1) > 10 && (double)(cells[label - 1].x_max - cells[label - 1].x_min + 1) / (double)(cells[label - 1].y_max - cells[label - 1].y_min + 1) > 1.5){
                    printf("passe2\n");
                    int white_min = -1;
                    size_t x_final = 0;
                    size_t diff = cells[label - 1].x_max - cells[label - 1].x_min + 1 ;
                    for (size_t x = cells[label - 1].x_min + diff / 3 + 2; x <= cells[label - 1].x_max - diff / 3 - 2; x++){
                        double sum = 0;
                        for (size_t y = cells[label - 1].y_min + 2; y <= cells[label - 1].y_max - 2; y++){
                            sum += abs(255 - img[y][x]);
                        }
                        sum *= 1 + abs(x - cells[label - 1].x_min - (cells[label - 1].x_max - cells[label - 1].x_min + 1)) / 2;
                        if (white_min == -1 || sum < white_min){
                            white_min = sum;
                            x_final = x;
                        }
                    }

                    
                    if (white_min != -1){

                        label++;
                        if (label >= alloc_size)
                        {
                            alloc_size *=2;
                            cells = realloc(cells, alloc_size*sizeof(struct Cell));
                            if (cells == NULL)
                                errx(EXIT_FAILURE, "fail realloc cells");
                        }

                        cells[label-1].label = label;
                        cells[label-1].x_min = x_final + 1;
                        cells[label-1].x_max = cells[label - 2].x_max;
                        cells[label-1].y_min = cells[label - 2].y_min;
                        cells[label-1].y_max = cells[label - 2].y_max;

                        for (size_t x = cells[label-1].x_min; x < cells[label-1].x_max; x++){
                            for (size_t y = cells[label-1].y_min; y < cells[label-1].y_max; y++){
                                if (labels[y][x] == label - 1)
                                    labels[y][x] = label;
                            }
                        }


                        cells[label-2].x_max = x_final;

                        if (cells[label-2].y_max - cells[label-2].y_min < 10)
                        {
                            label--;
                        }
                    }
                    if (cells[label-1].y_max - cells[label-1].y_min < 10)
                    {
                        label--;
                    }          

                }
                else{
                    //check if another cell contain this cell
                    
                    for (int i = 0; i < label-1; i++)
                    {
                        struct Cell *c = &cells[i];
                        if (c->family != 2 &&
                                cells[label-1].x_min > c->x_min &&
                                cells[label-1].x_max < c->x_max &&
                                cells[label-1].y_min > c->y_min &&
                                cells[label-1].y_max < c->y_max)
                            c->family = 2;
                    }

                    black_TH = BLACK_THRESHOLD;
                }
            }
        }
    }
    
    int i = 0;
    while (i < label)
    {
        int y = cells[i].y_min;
        int s = 1;
        int max_diff = 5; //abs(cells[i].y_max - cells[i].y_min);
        while (i + s < label && abs(y - cells[i + s].y_min) < max_diff)
            s++;
        
        for (int j = i; j < i + s; j++)
        {
            struct Cell curr = cells[j];
            int k = j - 1;

            while (k >= i && cells[k].x_min > curr.x_min) 
            {
                cells[k + 1] = cells[k];
                k--;
            }
            cells[k + 1] = curr;
        }
        i += s;
    }

    for (size_t i = 0; i < label; i ++){
        cells[i].label = i;
    }

    *out = cells;
    return label;
}


void sort_by_families(struct Cell* cells, size_t n, struct Dist_with*** families_sorted, struct Center **c)
{
    struct Center *centers = calloc(n, sizeof(struct Center));
    if (centers == NULL)
        errx(EXIT_FAILURE, "fail calloc centers");
    for (size_t i = 0; i < n; i++)
    {
        centers[i].center_x = (cells[i].x_min + cells[i].x_max) /2;
        centers[i].center_y = (cells[i].y_min + cells[i].y_max) /2;
        centers[i].size_x = cells[i].x_max - cells[i].x_min;
        centers[i].size_y = cells[i].y_max - cells[i].y_min;
    }

    //array with distance between all centers
    
    struct Dist_with **distances = calloc(n, sizeof(struct Dist_with*));
    if (distances == NULL)
        errx(EXIT_FAILURE, "fail in calloc distances");
    for (size_t i = 0; i < n; i++)
    {
        struct Dist_with *tab = calloc(n-1, sizeof(struct Dist_with));
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
    }
    
    /*for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n-1; j++)
        {
            printf("i = %zu j = %zu dist = %f index = %zu\n", i, j,  distances[i][j].dist,  distances[i][j].index);
        }
    }*/
    *c = centers;
    *families_sorted  = distances;
}

double distance(struct Center c1, struct Center c2)
{
    double dist_x = sqrt((c2.center_x - c1.center_x)*(c2.center_x - c1.center_x));
    double dist_y = sqrt((c2.center_y - c1.center_y)*(c2.center_y - c1.center_y));
    double diff_x = (double)abs(c1.size_x - c2.size_x);
    double diff_y = (double)abs(c1.size_y - c2.size_y);
    return dist_x*2 + dist_y*2 + diff_x*0.5 + diff_y*3;
}


void merge(struct Dist_with arr[], size_t l, size_t m, size_t r)
{
    
    size_t i, j, k;
    size_t n1 = m - l + 1;
    size_t n2 = r - m;

    struct Dist_with L[n1], R[n2];
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

void mergeSort(struct Dist_with arr[], size_t l, size_t r)
{
    
    if (l < r) 
    {
        size_t m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}


void sort_heights(struct Dist_with **heights, size_t size){
    for (size_t i = 1; i < size; i++) 
    {
        struct Dist_with curr = (*heights)[i];
        long int j = i - 1;

        while (j >= 0 && (*heights)[j].dist > curr.dist) 
        {
            (*heights)[j + 1] = (*heights)[j];
            j = j - 1;
        }
        (*heights)[j + 1] = curr;
    }
}

void sort_y(int **tab, size_t size, struct Center *centers){
    for (size_t i = 1; i < size; i++) 
    {
        int curr = (*tab)[i];
        long int j = i - 1;

        while (j >= 0 && centers[(*tab)[j]].center_y > centers[curr].center_y) 
        {
            (*tab)[j + 1] = (*tab)[j];
            j = j - 1;
        }
        (*tab)[j + 1] = curr;
    }
}
void sort_x(int **tab, size_t size, struct Center *centers){
    for (size_t i = 1; i < size; i++) 
    {
        int curr = (*tab)[i];
        long int j = i - 1;

        while (j >= 0 && centers[(*tab)[j]].center_x > centers[curr].center_x) 
        {
            (*tab)[j + 1] = (*tab)[j];
            j = j - 1;
        }
        (*tab)[j + 1] = curr;
    }
}
/**/
void Remove_too_far_mediane(struct Cell** cells, size_t n, struct Center *centers)
{
    //start with the y
    struct Dist_with *heights = calloc(n, sizeof(struct Dist_with));
    struct Dist_with *heights2 = calloc(n, sizeof(struct Dist_with));
    size_t size1 = 0;
    size_t size2 = 0;
    if (heights == NULL || heights2 == NULL)
        errx(EXIT_FAILURE, "fail calloc distance with origin");


    for (size_t i = 0; i < n; i++)
    {
        struct Dist_with temp = {centers[i].size_y, i};
        if ((*cells)[i].family == 1){
            heights2[size2] = temp;
            size2 ++;
        }
        else if ((*cells)[i].family != 2)
        {
            heights[size1] = temp;
            size1 ++;
        }
    }
    
    
    //sort by distances
    sort_heights(&heights, size1);
    sort_heights(&heights2, size2);
    
    //find the mediane
    double mediane1 = heights[size1 / 2].dist;
    double mediane2 = heights2[size2 / 2].dist;

    double coeff1 = 0.2;
    double coeff2 = 0.3;
    for (size_t i = 0; i < size1; i++)
    {
        if (heights[i].dist < mediane1 * (1 - coeff1) || //if too far from mediane
        heights[i].dist > mediane1 * (1 + coeff1))
        {
            (*cells)[heights[i].index].family = 2;
        }  
    }

    for (size_t i = 0; i < size2; i++)
    {
        if (heights2[i].dist < mediane2 * (1 - coeff2) || //if too far from mediane
        heights2[i].dist > mediane2 * (1 + coeff2))
        {
            (*cells)[heights2[i].index].family = 2;
        }  
    }








    
    //too far away
    int *tab1 = calloc(n, sizeof(int));
    int *tab2 = calloc(n, sizeof(int));
    size1 = 0;
    size2 = 0;
    if (heights == NULL || heights2 == NULL)
        errx(EXIT_FAILURE, "fail calloc distance with origin");


    for (size_t i = 0; i < n; i++)
    {
        if ((*cells)[i].family == 1){
            tab2[size2] = i;
            size2 ++;
        }
        else if ((*cells)[i].family != 2)
        {
            tab1[size1] = i;
            size1 ++;
        }
    }

    
    sort_y(&tab1,size1,centers);
    sort_y(&tab2,size2,centers);

    

    size_t i = 0;
    double coef = 0.01;
    while (i < size1 - 1 && (centers[tab1[i]].center_y < centers[tab1[i + 1]].center_y * (1 - coef) || centers[tab1[i]].center_y > centers[tab1[i + 1]].center_y * (1 + coef))){
        (*cells)[tab1[i]].family = 2;
        i++;
    }
    i = size1 - 1;
    while (i > 0 && (centers[tab1[i]].center_y < centers[tab1[i - 1]].center_y * (1 - coef) || centers[tab1[i]].center_y > centers[tab1[i - 1]].center_y * (1 + coef))){
        (*cells)[tab1[i]].family = 2;
        i--;
    }

    i = 0;
    coef = 0.1;
    while (i < size2){
        char checked = 0;
        size_t j = 0;
        while ( j < size2 && checked < 7 ){
            if ( i != j && centers[tab2[i]].center_y >= centers[tab2[j]].center_y * (1 - coef) && centers[tab2[i]].center_y <= centers[tab2[j]].center_y * (1 + coef))
                checked ++;
            j++;
        }
        if (checked < 7)
            (*cells)[tab2[i]].family = 2;
        i++;
    }




    sort_x(&tab1,size1,centers);
    sort_x(&tab2,size2,centers);

    i = 0;
    coef = 0.01;
    while ((*cells)[tab1[i]].family != 2 && i < size1 - 1 && (centers[tab1[i]].center_x < centers[tab1[i + 1]].center_x * (1 - coef) || centers[tab1[i]].center_x > centers[tab1[i + 1]].center_x * (1 + coef))){
        (*cells)[tab1[i]].family = 2;
        i++;
    }
    i = size1 - 1;
    while ((*cells)[tab1[i]].family != 2 && i > 0 && (centers[tab1[i]].center_x < centers[tab1[i - 1]].center_x * (1 - coef) || centers[tab1[i]].center_x > centers[tab1[i - 1]].center_x * (1 + coef))){
        (*cells)[tab1[i]].family = 2;
        i--;
    }

    i = 0;
    coef = 0.1;
    while (i < size2){
        char checked = 0;
        size_t j = 0;
        while ( j < size2 && checked < 7 ){
            if ( i != j && centers[tab2[i]].center_x >= centers[tab2[j]].center_x * (1 - coef) && centers[tab2[i]].center_x <= centers[tab2[j]].center_x * (1 + coef))
                checked ++;
            j++;
        }
        if (checked < 7)
            (*cells)[tab2[i]].family = 2;
        i++;
    }



    free(heights);
    free(heights2);
}
    

// do f1 and f2 contains the same elements ? -> 1 = true, 0 = false
char Same_families(struct family f1, struct family f2){
    if (f1.size != f2.size)
	return 0;
    for (size_t i = 0; i < f1.size; i++){
        size_t j = 0;
        while (j < f2.size && f2.tab[j].ind != f1.tab[i].ind)
            j++;
        if (j == f2.size)
            return 0;
    }
    return 1;
}
void Remove_same_families(struct family** all_families, int n){
    for (size_t i = 0; i < n; i++){
        size_t j = 0;
        while (j < n && (*all_families)[i].completed != -1){
            if (  i != j && (*all_families)[j].completed != -1 && Same_families((*all_families)[i], (*all_families)[j]) ){
                (*all_families)[i].completed = -1;
            }
            j++;
        }
    }
}

double Max_possible_dist(struct family f){
    double coef = 1.7;
    return f.max_dist * (1 + coef/ f.size);
}

//unused but for a more specifit traitment of words
double Max_possible_dist_two(struct family f){
    double coef = 2;
    return f.max_dist * (1 + coef/ f.size);
}

char contains(struct family f, int elt){
    for (size_t i = 0; i < f.size; i++){
	if (f.tab[i].ind == elt)
	    return 1;
    }
    return 0;
}

char Add_next_element(struct family** all_families, struct Dist_with **families, int n, char second_call){
    char changed = 0;
    for (size_t i = 0; i < n; i++){
        struct family fam = (*all_families)[i];
        if (fam.completed == 0){
            changed = 1;
            double min = -1;
            size_t index = 0;
            for (size_t j = 0; j < fam.size; j++){
                while (fam.tab[j].actual < n - 1 && contains(fam, families[fam.tab[j].ind][fam.tab[j].actual].index))
                    fam.tab[j].actual ++;
                if (fam.tab[j].actual < n - 1 && (min == -1 || families[fam.tab[j].ind][fam.tab[j].actual].dist < min)){
                    min = families[fam.tab[j].ind][fam.tab[j].actual].dist;
                    index = families[fam.tab[j].ind][fam.tab[j].actual].index;
                }
            }
            if (min == - 1 || (fam.max_dist != 0 && min > (second_call ? Max_possible_dist_two(fam) : Max_possible_dist(fam))))
                fam.completed = 1;
            else{
                struct fam_elt new = {0,index};
                fam.tab[fam.size] = new;
                fam.size ++;
                if (min > fam.max_dist)
                    fam.max_dist = min;		
                
            }
            (*all_families)[i] = fam;
        }
    }
    return changed;
}
/*double medianeDistance(struct family** all_families)
{
    return 0;
}*/

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

    struct Center *centers;
    struct Dist_with **families;
    sort_by_families(cells, (size_t) n, &families, &centers);

    
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
    while (Add_next_element(&all_families, families, n, 0)){
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





    
    //to make the selection more specific
    /*
    size_t new_size = 0;
    for (size_t i = 0 ;i < n; i++){
        if (cells[i].family == 0)
            new_size ++;
    }

    all_families = calloc(new_size, sizeof(struct family));
    for (size_t i = 0 ;i < n; i++){
        if (cells[i].family == 0){
            struct family fam = {0,calloc(n, sizeof(struct fam_elt)),1,0};
            struct fam_elt first = {0,i};
            fam.tab[0] = first;
            all_families[i] = fam;
        }
    }

    iter = 0;
    while (Add_next_element(&all_families, families, n, 1)){
        //only for the three first because too slow
        if (iter < 3){
            iter++;
            Remove_same_families(&all_families, n);
        }
    }   
    Remove_same_families(&all_families, n);
    */





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
 
    //

    //clear the folders
    system("rm -r grid");
    system("rm -r letters");
    system("mkdir grid");
    system("mkdir letters");

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

            SDL_Rect srcRect = { c.x_min, c.y_min, width, height };

            SDL_Surface *letter = SDL_CreateRGBSurfaceWithFormat(0, width, height, img->format->BitsPerPixel, SDL_PIXELFORMAT_RGBA8888);
            if (letter == NULL)
                errx(EXIT_FAILURE, "fail letter");
                
            SDL_BlitSurface(img, &srcRect, letter, NULL); //copy letter in surface

            char filename[64];
            if (c.family == 1)
                {
                    //printf("grid/letter_%i x_min : %i y_min : %i\n", c.label-offset, c.x_min, c.y_min);
                    snprintf(filename, sizeof(filename), "grid/letter_%i.png", c.label-offset);
                }
            else
                {
                    //printf("letters/word_%i_letter_%i x_min : %i y_min : %i\n", c.family - 2, c.label-offset, c.x_min, c.y_min);
                    snprintf(filename, sizeof(filename), "letters/word_%i_letter_%i.png", c.family - 2, c.label-offset);
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

    free(gray);
    free(labels);
    free(cells);
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;

}
