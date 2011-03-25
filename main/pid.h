#ifndef PID_h_
#define PID_h_

#include <stdint.h>

enum {
	TERM_P,
	TERM_I,
	TERM_D
};

enum limits {
	LIMIT_I,
	LIMIT_D,
	LIMIT_E,
	LIMIT_O
};

extern int16_t pid_gain[3];
extern int16_t pid_limit[4][2];

void pid_load_default();
void pid_init();
void pid_save();
void pid_load();
int16_t pid_do(int16_t in, int16_t target);

#endif

