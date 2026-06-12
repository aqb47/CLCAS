#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <string.h>

static Token lexer_scan(Lexer* lexer);

static int left_binding_power(TokType token_type);
static int right_binding_power(TokType token_type);
static int is_operation(TokType token_type);
static NodeType toktype_to_nodetype(TokType token_type);

static Node* parse_prefix(Lexer* lexer);

Node* parse_expr(Lexer* lexer, int min_binding_power) {
    Node* lhs = parse_prefix(lexer);

    while (1) {
        // Check for valid binary infix operation
        Token operation = lexer_peek(lexer);
        if (operation.type == TOK_ERR || operation.type == TOK_EOF) {
            break;
        }
        if (!is_operation(operation.type)) {
            break;
        }

        // Check binding powers
        int lbp = left_binding_power(operation.type);
        int rbp = right_binding_power(operation.type);
        if (lbp <= min_binding_power) {
            break;
        }

        // Consume token
        operation = lexer_next(lexer);

        // Build tree
        NodeType operation_nodetype = toktype_to_nodetype(operation.type);

        Node* rhs = parse_expr(lexer, rbp);
        
        lhs = node_binop(operation_nodetype, lhs, rhs);
    }

    return lhs;
}

void lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->position = 0;
    lexer->has_peek = 0;
}

Token lexer_scan(Lexer* lexer) {
    Token output_token;

    // Move through source string until non-whitespace character or end of string is found
    while (lexer->source[lexer->position] != '\0' && isspace(lexer->source[lexer->position])) {
        lexer->position += 1;
    }

    // Return EOF token for end of string, and if not advance the position 
    if (lexer->source[lexer->position] == '\0') {
        output_token.type = TOK_EOF;
    }

    // For a numerical value
    else if (isdigit(lexer->source[lexer->position])) {
        output_token.type = TOK_NUM;

        char number_string[TOKEN_NUMBER_MAX_LENGTH];
        char* end;
        
        int i;

        // Read rest of the number and copy it into the number string
        for (i = 0; (isdigit(lexer->source[lexer->position]) || lexer->source[lexer->position] == '.') && i < TOKEN_NUMBER_MAX_LENGTH - 1 && lexer->source[lexer->position] != '\0'; i++, lexer->position += 1) {
            number_string[i] = lexer->source[lexer->position];
        }
        number_string[i] = '\0';

        double number_value = strtod(number_string, &end);

        output_token.number = number_value;
    }

    // For an identifier
    else if (isalpha(lexer->source[lexer->position])) {
        output_token.type = TOK_IDENT;

        int j;

        // Read rest of identifier and copy it into the identifier string
        for (j = 0; (isalpha(lexer->source[lexer->position])) && j < TOKEN_IDENTIFIER_LENGTH - 1 && lexer->source[lexer->position] != '\0'; j++, lexer->position += 1) {
            output_token.identifier[j] = lexer->source[lexer->position];
        }
        output_token.identifier[j] = '\0';
    }

    // For symbols
    else {
        switch (lexer->source[lexer->position]) {
            case('+'): output_token.type = TOK_PLUS; break;
            case('-'): output_token.type = TOK_MINUS; break;
            
            case('*'): output_token.type = TOK_STAR; break;
            case('/'): output_token.type = TOK_SLASH; break;
            
            case('^'): output_token.type = TOK_CARET; break;
            
            case('('): output_token.type = TOK_LPAREN; break;
            case(')'): output_token.type = TOK_RPAREN; break;
            
            // Unknown symbol
            default: output_token.type = TOK_ERR;
        }

        lexer->position += 1;
    }

    return output_token;
}

Token lexer_next(Lexer* lexer) {
    // If we have peeked return generated peeked token, and if not advance directly by scanning
    if (lexer->has_peek) {
        lexer->has_peek = 0;
        return lexer->peeked;
    }
    else {
        return lexer_scan(lexer);
    }
}

Token lexer_peek(Lexer* lexer) {
    Token peeked;

    // If we haven't peeked just generate new peeked token, and if we have return previously generated peeked token
    if (!lexer->has_peek) {
        lexer->has_peek = 1;

        peeked = lexer_scan(lexer);
        lexer->peeked = peeked;
    }
    else {
        peeked = lexer->peeked;
    }

    return peeked;
}

Node* parse_prefix(Lexer* lexer) {
    Token token = lexer_peek(lexer);

    // Rational constant
    if (token.type == TOK_NUM) {
        token = lexer_next(lexer);

        return node_num(token.number);
    }

    else if (token.type == TOK_IDENT) {
        // Variable
        if (strlen(token.identifier) == 1) {
            token = lexer_next(lexer);

            return node_var(token.identifier[0]);
        }
        // Function
        else {
            char function_name[TOKEN_IDENTIFIER_LENGTH];
            Node* function_argument;

            token = lexer_next(lexer);
            
            // Save function name
            strncpy(function_name, token.identifier, TOKEN_IDENTIFIER_LENGTH);

            // Expect '('
            token = lexer_peek(lexer);
            if (token.type != TOK_LPAREN) {
                return NULL;
            }
            token = lexer_next(lexer);

            // Get argument
            function_argument = parse_expr(lexer, 0);
            if (function_argument == NULL) {
                return NULL;
            }

            // Expect ')'
            token = lexer_peek(lexer);
            if (token.type != TOK_RPAREN) {
                return NULL;
            }
            token = lexer_next(lexer);

            return node_func(function_name, function_argument);
        }
    }

    // Grouped expression starting with '('
    else if (token.type == TOK_LPAREN) {
        Node* inner_expression;

        token = lexer_next(lexer);

        inner_expression = parse_expr(lexer, 0);

        // Expect ')'
        token = lexer_peek(lexer);
        if (token.type != TOK_RPAREN){
            return NULL;
        }
        token = lexer_next(lexer);

        return inner_expression;
    }

    // Unary minus
    else if (token.type == TOK_MINUS) {
        Node* inner_expression;

        token = lexer_next(lexer);

        // Binding power of right side of unary minus is +5
        inner_expression = parse_expr(lexer, 5);

        return node_neg(inner_expression);
    }

    // Unexpected token
    else {
        printf("Unexpected token\n");

        return NULL;
    }
}

// Left-associative operators have higher right binding power
// Right-associative operators have higher left binding power
int left_binding_power(TokType token_type) {
    switch (token_type) {
        case(TOK_PLUS): return 1;
        case(TOK_MINUS): return 1;

        case(TOK_STAR): return 3;
        case(TOK_SLASH): return 3;

        case(TOK_CARET): return 6;

        default: return 0;
    }
}

int right_binding_power(TokType token_type) {
    switch (token_type) {
        case(TOK_PLUS): return 2;
        case(TOK_MINUS): return 2;

        case(TOK_STAR): return 4;
        case(TOK_SLASH): return 4;

        case(TOK_CARET): return 5;

        default: return 0;
    }
}

// Check if token type is that of a binary operation
int is_operation(TokType token_type) {
    if (left_binding_power(token_type)) {
        return 1;
    }
    return 0;
}

// Get corresponding node type to token type
NodeType toktype_to_nodetype(TokType token_type) {
    switch (token_type) {
        case(TOK_PLUS): return NODE_ADD;
        case(TOK_MINUS): return NODE_SUB;
        case(TOK_STAR): return NODE_MUL;
        case(TOK_SLASH): return NODE_DIV;
        case(TOK_CARET): return NODE_POW;
        default: return NODE_ERR;
    }
}