#ifndef NUMERICAL_H
#define NUMERICAL_H

// Within tree replace instances of variable with variable value and return the total evaluated value of tree
double node_eval(Node* node, char variable, double variable_value);

// Represent function as infinite sum of polynomials
// e.g. f(x - a) = f(a)/0! * (x - a) ^ 0 + f1(a)/1! * (x - a) ^ 1 + ... + fn(a)/n! * (x - a) ^ n 
// [For 'n' as order, 'x' as variable and 'a' as center]
Node* taylor_series(Node* node, char variable, double center, int order);

#endif