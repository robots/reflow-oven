

#include "eeprom.h"
#include "ramp.h"

#include <string.h>
#include <avr/pgmspace.h>


static uint8_t ramp_number;
static uint8_t ramp_state = 0;
// time
static uint16_t ramp_switch_time = 0;
static uint16_t ramp_cur_len = 0;
// temp
static uint16_t ramp_cur_temp;
static uint16_t ramp_last_temp;

int16_t ramp_coef[10][2];

int16_t ramp_coef_default[10][2] PROGMEM = {
	{10, 30},
	{13, 50},
	{11, 100},
	{13, 100},
	{16, 150},
	{8, 200},
	{7, 300},
	{12, 150},
	{7, 100},
	{11, 30},
};


void ramp_load_default()
{
	memcpy_P(ramp_coef, ramp_coef_default, sizeof(int16_t) * 10 * 2);
}

void ramp_load(uint8_t x)
{
	uint8_t i;
	uint8_t *p;

	p = (uint8_t *)ramp_coef;
	for (i = 0; i < RAMP_SIZE; i ++) {
		*p = eeprom_read(RAMP_ADDR + x * RAMP_SIZE + i);
		p++;
	}
}

void ramp_save(uint8_t x)
{
	uint8_t i;
	uint8_t *p;

	p = (uint8_t *)ramp_coef;
	for (i = 0; i < 10 * 2 * sizeof(int16_t); i ++) {
		eeprom_write(RAMP_ADDR + x * RAMP_SIZE + i, *p);
		p++;
	}
}

void ramp_init(uint8_t x)
{
	ramp_state = 0;
	ramp_number = x;

	ramp_last_temp = 30; // sane default :-)
	ramp_switch_time = 0;
	
	ramp_load(x);

	ramp_cur_len = ramp_coef[ramp_state][0];
	ramp_cur_temp = ramp_coef[ramp_state][1];
}

int16_t ramp_get(uint16_t time)
{
	uint16_t dt;
	int16_t out;

	if (ramp_switch_time == 0) {
		ramp_switch_time = time;
	}

	if (time > ramp_switch_time + ramp_cur_len) {
		ramp_state ++;

		// last state finished
		if (ramp_state >= 10) {
			return -1000;
		}

		ramp_switch_time += ramp_cur_len;
		ramp_last_temp = ramp_cur_temp;

		ramp_cur_len = ramp_coef[ramp_state][0];
		ramp_cur_temp = ramp_coef[ramp_state][1];

		// magic number ... then exit
		if (ramp_cur_temp == -1000) 
			return -1000;
	}

	// calculate temperature
	dt = time - ramp_switch_time;

	// ramp_cur_temp + (dt / cur_len) * (ramp_cur_temp - ramp_last_temp)
	if (ramp_cur_temp >= ramp_last_temp) {
		out = ramp_cur_temp - ramp_last_temp;
		out *= dt;
		out /= ramp_cur_len;

		out += ramp_last_temp;
	} else {
		out = ramp_last_temp - ramp_cur_temp;
		out *= ramp_cur_len - dt;
		out /= ramp_cur_len;

		out += ramp_cur_temp;
	}

	return out;
}

