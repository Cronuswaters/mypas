/**@<pseudocode.c>::**/

#include <stdio.h>
#include <stdlib.h>
#include <keywords.h>
#include <pseudocode.h>

FILE *source, *object;

/*
	Create var label
*/
void mklabel(char *label){
	fprintf(object, ".L %s\n",label);
}

/*
	Control pseudoinstructions
*/
void gofalse(size_t label){
	fprintf(object, "\tjz .L %ld\n",label);
}

void jmp(size_t label){
	fprintf(object, "\tjmp .L %ld\n",label);
}

void mklooplabel(size_t label){
	fprintf(object, ".L %ld\n",label);
}

/*
	Move pseudoinstructions
*/
void push(char *suffix){
	fprintf(object, "\tpush %%acc%s\n",suffix);
}
void pop(char *suffix){
	fprintf(object, "\tpop %%acc%s\n",suffix);
}
void ld(char *var, char *suffix){	// Load (var -> acc)
	fprintf(object, "\tmov %s, %%acc%s\n",var,suffix);
}
void st(char *var, char *suffix){ // Store (acc -> var)
	fprintf(object, "\tmov %%acc%s, %s\n",suffix,var);
}

/*
	LAU pseudoinstructions
*/

// Add
void addi(void){ // Add (integer)
	fprintf(object, "\taddi %%accl, %%(spl)\n");
}
void fadd(void){ // Add (float)
	fprintf(object, "\tfadd %%accf, %%(spf)\n");
}
void dfadd(void){	// Add (double)
	fprintf(object, "\tdfaff %%accdf, %%(spdf)\n");
}
void subi(void){ // Subtract (integer)
	fprintf(object, "\tsubi %%accl, %%(spl)");
}
void fsub(void){ // Subtract (float)
	fprintf(object, "\tfsub %%accf, %%(spf)\n");
}
void dfsub(void){ // Subtract (double)
	fprintf(object, "\tdfsub %%accdf, %%(spdf)\n");
}
void orb(){ // Bytewise OR (logical)
	fprintf(object, "\torb %%accb, %%(spb)\n");
}

/*
	Macro-instruction add
*/
int add(int op, int size){
	if(op == '+'){
		switch(size){
		case 1:
			addi();
			break;
		case 2:
			fadd();
			break;
		case 3:
			dfadd();
			break;
		default:
			return -1;
		}
	} else if(op == '-'){
		switch(size){
		case 1:
			subi();
			break;
		case 2:
			fsub();
			break;
		case 3:
			dfsub();
			break;
		default:
			return -1;
		}
	} else if(op == OR){
		if(size == 4) orb();
		else return -1;
	} else return -1;
	return 0;
}

// Mul
void lmul(void){ // Multiply (integer)
	fprintf(object, "\tlmul %%accl, %%(spl)\n");
}
void fmul(void){ // Multiply (float)
	fprintf(object, "\tfmul %%accf, %%(spf)\n");
}
void dfmul(void){ // Multiply (double)
	fprintf(object, "\tdfmul %%accdf, %%(spdf)\n");
}
void divl(void){ // Divide (integer, also has remainder)
	fprintf(object, "\tldiv %%accl, %%(spl)\n");
}
void fdiv(void){ // Divide (float)
	fprintf(object, "\tfdiv %%accf, %%(spf)\n");
}
void dfdiv(void){ // Divide (double)
	fprintf(object, "\tdfdiv %%accdf, %%(spdf)\n");
}
void andb(void){ // Bytewise AND (logical)
	fprintf(object, "\tandb %%accb, %%(spb)\n");
}

void accltof(void){
	fprintf(object, "\tcvltof %%accl, %%accf\n");
}

void spltof(void){
	fprintf(object, "\tcvltof %%(spl), %%(spf)\n");
}

/*
	Macro-instruction mul
*/

int mul(int op, int size){
	if(op == '*'){
		switch(size){
		case 1:
			lmul();
			break;
		case 2:
			fmul();
			break;
		case 3:
			dfmul();
			break;
		default:
			return -1;
		}
	} else if(op == '/'){
		switch(size){
		case 1:
			accltof();
		case 2:
			fdiv();
			break;
		case 3:
			dfdiv();
			break;
		default:
			return -1;
		}
	} else if(op == DIV){
		if(size == 1) divl();
		else return -1;
	} else if(op == MOD){
		if(size == 1){
			divl();
			ld("%%rmdr", "l");
		} else return -1;
	}else if(op == AND){
		if(size == 4) andb();
		else return -1;
	}
	return 0;
}

// Unary operations
void ineg(void){ // Negate (integer)
	fprintf(object, "\tineg %%accl\n");
}
void fneg(void){ // Negate (float)
	fprintf(object, "\tfneg %%accf\n");
}
void dfneg(void){ // Negate (double)
	fprintf(object, "\tdfneg %%accdf\n");
}
void notb(void){ // Bytewise NOT (logical)
	fprintf(object, "\tnotb %%accb\n");
}

/*
	Macro-instruction negate
*/
int negate(int size){
	switch(size){
	case 1:
		ineg();
		break;
	case 2:
		fneg();
		break;
	case 3:
		dfneg();
		break;
	case 4:
		notb();
		break;
	default:
		return -1;
	}
	return 0;
}
