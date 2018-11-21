/**@<pseudocode.h>::**/
#pragma once
#include <stdlib.h>

extern FILE *source, *object;

/* Create var label */
void mklabel(char *);

/* Control operations */
void gofalse(size_t);
void jmp(size_t);
void mklooplabel(size_t);

/* Move operations */
void push(char *);
void pop(char *);
void ld(char *, char *);
void st(char *, char *);

/* LAU operations */
int add(int, int);
int mul(int, int);
int negate(int);
