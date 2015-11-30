#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>

void fprint_array(FILE *fp, char *name, BIGNUM *bn) {
  unsigned char *buf;
  int len;
  int i;

  if ((buf = malloc(BN_num_bytes(bn))) != NULL) {
    len = BN_bn2bin(bn, buf);

    fprintf(fp, "const unsigned char key_%s[] = {", name);
    for (i = 0; i < len; i++) {
      fprintf(fp, "0x%02x", buf[i]);
      if (i < len-1) {
        fprintf(fp, ", ");
      }
    }
    fprintf(fp, "};\n");
    fprintf(fp, "const size_t key_%s_size = sizeof(key_%s);\n", name, name);

    free(buf);
  }
}

int main(int argc, char *argv[]) {
  FILE *fp = fopen("key.c", "w");
  fprintf(fp, "#include <stddef.h>\n\n");

  RSA *keypair = RSA_generate_key(1024, 65537, NULL, NULL);
  fprint_array(fp, "n", keypair->n);
  fprint_array(fp, "e", keypair->e);
  fprint_array(fp, "d", keypair->d);
  fprint_array(fp, "p", keypair->p);
  fprint_array(fp, "q", keypair->q);
  fprint_array(fp, "dmp1", keypair->dmp1);
  fprint_array(fp, "dmq1", keypair->dmq1);
  fprint_array(fp, "iqmp", keypair->iqmp);
  fclose(fp);

  fp = fopen("key.priv.pem", "w");
  PEM_write_RSAPrivateKey(fp, keypair, NULL, NULL, 0, NULL, NULL);
  fclose(fp);
  fp = fopen("key.pub.pem", "w");
  PEM_write_RSAPublicKey(fp, keypair);
  fclose(fp);
  return 0;
}
