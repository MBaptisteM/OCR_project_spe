#include "training.h"
#include "extract_data.h"



int main(){
    char* tab = extract("training_data/333.png");
    for (size_t i = 0; i < 49; i++){
        for (size_t j = 0; j < 42; j++){
            printf("%i", tab[i * 42 + j]);
        }
        printf("\n");
    }

    free(tab);


    training(100);
}