
#include <balongv7r2/types.h>
#include "product_config.h"
#include <boot/boot.h>
#include <balongv7r2/pmu.h>
#include <balongv7r2/parameters.h>
#include <boot/flash.h>
#include "bsp_nandc.h"
#include "ptable_com.h"
#include <balongv7r2/gpio.h>

#ifndef _LCD_H_
#define _LCD_H_

#define LCD_ERROR  -1
#define LCD_OK 0

#define LCD_POWEROFF LCD_ERROR
#define LCD_POWERON LCD_OK

/*系统控制器 xuc*/

#define V7R2_SC_VA_BASE      0x90000000



#define LCD_ILITEK				0x2		/*天马LCD*/
#define LCD_CHIMEI				0x0		/*奇美LCD*/

#ifdef CONFIG_FB_2_4_INCH_BALONG
#define  LCD_HIGHT   (320 - 1)
#define  LCD_WIDTH   (240 - 1)

#define LCD_INIT_X0 0
#define LCD_INIT_Y0 0
#define LCD_INIT_X1 127
#define LCD_INIT_Y1 63

#define CHAR_WIDTH (8)
#define CHAR_HIGHT (16)


#elif defined CONFIG_FB_1_4_5_INCH_BALONG
#define  LCD_HIGHT   (128 - 1)
#define  LCD_WIDTH   (128 - 1)

#define LCD_INIT_X0 0
#define LCD_INIT_Y0 32
#define LCD_INIT_X1 127
#define LCD_INIT_Y1 95

#define CHAR_WIDTH (8)
#define CHAR_HIGHT (16)


#endif

#define BPP 2


#define INIT_LOGO_OFFSET    0    /*开机logo在flash分区中的位置偏移量*/
#define INIT_LOGO_SIZE     ((LCD_WIDTH + 1) * (LCD_HIGHT + 1) *2)

/*LCD命令字*/
#define COLUMN_ADDRESS_SET 0x2a
#define PAGE_ADDRESS_SET   0x2b
#define MEMORY_WRITE       0x2c


struct balong_lcd_seq {
	unsigned char		format;
	unsigned char		value;
};



s32 lcd_warm_up(void);
s32 lcd_power_on(void);
s32 lcd_init_image(int offset);
void lcd_show_string(unsigned char * str);
void lcd_string_display(u16 ucX, u16 ucY, unsigned char *pucStr);


#endif

