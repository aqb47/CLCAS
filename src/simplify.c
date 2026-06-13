#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <math.h>

static int simplify_lowest_branch(Node* node);
static int is_binary_operation(NodeType node_type);

void simplify(Node* node) {
    // Continuously attempt simplification of AST until no more simplification operations can be performed 
    while (1) {
        if (simplify_lowest_branch(node) == 0) {
            return;
        }
    }
}

// Simplify at lowest possible branches of AST and return how many simplification operations were performed. No simplification operation = most simplified form of AST
int simplify_lowest_branch(Node* node) {
    // How many times we've performed a simplification operation
    int simplify_count = 0;

    // Simplify by calculating new node value for certain binary operation nodes in AST
    if (is_binary_operation(node->type)) {
        // NUMBER (OPERATION) NUMBER
        if (node->BinaryOperation.left_child->type == NODE_NUM && node->BinaryOperation.right_child->type == NODE_NUM) {
            double new_node_number;

            // Perform binary operation
            switch (node->type) {
                case(NODE_ADD): 
                    new_node_number = node->BinaryOperation.left_child->number + node->BinaryOperation.right_child->number; 
                    break;
                case(NODE_SUB): 
                    new_node_number = node->BinaryOperation.left_child->number - node->BinaryOperation.right_child->number; 
                    break;
                
                case(NODE_MUL): 
                    new_node_number = node->BinaryOperation.left_child->number * node->BinaryOperation.right_child->number; 
                    break;
                case(NODE_DIV): 
                    new_node_number = node->BinaryOperation.left_child->number / node->BinaryOperation.right_child->number; 
                    break;
                
                case(NODE_POW): 
                    new_node_number = pow(node->BinaryOperation.left_child->number, node->BinaryOperation.right_child->number); 
                    break;

                default: return simplify_count;
            }

            simplify_count += 1;

            // Free left and right children (which were just numeric nodes)
            node_free(node->BinaryOperation.left_child);
            node_free(node->BinaryOperation.right_child);

            // Set new node
            node->number = new_node_number;
            node->type = NODE_NUM;
        }
        
        // Otherwise recursively simplify children of binary operation
        else {
            simplify_count += simplify_lowest_branch(node->BinaryOperation.left_child);
            simplify_count += simplify_lowest_branch(node->BinaryOperation.right_child);
        }
    }

    // Rrecursively simplify unary operation children
    else if (node->type == NODE_NEG) {
        simplify_count += simplify_lowest_branch(node->UnaryOperation.child);
    }
    else if (node->type == NODE_FUNC) {
        simplify_count += simplify_lowest_branch(node->Function.child);
    }

    return simplify_count;
}

int is_binary_operation(NodeType node_type) {
    if (node_type == NODE_ADD || node_type == NODE_SUB || node_type == NODE_MUL || node_type == NODE_DIV || node_type == NODE_POW) {
        return 1;
    }
    else {
        return 0;
    }
}