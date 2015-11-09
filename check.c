#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <stdio.h>
#include "key.c"

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
  unsigned char *to;
  if ((to = malloc(size)) == NULL) {
    return 1;
  }
  int i;

  if (RSA_public_encrypt(sizeof(a_num), a_num, to, keypair, RSA_NO_PADDING) == -1) {
    printf("Could not use RSA_public_encrypt:\n");
    ERR_print_errors_fp(stderr);
  } else {
    printf("RSA_public_encrypt: 0x");
    for (i = 0; i < size; i++) {
            printf("%02x", to[i]);
    }
    printf("\n");
  }

  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();
  BIGNUM *a = BN_new();
  BIGNUM *r = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  
  BN_bin2bn(key_n, sizeof(key_n), n);
  BN_bin2bn(key_e, sizeof(key_e), e);
  BN_bin2bn(a_num, sizeof(a_num), a);

  BN_mod_exp(r, a, e, n, ctx);

  printf("BN_mod_exp: 0x");
  BN_print_fp(stdout, r);
  printf("\n");

  free(to);
  BN_free(n);
  BN_free(e);
  BN_free(a);
  BN_free(r);
  BN_CTX_free(ctx);
}
