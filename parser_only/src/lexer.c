/**@<lexer.c>::**/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <mytype.h>
#include <tokens.h>
#include <errcodes.h>
#include <lexer.h>

char            lexeme[MAXIDLEN + 1];
size_t		linenumber = 1;
size_t          colnumber = 1;

token_t ignoreneutrals(FILE * tape)
{
    int             head;

_ignoreneutrals_start:

    while (isspace(head = getc(tape))) {
        colnumber++;
	    if (head == '\n') {
		    linenumber++;
		    colnumber = 1;
	    }
    }
    colnumber++;
    if ( head == '{' ) {
        colnumber++;
	    while ( (head = getc(tape)) != '}' ) {
            colnumber++;
		    if (head == EOF) {
			    return EOF;
		    }
		    if (head == '\n') {
			    linenumber++;
			    colnumber = 1;
		    }
	    }
	    goto _ignoreneutrals_start;
    }
    ungetc(head, tape);
    colnumber--;
    return 0;
}

/*
    ASGN = ":="
*/
token_t isASGN(FILE * tape)
{
	int i;
	colnumber++;
	if ( (lexeme[i = 0] = getc(tape)) == ':' ) {
        colnumber++;
		if ( (lexeme[++i] = getc(tape)) == '=' ) {
			lexeme[i+1] = 0;
			return ASGN;
		}
		ungetc(lexeme[i], tape);
		i--;
		colnumber--;
	}
	ungetc(lexeme[i], tape);
	colnumber--;
	return lexeme[i] = 0;
}

/*
    RELOP = < | "<=" | "<>" | > | ">=" | =
*/
token_t isRELOP(FILE * tape)
{
	int i;
	colnumber++;
	switch (lexeme[i = 0] = getc(tape)) {
	case'<':
        colnumber++;
		lexeme[++i] = getc(tape);
		lexeme[i+1] = 0;
		switch (lexeme[i]) {
		case'=':
			return LEQ;
		case'>':
			return NEQ;
		}
		colnumber--;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return lexeme[--i];
	case'>':
	    colnumber++;
		lexeme[++i] = getc(tape);
		if (lexeme[i] == '=') {
			lexeme[i+1] = 0;
			return GEQ;
		}
		colnumber--;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return lexeme[--i];
	case'=':
		lexeme[i+1] = 0;
		return lexeme[i];
	}
	colnumber--;
	ungetc(lexeme[i], tape);
	return lexeme[i] = 0;
}

/*
    ID = [A-Za-z][A-Za-z0-9_]*
*/
token_t isID(FILE * tape)
{
    int             i = 0;
    int             token;

    colnumber++;
    if (isalpha(lexeme[i] = getc(tape))) {
        i++;
        colnumber++;
        while ((isalnum(lexeme[i] = getc(tape))) || (lexeme[i] == '_')){
	    i++;
	    colnumber++;
	}
    ungetc(lexeme[i], tape);
	colnumber--;
    lexeme[i] = 0;
    token = iskeyword(lexeme);
    if (token) return token;
    return ID;
    }

    ungetc(lexeme[i], tape);
    colnumber--;
    return 0;
}

/*
    EE = [Ee]['+''-']?[0-9]+
*/
int chkEE(FILE *tape, int i0){
    int i = i0;
    colnumber++;
    if(toupper(lexeme[i] = getc(tape)) == 'E'){
        i++;
        colnumber++;
        if((lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-'){
            i++;
        } else{
            colnumber--;
            ungetc(lexeme[i], tape);
            lexeme[i] = 0;
        }

        colnumber++;
        if(isdigit(lexeme[i] = getc(tape))){
            i++;
            colnumber++;
            while(isdigit(lexeme[i++] = getc(tape))) colnumber++;
            colnumber--;
            ungetc(lexeme[i], tape);
            lexeme[i] = 0;
            return i;
        } else{
            for(; i >= i0; i--){
                colnumber--;
                ungetc(lexeme[i], tape);
            }
            return i0;
        }
    }
    colnumber--;
    ungetc(lexeme[i], tape);
    return i0;
}

/*
    NUM = ((0|[1-9][0-9]*)('.'[0-9]*)?|(0|[1-9][0-9]*)?'.'[0-9]*)EE?
*/
token_t isNUM(FILE *tape){
    int i, i0;
    int token = 0;
    colnumber++;
    if(isdigit(lexeme[i = 0] = getc(tape))){
        token = UINT;
        if(lexeme[i] == '0') i++;
        else{
            colnumber++;
            while(isdigit(lexeme[++i] = getc(tape))) colnumber++;;
            colnumber--;
            ungetc(lexeme[i], tape);
        }
    } else{
        colnumber--;
        ungetc(lexeme[i], tape);
    }
    lexeme[i] = 0;

    colnumber++;
    if((lexeme[i] = getc(tape)) == '.'){
        i++;
        if(token == UINT) token = FLTP;
        else{
            colnumber++;
            if(isdigit(lexeme[i] = getc(tape))) token = FLTP;
            else{
                colnumber--;
                ungetc(lexeme[i], tape);
                colnumber--;
                ungetc('.', tape);
                lexeme[--i] = 0;
                return token;
            }
        }
        colnumber++;
        while(isdigit(lexeme[i] = getc(tape))){
            colnumber++;
            i++;
        }
        colnumber--;
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
    } else{
        colnumber--;
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
    }

    if(token == 0){
        return 0;
    }

    i0 = i;
    i = chkEE(tape, i);
    if(i != i0 && token == UINT) token = FLTP;
    return token;
}

token_t gettoken(FILE * tape)
{
    int             token;
    token = ignoreneutrals(tape);
    if(token){
        fprintf(stderr, "ERROR: End-of-file found before comment block was closed.\n");
        exit(ERR_PREMATURE_EOF);
    }
    token = isASGN(tape);
    if(token) return token;
    token = isRELOP(tape);
    if(token) return token;
    token = isID(tape);
    if(token) return token;
    token = isNUM(tape);
    if(token) return token;
    return token = getc(tape);
}

