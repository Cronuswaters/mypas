/**@<symtab.c>::**/
#include <lexer.h>
#include <string.h>

#define LINESIZE 80
#define MAXSYMBOLS 0xFFFF
#define FLAGSIZE 4
#define VAR_FLAG 0 // 0 - var, 1 - procedure, 2 - function
#define ARG_COUNT 1

struct __symtab__{
	char name[MAXIDLEN+1];
	int type;
	char *code;
	char flags[FLAGSIZE]; // should be 32 bits
};

typedef struct __symtab__ symtab;

symtab table[MAXSYMBOLS]; // 65535 symbols
size_t cursymbol = 0; // next symbol

size_t lookup(const char *query){
	size_t i;
	for(i = 0; i < cursymbol; i++) if(!strcmp(table[i].name, query)) return i;
	return -1; // means symbol was not found
}

char* make_code(int index){
	char *result = calloc(LINESIZE+1, sizeof(char));
	strcpy(result, table[index].name);
	strcat(result, "\t");
	switch(table[index].type){
	case 1:
	case 2:
		strcat(result, "DD");
		break;
	case 3:
		strcat(result, "DQ");
		break;
	default:
		strcat(result, "DB");
		break;
	}
	strcat(result, "\t");
	return result;
}


int append(const char *symbol, int type, char flags[FLAGSIZE]){
// TODO: add overflow protection
	if(lookup(symbol) == -1){
        /*table[cursymbol].name = 0;
		strcpy(table[cursymbol].name, symbol);*/
		memcpy(table[cursymbol].name, symbol, strlen(symbol));
		table[cursymbol].type = type;
		table[cursymbol].code = make_code(cursymbol);
		int i;
		for(i = 0; i < FLAGSIZE; i++) table[cursymbol].flags[i] = flags[i];
		cursymbol++;
		return 1;
	}
	return 0;
}

int get_symtype(int index){
    return table[index].type;
}
