#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "key.c"

// Set up sizes appropriately for architecture
typedef uint16_t word;    // size of a multiplication output
typedef uint8_t halfword; // size of a multiplication input

typedef struct bignum {
				halfword *num;
				unsigned int length;
} bignum;

void multiply(bignum *out, const bignum *in1, const bignum *in2) {
  int i;
  int j;
  halfword C;
  word intermediate;
  int offset = out->length - in1->length - in2->length + 1;

  memset(out->num, 0, out->length);
  for (i = in2->length-1; i >= 0; i--) {
    C = 0;
    for (j = in1->length-1; j >= 0; j--) {
      intermediate = out->num[i+j+offset] +
        in1->num[j]*in2->num[i] + C;
      C = intermediate >> 8*sizeof(*out->num);
      out->num[i+j+offset] = intermediate;
    }
    out->num[i+offset-1] = C;
  }
}

/*
void mod(halfword *out, const int out_length,
        const halfword *t, const int t_length,
        const halfword *n, const int n_length) {
    
}

void rsa_perform(const unsigned char *e, const int e_length,
								const unsigned char *n, const int n_length,
								const unsigned char *in, const int in_length,
								unsigned char *out, const int out_length) {
				unsigned char *result = malloc(n_length);
				unsigned char *temp = malloc(n_length);
				multiply(result, n_length, in, in_length, in, in_length);
}
*/

int main(int argc, char *argv[]) {
	unsigned char a_num[] = {0x12, 0x34, 0xab, 0xff};
	unsigned char b_num[] = {0x01, 0x03, 0x72, 0xea};
	unsigned char out_num[sizeof(a_num)+sizeof(b_num)];
  bignum a = {a_num, sizeof(a_num)};
  bignum b = {b_num, sizeof(b_num)};
  bignum out = {out_num, sizeof(out_num)};
	int i;

	multiply(&out, &a, &b);
	for (i = 0; i < out.length; i++) {
					printf("%02x", out.num[i]);
	}
	printf("\n");
}
