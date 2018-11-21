/**@<parser.c>::**/
#include <stdio.h>
#include <stdlib.h>
#include <keywords.h>
#include <tokens.h>
#include <mytype.h>
#include <errcodes.h>
#include <lexer.h>
#include <pseudocode.h>
#include <typecheck.h>
#include <parser.h>

#define loopalloc loopcount++

token_t lookahead;
size_t loopcount = 1;
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
	// MyPas Keywords
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
	// Unary operators:
	case '+':
	case '-':
	case NOT:
	// first for variables, functions, procedures
	case ID:
	// first for constants
	case UINT:
	case FLTP:
	case TRUE:
	case FALSE:
	// parentheses
	case '(':
		smpexpr();
		break;
	default:
		// Default abstracts an empty statement
		break;
	}
}

/***************************************************************************

ifstmt ->				|| <ifstmt>.as <-
		IF expr			||	<expr>.as
		THEN			||	"gofalse .L <<_lbl_endif = _lbl_else = loopcount++>>"
		stmt			||	<stmt>.as
		[ ELSE stmt ]	||	"JMP .L <<_lbl_endif = loopcount++>>
						|| ".L <<_lbl_else>>"
						||	<stmt>.as
						|| ".L <<_lbl_endif>>"

***************************************************************************/
void ifstmt(void){
	/* 0 */
	size_t _lbl_else, _lbl_endif;
	/* 0' */
	match(IF);
	expr();
	match(THEN);
	/* 1 */
	//printf("\tgofalse .L %ld\n",_lbl_endif = _lbl_else = loopalloc);
	gofalse(_lbl_endif = _lbl_else = loopalloc);
	/* 1' */
	stmt();
	if(lookahead == ELSE){
		match(ELSE);
		/* 2 */
		//printf("\tJMP .L %ld\n",_lbl_endif = loopalloc);
		jmp(_lbl_endif = loopalloc);
		// printf(".L %ld\n",_lbl_else);
		mklooplabel(_lbl_else);
		/* 2' */
		stmt();
	}
	/* 3 */
	//printf(".L %ld\n",_lbl_endif);
	mklooplabel(_lbl_endif);
	/* 3' */
}

/***************************************************************************

whlstmt ->				|| <whlstmt>.as <-
		WHILE expr DO	|| ".L <<_whilehead = loopcount++>>
		stmt			||	<expr>.as
						||	"gofalse .L <<_whiletail = loopcount++>>
						||	<stmt>.as
						||	"JMP .L <<_whilehead>>
						|| ".L <<_whiletail>>

***************************************************************************/
void whlstmt(void){
	/* 0 */
	size_t _whilehead, _whiletail;
	/* 0' */
	match(WHILE);
	/* 1 */
	//printf(".L %ld\n", _whilehead = loopalloc);
	mklooplabel(_whilehead = loopalloc);
	/* 1' */
	expr();
	/* 2 */
	//printf("\tgofalse .L %ld\n", _whiletail = loopalloc);
	gofalse(_whiletail = loopalloc);
	/* 2' */
	match(DO);
	stmt();
	/* 3 */
	//printf("\tJMP .L %ld\n", _whilehead);
	jmp(_whilehead);
	//printf(".L %ld\n", _whiletail);
	mklooplabel(_whiletail);
	/* 3' */
}

/***************************************************************************

repstmt ->		|| <repstmt>.as
	REPEAT		|| ".L <<_rephead = loopcount++>>
	stmtlist	||	<stmtlist>.as
	UNTIL expr	||	<expr>.as
				||	"gofalse .L <<_rephead>>"

***************************************************************************/
void repstmt(void){
	/* 0 */
	size_t _rephead;
	/* 0' */
	match(REPEAT);
	/* 1 */
	//printf(".L %ld\n", _rephead = loopalloc);
	mklooplabel(_rephead = loopalloc);
	/* 1' */
	stmtlist();
	match(UNTIL);
	expr();
	/* 2 */
	//printf("\tgofalse .L %ld\n", _rephead);
	gofalse(_rephead);
	/* 2' */
}

/***************************************************************************

expr -> smpexpr [ relop smpexpr ]

***************************************************************************/
int expr(int type){
	/**/int syntype;/**/
	/**/syntype = /**/smpexpr();
	switch(lookahead){
	case '=':
	case '>':
	case '<':
	case GEQ:
	case LEQ:
	case NEQ:
		relop();
		/**/syntype = /**/smpexpr();
		break;
	default:
		break;
	}
	return syntype;
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

/*****************************************************************************************************

smpexpr -> [neg] term { oplus term }		neg = '+' | '-' | NOT		oplus= '+' | '-' | OR

******************************************************************************************************/
flag_t founderror = 0;
int smpexpr(int inheritedtype){
/**/int syntype = 0;/**/
/**/int currenttype = inheritedtype;/**/
/**/int mintype = 0;/**/
/**/int negate_f = 0;/**/
/**/int typecheck;/**/
/**/int isneg;/**/
/**/token_t oplus;/**/

	if(lookahead == '+' || lookahead == '-' || lookahead == NOT){
		/**/isneg = opcheck(lookahead);/**/
		/**/negate_f = lookahead == '+' ? 0 : 1;/**/
		match(lookahead);
		/**/mintype = getrange(isneg);/**/
		/**/typecheck = opcompat(isneg, inheritedtype);/**/
		/**/
			if(!typecheck){
				fprintf(stderr, "ERROR: Type mismatch, exiting...\n");
				exit(ERR_TYPE_MISMATCH);
			}
		/**/
		/**/currenttype = promote(inheritedtype, mintype);/**/
		/**/
			switch(currenttype){
			case -1:
				fprintf(stderr, "ERROR: attempted to cast type to void.\n");
				exit(ERR_CAST_ERROR);
			case -2:
				fprintf(stderr, "ERROR: attempted to cast from boolean to numeric.\n");
				exit(ERR_CAST_ERROR);
			case -3:
				fprintf(stderr, "ERROR: attempted to cast from numeric to boolean.\n");
				exit(ERR_CAST_ERROR);
			}
		/**/
	}
	// check for pending negate
	/**/
		if(negate_f){
			negate(currenttype);
		}
	/**/
_oplus_check:
	// check for pending adds
	/**/
		if(oplus){
			switch(currenttype){
			case 1:
				push("l");
				break;
			case 2:
				push("f");
				break;
			case 3:
				push("df");
				break;
			case 4:
				push("b");
				break;
			}
			add(oplus, currenttype);
			oplus = 0;
		}
	/**/
	syntype = term(currenttype);
	if(lookahead == '+' || lookahead == '-' || lookahead == OR){
		/**/oplus = lookahead;/**/
		match(lookahead);
		/**/typecheck = opcompat(oplus, syntype);/**/
		/**/
			if(!typecheck){
				fprintf(stderr, "ERROR: Type mismatch, exiting...\n");
				exit(ERR_TYPE_MISMATCH);
			}
		goto _oplus_check;
	}
	return syntype;
}
/***************************************************************************

term -> factor { otimes factor }     otimes= '*' | '/' | DIV | MOD | AND

***************************************************************************/
int term(int inheritedtype){
	/**/int syntype = inheritedtype;/**/
	/**/token_t otimes;/**/
	/**/int typecheck;/**/

_otimes_check:
	// check for pending multiply
	/**/
		if(otimes){
			switch(syntype){
			case 1:
				push("l");
				break;
			case 2:
				push("f");
				break;
			case 3:
				push("df");
				break;
			case 4:
				push("b");
				break;
			}
			mul(otimes, syntype);
			otimes = 0;
		}
	/**/
	syntype = factor(inheritedtype);
	
	if(lookahead == '*' || lookahead == '/' || lookahead == DIV || lookahead == MOD || lookahead == AND){
		/**/otimes = lookahead;/**/
		match(lookahead);
		/**/typecheck = opcompat(otimes, syntype);/**/
		if(!typecheck){
			fprintf(stderr, "ERROR: Type mismatch, exiting...\n");
			exit(ERR_TYPE_MISMATCH);
		}
		/**/syntype = promote(inheritedtype, syntype);/**/
		goto _otimes_check;
	}
	return syntype;
}

/***************************************************************************

factor ->   ID [ ":=" expr  | ( exprlist ) ]
  | UINT
  | FLTP
  | ( expr )
  
***************************************************************************/
int factor(int inheritedtype){
/**/int factor_t = inheritedtype;/**/
	switch(lookahead){
	case ID:
		match(ID);
		if(lookahead == ASGN){
			match(lookahead);
			factor_t = expr(inheritedtype);
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
		expr(0);
		match(')');
		break;
	}
	return factor_t;
}

/***************************************************************************

exprlist -> expr { , expr }

***************************************************************************/
void exprlist(void){
_exprlist_start:
	expr(0);
	if(lookahead == ','){
		match(',');
		goto _exprlist_start;
	}
}


void match(token_t expected){
	if(lookahead == expected){
		lookahead = gettoken(source);
	} else{
	  fprintf(stderr, "ERROR: Token Mismatch at line %ld, column %ld\nExpected (blank), found (blank)\n",linenumber,colnumber);
		exit(ERR_TOKEN_MISMATCH);
	}
}
