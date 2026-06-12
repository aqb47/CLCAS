#include "ast.h"
#include "parser.h"

int main(void) {
    // Manual AST: x^2 + 1
    Node *tree = node_binop(NODE_ADD,
    node_binop(NODE_POW, node_var('x'), node_num(2)),
    node_num(1));
    
    node_print(tree);
    node_free(tree);

    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "dog(x^2 + 1)");
    
    Node *parsed = parse_expr(&l, 0);
    
    node_print(parsed);
    node_free(parsed);

    return 0;
}