#include <stdlib.h>
#include "encrypt.h"
#include "key.h"

#define PROFILING 0
#define PROFILING_MAIN 1
#define MAXPROF 15
#include "profiling.h"

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
unsigned int timer;

#define BIGNUM_SIZE 256

void bignum_print(bignum *bn, char *label) {
  int i;
  int size = bignum_size(bn);
  Serial.print(label);
  Serial.print("Length:");
  Serial.print(bn->length);
  Serial.print(" Offset:");
  Serial.print(bn->offset);
  Serial.print(" Num:0x");

  for (i = 0; i < size; i++) {
    Serial.print(bignum_index(bn, i), HEX);
  }
  Serial.print("\n");
}

void timer_start(void) {
  // Based on code from http://www.instructables.com/id/Arduino-Timer-Interrupts
  
  cli(); // stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

  timer = 0;
   
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // enable interupts

  // Set CS10 and CS12 bits for 1024 prescaler and reset prescaler
  GTCCR |= (1 << PSRSYNC);
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

void timer_stop(void) {
  TCCR1B &= ~((1 << CS12) | (1 << CS10));
}

ISR(TIMER1_COMPA_vect) {
  timer++;
}

// Inspired by http://www.dudley.nu/arduino_profiling/
void profile_start(void) {
  cli();
  //Timer2 Settings:  Timer Prescaler /1024
  // Select clock source: internal I/O clock
  ASSR &= ~(1<<AS2);
  // Reset registers
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = 0;
  TCNT2 = 0;
  
  // Configure timer2 in CTC mode
  TCCR2A |= (1<<WGM21);

  // Get interrupts for reaching compare value
  TIMSK2 |= (1<<OCIE2A);
  sei();
  
  // Configure interrupt frequency
  TCCR2B |= (1<<CS22) | (1<<CS20); // Divide by 128
  OCR2A = 125; // Frequency: 16000000 / (128 * 125) = 1000
}

ISR(TIMER2_COMPA_vect) {
  #if PROFILING
    update_profiling_data();
  #endif
}

void setup(void) {
  // put your setup code here, to run once:
  unsigned char out_num[BIGNUM_SIZE];
  unsigned char temp1_num[BIGNUM_SIZE];
  unsigned char temp2_num[BIGNUM_SIZE];

  bignum e = {key_e, key_e_size, 0};
  bignum n = {key_n, key_n_size, 0};
  bignum out = {out_num, BIGNUM_SIZE, 0};
  bignum temp1 = {temp1_num, BIGNUM_SIZE, 0};
  bignum temp2 = {temp2_num, BIGNUM_SIZE, 0};

  unsigned int i, t;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  for (i = 0; i < sizeof(input_val); i++) {
    bignum_set(&in, i, input_val[i]);
  }

#if PROFILING
  profile_start();
#endif
  
  bignum_print(&in, "In: ");
  timer_start();
  bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);
  timer_stop();
  Serial.print("Time elapsed: ");
  Serial.print(timer);
  Serial.print("\n");
  bignum_print(&out, "Out: ");

#if PROFILING
  dump_profiling_data();
#endif

  bignum_copy(&in, &out);
  e.num = key_d;
  e.length = key_d_size;
  e.offset = 0;

  timer_start();
  bignum_modexp(&out, &in, &e, &n, &temp1, &temp2);
  timer_stop();
  Serial.print("Time elapsed: ");
  Serial.print(timer);
  Serial.print("\n");
  bignum_print(&out, "Out2: ");

#if PROFILING
  dump_profiling_data();
#endif
}

void loop() {
  // put your main code here, to run repeatedly:

}