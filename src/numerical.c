#include "ast.h"
#include "simplify.h"
#include "numerical.h"

#include <math.h>

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