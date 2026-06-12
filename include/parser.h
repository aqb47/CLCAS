#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

#include <ctype.h>
#include <string.h>

#define TOKEN_IDENTIFIER_LENGTH 16
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
    
    char identifier[TOKEN_IDENTIFIER_LENGTH]; // TOK_IDENT
} Token;

typedef struct {
    const char* source; // Input string

    int position; // Which letter lexer is on within string
    
    Token current; // Current token on position

    Token peeked; // Peeked token ahead of position

    int has_peek; // Is peeked token valid
} Lexer;

void lexer_init(Lexer* lexer, const char* source);

Token lexer_next(Lexer* lexer); // Advances and returns token

Token lexer_peek(Lexer* lexer); // Returns without advancing

Node* parse_expr(Lexer* lexer, int min_binding_power);

#endif