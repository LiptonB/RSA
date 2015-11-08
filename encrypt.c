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
        unsigned int offset;
} bignum;


void bignum_set(bignum *bn, int index, halfword val) {
  assert(bn->offset + index < bn->length);
  bn->num[index+bn->offset] = val;
}

halfword bignum_index(const bignum *bn, int index) {
  assert(bn->offset + index < bn->length);
  return bn->num[bn->offset + index];
}

int bignum_size(const bignum *bn) {
  return bn->length - bn->offset;
}

int bignum_setsize(bignum *bn, int size) {
  assert(size <= bn->length);
  bn->offset = bn->length - size;
}

void bignum_zero(bignum *bn) {
  memset(bn->num, 0, bn->length);
}

void find_topbit(const bignum *bn, int *topbyte, int *topbit) {
  halfword temp;

  for (*topbyte = 0; *topbyte < bignum_size(bn); (*topbyte)++) {
    if (bignum_index(bn, *topbyte) != 0) {
      break;
    }
  }

  temp = bignum_index(bn, *topbyte);
  for (*topbit = 7; *topbit >= 0; (*topbit)--) {
    if (temp & (1<<*topbit)) {
      break;
    }
  }
}

void bignum_truncate(bignum *bn) {
  int topbit;
  int topbyte;
  bn->offset = 0;
  // TODO: computing and passing topbit is unnecessary work
  find_topbit(bn, &topbyte, &topbit);
  bn->offset = topbyte;
}

void bignum_print(bignum *bn, char *label) {
  int i;
  printf("%s0x", label);
  for (i = 0; i < bignum_size(bn); i++) {
					printf("%02x", bignum_index(bn, i));
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
  //int offset = bignum_size(out) - bignum_size(in1) - bignum_size(in2) + 1;

  assert(out != in1);
  assert(out != in2);

  bignum_setsize(out, bignum_size(in1) + bignum_size(in2));
  bignum_zero(out);
  for (i = bignum_size(in2)-1; i >= 0; i--) {
    C = 0;
    for (j = bignum_size(in1)-1; j >= 0; j--) {
      intermediate = bignum_index(out, i+j+1) +
        bignum_index(in1, j)*bignum_index(in2, i) + C;
      C = intermediate >> 8*sizeof(halfword);
      bignum_set(out, i+j+1, intermediate);
    }
    bignum_set(out, i, C);
  }
}

void bignum_copy(bignum *dst, bignum *src) {
  // TODO: this might be wrong if length is measured in halfwords, not bytes
  int src_size = bignum_size(src);
  assert(dst->length >= src_size);

  memcpy(&dst->num[dst->length - src_size], &src->num[src->offset], src_size);
  memset(dst->num, 0, dst->length - src_size);
  bignum_truncate(dst);
}

int maybe_subtract(bignum *out, bignum *n, bignum *temp,
    int byteshift, int bitshift) {
  int n_index;
  int out_index;
  halfword borrow = 0;
  halfword effective_nbyte;
  halfwordsigned result = 0;

  assert(bignum_size(out) >= bignum_size(n) + byteshift);

  bignum_copy(temp, out);

  for (out_index = bignum_size(out)-1-byteshift; out_index >= 0; out_index--) {
    n_index = out_index - (bignum_size(out) - bignum_size(n)) + byteshift;
    effective_nbyte = 0;
    if (n_index >= 0) {
      effective_nbyte |= bignum_index(n, n_index) << bitshift;
    }
    if (n_index < bignum_size(n)-1) {
      effective_nbyte |= bignum_index(n, n_index+1) >> (8-bitshift);
    }

    result = bignum_index(out, out_index) - effective_nbyte - borrow;
    if (result < 0) {
      result += 1<<(8*sizeof(halfword));
      borrow = 1;
    } else {
      borrow = 0;
    }
    bignum_set(out, out_index, result);
  }

  // If the result is negative, we need to undo the subtraction
  if (borrow == 1) {
    // TODO: Might be possible to swap pointers instead of copying
    bignum_copy(out, temp);
  }

  return !borrow;
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

  assert(temp->length >= bignum_size(t));

  bignum_copy(out, t);

  find_topbit(out, &out_topbyte, &out_topbit);
  find_topbit(n, &n_topbyte, &n_topbit);
  byteshift = bignum_size(out) - bignum_size(n) - out_topbyte + n_topbyte;
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

  bignum_truncate(out);
}

void bignum_modexp(bignum *out, bignum *M, bignum *e, bignum *n, bignum *temp1, bignum *temp2) {
  int e_byte;
  int e_bit;

  bignum_setsize(out, 1);
  bignum_set(out, 0, 1);

  find_topbit(e, &e_byte, &e_bit);

  while (e_byte >= 0) {
    bignum_multiply(temp1, out, out);
    bignum_mod(out, temp1, n, temp2);
    if (bignum_index(e, e_byte) & 1<<e_bit) {
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

int main(int argc, char *argv[]) {
	unsigned char a_num[] = {0x12, 0x34};
	unsigned char e_num[] = {0x04};
  unsigned char n_num[] = {0x05, 0xea};
	unsigned char out_num[2*sizeof(n_num)];
  unsigned char temp1_num[sizeof(out_num)];
  unsigned char temp2_num[sizeof(out_num)];
  bignum a = {a_num, sizeof(a_num), 0};
  bignum e = {e_num, sizeof(e_num), 0};
  bignum n = {n_num, sizeof(n_num), 0};
  bignum out = {out_num, sizeof(out_num), 0};
  bignum temp1 = {temp1_num, sizeof(temp1_num), 0};
  bignum temp2 = {temp2_num, sizeof(temp2_num), 0};

  bignum_truncate(&out);
  bignum_truncate(&temp1);
  bignum_truncate(&temp2);

  bignum_modexp(&out, &a, &e, &n, &temp1, &temp2);
	bignum_print(&out, "");
}
