#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

#include <ctype.h>
#include <string.h>

// Maximum length of token identifier
#define TOKEN_IDENTIFIER_LENGTH 16

// Maximum length of number string to be converted to number of token
#define TOKEN_NUMBER_MAX_LENGTH 25

typedef enum {
    TOK_NUM, 
    TOK_IDENT, 
    
    TOK_PLUS, 
    TOK_MINUS, 
    TOK_STAR,
    TOK_SLASH, 
    TOK_CARET, // ^ (I didn't know this lol)
    
    TOK_LPAREN, 
    TOK_RPAREN, 
    
    TOK_EOF,

    TOK_ERR
} TokType;

typedef struct {
    TokType type;
    
    double number; // TOK_NUM
    
    char identifier[TOKEN_IDENTIFIER_LENGTH]; // TOK_IDENT - variable letter / function name
} Token;

typedef struct {
    const char* source; // Input string

    int position; // Which letter lexer is on within string
    
    Token current; // Current token on position

    Token peeked; // Peeked token

    int has_peek; // If we peeked already
} Lexer;

// Initialize lexer from source string
void lexer_init(Lexer* lexer, const char* source);

// Parse lexer depending on a minimum binding power which represents operation precedence. Main Pratt parser, uses loop + recursion. Will return head of created abstract syntax tree
Node* parse_expr(Lexer* lexer, int min_binding_power);

#endif