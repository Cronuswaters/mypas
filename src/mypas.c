/**@<mypas.c>::**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>
#include <pseudocode.h>
#include <errcodes.h>

#define NUM_EXTENSIONS 3

const char *extensions[] = {".pas", ".mypas", ".npas"};
const char *default_obj = "a.out";

int main(int argc, char *argv[]){
	int i, j;
	char *objpath = 0, *srcpath = 0;
	if(argc == 1) printf("Please specify a file to open\n");
	else{
		for(i = 1; i < argc; i++){
			if(!strcmp(argv[i], "-o")){ /*if(argv[i] == "-o"){*/
				//i++;
				//strcpy(objpath, argv[i]);
				objpath = argv[++i];
			}
			else{
				for(j = 0; j < NUM_EXTENSIONS; j++){
					if(strstr(argv[i], extensions[j]) != NULL){
						//strcpy(srcpath, argv[i]);
						srcpath = argv[i];
						break;
					}
				}
			}
		}
		if(srcpath != 0){
			source = fopen(srcpath, "r");
			if(source == NULL){
				fprintf(stderr, "Error opening source file %s.\n",srcpath);
				exit(ERR_FILE_ERROR);
			}
		}else{
			fprintf(stderr, "Could not open source file: File is not a valid Pascal source file.\n");
			exit(ERR_INVALID_FILE);
		}

		if(objpath == 0) objpath = "a.out";//strcpy(objpath, default_obj);
		object = fopen(objpath, "wt");
		if(object == NULL){
			fprintf(stderr, "Error opening object file %s.\n", objpath);
			exit(ERR_FILE_ERROR);
		}

		lookahead = gettoken(source);

		mypas();
	}
	exit(0);
}
