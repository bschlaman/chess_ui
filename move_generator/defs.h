#include <stdlib.h>

#define DEBUG
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)){ \
    printf(RED "====== ERROR\n" YEL "%s\n" reset, #n); \
    printf(RED "file: " reset "%s ", __FILE__); \
    printf(RED "line: " reset "%d\n", __LINE__); \
    exit(1); \
}
#endif

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef unsigned long long U64;

enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH };
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};

enum { false, true };

// 0 0 0 0
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

typedef struct {

    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;

} S_UNDO;

typedef struct {
		// rename to pieces?
    int board[120];
    int fiftyMove;

		// fen?

    int side;
    int enPas;
    int castlePermission;

    // Hash key, unique representation of board
    U64 posKey;

    // Number of pieces of type [x]

    S_UNDO history[50];

    // piece list
    int piecesList[13][10];
    // pList[wN][0] = E1;
    // pList[wN][1] = D4;

} BOARD_STATE;


char pieceChar[] = ".PNBRQKpnbrqkx";
// use EMPTY for no piece, and also when there is no enPas square
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, CANDIDATESQ };
int isPawn[] = {false, true, false, false, false, false, false, true, false, false, false, false, false, false};
int isKing[] = {false, false, false, false, false, false, true, false, false, false, false, false, true, false};
int OFFBOARD = -1;

// /* MACROS */
// #define FR2SQ(f,r) ( (21+(f)) + ((r)*10) )
// #define SQ64(sq120) Sq120ToSq64[sq120]
// #define SQ120(sq64) Sq64ToSq120[sq64]
// #define POP(b) PopBit(b)
// #define CNT(b) CountBits(b)
// #define CLRBIT(bb, sq) (bb &= clearMask[sq])
// #define SETBIT(bb, sq) (bb |= setMask[sq])
// /* GLOBALS */
// extern int Sq120ToSq64[BRD_SQ_NUM];
// extern int Sq64ToSq120[64];
// extern U64 setMask[64];
// extern U64 clearMask[64];
// extern U64 pieceKeys[13][120];
// extern U64 sideKey;
// extern U64 castleKeys[16];
// extern char pieceChar[];
// extern char sideChar[];
// extern char rankChar[];
// extern char fileChar[];
// // Ask is this piece maj, what's it's color?
// extern int pieceBig[13];
// extern int pieceMaj[13];
// extern int pieceMin[13];
// extern int pieceVal[13];
// extern int pieceCol[13];
// /* FUNCTIONS */
// // init.c
// extern void allInit();
// // bitboard.c
// extern void printBitBoard(U64 bb);
// extern int PopBit(U64 *bb);
// extern int CountBits(U64 b);
// // hashkey.c
// extern U64 generatePosKey(const S_BOARD *pos);
// // board.c
// extern void resetBoard(S_BOARD *pos);
// extern int parseFEN(char *fen, S_BOARD *pos);
// extern void printBoard(const S_BOARD *pos);
