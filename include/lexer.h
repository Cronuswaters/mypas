/**@<lexer.h>::**/
token_t iskeyword(const char *);
#define MAXIDLEN 0x100
extern size_t linenumber ;
extern char lexeme[MAXIDLEN + 1];