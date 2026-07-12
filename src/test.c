#include "ast.h"
#include "parser.h"
#include "simplify.h"
#include "numerical.h"
#include "diff.h"

int main(void) {
    // Parser round-trip
    Lexer l; 
    lexer_init(&l, "exp(x) - x^3 + 4*x^2");
    
    Node *parsed = simplify(parse_expr(&l, 0));
    Node* derivative = simplify(differentiate(parsed, 'x'));
    Node* series = maclaurin_series(parsed, 'x', 5);

    double guess = newton_rhapson_root(parsed, 'x', -2, DEFAULT_TOLERANCE_NEWTON_RHAPSON, DEFAULT_MAX_ITERATION);

    node_print_infix(parsed);
    printf("\n");

    node_print_infix(derivative);
    printf("\n");

    node_print_infix(series);
    printf("\n");

    printf("Parsed evaluated value: %lf\n", node_eval(parsed, 'x', 1));
    printf("Parsed derivative value: %lf\n", node_eval(derivative, 'x', 1));
    printf("Newton-Rhapson approximation: %lf\n", guess);
    printf("Simpson's rule approximation for integral: %lf\n", simpson_integrate(parsed, 'x', 0, 2, DEFAULT_TOLERANCE_SIMPSON));

    node_free(parsed);
    node_free(derivative);
    node_free(series);
    
    return 0;
}