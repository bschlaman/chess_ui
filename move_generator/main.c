#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "colors.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "Q1b1k1r1/2p2p1p/p3q3/1p2p1p1/2P5/b1NB1N2/PP1B1PPP/R3K2R w KQ - 2 15"
#define FEN3 "1r1qkb1r/1bp2pp1/p2p1n1p/3Np3/2pPP3/5N2/PPPQ1PPP/R1B2RK1 b k a6 0 13"
#define FEN4 "3k2Q1/7R/1p1p4/p1p2P2/2P1K3/1P3P2/P7/8 b - b8 12 51"

void resetBoard(BOARD_STATE *bs);
void printBoard(BOARD_STATE *bs, int options);
int sq64To120(int sq64);
int sq120To64(int sq120);
int frToSq(int file, int rank);
void sqName(char *sqfr, int sq120);
int pieceMoves(int *moves, int *board, int piece, int sq);
int genAllMoves(int *moves, BOARD_STATE *bs, int side);
void testPieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq);
void saveMove(int from, int to, int capture);
int getType(int piece);
int getColor(int piece);

int sq64To120(int sq64){
	return sq64 + 21 + 2 * (sq64 - sq64 % 8) / 8;
}

int sq120To64(int sq120){
	return sq120 - 17 - 2 * (sq120 - sq120 % 10) / 10;;
}

int frToSq(int file, int rank){
	return (8 - rank) * 8 + file;
}

void sqName(char *sqfr, int sq120){
	int sq64 = sq120To64(sq120);
	char sqstr[] = {(sq64 % 8) + 'a', '8' - ((sq64 - sq64 % 8) / 8), '\0'};
	strcpy(sqfr, sqstr);
}

int getType(int piece){
	return (piece - 2) % 6;
}

int getColor(int piece){
	// black is 7 - 12
	return piece > 6 && piece < 13;
}

int parseFEN(char *fen, BOARD_STATE *bs){
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
			bs -> board[sq64To120(frToSq(file, rank))] = piece;
			file++;
		}
		fen++;
	}

	// side
	printf(GRN "%s\n" reset, fen);
	bs -> side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2;

	// castling
	for(i = 0 ; i < 4 ; i++){
		if(*fen == ' ') break;
		switch(*fen){
			case 'K': bs -> castlePermission |= WKCA; break;
			case 'Q': bs -> castlePermission |= WQCA; break;
			case 'k': bs -> castlePermission |= BKCA; break;
			case 'q': bs -> castlePermission |= BQCA; break;
			default: break;
		}
		fen++;
	}
	fen++;
	ASSERT(bs -> castlePermission >= 0 && bs -> castlePermission <= 15);

	// en passant
	if(*fen != '-'){
		file = fen[0] - 'a';
		rank = fen[1] - '0';
		ASSERT(file >= 0 && file <= 8);
		ASSERT(rank >= 0 && rank <= 8);
		bs -> enPas = frToSq(file, rank);
	}

	return 0;
}

void saveMove(int from, int to, int capture){
	char sqfr[1];
	printf(YEL " == move: " reset);
	sqName(sqfr, from);
	printf("from: %s ", sqfr);
	sqName(sqfr, to);
	printf("to: %s ", sqfr);
	printf("capture: %s\n", capture ? "yes" : "no");
}

int genAllMoves(int *moves, BOARD_STATE *bs, int side){
	int i, piece, sq;
	for(i = 0 ; i < 64 ; i++){
		sq = sq64To120(i);
		piece = bs -> board[sq];
		if(piece != EMPTY && getColor(piece) == side){
			printf(GRN " == PIECE: %c ==\n" reset, pieceChar[piece]);
			pieceMoves(moves, bs, piece, sq);
		}
	}
}

int pieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq){
	// plan here is to use the 120 sq board
	// and move in particular "directions"
	// until the piece is OFFBOARD
	// cs = candidate square
	int i = 0, cs = sq, enPasCaptureFromSq = OFFBOARD;
	int *board = bs -> board;

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

		// forward 1
		// mapping {0,1} -> {-1,1} -> {-10,10}
		cs = sq - (1 - 2 * getColor(piece)) * 10;
		if(board[cs] == EMPTY){ saveMove(sq, cs, 0);  moves[i] = cs; i++; }
		// forward 2
		if(sq - 80 + 50 * getColor(piece) > 0 && sq - 80 + 50 * getColor(piece) < 9){
			cs = sq - (1 - 2 * getColor(piece)) * 20;
			if(board[cs] == EMPTY){
				// TODO: set enPas, although that would probably be in a different function
				saveMove(sq, cs, 0);  moves[i] = cs; i++;
			}
		}

		// captures
		cs = sq - (1 - 2 * getColor(piece)) * 10 + 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD){
			saveMove(sq, cs, 1);  moves[i] = cs; i++;
		}
		cs = sq - (1 - 2 * getColor(piece)) * 10 - 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD){
			saveMove(sq, cs, 1);  moves[i] = cs; i++;
		}
		// enPas
		if(bs -> enPas != EMPTY){
			
		}

	}
	return -1;
}

void resetBoard(BOARD_STATE *bs){
	// this can go on a diet
	int i;
	for(i = 0 ; i < 120 ; i++){
		bs -> board[i] = OFFBOARD;
	}
	for(i = 0 ; i < 64 ; i++){
		bs -> board[sq64To120(i)] = EMPTY;
	}
	bs -> side = BOTH;
	bs -> castlePermission = 0;
	// TODO: make sure these are always 120sq
	// because EMPTY = 0
	bs -> enPas = EMPTY;
}

void printBoard(BOARD_STATE *bs, int option){
	int i, rank, file, piece, sq64;

	if(option == 1){
		printf(BLU "\n120 Board:\n\n" reset);
		for(i = 0 ; i < 120 ; i++){
			printf("%2d ", bs -> board[i]);
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
			piece = bs -> board[sq64To120(frToSq(file, rank))];
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

	printf(BLU "side to move: %s\n" reset, bs -> side == WHITE ? "white" : "black");
	char sqfr[1];
	sqName(sqfr, sq64To120(bs -> enPas));
	printf(BLU "enPas namesquare: %s\n" reset, sqfr);
	printf(BLU "enPas 64square: %d\n" reset, bs -> enPas);
	printf(BLU "rank: %d\n" reset, rank);
	printf(BLU "file: %d\n" reset, file);


}

void testPieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq){
	int i;
	// create a dummy board to see candidate squares
	BOARD_STATE tmpBoard[1];
	resetBoard(tmpBoard);
	tmpBoard -> board[sq] = piece;

	pieceMoves(moves, tmpBoard -> board, piece, sq);
	printf(YEL " == piece: %c == \n" reset, pieceChar[piece]);
	for(i = 0 ; moves[i] != -1 ; i++){
		tmpBoard -> board[moves[i]] = CANDIDATESQ;
	}

	printBoard(tmpBoard, 0);
}

int main(){
	BOARD_STATE bs[1];
	int moves[27];
	resetBoard(bs);
	parseFEN(START_FEN, bs);
	printf("asdf\n");
	resetBoard(bs);
	parseFEN(FEN4, bs);
	printBoard(bs, 0);

	genAllMoves(moves, bs, WHITE);

}
