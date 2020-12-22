#include "defs.h"

const char pieceChar[] = ".PNBRQKpnbrqkx";
const char castleChar[] = "KQkq";
const int isPawn[] = {false, true, false, false, false, false, false, true, false, false, false, false, false, false};
const int isKing[] = {false, false, false, false, false, false, true, false, false, false, false, false, true, false};
const int OFFBOARD = -1;
