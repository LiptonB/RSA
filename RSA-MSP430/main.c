#include <msp430.h>
#include "encrypt.h"
#include "key.h"

unsigned char input_num[] = {0x12, 0x34};

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    unsigned char out_num[key_n_size<<1];
    unsigned char temp1_num[sizeof(out_num)];
    unsigned char temp2_num[sizeof(out_num)];
    bignum in = {input_num, sizeof(input_num), 0};
    bignum e = {key_e, key_e_size, 0};
    bignum n = {key_n, key_n_size, 0};
    bignum out = {out_num, sizeof(out_num), 0};
    bignum temp1 = {temp1_num, sizeof(temp1_num), 0};
    bignum temp2 = {temp2_num, sizeof(temp2_num), 0};

    bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);

	return 0;
}
