#include "ast.h"
#include "parser.h"
#include "simplify.h"
#include "numerical.h"
#include "diff.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "x^2 + 3*x");
    
    Node *parsed = simplify(parse_expr(&l, 0));
    Node* derivative = simplify(differentiate(parsed, 'x'));
    Node* series = maclaurin_series(parsed, 'x', 5);

    double guess = newton_rhapson_root(parsed, 'x', -2, 0.0000001, 5);

    node_print_infix(parsed);
    printf("\n");

    node_print_infix(derivative);
    printf("\n");

    node_print_infix(series);
    printf("\n");

    printf("Parsed evaluated value: %lf\n", node_eval(parsed, 'x', 1));
    printf("Parsed derivative value: %lf\n", node_eval(derivative, 'x', 1));
    printf("Newton-Rhapson approximation: %lf\n", guess);

    node_free(parsed);
    node_free(derivative);
    node_free(series);
    
    return 0;
}