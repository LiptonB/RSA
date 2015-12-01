#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <stdio.h>
#include <string.h>
#include "key.h"

int main() {
  RSA *keypair = NULL;
  FILE *fp = fopen("key.pub.pem", "r");
  if(PEM_read_RSAPublicKey(fp, &keypair, NULL, NULL) == NULL) {
    ERR_print_errors_fp(stderr);
    return 1;
  }
  fclose(fp);

	unsigned char a_num[] = {0x12, 0x34};
  size_t size = RSA_size(keypair);
  unsigned char *to, *from;
  if ((to = malloc(size)) == NULL || (from = malloc(size)) == NULL) {
    return 1;
  }

  memset(from, 0, size);
  memcpy(from+size-sizeof(a_num), a_num, sizeof(a_num));
  int i;

  if (RSA_public_encrypt(size, from, to, keypair, RSA_NO_PADDING) == -1) {
    printf("Could not use RSA_public_encrypt:\n");
    ERR_print_errors_fp(stderr);
  } else {
    printf("RSA_public_encrypt: 0x");
    for (i = 0; i < size; i++) {
            printf("%02x", to[i]);
    }
    printf("\n");
  }
  free(to);

  BN_CTX *ctx = BN_CTX_new();
	BN_CTX_start(ctx);
  BIGNUM *n = BN_CTX_get(ctx);
  BIGNUM *e = BN_CTX_get(ctx);
  BIGNUM *a = BN_CTX_get(ctx);
  BIGNUM *r = BN_CTX_get(ctx);
  
  BN_bin2bn(key_n, key_n_size, n);
  BN_bin2bn(key_e, key_e_size, e);
  BN_bin2bn(a_num, sizeof(a_num), a);

  BN_mod_exp(r, a, e, n, ctx);

  printf("BN_mod_exp: 0x");
  BN_print_fp(stdout, r);
  printf("\n");

	BIGNUM *d = BN_CTX_get(ctx);
	BIGNUM *p = BN_CTX_get(ctx);
	BIGNUM *q = BN_CTX_get(ctx);
	BIGNUM *dmp1 = BN_CTX_get(ctx);
	BIGNUM *dmq1 = BN_CTX_get(ctx);
	BIGNUM *iqmp = BN_CTX_get(ctx);
	BIGNUM *m1 = BN_CTX_get(ctx);
	BIGNUM *m2 = BN_CTX_get(ctx);
	BN_bin2bn(key_d, key_d_size, d);
	BN_bin2bn(key_p, key_p_size, p);
	BN_bin2bn(key_q, key_q_size, q);
	BN_bin2bn(key_dmp1, key_dmp1_size, dmp1);
	BN_bin2bn(key_dmq1, key_dmq1_size, dmq1);
	BN_bin2bn(key_iqmp, key_iqmp_size, iqmp);

	BN_mod_exp(m1, a, dmp1, p, ctx);
	BN_mod_exp(m2, a, dmq1, q, ctx);
	printf("m1: 0x");
	BN_print_fp(stdout, m1);
	printf("\n");
	printf("m2: 0x");
	BN_print_fp(stdout, m2);
	printf("\n");

	BN_mod_sub(r, m1, m2, p, ctx);
	printf("(m1 - m2) mod p: 0x");
	BN_print_fp(stdout, r);
	printf("\n");

	BN_mod_mul(r, r, iqmp, p, ctx);
	printf("((m1 - m2) * iqmp) mod p: 0x");
	BN_print_fp(stdout, r);
	printf("\n");

	BN_mul(r, r, q, ctx);
	printf("(((m1 - m2) * iqmp) mod p) * q: 0x");
	BN_print_fp(stdout, r);
	printf("\n");

	BN_add(r, r, m2);
	printf("m2 + (((m1 - m2) * iqmp) mod p) * q: 0x");
	BN_print_fp(stdout, r);
	printf("\n");

  BN_mod_exp(r, a, d, n, ctx);
	printf("a^d mod n: 0x");
	BN_print_fp(stdout, r);
	printf("\n");

  BN_mod(m1, r, p, ctx);
	printf("a^d mod p: 0x");
	BN_print_fp(stdout, m1);
	printf("\n");
  BN_mod(m2, r, q, ctx);
	printf("a^d mod q: 0x");
	BN_print_fp(stdout, m2);
	printf("\n");

	BN_CTX_end(ctx);
  BN_CTX_free(ctx);
}
