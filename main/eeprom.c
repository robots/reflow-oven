#include "eeprom.h"

uint8_t eeprom_read(uint8_t addr)
{
	EEARL = addr;
	EEARH = 0;
	EECR |= (1 << EERE);

	return EEDR;
}

void eeprom_write(uint8_t addr, uint8_t data)
{
	EEARL = addr;
	EEARH = 0;
	EEDR = data;

	EECR |= (1 << EEMWE);
	EECR |= (1 << EEWE);
	while (EECR & (1 << EEWE));
}

