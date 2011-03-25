
#include "lcd.h"
#include "ramp.h"
#include "pid.h"
#include "pwm.h"
#include "enc.h"
#include "eeprom.h"
#include "max6675.h"
#include "serial.h"
#include "menu.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

// 200ms / (8mhz /64 prescaler) = 25000 = 0x61A8
#define TIMER_HIGH 0x61
#define TIMER_LOW  0xA8


unsigned int max6675_data;
volatile uint16_t global_time = 0;
uint8_t max6675_ready;

inline void init_timer();

void init(void) {
	// pb0 as MAX6675 CS, pb5 as SCK, PB1, pb2 as pwm outputs
	DDRB |= _BV(PB0) | _BV(PB5) | _BV(PB1) | _BV(PB2);

	// pc[0:3] as Lcd_data, pc4 - lcd_e, pc5 - lcd_rs
	DDRC |= _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4) | _BV(PC5);

	// set all low
	PORTC = 0;

	// pd4 as output for backlight
	DDRD |= _BV(PD4);

	// enable pullup for encoder and button
	PORTD |= _BV(PD2) | _BV(PD3) | _BV(PD5);


	//Enable fast PWM generation on PD4 and PD5 using timer 1
/* TODO: frequency
  TCCR1A |= _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10); //Setting the PWM mode and waveform generation...
	TCCR1B |= _BV(WGM12) | _BV(CS10);            //...mode and timer source for the timer 1
	OCR1AH = 0x00;                        //Setting PWM duty to 0%
	OCR1AL = 0x00;
*/

	//Enable external interrupts - for encoder
	MCUCR &= ~_BV(ISC11); // 0
	MCUCR |=  _BV(ISC10); // 1 (ISCn1:0 == 01 => external interrupt n will be triggered on level change)
	MCUCR &= ~_BV(ISC01); // 0
	MCUCR |=  _BV(ISC00); // 1

	GICR  |=  _BV(INT0) | _BV(INT1);
	
	//Enable interrupts
	sei();
}

static void print_welcome()
{
	LCD_cursor(0, 0);
	LCD_text(" Super  ");
	LCD_cursor(1, 0);
	LCD_text(" Solder ");
	_delay_ms(500);
	LCD_cursor(0, 0);
	LCD_text(" Solder ");
	LCD_cursor(1, 0);
	LCD_text("  3000  ");
	_delay_ms(500);
	LCD_cursor(0, 0);
	LCD_text("  3000  ");
	LCD_cursor(1, 0);
	LCD_text("        ");
	_delay_ms(500);
}

int main(void) {
	init();

	serial_init();
	//serial_write('\r'); // bootloader bug workaround :)

	max6675_init();
	enc_init();
	LCD_init();
	pid_init();

	BL_ON;

	print_welcome();

	pwm_init();
	init_timer();
	menu_do();

	while (1) {
	}
}

inline void init_timer()
{
	TCCR1A = 0; // normal, ovfl on MAX
	TCNT1H = TIMER_HIGH;
	TCNT1L = TIMER_LOW;
	TCCR1B = 4; /* 4 = 256 prescaler  5 = 1024 */

	/* clear overflow flag, enable interupt */
	TIFR |= _BV(TOV1);
	TIMSK |= _BV(TOIE1);
}

ISR(TIMER1_OVF_vect)
{
	/* reset the count */
	TCNT1H = TIMER_HIGH;
	TCNT1L = TIMER_LOW;

	max6675_data = max6675_read();
	max6675_ready = 1;

	global_time ++;
}

