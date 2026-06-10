#ifndef AST_H
#define AST_H

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
    NODE_FUNC // Mathematical unary functions (sin, cos etc.)
} NodeType;

typedef struct {
    NodeType type;

    // Node data
    union {
        double num; // NODE_NUM

        char var; // NODE_VAR

        struct {
            Node* left_child;
            Node* right_child;
        } BinaryOperation; // +, -, *, /, ^

        struct {
            Node* child;
        } UnaryOperation; // Negation

        struct {
            char function_name[FUNCTION_NAME_LENGTH];
            Node* child;
        } Function; // sin, cos, tan
    };
} Node;

Node* node_num(double val);

Node* node_var(void);

Node* node_binop(NodeType type, Node *node_left, Node *node_right);

Node* node_neg(Node *child);

Node* node_func(const char *function_name, Node *child);

Node* node_copy(const Node *n); // Deep copy

void node_free(Node *n);         

void node_print(const Node *n);  

#endif