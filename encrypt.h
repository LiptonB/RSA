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
	unsigned int length;
	unsigned int offset;
} bignum;

void bignum_modexp(bignum *out, const bignum *M, const bignum *e, const bignum *n, bignum *temp1, bignum *temp2);
void bignum_truncate(bignum *bn);
void bignum_set(bignum *bn, int index, halfword val);
int bignum_size(const bignum *bn);
halfword bignum_index(const bignum *bn, int index);
void bignum_copy(bignum *dst, const bignum *src);

#ifdef WITH_PRINTF
void bignum_print(bignum *bn, char *label);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
