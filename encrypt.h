#ifndef _RSA_ENCRYPT_H
#define _RSA_ENCRYPT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Set up sizes appropriately for architecture
typedef uint16_t word;    // size of a multiplication output
typedef uint8_t halfword; // size of a multiplication input
typedef int16_t halfwordsigned;  // a halfword plus sign bit

typedef struct bignum {
	halfword *num;
	int length;
	int offset;
  unsigned char sign; // 0 if positive, 1 if negative
} bignum;

void bignum_modexp(bignum *out, const bignum *M, const bignum *e,
    const bignum *n, bignum *temp1, bignum *temp2);
void bignum_modexp_crt(bignum *out, const bignum *M, const bignum *p,
    const bignum *q, const bignum *dmp1, const bignum *dmq1,
    const bignum *iqmp, bignum *temp1, bignum *temp2, bignum *temp3);
void bignum_truncate(bignum *bn);
void bignum_set(bignum *bn, int index, halfword val);
int bignum_size(const bignum *bn);
halfword bignum_index(const bignum *bn, int index);
void bignum_copy(bignum *dst, const bignum *src);
void bignum_subtract_mod(bignum *out, const bignum *a, const bignum *b,
    const bignum *n, bignum *temp);

#ifdef WITH_PRINTF
void bignum_print(const bignum *bn, char *label);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
