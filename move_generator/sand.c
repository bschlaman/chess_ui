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

void initRand(){
	time_t t;
	srand((unsigned) time(&t));
}
int randInt(int lb, int ub){
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

typedef struct {
	int data;
} STATE;
typedef struct {
	STATE hist[10];
} BOARD;
BOARD* init(){
	BOARD *b = malloc(sizeof(BOARD));
	return b;
}

int testEval(){
	return randInt(-5, 5);
}

// int legalMoves[255][3];
int testGenLegalMoves(){
	int i, total = 0;

	// init legalMoves
	for(int m  = 0 ; m < 255 ; m++){
		legalMoves[m][2] = 0;
	}
	total = randInt(2,2);
	for(i = 0 ; i < total ; i++){
		legalMoves[i][2] = 3;
	}
	return total;
}

void printArr(int arr[][3]){
	for(int i = 0 ; i < 5 ; i++){
		printf(YEL "arr[i][2]: %d\n" reset, arr[i][2]);
	}
}

int miniMax(int depth){
	if(depth == 0) return testEval(NULL);
	int max = -25500, score = 0;
	int numMoves = testGenLegalMoves();
	printf(CYN "numMoves: %d\n" reset, numMoves);

	int cpy[255][3];
	memcpy(cpy, legalMoves, numMoves * sizeof(cpy[0]));

	//printArr(legalMoves);
	printf(RED "------\n" reset);
	//printArr(cpy);

	int i = 0;
	while(cpy[i][2] != 0){
		// makeMove(bs, legalMoves[i][0], legalMoves[i][1], legalMoves[i][2]);
		score = -1 * miniMax(depth - 1);
		// undoMove();
		if(score > max) max = score;
		i++;
	}
	return max;
}

int genLeg(int moves[][4]){
	for(int i = 0 ; i < 10 ; i++){
		moves[i][2] = -1;
	}
}

int main(int argc, char *argv[]){
	initRand();
	// arg stuff
	char inputFEN[99];
	int res = parseArgs(inputFEN, argc, argv);

	int moves[255][4];
	genLeg(moves);

	for(int i = 0 ; i < 30 ; i++){
		printf("%d ", moves[i][2]);
	}

	return 0;
}
