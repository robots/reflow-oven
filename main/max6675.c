
#include "main.h"

#include "max6675.h"

#include <util/delay.h>

void max6675_init()
{
	MAX6675_CS_HIGH;
	MAX6675_MISO_PULLUP;
}

uint16_t max6675_read()
{
	uint16_t out = 0;
	uint8_t i;

	MAX6675_SCK_LOW;
	MAX6675_CS_LOW;

	for (i = 0; i < 16; i ++) {
		MAX6675_SCK_HIGH;
		out = out << 1;
		MAX6675_SCK_LOW;
		out |= MAX6675_MISO != 0;
	}

	MAX6675_CS_HIGH;

	// test bits that should be zero
	if (out & 0x8002) {
		return 0;
	}


	return out;
}


