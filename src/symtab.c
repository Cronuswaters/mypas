/**@<symtab.c>::**/
#include <string.h>

#define LINESIZE 80
#define MAXSYMBOLS 0xFFFF

struct __symtab__{
	char *name;
	int type;
	char offset[LINESIZE+1];
};

typedef struct __symtab__ symtab;

symtab table[MAXSYMBOLS]; // 65535 symbols
size_t cursymbol = 0; // next symbol

size_t lookup(const char *query){
	size_t i;
	for(i = 0; i < cursymbol; i++) if(!strcmp(table[i].name, query)) return i;
	return -1; // means symbol was not found
}

int append(const char *symbol, int type){
// TODO: add overflow protection
	if(lookup(symbol) == -1){
		strcpy(table[cursymbol].name, symbol);
		table[cursymbol].type = type;
		cursymbol++;
		return 1;
	}
	return 0;
}

