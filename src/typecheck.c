/**@<typecheck.c>::**/
#include <stdio.h>
#include <stdlib.h>
#include <lexer.h>
#include <mytype.h>
#include <tokens.h>
#include <errcodes.h>
#include <keywords.h>

/*
	0: VOID
	1: INT32
	2: FLT32
	3: FLT64
	4: BOOL
*/

/*
		0	1	2	3	4
	+	1	1	1	1	0
	-	1	1	1	1	0
	OR	0	0	0	0	1
	*	0	1	1	1	0
	/	0	1	1	1	0
	DIV	0	1	0	0	0
	MOD	0	1	0	0	0
	AND	0	0	0	0	1
	NOT	1	0	0	0	1
*/

#define NUM_OPER 9
#define NUM_TYPE 5

/*										VOID	INT32	FLT32	FLT64	BOOLEAN */
flag_t opcheck[NUM_OPER][NUM_TYPE] = {
/*  +  */								{1,		  1,	  1,	  1,	   0},
/*  -  */								{1,		  1, 	  1,	  1,	   0},
/* OR  */								{0,		  0,	  0,	  0,	   1},
/*  *  */								{0,		  1,	  1,	  1,	   0},
/*  /  */								{0,		  1,	  1,	  1,	   0},
/* DIV */								{0,		  1,	  0,	  0,	   0},
/* MOD */								{0,		  1,	  0,	  0,	   0},
/* AND */								{0,		  0,	  0,	  0,	   1},
/* NOT */								{1,		  0,	  0,	  0,	   1}
									 };

/*											VOID	INT32	FLT32	FLT64	BOOLEAN *//*(to)*/
flag_t promotes[NUM_TYPE][NUM_TYPE] =	{
/*(from)*/
/*  VOID   */								{ 0,	   1,	   2,	   3,    	4},
/*  INT32  */								{-1,	   1,	   2,	   3,	   -3},
/*  FLT32  */								{-1,	   2,	   2,	   3,	   -3},
/*  FLT64  */								{-1,	   3,	   3,	   3,	   -3},
/* BOOLEAN */								{-1,	  -2,	  -2,	  -2,	    4}
										};
flag_t opcompat(int oper, int type){
	return opcheck[oper][type];
}

/*flag_t typecompat(int type1, int type2){
	return typecheck[type1][type2];
}*/

int promote(int oldtype, int newtype){
	// possibly add instruction to expand acc here
	return promotes[oldtype][newtype];
}

int toktoindex(token_t op){
	switch(op){
	case '+':
		return 0;
	case '-':
		return 1;
	case OR:
		return 2;
	case '*':
		return 3;
	case '/':
		return 4;
	case DIV:
		return 5;
	case MOD:
		return 6;
	case AND:
		return 7;
	case NOT:
		return 8;
	default:
		return -1;
	}
}

int getrange(int op){
	switch(op){
	case 0: // +
	case 1: // -
	case 3: // *
	case 4: // /
	case 5: // DIV
	case 6: // MOD
		return 1;
	case 2: // OR
	case 7: // AND
	case 8: // NOT
		return 4;
	default:
	    return 0;
	}
}

int toktotype(token_t lookahead){
	switch(lookahead){
	case INTEGER:
		return 1;
		break;
	case REAL:
		return 2;
		break;
	case DOUBLE:
		return 3;
		break;
	case BOOLEAN:
		return 4;
		break;
	default:
		return 0;
		break;
	}
}

void verify_promote(int currenttype){
    switch(currenttype){
	case -1:
		fprintf(stderr, "ERROR (l%ld, c%ld): attempted to cast type to void.\n",linenumber,colnumber);
		exit(ERR_CAST_ERROR);
	case -2:
		fprintf(stderr, "ERROR (l%ld, c%ld): attempted to cast from boolean to numeric.\n",linenumber,colnumber);
		exit(ERR_CAST_ERROR);
	case -3:
		fprintf(stderr, "ERROR (l%ld, c%ld): attempted to cast from numeric to boolean.\n",linenumber,colnumber);
		exit(ERR_CAST_ERROR);
    }
}

int relop_typecheck(int ltype, int rtype, int relop){
    int result = 0;
    switch(relop){
    case '=':
    case NEQ:
        // = and NEQ can only be used if both are the same type
        if(ltype == rtype) result = 1;
        break;
    case '>':
    case '<':
    case GEQ:
    case LEQ:
        // >, <, GEQ and LEQ can only be used if both are the same type AND both are numeric
        if(ltype == rtype){
            switch(ltype){
            case 1:
            case 2:
            case 3:
                result = 1;
                break;
            }
        }
        break;
    }
    return result;
}

void typecheck_asgn(int id_type, int factor_t){
    // Types must be similar (Void-Void, Numeric-Numeric, Boolean-Boolean)
    int result;
    char *id_str, *factor_str;
    switch(id_type){
    case 0:
        if(id_type != factor_t){
            switch(factor_t){
            case 1:
                factor_str = "INTEGER";
                break;
            case 2:
                factor_str = "FLOAT";
                break;
            case 3:
                factor_str = "DOUBLE";
                break;
            case 4:
                factor_str = "BOOLEAN";
                break;
            }
            id_str = "VOID";
            result = 0;
        }
        break;
    case 1:
    case 2:
    case 3:
        id_str = id_type == 1 ? "INTEGER" : id_type == 2 ? "FLOAT" : "DOUBLE";
        if(factor_t == 0){
            factor_str = "VOID";
            result = 0;
        } else if(factor_t == 4){
            factor_str = "BOOLEAN";
            result = 0;
        }
        break;
    case 4:
        if(id_type != factor_t){
            switch(factor_t){
            case 0:
                factor_str = "VOID";
                break;
            case 1:
                factor_str = "INTEGER";
                break;
            case 2:
                factor_str = "FLOAT";
                break;
            case 3:
                factor_str = "DOUBLE";
                break;
            }
            id_str = "BOOLEAN";
        }
        break;
    }
    if(!result){
        fprintf(stderr, "Type error (l%ld, c%ld): Mismatched types on assignment, expected %s, found %s.\n",linenumber,colnumber,id_str,factor_str);
        fprintf(stderr, "Exiting compiler...\n");
        exit(ERR_TYPE_MISMATCH);
    }
}
