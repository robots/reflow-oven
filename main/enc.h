#ifndef ENC_h_
#define ENC_h_

#include <stdint.h>

enum enc_dir {
	ENC_STOP,
	ENC_UP,
	ENC_DOWN,
};

void enc_init();
uint8_t enc_read();

#endif

