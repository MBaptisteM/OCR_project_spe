How to use our OCR ?
First, type "make all" in your terminal. Then, execute the ocr file using ./all without any arguments.
The first menu appears and with it, two buttons. 
For training you AI, click on the training button. A new window appears and you will be able to see the training of your AI. To stop the training, click on the stop button. The training will automatically be saved. You then will be back on the first menu window.
Now, if you want to resolve a grid, the picture you want must be in the images folder located in the READ_GRID folder. 
If you do not have any pictures, nothing will appear. Between one and ten, they will all be displayed. However, if you have more than ten pictures, only the ten firsts will be displayed.
Now, choose the image you want, click on it and within seconds, the results will be displayed ! All the words found will be marked by a colored line in the grid. The results are also saved in your files with the name "results.png" and you'll also find the grid of your image written in a txt file under the name "grid_solver.txt".

```mermaid
flowchart TB
    subgraph UI["Gradio UI (app.py) — presets + Advanced + Batch/Sweep + browser + panorama"]
        CTRL["Photo upload + feature controls<br/>(roundness · saturation · brightness · hue ·<br/>leaf class · kawaii mode · element · light · room · ceiling · furniture · seed/steps/CFG)"]
    end

    CTRL --> MODE{"PROMPT_MODE"}

    %% Template path
    MODE -->|template| TPL["build_prompt_segments → optimize_prompt<br/>(seeded synonym pools; IVs fixed)"]

    %% VL path (no retrieval / index)
    MODE -->|vl| VLGEN["VL generator (prompt_rag)<br/>select static grounding by features"]
    TAX["static citation-tagged knowledge<br/>species per leaf class · kawaii archetypes · biophilic/interior"] --> VLGEN
    VLGEN --> VL["ChatOllama vision · Qwen3-VL (sees photo)<br/>JSON output + anti-repeat memory"]
    VL --> RAGOUT["grounded description (+ provenance)"]
    VLGEN -. "fallback: Ollama down / error / control" .-> TPL

    TPL --> INSTR["edit_instruction()<br/>+ preservation + modest-scale clause"]
    RAGOUT --> INSTR

    PHOTO["input photo → aspect-fit, /16-aligned"] --> GEN
    INSTR --> GEN["Qwen-Image-Edit-2511<br/>Nunchaku NVFP4 (5080) · bnb 4-bit fallback<br/>CPU offload · instruction edit, no mask · pipeline.py"]

    GEN --> PRES{"PRESERVE_UNCHANGED?"}
    PRES -->|"off (default)"| OUT["result image"]
    PRES -->|"on (opt-in)"| COMP["_composite_preserve:<br/>original detail + generated tone;<br/>element kept by hysteresis"] --> OUT

    OUT --> ANA["analysis.py<br/>fractal D on changed region · optional CLIP score · research explanation"]
    OUT --> PANO["360° panorama (py360convert, no GPU)"]
    ANA --> DSET[("dataset.py<br/>PNG + JSON + dataset.jsonl/csv<br/>3 condition folders + provenance")]

    %% Two-machine pipelining for batch/sweep
    VLGEN -. "remote VL host (Mac / cuda:1), kept warm; producer/consumer overlap with render" .-> GEN
```