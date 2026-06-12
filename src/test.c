#include "ast.h"
#include "parser.h"
#include "simplify.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "1 + 2 + 3*4 + 9 - 8^2 + 8^(5/3)");
    
    Node *parsed = parse_expr(&l, 0);
    simplify(parsed);
    
    node_print_infix(parsed);
    printf("\n");
    node_print_prefix(parsed);

    node_free(parsed);

    return 0;
}