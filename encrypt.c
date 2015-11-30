#include <string.h>
#include <stdlib.h>
#include "encrypt.h"

#define PROFILING 0
#define MAXPROF 15
#include "profiling.h"

#ifdef WITH_PRINTF
#include <assert.h>
#include <stdio.h>
#define RSA_ASSERT(a) assert(a)
#else
#define RSA_ASSERT(a)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void bignum_set(bignum *bn, int index, halfword val) {
  RSA_ASSERT(bn->offset + index < bn->length);
  bn->num[index+bn->offset] = val;
}

halfword bignum_index(const bignum *bn, int index) {
  RSA_ASSERT(bn->offset + index < bn->length);
  return bn->num[bn->offset + index];
}

int bignum_size(const bignum *bn) {
  return bn->length - bn->offset;
}

void bignum_setsize(bignum *bn, int size) {
  RSA_ASSERT(size <= bn->length);
  bn->offset = bn->length - size;
}

void bignum_zero(bignum *bn) {
  memset(bn->num, 0, bn->length);
}

void find_topbit(const bignum *bn, int *topbyte, int *topbit) {
  halfword temp;
  int size = bignum_size(bn);

  for (*topbyte = 0; *topbyte < size; (*topbyte)++) {
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

#ifdef WITH_PRINTF
void bignum_print(bignum *bn, char *label) {
  int i;
  int size = bignum_size(bn);

  printf("%s0x", label);

  for (i = 0; i < size; i++) {
					printf("%02x", bignum_index(bn, i));
	}
	printf("\n");
}
#endif

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

  PF(4);
  RSA_ASSERT(out != in1);
  RSA_ASSERT(out != in2);

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

void bignum_copy(bignum *dst, const bignum *src) {
  // TODO: this might be wrong if length is measured in halfwords, not bytes
  int src_size = bignum_size(src);
  RSA_ASSERT(dst->length >= src_size);

  memcpy(&dst->num[dst->length - src_size], &src->num[src->offset], src_size);
  memset(dst->num, 0, dst->length - src_size);
  dst->offset = dst->length - src_size;
}

int maybe_subtract(bignum *out, const bignum *n, bignum *temp,
    int byteshift, int bitshift) {
  int n_index;
  int out_index;
  halfword borrow = 0;
  halfword effective_nbyte;
  halfword lookup_byte;
  halfwordsigned result = 0;

  PF(3);
  int out_size = bignum_size(out);
  int n_size = bignum_size(n);

  PF(5);
  RSA_ASSERT(out_size >= n_size + byteshift);

  bignum_copy(temp, out);

  PF(3);

  for (out_index = out_size-1-byteshift; out_index >= 0; out_index--) {
    PF(7);
    n_index = out_index - (out_size - n_size) + byteshift;

    PF(8);
    effective_nbyte = 0;
    if (n_index >= -1 && n_index < n_size-1) {
      effective_nbyte |= lookup_byte >> (8-bitshift);
    }
    if (n_index >= 0) {
      lookup_byte = bignum_index(n, n_index);
      effective_nbyte |= lookup_byte << bitshift;
    }

    PF(9);
    result = bignum_index(out, out_index) - effective_nbyte - borrow;
    PF(10);
    if (result < 0) {
      result += 1<<(8*sizeof(halfword));
      borrow = 1;
    } else {
      borrow = 0;
    }
    PF(11);
    bignum_set(out, out_index, result);
  }

  // If the result is negative, we need to undo the subtraction
  if (borrow == 1) {
    PF(6);
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
void bignum_mod(bignum *out, const bignum *t, const bignum *n, bignum *temp) {
  // TODO: byteshift should probably become haflwordshift, etc., but we'll need
  // to be able to read an arbitrary halfword out of a bignum - or can we do
  // that already?
  int byteshift;
  int bitshift;
  int out_topbyte;
  int out_topbit;
  int n_topbyte;
  int n_topbit;

  PF(0);
  RSA_ASSERT(temp->length >= bignum_size(t));

  bignum_copy(out, t);

  find_topbit(out, &out_topbyte, &out_topbit);
  find_topbit(n, &n_topbyte, &n_topbit);
  byteshift = bignum_size(out) - bignum_size(n) - out_topbyte + n_topbyte;
  bitshift = out_topbit - n_topbit;
  if (bitshift < 0) {
    bitshift += 8;
    byteshift--;
  }

  PF(1);
  while (byteshift >= 0) {
    maybe_subtract(out, n, temp, byteshift, bitshift);
    PF(2);
    bitshift--;
    if (bitshift < 0) {
      bitshift = 7;
      byteshift--;
    }
  }

  bignum_truncate(out);
}

void bignum_modexp(bignum *out, const bignum *M, const bignum *e, const bignum *n, bignum *temp1, bignum *temp2) {
  int e_byte;
  int e_bit;
  int e_size = bignum_size(e);

  bignum_setsize(out, 1);
  bignum_set(out, 0, 1);

  find_topbit(e, &e_byte, &e_bit);

  while (e_byte < e_size) {
    bignum_multiply(temp1, out, out);
    //bignum_print(temp1, "1: ");
    bignum_mod(out, temp1, n, temp2);
    //bignum_print(out, "2: ");
    if (bignum_index(e, e_byte) & 1<<e_bit) {
      bignum_multiply(temp1, out, M);
      //bignum_print(temp1, "3: ");
      bignum_mod(out, temp1, n, temp2);
      //bignum_print(out, "4: ");
    }
    e_bit--;
    if (e_bit < 0) {
      e_bit = 7;
      e_byte++;
    }
  }
}

#ifdef __cplusplus
} // extern "C"
#endif
