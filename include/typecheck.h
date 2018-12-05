/**@<typecheck.h>::**/
#pragma once
flag_t opcompat(int, int);
flag_t typecompat(int, int);

int promote(int, int);

int opcheck(token_t);

int getrange(int);
int toktotype(token_t);

void verify_promote(int);
void typecheck_asgn(int, int);
