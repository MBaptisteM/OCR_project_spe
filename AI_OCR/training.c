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
    
    load("AI_OCR/weights.txt");
    *c->accuracy = accuracy_calcul();

    training(c->run, c->accuracy, c->mutex);

    return NULL;
}

int save(const char *filename){
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



int load(const char *filename){
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
        snprintf(temp_path, sizeof(temp_path), "%s/%zu.png", "AI_OCR/training_data", i);
        char r = get_character(temp_path);
        char expected = dict[i].value;
        if (r == expected)
            correct++;
    }

    float accuracy = 100.0f * correct / 1792;
    
    return accuracy;
}


void training(char* running, float* accuracy, pthread_mutex_t *mutex){
    size_t num = 1792;

    char** inputs_tab = calloc(num, sizeof(char*)); // allocation is correct for num samples
    for (size_t i = 0; i < num; i++){
        char* temp_path;
        int test = asprintf(&temp_path, "%s/%zu.png", "AI_OCR/training_data", i);
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
        pthread_mutex_lock(mutex);
        *accuracy = accuracy_calcul();
        pthread_mutex_unlock(mutex);
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


#include <math.h>

#define LOG_BASE 1.1f

float log_custom(float x) {
    return logf(x) / logf(LOG_BASE);
}

void generate_network(){
    FILE *f = fopen("network.dot", "w");
    if (f == 0)
        return;

    fprintf(f, "digraph neural_net {\n");   
    fprintf(f, "    rankdir=LR;\n");    
    fprintf(f, "    bgcolor=\"transparent\";\n"); 
    fprintf(f, "    splines=false;\n"); 
    fprintf(f, "    node [shape=circle, style=filled, fontname=\"Arial\"];\n\n");

    // Calculate node strides
    int count_i  = (int)log_custom((float)INPUT_SIZE);
    int stride_i = (count_i > 0) ? (INPUT_SIZE / count_i) : 1;
    if (stride_i < 1) stride_i = 1;

    int count_h1  = (int)log_custom((float)H1);
    int stride_h1 = (count_h1 > 0) ? (H1 / count_h1) : 1;
    if (stride_h1 < 1) stride_h1 = 1;

    int count_h2  = (int)log_custom((float)H2);
    int stride_h2 = (count_h2 > 0) ? (H2 / count_h2) : 1;
    if (stride_h2 < 1) stride_h2 = 1;

    // Calculate edge strides based on total potential connections per layer pair
    int total_edges1 = count_i * count_h1;
    int log_edges1   = (int)log_custom((float)total_edges1);
    int stride_edge1 = (log_edges1 > 0) ? (total_edges1 / log_edges1) : 1;
    if (stride_edge1 < 1) stride_edge1 = 1;

    int total_edges2 = count_h1 * count_h2;
    int log_edges2   = (int)log_custom((float)total_edges2);
    int stride_edge2 = (log_edges2 > 0) ? (total_edges2 / log_edges2) : 1;
    if (stride_edge2 < 1) stride_edge2 = 1;

    int total_edges3 = count_h2 * OUTPUT;
    int log_edges3   = (int)log_custom((float)total_edges3);
    int stride_edge3 = (log_edges3 > 0) ? (total_edges3 / log_edges3) : 1;
    if (stride_edge3 < 1) stride_edge3 = 1;

    // Generate Input Layer Nodes
    fprintf(f, "    subgraph cluster_input {\n");
    fprintf(f, "        label=\"Input Layer (Log Scaled)\"; color=forestgreen; style=dashed;\n");
    for (int i = 0; i < INPUT_SIZE; i += stride_i) {
        fprintf(f, "        I%d [label=\"In %d\", fillcolor=forestgreen, fontcolor=white];\n", i, i + 1);
    }
    fprintf(f, "    }\n\n");

    // Generate Hidden Layer 1 Nodes
    fprintf(f, "    subgraph cluster_hidden1 {\n");
    fprintf(f, "        label=\"Hidden Layer 1 (H1 - Log Scaled)\"; color=blue; style=dashed;\n");
    for (int i = 0; i < H1; i += stride_h1) {
        fprintf(f, "        H1_%d [label=\"b: %.2f\", fillcolor=lightblue];\n", i, b1[i]);
    }
    fprintf(f, "    }\n\n");

    // Generate Hidden Layer 2 Nodes
    fprintf(f, "    subgraph cluster_hidden2 {\n");
    fprintf(f, "        label=\"Hidden Layer 2 (H2 - Log Scaled)\"; color=purple; style=dashed;\n");
    for (int i = 0; i < H2; i += stride_h2) {
        fprintf(f, "        H2_%d [label=\"b: %.2f\", fillcolor=plum];\n", i, b2[i]);
    }
    fprintf(f, "    }\n\n");

    // Generate Output Layer Nodes
    fprintf(f, "    subgraph cluster_output {\n");
    fprintf(f, "        label=\"Output Layer\"; color=orange; style=dashed;\n");
    for (int i = 0; i < OUTPUT; i++) {
        fprintf(f, "        O%d [label=\"%c\\nb: %.2f\", fillcolor=gold, shape=doublecircle];\n", i, 'A' + i, b3[i]);
    }
    fprintf(f, "    }\n\n");

    // Generate Layer 1 -> Layer 2 Edges (Log Scaled)
    int edge_idx1 = 0;
    for (int i = 0; i < INPUT_SIZE; i += stride_i) {
        for (int j = 0; j < H1; j += stride_h1) {
            if (edge_idx1 % stride_edge1 == 0) {
                float w = W1[j][i];
                fprintf(f, "    I%d -> H1_%d [color=%s];\n", i, j, w >= 0 ? "black" : "grey49");
            }
            edge_idx1++;
        }
    }

    // Generate Layer 2 -> Layer 3 Edges (Log Scaled)
    int edge_idx2 = 0;
    for (int i = 0; i < H1; i += stride_h1) {
        for (int j = 0; j < H2; j += stride_h2) {
            if (edge_idx2 % stride_edge2 == 0) {
                float w = W2[j][i];
                fprintf(f, "    H1_%d -> H2_%d [color=%s];\n", i, j, w >= 0 ? "black" : "grey49");
            }
            edge_idx2++;
        }
    }

    // Generate Layer 3 -> Output Edges (Log Scaled)
    int edge_idx3 = 0;
    for (int i = 0; i < H2; i += stride_h2) {
        for (int j = 0; j < OUTPUT; j++) {
            if (edge_idx3 % stride_edge3 == 0) {
                float w = W3[j][i];
                fprintf(f, "    H2_%d -> O%d [color=%s];\n", i, j, w >= 0 ? "black" : "grey49");
            }
            edge_idx3++;
        }
    }

    fprintf(f, "}\n");
    fclose(f);

    printf("File 'network.dot' generated using Log Base %.2f.\n", LOG_BASE);
    printf("run : 'dot -Tpng network.dot -o network.png' to get a png\n");
}