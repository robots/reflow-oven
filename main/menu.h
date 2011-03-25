#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>

#define MENU_MAX_DEPTH 3

#define MENU_ARRAY_SIZE(x) (sizeof(x)/sizeof(menuitem_t))

enum {
	MENU_LAST=0,
	MENU_BACK,
	MENU_LABEL,
	MENU_SUB,
	MENU_EXEC,
	MENU_VAL_U16,
	MENU_VAL_I16,
};

struct menuitem_t {
	const char *text1;
	const char *text2;
	uint8_t type;
	void (*func)(void);
	void *p;
};

void menu_do();
void menu_redraw();

#endif

