#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <math.h>

// Simplify numeric children of binary operations, along with variables within AST and return new tree
Node* simplify(Node* node);

#endif