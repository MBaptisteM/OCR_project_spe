#ifndef TRAIN
#define TRAIN

#define _GNU_SOURCE //for asprintf
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define INPUT_SIZE 2058

struct call{
    pthread_mutex_t *mutex;
    float* accuracy;
    char* run;
};

int save(const char *filename);
int load(const char *filename);
void training(char* running, float* accuracy, pthread_mutex_t *mutex);
void init_weights();
void forward(char* inputs);
void backward(char* inputs, int label, float lr);
char get_character(char* path);
float accuracy_calcul();
void* start_training(void* arg);
void generate_network();

#endif