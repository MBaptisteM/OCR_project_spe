#include "training.h"
#include "extract_data.h"
#include "dict.h"
//actual version : 2058 -> 512 -> 128 -> 26

#define H1 512
#define H2 128
#define OUTPUT 26
#define PATH "training_data"

//weights / biais
float W1[H1][INPUT_SIZE];
float b1[H1];

float W2[H2][H1];
float b2[H2];

float W3[OUTPUT][H2];
float b3[OUTPUT];

//buffers for forward
float z1[H1], a1[H1];
float z2[H2], a2[H2];
float z3[OUTPUT], yhat[OUTPUT];

//gradients
float dW1[H1][INPUT_SIZE], db1[H1];
float dW2[H2][H1], db2[H2];
float dW3[OUTPUT][H2], db3[OUTPUT];

float d1[H1], d2[H2], d3[OUTPUT];


void* start_training(void* arg){
    struct call* c = (struct call*)arg;
    
    load("/home/baptiste/OCR_project_spe/AI_OCR/weights.txt");
    *c->accuracy = accuracy_calcul();
    printf("changed\n");

    training(c->run, c->accuracy, c->mutex);

    return NULL;
}

int save(const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (f == 0)
        return 0;

    //first layer
    for (int i = 0; i < H1; i++){
        for (int j = 0; j < INPUT_SIZE; j++)
            fprintf(f, "%.7f ", W1[i][j]);

        fprintf(f, "%.7f\n", b1[i]);
    }

    //second layer
    for (int i = 0; i < H2; i++){
        for (int j = 0; j < H1; j++)
            fprintf(f, "%.7f ", W2[i][j]);

        fprintf(f, "%.7f\n", b2[i]);
    }

    //third layer
    for (int i = 0; i < OUTPUT; i++){
        for (int j = 0; j < H2; j++)
            fprintf(f, "%.7f ", W3[i][j]);

        fprintf(f, "%.7f\n", b3[i]);
    }

    fclose(f);
    return 1;
}



int load(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == 0)
        return 0;

    //first layer
    for (int i = 0; i < H1; i++){
        for (int j = 0; j < INPUT_SIZE; j++)
            if (fscanf(f, "%f", &W1[i][j]) != 1){
                fclose(f);
                return 0;
            }

        if (fscanf(f, "%f", &b1[i]) != 1){
            fclose(f);
            return 0;
        }
    }

    //second layer
    for (int i = 0; i < H2; i++){
        for (int j = 0; j < H1; j++)
            if (fscanf(f, "%f", &W2[i][j]) != 1){
                fclose(f);
                return 0;
            }

        if (fscanf(f, "%f", &b2[i]) != 1){
            fclose(f);
            return 0;
        }
    }

    //third layer
    for (int i = 0; i < OUTPUT; i++){
        for (int j = 0; j < H2; j++)
            if (fscanf(f, "%f", &W3[i][j]) != 1){
                fclose(f);
                return 0;
            }

        if (fscanf(f, "%f", &b3[i]) != 1){
            fclose(f);
            return 0;
        }
    }

    fclose(f);
    return 1;
}

float accuracy_calcul(){
    size_t correct = 0;
    for (size_t i = 0; i < 1792; i++) {
        char temp_path[256];
        snprintf(temp_path, sizeof(temp_path), "%s/%zu.png", "/home/baptiste/OCR_project_spe/AI_OCR/training_data", i);
        char r = get_character(temp_path);
        char expected = dict[i].value;
        if (r == expected)
            correct++;
    }

    float accuracy = 100.0f * correct / 1792;
    
    return accuracy;
}


void training(char* running, float* accuracy, pthread_mutex_t mutex){
    size_t num = 1792;

    char** inputs_tab = calloc(num, sizeof(char*)); // allocation is correct for num samples
    for (size_t i = 0; i < num; i++){
        char* temp_path;
        int test = asprintf(&temp_path, "%s/%zu.png", "/home/baptiste/OCR_project_spe/AI_OCR/training_data", i);
        (void)test;
        inputs_tab[i] = extract(temp_path);

    }
    while (*running){
        for (size_t j = 0; j < num; j++){
            if (*running == 0)
                break;
            forward(inputs_tab[j]);
            backward(inputs_tab[j], (int)(dict[j].value - 'A'), 0.00005); //learning rate here
        }
        pthread_mutex_lock(&mutex);
        *accuracy = accuracy_calcul();
        pthread_mutex_unlock(&mutex);
    }
    for (size_t i = 0; i < num; i++)
        free(inputs_tab[i]);
    free(inputs_tab);
}


char get_character(char* path){
    char* inputs = extract(path);
    forward(inputs);

    float max = 0;
    char result = 0;
    for (size_t i = 0; i < OUTPUT; i++){
        if (yhat[i] > max){
            max = yhat[i];
            result = i + 'A';
        }
    }

    free(inputs);

    if (result == 0)
        errx(EXIT_FAILURE, "error with exit");
    return result;
}

void init_weights(){
    float scale = sqrtf(2.0f / INPUT_SIZE);
    for(int i = 0; i < H1; i++){
        b1[i] = 0;
        for(int j = 0; j < INPUT_SIZE; j++)
            W1[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * scale;
    }

    scale = sqrtf(2.0f / H1);
    for(int i = 0; i < H2; i++){
        b2[i] = 0;
        for(int j = 0; j < H1; j++)
            W2[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * scale;;
    }

    scale = sqrtf(2.0f / H2);
    for(int i = 0;i < OUTPUT; i++){
        b3[i] = 0;
        for(int j = 0;j < H2; j++)
            W3[i][j] = ((float)rand() / RAND_MAX * 2 - 1) * scale;;
    }
}


void forward(char* inputs)
{
    //layer 1 — z1
    for (int i = 0; i < H1; i++) {
        float sum = b1[i];
        float *w = W1[i];
        for (int j = 0; j < INPUT_SIZE; j++)
            sum += w[j] * inputs[j];

        //relu to try to make the code more stable
        z1[i] = sum;
        a1[i] = (sum > 0.0f) ? sum : 0.0f;
    }

    //layer 2 — z2
    for (int i = 0; i < H2; i++) {
        float sum = b2[i];
        float *w = W2[i];
        for (int j = 0; j < H1; j++)
            sum += w[j] * a1[j];

        z2[i] = sum;
        a2[i] = (sum > 0.0f) ? sum : 0.0f;
    }

    //output layer — z3
    float maxv = -1e30f;
    for (int i = 0; i < OUTPUT; i++) {
        float sum = b3[i];
        float *w = W3[i];
        for (int j = 0; j < H2; j++)
            sum += w[j] * a2[j];

        z3[i] = sum;
        if (sum > maxv)
            maxv = sum;
    }

    //try to make the code more stable
    float exp_sum = 0.0f;
    for (int i = 0; i < OUTPUT; i++) {
        float e = expf(z3[i] - maxv);
        yhat[i] = e;
        exp_sum += e;
    }

    float inv = 1.0f / exp_sum;
    for (int i = 0; i < OUTPUT; i++)
        yhat[i] *= inv;
}



void backward(char* inputs, int label, float lr)
{
    //d3 = yhat - y
    for (int i = 0; i < OUTPUT; i++){
        d3[i] = yhat[i] - (i == label ? 1.0f : 0.0f);
    }

    //dW3 / db3
    for (int i = 0; i < OUTPUT; i++) {
        float delta = d3[i];
        db3[i] = delta;
        float *dw = dW3[i];
        //float *w = W3[i];
        for (int j = 0; j < H2; j++)
            dw[j] = delta * a2[j];
    }

    //delta2
    for (int i = 0; i < H2; i++) {
        float sum = 0.0f;
        for (int j = 0; j < OUTPUT; j++)
            sum += W3[j][i] * d3[j];

        d2[i] = (z2[i] > 0.0f) ? sum : 0.0f;
    }

    //dW2 / db2
    for (int i = 0; i < H2; i++) {
        float delta = d2[i];
        db2[i] = delta;
        float *dw = dW2[i];
        for (int j = 0; j < H1; j++)
            dw[j] = delta * a1[j];
    }

    //delta1
    for (int i = 0; i < H1; i++) {
        float sum = 0.0f;
        for (int j = 0; j < H2; j++)
            sum += W2[j][i] * d2[j];

        d1[i] = (z1[i] > 0.0f) ? sum : 0.0f;
    }

    //dW1 / db1
    for (int i = 0; i < H1; i++) {
        float delta = d1[i];
        db1[i] = delta;
        float *dw = dW1[i];
        for (int j = 0; j < INPUT_SIZE; j++)
            dw[j] = delta * inputs[j];
    }

    // gradient clipping
    // used to major gradiant values to avoid NaN
    const float CLIP = 10.0f;

    for (int i = 0; i < OUTPUT; i++) {
        if (db3[i] > CLIP) db3[i] = CLIP;
        if (db3[i] < -CLIP) db3[i] = -CLIP;
        for (int j = 0; j < H2; j++) {
            if (dW3[i][j] > CLIP) dW3[i][j] = CLIP;
            if (dW3[i][j] < -CLIP) dW3[i][j] = -CLIP;
        }
    }

    for (int i = 0; i < H2; i++) {
        if (db2[i] > CLIP) db2[i] = CLIP;
        if (db2[i] < -CLIP) db2[i] = -CLIP;
        for (int j = 0; j < H1; j++) {
            if (dW2[i][j] > CLIP) dW2[i][j] = CLIP;
            if (dW2[i][j] < -CLIP) dW2[i][j] = -CLIP;
        }
    }

    for (int i = 0; i < H1; i++) {
        if (db1[i] > CLIP) db1[i] = CLIP;
        if (db1[i] < -CLIP) db1[i] = -CLIP;
        for (int j = 0; j < INPUT_SIZE; j++) {
            if (dW1[i][j] > CLIP) dW1[i][j] = CLIP;
            if (dW1[i][j] < -CLIP) dW1[i][j] = -CLIP;
        }
    }

    //update
    //W1 / b1
    for (int i = 0; i < H1; i++) {
        b1[i] -= lr * db1[i];
        float *dw = dW1[i];
        float *w = W1[i];
        for (int j = 0; j < INPUT_SIZE; j++)
            w[j] -= lr * dw[j];
    }

    //W2 / b2
    for (int i = 0; i < H2; i++) {
        b2[i] -= lr * db2[i];
        float *dw = dW2[i];
        float *w = W2[i];
        for (int j = 0; j < H1; j++)
            w[j] -= lr * dw[j];
    }

    //W3 / b3
    for (int i = 0; i < OUTPUT; i++) {
        b3[i] -= lr * db3[i];
        float *dw = dW3[i];
        float *w = W3[i];
        for (int j = 0; j < H2; j++)
            w[j] -= lr * dw[j];
    }
}
