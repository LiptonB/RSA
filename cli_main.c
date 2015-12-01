#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encrypt.h"
#include "key.h"

int main(int argc, char *argv[]) {
  unsigned char input_num[128];
  char input_str[258];
  size_t buffer_size = 2 * key_n_size;
  unsigned char *out_num = malloc(buffer_size);
  unsigned char *temp1_num = malloc(buffer_size);
  unsigned char *temp2_num = malloc(buffer_size);
  unsigned char *temp3_num = malloc(buffer_size);
  unsigned char extraval1_num[] = {0xab, 0x23};
  unsigned char extraval2_num[] = {0xff, 0xff};
  bignum in = {input_num, sizeof(input_num), 0, 0};
  bignum e = {key_e, key_e_size, 0, 0};
  bignum d = {key_d, key_d_size, 0, 0};
  bignum n = {key_n, key_n_size, 0, 0};
  bignum p = {key_p, key_p_size, 0, 0};
  bignum q = {key_q, key_q_size, 0, 0};
  bignum dmp1 = {key_dmp1, key_dmp1_size, 0, 0};
  bignum dmq1 = {key_dmq1, key_dmq1_size, 0, 0};
  bignum iqmp = {key_iqmp, key_iqmp_size, 0, 0};
  bignum out = {out_num, buffer_size, 0, 0};
  bignum temp1 = {temp1_num, buffer_size, 0, 0};
  bignum temp2 = {temp2_num, buffer_size, 0, 0};
  bignum temp3 = {temp3_num, buffer_size, 0, 0};
  bignum extraval1 = {extraval1_num, sizeof(extraval1_num), 0, 0};
  bignum extraval2 = {extraval2_num, sizeof(extraval2_num), 0, 0};
  int str_idx, num_idx;

  fgets(input_str, sizeof(input_str), stdin);
  memset(input_num, 0, sizeof(input_num));
  num_idx = sizeof(input_num)-1;
  for (str_idx = strlen(input_str)-3; str_idx >= 0; str_idx -= 2) {
    sscanf(&input_str[str_idx], "%2hhx", &input_num[num_idx]);
    num_idx--;
  }

  bignum_truncate(&in);
  bignum_print(&in, "input: ");

  bignum_modexp(&out, &in, &d, &n, &temp1, &temp2);
  //bignum_modexp_crt(&out, &in, &p, &q, &dmp1, &dmq1, &iqmp, &temp1, &temp2, &temp3);
  bignum_print(&out, "");

  free(out_num);
  free(temp1_num);
  free(temp2_num);
}
