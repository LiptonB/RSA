#ifndef _RSA_KEY_H
#define _RSA_KEY_H

#include <stddef.h>

extern const unsigned char key_n[];
extern const size_t key_n_size;
extern const unsigned char key_e[];
extern const size_t key_e_size;
extern const unsigned char key_d[];
extern const size_t key_d_size;
extern const unsigned char key_p[];
extern const size_t key_p_size;
extern const unsigned char key_q[];
extern const size_t key_q_size;
extern const unsigned char key_dmp1[];
extern const size_t key_dmp1_size;
extern const unsigned char key_dmq1[];
extern const size_t key_dmq1_size;
extern const unsigned char key_iqmp[];
extern const size_t key_iqmp_size;
extern const bignum e;
extern const bignum d;
extern const bignum n;
extern const bignum p;
extern const bignum q;
extern const bignum dmp1;
extern const bignum dmq1;
extern const bignum iqmp;

#endif
