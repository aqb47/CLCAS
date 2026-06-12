#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum length of function name for function nodes 
#define FUNCTION_NAME_LENGTH 8

typedef struct Node Node;

typedef enum {
    NODE_NUM, // Rational constants
    NODE_VAR, // Variables
    
    // General binary operations
    NODE_SUB,
    NODE_ADD,

    NODE_DIV,
    NODE_MUL,
    
    NODE_POW, 
    
    // General unary functions
    NODE_NEG, // Minus (negation)
    NODE_FUNC, // Mathematical unary functions (sin, cos etc.)
    
    NODE_ERR, // In case of an error within a node returning function
} NodeType;

// Nodes comprising the abstract syntax tree. A node will have a type and data (depending on that type it is)
struct Node {
    NodeType type;

    // Node data
    union {
        double number; // For NODE_NUM

        char variable; // For NODE_VAR

        struct {
            Node* left_child;
            Node* right_child;
        } BinaryOperation; // For NODE_ADD, NODE_SUB, NODE_MUL, NODE_DIV, NODE_POW etc. (+, -, *, /, ^ etc.)

        struct {
            Node* child;
        } UnaryOperation; // For NODE_NEG

        struct {
            char function_name[FUNCTION_NAME_LENGTH];
            Node* child;
        } Function; // For NODE_FUNC (sin, cos, tan)
    };
};

/// Node initialization functions (use malloc() and should be freed with node_free())

// Initialize rational number node (0.1, 10, -1,23 etc.)
Node* node_num(double value);

// Initialize variable letter node ('x', 'y', 'z' etc.)
Node* node_var(char variable);

// Initialize binary operation node (addition, subtraction, multiplication, division, exponentiation etc.)
Node* node_binop(NodeType type, Node* node_left, Node* node_right);

// Initialize unary negation node
Node* node_neg(Node* child);

// Initialize unary function node (sine, cosine, tangent, natural logarithm etc.)
Node* node_func(const char* function_name, Node* child);

/// Other node stuff

// Recursively create a deep copy of tree from head
Node* node_copy(const Node* node);

// Recursively free entire tree from head
void node_free(Node* node);         

// Print tree in prefix format (Polish notation with parenthesis or (OPERATION LEFT_CHILD RIGHT_CHILD) for every binary operation)
void node_print_prefix(const Node* node);

// Print tree in infix format
void node_print_infix(const Node* node);

#endif