#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <math.h>

static int is_binary_operation(NodeType node_type);

void simplify(Node* node) {
    // If left and right child of binary operations are both numerical we can simplify by calculating new node value
    if (is_binary_operation(node->type)) {
        if (node->BinaryOperation.left_child->type == NODE_NUM && node->BinaryOperation.right_child->type == NODE_NUM) {
            double new_node_number;

            // Perform binary operation
            switch (node->type) {
                case(NODE_ADD): new_node_number = node->BinaryOperation.left_child->number + node->BinaryOperation.right_child->number; break;
                case(NODE_SUB): new_node_number = node->BinaryOperation.left_child->number - node->BinaryOperation.right_child->number; break;
                
                case(NODE_MUL): new_node_number = node->BinaryOperation.left_child->number * node->BinaryOperation.right_child->number; break;
                case(NODE_DIV): new_node_number = node->BinaryOperation.left_child->number / node->BinaryOperation.right_child->number; break;
                
                case(NODE_POW): new_node_number = pow(node->BinaryOperation.left_child->number, node->BinaryOperation.right_child->number); break;

                default: printf("Unknown operation\n"); return;
            }

            // Free left and right children (which were just numeric nodes)
            node_free(node->BinaryOperation.left_child);
            node_free(node->BinaryOperation.right_child);

            // Set new node
            node->number = new_node_number;
            node->type = NODE_NUM;
        }
        // Otherwise recursively simplify children of binary operation
        else {
            simplify(node->BinaryOperation.left_child);
            simplify(node->BinaryOperation.right_child);
        }
    }

    // Rrecursively simplify unary operation children
    else if (node->type == NODE_NEG) {
        simplify(node->UnaryOperation.child);
    }
    else if (node->type == NODE_FUNC) {
        simplify(node->Function.child);
    }
}

int is_binary_operation(NodeType node_type) {
    if (node_type == NODE_ADD || node_type == NODE_SUB || node_type == NODE_MUL || node_type == NODE_DIV || node_type == NODE_POW) {
        return 1;
    }
    else {
        return 0;
    }
}