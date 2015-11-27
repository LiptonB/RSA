#include <msp430.h>
#include <stdlib.h>
#include "encrypt.h"
#include "key.h"

const unsigned char input_val[] = {0x12, 0x34};

unsigned char input_num[128];
bignum in = {input_num, sizeof(input_num), sizeof(input_num)-sizeof(input_val)};
volatile int timer;

#pragma vector=TIMERA0_VECTOR
__interrupt void inc_timer(void) {
	timer++;
}

#define BIGNUM_SIZE 256

int main(void) {
	unsigned int i;

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // Make system clock as fast as it will go
    // MCLK = 16MHz, SMCLK = 2MHz
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_3;

    unsigned char out_num[BIGNUM_SIZE];
    unsigned char temp1_num[BIGNUM_SIZE];
    unsigned char temp2_num[BIGNUM_SIZE];

    if (out_num == NULL || temp1_num == NULL || temp2_num == NULL) {
    	return 1;
    }

    bignum e = {key_e, key_e_size, 0};
    bignum n = {key_n, key_n_size, 0};
    bignum out = {out_num, BIGNUM_SIZE, 0};
    bignum temp1 = {temp1_num, BIGNUM_SIZE, 0};
    bignum temp2 = {temp2_num, BIGNUM_SIZE, 0};

    for (i = 0; i < sizeof(input_val); i++) {
    	bignum_set(&in, i, input_val[i]);
    }

    // Set up and start 256KHz timer
    TACTL = TASSEL_2 | ID_3;
    TAR = 0;
    TACCR0 = 0;
    TACCTL0 |= CCIE;
    timer = 0;
    _BIS_SR(GIE);
    TACTL |= MC_2;

    bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);
    TACTL &= ~(MC0 | MC1);

	return 0;
}
