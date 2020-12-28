#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>	
#include <time.h>
#include "colors.h"

int parseArgs(char *inputFEN, int argc, char *argv[]){
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "f:")) != -1){
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

int randInt(int lb, int ub){
	time_t t;
	srand((unsigned) time(&t));
	return rand() % (ub - lb + 1) + lb;
}

struct node {
	int data;
	char name[30];
	struct node *children[4];
};

// returns pointer to node
struct node* newNode(int data){
	struct node* n = (struct node*)malloc(sizeof(struct node));
	n -> data = data;
	for(int i = 0 ; i < sizeof(((struct node *)0) -> children)/sizeof(((struct node *)0) -> children[0]) ; i++){
		n -> children[i] = NULL;
	}
	return n;
}

void nodeTest(){
	struct node *n = newNode(34143143);
	printf(YEL "n->data: %d\n" reset, n -> data);
}

int mode = 4;
void testMode(){
	printf(CYN "mode: %d\n" reset, mode);
}

int main(int argc, char *argv[]){
	// arg stuff
	char inputFEN[99];
	int res = parseArgs(inputFEN, argc, argv);

	mode = 5;
	testMode();
	// testing globals
	return 0;
}
