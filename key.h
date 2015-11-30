#ifndef _RSA_KEY_H
#define _RSA_KEY_H

#include <stddef.h>

extern unsigned char key_n[];
extern size_t key_n_size;
extern unsigned char key_e[];
extern size_t key_e_size;
extern unsigned char key_d[];
extern size_t key_d_size;
extern unsigned char key_p[];
extern size_t key_p_size;
extern unsigned char key_q[];
extern size_t key_q_size;
extern unsigned char key_dmp1[];
extern size_t key_dmp1_size;
extern unsigned char key_dmq1[];
extern size_t key_dmq1_size;
extern unsigned char key_iqmp[];
extern size_t key_iqmp_size;

#endif
