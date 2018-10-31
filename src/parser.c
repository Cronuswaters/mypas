/**@<parser.c>::**/
#include <stdio.h>
#include <stdlib.h>
#include <keywords.h>
#include <tokens.h>
#include <mytype.h>
#include <errcodes.h>
#include <lexer.h>
#include <parser.h>

token_t lookahead;
/***************************************************************************
Syntax definitions for the academic project My Pascal,
which is a simplified / modified Pascal compiler,
according to the following EBNF grammar:

Start symbol:: mypas

Production Rules::

mypas -> PROGRAM ID ; header body .
***************************************************************************/
void mypas(void)
{
	match(PROGRAM);
	match(ID);
	match(';');
	header();
	body();
	match('.');
}
/***************************************************************************
Declarative scope::

header -> varmodel { procmodel | funcmodel }
***************************************************************************/
void header(void){
    varmodel();
_procmodel_start:
    if(lookahead == PROCEDURE){
        procmodel();
        goto _procmodel_start;
    }
    if(lookahead == FUNCTION){
        funcmodel();
        goto _procmodel_start;
    }
}

/***************************************************************************

varmodel -> [ VAR  vargroup { vargroup } ]
***************************************************************************/
void varmodel(void){
	if(lookahead == VAR){
        match(VAR);
_vargroup_start:
        vargroup();
        if(lookahead == ID) goto _vargroup_start;
	}
}
/***************************************************************************

vargroup -> varlist : typemodif ;
***************************************************************************/
void vargroup(void){
    varlist();
    match(':');
    typemodif();
	match(';');
}

/***************************************************************************

varlist -> ID { , ID }
***************************************************************************/
void varlist(void){
_varlist_start:
    match(ID);
    if(lookahead == ','){
        match(',');
        goto _varlist_start;
    }
}

/***************************************************************************

typemodif -> BOOLEAN | INTEGER | REAL | DOUBLE
***************************************************************************/
void typemodif(void){
    switch(lookahead){
    case BOOLEAN:
        match(BOOLEAN);
        break;
    case INTEGER:
        match(INTEGER);
        break;
    case REAL:
        match(REAL);
        break;
    case DOUBLE:
        match(DOUBLE);
        break;
    default:
        fprintf(stderr, "ERROR: Uknown variable type\n");
        exit(ERR_UNKNOWN_TYPE);
    }
}

/***************************************************************************

procmodel -> PROCEDURE ID formalparams ; header body ;
***************************************************************************/
void procmodel(void){
	match(PROCEDURE);
	match(ID);
	formalparams();
	match(';');
	header();
	body();
	match(';');
}

/***************************************************************************

funcmodel -> FUNCTION ID formalparams : typemodif ; header body ;
***************************************************************************/
void funcmodel(void){
	match(FUNCTION);
	match(ID);
	formalparams();
	match(':');
	typemodif();
	match(';');
	header();
	body();
	match(';');
}

/***************************************************************************

formalparams -> [ ( arglist ) ]
***************************************************************************/
void formalparams(void){
	if(lookahead == '('){
		match('(');
		arglist();
		match(')');
	}
}

/***************************************************************************

arglist -> argmodel { ; argmodel }
***************************************************************************/
void arglist(void){
_arglist_start:
	argmodel();
	if(lookahead == ';'){
		match(';');
		goto _arglist_start;
	}
}

/***************************************************************************

argmodel -> [ VAR ] varlist : typemodif
***************************************************************************/
void argmodel(void){
	if(lookahead == VAR) match(VAR);
	varlist();
	match(':');
	typemodif();
}

/***************************************************************************
Imperative scope::

body -> BEGIN stmtlist END
***************************************************************************/
void body(void){
	match(BEGIN);
	stmtlist();
	match(END);
}

/***************************************************************************

stmtlist -> stmt { ; stmt }
***************************************************************************/
void stmtlist(void){
_stmtlist_start:
	stmt();
	if(lookahead == ';'){
		match(';');
		goto _stmtlist_start;
	}
}

/***************************************************************************

stmt ->   body
| ifstmt
| whlstmt
| repstmt
| smpexpr
| <empty>
***************************************************************************/
void stmt(void){
	switch(lookahead){
	case BEGIN:
		body();
		break;
	case IF:
		ifstmt();
		break;
	case WHILE:
		whlstmt();
		break;
	case REPEAT:
		repstmt();
		break;
	case '+':
	case '-':
	case '(':
	case NOT:
	case ID:
	case UINT:
	case FLTP:
	case TRUE:
	case FALSE:
		smpexpr();
		break;
	default:
		break;
	}
}

/***************************************************************************

ifstmt -> IF expr THEN stmt [ ELSE stmt ]
***************************************************************************/
void ifstmt(void){
	match(IF);
	expr();
	match(THEN);
	stmt();
	if(lookahead == ELSE){
		match(ELSE);
		stmt();
	}
}

/***************************************************************************

whlstmt -> WHILE expr DO stmt
***************************************************************************/
void whlstmt(void){
	match(WHILE);
	expr();
	match(DO);
	stmt();
}

/***************************************************************************

repstmt -> REPEAT stmtlist UNTIL expr
***************************************************************************/
void repstmt(void){
	match(REPEAT);
	stmtlist();
	match(UNTIL);
	expr();
}

/***************************************************************************

expr -> smpexpr [ relop smpexpr ]

***************************************************************************/
void expr(void){
	smpexpr();
	switch(lookahead){
	case '=':
	case '>':
	case '<':
	case GEQ:
	case LEQ:
	case NEQ:
		relop();
		smpexpr();
		break;
	default:
		break;
	}
}

/***************************************************************************

relop ->  =
	| >
	| ">="	// GEQ
	| <
	| "<="	// LEQ
	| "<>"	// NEQ


***************************************************************************/
void relop(void){
	switch(lookahead){
	case '=':
		match('=');
		break;
	case '>':
		match('>');
		break;
	case '<':
		match('<');
		break;
	case GEQ:
		match(GEQ);
		break;
	case LEQ:
		match(LEQ);
		break;
	case NEQ:
		match(NEQ);
		break;
	default:
		break;
	}
}

/***************************************************************************

smpexpr -> [ + | - ] term { oplus term }
***************************************************************************/
void smpexpr(void){
	if(lookahead == '+' || lookahead == '-') match(lookahead);
	term();
_oplus_check:
	if(lookahead == '+' || lookahead == '-'){
		match(lookahead);
		term();
		goto _oplus_check;
	}
}

/***************************************************************************

term -> factor { otimes factor }
***************************************************************************/
void term(void){
	factor();
_otimes_check:
	if(lookahead == '*' || lookahead == '/'){
		match(lookahead);
		factor();
		goto _otimes_check;
	}
}

/***************************************************************************

factor ->   ID [ ":=" expr  | ( exprlist ) ]
  | UINT
  | FLTP
  | ( expr )
***************************************************************************/
void factor(void){
	switch(lookahead){
	case ID:
		match(ID);
		if(lookahead == ASGN){
			match(lookahead);
			expr();
		} else if(lookahead == '('){
			match(lookahead);
			exprlist();
			match(')');
		}
		break;
	case UINT:
	case FLTP:
		match(lookahead);
		break;
	default:
		match('(');
		expr();
		match(')');
		break;
	}
}

/***************************************************************************

exprlist -> expr { , expr }
***************************************************************************/
void exprlist(void){
_exprlist_start:
	expr();
	if(lookahead == ','){
		match(',');
		goto _exprlist_start;
	}
}


void match(token_t expected){
	if(lookahead == expected){
		lookahead = gettoken(source);
	} else{
		fprintf(stderr, "ERROR: Token Mismatch at line %d\n",linenumber);
		exit(ERR_TOKEN_MISMATCH);
	}
}
