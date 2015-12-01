DEFS = -D WITH_PRINTF
DEBUG = 

.PHONY: all clean
all: keygen encrypt check

keygen: keygen.c
	gcc -o keygen keygen.c -lcrypto

check: check.o key.o
	gcc -o $@ $(DEBUG) $^ -lcrypto

encrypt: cli_main.o encrypt.o key.o
	gcc -o $@ $(DEBUG) $^

%.o: %.c
	gcc -c $< -o $@ $(DEBUG) $(DEFS)

test: test.c
	gcc -o test test.c

clean:
	rm -f keygen check encrypt test *.o
