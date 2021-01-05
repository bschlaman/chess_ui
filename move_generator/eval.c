#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "colors.h"

int mobility(BOARD_STATE *bs);

int eval(BOARD_STATE *bs){
	// printf(CYN "vvv " reset);
	int materialEval = 0;
	int mobilityEval = 0;

	int kingWeight = 200;
	int queenWeight = 9;
	int rookWeight = 5;
	int bishopWeight = 3;
	int knightWeight = 3;
	int pawnWeight = 1;

	// float mobilityWeight = 0.1;
	int mobilityWeight = 1;
	int numLegalMoves = 1;
	numLegalMoves = mobility(bs);

	int *board = bs -> board;
	int side = bs -> side;
	int kings = 0;
	int queens = 0;
	int rooks = 0;
	int bishops = 0;
	int knights = 0;
	int pawns = 0;
	int piece;
	int factor;
	for(int i = 0 ; i < 64 ; i++){
		piece = board[sq64to120(i)];
		factor = 1 - 2 * (side ^ getColor(piece));
		if(piece == wK || piece == bK) kings += factor;
		if(piece == wQ || piece == bQ) queens += factor;
		if(piece == wR || piece == bR) rooks += factor;
		if(piece == wB || piece == bB) bishops += factor;
		if(piece == wN || piece == bN) knights += factor;
		if(piece == wP || piece == bP) pawns += factor;
	}
	materialEval += kings * kingWeight;
	materialEval += queens * queenWeight;
	materialEval += rooks * rookWeight;
	materialEval += bishops * bishopWeight;
	materialEval += knights * knightWeight;
	materialEval += pawns * pawnWeight;
	mobilityEval = numLegalMoves * mobilityWeight;
	// avoid checkmate
	// TODO: make this better obviously
	// if(bs -> side == BLACK){
	// 	printf(RED "%d\n" reset, numLegalMoves);
	// 	printf(RED "%d\n" reset, materialEval);
	// } else {
	// 	printf(YEL "%d\n" reset, numLegalMoves);
	// 	printf(YEL "%d\n" reset, materialEval);
	// }
	if(numLegalMoves == 0){
		return -100000000;
	}
	return materialEval;
}

// TODO: this is approximate
// doesn't count castling, en passant capture
int mobility(BOARD_STATE *bs){
	int i, total = 0, sq, cs, cs2, piece;
	int d, type;
	int *board = bs -> board;
	int side = bs -> side;
	int cperm = bs -> castlePermission;
	
	for(i = 0 ; i < 64 ; i++){
		sq = sq64to120(i);
		piece = board[sq64to120(i)];	
		if(piece != EMPTY && getColor(piece) == side){
			// pieces
			if(!isPawn[piece]){
				type = getType(piece);
				for(d = 0 ; d < numDirections[type] ; d++){
					cs = sq;
					while(board[cs += translation[type][d]] != OFFBOARD){
						if(board[cs] == EMPTY){
							if(!newBoardCheck(board, sq, cs)) total++;
						} else {
							if(side != getColor(board[cs])){
								if(!newBoardCheck(board, sq, cs)) total++;
							}
							break;
						}
						if(type == 0 || type == 4){ break; }
					}
				}
			} else {
				// pawns

				// forward 1
				// mapping {0,1} -> {-1,1} -> {-10,10}
				cs = sq - (1 - 2 * getColor(piece)) * 10;
				if(board[cs] == EMPTY && !newBoardCheck(board, sq, cs)){ total++; }
				// forward 2
				if(sq - 80 + 50 * getColor(piece) > 0 && sq - 80 + 50 * getColor(piece) < 9){
					cs = sq - (1 - 2 * getColor(piece)) * 20;
					cs2 = sq - (1 - 2 * getColor(piece)) * 10;
					if(board[cs] == EMPTY && board[cs2] == EMPTY && !newBoardCheck(board, sq, cs)){
						total++;
					}
				}

				// captures
				cs = sq - (1 - 2 * getColor(piece)) * 10 + 1;
				if(board[cs] != EMPTY && board[cs] != OFFBOARD \
					&& getColor(piece) != getColor(board[cs]) \
					&& !newBoardCheck(board, sq, cs)){
					total++;
				}
				cs = sq - (1 - 2 * getColor(piece)) * 10 - 1;
				if(board[cs] != EMPTY && board[cs] != OFFBOARD \
					&& getColor(piece) != getColor(board[cs]) \
					&& !newBoardCheck(board, sq, cs)){
					total++;
				}
			}
		}
	}
	return total;
}

int randInt(int lb, int ub){
	return rand() % (ub - lb + 1) + lb;
}

int negaMax(BOARD_STATE *bs, int depth){
	if(depth == 0) return eval(bs);
	int max = -100000, score;

	genLegalMoves(bs);
	int cpy[218][4];
	for(int m = 0 ; m < 218 ; m++){
		cpy[m][2] = -1;
	}
	// memcpy(cpy, legalMoves, numMoves * sizeof(cpy[0]));
	memcpy(cpy, legalMoves, 218 * sizeof(cpy[0]));

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

int treeSearch(BOARD_STATE *bs, int depth){
	if(depth == 0){ return eval(bs); }
	int m, from, to, moveType;
	int posEval, b = 0;
	genLegalMoves(bs);
	int localLM[218][4];
	memcpy(localLM, legalMoves, 218 * sizeof(localLM[0]));

	// // debug
	// if(depth == 2){
	//  	printf(BLU "side to move: " reset "%s\n", bs -> side == WHITE ? "white" : "black");
	// 	printLegalMoves(bs);
	// 	// for(m = 0 ; m < 100 ; m++){
	// 	// // for(m = 0 ; localLM[m][2] != -1 ; m++){
	// 	// 	printf(CYN "%d\n" reset, localLM[m][2]);
	// 	// }
	// 	// exit(0);
	// }
	// if(depth == 1 && bs -> board[72] == wQ){
	//  	printf(BLU "side to move: " reset "%s\n", bs -> side == WHITE ? "white" : "black");
	// 	printLegalMoves(bs);
	// }

	int bestScore = -100000;
	for(m = 0 ; localLM[m][2] != -1 ; m++){
		// debug
		// if(depth == 2 && m > 28) return 1;
		// if(depth == 2 && m > 28) return 1;
		from = localLM[m][0];
		to = localLM[m][1];
		moveType = localLM[m][2];

		// printf("response: ");
		// printMove(m, legalMoves[m][0], legalMoves[m][1], legalMoves[m][2]);
		makeMove(bs, from, to, moveType);
		posEval = -1 * treeSearch(bs, depth - 1);
		undoMove(bs);

		if(posEval > bestScore){
			bestScore = posEval;
			b = m;
			// if(depth == 2){
			// 	printf(RED "move: %d score: %d\n" reset, m, bestScore);
			// }
		}
	}
	// if(depth == 2){
	// 	printf("------------\n");
	// 	printMove(b, localLM[b][0], localLM[b][1], localLM[b][2]);
	// 	printf("------------\n");
	// 	exit(0);
	// }
	return bestScore;
}






















