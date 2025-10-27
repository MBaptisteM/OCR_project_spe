#pragma once

//to print outputs
#include <stdio.h>

//to use rand()
#include <stdlib.h>

//to calculate sigmoid function
#include <math.h>

//to change the random seed between two runs
#include <time.h>

//to return error if the outputs are wrong
#include <err.h>


double rnd(double a, double b);
double sigmoid(double x);
double sigmoid_prime(double x);
void generate_network(double w_ih[2][2], double w_ho[2], double b_h[2], double b_o);
//double calculate(int x0, int x1);

