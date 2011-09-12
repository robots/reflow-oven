#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>

//Needed by <avr/delay.h>
#define F_CPU    8000000UL

#define TICKS_PER_SEC 5
#define S2TIME(x) ((x)*TICKS_PER_SEC)

#define BL_PORT  PORTD
#define BL_PIN   PD4
#define BL_ON    BL_PORT |= _BV(BL_PIN)
#define BL_OFF   BL_PORT &= ~_BV(BL_PIN)

#define BUTTONS  PIND
#define BUT      PD5
#define ENC1     PD2
#define ENC2     PD3

#define PWM_PORT PORTB
#define PWM_1    PB1
#define PWM_2    PB2

#define SPI_PORT PORTB
#define MISO     PB4
#define SCK      PB5

extern unsigned int max6675_data;
extern volatile uint16_t global_time;
extern uint8_t max6675_ready;

#endif
