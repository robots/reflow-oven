
#include "eeprom.h"

#include "pid.h"

#include <string.h>
#include <avr/pgmspace.h>

int16_t pid_gain[3];

int16_t pid_limit[4][2];

static int16_t pid_lasterror;
static int16_t pid_ierror;

int16_t pid_gain_default[3] PROGMEM = { 16, 8, 1 };
int16_t pid_limit_default[4][2] PROGMEM = {
	{-256, 256},
	{-256, 256},
	{-512, 512},
	{0, 4095},
};

void pid_load_default()
{
	memcpy_P(pid_gain, pid_gain_default, sizeof(int16_t) * 3);
	memcpy_P(pid_limit, pid_limit_default, sizeof(int16_t) * 4 * 2);
}

void pid_load()
{
	uint8_t i;
	uint8_t *p;

	// gain
	p = (uint8_t *)pid_gain; 
	for (i = 0; i < PID_GAIN_SIZE; i ++) {
		*p = eeprom_read(PID_GAIN_ADDR + i);
		p++;
	}

	p = (uint8_t *)pid_limit; 
	for (i = 0; i < PID_ILIMITS_SIZE * 4; i ++) {
		*p = eeprom_read(PID_ILIMITS_ADDR + i);
		p++;
	}
}

void pid_save()
{
	uint8_t i;
	uint8_t *p;

	// gain
	p = (uint8_t *)pid_gain; 
	for (i = 0; i < PID_GAIN_SIZE; i ++) {
		eeprom_write(PID_GAIN_ADDR + i, *p);
		p++;
	}

	p = (uint8_t *)pid_limit; 
	for (i = 0; i < PID_ILIMITS_SIZE * 4; i ++) {
		eeprom_write(PID_ILIMITS_ADDR + i, *p);
		p++;
	}

}

void pid_init()
{
	pid_load();

	pid_lasterror = 0;
	pid_ierror = 0;
}

int16_t pid_do(int16_t in, int16_t target)
{
	int16_t out;
	int16_t error;
	int16_t pid_derror;

	error = target - in;

	if (error < pid_limit[LIMIT_E][0])
		error = pid_limit[LIMIT_E][0];
	if (error > pid_limit[LIMIT_E][1])
		error = pid_limit[LIMIT_E][1];

	pid_derror = (error - pid_lasterror); 

	if (pid_derror < pid_limit[LIMIT_D][0])
		pid_derror = pid_limit[LIMIT_D][0];
	if (pid_derror > pid_limit[LIMIT_D][1])
		pid_derror = pid_limit[LIMIT_D][1];

	pid_ierror += error;

	if (pid_ierror < pid_limit[LIMIT_I][0])
		pid_ierror = pid_limit[LIMIT_I][0];
	if (pid_ierror > pid_limit[LIMIT_I][1])
		pid_ierror = pid_limit[LIMIT_I][1];

	out  = pid_gain[TERM_P] * error;
	out += pid_gain[TERM_I] * pid_ierror;
	out += pid_gain[TERM_D] * pid_derror;

	pid_lasterror = error;

	if (out < pid_limit[LIMIT_O][0]) 
		out = pid_limit[LIMIT_O][0];
	if (out > pid_limit[LIMIT_O][1])
		out = pid_limit[LIMIT_O][1];

  return out;
}

