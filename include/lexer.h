/**@<lexer.h>::**/
#include <mytype.h>
#pragma once
token_t iskeyword(const char *symbol);
token_t gettoken(FILE *tape);
#define MAXIDLEN 0x100
extern size_t linenumber;
extern char lexeme[MAXIDLEN + 1];
