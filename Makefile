.PHONY: all
all: keygen

keygen: keygen.c
	gcc -o keygen keygen.c -lcrypto
