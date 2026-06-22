#include "ast.h"
#include "simplify.h"
#include "diff.h"

#include <math.h>
#include <string.h>

Node* differentiate(const Node* node, char variable) {
    Node* derivative;

    // For constants
    if (node->type == NODE_NUM) {
        // d/dx(c) = 0 
        derivative = node_num(0);
    }

    // For variables
    else if (node->type == NODE_VAR) {
        // d/dx(x) = 1
        if (node->variable == variable) {
            derivative = node_num(1);
        }
        // d/dx(y) = 0
        else {
            derivative = node_num(0);
        }
    }

    // If we have binary operations
    else if (is_binary_operation(node->type)) {
        Node* f = node->BinaryOperation.left_child;
        Node* g = node->BinaryOperation.right_child;

        Node* f_prime = differentiate(f, variable);
        Node* g_prime = differentiate(g, variable);

        // d/dx(f + g) = f' + g' and d/dx(f - g) = f' - g'
        if (node->type == NODE_ADD || node->type == NODE_SUB) {
            derivative = node_binop(node->type, f_prime, g_prime);
        }

        // d/dx(f * g) = f * g' + f' * g
        else if (node->type == NODE_MUL) {
            derivative = node_binop(NODE_ADD, 
                            node_binop(NODE_MUL, node_copy(f), g_prime),
                            node_binop(NODE_MUL, f_prime, node_copy(g))
                        );
        }

        // d/dx(f / g) = (g * f' - f * g') / g ^ 2
        else if (node->type == NODE_DIV) {
            derivative = node_binop(NODE_DIV,
                            node_binop(NODE_SUB,
                                    node_binop(NODE_MUL, node_copy(g), f_prime),
                                    node_binop(NODE_MUL, node_copy(f), g_prime)
                            ),
                            node_binop(NODE_POW,
                                node_copy(g), node_num(2)
                            )
                        );
        }

        // d/dx(f^g) = f^g * (g * (f' / f) + ln(f) * g')
        else if (node->type == NODE_POW) {
            derivative = node_binop(NODE_MUL,
                            node_binop(NODE_POW, node_copy(f), node_copy(g)),
                            node_binop(NODE_ADD,
                                node_binop(NODE_MUL, node_func("ln", node_copy(f)), g_prime),
                                node_binop(NODE_MUL, node_binop(NODE_DIV, f_prime, node_copy(f)), node_copy(g))
                            )
                        );
        }
    }

    // Unary stuff
    else if (node->type == NODE_FUNC) {
        // In case of an unknown function we'll add a ' after its name to indicate derivative (e.g sin(x) -> sin'(x))
        char function_prime[FUNCTION_NAME_LENGTH + 1];
        snprintf(function_prime, sizeof(function_prime), "%s'", node->Function.function_name);

        Node* g = node->Function.child;
        Node* g_prime = differentiate(g, variable);

        // d/dx(sin(g)) = cos(g) * g'
        if (strcmp(node->Function.function_name, "sin") == 0) {
            derivative = node_binop(NODE_MUL,
                            node_func("cos", node_copy(g)),
                            g_prime
                        );
        }
        // d/dx(cos(g)) = - sin(g) * g'
        else if (strcmp(node->Function.function_name, "cos") == 0) {
            derivative = node_neg(
                            node_binop(NODE_MUL,
                                node_func("sin", node_copy(g)),
                                g_prime
                            )
                        );
        }
        // d/dx(tan(g)) = (1 / cos(g)) ^ 2 * g'
        else if (strcmp(node->Function.function_name, "tan") == 0) {
            derivative = node_binop(NODE_MUL,
                            node_binop(NODE_POW,
                                node_binop(NODE_DIV,
                                    node_num(1), node_func("cos", node_copy(g))
                                ),
                                node_num(2)
                            ),
                            g_prime
                        );
        }
        // d/dx(exp(g)) = exp(g) * g' 
        else if (strcmp(node->Function.function_name, "exp") == 0) {
            derivative = node_binop(NODE_MUL,
                            node_func("exp", node_copy(g)),
                            g_prime
                        );
        }
        // d/dx(ln(g)) = g' / g
        else if (strcmp(node->Function.function_name, "ln") == 0) {
            derivative = node_binop(NODE_DIV,
                            g_prime,
                            node_copy(g)
                        );
        }
        // d/dx(f(g)) = f'(g) * g'
        else {
            derivative = node_binop(NODE_MUL,
                            node_func(function_prime, node_copy(g)),
                            g_prime
                        );
        }
    }
    else if (node->type == NODE_NEG) {
        Node* g = node->UnaryOperation.child;
        Node* g_prime = differentiate(g, variable);

        derivative = node_neg(g_prime);
    }

    return derivative;
}