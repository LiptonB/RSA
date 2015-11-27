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
  bignum in = {input_num, sizeof(input_num), 0};
  bignum e = {key_e, key_e_size, 0};
  bignum d = {key_d, key_d_size, 0};
  bignum n = {key_n, key_n_size, 0};
  bignum out = {out_num, buffer_size, 0};
  bignum temp1 = {temp1_num, sizeof(temp1_num), 0};
  bignum temp2 = {temp2_num, sizeof(temp2_num), 0};
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

  bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);
  bignum_print(&out, "");
}
