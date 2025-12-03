#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum 
{
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    WHILE_KEYWORD,
    RETURN_KEYWORD,
    EQUAL,
    COMMA,
    EOL,
    VARTYPE,
    IDENTIFIER,
    BINOP,
    NUMBER,
} TokenType;

typedef struct 
{
    TokenType token_type;
    char lexeme[257];    
} Lex;

const char* token_to_string(TokenType type) 
{
    switch (type)
    {
        case LEFT_PARENTHESIS: return "LEFT_PARENTHESIS";
        case RIGHT_PARENTHESIS: return "RIGHT_PARENTHESIS";
        case LEFT_BRACKET: return "LEFT_BRACKET";
        case RIGHT_BRACKET: return "RIGHT_BRACKET";
        case WHILE_KEYWORD: return "WHILE_KEYWORD";
        case RETURN_KEYWORD: return "RETURN_KEYWORD";
        case EQUAL: return "EQUAL";
        case COMMA: return "COMMA";
        case EOL: return "EOL";
        case VARTYPE: return "VARTYPE";
        case IDENTIFIER: return "IDENTIFIER";
        case BINOP: return "BINOP";
        case NUMBER: return "NUMBER";
        default: return "UNKNOWN";
    }
} 
#endif