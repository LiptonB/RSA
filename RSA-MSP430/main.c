#include <msp430.h>
#include <stdlib.h>
#include "encrypt.h"
#include "key.h"

const unsigned char input_num[] = {0x12, 0x34};
const bignum in = {input_num, sizeof(input_num), 0};

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    size_t out_size = key_n_size << 1;
    unsigned char *out_num = malloc(out_size);
    unsigned char *temp1_num = malloc(out_size);
    unsigned char *temp2_num = malloc(out_size);

    if (out_num == NULL || temp1_num == NULL || temp2_num == NULL) {
    	return 1;
    }

    bignum e = {key_e, key_e_size, 0};
    bignum n = {key_n, key_n_size, 0};
    bignum out = {out_num, out_size, 0};
    bignum temp1 = {temp1_num, out_size, 0};
    bignum temp2 = {temp2_num, out_size, 0};

    bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);

	return 0;
}
