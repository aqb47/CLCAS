#include "ast.h"
#include "parser.h"
#include "simplify.h"
#include "numerical.h"
#include "diff.h"

int main(void) {
    Node* parsed = simplify(parse_string("exp(x) - x^3 + 4*x^2"));
    Node* derivative = simplify(differentiate(parsed, 'x'));
    Node* series = simplify(maclaurin_series(parsed, 'x', 5));

    Node* comparison_test_1 = simplify(parse_string("sin(x + 3*x) - 4"));
    Node* comparison_test_2 = simplify(parse_string("sin(x + 3*x*1) + 0 - 4"));

    Node* power_test = simplify(simplify(parse_string("x^5 * x^2")));

    double guess = newton_rhapson_root(parsed, 'x', -2, DEFAULT_TOLERANCE_NEWTON_RHAPSON, DEFAULT_MAX_ITERATION);

    node_print_infix(parsed);
    printf("\n");

    node_print_infix(derivative);
    printf("\n");

    node_print_infix(series);
    printf("\n");

    node_print_infix(power_test);
    printf("\n");

    printf("Parsed evaluated value: %lf\n", node_eval(parsed, 'x', 1));
    printf("Parsed derivative value: %lf\n", node_eval(derivative, 'x', 1));
    printf("Newton-Rhapson approximation: %lf\n", guess);
    printf("Simpson's rule approximation for integral: %lf\n", simpson_integrate(parsed, 'x', 0, 2, DEFAULT_TOLERANCE_SIMPSON));
    printf("Node comparison between test cases: %d\n", node_comp(comparison_test_1, comparison_test_2));

    node_free(parsed);
    node_free(derivative);
    
    node_free(series);

    node_free(comparison_test_1);
    node_free(comparison_test_2);
    
    node_free(power_test);

    return 0;
}