#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "colors.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "Q1b1k1r1/2p2p1p/p3q3/1p2p1p1/2P5/b1NB1N2/PP1B1PPP/R3K2R w KQ - 2 15"
#define FEN3 "1r1qkb1r/1bp2pp1/p2p1n1p/3Np3/2pPP3/5N2/PPPQ1PPP/R1B2RK1 w k - 0 13"

void resetBoard(int *board);
void printBoard(int *board);
int sb(int sq64);
int legalMoves(int *moves, int *board, int piece, int sq);
void testMoves(int *moves, int *board, int piece, int sq);

int sb(int sq64){
	return sq64 + 21 + 2 * (sq64 - sq64 % 8) / 8;
}

int parseFEN(char *fen, int *board){
	// ranks start canonically at 8
	// but files are letters so who cares
	int i, num, piece, rank = 8, file = 0, sq64;
	while(*fen && rank > 0){
		num = 1;
		switch(*fen){
			case 'p': piece = bP; break;
			case 'r': piece = bR; break;
			case 'n': piece = bN; break;
			case 'b': piece = bB; break;
			case 'q': piece = bQ; break;
			case 'k': piece = bK; break;
			case 'P': piece = wP; break;
			case 'R': piece = wR; break;
			case 'N': piece = wN; break;
			case 'B': piece = wB; break;
			case 'Q': piece = wQ; break;
			case 'K': piece = wK; break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = EMPTY;
				num = *fen - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = 0;
				fen++;
				continue;

			default:
				printf(RED "Error with FEN\n" reset);
					return -1;

		}

		for(i = 0 ; i < num ; i++){
			sq64 = (8 - rank) * 8 + file;
			board[sb(sq64)] = piece;
			file++;
		}
		fen++;
	}
	return 0;
}

int legalMoves(int *moves, int *board, int piece, int sq){
	// plan here is to use the 120 sq board
	// and move in particular "directions"
	// until the piece is OFFBOARD
	// cs = candidate square
	int i = 0, cs = sq;
	for(i = 0 ; i < 27 ; i++){
		moves[i] = -1;
	}
	moves[i] = -1;
	i = 0;
	// TODO: is there a better way to check for invalid index?
	if(sq < 0 || sq > 120 || board[sq] == OFFBOARD){
		printf(RED "ERROR: invalid square\n" reset);
		return -1;
	}
	// pawns
	if(piece == wP){
		moves[i] = sq - 10;
		i++;
		if(sq > 80 && sq < 89){
			moves[i] = sq - 20;
			i++;
		}
		moves[i] = -1;
		return 0;
	}
	if(piece == bP){
		moves[i] = sq + 10;
		i++;
		if(sq > 30 && sq < 39){
			moves[i] = sq + 20;
			i++;
		}
		moves[i] = -1;
		return 0;
	}

	// rooks
	if(piece == wR || piece == bR){
		// up
		cs = sq;
		while(board[cs -= 10] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// left
		cs = sq;
		while(board[cs -= 1] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// down
		cs = sq;
		while(board[cs += 10] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// right
		cs = sq;
		while(board[cs += 1] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		return 0;
	}

	// bishops
	if(piece == wB || piece == bB){
		// nw
		cs = sq;
		while(board[cs -= 11] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// sw
		cs = sq;
		while(board[cs += 9] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// se
		cs = sq;
		while(board[cs += 11] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// ne
		cs = sq;
		while(board[cs -= 9] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		return 0;
	}

	// queens
	if(piece == wQ || piece == bQ){
		// up
		cs = sq;
		while(board[cs -= 10] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// left
		cs = sq;
		while(board[cs -= 1] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// down
		cs = sq;
		while(board[cs += 10] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// right
		cs = sq;
		while(board[cs += 1] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// nw
		cs = sq;
		while(board[cs -= 11] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// sw
		cs = sq;
		while(board[cs += 9] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// se
		cs = sq;
		while(board[cs += 11] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		// ne
		cs = sq;
		while(board[cs -= 9] != OFFBOARD){
			moves[i] = cs;
			i++;
		}
		return 0;
	}

	// kings
	if(piece == wK || piece == bK){
		// up
		cs = sq - 10; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// left
		cs = sq - 1; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// down
		cs = sq + 10; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// right
		cs = sq + 1; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// nw
		cs = sq - 11; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// sw
		cs = sq + 9; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// se
		cs = sq + 11; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// ne
		cs = sq - 9; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		return 0;
	}

	// knights
	if(piece == wN || piece == bN){
		// 1
		cs = sq - 21; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 2
		cs = sq - 12; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 3
		cs = sq + 8; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 4
		cs = sq + 19; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 5
		cs = sq + 21; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 6
		cs = sq + 12; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 7
		cs = sq - 8; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		// 8
		cs = sq - 19; if(board[cs] != OFFBOARD){ moves[i] = cs; i++; }
		return 0;
	}
}

void resetBoard(int *board){
	int i;
	for(i = 0 ; i < 120 ; i++){
		board[i] = OFFBOARD;
	}
	for(i = 0 ; i < 64 ; i++){
		board[sb(i)] = EMPTY;
	}
}

void printBoard(int *board){
	int i, rank, file, piece, sq64;

  printf(BLU "\n120 Board:\n\n" reset);

	for(i = 0 ; i < 120 ; i++){
		printf("%2d ", board[i]);
		if((i + 1) % 10 == 0){
			printf("\n");
		}
	}
	printf("\n");

  printf(YEL "\nGame Board:\n\n" reset);

	for(rank = 8 ; rank > 0 ; rank--){
		printf("%d ", rank);
		for(file = 0 ; file < 8 ; file++){
			sq64 = (8 - rank) * 8 + file;
			piece = board[sb(sq64)];
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}
	// print the files
	printf("\n  ");
	for(file = 0 ; file < 8 ; file++){
		printf(RED "%2c" reset, file + 'a');
	}
	printf("\n");
}

void testMoves(int *moves, int *board, int piece, int sq){
	int i;
	resetBoard(board);
	board[sq] = piece;
	legalMoves(moves, board, piece, sq);
	printf(YEL " == piece: %c == \n" reset, pieceChar[piece]);
	for(i = 0 ; moves[i] != -1 ; i++){
		printf("move %d: %d\n", i, moves[i]);
		board[moves[i]] = CANDIDATESQ;
	}
	printBoard(board);
}

int main(){
	int board[120];
	int moves[27];
	resetBoard(board);
	parseFEN(FEN2, board);
	printBoard(board);

	// testMoves(moves, board, wP, 83);
	// testMoves(moves, board, wP, 183);
	// testMoves(moves, board, wP, 113);
	// testMoves(moves, board, bP, 83);
	// testMoves(moves, board, bP, 33);
	// testMoves(moves, board, wR, 33);
	// testMoves(moves, board, wK, 33);
	// testMoves(moves, board, wB, 33);
	testMoves(moves, board, wQ, 33);
	// testMoves(moves, board, wK, 33);
}
