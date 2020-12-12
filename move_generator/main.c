#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "colors.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "1r1qkb1r/1bp2pp1/p2p1n1p/3Np3/2pPP3/5N2/PPPQ1PPP/R1B2RK1 w k - 0 13"

void resetBoard(int *board);
void printBoard(int *board);
int sb(int sq64);

// void helper(){
//     int rank, file, sq;
//     int sq64 = 0;
//     for(rank = 0 ; rank <= 8 ; ++rank){
//         for(file = FILE_A ; file <= FILE_H ; ++file){
//             sq = FR2SQ(file,rank);
//             printf("%d", sq);
//             sq64++;
//         }
//     }
// }

int sb(int sq64){
	return sq64 + 21 + 2 * (sq64 - sq64 % 8) / 8;
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
	int i, rank, file, piece;

  printf(BLU "\n120 Board:\n\n" reset);
	for(i = 0 ; i < 64 ; i++){
		printf("%d -> %d\n", i, sb(i));
	}

	for(i = 0 ; i < 120 ; i++){
		printf("%2d ", board[i]);
		if((i + 1) % 10 == 0){
			printf("\n");
		}
	}
	printf("\n");
	printf("%d %d\n", sizeof(board), sizeof(board[0]));

  printf(YEL "\nGame Board:\n\n" reset);

	for(rank = 8 ; rank > 0 ; rank--){
		printf("%d ", rank);
		for(file = 0 ; file < 8 ; file++){
			piece = 4;
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

int main(){
	int board[120];
	resetBoard(board);
	printBoard(board);
}
