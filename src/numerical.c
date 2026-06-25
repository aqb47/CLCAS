#include "ast.h"
#include "diff.h"
#include "simplify.h"
#include "numerical.h"

#include <math.h>

static long factorial(long number);

double node_eval(Node* node, char variable, double variable_value) {
    // Simplify node first
    node = simplify(node);

    // Base cases
    if (node->type == NODE_NUM) {
        return node->number;
    }
    else if (node->type == NODE_VAR) {
        if (node->variable == variable) {
            return variable_value;
        }
        // Treat other variables = 0 for now
        else {
            return 0.0;
        }
    }

    // Binary operations
    else if (is_binary_operation(node->type)) {
        double left_child_eval = node_eval(node->BinaryOperation.left_child, variable, variable_value);
        double right_child_eval = node_eval(node->BinaryOperation.right_child, variable, variable_value);

        return evaluate_binop(node->type, left_child_eval, right_child_eval);
    }
    
    // Unary stuff
    else if (node->type == NODE_NEG) {
        return -node_eval(node->UnaryOperation.child, variable, variable_value);
    }
    else if (node->type == NODE_FUNC) {
        return evaluate_function(node->Function.function_name, node_eval(node->Function.child, variable, variable_value));
    }

    else {
        return NAN;
    }
}

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
    Node* result;

    // From 0 to n
    for (int i = 0; i <= order; i++) {
        // fn(a) / n!
        double coefficient = node_eval(current_derivative, variable, center) / factorial(i);

        // (fn(a) / n!) * (x - a) ^ n
        Node* term = node_binop(NODE_MUL, 
                        node_num(coefficient), 
                        node_binop(NODE_POW, node_copy(base), node_num(i))
                    );

        // First value means no addition
        if (i == 0) {
            result = term;
        }
        // Else add term to result
        else {
            result = node_binop(NODE_ADD, result, term);
        }

        // Advance to next derivative
        Node* next = simplify(differentiate(current_derivative, variable));
        node_free(current_derivative);
        current_derivative = next;
    }

    node_free(current_derivative);
    node_free(base);

    return result;
}

Node* maclaurin_series(Node* node, char variable, int order) {
    return taylor_series(node, variable, 0, order);
}