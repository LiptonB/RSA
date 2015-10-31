#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "key.c"

struct bignum {
				unsigned char *num;
				unsigned int length;
};

void multiply(unsigned char *out, const int out_length,
								const unsigned char *in1, const int in1_length,
								const unsigned char *in2, const int in2_length) {
				int i;
				int j;
				int C;
				int intermediate;
				int index;
				memset(out, 0, out_length);
				for (i = in2_length-1; i >= 0; i--) {
					C = 0;
					for (j = in1_length-1; j >= 0; j--) {
									intermediate = out[i+j] + in1[j]*in2[i] + C;
									C = intermediate >> sizeof(*out);
									out[i+j] = intermediate;
					}
					out[i+in1_length] = C;
				}
}

void rsa_perform(const unsigned char *e, const int e_length,
								const unsigned char *n, const int n_length,
								const unsigned char *in, const int in_length,
								unsigned char *out, const int out_length) {
				unsigned char *result = malloc(n_length);
				unsigned char *temp = malloc(n_length);
				multiply(result, n_length, in, in_length, in, in_length);
}

int main(int argc, char *argv[]) {
	unsigned char a[] = {0x12, 0x34};
	unsigned char b[] = {0x01, 0x03};
	unsigned char out[4];
	int i;

	multiply(out, sizeof(out), a, sizeof(a), b, sizeof(b));
	for (i = 0; i < sizeof(out); i++) {
					printf("%02x", out[i]);
	}
	printf("\n");
}
