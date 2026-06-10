#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>

// Maximum length of function name for function nodes 
#define FUNCTION_NAME_LENGTH 8

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
} NodeType;

// Nodes comprising the abstract syntax tree. A node will have a type and data (depending on that type it is)
typedef struct {
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
} Node;

// Node initialization functions
Node* node_num(double value);

Node* node_var(char variable);

Node* node_binop(NodeType type, Node *node_left, Node *node_right);

Node* node_neg(Node *child);

Node* node_func(const char *function_name, Node *child);

// Other node stuff
Node* node_copy(const Node *node); // Deep copy

void node_free(Node *node);         

void node_print(const Node *node);  

#endif