#include "Common.h"
#include "Recognizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 10000

Lex tokens[MAX_TOKENS];
int token_count = 0;
int current_token = 0;
FILE* error_output = NULL;


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    error_output = fopen(argv[2], "w");
    if (!error_output)
    {
        perror("Error opening output file");
        return 1;
    }

    load_tokens(argv[1]);

    if (!function())
    {
        error_nonterminal("function", "function");
    }

    //Check all consumed tokens
    if (current_token < token_count)
    {
        fprintf(error_output, "Error: Only consumed %d of the %d given tokens\n",
                current_token, token_count);
        fclose(error_output);
        exit(0);
    }

    //Success output
    fprintf(error_output, "PARSED!!!\n");
    fclose(error_output);
    return 0;
}

//helper function

void load_tokens(const char* input_file)
{
    FILE* input = fopen(input_file, "r");
    if (!input)
    {
        perror("Error opening input file");
        exit(1);
    }

    char token_str[100];
    char lexeme[257];

    while (fscanf(input, "%s %256[^\n]", token_str, lexeme) == 2)
    {
        tokens[token_count].token_type = string_to_token(token_str);
        strcpy(tokens[token_count].lexeme, lexeme);
        token_count++;
    }

    fclose(input);
}

TokenType string_to_token(const char* str)
{
    if (strcmp(str, "LEFT_PARENTHESIS") == 0) return LEFT_PARENTHESIS;
    if (strcmp(str, "RIGHT_PARENTHESIS") == 0) return RIGHT_PARENTHESIS;
    if (strcmp(str, "LEFT_BRACKET") == 0) return LEFT_BRACKET;
    if (strcmp(str, "RIGHT_BRACKET") == 0) return RIGHT_BRACKET;
    if (strcmp(str, "WHILE_KEYWORD") == 0) return WHILE_KEYWORD;
    if (strcmp(str, "RETURN_KEYWORD") == 0) return RETURN_KEYWORD;
    if (strcmp(str, "EQUAL") == 0) return EQUAL;
    if (strcmp(str, "COMMA") == 0) return COMMA;
    if (strcmp(str, "EOL") == 0) return EOL;
    if (strcmp(str, "VARTYPE") == 0) return VARTYPE;
    if (strcmp(str, "IDENTIFIER") == 0) return IDENTIFIER;
    if (strcmp(str, "BINOP") == 0) return BINOP;
    if (strcmp(str, "NUMBER") == 0) return NUMBER;

    return IDENTIFIER;
}

//error helper, print then exit
void error_token(const char* rule, TokenType expected)
{
    fprintf(error_output, "Error: In grammar rule %s, expected token #%d to be %s but was %s\n",
            rule,
            current_token + 1,
            token_to_string(expected),
            token_to_string(tokens[current_token].token_type));
    fclose(error_output);
    exit(0);
}

void error_nonterminal(const char* rule, const char* nonterminal)
{
    fprintf(error_output, "Error: In grammar rule %s, expected a valid %s non-terminal to be present but was not\n",
            rule, nonterminal);
    fclose(error_output);
    exit(0);
}

TokenType peek()
{
    if (current_token < token_count)
    {
        return tokens[current_token].token_type;
    }
    return -1;
}

void consume(TokenType expected, const char* rule)
{
    if (current_token >= token_count || tokens[current_token].token_type != expected)
    {
        error_token(rule, expected);
    }
    current_token++;
}

int match(TokenType type)
{
    return peek() == type;
}

//grammar rule function

int function()
{
    if (!header())
    {
        error_nonterminal("function", "header");
    }

    if (!body())
    {
        error_nonterminal("function", "body");
    }

    return 1;
}

int header()
{
    if (!match(VARTYPE))
    {
        return 0;
    }

    consume(VARTYPE, "header");
    consume(IDENTIFIER, "header");
    consume(LEFT_PARENTHESIS, "header");

    if (match(VARTYPE))
    {
        if (!arg_decl())
        {
            error_nonterminal("header", "arg-decl");
        }
    }

    consume(RIGHT_PARENTHESIS, "header");
    return 1;
}

int arg_decl()
{
    consume(VARTYPE, "arg-decl");
    consume(IDENTIFIER, "arg-decl");

    while (match(COMMA))
    {
        consume(COMMA, "arg-decl");
        consume(VARTYPE, "arg-decl");
        consume(IDENTIFIER, "arg-decl");
    }

    return 1;
}

int body()
{
    consume(LEFT_BRACKET, "body");

    if (match(WHILE_KEYWORD) || match(RETURN_KEYWORD) || match(IDENTIFIER))
    {
        if (!statement_list())
        {
            error_nonterminal("body", "statement-list");
        }
    }

    consume(RIGHT_BRACKET, "body");
    return 1;
}

int statement_list()
{
    if (!statement())
    {
        return 0;
    }

    while (match(WHILE_KEYWORD) || match(RETURN_KEYWORD) || match(IDENTIFIER))
    {
        if (!statement())
        {
            return 0;
        }
    }

    return 1;
}

int statement()
{
    if (match(WHILE_KEYWORD))
    {
        return while_loop();
    }
    else if (match(RETURN_KEYWORD))
    {
        return return_stmt();
    }
    else if (match(IDENTIFIER))
    {
        return assignment();
    }
    else
    {
        return 0;
    }
}

int while_loop()
{
    consume(WHILE_KEYWORD, "while-loop");
    consume(LEFT_PARENTHESIS, "while-loop");

    if (!expression())
    {
        error_nonterminal("while-loop", "expression");
    }

    consume(RIGHT_PARENTHESIS, "while-loop");

    if (!body())
    {
        error_nonterminal("while-loop", "body");
    }

    return 1;
}

int return_stmt()
{
    consume(RETURN_KEYWORD, "return");

    if (!expression())
    {
        error_nonterminal("return", "expression");
    }

    consume(EOL, "return");
    return 1;
}

int assignment()
{
    consume(IDENTIFIER, "assignment");
    consume(EQUAL, "assignment");

    if (!expression())
    {
        error_nonterminal("assignment", "expression");
    }

    consume(EOL, "assignment");
    return 1;
}

int expression()
{
    if (match(LEFT_PARENTHESIS))
    {
        consume(LEFT_PARENTHESIS, "expression");

        if (!expression())
        {
            error_nonterminal("expression", "expression");
        }

        consume(RIGHT_PARENTHESIS, "expression");
    }
    else if (match(IDENTIFIER) || match(NUMBER))
    {
        if (!term())
        {
            return 0;
        }

        while (match(BINOP))
        {
            consume(BINOP, "expression");

            if (!term())
            {
                error_nonterminal("expression", "term");
            }
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

int term()
{
    if (match(IDENTIFIER))
    {
        consume(IDENTIFIER, "term");
    }
    else if (match(NUMBER))
    {
        consume(NUMBER, "term");
    }
    else
    {
        return 0;
    }

    return 1;
}
