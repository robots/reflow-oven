#ifndef EEPROM_h_
#define EEPROM_h_

#include <stdint.h>
#include <avr/io.h>

// pid gains
#define PID_GAIN_ADDR 0
#define PID_GAIN_SIZE (3*sizeof(int16_t))

// pid i-term limits
#define PID_ILIMITS_ADDR (PID_GAIN_ADDR + PID_GAIN_SIZE)
#define PID_ILIMITS_SIZE (2*sizeof(int16_t))

// pid d-term limits
#define PID_DLIMITS_ADDR (PID_ILIMITS_ADDR + PID_ILIMITS_SIZE)
#define PID_DLIMITS_SIZE (2*sizeof(int16_t))

// pid error limits
#define PID_ELIMITS_ADDR (PID_DLIMITS_ADDR + PID_DLIMITS_SIZE)
#define PID_ELIMITS_SIZE (2*sizeof(int16_t))

// pid output limits
#define PID_OLIMITS_ADDR (PID_ELIMITS_ADDR + PID_ELIMITS_SIZE)
#define PID_OLIMITS_SIZE (2*sizeof(int16_t))

// storage for 2 ramps: 10 points/each * 2 (time + temp)
#define RAMP_ADDR (PID_OLIMITS_ADDR + PID_OLIMITS_SIZE)
#define RAMP_SIZE (10*2*sizeof(int16_t))

#define RAMP1_ADDR (RAMP1_ADDR + RAMP1_SIZE)
#define RAMP1_SIZE (10*2*sizeof(int16_t))


uint8_t eeprom_read(uint8_t addr);
void eeprom_write(uint8_t addr, uint8_t data);

#endif

