

#ifndef _TCA8418_KEYPAD_H
#define _TCA8418_KEYPAD_H

#include <linux/types.h>
#include <linux/input/matrix_keypad.h>

#define TCA8418_I2C_ADDR	0x34
#define	TCA8418_NAME		"tca8418_keypad"

struct tca8418_keypad_platform_data {
	const struct matrix_keymap_data *keymap_data;
	unsigned rows;
	unsigned cols;
	bool rep;
	bool irq_is_gpio;
};

#endif
