#ifndef PARSER_H
#define PARSER_H

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
    
    TOK_EOF
} TokType;

typedef struct {
    TokType type;
    
    double num;
    
    char ident[16];
} Token;

typedef struct {
    const char *src;

    int pos;
    
    Token current;
} Lexer;

void  lexer_init(Lexer *lexer, const char *source);

Token lexer_next(Lexer *lexer); // Advances and returns token

Token lexer_peek(Lexer *lexer); // Returns without advancing

#endif