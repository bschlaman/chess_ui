all:
	cc main.c fen.c data.c init.c moves.c eval.c -o out
	./out
fen:
	cc main.c fen.c data.c init.c moves.c eval.c -o out
	./out -f "${FEN}"
rand:
	cc main.c fen.c data.c init.c moves.c eval.c -o out
	./out -r
search:
	cc main.c fen.c data.c init.c moves.c eval.c -o out
	./out -s
only:
	cc main.c fen.c data.c init.c moves.c eval.c -o out
test:
	cc test.c data.c -o out_test
	./out_test
