/**@<lexer.c>::**/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <mytype.h>
#include <tokens.h>
#include <lexer.h>

char            lexeme[MAXIDLEN + 1];
size_t			linenumber = 1;

token_t ignoreneutrals(FILE * tape)
{
    int             head;

_ignoreneutrals_start:

    while (isspace(head = getc(tape))) {
	    if (head == '\n') {
		    linenumber++;
	    }
    }
    if ( head == '{' ) {
	    while ( (head = getc(tape)) != '}' ) {
		    if (head == EOF) {
			    return EOF;
		    }
		    if (head == '\n') {
			    linenumber++;
		    }
	    }
	    goto _ignoreneutrals_start;
    }
    ungetc(head, tape);
    return 0;
}

token_t isASGN(FILE * tape)
{
	/*eu pensei que não era pra por número hardcoded no código...?*/
	int i;
	if ( (lexeme[i = 0] = getc(tape)) == ':' ) {
		if ( (lexeme[++i] = getc(tape)) == '=' ) {
			lexeme[i+1] = 0;
			return ASGN;
		}
		ungetc(lexeme[i], tape);
	}
	ungetc(lexeme[--i], tape);
	return lexeme[i] = 0;
}

token_t isRELOP(FILE * tape)
{
	int i;
	switch (lexeme[i = 0] = getc(tape)) {
	case'<':
		lexeme[++i] = getc(tape);
		lexeme[i+1] = 0;
		switch (lexeme[i]) {
		case'=':
			return LEQ;
		case'>':
			return NEQ;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return lexeme[--i];
	case'>':
		lexeme[++i] = getc(tape);
		if (lexeme[i] == '=') {
			lexeme[i+1] = 0;
			return GEQ;
		} 
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return lexeme[--i];
	case'=':
		lexeme[i+1] = 0;
		return lexeme[i];
	}
	ungetc(lexeme[i], tape);
	return lexeme[i] = 0;
}

token_t isID(FILE * tape)
{
    int             i = 0;
    int             token;

    if (isalpha(lexeme[i] = getc(tape))) {
        i++;
        while (isalnum(lexeme[i] = getc(tape)))
            i++;
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        token = iskeyword(lexeme);
        if (token)
            return token;
        return ID;
    }

    ungetc(lexeme[i], tape);
    return 0;
}

token_t gettoken(FILE * tape)
{
    int             token;
    ignoreneutrals(tape);
    token = isID(tape);
    if (token)
        return token;
    return token = getc(tape);
}
