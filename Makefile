DEFS = -D WITH_PRINTF

.PHONY: all
all: keygen encrypt check

keygen: keygen.c
	gcc -o keygen keygen.c -lcrypto

check: check.c key.c
	gcc -o check check.c -lcrypto

encrypt: cli_main.o encrypt.o key.o
	gcc -o $@ $^

%.o: %.c
	gcc -c $< -o $@ $(DEFS)

test: test.c
	gcc -o test test.c
