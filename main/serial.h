#ifndef SERIAL_h_
#define SERIAL_h_

#include <avr/io.h>

// 19200 @ 8mhz 
#define	BRREG_VALUE	25


void serial_init(void);
void serial_write_hex(uint8_t v);
void serial_write(uint8_t c);
uint8_t serial_read(void);

#endif

