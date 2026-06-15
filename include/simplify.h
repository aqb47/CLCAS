#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <math.h>

typedef enum {
    LEFT,
    CENTER,
    RIGHT
} NodePos;

// Simplify only numeric children of binary operations within AST in-place 
void simplify_in_place(Node* node);

// Simplify numeric children of binary operations, along with variables within AST and return new tree
Node* simplify(Node* node);

#endif