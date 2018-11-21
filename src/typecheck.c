/**@<typecheck.c>::**/
#include <mytype.h>
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

flag_t typecompat(int type1, int type2){
	return typecheck[type1][type2];
}

int promote(int oldtype, int newtype){
	// possibly add instruction to expand acc here
	return promotes[oldtype][newtype];
}
	
int opcheck(token_t op){
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
	}
}
