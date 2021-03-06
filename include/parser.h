/**@<parser.h>**/
#pragma once
extern token_t lookahead;
extern FILE *source;

void match(token_t expected);
void token_mismatch(token_t, token_t);
void mypas(void);
void header(void);
void body(void);
void varmodel(void);
void vargroup(void);
void varlist(void);
void typemodif(void);
void procmodel(void);
void funcmodel(void);
void formalparams(void);
void arglist(void);
void argmodel(void);
void stmtlist(void);
void stmt(void);
void ifstmt(void);
void whlstmt(void);
void repstmt(void);
int expr(void);
int relop(void);
int smpexpr(int);
int term(int);
int factor(int);
void exprlist(void);
