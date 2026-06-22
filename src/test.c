#include "ast.h"
#include "parser.h"
#include "simplify.h"
#include "numerical.h"
#include "diff.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "(tan(x))^(1/2)");
    
    Node *parsed = simplify(parse_expr(&l, 0));
    Node* derivative = simplify(differentiate(parsed, 'x'));

    node_print_infix(parsed);
    printf("\n");

    node_print_infix(derivative);
    printf("\n");

    printf("Parsed evaluated value: %lf\n", node_eval(parsed, 'x', 1));
    printf("Parsed derivative value: %lf\n", node_eval(derivative, 'x', 1));

    node_free(parsed);
    node_free(derivative);
    
    return 0;
}