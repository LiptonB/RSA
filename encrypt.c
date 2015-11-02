#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "key.c"

// Set up sizes appropriately for architecture
typedef uint16_t word;    // size of a multiplication output
typedef uint8_t halfword; // size of a multiplication input
typedef int16_t halfwordsigned;  // a halfword plus sign bit

typedef struct bignum {
				halfword *num;
				unsigned int length;
} bignum;

/* multiply: multiplies in1 and in2, returns result in out.
 *  in1: bignum of length k1
 *  in2: bignum of length k2
 *  out: bignum of at least k1+k2 bytes
 */
void bignum_multiply(bignum *out, const bignum *in1, const bignum *in2) {
  int i;
  int j;
  halfword C;
  word intermediate;
  assert(out->length >= in1->length + in2->length);
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

bool maybe_subtract(bignum *out, bignum *n, int byteshift, int bitshift) {
  int index;
  halfword combined;
  halfwordsigned result;

  // TODO: it's not going to work to go from MSB to LSB, because of borrowing
  for (index = 0; index < n->length; index++) {
    combined = n->num[index] << bitshift | n->num[+1] >> (8-bitshift);
    result = out->num[index] - combined;
    if (result < 0) {

    }
  }
}

void find_topbit(bignum *num, int *topbyte, int *topbit) {
  halfword temp;

  for (*topbyte = 0; *topbyte < num->length; *topbyte++) {
    if (out->num[*topbyte] != 0) {
      break;
    }
  }

  temp = out->num[*topbyte];
  for (*topbit = 7; *topbit >= 0; *topbit--) {
    if (temp & (1<<*topbit)) {
      break;
    }
  }
}

/* mod: divides t by n and returns result in out.
 *  t: bignum of maximum length 2k
 *  n: bignum of length k
 *  out: bignum of at least length 2k (will occupy no more than k bytes on
 *    completion)
 */
void bignum_mod(bignum *out, bignum *t, bignum *n) {
  // TODO: byteshift should probably become haflwordshift, etc., but we'll need
  // to be able to read an arbitrary halfword out of a bignum
  int byteshift;
  int bitshift;
  int out_topbyte;
  int out_topbit;
  int n_topbyte;
  int n_topbit;
  bignum_copy(out, t);

  find_topbit(out, &out_topbyte, &out_topbit);
  find_topbit(n, &n_topbyte, &n_topbit);
  byteshift = out->length - n->length - out_topbyte + n_topbyte;
  bitshift = out_topbit - n_topbit;
  if (bitshift < 0) {
    bitshift += 8;
    byteshift--;
  }

  while(byteshift > 0 && bitshift > 0) {
    maybe_subtract(out, n, byteshift, bitshift);
    bitshift--;
    if (bitshift == 0) {
      bitshift = 7;
      byteshift--;
    }
  }
}

/*
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
