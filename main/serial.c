#include "serial.h"

void serial_init(void)
{
	UBRRL = BRREG_VALUE;
	UCSRB = _BV(TXEN) | _BV(RXEN);
}

void serial_write_hex(uint8_t v)
{
	 unsigned char temp;

	temp = v >> 4;
	if(temp > 9)
		temp += 55;
	else
		temp += '0';
	serial_write(temp);

	temp = v & 0x0F;
	if(temp > 9)
		temp += 55;
	else
		temp += '0';
	serial_write(temp);
}

void serial_write(uint8_t c)
{
	UDR = c;
	while (!(UCSRA & _BV(TXC)));
	UCSRA |= _BV(TXC);
}


uint8_t serial_read(void)
{
	while(!(UCSRA & _BV(RXC)));
	return UDR;
}
