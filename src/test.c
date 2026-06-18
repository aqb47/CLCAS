#include "ast.h"
#include "parser.h"
#include "simplify.h"
#include "diff.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "sin(x ^ 2)");
    
    Node *parsed = simplify(parse_expr(&l, 0));
    
    Node* derivative = simplify(differentiate(parsed, 'x'));

    node_print_infix(derivative);
    printf("\n");

    node_free(derivative);
    node_free(parsed);
    return 0;
}