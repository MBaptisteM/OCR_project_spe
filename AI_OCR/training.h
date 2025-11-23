#ifndef TRAIN
#define TRAIN

#define _GNU_SOURCE //for asprintf
#include <stdio.h>
#include <stdlib.h>

#define INPUT_SIZE 2058

void training(size_t num);
void init_weights();
void forward(char* inputs);
void backward(char* inputs, int label, float lr);
char result(char* path);

#endif