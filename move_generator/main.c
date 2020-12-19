#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "colors.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "Q1b1k1r1/2p2p1p/p3q3/1p2p1p1/2P5/b1NB1N2/PP1B1PPP/R3K2R w KQ - 2 15"
#define FEN3 "1r1qkb1r/1bp2pp1/p2p1n1p/3Np3/2pPP3/5N2/PPPQ1PPP/R1B2RK1 w k - 0 13"

void resetBoard(int *board);
void printBoard(int *board, int options);
int sb(int sq64);
int legalMoves(int *moves, int *board, int piece, int sq);
void testMoves(int *moves, int *board, int piece, int sq);
void saveMove(int from, int to, int capture);
int getType(int piece);
int getColor(int piece);

int sb(int sq64){
	return sq64 + 21 + 2 * (sq64 - sq64 % 8) / 8;
}

int getType(int piece){
	return (piece - 2) % 6;
}
int getColor(int piece){
	// black is 7 - 12
	return piece > 6 && piece < 13;
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

void saveMove(int from, int to, int capture){
	printf(YEL " == move ==\n" reset);
	printf("from: %d\n", from);
	printf("to: %d\n", to);
	printf("capture: %d\n", capture);
}

int legalMoves(int *moves, int *board, int piece, int sq){
	// plan here is to use the 120 sq board
	// and move in particular "directions"
	// until the piece is OFFBOARD
	// cs = candidate square
	int i = 0, cs = sq;

	int numDirections[] = {8, 4, 4, 8, 8};
	int translation[][8] = {
		{-21, -12, 8, 19, 21, 12, -8, -19}, // knights 0
		{-11, 9, 11, -9}, // bishops 1
		{-10, -1, 10, 1}, // rooks 2
		{-11, 9, 11, -9, -10, -1, 10, 1}, // queens 3
		{-11, 9, 11, -9, -10, -1, 10, 1}  // kings 4
	};

	// initialize moves array
	for(i = 0 ; i < 27 ; i++){
		moves[i] = -1;
	}
	i = 0;
	// TODO: is there a better way to check for invalid index?
	if(sq < 0 || sq > 120 || board[sq] == OFFBOARD){
		// TODO: get rid of this
		printf(RED "ERROR: invalid square\n" reset);
		return -1;
	}

	// a better move generation
	if(!isPawn[piece]){
		// type = proper index for translation[][]
		int d, type = getType(piece);
		printf(BLU "type: %d\n" reset, type);
		// for each direction
		for(d = 0 ; d < numDirections[type] ; d++){
			cs = sq;
			// while sq not offboard
			while(board[cs += translation[type][d]] != OFFBOARD){
				// printf(BLU "cs: %d\n" reset, cs);
				if(board[cs] == EMPTY){
					saveMove(sq, cs, 0);  moves[i] = cs; i++;
				} else {
					if(getColor(piece) != getColor(board[cs])){ saveMove(sq, cs, 1); moves[i] = cs; i++;}
					break;
				}
				// stop if piece is a knight or king
				if(type == 0 || type == 4){ break; }
			}
		}
		return 0;
	} else {
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
	}
	return -1;
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

void printBoard(int *board, int option){
	int i, rank, file, piece, sq64;

	if(option == 1){
		printf(BLU "\n120 Board:\n\n" reset);
		for(i = 0 ; i < 120 ; i++){
			printf("%2d ", board[i]);
			if((i + 1) % 10 == 0){
				printf("\n");
			}
		}
		printf("\n");
	}

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
	// create a dummy board to see candidate squares
	int tmpBoard[120];
	resetBoard(tmpBoard);
	tmpBoard[sq] = piece;

	legalMoves(moves, board, piece, sq);
	printf(YEL " == piece: %c == \n" reset, pieceChar[piece]);
	for(i = 0 ; moves[i] != -1 ; i++){
		// printf("move %d: %d\n", i, moves[i]);
		tmpBoard[moves[i]] = CANDIDATESQ;
	}

	printBoard(tmpBoard, 0);
}

int main(){
	int board[120];
	int moves[27];
	resetBoard(board);
	parseFEN(FEN2, board);
	printBoard(board, 1);

	// testMoves(moves, board, wP, 83);
	// testMoves(moves, board, bP, 83);
	// testMoves(moves, board, bP, 33);
	testMoves(moves, board, wQ, 65);
	// testMoves(moves, board, wN, 33);
	// testMoves(moves, board, wB, 33);
	// testMoves(moves, board, wQ, 33);
	// testMoves(moves, board, wK, 33);
}
