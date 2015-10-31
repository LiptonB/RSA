.PHONY: all
all: keygen

keygen: keygen.c
	gcc -o keygen keygen.c -lcrypto

encrypt: encrypt.c key.c
	gcc -g -o encrypt encrypt.c

test: test.c
	gcc -o test test.c
