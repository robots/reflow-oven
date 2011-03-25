#include "main.h"
#include "enc.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#define ENC_STATE ((BUTTONS & (_BV(ENC1) | _BV(ENC2))) >> 2)

static uint8_t enc_dir = ENC_STOP;
static uint8_t enc_last = 0;

void enc_init()
{
	enc_last = ENC_STATE;
}

uint8_t enc_read()
{
	uint8_t out;
	out = enc_dir;
	enc_dir = ENC_STOP;
	return out;
}

static void enc_int()
{
	uint8_t new_state;
	static uint8_t dir;

	new_state = ENC_STATE;

	if (new_state == enc_last) {
		enc_dir = ENC_STOP;
		return;
	}
/* tooo precise :(	
	enc_dir = ENC_DOWN;
	switch (enc_last & 3) {
		case 0: // 00 -> 10 = up
			if (new_state == 2)
				enc_dir = ENC_UP;
			break;
		case 1: // 01 -> 00 = up
			if (new_state == 0)
				enc_dir = ENC_UP;
			break;
		case 2: // 10 -> 11 = up
			if (new_state == 3)
				enc_dir = ENC_UP;
			break;
		case 3: // 11 -> 01 = up
			if (new_state == 1)
				enc_dir = ENC_UP;
			break;
	}
*/

	if (enc_last == 3) {
		if (new_state == 1) {
			dir = ENC_UP;
		} else if (new_state == 2) {
			dir = ENC_DOWN;
		}
	}

	if ((enc_last != 3) && (new_state == 3)) {
		enc_dir = dir;
	}
	enc_last = new_state;
}

ISR(INT1_vect)
{
	enc_int();
}

ISR(INT0_vect)
{
	enc_int();
}


