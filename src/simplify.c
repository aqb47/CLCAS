#include "simplify.h"
#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <math.h>

static int simplify_lowest_branch_in_place(Node* node);
static int is_binary_operation(NodeType node_type);

Node* simplify(Node* node) {
    if (is_binary_operation(node->type)) {
        // Go deeper for non-numerical child nodes
        if (node->BinaryOperation.left_child->type != NODE_NUM) {
            node->BinaryOperation.left_child = simplify(node->BinaryOperation.left_child);
        }
        if (node->BinaryOperation.right_child->type != NODE_NUM) {
            node->BinaryOperation.right_child = simplify(node->BinaryOperation.right_child);
        }

        // If both children of binary operation are numeric we'll do direct calculations
        if (node->BinaryOperation.left_child->type == NODE_NUM && node->BinaryOperation.right_child->type == NODE_NUM) {
            double left_child_number = node->BinaryOperation.left_child->number;
            double right_child_number = node->BinaryOperation.right_child->number;
            
            double new_node_number;

            switch(node->type) {
                case(NODE_ADD): new_node_number = left_child_number + right_child_number; break;
                case(NODE_SUB): new_node_number = left_child_number - right_child_number; break;
                
                case(NODE_MUL): new_node_number = left_child_number * right_child_number; break;
                case(NODE_DIV): new_node_number = left_child_number / right_child_number; break;
                
                case(NODE_POW): new_node_number = pow(left_child_number, right_child_number); break;

                default: break;
            }

            node_free(node);
            node = node_num(new_node_number);
        }

        // Only left child is numerical
        else if (node->BinaryOperation.left_child->type == NODE_NUM) {
            // Cases with 0 as left part of binary operation
            if (node->BinaryOperation.left_child->number == 0) {
                switch(node->type) {
                    // 0+, 0- -> right child remains same. That is, the new node is just the right node
                    case(NODE_ADD):
                        // Fall through
                    case(NODE_SUB):
                        Node* temp_right_child_node = node_copy(node->BinaryOperation.right_child);
                        node_free(node);

                        node = node_copy(temp_right_child_node);
                        node_free(temp_right_child_node);

                        break;

                    // 0*, 0/, 0^ -> 0
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
                    case(NODE_MUL):
                        Node* temp_right_child_node = node_copy(node->BinaryOperation.right_child);
                        node_free(node);

                        node = node_copy(temp_right_child_node);
                        node_free(temp_right_child_node);

                        break;
                    
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
                    case(NODE_SUB):
                        Node* temp_left_child_node = node_copy(node->BinaryOperation.left_child);
                        node_free(node);

                        node = node_copy(temp_left_child_node);
                        node_free(temp_left_child_node);

                        break;

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
                    case(NODE_POW):
                        Node* temp_left_child_node = node_copy(node->BinaryOperation.left_child);
                        node_free(node);

                        node = node_copy(temp_left_child_node);
                        node_free(temp_left_child_node);

                        break;
                    
                    default: break;
                }
            }
        }
    }

    // Unary stuff, go deeper into tree
    else if (node->type == NODE_NEG) {
        node->UnaryOperation.child = simplify(node->UnaryOperation.child);
    }
    else if (node->type == NODE_FUNC) {
        node->Function.child = simplify(node->Function.child);
    }

    return node;
}

void simplify_in_place(Node* node) {
    // Continuously attempt simplification of AST until no more simplification operations can be performed 
    while (1) {
        if (simplify_lowest_branch_in_place(node) == 0) {
            return;
        }
    }
}

// Simplify at lowest possible branches of AST and return how many simplification operations were performed. No simplification operation = most simplified form of AST
int simplify_lowest_branch_in_place(Node* node) {
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
            simplify_count += simplify_lowest_branch_in_place(node->BinaryOperation.left_child);
            simplify_count += simplify_lowest_branch_in_place(node->BinaryOperation.right_child);
        }
    }

    // Rrecursively simplify unary operation children
    else if (node->type == NODE_NEG) {
        simplify_count += simplify_lowest_branch_in_place(node->UnaryOperation.child);
    }
    else if (node->type == NODE_FUNC) {
        simplify_count += simplify_lowest_branch_in_place(node->Function.child);
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