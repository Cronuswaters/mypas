/**@<mypas.c>::**/
#include <stdio.h>
#include <stdlib.h>
#include <lexer.h>
#include <parser.h>
#include <errcodes.h>

FILE *source;

int main(int argc, char *argv[]){
	if(argc == 1) printf("Please specify a file to open");
	else{
		source = fopen(argv[1], "r");
		if(source == NULL){
		        fprintf(stderr, "Error opening file %s.\n",argv[1]);
			exit(ERR_FILE_ERROR);
		}

		lookahead = gettoken(source);

		mypas();
	}
	exit(0);
}
