#include "parser.h"
#include "ast.h"

#include <ctype.h>
#include <string.h>

void lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->position = 0;
}

Token lexer_next(Lexer* lexer) {
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
    if (isdigit(lexer->source[lexer->position])) {
        char number_string[TOKEN_NUMBER_MAX_LENGTH];
        char* end;
        
        int i;

        // Read rest of the number and copy it into the number string
        for (i = 0; (isdigit(lexer->source[lexer->position]) || lexer->source[lexer->position] == '.') && i < TOKEN_NUMBER_MAX_LENGTH - 1; i++, lexer->position += 1) {
            number_string[i] = lexer->source[lexer->position];
        }
        number_string[i] = '\0';

        double number_value = strtod(number_string, end);

        output_token.type = TOK_NUM;
        output_token.number = number_value;
    }

    // For a function
    if (isalpha(lexer->source[lexer->position])) {
    }

    switch (lexer->source[lexer->position]) {
    }
}

Token lexer_peek(Lexer* lexer) {
    // TODO
}