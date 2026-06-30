How to use our OCR ?
First, type "make all" in your terminal. Then, execute the ocr file using ./all without any arguments.
The first menu appears and with it, two buttons. 
For training you AI, click on the training button. A new window appears and you will be able to see the training of your AI. To stop the training, click on the stop button. The training will automatically be saved. You then will be back on the first menu window.
Now, if you want to resolve a grid, the picture you want must be in the images folder located in the READ_GRID folder. 
If you do not have any pictures, nothing will appear. Between one and ten, they will all be displayed. However, if you have more than ten pictures, only the ten firsts will be displayed.
Now, choose the image you want, click on it and within seconds, the results will be displayed ! All the words found will be marked by a colored line in the grid. The results are also saved in your files with the name "results.png" and you'll also find the grid of your image written in a txt file under the name "grid_solver.txt".

```mermaid
flowchart TD
    %% Entrée globale en dehors des sous-graphes
    IMG_IN["Input Image<br/>(Selected from READ_GRID/images)"]

    %% Step 1
    subgraph STAGE_1["1. Pretreatment"]
        PRE["Pretreatment Processing"]
        GRAY["Contrast & Color Correction"]
        BIN["Noise Reduction"]
        CORR["Angle Correction"]

        PRE --> GRAY --> BIN --> CORR
    end



    IMG_IN --> PRE


        EXT["Letter Extraction"]
        
        CORR --> EXT

    %% Step 2
    subgraph STAGE_3["3. Grid & Word Recognition"]
        REC_G["Recognition of the letters in thegrid"]
        REC_W["Recognition of every word"]
        DEL["Deletion of useless information in the image"]

        EXT --> REC_G & REC_W --> DEL
    end

    %% Step 3
    subgraph STAGE_4["4. OCR ai"]
        AI["Recognition of every character using OCR AI"]

        DEL --> AI
    end

    %% Step 4
    subgraph STAGE_5["5. Solver Module"]
        SOLVER["Finfing every word thanks to solver algorithm"]

        AI --> SOLVER
    end

    WRITE["Write Answers on Image<br/>(Draw red lines over found words)"]
    OUT_IMG[("results shown in the interface")]

    WRITE --> OUT_IMG

    %% Connexions finales et liaison avec l'image d'origine
    SOLVER --> WRITE
    IMG_IN -..-> WRITE
```
