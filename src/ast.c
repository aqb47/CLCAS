#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char get_operation_symbol(NodeType operation_enum);

Node* node_num(double value) {
    Node* output_node_ptr = malloc(sizeof(Node));
    if (output_node_ptr == NULL) {
        return NULL;
    }

    output_node_ptr->type = NODE_NUM;
    output_node_ptr->number = value;

    return output_node_ptr;
}

Node* node_var(char variable) {
    Node* output_node_ptr = malloc(sizeof(Node));
    if (output_node_ptr == NULL) {
        return NULL;
    }

    output_node_ptr->type = NODE_VAR;
    output_node_ptr->variable = variable;

    return output_node_ptr;
}

Node* node_binop(NodeType type, Node* node_left, Node* node_right) {
    Node* output_node_ptr = malloc(sizeof(Node));
    if (output_node_ptr == NULL) {
        return NULL;
    }

    output_node_ptr->type = type;
    output_node_ptr->BinaryOperation.left_child = node_left;
    output_node_ptr->BinaryOperation.right_child = node_right;

    return output_node_ptr;
}

Node* node_neg(Node* child) {
    Node* output_node_ptr = malloc(sizeof(Node));
    if (output_node_ptr == NULL) {
        return NULL;
    }

    output_node_ptr->type = NODE_NEG;
    output_node_ptr->UnaryOperation.child = child;

    return output_node_ptr;
}

Node* node_func(const char* function_name, Node* child) {
    Node* output_node_ptr = malloc(sizeof(Node));
    if (output_node_ptr == NULL) {
        return NULL;
    }

    output_node_ptr->type = NODE_FUNC;

    strncpy(output_node_ptr->Function.function_name, function_name, FUNCTION_NAME_LENGTH - 1);
    output_node_ptr->Function.function_name[FUNCTION_NAME_LENGTH - 1] = '\0';

    output_node_ptr->Function.child = child;

    return output_node_ptr;
}

void node_free(Node* node) {
    // Base cases
    if (node == NULL) {
        return;
    }

    else if (node->type == NODE_NUM || node->type == NODE_VAR) {
        free(node);
    }

    // Unary operations
    else if (node->type == NODE_NEG) {
        node_free(node->UnaryOperation.child);

        free(node);
    }
    else if (node->type == NODE_FUNC) {
        node_free(node->Function.child);

        free(node);
    }

    // Binary operations
    else {
        node_free(node->BinaryOperation.left_child);
        node_free(node->BinaryOperation.right_child);

        free(node);
    }
}

void node_print(const Node* node) {
    // Base cases
    if (node == NULL) {
        printf("? ");
    }

    else if (node->type == NODE_NUM) {
        printf("%lf ", node->number);
    }
    else if (node->type == NODE_VAR) {
        printf("%c ", node->variable);
    }

    // Unary operations
    else if (node->type == NODE_NEG) {
        printf("-(");
        
        node_print(node->UnaryOperation.child);
        
        printf(") ");
    }
    else if (node->type == NODE_FUNC) {
        printf("%s(", node->Function.function_name);
        
        node_print(node->Function.child);
        printf(") ");
    }

    // Binary operations
    else {
        char operation_symbol = get_operation_symbol(node->type);

        printf("(%c ", operation_symbol);
        
        node_print(node->BinaryOperation.left_child);
        node_print(node->BinaryOperation.right_child);
        
        printf(") ");
    }
}

Node* node_copy(const Node* node) {
    Node* output_node_ptr;

    // Base cases
    if (node == NULL) {
        output_node_ptr = NULL;
    }

    else if (node->type == NODE_NUM) {
        output_node_ptr = node_num(node->number);
    }
    else if (node->type == NODE_VAR) {
        output_node_ptr = node_var(node->variable);
    }

    // Unary operations
    else if (node->type == NODE_NEG) {
        output_node_ptr = node_neg(node->UnaryOperation.child);

        output_node_ptr->UnaryOperation.child = node_copy(node->UnaryOperation.child);
    }
    else if (node->type == NODE_FUNC) {
        output_node_ptr = node_func(node->Function.function_name, node->Function.child);
        
        output_node_ptr->Function.child = node_copy(node->Function.child);
    }

    // Binary operations
    else {
        output_node_ptr = node_binop(node->type, node->BinaryOperation.left_child, node->BinaryOperation.right_child);

        output_node_ptr->BinaryOperation.left_child = node_copy(node->BinaryOperation.left_child);
        output_node_ptr->BinaryOperation.right_child = node_copy(node->BinaryOperation.right_child);
    }

    return output_node_ptr;
}

// Get operation symbol ASCII character depending on type enum
char get_operation_symbol(NodeType operation_enum) {
    switch (operation_enum) {
        case(NODE_ADD): return '+';
        case(NODE_SUB): return '-';
        case(NODE_MUL): return '*';
        case(NODE_DIV): return '/';
        case(NODE_POW): return '^';

        default: return '?';
    }
}