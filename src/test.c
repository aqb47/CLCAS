#include "ast.h"
#include "parser.h"
#include "simplify.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, " (x + y) ^ 1");
    
    Node *parsed = parse_expr(&l, 0);
    simplify(parsed);

    node_print_infix(parsed);
    printf("\n");
    node_print_prefix(parsed);

    node_free(parsed);

    return 0;
}