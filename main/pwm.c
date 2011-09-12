
#include "main.h"
#include "pwm.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t pwm_count;
static uint8_t pwm_value1;
static uint8_t pwm_value2;

void pwm_init()
{
	pwm_count = 0;
	pwm_value1 = 0;
	pwm_value2 = 0;

	// 8mhz / 256 / 256 = 122/sec
	// pwm period ~ 2sec
	TIFR |= _BV(TOV0);
	TIMSK |= _BV(TOIE0);
	TCNT0 = 0;
	TCCR0 |= _BV(CS02) /*| _BV(CS00)*/;
}

void pwm_set1(uint8_t in)
{
	pwm_value1 = in;
}

void pwm_set2(uint8_t in)
{
	pwm_value2 = in;
}

ISR(TIMER0_OVF_vect)
{
	pwm_count ++;
	if (pwm_value1 >= pwm_count) {
		PWM_PORT |= _BV(PWM_1);
	} else {
		PWM_PORT &= ~_BV(PWM_1);
	}
	if (pwm_value2 >= pwm_count) {
		PWM_PORT |= _BV(PWM_2);
	} else {
		PWM_PORT &= ~_BV(PWM_2);
	}
}

