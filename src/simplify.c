#include "simplify.h"
#include "parser.h"
#include "ast.h"

#include <string.h>
#include <ctype.h>
#include <math.h>

Node* simplify(Node* node) {
    if (is_binary_operation(node->type)) {
        // Go deeper for non-numerical child nodes
        node->BinaryOperation.left_child = simplify(node->BinaryOperation.left_child);
        node->BinaryOperation.right_child = simplify(node->BinaryOperation.right_child);

        // If both children of binary operation are numeric we'll do direct calculations
        if (node->BinaryOperation.left_child->type == NODE_NUM && node->BinaryOperation.right_child->type == NODE_NUM) {
            double left_child_number = node->BinaryOperation.left_child->number;
            double right_child_number = node->BinaryOperation.right_child->number;
            
            double new_node_number;

            new_node_number = evaluate_binop(node->type, left_child_number, right_child_number);

            node_free(node);
            node = node_num(new_node_number);
        }

        // Only left child is numerical
        else if (node->BinaryOperation.left_child->type == NODE_NUM) {
            // Cases with 0 as left part of binary operation
            if (node->BinaryOperation.left_child->number == 0) {
                switch(node->type) {
                    // 0+ -> right child remains same
                    case(NODE_ADD): {
                        Node* right_child = node->BinaryOperation.right_child;

                        node_free(node->BinaryOperation.left_child);

                        free(node);
                        node = right_child;

                        break;
                    }

                    // 0- -> same as negation
                    case(NODE_SUB): {
                        Node* right_child = node->BinaryOperation.right_child;

                        node_free(node->BinaryOperation.left_child);

                        free(node);
                        node = node_neg(right_child);

                        break;
                    }

                    // 0*, 0/, 0^ -> 0. Except when x < 0, 0^x is undefined but we'll avoid that edge case for now
                    case(NODE_MUL):
                        // Fall through
                    case(NODE_DIV):
                        // Fall through
                    case(NODE_POW):
                        node_free(node);
                        node = node_num(0);
                        break;

                    default: break;
                }
            }

            // Cases with 1 as left part of binary operator
            else if (node->BinaryOperation.left_child->number == 1) {
                switch(node->type) {
                    // 1* -> nothing changes
                    case(NODE_MUL): {
                        Node* right_child = node->BinaryOperation.right_child;
                        
                        node_free(node->BinaryOperation.left_child);

                        free(node);
                        node = right_child;

                        break;
                    }

                    // 1^ -> 1
                    case(NODE_POW):
                        node_free(node);
                        node = node_num(1);
                        break;
                    
                    default: break;
                }
            }
        }

        // Only right child is only numerical
        else if (node->BinaryOperation.right_child->type == NODE_NUM) {
            // Cases with 0 as right part of binary operation
            if (node->BinaryOperation.right_child->number == 0) {
                switch(node->type) {
                    // +0, -0 -> left child remains same. That is, the new node is just the left node
                    case(NODE_ADD):
                        // Fall through
                    case(NODE_SUB): {
                        Node* left_child = node->BinaryOperation.left_child;

                        node_free(node->BinaryOperation.right_child);

                        free(node);
                        node = left_child;

                        break;
                    }

                    // *0 -> 0
                    case(NODE_MUL):
                        node_free(node);
                        node = node_num(0);
                        break;
                    // /0 -> inf
                    case(NODE_DIV):
                        node_free(node);
                        node = node_num(INFINITY);
                        break;

                    // ^0 -> 1
                    case(NODE_POW):
                        node_free(node);
                        node = node_num(1);
                        break;

                    default: break;
                }
            }

            // Cases with 1 as right part of binary operation
            else if (node->BinaryOperation.right_child->number == 1) {
                switch(node->type) {
                    // *1, /1, ^1 -> nothing changes
                    case(NODE_MUL):
                        // Fall through
                    case(NODE_DIV):
                        // Fall through
                    case(NODE_POW): {
                        Node* left_child = node->BinaryOperation.left_child;

                        node_free(node->BinaryOperation.right_child);

                        free(node);
                        node = left_child;

                        break;
                    }
                    
                    default: break;
                }
            }
        }
    }

    // Unary stuff
    else if (node->type == NODE_NEG) {
        node->UnaryOperation.child = simplify(node->UnaryOperation.child);

        // If child is numeric node can be replaced directly with negative numeric value
        if (node->UnaryOperation.child->type == NODE_NUM) {
            double new_node_number = -node->UnaryOperation.child->number;

            node_free(node->UnaryOperation.child);
            free(node);

            node = node_num(new_node_number);
        }
        // Double negation -> nothing changes
        else if (node->UnaryOperation.child->type == NODE_NEG) {
            Node* grand_child = node->UnaryOperation.child->UnaryOperation.child;

            free(node->UnaryOperation.child);
            free(node);

            node = grand_child;
        }
    }

    else if (node->type == NODE_FUNC) {
        node->Function.child = simplify(node->Function.child);

        // Function can be evaluated directly as its child is numerical
        if (node->Function.child->type == NODE_NUM) {
            double new_node_number;

            new_node_number = evaluate_function(node->Function.function_name, node->Function.child->number);

            // Use new value if we found a valid function, else leave as is
            if (!isnan(new_node_number)) {
                node_free(node);
                node = node_num(new_node_number);
            }
        }
    }

    return node;
}

double evaluate_binop(NodeType binop, double left_child_value, double right_child_value) {
    switch(binop) {
        case(NODE_ADD): return left_child_value + right_child_value;
        case(NODE_SUB): return left_child_value - right_child_value;

        case(NODE_MUL): return left_child_value * right_child_value;
        case(NODE_DIV): return left_child_value / right_child_value;

        case(NODE_POW): return pow(left_child_value, right_child_value);

        default: return NAN;
    }
}

double evaluate_function(const char* function, double child_value) {
    // Natural logarithm
    if (strcmp(function, "ln") == 0) {
        return log(child_value);
    }
    // Exponent
    else if (strcmp(function, "exp") == 0) {
        return exp(child_value);
    }

    // Sine
    else if (strcmp(function, "sin") == 0) {
        return sin(child_value);
    }
    // Cosine
    else if (strcmp(function, "cos") == 0) {
        return cos(child_value);
    }
    // Tangent
    else if (strcmp(function, "tan") == 0) {
        return tan(child_value);
    }

    // Unknown function
    else {
        return NAN;
    }
}