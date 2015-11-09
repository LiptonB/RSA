#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdio.h>

int main() {
  RSA *keypair = NULL;
  FILE *fp = fopen("key.pub.pem", "r");
  if(PEM_read_RSAPublicKey(fp, &keypair, NULL, NULL) == NULL) {
    ERR_print_errors_fp(stderr);
  }
  fclose(fp);

	unsigned char a_num[] = {0x12, 0x34};
  size_t size = RSA_size(keypair);
  unsigned char *to = malloc(size);
  int i;
  RSA_public_encrypt(sizeof(a_num), a_num, to, keypair, RSA_NO_PADDING);

  printf("0x");
  for (i = 0; i < size; i++) {
					printf("%02x", to[i]);
	}
	printf("\n");

}
