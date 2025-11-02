/* neuronal netword (2 inputs -> 2 hidden -> 1 output)
   learn XNOR : A.B + !A.!B <=> A = B
*/

#include "AInxor.h"


//random function -> [a,b]
double rnd(double a, double b) { 
	return a + (b - a) * ((double)rand() / RAND_MAX); //rand() (stdlib function) ->  [0, RAND_MAX]
}

//sigmiod function -> [0,1]
double sigmoid(double x) { 
	return 1 / (1 + exp(-x)); 
}

//sigmoid prime -> error value
double sigmoid_prime(double x) {
    double y = sigmoid(x);
    return y * (1 - y); 
}

int main(int argc, char* argv[]) {
    srand((unsigned)time(NULL)); //set a random seed depending on the time (change between two runs)

    int X[4][2] = { {0,0}, {0,1}, {1,0}, {1,1} }; //every possible inputs
    int targets[4] = {1, 0, 0, 1}; //expected outputs

    //hyperparameters 
    //default value
    double lr = 0.6;      	//learning rate
    int max_gens = 20000; 	//max iterations
    double error_goal = 1e-6;	//the min error reachable

    //value set with argv
    if (argc > 1){
	int mg_input = atoi(argv[1]);
        if (mg_input == 0)
	    errx(EXIT_FAILURE, "the first argument must be a positive int > 0  ( N* )");
    	max_gens = mg_input;
    }
    if (argc > 2){
    	double lr_input = atof(argv[2]);
	if (lr_input == 0 || lr_input > 1)
		errx(EXIT_FAILURE, "the second argument must be a float > 0.0 && <= 1.0 ( ]0,1] )");
	lr = lr_input;
    }
    if (argc > 3){
   	int err_goal_input = atoi(argv[3]);
        if (err_goal_input == 0)
		errx(EXIT_FAILURE, "the third argument must be a positive int > 0  ( N* )");
	error_goal = pow(10, -err_goal_input);

    }
    if (argc > 4){
    	printf("ERROR : more than three parameters were given, they are useless.\n");
    }
    printf("REMEMBER parameters : 1 -> int max_gen (N*) | 2 -> double learning rate (]0,1]) | 3 -> int error_goal (num of nubers after .) (N*)\n\n---------- Training ----------\n\n");



    //initialisation of weights : input->hidden (2*2), hidden bias (2), hidden->out (2*1), out bias (1)
    double w_ih[2][2];
    double b_h[2];
    double w_ho[2];
    double b_o;

    //random initialisation
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) w_ih[i][j] = rnd(-1, 1);
        b_h[i] = rnd(-1, 1);
        w_ho[i] = rnd(-1, 1);
    }
    b_o = rnd(-1, 1);

    //training
    int gen;
    double total_error = 0;
    int count = 0;
    for (gen = 0; gen < max_gens; ++gen) {
        total_error = 0;

        for (int p = 0; p < 4; ++p) {
            double t = (double)targets[p];

            //forward -->
            double x0 = X[p][0];
            double x1 = X[p][1];
            double net_h0 = w_ih[0][0]*x0 + w_ih[0][1]*x1 + b_h[0];
            double net_h1 = w_ih[1][0]*x0 + w_ih[1][1]*x1 + b_h[1];
            double h0 = sigmoid(net_h0);
            double h1 = sigmoid(net_h1);
            double net_o = w_ho[0]*h0 + w_ho[1]*h1 + b_o;
            double o = sigmoid(net_o);

            //error
            double err = 0.5 * (t - o) * (t - o);
            total_error += err;

            //backward <--
            //output
            double delta_o = (t - o) * sigmoid_prime(net_o);

            //hidden
            double delta_h0 = sigmoid_prime(net_h0) * (delta_o * w_ho[0]);
            double delta_h1 = sigmoid_prime(net_h1) * (delta_o * w_ho[1]);

            //weights update
            //hidden -> out
            w_ho[0] += lr * delta_o * h0;
            w_ho[1] += lr * delta_o * h1;
            b_o     += lr * delta_o * 1;

            //input -> hidden
            w_ih[0][0] += lr * delta_h0 * x0;
            w_ih[0][1] += lr * delta_h0 * x1;
            b_h[0]     += lr * delta_h0 * 1;

            w_ih[1][0] += lr * delta_h1 * x0;
            w_ih[1][1] += lr * delta_h1 * x1;
            b_h[1]     += lr * delta_h1 * 1;
        }

        //print progression
        if (gen % (max_gens / 10) == 0) {
            printf("-%i | total_error = %.10f | gen = %i\n", count, total_error, gen);
	    count ++;
        }

        if (total_error < error_goal) {
            printf("\nConverged (error < %.1e) at gen %i\n", error_goal, gen);
            break;
        }
    }

    if (gen >= max_gens) {
        printf("\nEnd: reached %i gens. last total_error = %.8f\n", max_gens, total_error);
    }
    printf("\n---------- Final ----------\n");

    //print final weights
    printf("\nFinal weights :\nInput : ");
    for (int i=0;i<2;i++){
	    printf("w_ho[%i]=%.6f   ", i, w_ho[i]);
    }
    for (int i=0;i<2;i++) {
        printf("\nHidden %i : w_ih[%i]=[%.6f, %.6f] b_h=%.6f\n", i, i, w_ih[i][0], w_ih[i][1], b_h[i]);
    }
    printf("Output : b_o=%.6f\n", b_o);
    printf("\n---------- Tests ----------\n\n");

    //final tests
    printf("Tests ( input {0,1}^2 -> output [0,1] -> rounded {0,1} ) :\n");
    for (int p = 0; p < 4; ++p) {

        double x0 = X[p][0];
        double x1 = X[p][1];
        double net_h0 = w_ih[0][0]*x0 + w_ih[0][1]*x1 + b_h[0];
        double net_h1 = w_ih[1][0]*x0 + w_ih[1][1]*x1 + b_h[1];
        double h0 = sigmoid(net_h0);
        double h1 = sigmoid(net_h1);
        double net_o = w_ho[0]*h0 + w_ho[1]*h1 + b_o;
        double o = sigmoid(net_o);
        int out = o >= 0.5 ? 1 : 0;

        printf("%i %i -> %.6f -> %i (success = %s)\n", X[p][0], X[p][1], o, out, targets[p] == out ? "True" : "False");
    }

    //generate a png if dot is installed
    if (system("command -v dot >/dev/null 2>&1") == 0)
        generate_network(w_ih, w_ho, b_h, b_o);
    else
        printf("\nInstall .dot to generate a .png of the network !\nThe command to install it = sudo apt install graphviz\n");

    return EXIT_SUCCESS;
}



void generate_network(double w_ih[2][2], double w_ho[2], double b_h[2], double b_o) {
    FILE *f = fopen("network.dot", "w");
    if (f == 0) 
        return;

    fprintf(f, "digraph neural_net {\n");   //type of graph
    fprintf(f, "    rankdir=LR;\n");    //right to left and not up to down
    fprintf(f, "    splines=false;\n"); //straight lines
    fprintf(f, "    node [shape=circle, style=filled, fontname=\"Arial\"];\n\n");

    //layers
    fprintf(f, "    subgraph cluster_input {\n");   //definition of the input layer
    fprintf(f, "        label=\"Input Layer\"; color=forestgreen; style=dashed;\n");
    fprintf(f, "        A [label=\"A\", fillcolor=forestgreen];\n");
    fprintf(f, "    Xspace [style=invis, width=0, height=0];\n");
    fprintf(f, "        B [label=\"B\", fillcolor=forestgreen];\n");
    fprintf(f, "    }\n\n");

    fprintf(f, "    subgraph cluster_hidden {\n");  //definition of the hidden layer
    fprintf(f, "        label=\"Hidden Layer\"; color=blue; style=dashed;\n");
    fprintf(f, "        H1 [label=\"%.2f\", fillcolor=lightblue];\n", b_h[0]);
    fprintf(f, "        H2 [label=\"%.2f\", fillcolor=lightblue];\n", b_h[1]);
    fprintf(f, "    }\n\n");

    fprintf(f, "    subgraph cluster_output {\n");  //definition of the output layer
    fprintf(f, "        label=\"Output Layer\"; color=orange; style=dashed;\n");
    fprintf(f, "        O [label=\"%.2f\", fillcolor=gold, shape=doublecircle];\n", b_o);
    fprintf(f, "    }\n\n");

    //links input -> hidden (set the color of the links and the value of the weights)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            fprintf(f, "    %c -> H%d [label=\"%+.2f\", color=%s];\n",
                'A' + i, 
                j+1,
                w_ih[i][j],
                w_ih[i][j] >= 0 ? "black" : "grey49");
        }
    }

    //links hidden -> output (set the color of the links and the value of the weights)
    for (int j = 0; j < 2; j++) {
        fprintf(f, "    H%d -> O [label=\"%+.2f\", color=%s];\n",
            j+1,
            w_ho[j],
            w_ho[j] >= 0 ? "black" : "grey49");
    }

    fprintf(f, "}\n");
    fclose(f);

    //generate the png
    system("dot -Tpng network.dot -o network.png");
    system("rm network.dot");

    printf("\nThe file network.png has been generated, open it to see the network\n");
}

/*
 * If we want to use it, it is necessary to make weights global
double calculate(int x0, int x1){
    double net_h0 = w_ih[0][0]*x0 + w_ih[0][1]*x1 + b_h[0];
    double net_h1 = w_ih[1][0]*x0 + w_ih[1][1]*x1 + b_h[1];
    double h0 = sigmoid(net_h0);
    double h1 = sigmoid(net_h1);
	double net_o = w_ho[0]*h0 + w_ho[1]*h1 + b_o;

	return sigmoid(net_o);
}
*/
