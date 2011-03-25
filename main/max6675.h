#ifndef MAX6675_h_
#define MAX6675_h_

#include <avr/io.h>
#include <stdint.h>

#define MAX6675_MISO     (PINB & _BV(PB4))
#define MAX6675_MISO_PULLUP PORTB |= _BV(PB4)

#define MAX6675_SCK_HIGH PORTB |= _BV(PB5)
#define MAX6675_SCK_LOW  PORTB &= ~_BV(PB5)

#define MAX6675_CS_HIGH  PORTB |= _BV(PB0)
#define MAX6675_CS_LOW   PORTB &= ~_BV(PB0)

#define MAX6675_TEMP(x)  ((x) >> 3)
#define MAX6675_TC_OPEN  0x0004

void max6675_init();
uint16_t max6675_read();


#endif

