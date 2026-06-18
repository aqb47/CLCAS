#ifndef DIFF_H
#define DIFF_H

#include "ast.h"
#include "simplify.h"

Node* differentiate(const Node* node, char variable);

#endif