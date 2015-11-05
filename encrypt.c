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

void bignum_print(bignum *bn, char *label) {
  int i;
  printf("%s0x", label);
  for (i = 0; i < bn->length; i++) {
					printf("%02x", bn->num[i]);
	}
	printf("\n");
}


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
  int offset = out->length - in1->length - in2->length + 1;

  assert(out->length >= in1->length + in2->length);
  assert(out != in1);
  assert(out != in2);

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

void bignum_copy(bignum *dst, bignum *src) {
  // TODO: this might be wrong if length is measured in halfwords, not bytes
  memcpy(&dst->num[dst->length - src->length], src->num, src->length);
  memset(dst->num, 0, dst->length - src->length);
}

int maybe_subtract(bignum *out, bignum *n, bignum *temp,
    int byteshift, int bitshift) {
  int n_index;
  int out_index;
  halfword borrow = 0;
  halfword effective_nbyte;
  halfwordsigned result = 0;

  assert(out->length >= n->length + byteshift);
  assert(temp->length == out->length);

  bignum_copy(temp, out);

  for (out_index = out->length-1-byteshift; out_index >= 0; out_index--) {
    n_index = out_index - (out->length - n->length) + byteshift;
    if (n_index >= 0) {
      effective_nbyte = n->num[n_index] << bitshift;
      if (n_index < n->length-1) {
        effective_nbyte |= n->num[n_index+1] >> (8-bitshift);
      }
    } else {
      effective_nbyte = 0;
    }

    result = out->num[out_index] - effective_nbyte - borrow;
    if (result < 0) {
      result += 1<<(8*sizeof(halfword));
      borrow = 1;
    } else {
      borrow = 0;
    }
    out->num[out_index] = result;
  }

  // If the result is negative, we need to undo the subtraction
  if (borrow == 1) {
    // TODO: Might be possible to swap pointers instead of copying
    bignum_copy(out, temp);
  }

  return !borrow;
}

void find_topbit(bignum *bn, int *topbyte, int *topbit) {
  halfword temp;

  for (*topbyte = 0; *topbyte < bn->length; (*topbyte)++) {
    if (bn->num[*topbyte] != 0) {
      break;
    }
  }

  temp = bn->num[*topbyte];
  for (*topbit = 7; *topbit >= 0; (*topbit)--) {
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
 *  temp: bignum of the same length as out
 */
void bignum_mod(bignum *out, bignum *t, bignum *n, bignum *temp) {
  // TODO: byteshift should probably become haflwordshift, etc., but we'll need
  // to be able to read an arbitrary halfword out of a bignum - or can we do
  // that already?
  int byteshift;
  int bitshift;
  int out_topbyte;
  int out_topbit;
  int n_topbyte;
  int n_topbit;

  assert(t->length <= n->length * 2);
  assert(out->length >= n->length *2);
  assert(temp->length == out->length);

  bignum_copy(out, t);

  find_topbit(out, &out_topbyte, &out_topbit);
  find_topbit(n, &n_topbyte, &n_topbit);
  byteshift = out->length - n->length - out_topbyte + n_topbyte;
  bitshift = out_topbit - n_topbit;
  if (bitshift < 0) {
    bitshift += 8;
    byteshift--;
  }

  while (byteshift >= 0) {
    maybe_subtract(out, n, temp, byteshift, bitshift);
    bitshift--;
    if (bitshift < 0) {
      bitshift = 7;
      byteshift--;
    }
  }
}

void bignum_modexp(bignum *out, bignum *M, bignum *e, bignum *n, bignum *temp1, bignum *temp2) {
  memset(out->num, 0, out->length);
  out->num[out->length - 1] = 1;

  int e_byte;
  int e_bit;
  find_topbit(e, &e_byte, &e_bit);

  while (e_byte >= 0) {
    bignum_multiply(temp1, out, out);
    bignum_mod(out, temp1, n, temp2);
    if (e->num[e_byte] & 1<<e_bit) {
      bignum_multiply(temp1, out, M);
      bignum_mod(out, temp1, n, temp2);
    }
    e_bit--;
    if (e_bit < 0) {
      e_bit = 7;
      e_byte--;
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
	unsigned char a_num[] = {0x12, 0x34};
	unsigned char e_num[] = {0x03};
  unsigned char n_num[] = {0x05, 0xea};
	unsigned char out_num[2*sizeof(n_num)];
  unsigned char temp1_num[sizeof(out_num)];
  unsigned char temp2_num[sizeof(out_num)];
  bignum a = {a_num, sizeof(a_num)};
  bignum e = {e_num, sizeof(e_num)};
  bignum n = {n_num, sizeof(n_num)};
  bignum out = {out_num, sizeof(out_num)};
  bignum temp1 = {temp1_num, sizeof(temp1_num)};
  bignum temp2 = {temp2_num, sizeof(temp2_num)};

  bignum_modexp(&out, &a, &e, &n, &temp1, &temp2);
	bignum_print(&out, "");
}
