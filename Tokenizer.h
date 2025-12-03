#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "Common.h"

int read_files(const char* filenames);
int read_lines(void);
int is_symbol_char(char c);
int get_lexeme(const char *word, int length);
void add_token(TokenType type, const char* lexeme);
void write_tokens(const char* output_file);
void error_nonterminal(const char* rule, const char* nonterminal);
#endif