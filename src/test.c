#include "ast.h"

int main(void) {
    Node* tree = node_binop(NODE_ADD, node_binop(NODE_POW, node_var('x'), node_num(2)), node_num(1));
    Node* tree_copy = node_copy(tree);

    node_print(tree);
    printf("\n");
    node_print(tree_copy);
    
    node_free(tree);
    node_free(tree_copy);

    return 0;
}