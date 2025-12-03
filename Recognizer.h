#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "Common.h"

extern Lex tokens[];
extern int token_count;
extern int current_token;
extern FILE* error_output;

void load_tokens(const char* input_file);
TokenType string_to_token(const char* str);
void error(const char* rule, const char* message);
TokenType peek();
void consume(TokenType expected, const char* rule);
int match(TokenType type);
void error_nonterminal(const char* rule, const char* nonterminal);

int function();
int header();
int arg_decl();
int body();
int statement_list();
int statement();
int while_loop();
int return_stmt();
int assignment();
int expression();
int term();

#endif