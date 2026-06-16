#include "ast.h"
#include "parser.h"
#include "simplify.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "-(0 - x)");
    
    Node *parsed = parse_expr(&l, 0);
    
    parsed = simplify(parsed);

    node_print_infix(parsed);
    printf("\n");

    node_free(parsed);
    return 0;
}