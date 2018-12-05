/**@<symtab.h>::**/
#pragma once

#define LINESIZE 80
#define MAXSYMBOLS 0xFFFF
#define FLAGSIZE 4
#define VAR_FLAG 0 // 0 - var, 1 - procedure, 2 - function
#define ARG_COUNT 1


struct __symtab__{
	char *name;
	int type;
	char code[LINESIZE+1];
	char flags[FLAGSIZE]; // 32 bits
};

typedef struct __symtab__ symtab;

symtab table[MAXSYMBOLS]; // 65535 symbols

size_t lookup(const char *query);
int append(const char *symbol, int type, char *flags);
int get_symtype(int index);
