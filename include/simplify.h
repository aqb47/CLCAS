#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <math.h>

// Simplify entire AST 
void simplify(Node* node);

#endif