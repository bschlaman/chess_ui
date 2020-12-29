#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>	
#include <time.h>
#include "colors.h"
#include "defs.h"

int parseArgs(char *inputFEN, int argc, char *argv[]){
	int c;
	while((c = getopt(argc, argv, "f:")) != -1){
		switch(c){
			case 'f':
				strcpy(inputFEN, optarg);
				return FEN_MODE;
				break;
			case '?':
				if (optopt == 'f')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				fprintf(stderr, "Error with arg parsing", optopt);
				exit(1);
		}
	}
	return NORMAL_MODE;
}

int randInt(int lb, int ub){
	time_t t;
	srand((unsigned) time(&t));
	return rand() % (ub - lb + 1) + lb;
}

const int levels = 3;
struct node {
	int data;
	char name[30];
	struct node *children[4];
};

int indent(int data){
	return levels - data + 1;
}
int numChildren(){
	return sizeof(((struct node *)0) -> children) / sizeof(((struct node *)0) -> children[0]);
}

// returns pointer to node
struct node* newNode(int data){
	if(data == 0){
		return NULL;
	}
	struct node* n = (struct node*)malloc(sizeof(struct node));
	n -> data = data;
	for(int i = 0 ; i < numChildren() ; i++){
		n -> children[i] = newNode(data - 1);
	}
	return n;
}


void printNode(struct node* n){
	printf(YEL "level: " reset "%d", n -> data);
	for(int i = 0 ; i < numChildren() ; i++){
		printf("\n");
		for(int j = 0  ; j < indent(n -> data) ; j++){ printf("+"); }
		if(n -> children[i] == NULL){
			printf("null_child ");
		} else {
			printNode(n -> children[i]);
		}
	}
}


void nodeTest(){
	struct node *n = newNode(levels);
	printNode(n);
}

int main(int argc, char *argv[]){
	// arg stuff
	char inputFEN[99];
	int res = parseArgs(inputFEN, argc, argv);

	// node stuff (12/28)
	nodeTest();

	unsigned short int fromto = 65534;
	printf(CYN "fromto: %d\n" reset, fromto & 15);

	return 0;
}
