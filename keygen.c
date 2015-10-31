#include <openssl/rsa.h>
#include <openssl/engine.h>

void print_array(char *name, BIGNUM *bn) {
  unsigned char *buf;
  int len;
  int i;

  if ((buf = malloc(BN_num_bytes(bn))) != NULL) {
    len = BN_bn2bin(bn, buf);

    printf("char %s[] = {", name);
    for (i = 0; i < len; i++) {
      printf("0x%02x", buf[i]);
      if (i < len-1) {
        printf(", ");
      }
    }
    printf("};\n");

    free(buf);
  }
}

int main(int argc, char *argv[]) {
  RSA *keypair = RSA_generate_key(1024, 65537, NULL, NULL);
  print_array("n", keypair->n);
  print_array("e", keypair->e);
  print_array("d", keypair->d);
  print_array("p", keypair->d);
  print_array("q", keypair->d);
  print_array("dmp1", keypair->d);
  print_array("dmq1", keypair->d);
  print_array("iqmp", keypair->d);
  return 0;
}
