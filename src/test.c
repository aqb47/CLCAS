#include "ast.h"
#include "parser.h"

int main(void) {
    // Manual AST: x^2 + 1
    Node *tree = node_binop(NODE_ADD,
    node_binop(NODE_POW, node_var('x'), node_num(2)),
    node_num(1));
    
    node_print_prefix(tree);
    printf("\n");

    node_free(tree);

    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "2^3^4");
    
    Node *parsed = parse_expr(&l, 0);
    
    node_print_infix(parsed);
    printf("\n");
    node_print_prefix(parsed);

    node_free(parsed);

    return 0;
}