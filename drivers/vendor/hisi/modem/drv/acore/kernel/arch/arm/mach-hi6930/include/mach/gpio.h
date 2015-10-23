/* arch/arm/mach-balong/include/mach/gpio.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * BALONGV7R2 - GPIO lib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#ifndef	__BALONG_GPIO_H
#define	__BALONG_GPIO_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/types.h>
#include "bsp_memmap.h"

#define gpio_get_value	__gpio_get_value
#define gpio_set_value	__gpio_set_value
#define gpio_cansleep	__gpio_cansleep
#define gpio_to_irq		__gpio_to_irq

#define GPIO_MAX_PINS 	              (32)

#ifdef HI_GPIO4_REGBASE_ADDR
#define GPIO_MAX_BANK_NUM	          (6)
#define ARCH_NR_GPIOS	              (6 * 32)
#define GPIO_DPM_BANK_NUM	          (4)
#else
#define GPIO_MAX_BANK_NUM	          (4)

/*for v711*/
//#define ARCH_NR_GPIOS	              (160)

#define ARCH_NR_GPIOS	              (4 * 32)
#define GPIO_DPM_BANK_NUM	          (3)
#endif
#define GPIO_DPM_REG_NUM	          (7)

/*gpio number*/
#define GPIO_0_0	                  (0)
#define GPIO_0_1	                  (1)
#define GPIO_0_2	                  (2)
#define GPIO_0_3	                  (3)
#define GPIO_0_4	                  (4)
#define GPIO_0_5	                  (5)
#define GPIO_0_6	                  (6)
#define GPIO_0_7	                  (7)
#define GPIO_0_8	                  (8)
#define GPIO_0_9	                  (9)
#define GPIO_0_10	                  (10)
#define GPIO_0_11	                  (11)
#define GPIO_0_12					  (12)
#define GPIO_0_13	                  (13)
#define GPIO_0_14	                  (14)
#define GPIO_0_15	                  (15)
#define GPIO_0_16	                  (16)
#define GPIO_0_17	                  (17)
#define GPIO_0_18	                  (18)
#define GPIO_0_19	                  (19)
#define GPIO_0_20	                  (20)
#define GPIO_0_21	                  (21)
#define GPIO_0_22	                  (22)
#define GPIO_0_23	                  (23)
#define GPIO_0_24	                  (24)
#define GPIO_0_25	                  (25)
#define GPIO_0_26	                  (26)
#define GPIO_0_27	                  (27)
#define GPIO_0_28	                  (28)
#define GPIO_0_29					  (29)
#define GPIO_0_30	                  (30)
#define GPIO_0_31	                  (31)

#define GPIO_1_0	                  (32)
#define GPIO_1_1	                  (33)
#define GPIO_1_2	                  (34)
#define GPIO_1_3	                  (35)
#define GPIO_1_4	                  (36)
#define GPIO_1_5	                  (37)
#define GPIO_1_6	                  (38)
#define GPIO_1_7	                  (39)
#define GPIO_1_8	                  (40)
#define GPIO_1_9	                  (41)
#define GPIO_1_10	                  (42)
#define GPIO_1_11	                  (43)
#define GPIO_1_12					  (44)
#define GPIO_1_13	                  (45)
#define GPIO_1_14	                  (46)
#define GPIO_1_15	                  (47)
#define GPIO_1_16	                  (48)
#define GPIO_1_17	                  (49)
#define GPIO_1_18	                  (50)
#define GPIO_1_19	                  (51)
#define GPIO_1_20	                  (52)
#define GPIO_1_21	                  (53)
#define GPIO_1_22	                  (54)
#define GPIO_1_23	                  (55)
#define GPIO_1_24	                  (56)
#define GPIO_1_25	                  (57)
#define GPIO_1_26	                  (58)
#define GPIO_1_27	                  (59)
#define GPIO_1_28	                  (60)
#define GPIO_1_29					  (61)
#define GPIO_1_30	                  (62)
#define GPIO_1_31	                  (63)

#define GPIO_2_0	                  (64)
#define GPIO_2_1	                  (65)
#define GPIO_2_2	                  (66)
#define GPIO_2_3	                  (67)
#define GPIO_2_4	                  (68)
#define GPIO_2_5	                  (69)
#define GPIO_2_6	                  (70)
#define GPIO_2_7	                  (71)
#define GPIO_2_8	                  (72)
#define GPIO_2_9	                  (73)
#define GPIO_2_10	                  (74)
#define GPIO_2_11	                  (75)
#define GPIO_2_12					  (76)
#define GPIO_2_13	                  (77)
#define GPIO_2_14	                  (78)
#define GPIO_2_15	                  (79)
#define GPIO_2_16	                  (80)
#define GPIO_2_17	                  (81)
#define GPIO_2_18	                  (82)
#define GPIO_2_19	                  (83)
#define GPIO_2_20	                  (84)
#define GPIO_2_21	                  (85)
#define GPIO_2_22	                  (86)
#define GPIO_2_23	                  (87)
#define GPIO_2_24	                  (88)
#define GPIO_2_25	                  (89)
#define GPIO_2_26	                  (90)
#define GPIO_2_27	                  (91)
#define GPIO_2_28	                  (92)
#define GPIO_2_29					  (93)
#define GPIO_2_30	                  (94)
#define GPIO_2_31	                  (95)

#define GPIO_3_0	                  (96)
#define GPIO_3_1	                  (97)
#define GPIO_3_2	                  (98)
#define GPIO_3_3	                  (99)
#define GPIO_3_4	                  (100)
#define GPIO_3_5	                  (101)
#define GPIO_3_6	                  (102)
#define GPIO_3_7	                  (103)
#define GPIO_3_8	                  (104)
#define GPIO_3_9	                  (105)
#define GPIO_3_10	                  (106)
#define GPIO_3_11	                  (107)
#define GPIO_3_12					  (108)
#define GPIO_3_13	                  (109)
#define GPIO_3_14	                  (110)
#define GPIO_3_15	                  (111)
#define GPIO_3_16	                  (112)
#define GPIO_3_17	                  (113)
#define GPIO_3_18	                  (114)
#define GPIO_3_19	                  (115)
#define GPIO_3_20	                  (116)
#define GPIO_3_21	                  (117)
#define GPIO_3_22	                  (118)
#define GPIO_3_23	                  (119)
#define GPIO_3_24	                  (120)
#define GPIO_3_25	                  (121)
#define GPIO_3_26	                  (122)
#define GPIO_3_27	                  (123)
#define GPIO_3_28	                  (124)
#define GPIO_3_29					  (125)
#define GPIO_3_30	                  (126)
#define GPIO_3_31	                  (127)

#define GPIO_4_0	                  (128)
#define GPIO_4_1	                  (129)
#define GPIO_4_2	                  (130)
#define GPIO_4_3	                  (131)
#define GPIO_4_4	                  (132)
#define GPIO_4_5	                  (133)
#define GPIO_4_6	                  (134)
#define GPIO_4_7	                  (135)
#define GPIO_4_8	                  (136)
#define GPIO_4_9	                  (137)
#define GPIO_4_10	                  (138)
#define GPIO_4_11	                  (139)
#define GPIO_4_12					  (140)
#define GPIO_4_13	                  (141)
#define GPIO_4_14	                  (142)
#define GPIO_4_15	                  (143)
#define GPIO_4_16	                  (144)
#define GPIO_4_17	                  (145)
#define GPIO_4_18	                  (146)
#define GPIO_4_19	                  (147)
#define GPIO_4_20	                  (148)
#define GPIO_4_21	                  (159)
#define GPIO_4_22	                  (150)
#define GPIO_4_23	                  (151)
#define GPIO_4_24	                  (152)
#define GPIO_4_25	                  (153)
#define GPIO_4_26	                  (154)
#define GPIO_4_27	                  (155)
#define GPIO_4_28	                  (156)
#define GPIO_4_29					  (157)
#define GPIO_4_30	                  (158)
#define GPIO_4_31	                  (159)

#define GPIO_5_0	                  (160)
#define GPIO_5_1	                  (161)
#define GPIO_5_2	                  (162)
#define GPIO_5_3	                  (163)
#define GPIO_5_4	                  (164)
#define GPIO_5_5	                  (165)
#define GPIO_5_6	                  (166)
#define GPIO_5_7	                  (167)
#define GPIO_5_8	                  (168)
#define GPIO_5_9	                  (169)
#define GPIO_5_10	                  (170)
#define GPIO_5_11	                  (171)
#define GPIO_5_12					  (172)
#define GPIO_5_13	                  (173)
#define GPIO_5_14	                  (174)
#define GPIO_5_15	                  (175)
#define GPIO_5_16	                  (176)
#define GPIO_5_17	                  (177)
#define GPIO_5_18	                  (178)
#define GPIO_5_19	                  (179)
#define GPIO_5_20	                  (180)
#define GPIO_5_21	                  (181)
#define GPIO_5_22	                  (182)
#define GPIO_5_23	                  (183)
#define GPIO_5_24	                  (184)
#define GPIO_5_25	                  (185)
#define GPIO_5_26	                  (186)
#define GPIO_5_27	                  (187)
#define GPIO_5_28	                  (188)
#define GPIO_5_29					  (189)
#define GPIO_5_30	                  (190)
#define GPIO_5_31	                  (191)

#define GPIO_OK        0
#define GPIO_ERROR     -1


/* GPIO TLMM: Function -- GPIO specific */

/* GPIO TLMM: Direction */
enum {
	GPIO_INPUT = 0,
	GPIO_OUTPUT = 1,
};

/* GPIO TLMM: Pullup/Pulldown */
enum {
	GPIO_NO_PULL,
	GPIO_PULL_DOWN,
	GPIO_KEEPER,
	GPIO_PULL_UP,
};

/*GPIO function*/
enum{
	GPIO_NORMAL= 0,
	GPIO_INTERRUPT = 1,
};

/*interrupt mask*/
enum {
	GPIO_INT_ENABLE = 0,
	GPIO_INT_DISABLE = 1,
};

/*interrupt level trigger*/
enum{
	GPIO_INT_TYPE_LEVEVL = 0,
	GPIO_INT_TYPE_EDGE = 1,
};

/*interrupt polarity*/
enum{
	GPIO_INT_POLARITY_FAL_LOW = 0,
	GPIO_INT_POLARITY_RIS_HIGH = 1,
};

/*************************************************
  Function:       gpio_direction_get
  Description:    Get GPIO Direction value,
  				  include GPIO_INPUT and GPIO_OUTPUT
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio, such as GPIO_*_**

  Return:         GPIO_INPUT, GPIO_OUTPUT
  Others:
************************************************/
extern int gpio_direction_get(unsigned gpio);

/*************************************************
  Function:       gpio_set_function
  Description:    Set GPIO function
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio, int mode, include
  	              GPIO_NORMAL,
				  GPIO_INTERRUPT,

  Return:         NONE
  Others:
************************************************/
extern void gpio_set_function(unsigned gpio, int mode);

/*************************************************
  Function:       gpio_int_mask_set
  Description:    Mask GPIO interrupt
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio, include GPIO_INT_ENABLE
  				  and GPIO_INT_DISABLE
  Return:         NONE
  Others:mask bit refer to GPIO_INT_ENABLE, GPIO_INT_DISABLE
************************************************/
extern void gpio_int_mask_set(unsigned gpio);

/*************************************************
  Function:       gpio_int_unmask_set
  Description:    unMask GPIO interrupt
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio

  Return:        NONE
  Others:mask bit refer to GPIO_INT_ENABLE, GPIO_INT_DISABLE
************************************************/
extern void gpio_int_unmask_set(unsigned gpio);

/*************************************************
  Function:       gpio_int_state_clear
  Description:    get GPIO interrupt status
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio

  Return:         NONE
  Others:
************************************************/
extern void gpio_int_state_clear(unsigned gpio);

/*************************************************
  Function:       gpio_int_state_get
  Description:    get GPIO interrupt  status
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio

  Return:         NONE
  Others:
************************************************/
extern int gpio_int_state_get(unsigned gpio);

/*************************************************
  Function:       gpio_raw_int_state_get
  Description:    Clear GPIO raw interrupt
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio

  Return:         NONE
  Others:clear refer to
************************************************/
extern int gpio_raw_int_state_get(unsigned gpio);

/*************************************************
  Function:       gpio_int_trigger_set
  Description:    set GPIO Interrupt Triger style
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned gpio, int trigger_type
				  IRQ_TYPE_EDGE_RISING,
				  IRQ_TYPE_EDGE_FALLING,
				  IRQ_TYPE_LEVEL_HIGH,
				  IRQ_TYPE_LEVEL_LOW
  Output:         NONE
  Return:         OSAL_OK : successfully
                  OSAL_ERROR: fail
  Others:
************************************************/
extern void gpio_int_trigger_set(unsigned gpio, int trigger_type);

#include <asm-generic/gpio.h>
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif
