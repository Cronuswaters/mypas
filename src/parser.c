/**@<parser.c>::**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <keywords.h>
#include <tokens.h>
#include <mytype.h>
#include <errcodes.h>
#include <lexer.h>
#include <pseudocode.h>
#include <typecheck.h>
#include <symtab.h>
#include <parser.h>

#define loopalloc loopcount++

#define MAXSYMBOLS 0xFFFF
#define MAXIDLEN 0x100
#define FLAGSIZE 4
#define VAR_FLAG 0 // 0 - var, 1 - procedure, 2 - function
#define ARG_COUNT 1

token_t lookahead;
size_t loopcount = 1;
char varnames[MAXSYMBOLS][MAXIDLEN+1];
int var_index;
int type_id;
char flags[FLAGSIZE];

char *token_str[] =
{
    "ID",
    "UINT",
    "FLTP",
    "DBLPT",
    "GEQ",
    "LEQ",
    "NEQ",
    "ASGN",
    NULL
};

char *keyword_str[] =
{
    "BEGIN",
	"END",
	"IF",
	"THEN",
	"ELSE",
	"WHILE",
	"DO",
	"REPEAT",
	"UNTIL",
	"PROGRAM",
	"VAR",
	"PROCEDURE",
	"FUNCTION",
	"INTEGER",
	"REAL",
	"DOUBLE",
	"BOOLEAN",
	"NOT",
	"OR",
	"DIV",
	"MOD",
	"AND",
	"TRUE",
	"FALSE",
	NULL
};
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
	/**/int i;/**/
	/**/for(i = 0; i < var_index; i++){
	    /**/flags[VAR_FLAG] = 0;/**/
	    /**/flags[ARG_COUNT] = 0;/**/
		/**/append(varnames[i], type_id, flags);/**/
	/**/}/**/
	/**/var_index = 0;/**/
}

/***************************************************************************

varlist -> ID { , ID }

***************************************************************************/
void varlist(void){
_varlist_start:
    /**/strcpy(varnames[var_index], lexeme);/**/
	/**/var_index++;/**/
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
	/**/type_id = toktotype(lookahead);/**/
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
	/**/strcpy(varnames[var_index], lexeme);/**/
	/**/var_index++;/**/
	match(ID);
	formalparams();
	match(';');
	/**/flags[VAR_FLAG] = 1;/**/
	/**/flags[ARG_COUNT] = var_index-1;/**/
	/**/var_index = 0;/**/
	/**/append(varnames[var_index], 0, flags);/**/
	header();
	body();
	match(';');
}

/***************************************************************************

funcmodel -> FUNCTION ID formalparams : typemodif ; header body ;

***************************************************************************/
void funcmodel(void){
	match(FUNCTION);
	/**/strcpy(varnames[var_index], lexeme);/**/
	/**/var_index++;/**/
	match(ID);
	formalparams();
	match(':');
	typemodif();
	match(';');
	/**/flags[VAR_FLAG] = 2;/**/
	/**/flags[ARG_COUNT] = var_index-1;/**/
	/**/var_index = 0;/**/
	/**/append(varnames[var_index], type_id, flags);/**/
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
	/**/int i;/**/
	/**/for(i = var_index; i > 0; i--){/**/
        /**/flags[VAR_FLAG] = 0;/**/
        /**/flags[ARG_COUNT] = 0;/**/
		/**/append(varnames[i], type_id, flags);/**/
	/**/}/**/
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
		smpexpr(0);
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
	int expr_type;
	/* 0' */
	match(IF);
	/**/expr_type = /**/expr();
	/**/
        if(expr_type != 4){
            fprintf(stderr, "Error at line %ld, column %ld: Expression must be logical.\n",linenumber,colnumber);
            fprintf(stderr, "Exiting compiler...\n");
            exit(ERR_TYPE_MISMATCH);
        }
    /**/
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
	int expr_type;
	/* 0' */
	match(WHILE);
	/* 1 */
	//printf(".L %ld\n", _whilehead = loopalloc);
	mklooplabel(_whilehead = loopalloc);
	/* 1' */
	/**/expr_type = /**/expr();
	/**/
        if(expr_type != 4){
            fprintf(stderr, "Error at line %d, column %d: Expression must be logical.\n");
            fprintf(stderr, "Exiting compiler...\n");
            exit(ERR_TYPE_MISMATCH);
        }
    /**/
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
	int expr_type;
	/* 0' */
	match(REPEAT);
	/* 1 */
	//printf(".L %ld\n", _rephead = loopalloc);
	mklooplabel(_rephead = loopalloc);
	/* 1' */
	stmtlist();
	match(UNTIL);
	/**/expr_type = /**/expr();
	/**/
        if(expr_type != 4){
            fprintf(stderr, "Error at line %d, column %d: Expression must be logical.\n");
            fprintf(stderr, "Exiting compiler...\n");
            exit(ERR_TYPE_MISMATCH);
        }
    /**/
	/* 2 */
	//printf("\tgofalse .L %ld\n", _rephead);
	gofalse(_rephead);
	/* 2' */
}

/***************************************************************************

expr -> smpexpr [ relop smpexpr ]

***************************************************************************/
int expr(void){
    /**/int syntype = 0;/**/
	/**/int ltype = 0, rtype = 0, rel_op = 0;/**/
	/**/int typecheck;/**/
	/**/ltype = /**/smpexpr(0);
	syntype = ltype;
	switch(lookahead){
	case '=':
	case '>':
	case '<':
	case GEQ:
	case LEQ:
	case NEQ:
		/**/rel_op = /**/relop();
		/**/syntype = 4;/**/
		/**/rtype = /**/smpexpr(ltype);
		/**/ltype = promote(ltype, rtype);/**/
		/**/verify_promote(ltype);/**/
		/**/typecheck = relop_typecheck(ltype, rtype, rel_op);/**/
		/**/
            if(!typecheck){
                fprintf(stderr, "TYPE ERROR (line %ld, column %ld): Attempted to apply relational operator on mismatched types.\n",linenumber,colnumber);
                fprintf(stderr, "Exiting compiler...\n");
                exit(ERR_TYPE_MISMATCH);
            }
        /**/
		break;
	default:
		break;
	}
	/**/return syntype;/**/
}

/***************************************************************************

relop ->  =
	| >
	| ">="	// GEQ
	| <
	| "<="	// LEQ
	| "<>"	// NEQ

***************************************************************************/
int relop(void){
    /**/int op = 0;/**/
	switch(lookahead){
	case '=':
		match('=');
		/**/op = '=';/**/
		break;
	case '>':
		match('>');
		/**/op = '>';/**/
		break;
	case '<':
		match('<');
		/**/op = '<';/**/
		break;
	case GEQ:
		match(GEQ);
		/**/op = GEQ;/**/
		break;
	case LEQ:
		match(LEQ);
		/**/op = LEQ;/**/
		break;
	case NEQ:
		match(NEQ);
		/**/op = NEQ;/**/
		break;
	default:
		break;
	}
	/**/return op;/**/
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
/**/int typecheck = 0;/**/
/**/int isneg = 0;/**/
/**/token_t oplus = 0;/**/

	if(lookahead == '+' || lookahead == '-' || lookahead == NOT){
		/**/isneg = opcheck(lookahead);/**/
		/**/negate_f = lookahead == '+' ? 0 : 1;/**/
		match(lookahead);
		/**/mintype = getrange(isneg);/**/
		/**/typecheck = opcompat(isneg, inheritedtype);/**/
		/**/
			if(!typecheck){
				fprintf(stderr, "ERROR: Type mismatch at line %ld, column %ld, exiting...\n",linenumber,colnumber);
				exit(ERR_TYPE_MISMATCH);
			}
		/**/
		/**/currenttype = promote(inheritedtype, mintype);/**/
		/**/verify_promote(currenttype);/**/
	}
_oplus_check:
	// check for pending adds
	syntype = term(currenttype);

	// check for pending negate
	/**/
		if(negate_f){
			negate(currenttype);
		}
	/**/

	/**/
		if(oplus){
			add(oplus, currenttype);
			oplus = 0;
		}
	/**/

	if(lookahead == '+' || lookahead == '-' || lookahead == OR){
		/**/oplus = lookahead;/**/
		/**/
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
        /**/
		match(lookahead);
		int op_index = toktoindex(oplus);
		mintype = getrange(op_index);
		/**/typecheck = opcompat(op_index, syntype);/**/
		/**/
			if(!typecheck){
				fprintf(stderr, "ERROR: Type mismatch at line %ld, column %ld, exiting...\n",linenumber,colnumber);
				exit(ERR_TYPE_MISMATCH);
			}
        /**/
        /**/syntype = promote(syntype, mintype);/**/
        /**/verify_promote(syntype);/**/
		goto _oplus_check;
	}
	return syntype;
}
/***************************************************************************

term -> factor { otimes factor }     otimes= '*' | '/' | DIV | MOD | AND

***************************************************************************/
int term(int inheritedtype){
	/**/int syntype = inheritedtype;/**/
	/**/token_t otimes = 0;/**/
	/**/int typecheck = 0;/**/

_otimes_check:
	syntype = factor(inheritedtype);

	// check for pending multiply
	/**/
		if(otimes){

			mul(otimes, syntype);
			otimes = 0;
		}
	/**/

	if(lookahead == '*' || lookahead == '/' || lookahead == DIV || lookahead == MOD || lookahead == AND){
		/**/otimes = lookahead;/**/
		/**/
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
        /**/
		match(lookahead);
		int op_index = toktoindex(otimes);
		/**/syntype = getrange(op_index);/**/
		/**/typecheck = opcompat(op_index, syntype);/**/
		/**/
            if(!typecheck){
                fprintf(stderr, "ERROR: Type mismatch at line %ld, column %ld, exiting...\n",linenumber,colnumber);
                exit(ERR_TYPE_MISMATCH);
            }
        /**/
		/**/syntype = promote(inheritedtype, syntype);/**/
		/**/verify_promote(syntype);/**/
		goto _otimes_check;
	}
	return syntype;
}

/***************************************************************************

factor ->   ID [ ":=" expr  | ( exprlist ) ]
  | UINT
  | FLTP
  | DBLPT
  | ( expr )

***************************************************************************/
int factor(int inheritedtype){
/**/int factor_t = inheritedtype;/**/
/**/int id_type;/**/
/**/int index;/**/
	switch(lookahead){
	case ID:
	    /**/index = (int)lookup(lexeme);/**/
	    /**/
            if(index == -1){ // error, symbol not found
                fprintf(stderr, "ERROR (line %ld, column %ld): variable %s not declared.\n",linenumber,colnumber,lexeme);
            }
        /**/
	    /**/id_type = get_symtype(index);/**/
		match(ID);
		if(lookahead == ASGN){
			match(lookahead);
			/**/factor_t = /**/expr();
			/**/typecheck_asgn(id_type, factor_t);/**/
		} else{
		    /**/factor_t = id_type;/**/
		    if(lookahead == '('){
                match(lookahead);
                exprlist();
                match(')');
		    }
		}
		break;
	case UINT:
	    /**/factor_t = 1;/**/
	    match(lookahead);
	    break;
	case FLTP:
	    /**/factor_t = 2;/**/
		match(lookahead);
		break;
    case DBLPT:
        /**/factor_t = 3;/**/
        match(lookahead);
        break;
    case TRUE:
    case FALSE:
        /**/factor_t = 4;/**/
        match(lookahead);
        break;
	default:
		match('(');
		expr();
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
	    token_mismatch(lookahead, expected);
	}
}

void token_mismatch(token_t lookahead, token_t expected){
    char *expected_str = "NULL";
    char *found_str = "NULL";
    if(expected >= BEGIN){ // expected is a keyword (>=0x20000)
        expected_str = keyword_str[expected - BEGIN];
    } else{ // expected is a token (>=0x10000)
        expected_str = token_str[expected - ID];
    }

    if(lookahead >= BEGIN){ // found is a keyword (>=0x20000)
        found_str = keyword_str[lookahead - BEGIN];
    } else{
        found_str = token_str[lookahead - ID];
    }
    fprintf(stderr, "ERROR: Token Mismatch at line %ld, column %ld\nExpected %s, found %s\n",linenumber,colnumber,expected_str,found_str);
    exit(ERR_TOKEN_MISMATCH);
}
