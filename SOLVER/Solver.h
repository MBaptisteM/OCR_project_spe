#pragma once

#define MAX 100		// Maximum grid size (100x100)

extern char gridSolver[MAX][MAX];
struct solverResult
{
    size_t totalsize;
    int *x0;
    int *y0;
    int *x1;
    int *y1;
};

int loadGrid(const char *filename);
int searchWord(const char *word, int *x0, int *y0, int *x1, int *y1);
int solve(int argc, char *argv[]);
