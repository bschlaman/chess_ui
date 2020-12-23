#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>	
#include <time.h>
#include "colors.h"

int genFEN(char *fen){
	int i, f;
	for(i = 0, f=0 ; i < 64 ; i++){
		if(i %3 == 0){
			fen[f] = '3';
			f++;
		}
	}
	fen[f] = '\0';
	return 0;
}

int parseArgs(char *inputFEN, int argc, char *argv[]){
	int c;
	opterr = 0;
  while ((c = getopt (argc, argv, "f:")) != -1){
		switch (c){
			case 'f':
				strcpy(inputFEN, optarg);
				break;
			case '?':
				if (optopt == 'f')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				exit(1);
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	char fen[100];
	char arr[100];
	char *ptr = arr; //same as char *ptr = &arr[0]
	genFEN(fen);

	char inputFEN[99];
	int ret = parseArgs(inputFEN, argc, argv);
	
	srand(time(0));
	int r = rand() % (7 - 3 + 1) + 3;
	printf(CYN "rand(): %d" reset, r);

}
