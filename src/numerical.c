#include "ast.h"
#include "diff.h"
#include "simplify.h"
#include "numerical.h"

#include <math.h>

// Maximum recursive depth of adaptive Simpson integration
#define MAX_DEPTH 50

static long factorial(long number);
static double simpson(Node* node, char variable, double a, double b);
static double adaptive_simpson(Node* node, char variable, double a, double b, double integral_whole, double tolerance, int depth);

long factorial(long number) {
    if (number == 1 || number == 0) {
        return 1;
    }

    return number * factorial(number - 1);
}

Node* taylor_series(Node* node, char variable, double center, int order) {
    Node* current_derivative = node_copy(node);

    // (x - a)
    Node* base = node_binop(NODE_SUB, node_var(variable), node_num(center));
    Node* result = NULL;

    // From 0 to n
    for (int i = 0; i <= order; i++) {
        // fn(a) / n!
        double coefficient = node_eval(current_derivative, variable, center) / factorial(i);

        // Add non-zero terms
        if (fabs(coefficient) > 1e-14) {
            // (fn(a) / n!) * (x - a) ^ n
            Node* term = node_binop(NODE_MUL, 
                            node_num(coefficient), 
                            node_binop(NODE_POW, node_copy(base), node_num(i))
                        );

            // If result is empty we don't add anything
            if (result == NULL) {
                result = term;
            }
            // Else add term to result
            else {
                result = node_binop(NODE_ADD, result, term);
            }
        }

        // Advance to next derivative
        if (i < order) {
            Node* next = simplify(differentiate(current_derivative, variable));
            node_free(current_derivative);
            current_derivative = next;
        }
    }

    node_free(current_derivative);
    node_free(base);

    return result;
}

Node* maclaurin_series(Node* node, char variable, int order) {
    return taylor_series(node, variable, 0, order);
}

double newton_rhapson_root(Node* node, char variable, double x0, double tolerance, int max_iteration) {
    Node* derivative = simplify(differentiate(node, variable));
    double x1;

    // Keep performing operation until we hit max iterations
    for (int i = 0; i < max_iteration; i++) {
        double f_x0 = node_eval(node, variable, x0);

        double f_prime_x0 = node_eval(derivative, variable, x0);
        if (fabs(f_prime_x0) < 1e-14) {
            break;
        }

        x1 = x0 - f_x0 / f_prime_x0;
        if (fabs(x1 - x0) <= tolerance) {
            x0 = x1;
            break;
        }

        x0 = x1;
    }
    
    node_free(derivative);
    return x0;
}

// Perform Simpson's rule for n = 2
double simpson(Node* node, char variable, double a, double b) {
    double midpoint = (a + b) / 2;

    return ((b - a) / 6) * (node_eval(node, variable, a) + 4 * node_eval(node, variable, midpoint) + node_eval(node, variable, b));
}

double adaptive_simpson(Node* node, char variable, double a, double b, double integral_whole, double tolerance, int depth) {
    double midpoint = (a + b) / 2;
    
    double integral_left = simpson(node, variable, a, midpoint);
    double integral_right = simpson(node, variable, midpoint, b);

    double error = (integral_left + integral_right - integral_whole) / 15;

    if (fabs(error) <= tolerance || depth > MAX_DEPTH) {
        return integral_left + integral_right + error;
    }

    integral_left = adaptive_simpson(node, variable, a, midpoint, integral_left, tolerance / 2, depth + 1);
    integral_right = adaptive_simpson(node, variable, midpoint, b, integral_right, tolerance / 2, depth + 1);

    return integral_left + integral_right;
}

double simpson_integrate(Node* node, char variable, double a, double b, double tolerance) {
    return adaptive_simpson(node, variable, a, b, simpson(node, variable, a, b), tolerance, 0);
}