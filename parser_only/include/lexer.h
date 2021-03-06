/**@<lexer.h>::**/
#pragma once
typedef int token_t;

token_t iskeyword(const char *symbol);
token_t gettoken(FILE *tape);
#define MAXIDLEN 0x100
extern size_t linenumber, colnumber;
extern char lexeme[MAXIDLEN + 1];
