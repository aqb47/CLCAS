#include "ast.h"
#include "parser.h"
#include "simplify.h"
#include "diff.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "x^x");
    
    Node *parsed = parse_expr(&l, 0);
    
    Node* derivative = differentiate(parsed, 'x');

    node_print_infix(parsed);
    printf("\n");
    node_print_infix(derivative);
    printf("\n");

    node_free(parsed);
    node_free(derivative);
    return 0;
}