#include "ast.h"
#include "simplify.h"
#include "diff.h"

#include <math.h>

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
                            node_binop(NODE_MUL, f, g_prime),
                            node_binop(NODE_MUL, f_prime, g)
                        );
        }

        // d/dx(f / g) = (g * f' - f * g') / g ^ 2
        else if (node->type == NODE_DIV) {
            derivative = node_binop(NODE_DIV,
                            node_binop(NODE_SUB,
                                    node_binop(NODE_MUL, g, f_prime),
                                    node_binop(NODE_MUL, f, g_prime)
                            ),
                            node_binop(NODE_POW,
                                g, node_num(2)
                            )
                        );
        }

        // d/dx(f^g) = f^g * (g * (f' / f) + ln(f) * g')
        else if (node->type == NODE_POW) {
            derivative = node_binop(NODE_MUL,
                            node_binop(NODE_POW, f, g),
                            node_binop(NODE_ADD,
                                node_binop(NODE_MUL, node_func("ln", f), g_prime),
                                node_binop(NODE_MUL, node_binop(NODE_DIV, f_prime, f), g)
                            )
                        );
        }

        // d/dx(f(g)) = f' * g'
        else if (node->type == NODE_FUNC) {
            printf("I'll come back to this later\n");
        }
    }

    return derivative;
}