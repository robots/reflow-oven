#ifndef RAMP_h_
#define RAMP_h_

#include <stdint.h>

extern int16_t ramp_coef[10][2];

void ramp_load_default();
void ramp_load(uint8_t x);
void ramp_save(uint8_t x);
void ramp_init(uint8_t x);
int16_t ramp_get(uint16_t time);

#endif

