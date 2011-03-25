#include <stdio.h>

#include "ramp.h"
#include "pid.h"

#include "serial.h"
#include "eeprom.h"
#include "pwm.h"
#include "lcd.h"
#include "enc.h"
#include "main.h"
#include "menu.h"
#include "max6675.h"

#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define SAFE_MENU 0

static void menu_action();
static void menu_value_i16(int16_t *x);
static void menu_value_u16(uint16_t *x);

static void run_ramp1();
static void run_ramp2();
static void run_const_temp();
static void run_reset();

static void run_load_selected_profile();
static void run_save_selected_profile();
static void run_edit_ramp_time();
static void run_edit_ramp_temp();

static void run_reset_pid();
static void run_reset_ramp1();
static void run_reset_ramp2();

static uint16_t menu_selected_profile = 0;
static uint16_t menu_selected_parameter = 0;

static const struct menuitem_t menu_defaults[] PROGMEM = {
	{
		.text1 = "RESET   ",
		.text2 = "     PID",
		.type = MENU_EXEC,
		.func = run_reset_pid,
	},
	{
		.text1 = "RESET   ",
		.text2 = "PROFILE1",
		.type = MENU_EXEC,
		.func = run_reset_ramp1,
	},
	{
		.text1 = "RESET   ",
		.text2 = "PROFILE2",
		.type = MENU_EXEC,
		.func = run_reset_ramp2,
	},
	{
		.text1 = "        ",
		.text2 = "Back    ",
		.type = MENU_BACK,
	},
	{
		.type = MENU_LAST,
	}
};

static const struct menuitem_t menu_pidtune[] PROGMEM = {
	{
		.text1 = "Load    ",
		.text2 = "  stored",
		.type = MENU_EXEC,
		.func = pid_load,
	},
	{
		.text1 = "Set     ",
		.text2 = "  P gain",
		.type = MENU_VAL_I16,
		.p = &pid_gain[TERM_P],
	},
	{
		.text1 = "Set     ",
		.text2 = "  I gain",
		.type = MENU_VAL_I16,
		.p = &pid_gain[TERM_I],
	},
	{
		.text1 = "Set high",
		.text2 = " I limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_I][0],
	},
	{
		.text1 = "Set low ",
		.text2 = " I limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_I][1],
	},
	{
		.text1 = "Set high",
		.text2 = " D limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_D][0],
	},
	{
		.text1 = "Set low ",
		.text2 = " D limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_D][1],
	},
	{
		.text1 = "Set high",
		.text2 = " E limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_E][0],
	},
	{
		.text1 = "Set low ",
		.text2 = " E limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_E][1],
	},
	{
		.text1 = "Set high",
		.text2 = " O limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_O][0],
	},
	{
		.text1 = "Set low ",
		.text2 = " O limit",
		.type = MENU_VAL_I16,
		.p = &pid_limit[LIMIT_O][1],
	},
	{
		.text1 = "Store   ",
		.text2 = "settings",
		.type = MENU_EXEC,
		.func = pid_save,
	},
	{
		.text1 = "        ",
		.text2 = "Back    ",
		.type = MENU_BACK,
	},
	{
		.type = MENU_LAST,
	}
};

static const struct menuitem_t menu_ramptune[] PROGMEM = {
	{
		.text1 = "Select  ",
		.text2 = " profile",
		.type = MENU_VAL_U16,
		.p = &menu_selected_profile,
	},
	{
		.text1 = "Load    ",
		.text2 = " profile",
		.type = MENU_EXEC,
		.func = run_load_selected_profile,
	},
	{
		.text1 = "Select  ",
		.text2 = "param  #",
		.type = MENU_VAL_U16,
		.p = &menu_selected_parameter,
	},
	{
		.text1 = "Edit    ",
		.text2 = " time   ",
		.type = MENU_EXEC,
		.func = run_edit_ramp_time,
	},
	{
		.text1 = "Edit    ",
		.text2 = " temp   ",
		.type = MENU_EXEC,
		.func = run_edit_ramp_temp,
	},
	{
		.text1 = "Save    ",
		.text2 = " profile",
		.type = MENU_EXEC,
		.func = run_save_selected_profile,
	},
	{
		.text1 = "        ",
		.text2 = "Back    ",
		.type = MENU_BACK,
	},
	{
		.type = MENU_LAST,
	}
};

static const struct menuitem_t menu_settings[] PROGMEM = {
	{
		.text1 = "Set PID ",
		.text2 = "  params",
		.type = MENU_SUB,
		.p = (void *)menu_pidtune,
	},
	{
		.text1 = "Set prof",
		.text2 = "  params",
		.type = MENU_SUB,
		.p = (void *)menu_ramptune,
	},
	{
		.text1 = "        ",
		.text2 = "Back    ",
		.type = MENU_BACK,
	},
	{
		.type = MENU_LAST,
	}
};

static const struct menuitem_t menu_cook[] PROGMEM = {
	{
		.text1 = "Run  1st",
		.text2 = " profile",
		.type = MENU_EXEC,
		.func = run_ramp1,
	},
	{
		.text1 = "Run  2nd",
		.text2 = " profile",
		.type = MENU_EXEC,
		.func = run_ramp2,
	},
	{
		.text1 = "Constant",
		.text2 = "    temp",
		.type = MENU_EXEC,
		.func = run_const_temp,
	},
	{
		.text1 = "        ",
		.text2 = "Back    ",
		.type = MENU_BACK,
	},
	{
		.type = MENU_LAST,
	}
};

static const struct menuitem_t menu_main[] PROGMEM = {
	{
		.text1 = "Do some ",
		.text2 = " cooking",
		.type = MENU_SUB,
		.p = (void *)menu_cook,
	},
	{
		.text1 = "Set     ",
		.text2 = "  params",
		.type = MENU_SUB,
		.p = (void *)menu_settings,
	},
	{
		.text1 = "RESET   ",
		.text2 = "DEFAULTS",
		.type = MENU_SUB,
		.p = (void *)menu_defaults,
	},
	{
		.text1 = "RESTART ",
		.text2 = " RESTART",
		.type = MENU_EXEC,
		.func = run_reset,
	},
	{
		.type = MENU_LAST,
	}
};


const struct menuitem_t *menu_item_stack[MENU_MAX_DEPTH];
uint8_t menu_pos_stack[MENU_MAX_DEPTH];
uint8_t menu_depth;

void menu_do()
{
	uint16_t new_pos;
	const struct menuitem_t *menu;
		
	// initialize menu
	menu_depth = 0;
	menu_pos_stack[menu_depth] = 0;
	menu_item_stack[menu_depth] = menu_main;

	while (1) {
		uint8_t event = enc_read();
		
		// check encoder
		if (event == ENC_UP) {
			if (menu_pos_stack[menu_depth] > 0)
				menu_pos_stack[menu_depth]--;
		} else if (event == ENC_DOWN) {
			new_pos = menu_pos_stack[menu_depth] + 1;
			menu = menu_item_stack[menu_depth];

			if (pgm_read_byte(&menu[new_pos].type) != MENU_LAST)
				menu_pos_stack[menu_depth] = new_pos;
		}

		// check button - with stupid debouncer
		if (!(BUTTONS & _BV(BUT))) {
			_delay_ms(1);
			while (!(BUTTONS & _BV(BUT)));
			menu_action();
			_delay_ms(1);
			while (!(BUTTONS & _BV(BUT)));
		}

		menu_redraw();

		_delay_ms(10);
	}
}

void menu_redraw()
{
	const struct menuitem_t *menu;
	uint8_t pos;

	char *text1;
	char *text2;

	menu = menu_item_stack[menu_depth];
	pos = menu_pos_stack[menu_depth];

	text1 = (char *)pgm_read_word(&menu[pos].text1);
	text2 = (char *)pgm_read_word(&menu[pos].text2);

	LCD_cursor(0, 0);
	LCD_text(text1);

	LCD_cursor(1, 0);
	if (text2) {
		LCD_text(text2);
	} else {
		LCD_text("        ");
	}
}

static void menu_action()
{
	const struct menuitem_t *menu;
	void (*func)(void);
	uint8_t pos;

	pos = menu_pos_stack[menu_depth];
	menu = menu_item_stack[menu_depth];

	func = (void *)pgm_read_word(&menu[pos].func);

	switch (pgm_read_byte(&menu[pos].type)) {
		case MENU_LABEL:
			break; /* no action taken */
		case MENU_SUB:
			if (!(menu_depth+1 < MENU_MAX_DEPTH))
				break;

			menu_depth++;
			menu_pos_stack[menu_depth] = 0;
			menu_item_stack[menu_depth] = (struct menuitem_t *)pgm_read_word(&menu[pos].p);
			break;
		case MENU_EXEC:
			func();
			break;
		case MENU_VAL_U16:
			menu_value_u16((uint16_t *)pgm_read_word(&menu[pos].p));
			if (func != 0)
				func();
			break;
		case MENU_VAL_I16:
			menu_value_i16((int16_t *)pgm_read_word(&menu[pos].p));
			if (func != 0)
				func();
			break;
		case MENU_BACK:
			menu_depth--;
			break;
	}
}

static void menu_value_i16(int16_t *x)
{
	int16_t val; 
	char buf[9];

	uint8_t done = 0;
	uint8_t update = 1;

	val = *x;

	sprintf(buf, "O:%+06d", val);
	LCD_cursor(0, 0);
	LCD_text(buf);

	_delay_ms(100);

	while (done==0) {
		if (update == 1) {
			update = 0;
			sprintf(buf, "N:%+06d", val);
			LCD_cursor(1, 0);
			LCD_text(buf);
		}

		uint8_t event = enc_read();
		
		// check encoder
		if (event == ENC_UP) {
				val++;
				update = 1; 
		} else if (event == ENC_DOWN) {
				val--;
				update = 1;
		}

		// check button
		if (!(BUTTONS & _BV(BUT))) {
				done = 1;
		}
	}

	*x = val;
}

static void menu_value_u16(uint16_t *x)
{
	uint16_t val; 
	char buf[9];

	uint8_t done = 0;
	uint8_t update = 1;

	val = *x;

	sprintf(buf, "O: %05u", val);
	LCD_cursor(0, 0);
	LCD_text(buf);

	_delay_ms(100);

	while (done==0) {
		if (update == 1) {
			update = 0;
			sprintf(buf, "N: %05u", val);
			LCD_cursor(1, 0);
			LCD_text(buf);
		}

		uint8_t event = enc_read();
		
		// check encoder
		if (event == ENC_UP) {
				val++;
				update = 1; 
		} else if (event == ENC_DOWN) {
				val--;
				update = 1;
		}

		// check button
		if (!(BUTTONS & _BV(BUT))) {
				done = 1;
		}
	}

	*x = val;
}

static void wait_blink()
{
	uint8_t i;
	for (i = 0; i < 6; i ++) {
		BL_OFF;
		_delay_ms(200);
		BL_ON;
		_delay_ms(200);
	}
}

static void halt_tc_open()
{
	LCD_cursor(0, 0);
	LCD_text(" FATAL! ");
	LCD_cursor(1, 0);
	LCD_text("TC open!");
	while(1) {
		wdt_reset();
		BL_OFF;
		_delay_ms(100);
		BL_ON;
		_delay_ms(100);
	}

}

static void run_reflow(uint8_t mode)
{
	uint16_t temp = 0;
	int16_t target = 30;
	uint16_t pid_out;
	uint8_t done = 0;
	char buf[9];

	pid_init();

	_delay_ms(200);
	wdt_enable(WDTO_2S);
	global_time = 0;

	pwm_set2(255);
	while(done == 0) {

		if (max6675_ready) {
			max6675_ready = 0;
		
			wdt_reset();

			if ((max6675_data == 0) || (max6675_data & MAX6675_TC_OPEN)) {
				halt_tc_open();
				break;
			}

			// convert data to temp in degrees
			temp = MAX6675_TEMP(max6675_data);
			temp = temp / 4;

			if (mode == 0) {
				target = ramp_get(global_time);
				if (target == -1000) {
					done = 1;
					LCD_cursor(1, 0);
					LCD_text("        ");
					LCD_cursor(0, 0);
					LCD_text("DONE !!!");
					break;
				}
			}

			pid_out = pid_do(temp, target);

			// apply new pwm, pidout limit is 4096 scale to 256
			pwm_set1(pid_out >> 4);


			serial_write('0');
			serial_write('x');
			serial_write_hex(global_time >> 8);
			serial_write_hex(global_time& 0xff);
/*			serial_write(' ');
			serial_write('0');
			serial_write('x');
			serial_write_hex(max6675_data >> 8);
			serial_write_hex(max6675_data & 0xff);*/
			serial_write(' ');
			serial_write('0');
			serial_write('x');
			serial_write_hex(temp >> 8);
			serial_write_hex(temp & 0xff);
			serial_write(' ');
			serial_write('0');
			serial_write('x');
			serial_write_hex(target >> 8);
			serial_write_hex(target & 0xff);
			serial_write(' ');
			serial_write('0');
			serial_write('x');
			serial_write_hex(pid_out >> 8);
			serial_write_hex(pid_out & 0xff);
			serial_write('\n');

		}

		if (mode == 1) {
			uint8_t tmp = enc_read();
			if (tmp == ENC_UP) {
				if (target != 0xffff)
					target ++;
			} else if (tmp == ENC_DOWN) {
				if (target > 20)
					target --;
			}
		}

		if (!(BUTTONS & _BV(BUT))) {
			LCD_cursor(0, 0);
			LCD_text(" USER   ");
			LCD_cursor(1, 0);
			LCD_text("ABORT !!");
			break;
		}

		LCD_cursor(0, 0);
		snprintf(buf, 9, "C:  %04d", temp);
		LCD_text(buf);
		LCD_cursor(1, 0);
		snprintf(buf, 9, "T:  %04d", target);
		LCD_text(buf);

	}

	// pwm to zero
	pwm_set1(0);
	pwm_set2(0);
	wdt_disable();
	wait_blink();
}

static void run_ramp1()
{
	ramp_init(0);
	run_reflow(0);
}

static void run_ramp2()
{
	ramp_init(1);
	run_reflow(0);
}

static void run_const_temp()
{
	run_reflow(1);
}

static void run_reset()
{
/*
	void (*reset) (void) = 0;
	reset();
*/

	wdt_reset();
	wdt_enable(WDTO_500MS); 

	while(1);
}

static void run_load_selected_profile()
{
	if (menu_selected_profile > 1)
		menu_selected_profile = 1;

	ramp_load(menu_selected_profile);
}

static void run_save_selected_profile()
{
	if (menu_selected_profile > 1)
		return;

	ramp_save(menu_selected_profile);
}

static void run_edit_ramp_time()
{
	if (menu_selected_parameter > 9)
		return;
	
	menu_value_i16(&ramp_coef[menu_selected_parameter][0]);
}

static void run_edit_ramp_temp()
{
	if (menu_selected_parameter > 9)
		return;
	
	menu_value_i16(&ramp_coef[menu_selected_parameter][1]);
}

static void run_reset_pid()
{
	pid_load_default();
	pid_save();
}

static void run_reset_ramp1()
{
	ramp_load_default();
	ramp_save(0);
}

static void run_reset_ramp2()
{
	ramp_load_default();
	ramp_save(1);
}

