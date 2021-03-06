#include <msp430.h>
#include <stdlib.h>
#include "encrypt.h"
#include "key.h"

#define USING_CRT 1
#if USING_CRT
#define BIGNUM_SIZE 128
#else
#define BIGNUM_SIZE 256
#endif

const unsigned char input_val[] = {0x12, 0x34};
/*const unsigned char input_val[] = {0x70, 0x27, 0xc2, 0xa4, 0x9b, 0x88, 0xd8,
		0x11, 0x38, 0x35, 0x5c, 0x09, 0x91, 0xc9, 0x71, 0x48, 0x20, 0x6e, 0xf0,
		0x4a, 0x04, 0x7f, 0x73, 0x9a, 0x2f, 0x31, 0x17, 0x07, 0x2e, 0x7e, 0xa7,
		0x9c, 0xbc, 0x54, 0x1e, 0xb7, 0x9c, 0xbf, 0x91, 0xf5, 0x48, 0xcc, 0x88,
		0x71, 0x3d, 0xde, 0x00, 0x05, 0xb4, 0x06, 0x08, 0x1c, 0x9e, 0x15, 0x66,
		0x8e, 0xeb, 0x67, 0x73, 0x78, 0xa2, 0x29, 0xe5, 0x21, 0x8b, 0x78, 0xe2,
		0x9e, 0x89, 0xec, 0x65, 0x93, 0xb0, 0x07, 0xd7, 0xf8, 0xfa, 0xbe, 0x5e,
		0xd6, 0x59, 0x34, 0xe1, 0x29, 0x41, 0xbc, 0x89, 0x6b, 0x4e, 0xdd, 0xeb,
		0x09, 0xc0, 0x10, 0xbc, 0x75, 0xb3, 0xb7, 0xe3, 0xab, 0x50, 0x7a, 0x4e,
		0xb4, 0x29, 0xe0, 0xc2, 0x4c, 0x2e, 0x49, 0x37, 0xc9, 0x9d, 0xdf, 0x15,
		0xda, 0x5e, 0xd4, 0x34, 0xec, 0xff, 0x92, 0xed, 0x4a, 0x16, 0x88, 0x53,
		0xc3};*/

unsigned char input_num[128];
bignum in = {input_num, sizeof(input_num), sizeof(input_num)-sizeof(input_val)};
volatile int timer;
volatile int timeenc, timedec;

unsigned char out_num[BIGNUM_SIZE];
unsigned char temp1_num[BIGNUM_SIZE];
unsigned char temp2_num[BIGNUM_SIZE];
#if USING_CRT
unsigned char temp3_num[BIGNUM_SIZE];
bignum temp3 = {temp3_num, BIGNUM_SIZE, 0, 0};
#endif
bignum out = {out_num, BIGNUM_SIZE, 0, 0};
bignum temp1 = {temp1_num, BIGNUM_SIZE, 0, 0};
bignum temp2 = {temp2_num, BIGNUM_SIZE, 0, 0};

#pragma vector=TIMERA0_VECTOR
__interrupt void inc_timer(void) {
	timer++;
}

int main(void) {
	unsigned int i;

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // Make system clock as fast as it will go
    // MCLK = 16MHz, SMCLK = 2MHz
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_3;

    for (i = 0; i < sizeof(input_val); i++) {
    	bignum_set(&in, i, input_val[i]);
    }

    // Set up and start 250KHz timer
    TACTL = TASSEL_2 | ID_3;
    TAR = 0;
    TACCTL0 |= CCIE;
    timer = 0;
    _BIS_SR(GIE);

    TACTL |= MC_1;
    TACCR0 = 50000; // Will be hit 5 times per second
#if USING_CRT
    bignum_modexp_crt(&out, &in, &p, &q, &dmp1, &dmq1, &iqmp, &temp1, &temp2, &temp3);
#else
    bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);
#endif
    TACTL &= ~(MC0 | MC1);
    timeenc = timer;

#if !USING_CRT
    // set up next trial
    bignum_copy(&in, &out);

    timer = 0;
    TAR = 0;

    TACTL |= MC_2;
    bignum_modexp(&out, &in, &d, &n, &temp1, &temp2);
    TACTL &= ~(MC0 | MC1);
    timedec = timer;
#endif

	return 0;
}
