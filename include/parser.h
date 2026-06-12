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
    
    double number;
    
    char identifier[TOKEN_IDENTIFIER_LENGTH];
} Token;

typedef struct {
    const char* source;

    int position;
    
    Token current;
} Lexer;

void lexer_init(Lexer* lexer, const char* source);

Token lexer_next(Lexer* lexer); // Advances and returns token

Token lexer_peek(Lexer* lexer); // Returns without advancing

Node* parse_expr(Lexer* lexer, int min_binding_power);

#endif