#ifndef _RSA_ENCRYPT_H
#define _RSA_ENCRYPT_H

#include <stdint.h>

// Set up sizes appropriately for architecture
typedef uint16_t word;    // size of a multiplication output
typedef uint8_t halfword; // size of a multiplication input
typedef int16_t halfwordsigned;  // a halfword plus sign bit

typedef struct bignum {
	halfword *num;
	unsigned int length;
	unsigned int offset;
} bignum;

#endif

void bignum_modexp(bignum *out, bignum *M, bignum *e, bignum *n, bignum *temp1, bignum *temp2);
