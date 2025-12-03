#define _POSIX_C_SOURCE 200809L

#include "Common.h"
#include "Tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKENS 10000
#define MAX_LEXEME_LEN 256

//store tokens
Lex tokens[MAX_TOKENS];
int token_count = 0;

char *lines[MAX_LEXEME_LEN];
char buffer[MAX_LEXEME_LEN];
int linecount = 0;

//add token to array
void add_token(TokenType type, const char* lexeme) 
{
    if (token_count >= MAX_TOKENS) 
    {
        fprintf(stderr, "Too many tokens\n");
        exit(1);
    }

    tokens[token_count].token_type = type;
    strcpy(tokens[token_count].lexeme, lexeme);
    token_count++;
}

//write tokens to files
void write_tokens(const char* output_file) 
{
    FILE* output = fopen(output_file, "w");
    if (!output)
    {
        fprintf(stderr, "Can't open output file %s\n", output_file);
        exit(1);
    }

    for(int i = 0; i < token_count; i++) 
    {
        fprintf(output, "%s %s\n",
        token_to_string(tokens[i].token_type), tokens[i].lexeme);
    }

    fclose(output);
}

//read files line by line
int read_files(const char* filenames)
{
    FILE* file = fopen(filenames, "r");


    if (!file || file == NULL) 
    {
        fprintf(stderr, "Can't open file %s\n", filenames);
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), file))
    {
        lines[linecount] = strdup(buffer);
        linecount++;
    }

    fclose(file);
    return 0;
}

//read lines, character by character and tokenize the character
int read_lines(void)
{
    for(int i = 0; i < linecount; i++)
    {
        char *line = lines[i];
        int length = strlen(line);
    
        int j = 0;

        while (j < length) 
        {
            char current = line[j];

            if(isspace(current)) 
            {
                j++;
                continue;
            }
            
            //handle alphabetic works like keywords and identifier
            if(isalpha(current))
            {
                int word_start = j;

                while (j < length && isalnum(line[j]))
                {
                    j++;
                }

                int word_length = j - word_start;
                char word[MAX_LEXEME_LEN];
                strncpy(word, line + word_start, word_length);
                word[word_length] = '\0';

                get_lexeme(word, word_length);
                continue;
            }

            //handle numbers
            if(isdigit(current)) 
            {
                int num_start = j;

                while (j < length && isdigit(line[j]))
                {
                    j++;
                }
                
                int num_length = j - num_start;
                char number[MAX_LEXEME_LEN];
                strncpy(number, line + num_start, num_length);
                number[num_length] = '\0';

                add_token(NUMBER, number);
                continue;
            }

            //handle multi-character operation like != and ==
            if((current == '=' || current == '!') && j + 1 < length && line[j +1] == '=') 
            {
                char op[3];
                op[0] = current;
                op[1] = '=';
                op[2] = '\0';
                add_token(BINOP, op);
                j +=2;
                continue;
            }

            //handle single character symbol
            if(current == '(')
            {
                add_token(LEFT_PARENTHESIS, "(");
                j++;
                continue;
            }

            if(current == ')')
            {
                add_token(RIGHT_PARENTHESIS, ")");
                j++;
                continue;
            }

            if(current == '{')
            {
                add_token(LEFT_BRACKET, "{");
                j++;
                continue;
            }

            if(current == '}')
            {
                add_token(RIGHT_BRACKET, "}");
                j++;
                continue;
            }

            if (current == ',')
            {
                add_token(COMMA, ",");
                j++;
                continue;
            }

            if (current == '=')
            {
                add_token(EQUAL, "=");
                j++;
                continue;
            }

            if (current == ';')
            {
                add_token(EOL, ";");
                j++;
                continue;
            }

            //handle single char binary operators
            if((current == '+' || current == '%' || current == '*'))
            {
                char op[2];
                op[0] = current;
                op[1] = '\0';
                add_token(BINOP, op);
                j++;
                continue;
            }

            else {
                printf("%c\n", line[j]);
            }

            j++;
        }

    }
    return 0;
}

//dertmine non-singular char type or alpha-numeric
int get_lexeme(const char *word, int length)
{
    if(strcmp(word, "while") == 0)
    {
        add_token(WHILE_KEYWORD, word);
    }
    else if(strcmp(word, "return") == 0) 
    {
        add_token(RETURN_KEYWORD, word);
    }
    else if(strcmp(word, "int") == 0 || strcmp(word, "void") == 0)
    {
        add_token(VARTYPE, word);
    }
    else
    {
        add_token(IDENTIFIER, word);
    }
    
    return 0;
}

//implement token logic
int main(int argc, char** argv) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Expect 2 arguments%s\n", argv[0]);
        return 1;
    }
    
    read_files(argv[1]); //read input
    read_lines(); //tokenize
    write_tokens(argv[2]); //output

    return 0;
} 