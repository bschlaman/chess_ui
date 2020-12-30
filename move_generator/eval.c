#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "colors.h"

int eval(BOARD_STATE *bs){
	int materialEval = 0;
	int mobilityEval = 0;

	int kingWeight = 200;
	int queenWeight = 9;
	int rookWeight = 5;
	int bishopWeight = 3;
	int knightWeight = 3;
	int pawnWeight = 1;

	float mobilityWeight = 0.1;

	int *board = bs -> board;
	int kings = 0;
	int queens = 0;
	int rooks = 0;
	int bishops = 0;
	int knights = 0;
	int pawns = 0;
	int piece;
	for(int i = 0 ; i < 64 ; i++){
		piece = board[sq64to120(i)];
		if(piece == wK) kings++;
		if(piece == wQ) queens++;
		if(piece == wR) rooks++;
		if(piece == wB) bishops++;
		if(piece == wN) knights++;
		if(piece == wP) pawns++;
		if(piece == bK) kings--;
		if(piece == bQ) queens--;
		if(piece == bR) rooks--;
		if(piece == bB) bishops--;
		if(piece == bN) knights--;
		if(piece == bP) pawns--;
	}
	materialEval += kings * kingWeight;
	materialEval += queens * queenWeight;
	materialEval += rooks * rookWeight;
	materialEval += bishops * bishopWeight;
	materialEval += knights * knightWeight;
	materialEval += pawns * pawnWeight;
	return materialEval;
}

int randInt(int lb, int ub){
	return rand() % (ub - lb + 1) + lb;
}

int negaMax(BOARD_STATE *bs, int depth){
	if(depth == 0) return eval(bs);
	int max = -100000, score;

	genLegalMoves(bs);
	int cpy[1000][3];
	for(int m = 0 ; m < 1000 ; m++){
		cpy[m][2] = -1;
	}
	// memcpy(cpy, legalMoves, numMoves * sizeof(cpy[0]));
	memcpy(cpy, legalMoves, 1000 * sizeof(cpy[0]));

	int i = 0;
	while(cpy[i][2] != -1){
		makeMove(bs, legalMoves[i][0], legalMoves[i][1], legalMoves[i][2]);
		score = -1 * negaMax(bs, depth - 1);
		undoMove(bs);
		if(score > max) max = score;
		i++;
	}
	return max;
}

// int treeSearch(BOARD_STATE *bs, int depth){
// 	int posEval;
// 	genLegalMoves();
// 	int bestScore = -100000;
// 	while(m = getNextMove()){
// 		makeMove(m);
// 		posEval = eval(m);
// 		undoMove(m);
// 		if(posEval > bestScore) bestScore = posEval;
// 	}
// 	return bestScore;
// }






















