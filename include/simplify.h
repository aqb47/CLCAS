#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include "parser.h"
#include "ast.h"

#include <string.h>
#include <ctype.h>
#include <math.h>


// Simplify numeric children of binary operations, along with variables within AST and return new tree
Node* simplify(Node* node);

// Evaluate binary operation based on type and children values
double evaluate_binop(NodeType binop, double left_child_value, double right_child_value);

// Evaluate function operation based on name and children values
double evaluate_function(const char* function, double child_value);

#endif