#include <stdio.h>

int main() {
	unsigned char x;
	int y;

	y = 0xabcd;
	x = y;
	printf("%x %x\n", x, y);
}
