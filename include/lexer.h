/**@<lexer.h>::**/
#pragma once
#include <stdio.h>

#define MAXIDLEN 0x100

typedef int token_t;

token_t iskeyword(const char *symbol);
token_t gettoken(FILE *tape);
extern size_t linenumber, colnumber;
extern char lexeme[MAXIDLEN + 1];
