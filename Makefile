.PHONY: all
all: keygen encrypt check

keygen: keygen.c
	gcc -o keygen keygen.c -lcrypto

check: check.c key.c
	gcc -g -o check check.c -lcrypto

encrypt: encrypt.c key.c
	gcc -g -o encrypt encrypt.c

test: test.c
	gcc -o test test.c
