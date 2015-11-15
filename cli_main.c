int main(int argc, char *argv[]) {
	unsigned char input_num[128];
  char input_str[258];
	unsigned char out_num[2*sizeof(key_n)];
  unsigned char temp1_num[sizeof(out_num)];
  unsigned char temp2_num[sizeof(out_num)];
  bignum in = {input_num, sizeof(input_num), 0};
  bignum e = {key_e, sizeof(key_e), 0};
  bignum d = {key_d, sizeof(key_d), 0};
  bignum n = {key_n, sizeof(key_n), 0};
  bignum out = {out_num, sizeof(out_num), 0};
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

  bignum_modexp(&out, &in, &d, &n, &temp1, &temp2);
	bignum_print(&out, "");
}
