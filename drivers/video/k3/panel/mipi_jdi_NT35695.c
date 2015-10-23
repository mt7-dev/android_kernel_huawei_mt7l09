/* Copyright (c) 2008-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/

#include "k3_fb.h"
#include "k3_mipi_dsi.h"
#include <linux/lcd_tuning.h>

#include <linux/of.h>
#include <linux/log_jank.h>
#include <linux/huawei/hisi_adc.h>

#define DTS_COMP_JDI_NT35695 "hisilicon,mipi_jdi_NT35695"
static struct k3_fb_panel_data jdi_panel_data;
static bool fastboot_display_enable = true;
extern u32 frame_count;
static struct k3_fb_data_type *g_k3fd = NULL;
extern unsigned char (*get_tp_color)(void);
static bool debug_enable = false;
static int hkadc_buf = 0;
extern bool gesture_func;

/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
static char caset_data[] = {
	0x2A,
	0x00,0x00,0x04, 0x37,
};

static char paset_data[] = {
	0x2B,
	0x00,0x00,0x07,0x7f,
};

static char tear_on[] = {
	0x35,
	0x00,
};

static char display_on[] = {
	0x29,
};

static char exit_sleep[] = {
	0x11,
};

static char bl_enable[] = {
	0x53,
	0x24,
};

static char te_line[] = {
	0x44,
	0x03, 0x80,
};

/*******************************************************************************
** cmd3 data for initialization
*/
static char set_pagef0[] = {
	0xFF,
	0xF0,
};

static char cmd3_0xdd_data[] = {
	0xDD,
	0x02,
};

static char cmd3_0xe3_data[] = {
	0xE3,
	0x00,
};

static char cmd3_0xfb_data[] = {
	0xFB,
	0x01,
};

/*******************************************************************************
** setting PWM frequency to 58KHZ
*/

static char set_page3[] = {
	0xFF,
	0x23,
};

static char set_pwm_freq[] = {
	0x08,
	0x03,
};

static char set_page0[] = {
	0xFF,
	0x10,
};

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char bl_disable[] = {
	0x53,
	0x00,
};

static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};
/*******************************************************************************
** Display Effect Sequence(smart color, edge enhancement, smart contrast, cabc)
*/
//CMD2 Page2
static char cmd2_page2_0xFF[] = {
    0xFF,
    0x22,
};
//Non-reload
static char non_reload_0xFB[] = {
    0xFB,
    0x01,
};
//Color Enhancement Coefficient
static char color_enhancement_0x00[] = {
    0x00,
    0x0A,
};
static char color_enhancement_0x01[] = {
    0x01,
    0x0F,
};
static char color_enhancement_0x02[] = {
    0x02,
    0x0A,
};
static char color_enhancement_0x03[] = {
    0x03,
    0x0F,
};
static char color_enhancement_0x04[] = {
    0x04,
    0x14,
};
static char color_enhancement_0x05[] = {
    0x05,
    0x19,
};
static char color_enhancement_0x06[] = {
    0x06,
    0x14,
};
static char color_enhancement_0x07[] = {
    0x07,
    0x0F,
};
static char color_enhancement_0x08[] = {
    0x08,
    0x0A,
};
static char color_enhancement_0x09[] = {
    0x09,
    0x05,
};
static char color_enhancement_0x0A[] = {
    0x0A,
    0x0A,
};
static char color_enhancement_0x0B[] = {
    0x0B,
    0x0F,
};
static char color_enhancement_0x0C[] = {
    0x0C,
    0x0A,
};
static char color_enhancement_0x0D[] = {
    0x0D,
    0x0A,
};
static char color_enhancement_0x0E[] = {
    0x0E,
    0x14,
};
static char color_enhancement_0x0F[] = {
    0x0F,
    0x0A,
};
static char color_enhancement_0x10[] = {
    0x10,
    0x0F,
};
static char color_enhancement_0x11[] = {
    0x11,
    0x00,
};
static char color_enhancement_0x12[] = {
    0x12,
    0x00,
};
static char color_enhancement_0x13[] = {
    0x13,
    0x00,
};
static char color_enhancement_0x14[] = {
    0x14,
    0x00,
};
static char color_enhancement_0x1A[] = {
    0x1A,
    0x00,
};
static char color_enhancement_0x1B[] = {
    0x1B,
    0x00,
};
static char color_enhancement_0x1C[] = {
    0x1C,
    0x04,
};

static char color_enhancement_0x1D[] = {
    0x1D,
    0x08,
};
static char color_enhancement_0x1E[] = {
    0x1E,
    0x0C,
};
static char color_enhancement_0x1F[] = {
    0x1F,
    0x10,
};
static char color_enhancement_0x20[] = {
    0x20,
    0x14,
};
static char color_enhancement_0x21[] = {
    0x21,
    0x18,
};
static char color_enhancement_0x22[] = {
    0x22,
    0x20,
};
static char color_enhancement_0x23[] = {
    0x23,
    0x18,
};
static char color_enhancement_0x24[] = {
    0x24,
    0x10,
};
static char color_enhancement_0x25[] = {
    0x25,
    0x08,
};
static char color_enhancement_0x26[] = {
    0x26,
    0x00,
};
static char color_enhancement_0x27[] = {
    0x27,
    0x00,
};
static char color_enhancement_0x28[] = {
    0x28,
    0x00,
};
static char color_enhancement_0x29[] = {
    0x29,
    0x00,
};
static char color_enhancement_0x2A[] = {
    0x2A,
    0x00,
};

static char color_enhancement_0x2B[] = {
    0x2B,
    0x00,
};

static char color_enhancement_0x2F[] = {
    0x2F,
    0x00,
};

static char color_enhancement_0x30[] = {
    0x30,
    0x00,
};
static char color_enhancement_0x31[] = {
    0x31,
    0x00,
};
static char color_enhancement_0x32[] = {
    0x32,
    0x19,
};
static char color_enhancement_0x33[] = {
    0x33,
    0x17,
};
static char color_enhancement_0x34[] = {
    0x34,
    0x19,
};
static char color_enhancement_0x35[] = {
    0x35,
    0x19,
};
static char color_enhancement_0x36[] = {
    0x36,
    0x19,
};
static char color_enhancement_0x37[] = {
    0x37,
    0x17,
};
static char color_enhancement_0x38[] = {
    0x38,
    0x14,
};
static char color_enhancement_0x39[] = {
    0x39,
    0x12,
};
static char color_enhancement_0x3A[] = {
    0x3A,
    0x0F,
};
static char color_enhancement_0x3B[] = {
    0x3B,
    0x12,
};
static char color_enhancement_0x3F[] = {
    0x3F,
    0x12,
};
static char color_enhancement_0x40[] = {
    0x40,
    0x14,
};
static char color_enhancement_0x41[] = {
    0x41,
    0x14,
};
static char color_enhancement_0x42[] = {
    0x42,
    0x17,
};
static char color_enhancement_0x43[] = {
    0x43,
    0x19,
};
static char color_enhancement_0x44[] = {
    0x44,
    0x1C,
};
static char color_enhancement_0x45[] = {
    0x45,
    0x1E,
};
static char color_enhancement_0x46[] = {
    0x46,
    0x21,
};
static char color_enhancement_0x47[] = {
    0x47,
    0x1E,
};
static char color_enhancement_0x48[] = {
    0x48,
    0x1C,
};
static char color_enhancement_0x49[] = {
    0x49,
    0x19,
};
static char color_enhancement_0x4A[] = {
    0x4A,
    0x17,
};
static char color_enhancement_0x4B[] = {
    0x4B,
    0x19,
};
static char color_enhancement_0x4C[] = {
    0x4C,
    0x99,
};

static char color_enhancement_0x4D[] = {
    0x4D,
    0x00,
};
static char color_enhancement_0x53[] = {
    0x53,
    0x01,
};
static char color_enhancement_0x54[] = {
    0x54,
    0x00,
};
static char color_enhancement_0x55[] = {
    0x55,
    0x00,
};
static char color_enhancement_0x56[] = {
    0x56,
    0x00,
};
static char color_enhancement_0x58[] = {
    0x58,
    0x12,
};
static char color_enhancement_0x59[] = {
    0x59,
    0x0E,
};
static char color_enhancement_0x5A[] = {
    0x5A,
    0x14,
};
static char color_enhancement_0x5B[] = {
    0x5B,
    0x0E,
};
static char color_enhancement_0x5C[] = {
    0x5C,
    0x16,
};
static char color_enhancement_0x5D[] = {
    0x5D,
    0x0E,
};
static char color_enhancement_0x5E[] = {
    0x5E,
    0x08,
};
static char color_enhancement_0x5F[] = {
    0x5F,
    0x18,
};
static char color_enhancement_0x60[] = {
    0x60,
    0x06,
};
static char color_enhancement_0x61[] = {
    0x61,
    0x1A,
};
static char color_enhancement_0x62[] = {
    0x62,
    0x04,
};
static char color_enhancement_0x63[] = {
    0x63,
    0x1C,
};
static char color_enhancement_0x64[] = {
    0x64,
    0x2F,
};
static char color_enhancement_0x65[] = {
    0x65,
    0x00,
};
static char color_enhancement_0x66[] = {
    0x66,
    0xA8,
};
static char color_enhancement_0x67[] = {
    0x67,
    0xF8,
};
static char color_enhancement_0x68[] = {
    0x68,
    0x00,
};
static char color_enhancement_0x69[] = {
    0x69,
    0x12,
};
static char color_enhancement_0x97[] = {
    0x97,
    0x00,
};
static char color_enhancement_0x98[] = {
    0x98,
    0x10,
};
static char dither_enable_0xA2[] = {
    0xA2,
    0x20,
};
//page selection cmd start
static char page_selection_0xFF_0x20[] = {
    0xFF,
    0x20,
};
static char page_selection_0xFB_0x01[] = {
    0xFB,
    0x01,
};
//R(+) MCR cmd
static char gamma_r_positive_0x75[] = {
    0x75,
    0x00,
};
static char gamma_r_positive_0x76[] = {
    0x76,
    0x01,
};
static char gamma_r_positive_0x77[] = {
    0x77,
    0x00,
};
static char gamma_r_positive_0x78[] = {
    0x78,
    0x02,
};
static char gamma_r_positive_0x79[] = {
    0x79,
    0x00,
};
static char gamma_r_positive_0x7A[] = {
    0x7A,
    0x0F,
};
static char gamma_r_positive_0x7B[] = {
    0x7B,
    0x00,
};
static char gamma_r_positive_0x7C[] = {
    0x7C,
    0x44,
};
static char gamma_r_positive_0x7D[] = {
    0x7D,
    0x00,
};
static char gamma_r_positive_0x7E[] = {
    0x7E,
    0x66,
};
static char gamma_r_positive_0x7F[] = {
    0x7F,
    0x00,
};
static char gamma_r_positive_0x80[] = {
    0x80,
    0x80,
};
static char gamma_r_positive_0x81[] = {
    0x81,
    0x00,
};
static char gamma_r_positive_0x82[] = {
    0x82,
    0xA0,
};
static char gamma_r_positive_0x83[] = {
    0x83,
    0x00,
};
static char gamma_r_positive_0x84[] = {
    0x84,
    0xBB,
};
static char gamma_r_positive_0x85[] = {
    0x85,
    0x00,
};
static char gamma_r_positive_0x86[] = {
    0x86,
    0xCD,
};
static char gamma_r_positive_0x87[] = {
    0x87,
    0x01,
};
static char gamma_r_positive_0x88[] = {
    0x88,
    0x16,
};
static char gamma_r_positive_0x89[] = {
    0x89,
    0x01,
};
static char gamma_r_positive_0x8A[] = {
    0x8A,
    0x49,
};
static char gamma_r_positive_0x8B[] = {
    0x8B,
    0x01,
};
static char gamma_r_positive_0x8C[] = {
    0x8C,
    0x94,
};
static char gamma_r_positive_0x8D[] = {
    0x8D,
    0x01,
};
static char gamma_r_positive_0x8E[] = {
    0x8E,
    0xCA,
};
static char gamma_r_positive_0x8F[] = {
    0x8F,
    0x02,
};
static char gamma_r_positive_0x90[] = {
    0x90,
    0x19,
};
static char gamma_r_positive_0x91[] = {
    0x91,
    0x02,
};
static char gamma_r_positive_0x92[] = {
    0x92,
    0x59,
};
static char gamma_r_positive_0x93[] = {
    0x93,
    0x02,
};
static char gamma_r_positive_0x94[] = {
    0x94,
    0x5A,
};
static char gamma_r_positive_0x95[] = {
    0x95,
    0x02,
};
static char gamma_r_positive_0x96[] = {
    0x96,
    0x95,
};
static char gamma_r_positive_0x97[] = {
    0x97,
    0x02,
};
static char gamma_r_positive_0x98[] = {
    0x98,
    0xD3,
};
static char gamma_r_positive_0x99[] = {
    0x99,
    0x02,
};
static char gamma_r_positive_0x9A[] = {
    0x9A,
    0xF9,
};
static char gamma_r_positive_0x9B[] = {
    0x9B,
    0x03,
};
static char gamma_r_positive_0x9C[] = {
    0x9C,
    0x28,
};
static char gamma_r_positive_0x9D[] = {
    0x9D,
    0x03,
};
static char gamma_r_positive_0x9E[] = {
    0x9E,
    0x45,
};
static char gamma_r_positive_0x9F[] = {
    0x9F,
    0x03,
};
static char gamma_r_positive_0xA0[] = {
    0xA0,
    0x6A,
};
static char gamma_r_positive_0xA2[] = {
    0xA2,
    0x03,
};
static char gamma_r_positive_0xA3[] = {
    0xA3,
    0x74,
};
static char gamma_r_positive_0xA4[] = {
    0xA4,
    0x03,
};
static char gamma_r_positive_0xA5[] = {
    0xA5,
    0x81,
};
static char gamma_r_positive_0xA6[] = {
    0xA6,
    0x03,
};
static char gamma_r_positive_0xA7[] = {
    0xA7,
    0x8C,
};
static char gamma_r_positive_0xA9[] = {
    0xA9,
    0x03,
};
static char gamma_r_positive_0xAA[] = {
    0xAA,
    0x9A,
};
static char gamma_r_positive_0xAB[] = {
    0xAB,
    0x03,
};
static char gamma_r_positive_0xAC[] = {
    0xAC,
    0xB6,
};
static char gamma_r_positive_0xAD[] = {
    0xAD,
    0x03,
};
static char gamma_r_positive_0xAE[] = {
    0xAE,
    0xD3,
};
static char gamma_r_positive_0xAF[] = {
    0xAF,
    0x03,
};
static char gamma_r_positive_0xB0[] = {
    0xB0,
    0xF0,
};
static char gamma_r_positive_0xB1[] = {
    0xB1,
    0x03,
};
static char gamma_r_positive_0xB2[] = {
    0xB2,
    0xFF,
};
//R(-) MCR cmd
static char gamma_r_negative_0xB3[] = {
    0xB3,
    0x00,
};
static char gamma_r_negative_0xB4[] = {
    0xB4,
    0x01,
};
static char gamma_r_negative_0xB5[] = {
    0xB5,
    0x00,
};
static char gamma_r_negative_0xB6[] = {
    0xB6,
    0x02,
};
static char gamma_r_negative_0xB7[] = {
    0xB7,
    0x00,
};
static char gamma_r_negative_0xB8[] = {
    0xB8,
    0x0F,
};
static char gamma_r_negative_0xB9[] = {
    0xB9,
    0x00,
};
static char gamma_r_negative_0xBA[] = {
    0xBA,
    0x44,
};
static char gamma_r_negative_0xBB[] = {
    0xBB,
    0x00,
};
static char gamma_r_negative_0xBC[] = {
    0xBC,
    0x66,
};
static char gamma_r_negative_0xBD[] = {
    0xBD,
    0x00,
};
static char gamma_r_negative_0xBE[] = {
    0xBE,
    0x80,
};
static char gamma_r_negative_0xBF[] = {
    0xBF,
    0x00,
};
static char gamma_r_negative_0xC0[] = {
    0xC0,
    0xA0,
};
static char gamma_r_negative_0xC1[] = {
    0xC1,
    0x00,
};
static char gamma_r_negative_0xC2[] = {
    0xC2,
    0xBB,
};
static char gamma_r_negative_0xC3[] = {
    0xC3,
    0x00,
};
static char gamma_r_negative_0xC4[] = {
    0xC4,
    0xCD,
};
static char gamma_r_negative_0xC5[] = {
    0xC5,
    0x01,
};
static char gamma_r_negative_0xC6[] = {
    0xC6,
    0x16,
};
static char gamma_r_negative_0xC7[] = {
    0xC7,
    0x01,
};
static char gamma_r_negative_0xC8[] = {
    0xC8,
    0x49,
};
static char gamma_r_negative_0xC9[] = {
    0xC9,
    0x01,
};
static char gamma_r_negative_0xCA[] = {
    0xCA,
    0x94,
};
static char gamma_r_negative_0xCB[] = {
    0xCB,
    0x01,
};
static char gamma_r_negative_0xCC[] = {
    0xCC,
    0xCA,
};
static char gamma_r_negative_0xCD[] = {
    0xCD,
    0x02,
};
static char gamma_r_negative_0xCE[] = {
    0xCE,
    0x19,
};
static char gamma_r_negative_0xCF[] = {
    0xCF,
    0x02,
};
static char gamma_r_negative_0xD0[] = {
    0xD0,
    0x59,
};
static char gamma_r_negative_0xD1[] = {
    0xD1,
    0x02,
};
static char gamma_r_negative_0xD2[] = {
    0xD2,
    0x5A,
};
static char gamma_r_negative_0xD3[] = {
    0xD3,
    0x02,
};
static char gamma_r_negative_0xD4[] = {
    0xD4,
    0x95,
};
static char gamma_r_negative_0xD5[] = {
    0xD5,
    0x02,
};
static char gamma_r_negative_0xD6[] = {
    0xD6,
    0xD3,
};
static char gamma_r_negative_0xD7[] = {
    0xD7,
    0x02,
};
static char gamma_r_negative_0xD8[] = {
    0xD8,
    0xF9,
};
static char gamma_r_negative_0xD9[] = {
    0xD9,
    0x03,
};
static char gamma_r_negative_0xDA[] = {
    0xDA,
    0x28,
};
static char gamma_r_negative_0xDB[] = {
    0xDB,
    0x03,
};
static char gamma_r_negative_0xDC[] = {
    0xDC,
    0x45,
};
static char gamma_r_negative_0xDD[] = {
    0xDD,
    0x03,
};
static char gamma_r_negative_0xDE[] = {
    0xDE,
    0x6A,
};
static char gamma_r_negative_0xDF[] = {
    0xDF,
    0x03,
};
static char gamma_r_negative_0xE0[] = {
    0xE0,
    0x74,
};
static char gamma_r_negative_0xE1[] = {
    0xE1,
    0x03,
};
static char gamma_r_negative_0xE2[] = {
    0xE2,
    0x81,
};
static char gamma_r_negative_0xE3[] = {
    0xE3,
    0x03,
};
static char gamma_r_negative_0xE4[] = {
    0xE4,
    0x8C,
};
static char gamma_r_negative_0xE5[] = {
    0xE5,
    0x03,
};
static char gamma_r_negative_0xE6[] = {
    0xE6,
    0x9A,
};
static char gamma_r_negative_0xE7[] = {
    0xE7,
    0x03,
};
static char gamma_r_negative_0xE8[] = {
    0xE8,
    0xB6,
};
static char gamma_r_negative_0xE9[] = {
    0xE9,
    0x03,
};
static char gamma_r_negative_0xEA[] = {
    0xEA,
    0xD3,
};
static char gamma_r_negative_0xEB[] = {
    0xEB,
    0x03,
};
static char gamma_r_negative_0xEC[] = {
    0xEC,
    0xF0,
};
static char gamma_r_negative_0xED[] = {
    0xED,
    0x03,
};
static char gamma_r_negative_0xEE[] = {
    0xEE,
    0xFF,
};
//G(+) MCR cmd
static char gamma_g_positive_0xEF[] = {
    0xEF,
    0x00,
};
static char gamma_g_positive_0xF0[] = {
    0xF0,
    0x50,
};
static char gamma_g_positive_0xF1[] = {
    0xF1,
    0x00,
};
static char gamma_g_positive_0xF2[] = {
    0xF2,
    0x66,
};
static char gamma_g_positive_0xF3[] = {
    0xF3,
    0x00,
};
static char gamma_g_positive_0xF4[] = {
    0xF4,
    0x8D,
};
static char gamma_g_positive_0xF5[] = {
    0xF5,
    0x00,
};
static char gamma_g_positive_0xF6[] = {
    0xF6,
    0xA7,
};
static char gamma_g_positive_0xF7[] = {
    0xF7,
    0x00,
};
static char gamma_g_positive_0xF8[] = {
    0xF8,
    0xBF,
};
static char gamma_g_positive_0xF9[] = {
    0xF9,
    0x00,
};
static char gamma_g_positive_0xFA[] = {
    0xFA,
    0xD5,
};
//page selection cmd start
static char page_selection_0xFF_0x21[] = {
    0xFF,
    0x21,
};
//G(+) MCR cmd
static char gamma_g_positive_0x00[] = {
    0x00,
    0x00,
};
static char gamma_g_positive_0x01[] = {
    0x01,
    0xE8,
};
static char gamma_g_positive_0x02[] = {
    0x02,
    0x00,
};
static char gamma_g_positive_0x03[] = {
    0x03,
    0xF9,
};
static char gamma_g_positive_0x04[] = {
    0x04,
    0x01,
};
static char gamma_g_positive_0x05[] = {
    0x05,
    0x0A,
};
static char gamma_g_positive_0x06[] = {
    0x06,
    0x01,
};
static char gamma_g_positive_0x07[] = {
    0x07,
    0x3F,
};
static char gamma_g_positive_0x08[] = {
    0x08,
    0x01,
};
static char gamma_g_positive_0x09[] = {
    0x09,
    0x69,
};
static char gamma_g_positive_0x0A[] = {
    0x0A,
    0x01,
};
static char gamma_g_positive_0x0B[] = {
    0x0B,
    0xA9,
};
static char gamma_g_positive_0x0C[] = {
    0x0C,
    0x01,
};
static char gamma_g_positive_0x0D[] = {
    0x0D,
    0xDA,
};
static char gamma_g_positive_0x0E[] = {
    0x0E,
    0x02,
};
static char gamma_g_positive_0x0F[] = {
    0x0F,
    0x24,
};
static char gamma_g_positive_0x10[] = {
    0x10,
    0x02,
};
static char gamma_g_positive_0x11[] = {
    0x11,
    0x5F,
};
static char gamma_g_positive_0x12[] = {
    0x12,
    0x02,
};
static char gamma_g_positive_0x13[] = {
    0x13,
    0x61,
};
static char gamma_g_positive_0x14[] = {
    0x14,
    0x02,
};
static char gamma_g_positive_0x15[] = {
    0x15,
    0x99,
};
static char gamma_g_positive_0x16[] = {
    0x16,
    0x02,
};
static char gamma_g_positive_0x17[] = {
    0x17,
    0xD7,
};
static char gamma_g_positive_0x18[] = {
    0x18,
    0x02,
};
static char gamma_g_positive_0x19[] = {
    0x19,
    0xFD,
};
static char gamma_g_positive_0x1A[] = {
    0x1A,
    0x03,
};
static char gamma_g_positive_0x1B[] = {
    0x1B,
    0x2B,
};
static char gamma_g_positive_0x1C[] = {
    0x1C,
    0x03,
};
static char gamma_g_positive_0x1D[] = {
    0x1D,
    0x48,
};
static char gamma_g_positive_0x1E[] = {
    0x1E,
    0x03,
};
static char gamma_g_positive_0x1F[] = {
    0x1F,
    0x6B,
};
static char gamma_g_positive_0x20[] = {
    0x20,
    0x03,
};
static char gamma_g_positive_0x21[] = {
    0x21,
    0x75,
};
static char gamma_g_positive_0x22[] = {
    0x22,
    0x03,
};
static char gamma_g_positive_0x23[] = {
    0x23,
    0x81,
};
static char gamma_g_positive_0x24[] = {
    0x24,
    0x03,
};
static char gamma_g_positive_0x25[] = {
    0x25,
    0x8D,
};
static char gamma_g_positive_0x26[] = {
    0x26,
    0x03,
};
static char gamma_g_positive_0x27[] = {
    0x27,
    0x99,
};
static char gamma_g_positive_0x28[] = {
    0x28,
    0x03,
};
static char gamma_g_positive_0x29[] = {
    0x29,
    0xB6,
};
static char gamma_g_positive_0x2A[] = {
    0x2A,
    0x03,
};
static char gamma_g_positive_0x2B[] = {
    0x2B,
    0xD3,
};
static char gamma_g_positive_0x2D[] = {
    0x2D,
    0x03,
};
static char gamma_g_positive_0x2F[] = {
    0x2F,
    0xF0,
};
static char gamma_g_positive_0x30[] = {
    0x30,
    0x03,
};
static char gamma_g_positive_0x31[] = {
    0x31,
    0xFF,
};
//G(-) MCR cmd
static char gamma_g_negative_0x32[] = {
    0x32,
    0x00,
};
static char gamma_g_negative_0x33[] = {
    0x33,
    0x50,
};
static char gamma_g_negative_0x34[] = {
    0x34,
    0x00,
};
static char gamma_g_negative_0x35[] = {
    0x35,
    0x66,
};
static char gamma_g_negative_0x36[] = {
    0x36,
    0x00,
};
static char gamma_g_negative_0x37[] = {
    0x37,
    0x8D,
};
static char gamma_g_negative_0x38[] = {
    0x38,
    0x00,
};
static char gamma_g_negative_0x39[] = {
    0x39,
    0xA7,
};
static char gamma_g_negative_0x3A[] = {
    0x3A,
    0x00,
};
static char gamma_g_negative_0x3B[] = {
    0x3B,
    0xBF,
};
static char gamma_g_negative_0x3D[] = {
    0x3D,
    0x00,
};
static char gamma_g_negative_0x3F[] = {
    0x3F,
    0xD5,
};
static char gamma_g_negative_0x40[] = {
    0x40,
    0x00,
};
static char gamma_g_negative_0x41[] = {
    0x41,
    0xE8,
};
static char gamma_g_negative_0x42[] = {
    0x42,
    0x00,
};
static char gamma_g_negative_0x43[] = {
    0x43,
    0xF9,
};
static char gamma_g_negative_0x44[] = {
    0x44,
    0x01,
};
static char gamma_g_negative_0x45[] = {
    0x45,
    0x0A,
};
static char gamma_g_negative_0x46[] = {
    0x46,
    0x01,
};
static char gamma_g_negative_0x47[] = {
    0x47,
    0x3F,
};
static char gamma_g_negative_0x48[] = {
    0x48,
    0x01,
};
static char gamma_g_negative_0x49[] = {
    0x49,
    0x69,
};
static char gamma_g_negative_0x4A[] = {
    0x4A,
    0x01,
};
static char gamma_g_negative_0x4B[] = {
    0x4B,
    0xA9,
};
static char gamma_g_negative_0x4C[] = {
    0x4C,
    0x01,
};
static char gamma_g_negative_0x4D[] = {
    0x4D,
    0xDA,
};
static char gamma_g_negative_0x4E[] = {
    0x4E,
    0x02,
};
static char gamma_g_negative_0x4F[] = {
    0x4F,
    0x24,
};
static char gamma_g_negative_0x50[] = {
    0x50,
    0x02,
};
static char gamma_g_negative_0x51[] = {
    0x51,
    0x5F,
};
static char gamma_g_negative_0x52[] = {
    0x52,
    0x02,
};
static char gamma_g_negative_0x53[] = {
    0x53,
    0x61,
};
static char gamma_g_negative_0x54[] = {
    0x54,
    0x02,
};
static char gamma_g_negative_0x55[] = {
    0x55,
    0x99,
};
static char gamma_g_negative_0x56[] = {
    0x56,
    0x02,
};
static char gamma_g_negative_0x58[] = {
    0x58,
    0xD7,
};
static char gamma_g_negative_0x59[] = {
    0x59,
    0x02,
};
static char gamma_g_negative_0x5A[] = {
    0x5A,
    0xFD,
};
static char gamma_g_negative_0x5B[] = {
    0x5B,
    0x03,
};
static char gamma_g_negative_0x5C[] = {
    0x5C,
    0x2B,
};
static char gamma_g_negative_0x5D[] = {
    0x5D,
    0x03,
};
static char gamma_g_negative_0x5E[] = {
    0x5E,
    0x48,
};
static char gamma_g_negative_0x5F[] = {
    0x5F,
    0x03,
};
static char gamma_g_negative_0x60[] = {
    0x60,
    0x6B,
};
static char gamma_g_negative_0x61[] = {
    0x61,
    0x03,
};
static char gamma_g_negative_0x62[] = {
    0x62,
    0x75,
};
static char gamma_g_negative_0x63[] = {
    0x63,
    0x03,
};
static char gamma_g_negative_0x64[] = {
    0x64,
    0x81,
};
static char gamma_g_negative_0x65[] = {
    0x65,
    0x03,
};
static char gamma_g_negative_0x66[] = {
    0x66,
    0x8D,
};
static char gamma_g_negative_0x67[] = {
    0x67,
    0x03,
};
static char gamma_g_negative_0x68[] = {
    0x68,
    0x99,
};
static char gamma_g_negative_0x69[] = {
    0x69,
    0x03,
};
static char gamma_g_negative_0x6A[] = {
    0x6A,
    0xB6,
};
static char gamma_g_negative_0x6B[] = {
    0x6B,
    0x03,
};
static char gamma_g_negative_0x6C[] = {
    0x6C,
    0xD3,
};
static char gamma_g_negative_0x6D[] = {
    0x6D,
    0x03,
};
static char gamma_g_negative_0x6E[] = {
    0x6E,
    0xF0,
};
static char gamma_g_negative_0x6F[] = {
    0x6F,
    0x03,
};
static char gamma_g_negative_0x70[] = {
    0x70,
    0xFF,
};
//B(+) MCR cmd
static char gamma_b_positive_0x71[] = {
    0x71,
    0x00,
};
static char gamma_b_positive_0x72[] = {
    0x72,
    0x96,
};
static char gamma_b_positive_0x73[] = {
    0x73,
    0x00,
};
static char gamma_b_positive_0x74[] = {
    0x74,
    0x98,
};
static char gamma_b_positive_0x75[] = {
    0x75,
    0x00,
};
static char gamma_b_positive_0x76[] = {
    0x76,
    0xB9,
};
static char gamma_b_positive_0x77[] = {
    0x77,
    0x00,
};
static char gamma_b_positive_0x78[] = {
    0x78,
    0xD2,
};
static char gamma_b_positive_0x79[] = {
    0x79,
    0x00,
};
static char gamma_b_positive_0x7A[] = {
    0x7A,
    0xE7,
};
static char gamma_b_positive_0x7B[] = {
    0x7B,
    0x00,
};
static char gamma_b_positive_0x7C[] = {
    0x7C,
    0xFA,
};
static char gamma_b_positive_0x7D[] = {
    0x7D,
    0x01,
};
static char gamma_b_positive_0x7E[] = {
    0x7E,
    0x0B,
};
static char gamma_b_positive_0x7F[] = {
    0x7F,
    0x01,
};
static char gamma_b_positive_0x80[] = {
    0x80,
    0x1B,
};
static char gamma_b_positive_0x81[] = {
    0x81,
    0x01,
};
static char gamma_b_positive_0x82[] = {
    0x82,
    0x29,
};
static char gamma_b_positive_0x83[] = {
    0x83,
    0x01,
};
static char gamma_b_positive_0x84[] = {
    0x84,
    0x5A,
};
static char gamma_b_positive_0x85[] = {
    0x85,
    0x01,
};
static char gamma_b_positive_0x86[] = {
    0x86,
    0x80,
};
static char gamma_b_positive_0x87[] = {
    0x87,
    0x01,
};
static char gamma_b_positive_0x88[] = {
    0x88,
    0xBA,
};
static char gamma_b_positive_0x89[] = {
    0x89,
    0x01,
};
static char gamma_b_positive_0x8A[] = {
    0x8A,
    0xE8,
};
static char gamma_b_positive_0x8B[] = {
    0x8B,
    0x02,
};
static char gamma_b_positive_0x8C[] = {
    0x8C,
    0x2D,
};
static char gamma_b_positive_0x8D[] = {
    0x8D,
    0x02,
};
static char gamma_b_positive_0x8E[] = {
    0x8E,
    0x66,
};
static char gamma_b_positive_0x8F[] = {
    0x8F,
    0x02,
};
static char gamma_b_positive_0x90[] = {
    0x90,
    0x68,
};
static char gamma_b_positive_0x91[] = {
    0x91,
    0x02,
};
static char gamma_b_positive_0x92[] = {
    0x92,
    0x9F,
};
static char gamma_b_positive_0x93[] = {
    0x93,
    0x02,
};
static char gamma_b_positive_0x94[] = {
    0x94,
    0xDC,
};
static char gamma_b_positive_0x95[] = {
    0x95,
    0x03,
};
static char gamma_b_positive_0x96[] = {
    0x96,
    0x01,
};
static char gamma_b_positive_0x97[] = {
    0x97,
    0x03,
};
static char gamma_b_positive_0x98[] = {
    0x98,
    0x30,
};
static char gamma_b_positive_0x99[] = {
    0x99,
    0x03,
};
static char gamma_b_positive_0x9A[] = {
    0x9A,
    0x4E,
};
static char gamma_b_positive_0x9B[] = {
    0x9B,
    0x03,
};
static char gamma_b_positive_0x9C[] = {
    0x9C,
    0x75,
};
static char gamma_b_positive_0x9D[] = {
    0x9D,
    0x03,
};
static char gamma_b_positive_0x9E[] = {
    0x9E,
    0x82,
};
static char gamma_b_positive_0x9F[] = {
    0x9F,
    0x03,
};
static char gamma_b_positive_0xA0[] = {
    0xA0,
    0x98,
};
static char gamma_b_positive_0xA2[] = {
    0xA2,
    0x03,
};
static char gamma_b_positive_0xA3[] = {
    0xA3,
    0xA7,
};
static char gamma_b_positive_0xA4[] = {
    0xA4,
    0x03,
};
static char gamma_b_positive_0xA5[] = {
    0xA5,
    0xFE,
};
static char gamma_b_positive_0xA6[] = {
    0xA6,
    0x03,
};
static char gamma_b_positive_0xA7[] = {
    0xA7,
    0xFE,
};
static char gamma_b_positive_0xA9[] = {
    0xA9,
    0x03,
};
static char gamma_b_positive_0xAA[] = {
    0xAA,
    0xFE,
};
static char gamma_b_positive_0xAB[] = {
    0xAB,
    0x03,
};
static char gamma_b_positive_0xAC[] = {
    0xAC,
    0xFE,
};
static char gamma_b_positive_0xAD[] = {
    0xAD,
    0x03,
};
static char gamma_b_positive_0xAE[] = {
    0xAE,
    0xFF,
};
//B(-) MCR cmd
static char gamma_b_negative_0xAF[] = {
    0xAF,
    0x00,
};
static char gamma_b_negative_0xB0[] = {
    0xB0,
    0x96,
};
static char gamma_b_negative_0xB1[] = {
    0xB1,
    0x00,
};
static char gamma_b_negative_0xB2[] = {
    0xB2,
    0x98,
};
static char gamma_b_negative_0xB3[] = {
    0xB3,
    0x00,
};
static char gamma_b_negative_0xB4[] = {
    0xB4,
    0xB9,
};
static char gamma_b_negative_0xB5[] = {
    0xB5,
    0x00,
};
static char gamma_b_negative_0xB6[] = {
    0xB6,
    0xD2,
};
static char gamma_b_negative_0xB7[] = {
    0xB7,
    0x00,
};
static char gamma_b_negative_0xB8[] = {
    0xB8,
    0xE7,
};
static char gamma_b_negative_0xB9[] = {
    0xB9,
    0x00,
};
static char gamma_b_negative_0xBA[] = {
    0xBA,
    0xFA,
};
static char gamma_b_negative_0xBB[] = {
    0xBB,
    0x01,
};
static char gamma_b_negative_0xBC[] = {
    0xBC,
    0x0B,
};
static char gamma_b_negative_0xBD[] = {
    0xBD,
    0x01,
};
static char gamma_b_negative_0xBE[] = {
    0xBE,
    0x1B,
};
static char gamma_b_negative_0xBF[] = {
    0xBF,
    0x01,
};
static char gamma_b_negative_0xC0[] = {
    0xC0,
    0x29,
};
static char gamma_b_negative_0xC1[] = {
    0xC1,
    0x01,
};
static char gamma_b_negative_0xC2[] = {
    0xC2,
    0x5A,
};
static char gamma_b_negative_0xC3[] = {
    0xC3,
    0x01,
};
static char gamma_b_negative_0xC4[] = {
    0xC4,
    0x80,
};
static char gamma_b_negative_0xC5[] = {
    0xC5,
    0x01,
};
static char gamma_b_negative_0xC6[] = {
    0xC6,
    0xBA,
};
static char gamma_b_negative_0xC7[] = {
    0xC7,
    0x01,
};
static char gamma_b_negative_0xC8[] = {
    0xC8,
    0xE8,
};
static char gamma_b_negative_0xC9[] = {
    0xC9,
    0x02,
};
static char gamma_b_negative_0xCA[] = {
    0xCA,
    0x2D,
};
static char gamma_b_negative_0xCB[] = {
    0xCB,
    0x02,
};
static char gamma_b_negative_0xCC[] = {
    0xCC,
    0x66,
};
static char gamma_b_negative_0xCD[] = {
    0xCD,
    0x02,
};
static char gamma_b_negative_0xCE[] = {
    0xCE,
    0x68,
};
static char gamma_b_negative_0xCF[] = {
    0xCF,
    0x02,
};
static char gamma_b_negative_0xD0[] = {
    0xD0,
    0x9F,
};
static char gamma_b_negative_0xD1[] = {
    0xD1,
    0x02,
};
static char gamma_b_negative_0xD2[] = {
    0xD2,
    0xDC,
};
static char gamma_b_negative_0xD3[] = {
    0xD3,
    0x03,
};
static char gamma_b_negative_0xD4[] = {
    0xD4,
    0x01,
};
static char gamma_b_negative_0xD5[] = {
    0xD5,
    0x03,
};
static char gamma_b_negative_0xD6[] = {
    0xD6,
    0x30,
};
static char gamma_b_negative_0xD7[] = {
    0xD7,
    0x03,
};
static char gamma_b_negative_0xD8[] = {
    0xD8,
    0x4E,
};
static char gamma_b_negative_0xD9[] = {
    0xD9,
    0x03,
};
static char gamma_b_negative_0xDA[] = {
    0xDA,
    0x75,
};
static char gamma_b_negative_0xDB[] = {
    0xDB,
    0x03,
};
static char gamma_b_negative_0xDC[] = {
    0xDC,
    0x82,
};
static char gamma_b_negative_0xDD[] = {
    0xDD,
    0x03,
};
static char gamma_b_negative_0xDE[] = {
    0xDE,
    0x98,
};
static char gamma_b_negative_0xDF[] = {
    0xDF,
    0x03,
};
static char gamma_b_negative_0xE0[] = {
    0xE0,
    0xA7,
};
static char gamma_b_negative_0xE1[] = {
    0xE1,
    0x03,
};
static char gamma_b_negative_0xE2[] = {
    0xE2,
    0xFE,
};
static char gamma_b_negative_0xE3[] = {
    0xE3,
    0x03,
};
static char gamma_b_negative_0xE4[] = {
    0xE4,
    0xFE,
};
static char gamma_b_negative_0xE5[] = {
    0xE5,
    0x03,
};
static char gamma_b_negative_0xE6[] = {
    0xE6,
    0xFE,
};
static char gamma_b_negative_0xE7[] = {
    0xE7,
    0x03,
};
static char gamma_b_negative_0xE8[] = {
    0xE8,
    0xFE,
};
static char gamma_b_negative_0xE9[] = {
    0xE9,
    0x03,
};
static char gamma_b_negative_0xEA[] = {
    0xEA,
    0xFF,
};

//page selection cmd
static char page_selection_0xFF_0x23[] = {
    0xFF,
    0x23,
};
static char cabc_0x32[] = {
    0x32,
    0x03,
};

//Smart Color Ratio
static char smart_color_0x4D[] = {
    0x4D,
    0x00,
};
static char smart_color_0x4E[] = {
    0x4E,
    0x00,
};
static char smart_color_0x4F[] = {
    0x4F,
    0x10,
};
static char smart_color_0x50[] = {
    0x50,
    0x18,
};
static char smart_color_0x51[] = {
    0x51,
    0x20,
};
static char smart_color_0x52[] = {
    0x52,
    0x28,
};
//Vivid Color Disable
static char vivid_color_disable_0x1A[] = {
    0x1A,
    0x00,
};
//Smart Color Enable
static char smart_color_enable_0x53[] = {
    0x53,
    0x77,
};
//Contrast Disable
static char contrast_disable_0x56[] = {
    0x56,
    0x00,
};
//Sharpness
static char sharpness_0x68[] = {
    0x68,
    0x77,
};
static char sharpness_0x65[] = {
    0x65,
    0xA3,
};
static char sharpness_0x66[] = {
    0x66,
    0xC1,
};
static char sharpness_0x67[] = {
    0x67,
    0xF8,
};
static char sharpness_0x69[] = {
    0x69,
    0xF2,
};
static char sharpness_0x97[] = {
    0x97,
    0xFF,
};
static char sharpness_0x98[] = {
    0x98,
    0x1C,
};
//CMD1
static char cmd1_0xFF[] = {
    0xFF,
    0x10,
};
static char cabc_set_mode_UI[] = {
    0x55,
    0x91,
};
/*static char cabc_set_mode_STILL[] = {
    0x55,
    0x92,
};*/
static char cabc_set_mode_MOVING[] = {
    0x55,
    0x93,
};

static char cmd1_page0_0x5E[] = {
    0x5E,
    0x04,
};

static char cmd2_page0_0x5C[] = {
    0x5C,
    0x00,
};
static char cmd2_page0_0x5D[] = {
    0x5D,
    0x00,
};
static char cmd2_page0_0x5F[] = {
    0x5F,
    0x00,
};
static char cmd2_page0_0x60[] = {
    0x60,
    0x80,
};
static char cmd2_page0_0x6B[] = {
    0x6B,
    0x43,
};
static char cmd2_page0_0xFB[] = {
    0xFB,
    0x01,
};

static char page_selection_0xFF_0x24[] = {
    0xFF,
    0x24,
};
static char cmd2_page4_0x75[] = {
    0x75,
    0x15,
};
static char cmd2_page4_0x76[] = {
    0x76,
    0x02,
};
static char cmd2_page4_0x77[] = {
    0x77,
    0x01,
};
static char cmd2_page4_0x7F[] = {
    0x7F,
    0x15,
};
static char cmd2_page4_0x81[] = {
    0x81,
    0x02,
};
static char cmd2_page4_0x82[] = {
    0x82,
    0x01,
};
static char cmd2_page4_0x84[] = {
    0x84,
    0x02,
};
static char cmd2_page4_0x85[] = {
    0x85,
    0x02,
};
static char cmd2_page4_0x97[] = {
    0x97,
    0xB3,
};
static char cmd2_page4_0xFB[] = {
    0xFB,
    0x01,
};
static char page_selection_0xFF_0x26[] = {
    0xFF,
    0x26,
};
static char cmd2_page6_0xAD[] = {
    0xAD,
    0x77,
};
static char cmd2_page6_0xAE[] = {
    0xAE,
    0x77,
};
static char cmd2_page6_0xFB[] = {
    0xFB,
    0x01,
};

static struct dsi_cmd_desc jdi_display_effect_on_cmds[] = {
    //diplay effect
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page2_0xFF), cmd2_page2_0xFF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(non_reload_0xFB), non_reload_0xFB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x00), color_enhancement_0x00},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x01), color_enhancement_0x01},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x02), color_enhancement_0x02},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x03), color_enhancement_0x03},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x04), color_enhancement_0x04},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x05), color_enhancement_0x05},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x06), color_enhancement_0x06},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x07), color_enhancement_0x07},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x08), color_enhancement_0x08},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x09), color_enhancement_0x09},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x0A), color_enhancement_0x0A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x0B), color_enhancement_0x0B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x0C), color_enhancement_0x0C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x0D), color_enhancement_0x0D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x0E), color_enhancement_0x0E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x0F), color_enhancement_0x0F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x10), color_enhancement_0x10},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x11), color_enhancement_0x11},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x12), color_enhancement_0x12},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x13), color_enhancement_0x13},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x14), color_enhancement_0x14},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x1A), color_enhancement_0x1A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x1B), color_enhancement_0x1B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x1C), color_enhancement_0x1C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x1D), color_enhancement_0x1D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x1E), color_enhancement_0x1E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x1F), color_enhancement_0x1F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x20), color_enhancement_0x20},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x21), color_enhancement_0x21},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x22), color_enhancement_0x22},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x23), color_enhancement_0x23},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x24), color_enhancement_0x24},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x25), color_enhancement_0x25},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x26), color_enhancement_0x26},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x27), color_enhancement_0x27},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x28), color_enhancement_0x28},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x29), color_enhancement_0x29},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x2A), color_enhancement_0x2A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x2B), color_enhancement_0x2B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x2F), color_enhancement_0x2F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x30), color_enhancement_0x30},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x31), color_enhancement_0x31},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x32), color_enhancement_0x32},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x33), color_enhancement_0x33},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x34), color_enhancement_0x34},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x35), color_enhancement_0x35},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x36), color_enhancement_0x36},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x37), color_enhancement_0x37},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x38), color_enhancement_0x38},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x39), color_enhancement_0x39},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x3A), color_enhancement_0x3A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x3B), color_enhancement_0x3B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x3F), color_enhancement_0x3F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x40), color_enhancement_0x40},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x41), color_enhancement_0x41},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x42), color_enhancement_0x42},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x43), color_enhancement_0x43},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x44), color_enhancement_0x44},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x45), color_enhancement_0x45},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x46), color_enhancement_0x46},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x47), color_enhancement_0x47},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x48), color_enhancement_0x48},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x49), color_enhancement_0x49},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x4A), color_enhancement_0x4A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x4B), color_enhancement_0x4B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x4C), color_enhancement_0x4C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x4D), color_enhancement_0x4D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x53), color_enhancement_0x53},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x54), color_enhancement_0x54},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x55), color_enhancement_0x55},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x56), color_enhancement_0x56},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x58), color_enhancement_0x58},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x59), color_enhancement_0x59},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x5A), color_enhancement_0x5A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x5B), color_enhancement_0x5B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x5C), color_enhancement_0x5C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x5D), color_enhancement_0x5D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x5E), color_enhancement_0x5E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x64), color_enhancement_0x5F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x60), color_enhancement_0x60},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x61), color_enhancement_0x61},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x62), color_enhancement_0x62},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x63), color_enhancement_0x63},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x64), color_enhancement_0x64},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x65), color_enhancement_0x65},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x66), color_enhancement_0x66},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x67), color_enhancement_0x67},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x68), color_enhancement_0x68},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x69), color_enhancement_0x69},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x97), color_enhancement_0x97},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(color_enhancement_0x98), color_enhancement_0x98},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(dither_enable_0xA2), dither_enable_0xA2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_0x4D), smart_color_0x4D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_0x4E), smart_color_0x4E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_0x4F), smart_color_0x4F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_0x50), smart_color_0x50},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_0x51), smart_color_0x51},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_0x52), smart_color_0x52},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(vivid_color_disable_0x1A), vivid_color_disable_0x1A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(smart_color_enable_0x53), smart_color_enable_0x53},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
       sizeof(contrast_disable_0x56), contrast_disable_0x56},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x68), sharpness_0x68},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x65), sharpness_0x65},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x66), sharpness_0x66},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x67), sharpness_0x67},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x69), sharpness_0x69},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x97), sharpness_0x97},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(sharpness_0x98), sharpness_0x98},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFF_0x20), page_selection_0xFF_0x20},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFB_0x01), page_selection_0xFB_0x01},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
	    sizeof(gamma_r_positive_0x75), gamma_r_positive_0x75},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x76), gamma_r_positive_0x76},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x77), gamma_r_positive_0x77},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x78), gamma_r_positive_0x78},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x79), gamma_r_positive_0x79},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x7A), gamma_r_positive_0x7A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x7B), gamma_r_positive_0x7B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x7C), gamma_r_positive_0x7C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x7D), gamma_r_positive_0x7D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x7E), gamma_r_positive_0x7E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x7F), gamma_r_positive_0x7F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x80), gamma_r_positive_0x80},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x81), gamma_r_positive_0x81},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x82), gamma_r_positive_0x82},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x83), gamma_r_positive_0x83},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x84), gamma_r_positive_0x84},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x85), gamma_r_positive_0x85},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x86), gamma_r_positive_0x86},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x87), gamma_r_positive_0x87},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x88), gamma_r_positive_0x88},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x89), gamma_r_positive_0x89},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x8A), gamma_r_positive_0x8A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x8B), gamma_r_positive_0x8B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x8C), gamma_r_positive_0x8C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x8D), gamma_r_positive_0x8D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x8E), gamma_r_positive_0x8E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x8F), gamma_r_positive_0x8F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x90), gamma_r_positive_0x90},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x91), gamma_r_positive_0x91},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x92), gamma_r_positive_0x92},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x93), gamma_r_positive_0x93},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x94), gamma_r_positive_0x94},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x95), gamma_r_positive_0x95},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x96), gamma_r_positive_0x96},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x97), gamma_r_positive_0x97},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x98), gamma_r_positive_0x98},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x99), gamma_r_positive_0x99},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x9A), gamma_r_positive_0x9A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x9B), gamma_r_positive_0x9B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x9C), gamma_r_positive_0x9C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x9D), gamma_r_positive_0x9D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x9E), gamma_r_positive_0x9E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0x9F), gamma_r_positive_0x9F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA0), gamma_r_positive_0xA0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA2), gamma_r_positive_0xA2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA3), gamma_r_positive_0xA3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA4), gamma_r_positive_0xA4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA5), gamma_r_positive_0xA5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA6), gamma_r_positive_0xA6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA7), gamma_r_positive_0xA7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xA9), gamma_r_positive_0xA9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xAA), gamma_r_positive_0xAA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xAB), gamma_r_positive_0xAB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xAC), gamma_r_positive_0xAC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xAD), gamma_r_positive_0xAD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xAE), gamma_r_positive_0xAE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xAF), gamma_r_positive_0xAF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xB0), gamma_r_positive_0xB0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xB1), gamma_r_positive_0xB1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_positive_0xB2), gamma_r_positive_0xB2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB3), gamma_r_negative_0xB3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB4), gamma_r_negative_0xB4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB5), gamma_r_negative_0xB5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB6), gamma_r_negative_0xB6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB7), gamma_r_negative_0xB7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB8), gamma_r_negative_0xB8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xB9), gamma_r_negative_0xB9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xBA), gamma_r_negative_0xBA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xBB), gamma_r_negative_0xBB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xBC), gamma_r_negative_0xBC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xBD), gamma_r_negative_0xBD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xBE), gamma_r_negative_0xBE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xBF), gamma_r_negative_0xBF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC0), gamma_r_negative_0xC0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC1), gamma_r_negative_0xC1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC2), gamma_r_negative_0xC2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC3), gamma_r_negative_0xC3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC4), gamma_r_negative_0xC4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC5), gamma_r_negative_0xC5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC6), gamma_r_negative_0xC6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC7), gamma_r_negative_0xC7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC8), gamma_r_negative_0xC8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xC9), gamma_r_negative_0xC9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xCA), gamma_r_negative_0xCA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xCB), gamma_r_negative_0xCB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xCC), gamma_r_negative_0xCC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xCD), gamma_r_negative_0xCD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xCE), gamma_r_negative_0xCE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xCF), gamma_r_negative_0xCF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD0), gamma_r_negative_0xD0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD1), gamma_r_negative_0xD1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD2), gamma_r_negative_0xD2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD3), gamma_r_negative_0xD3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD4), gamma_r_negative_0xD4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD5), gamma_r_negative_0xD5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD6), gamma_r_negative_0xD6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD7), gamma_r_negative_0xD7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD8), gamma_r_negative_0xD8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xD9), gamma_r_negative_0xD9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xDA), gamma_r_negative_0xDA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xDB), gamma_r_negative_0xDB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xDC), gamma_r_negative_0xDC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xDD), gamma_r_negative_0xDD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xDE), gamma_r_negative_0xDE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xDF), gamma_r_negative_0xDF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE0), gamma_r_negative_0xE0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE1), gamma_r_negative_0xE1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE2), gamma_r_negative_0xE2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE3), gamma_r_negative_0xE3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE4), gamma_r_negative_0xE4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE5), gamma_r_negative_0xE5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE6), gamma_r_negative_0xE6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE7), gamma_r_negative_0xE7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE8), gamma_r_negative_0xE8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xE9), gamma_r_negative_0xE9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xEA), gamma_r_negative_0xEA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xEB), gamma_r_negative_0xEB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xEC), gamma_r_negative_0xEC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xED), gamma_r_negative_0xED},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_r_negative_0xEE), gamma_r_negative_0xEE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xEF), gamma_g_positive_0xEF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF0), gamma_g_positive_0xF0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF1), gamma_g_positive_0xF1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF2), gamma_g_positive_0xF2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF3), gamma_g_positive_0xF3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF4), gamma_g_positive_0xF4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF5), gamma_g_positive_0xF5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF6), gamma_g_positive_0xF6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF7), gamma_g_positive_0xF7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF8), gamma_g_positive_0xF8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xF9), gamma_g_positive_0xF9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0xFA), gamma_g_positive_0xFA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFF_0x21), page_selection_0xFF_0x21},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFB_0x01), page_selection_0xFB_0x01},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x00), gamma_g_positive_0x00},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x01), gamma_g_positive_0x01},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x02), gamma_g_positive_0x02},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x03), gamma_g_positive_0x03},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x04), gamma_g_positive_0x04},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x05), gamma_g_positive_0x05},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x06), gamma_g_positive_0x06},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x07), gamma_g_positive_0x07},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x08), gamma_g_positive_0x08},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x09), gamma_g_positive_0x09},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x0A), gamma_g_positive_0x0A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x0B), gamma_g_positive_0x0B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x0C), gamma_g_positive_0x0C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x0D), gamma_g_positive_0x0D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x0E), gamma_g_positive_0x0E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x0F), gamma_g_positive_0x0F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x10), gamma_g_positive_0x10},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x11), gamma_g_positive_0x11},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x12), gamma_g_positive_0x12},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x13), gamma_g_positive_0x13},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x14), gamma_g_positive_0x14},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x15), gamma_g_positive_0x15},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x16), gamma_g_positive_0x16},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x17), gamma_g_positive_0x17},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x18), gamma_g_positive_0x18},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x19), gamma_g_positive_0x19},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x1A), gamma_g_positive_0x1A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x1B), gamma_g_positive_0x1B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x1C), gamma_g_positive_0x1C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x1D), gamma_g_positive_0x1D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x1E), gamma_g_positive_0x1E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x1F), gamma_g_positive_0x1F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x20), gamma_g_positive_0x20},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x21), gamma_g_positive_0x21},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x22), gamma_g_positive_0x22},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x23), gamma_g_positive_0x23},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x24), gamma_g_positive_0x24},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x25), gamma_g_positive_0x25},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x26), gamma_g_positive_0x26},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x27), gamma_g_positive_0x27},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x28), gamma_g_positive_0x28},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x29), gamma_g_positive_0x29},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x2A), gamma_g_positive_0x2A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x2B), gamma_g_positive_0x2B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x2D), gamma_g_positive_0x2D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x2F), gamma_g_positive_0x2F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x30), gamma_g_positive_0x30},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_positive_0x31), gamma_g_positive_0x31},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x32), gamma_g_negative_0x32},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x33), gamma_g_negative_0x33},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x34), gamma_g_negative_0x34},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x35), gamma_g_negative_0x35},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x36), gamma_g_negative_0x36},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x37), gamma_g_negative_0x37},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x38), gamma_g_negative_0x38},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x39), gamma_g_negative_0x39},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x3A), gamma_g_negative_0x3A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x3B), gamma_g_negative_0x3B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x3D), gamma_g_negative_0x3D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x3F), gamma_g_negative_0x3F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x40), gamma_g_negative_0x40},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x41), gamma_g_negative_0x41},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x42), gamma_g_negative_0x42},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x43), gamma_g_negative_0x43},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x44), gamma_g_negative_0x44},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x45), gamma_g_negative_0x45},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x46), gamma_g_negative_0x46},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x47), gamma_g_negative_0x47},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x48), gamma_g_negative_0x48},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x49), gamma_g_negative_0x49},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x4A), gamma_g_negative_0x4A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x4B), gamma_g_negative_0x4B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x4C), gamma_g_negative_0x4C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x4D), gamma_g_negative_0x4D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x4E), gamma_g_negative_0x4E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x4F), gamma_g_negative_0x4F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x50), gamma_g_negative_0x50},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x51), gamma_g_negative_0x51},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x52), gamma_g_negative_0x52},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x53), gamma_g_negative_0x53},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x54), gamma_g_negative_0x54},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x55), gamma_g_negative_0x55},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x56), gamma_g_negative_0x56},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x58), gamma_g_negative_0x58},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x59), gamma_g_negative_0x59},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x5A), gamma_g_negative_0x5A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x5B), gamma_g_negative_0x5B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x5C), gamma_g_negative_0x5C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x5D), gamma_g_negative_0x5D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x5E), gamma_g_negative_0x5E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x5F), gamma_g_negative_0x5F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x60), gamma_g_negative_0x60},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x61), gamma_g_negative_0x61},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x62), gamma_g_negative_0x62},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x63), gamma_g_negative_0x63},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x64), gamma_g_negative_0x64},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x65), gamma_g_negative_0x65},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x66), gamma_g_negative_0x66},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x67), gamma_g_negative_0x67},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x68), gamma_g_negative_0x68},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x69), gamma_g_negative_0x69},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x6A), gamma_g_negative_0x6A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x6B), gamma_g_negative_0x6B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x6C), gamma_g_negative_0x6C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x6D), gamma_g_negative_0x6D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x6E), gamma_g_negative_0x6E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x6F), gamma_g_negative_0x6F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_g_negative_0x70), gamma_g_negative_0x70},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x71), gamma_b_positive_0x71},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x72), gamma_b_positive_0x72},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x73), gamma_b_positive_0x73},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x74), gamma_b_positive_0x74},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x75), gamma_b_positive_0x75},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x76), gamma_b_positive_0x76},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x77), gamma_b_positive_0x77},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x78), gamma_b_positive_0x78},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x79), gamma_b_positive_0x79},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x7A), gamma_b_positive_0x7A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x7B), gamma_b_positive_0x7B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x7C), gamma_b_positive_0x7C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x7D), gamma_b_positive_0x7D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x7E), gamma_b_positive_0x7E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x7F), gamma_b_positive_0x7F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x80), gamma_b_positive_0x80},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x81), gamma_b_positive_0x81},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x82), gamma_b_positive_0x82},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x83), gamma_b_positive_0x83},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x84), gamma_b_positive_0x84},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x85), gamma_b_positive_0x85},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x86), gamma_b_positive_0x86},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x87), gamma_b_positive_0x87},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x88), gamma_b_positive_0x88},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x89), gamma_b_positive_0x89},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x8A), gamma_b_positive_0x8A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x8B), gamma_b_positive_0x8B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x8C), gamma_b_positive_0x8C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x8D), gamma_b_positive_0x8D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x8E), gamma_b_positive_0x8E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x8F), gamma_b_positive_0x8F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x90), gamma_b_positive_0x90},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x91), gamma_b_positive_0x91},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x92), gamma_b_positive_0x92},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x93), gamma_b_positive_0x93},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x94), gamma_b_positive_0x94},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x95), gamma_b_positive_0x95},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x96), gamma_b_positive_0x96},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x97), gamma_b_positive_0x97},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x98), gamma_b_positive_0x98},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x99), gamma_b_positive_0x99},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x9A), gamma_b_positive_0x9A},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x9B), gamma_b_positive_0x9B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x9C), gamma_b_positive_0x9C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x9D), gamma_b_positive_0x9D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x9E), gamma_b_positive_0x9E},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0x9F), gamma_b_positive_0x9F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA0), gamma_b_positive_0xA0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA2), gamma_b_positive_0xA2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA3), gamma_b_positive_0xA3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA4), gamma_b_positive_0xA4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA5), gamma_b_positive_0xA5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA6), gamma_b_positive_0xA6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA7), gamma_b_positive_0xA7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xA9), gamma_b_positive_0xA9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xAA), gamma_b_positive_0xAA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xAB), gamma_b_positive_0xAB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xAC), gamma_b_positive_0xAC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xAD), gamma_b_positive_0xAD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_positive_0xAE), gamma_b_positive_0xAE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xAF), gamma_b_negative_0xAF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB0), gamma_b_negative_0xB0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB1), gamma_b_negative_0xB1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB2), gamma_b_negative_0xB2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB3), gamma_b_negative_0xB3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB4), gamma_b_negative_0xB4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB5), gamma_b_negative_0xB5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB6), gamma_b_negative_0xB6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB7), gamma_b_negative_0xB7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB8), gamma_b_negative_0xB8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xB9), gamma_b_negative_0xB9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xBA), gamma_b_negative_0xBA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xBB), gamma_b_negative_0xBB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xBC), gamma_b_negative_0xBC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xBD), gamma_b_negative_0xBD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xBE), gamma_b_negative_0xBE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xBF), gamma_b_negative_0xBF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC0), gamma_b_negative_0xC0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC1), gamma_b_negative_0xC1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC2), gamma_b_negative_0xC2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC3), gamma_b_negative_0xC3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC4), gamma_b_negative_0xC4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC5), gamma_b_negative_0xC5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC6), gamma_b_negative_0xC6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC7), gamma_b_negative_0xC7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC8), gamma_b_negative_0xC8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xC9), gamma_b_negative_0xC9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xCA), gamma_b_negative_0xCA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xCB), gamma_b_negative_0xCB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xCC), gamma_b_negative_0xCC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xCD), gamma_b_negative_0xCD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xCE), gamma_b_negative_0xCE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xCF), gamma_b_negative_0xCF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD0), gamma_b_negative_0xD0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD1), gamma_b_negative_0xD1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD2), gamma_b_negative_0xD2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD3), gamma_b_negative_0xD3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD4), gamma_b_negative_0xD4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD5), gamma_b_negative_0xD5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD6), gamma_b_negative_0xD6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD7), gamma_b_negative_0xD7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD8), gamma_b_negative_0xD8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xD9), gamma_b_negative_0xD9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xDA), gamma_b_negative_0xDA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xDB), gamma_b_negative_0xDB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xDC), gamma_b_negative_0xDC},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xDD), gamma_b_negative_0xDD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xDE), gamma_b_negative_0xDE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xDF), gamma_b_negative_0xDF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE0), gamma_b_negative_0xE0},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE1), gamma_b_negative_0xE1},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE2), gamma_b_negative_0xE2},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE3), gamma_b_negative_0xE3},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE4), gamma_b_negative_0xE4},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE5), gamma_b_negative_0xE5},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE6), gamma_b_negative_0xE6},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE7), gamma_b_negative_0xE7},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE8), gamma_b_negative_0xE8},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xE9), gamma_b_negative_0xE9},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(gamma_b_negative_0xEA), gamma_b_negative_0xEA},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFF_0x23), page_selection_0xFF_0x23},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(non_reload_0xFB), non_reload_0xFB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cabc_0x32), cabc_0x32},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFF_0x20), page_selection_0xFF_0x20},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page0_0x5C), cmd2_page0_0x5C},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page0_0x5D), cmd2_page0_0x5D},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page0_0x5F), cmd2_page0_0x5F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page0_0x60), cmd2_page0_0x60},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page0_0x6B), cmd2_page0_0x6B},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page0_0xFB), cmd2_page0_0xFB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFF_0x24), page_selection_0xFF_0x24},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x75), cmd2_page4_0x75},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x76), cmd2_page4_0x76},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x77), cmd2_page4_0x77},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x7F), cmd2_page4_0x7F},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x81), cmd2_page4_0x81},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x82), cmd2_page4_0x82},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x84), cmd2_page4_0x84},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x85), cmd2_page4_0x85},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0x97), cmd2_page4_0x97},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page4_0xFB), cmd2_page4_0xFB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(page_selection_0xFF_0x26), page_selection_0xFF_0x26},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page6_0xAD), cmd2_page6_0xAD},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page6_0xAE), cmd2_page6_0xAE},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd2_page6_0xFB), cmd2_page6_0xFB},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd1_0xFF), cmd1_0xFF},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cabc_set_mode_UI), cabc_set_mode_UI},
    {DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
        sizeof(cmd1_page0_0x5E), cmd1_page0_0x5E},
};

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
       {DTYPE_DCS_WRITE1, 0,10, WAIT_TYPE_US,
    	       sizeof(tear_on), tear_on},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(caset_data), caset_data},
	{DTYPE_DCS_LWRITE, 0,10, WAIT_TYPE_US,
		sizeof(paset_data), paset_data},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_page3), set_page3},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(set_pwm_freq), set_pwm_freq},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_page0), set_page0},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(te_line), te_line},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_pagef0), set_pagef0},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd3_0xdd_data), cmd3_0xdd_data},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd3_0xe3_data), cmd3_0xe3_data},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(cmd3_0xfb_data), cmd3_0xfb_data},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_page0), set_page0},
	{DTYPE_DCS_WRITE, 0, 115, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 0, 50, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc jdi_cabc_ui_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_UI), cabc_set_mode_UI},
};

/*static struct dsi_cmd_desc jdi_cabc_still_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_STILL), cabc_set_mode_STILL},
};*/

static struct dsi_cmd_desc jdi_cabc_moving_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(cabc_set_mode_MOVING), cabc_set_mode_MOVING},
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 1, WAIT_TYPE_MS,
		sizeof(bl_disable), bl_disable},
	{DTYPE_DCS_WRITE, 0, 60, WAIT_TYPE_MS,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep}
};

/*******************************************************************************
** LCD VCC
*/
#define VCC_LCDIO_NAME		"lcdio-vcc"
#define VCC_LCDANALOG_NAME	"lcdanalog-vcc"

static struct regulator *vcc_lcdio;
static struct regulator *vcc_lcdanalog;

static struct vcc_desc jdi_lcd_vcc_init_cmds[] = {
	/* vcc get */
	{DTYPE_VCC_GET, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0},
	{DTYPE_VCC_GET, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0},
	/* vcc set voltage */
	{DTYPE_VCC_SET_VOLTAGE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 3100000, 3100000},
};

static struct vcc_desc jdi_lcd_vcc_finit_cmds[] = {
	/* vcc put */
	{DTYPE_VCC_PUT, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0},
	{DTYPE_VCC_PUT, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0},
};

static struct vcc_desc jdi_lcd_vcc_enable_cmds[] = {
	/* vcc enable */
	{DTYPE_VCC_ENABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0},
	{DTYPE_VCC_ENABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0},
};

static struct vcc_desc jdi_lcd_vcc_disable_cmds[] = {
	/* vcc disable */
	{DTYPE_VCC_DISABLE, VCC_LCDIO_NAME, &vcc_lcdio, 0, 0},
	{DTYPE_VCC_DISABLE, VCC_LCDANALOG_NAME, &vcc_lcdanalog, 0, 0},
};

/*******************************************************************************
** LCD IOMUX
*/
static struct pinctrl_data pctrl;

static struct pinctrl_cmd_desc jdi_lcd_pinctrl_init_cmds[] = {
	{DTYPE_PINCTRL_GET, &pctrl, 0},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
	{DTYPE_PINCTRL_STATE_GET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

static struct pinctrl_cmd_desc jdi_lcd_pinctrl_normal_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_DEFAULT},
};

static struct pinctrl_cmd_desc jdi_lcd_pinctrl_lowpower_cmds[] = {
	{DTYPE_PINCTRL_SET, &pctrl, DTYPE_PINCTRL_STATE_IDLE},
};

/*******************************************************************************
** LCD GPIO
*/
#define GPIO_LCD_BL_ENABLE_NAME	"gpio_lcd_bl_enable"
#define GPIO_LCD_RESET_NAME	"gpio_lcd_reset"
#define GPIO_LCD_ID_NAME	"gpio_lcd_id"
#define GPIO_LCD_P5V5_ENABLE_NAME	"gpio_lcd_p5v5_enable"
#define GPIO_LCD_N5V5_ENABLE_NAME "gpio_lcd_n5v5_enable"

static u32 gpio_lcd_bl_enable;  /*gpio_4_3, gpio_035*/
static u32 gpio_lcd_reset;  /*gpio_4_5, gpio_037*/
static u32 gpio_lcd_id;  /*gpio_4_6, gpio_038*/
static u32 gpio_lcd_p5v5_enable;  /*gpio_5_1, gpio_041*/
static u32 gpio_lcd_n5v5_enable;  /*gpio_5_2, gpio_042*/

static struct gpio_desc jdi_lcd_gpio_request_cmds[] = {
	/* backlight enable */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* reset */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* lcd id */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
	/* AVDD_5.5V */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
	/* AVEE_-5.5V */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
};

static struct gpio_desc jdi_lcd_gpio_free_cmds[] = {
	/* backlight enable */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* reset */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* lcd id */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
	/* AVDD_5.5V */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
	/* AVEE_-5.5V */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_free_cmds[] = {
	/* reset */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* backlight enable */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* lcd id */
	{DTYPE_GPIO_FREE, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_request_cmds[] = {
	/* reset */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* backlight enable */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* lcd id */
	{DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_lp_cmds[] = {
	/* backlight disable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
};

static struct gpio_desc jdi_lcd_gpio_sleep_normal_cmds[] = {
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 65,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	/* backlight enable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 1},
	/* lcd id */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 40,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_normal_cmds[] = {
	/* AVDD_5.5V*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 1},
	/* AVEE_-5.5V */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 1},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 1},
	/* backlight enable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 1},
	/* lcd id */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 1,
		GPIO_LCD_ID_NAME, &gpio_lcd_id, 0},
};

static struct gpio_desc jdi_lcd_gpio_lowpower_cmds[] = {
	/* backlight enable */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* AVEE_-5.5V */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
	/* AVDD_5.5V*/
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
	/* reset */
	{DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
	/* backlight enable input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCD_BL_ENABLE_NAME, &gpio_lcd_bl_enable, 0},
	/* AVEE_-5.5V input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_N5V5_ENABLE_NAME, &gpio_lcd_n5v5_enable, 0},
	/* AVDD_5.5V input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 5,
		GPIO_LCD_P5V5_ENABLE_NAME, &gpio_lcd_p5v5_enable, 0},
	/* reset input */
	{DTYPE_GPIO_INPUT, WAIT_TYPE_US, 100,
		GPIO_LCD_RESET_NAME, &gpio_lcd_reset, 0},
};

/******************************************************************************/
static struct lcd_tuning_dev *p_tuning_dev = NULL;
static int cabc_mode = 1; /* allow application to set cabc mode to ui mode */
static char __iomem *mipi_dsi0_base_display_effect = NULL;
static volatile bool g_display_on = false;
static unsigned int g_csc_value[9];
static unsigned int g_is_csc_set;
static struct semaphore ct_sem;

static int jdi_set_cabc(struct lcd_tuning_dev *ltd, enum  tft_cabc cabc)
{
	int ret = 0;
	if (!g_k3fd->panel_power_on) {
		K3_FB_INFO("power off\n");
		return ret;
	}
	/* Fix me: Implement it */
	switch (cabc)
	{
		case CABC_UI:
			mipi_dsi_cmds_tx(jdi_cabc_ui_on_cmds, \
							ARRAY_SIZE(jdi_cabc_ui_on_cmds),\
							mipi_dsi0_base_display_effect);
			break;
		case CABC_VID:
			mipi_dsi_cmds_tx(jdi_cabc_moving_on_cmds, \
							ARRAY_SIZE(jdi_cabc_moving_on_cmds),\
							mipi_dsi0_base_display_effect);
			break;
		case CABC_OFF:
			break;
		default:
			ret = -1;
	}
	return ret;
}

static void jdi_store_ct_cscValue(unsigned int csc_value[])
{
	down(&ct_sem);
	g_csc_value [0] = csc_value[0];
	g_csc_value [1] = csc_value[1];
	g_csc_value [2] = csc_value[2];
	g_csc_value [3] = csc_value[3];
	g_csc_value [4] = csc_value[4];
	g_csc_value [5] = csc_value[5];
	g_csc_value [6] = csc_value[6];
	g_csc_value [7] = csc_value[7];
	g_csc_value [8] = csc_value[8];
	g_is_csc_set = 1;
	up(&ct_sem);

	return;
}

static int jdi_set_ct_cscValue(struct k3_fb_data_type *k3fd)
{
	char __iomem *dss_base = 0;
	dss_base = k3fd->dss_base;//0xe8500000
	down(&ct_sem);
	K3_FB_INFO("set color temperature: g_is_csc_set = %d, g_display_on = %d, R = 0x%x, G = 0x%x, B = 0x%x .\n",
		g_is_csc_set, g_display_on, g_csc_value[0], g_csc_value[4], g_csc_value[8]);
	if (1 == g_is_csc_set && g_display_on) {
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_01, g_csc_value[0], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_02, g_csc_value[1], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_03, g_csc_value[2], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_11, g_csc_value[3], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_12, g_csc_value[4], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_13, g_csc_value[5], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_21, g_csc_value[6], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_22, g_csc_value[7], 17, 0);
		set_reg(dss_base + DSS_DPP_LCP_OFFSET + LCP_XCC_COEF_23, g_csc_value[8], 17, 0);
	}
	up(&ct_sem);

	return 0;
}

static int jdi_set_color_temperature(struct lcd_tuning_dev *ltd, unsigned int csc_value[])
{
	int flag = 0;
	struct platform_device *pdev;
	struct k3_fb_data_type *k3fd;

	if (ltd == NULL) {
		return -1;
	}
	pdev = (struct platform_device *)(ltd->data);
	k3fd = (struct k3_fb_data_type *)platform_get_drvdata(pdev);

	if (k3fd == NULL) {
		return -1;
	}

	jdi_store_ct_cscValue(csc_value);
	flag = jdi_set_ct_cscValue(k3fd);
	return flag;

}
static ssize_t k3_lcd_model_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "jdi_NT35695 6.0' CMD TFT 1080 x 1920\n");
}

static ssize_t jdi_frame_count_show(struct device *dev,
          struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n", frame_count);
}

static ssize_t k3_hkadc_debug_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", hkadc_buf*4);
}

static ssize_t k3_hkadc_debug_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	int channel = 0;

	ret = sscanf(buf, "%u", &channel);
	if(ret <= 0) {
		K3_FB_ERR("Sscanf return invalid, ret = %d\n", ret);
		return count;
	}

	hkadc_buf = hisi_adc_get_value(channel);
	K3_FB_INFO("channel[%d] value is %d\n", channel, hkadc_buf);
	return count;
}

static struct lcd_tuning_ops sp_tuning_ops = {
	.set_cabc = jdi_set_cabc,
	.set_color_temperature = jdi_set_color_temperature,
};

static ssize_t show_cabc_mode(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", cabc_mode);
}

static ssize_t store_cabc_mode(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	unsigned long val = 0;

	ret = strict_strtoul(buf, 0, &val);
	if (ret)
		return ret;

	if (val == 1) {
		/* allow application to set cabc mode to ui mode */
		cabc_mode =1;
		jdi_set_cabc(p_tuning_dev, CABC_UI);
	} else if (val == 2) {
		/* force cabc mode to video mode */
		cabc_mode =2;
		jdi_set_cabc(p_tuning_dev, CABC_VID);
	}
	return snprintf((char *)buf, count, "%d\n", cabc_mode);
}

static struct device_attribute k3_lcd_class_attrs[] = {
	__ATTR(lcd_model, S_IRUGO, k3_lcd_model_show, NULL),
	__ATTR(cabc_mode, 0644, show_cabc_mode, store_cabc_mode),
	__ATTR(frame_count, S_IRUGO, jdi_frame_count_show, NULL),
	__ATTR(hkadc_debug, S_IRUSR|S_IWUSR, k3_hkadc_debug_show, k3_hkadc_debug_store),
	__ATTR_NULL,
};

static int mipi_jdi_panel_set_fastboot(struct platform_device *pdev)
{
	struct k3_fb_data_type *k3fd = NULL;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	K3_FB_DEBUG("fb%d, +.\n", k3fd->index);

	K3_FB_DEBUG("fb%d, -.\n", k3fd->index);

	return 0;
}

static int mipi_jdi_panel_on(struct platform_device *pdev)
{
	struct k3_fb_data_type *k3fd = NULL;
	struct k3_panel_info *pinfo = NULL;
	char __iomem *mipi_dsi0_base = NULL;
	u32 pkg_status = 0, power_status = 0, try_times = 0;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	K3_FB_INFO("fb%d, +!\n", k3fd->index);

	pinfo = &(k3fd->panel_info);
	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;
	mipi_dsi0_base_display_effect = mipi_dsi0_base;

	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		if (false == gesture_func) {
			/* lcd vcc enable */
			if (likely(!fastboot_display_enable)) {
				vcc_cmds_tx(pdev, jdi_lcd_vcc_enable_cmds,
					ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));
			}
		} else
			K3_FB_INFO("power on (gesture_func:%d)\n", gesture_func);

		pinfo->lcd_init_step = LCD_INIT_MIPI_LP_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));

		if (false == gesture_func) {
			/* lcd gpio request */
			gpio_cmds_tx(jdi_lcd_gpio_request_cmds, \
				ARRAY_SIZE(jdi_lcd_gpio_request_cmds));

			/* lcd gpio normal */
			if (likely(!fastboot_display_enable)) {
				gpio_cmds_tx(jdi_lcd_gpio_normal_cmds, \
					ARRAY_SIZE(jdi_lcd_gpio_normal_cmds));
				mipi_dsi_cmds_tx(jdi_display_effect_on_cmds, \
					ARRAY_SIZE(jdi_display_effect_on_cmds), mipi_dsi0_base);
			}
		} else {
			/* lcd gpio request */
			gpio_cmds_tx(jdi_lcd_gpio_sleep_request_cmds, \
				ARRAY_SIZE(jdi_lcd_gpio_sleep_request_cmds));

			/* backlights enable */
			if (likely(!fastboot_display_enable)) {
				gpio_cmds_tx(jdi_lcd_gpio_sleep_normal_cmds, \
					ARRAY_SIZE(jdi_lcd_gpio_sleep_normal_cmds));
				mipi_dsi_cmds_tx(jdi_display_effect_on_cmds, \
					ARRAY_SIZE(jdi_display_effect_on_cmds), mipi_dsi0_base);
			}

			K3_FB_INFO("lp send sequence (gesture_func:%d)\n", gesture_func);
		}

		mipi_dsi_cmds_tx(jdi_display_on_cmds, \
			ARRAY_SIZE(jdi_display_on_cmds), mipi_dsi0_base);
		/* jdi_cabc_ui_on_cmds*/
		mipi_dsi_cmds_tx(jdi_cabc_ui_on_cmds, \
			ARRAY_SIZE(jdi_cabc_ui_on_cmds), mipi_dsi0_base);
		g_display_on = true;
		jdi_set_ct_cscValue(k3fd);
		if (likely(!fastboot_display_enable)) {
			outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0A06);
			pkg_status = inp32(mipi_dsi0_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
			while (pkg_status & 0x10) {
				udelay(50);
				if (++try_times > 100) {
					K3_FB_ERR("Read lcd power status timeout\n");
					break;
				}
				pkg_status = inp32(mipi_dsi0_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
			}
			power_status = inp32(mipi_dsi0_base + MIPIDSI_GEN_PLD_DATA_OFFSET);

			K3_FB_INFO("LCD Power State = 0x%x\n", power_status);
		}
		fastboot_display_enable = false;
		debug_enable = true;
		pinfo->lcd_init_step = LCD_INIT_MIPI_HS_SEND_SEQUENCE;
	} else if (pinfo->lcd_init_step == LCD_INIT_MIPI_HS_SEND_SEQUENCE) {
		pr_jank(JL_KERNEL_LCD_POWER_ON, "%s", "JL_KERNEL_LCD_POWER_ON");
	} else
		K3_FB_ERR("failed to init lcd!\n");

	if (k3fd->panel_info.bl_set_type & BL_SET_BY_PWM)
		k3_pwm_on(pdev);
	else if (k3fd->panel_info.bl_set_type & BL_SET_BY_BLPWM)
		k3_blpwm_on(pdev);
	else if (k3fd->panel_info.bl_set_type & BL_SET_BY_MIPI)
		K3_FB_INFO("Set backlight by mipi\n");
	else
		K3_FB_ERR("No such bl_set_type!\n");

	K3_FB_INFO("fb%d, -!\n", k3fd->index);

	return 0;
}

static int mipi_jdi_panel_off(struct platform_device *pdev)
{
	struct k3_fb_data_type *k3fd = NULL;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	K3_FB_INFO("fb%d, +!\n", k3fd->index);
	pr_jank(JL_KERNEL_LCD_POWER_OFF, "%s", "JL_KERNEL_LCD_POWER_OFF");

	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;

	if (k3fd->panel_info.bl_set_type & BL_SET_BY_PWM)
		k3_pwm_off(pdev);
	else if (k3fd->panel_info.bl_set_type & BL_SET_BY_BLPWM)
		k3_blpwm_off(pdev);
	else if (k3fd->panel_info.bl_set_type & BL_SET_BY_MIPI)
		K3_FB_INFO("Set backlight by mipi\n");
	else
		K3_FB_ERR("No such bl_set_type!\n");

	g_display_on = false;
	/* lcd display off sequence */
	mipi_dsi_cmds_tx(jdi_display_off_cmds, \
		ARRAY_SIZE(jdi_display_off_cmds), mipi_dsi0_base);

	if (false == gesture_func) {
		/* lcd gpio lowpower(normal) */
		gpio_cmds_tx(jdi_lcd_gpio_lowpower_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_lowpower_cmds));

		/* lcd gpio free */
		gpio_cmds_tx(jdi_lcd_gpio_free_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_free_cmds));

		/* lcd pinctrl lowpower */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_lowpower_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_lowpower_cmds));

		/* lcd vcc disable */
		vcc_cmds_tx(pdev, jdi_lcd_vcc_disable_cmds,
			ARRAY_SIZE(jdi_lcd_vcc_disable_cmds));

	} else {
		K3_FB_INFO("display_off (gesture_func:%d)\n", gesture_func);

		/*backlights disable*/
		gpio_cmds_tx(jdi_lcd_gpio_sleep_lp_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_sleep_lp_cmds));

		/* lcd gpio free */
		gpio_cmds_tx(jdi_lcd_gpio_sleep_free_cmds, \
			ARRAY_SIZE(jdi_lcd_gpio_sleep_free_cmds));

		/* lcd pinctrl normal */
		pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_normal_cmds,
			ARRAY_SIZE(jdi_lcd_pinctrl_normal_cmds));
	}

	K3_FB_INFO("fb%d, -!\n", k3fd->index);

	return 0;
}

static int mipi_jdi_panel_remove(struct platform_device *pdev)
{
	struct k3_fb_data_type *k3fd = NULL;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);

	if (!k3fd) {
		return 0;
	}

	K3_FB_DEBUG("fb%d, +.\n", k3fd->index);

	/* lcd vcc finit */
	vcc_cmds_tx(pdev, jdi_lcd_vcc_finit_cmds,
		ARRAY_SIZE(jdi_lcd_vcc_finit_cmds));

	K3_FB_DEBUG("fb%d, -.\n", k3fd->index);

	return 0;
}

static int k3_mipi_set_backlight(struct k3_fb_data_type *k3fd)
{
	char __iomem *mipi_dsi0_base = NULL;
	u32 level = 0;
	u8 bl_level_adjust[2] = {
		0x51,
		0x00,
	};

	struct dsi_cmd_desc  jdi_bl_level_adjust[] = {
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(bl_level_adjust), bl_level_adjust},
	};
	level = k3fd->bl_level;
	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;

	//if need to avoid some level
	if (level < 4 && level != 0) {
		level = 4;
	}

	bl_level_adjust[1] = level;

	mipi_dsi_cmds_tx(jdi_bl_level_adjust, \
		ARRAY_SIZE(jdi_bl_level_adjust), mipi_dsi0_base);

	return 0;
}

static int mipi_jdi_panel_set_backlight(struct platform_device *pdev)
{
	int ret = 0;

	struct k3_fb_data_type *k3fd = NULL;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	K3_FB_DEBUG("fb%d, +.\n", k3fd->index);

	if (unlikely(debug_enable)) {
		K3_FB_INFO("Set backlight to %d\n", k3fd->bl_level);
		pr_jank(JL_KERNEL_LCD_RESUME, "%s, %d", "JL_KERNEL_LCD_RESUME", k3fd->bl_level);
		debug_enable = false;
	}

	if (k3fd->panel_info.bl_set_type & BL_SET_BY_PWM)
		ret = k3_pwm_set_backlight(k3fd);
	else if (k3fd->panel_info.bl_set_type & BL_SET_BY_BLPWM)
		ret = k3_blpwm_set_backlight(k3fd);
	else if (k3fd->panel_info.bl_set_type & BL_SET_BY_MIPI)
		ret = k3_mipi_set_backlight(k3fd);
	else
		K3_FB_ERR("No such bl_set_type!\n");

	K3_FB_DEBUG("fb%d, -.\n", k3fd->index);

	return ret;
}

static char lcd_disp_x[] = {
	0x2A,
	0x00, 0x00,0x04,0x37
};

static char lcd_disp_y[] = {
	0x2B,
	0x00, 0x00,0x07,0x7F
};

static struct dsi_cmd_desc set_display_address[] = {
	{DTYPE_DCS_LWRITE, 0, 5, WAIT_TYPE_US,
		sizeof(lcd_disp_x), lcd_disp_x},
	{DTYPE_DCS_LWRITE, 0, 5, WAIT_TYPE_US,
		sizeof(lcd_disp_y), lcd_disp_y},
};

static int mipi_jdi_panel_set_display_region(struct platform_device *pdev,
	struct dss_rect *dirty)
{
	struct k3_fb_data_type *k3fd = NULL;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL || dirty == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;

	lcd_disp_x[1] = (dirty->x >> 8) & 0xff;
	lcd_disp_x[2] = dirty->x & 0xff;
	lcd_disp_x[3] = ((dirty->x + dirty->w - 1) >> 8) & 0xff;
	lcd_disp_x[4] = (dirty->x + dirty->w - 1) & 0xff;
	lcd_disp_y[1] = (dirty->y >> 8) & 0xff;
	lcd_disp_y[2] = dirty->y & 0xff;
	lcd_disp_y[3] = ((dirty->y + dirty->h - 1) >> 8) & 0xff;
	lcd_disp_y[4] = (dirty->y + dirty->h - 1) & 0xff;

	mipi_dsi_cmds_tx(set_display_address, \
		ARRAY_SIZE(set_display_address), mipi_dsi0_base);

	return 0;
}

#ifdef CONFIG_LCD_CHECK_REG
static int mipi_jdi_check_reg(struct platform_device *pdev)
{
	struct k3_fb_data_type *k3fd = NULL;
	u32 read_value = 0xFF;
	int ret = LCD_CHECK_REG_PASS;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;

	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0A06);
	if (!mipi_dsi_read(&read_value, mipi_dsi0_base)) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Read register 0x0A timeout\n");
		goto check_fail;
	}
	if ((0x9c != (read_value & 0xFF))) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Register 0x0A value is wrong: 0x%x\n", read_value);
		goto check_fail;
	}

	read_value = 0xFF;
	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0B06);
	if (!mipi_dsi_read(&read_value, mipi_dsi0_base)) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Read register 0x0B timeout\n");
		goto check_fail;
	}
	if ((0x00 != (read_value & 0xFF))) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Register 0x0B value is wrong: 0x%x\n", read_value);
		goto check_fail;
	}

	read_value = 0xFF;
	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0C06);
	if (!mipi_dsi_read(&read_value, mipi_dsi0_base)) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Read register 0x0C timeout\n");
		goto check_fail;
	}
	if ((0x77 != (read_value & 0xFF))) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Register 0x0C value is wrong: 0x%x\n", read_value);
		goto check_fail;
	}

	read_value = 0xFF;
	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0D06);
	if (!mipi_dsi_read(&read_value, mipi_dsi0_base)) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Read register 0x0D timeout\n");
		goto check_fail;
	}
	if ((0x00 != (read_value & 0xFF))) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Register 0x0D value is wrong: 0x%x\n", read_value);
		goto check_fail;
	}

check_fail:
	return ret;
}
#endif

#ifdef CONFIG_LCD_MIPI_DETECT
static int mipi_jdi_mipi_detect(struct platform_device *pdev)
{
	struct k3_fb_data_type *k3fd = NULL;
	u32 err_bit = 0;
	u32 err_num = 0;
	int ret;
	char __iomem *mipi_dsi0_base = NULL;

	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;

	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0E06);
	if (!mipi_dsi_read(&err_bit, mipi_dsi0_base)) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Read register 0x0E timeout\n");
		goto read_reg_failed;
	}

	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0x0506);
	if (!mipi_dsi_read(&err_num, mipi_dsi0_base)) {
		ret = LCD_CHECK_REG_FAIL;
		K3_FB_ERR("Read register 0x05 timeout\n");
		goto read_reg_failed;
	}

	ret = ((err_bit & 0xFF) << 8) | (err_num & 0xFF);
	if (ret != 0x8000)
		K3_FB_INFO("ret_val: 0x%x\n", ret);
read_reg_failed:
	return ret;
}
#endif

#ifdef CONFIG_LCD_GRAM_CHECKSUM
static int mipi_jdi_panel_gram_checksum(struct platform_device *pdev, unsigned char *buf)
{
	struct k3_fb_data_type *k3fd = NULL;
	u32 read_value;
	int i;
	int ret = LCD_GRAM_CHECKSUM_PASS;
	char __iomem *mipi_dsi0_base = NULL;
	BUG_ON(pdev == NULL);
	k3fd = platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);
	mipi_dsi0_base = k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;

	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, (0xF0 << 16) | (0xFF << 8) | 0x15);
	if (buf[0] == LCD_GRAM_CHECKSUM_ENABLE) { //enable
		K3_FB_INFO("enable\n");
		outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, (0x00 << 16) | (0x7B << 8) | 0x15);
		outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, (0x01 << 16) | (0x92 << 8) | 0x15);
	} else if (buf[0] == LCD_GRAM_CHECKSUM_DISABLE) { //disable
		K3_FB_INFO("disable\n");
		outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, (0x00 << 16) | (0x92 << 8) | 0x15);
	} else if (buf[0] == LCD_GRAM_CHECKSUM_READ) { //read
		K3_FB_INFO("read\n");
		outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, (0x1 << 8) | 0x37);
		for (i = 0; i < 8; i++) {
			read_value = 0xFF;
			outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, ((0x73 + i) << 8) | 0x06);
			if (!mipi_dsi_read(&read_value, mipi_dsi0_base)) {
				K3_FB_ERR("Gram_checksum read register timeout\n");
				ret = LCD_GRAM_CHECKSUM_FAIL;
			}
			buf[i] = (unsigned char)read_value;
		}
		K3_FB_INFO("result:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	}
	outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, (0x10 << 16) | (0xFF << 8) | 0x15);
	return ret;
}
#endif

static struct k3_panel_info jdi_panel_info = {0};
static struct k3_fb_panel_data jdi_panel_data = {
	.panel_info = &jdi_panel_info,
	.set_fastboot = mipi_jdi_panel_set_fastboot,
	.on = mipi_jdi_panel_on,
	.off = mipi_jdi_panel_off,
	.remove = mipi_jdi_panel_remove,
	.set_backlight = mipi_jdi_panel_set_backlight,
	.set_display_region = mipi_jdi_panel_set_display_region,
#ifdef CONFIG_LCD_CHECK_REG
	.lcd_check_reg = mipi_jdi_check_reg,
#endif
#ifdef CONFIG_LCD_MIPI_DETECT
	.lcd_mipi_detect = mipi_jdi_mipi_detect,
#endif
#ifdef CONFIG_LCD_GRAM_CHECKSUM
	.lcd_gram_checksum = mipi_jdi_panel_gram_checksum,
#endif
};

unsigned char get_mipi_jdi_NT35695_tp_color(void)
{
	u32 out = 0;
	u8 lcd_chip_id = 0xFF;
	char __iomem *mipi_dsi0_base = NULL;
	u8 retry_nums = 3;

	if (!g_k3fd->panel_power_on) {
		K3_FB_ERR("power off\n");
		return lcd_chip_id;
	}

	/*enable vsync to avoid mipi into ULPS*/
	k3fb_activate_vsync(g_k3fd);
	do {
		mipi_dsi0_base = g_k3fd->dss_base + DSS_MIPI_DSI0_OFFSET;
		/*Read LCD ID*/
		outp32(mipi_dsi0_base + MIPIDSI_GEN_HDR_OFFSET, 0xDA06);
		if (!mipi_dsi_read(&out, mipi_dsi0_base)) {
			K3_FB_ERR("Read register 0xDA timeout\n");
		} else
			break;
		msleep(5);
	} while (-- retry_nums);

	k3fb_deactivate_vsync(g_k3fd);

	if (!retry_nums) {
		K3_FB_ERR("Read register 0xDA Failed\n");
		return lcd_chip_id;
	}

	lcd_chip_id = (u8)out;
	K3_FB_INFO("LCD Id = %d, mipi_dsi0_base = %p\n", lcd_chip_id, mipi_dsi0_base);
	return lcd_chip_id;
};

static u32 get_bl_type(struct device_node *np)
{
	u32 bl_type = -1;
	int ret = 0;
	ret = of_property_read_u32(np, "lcd-bl-type", &bl_type);
	if (ret) {
		K3_FB_ERR("of_property_read_u32 fail\n");
		return -1;
	}
	return bl_type;
}

static int mipi_jdi_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct k3_panel_info *pinfo = NULL;
	struct device_node *np = NULL;
	struct class *k3_lcd_class;
	struct platform_device *this_dev = NULL;
	struct lcd_properities lcd_props;

	if (k3_fb_device_probe_defer(PANEL_MIPI_CMD))
		goto err_probe_defer;

	K3_FB_DEBUG("+.\n");

	np = of_find_compatible_node(NULL, NULL, DTS_COMP_JDI_NT35695);
	if (!np) {
		K3_FB_ERR("NOT FOUND device node %s!\n", DTS_COMP_JDI_NT35695);
		goto err_return;
	}

	gpio_lcd_bl_enable = of_get_named_gpio(np, "gpios", 0);  /*gpio_4_3, gpio_035*/
	gpio_lcd_reset = of_get_named_gpio(np, "gpios", 1);  /*gpio_4_5, gpio_037*/
	gpio_lcd_id = of_get_named_gpio(np, "gpios", 2);  /*gpio_4_6, gpio_038*/
	gpio_lcd_p5v5_enable = of_get_named_gpio(np, "gpios", 3);  /*gpio_5_1, gpio_041*/
	gpio_lcd_n5v5_enable = of_get_named_gpio(np, "gpios", 4);  /*gpio_5_2, gpio_042*/

	pdev->id = 1;
	/* init lcd panel info */
	pinfo = jdi_panel_data.panel_info;
	memset(pinfo, 0, sizeof(struct k3_panel_info));
	pinfo->xres = 1080;
	pinfo->yres = 1920;
	pinfo->width  = 76;  //mm
	pinfo->height = 135; //mm
	pinfo->type = PANEL_MIPI_CMD;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = LCD_RGB888;
	pinfo->bgr_fmt = LCD_RGB;

	if (get_bl_type(np) == 1)
		pinfo->bl_set_type = BL_SET_BY_MIPI;
	else
		pinfo->bl_set_type = BL_SET_BY_PWM;

	pinfo->bl_min = 1;
	pinfo->bl_max = 255;
	pinfo->vsync_ctrl_type = (VSYNC_CTRL_ISR_OFF |
		VSYNC_CTRL_MIPI_ULPS);

	pinfo->frc_enable = 0;
	pinfo->esd_enable = 0;
	pinfo->dirty_region_updt_support = 0;

	pinfo->sbl_support = 1;
	pinfo->color_temperature_support = 1;
	pinfo->smart_bl.strength_limit = 180;
	pinfo->smart_bl.calibration_a = 30;
	pinfo->smart_bl.calibration_b = 95;
	pinfo->smart_bl.calibration_c = 5;
	pinfo->smart_bl.calibration_d = 1;
	pinfo->smart_bl.t_filter_control = 5;
	pinfo->smart_bl.backlight_min = 480;
	pinfo->smart_bl.backlight_max = 4096;
	pinfo->smart_bl.backlight_scale = 0xff;
	pinfo->smart_bl.ambient_light_min = 14;
	pinfo->smart_bl.filter_a = 1738;
	pinfo->smart_bl.filter_b = 6;
	pinfo->smart_bl.logo_left = 0;
	pinfo->smart_bl.logo_top = 0;

	pinfo->ifbc_type = IFBC_TYPE_NON;

	pinfo->ldi.h_back_porch = 23;
	pinfo->ldi.h_front_porch = 50;
	pinfo->ldi.h_pulse_width = 20;
	pinfo->ldi.v_back_porch = 12;
	pinfo->ldi.v_front_porch = 14;
	pinfo->ldi.v_pulse_width = 4;

	pinfo->mipi.lane_nums = DSI_4_LANES;
	pinfo->mipi.color_mode = DSI_24BITS_1;
	pinfo->mipi.vc = 0;
	pinfo->mipi.dsi_bit_clk = 480;

	pinfo->pxl_clk_rate = 150*1000000;

	/* lcd vcc init */
	ret = vcc_cmds_tx(pdev, jdi_lcd_vcc_init_cmds,
		ARRAY_SIZE(jdi_lcd_vcc_init_cmds));
	if (ret != 0) {
		K3_FB_ERR("LCD vcc init failed!\n");
		goto err_return;
	}

	if (fastboot_display_enable) {
		vcc_cmds_tx(pdev, jdi_lcd_vcc_enable_cmds,
			ARRAY_SIZE(jdi_lcd_vcc_enable_cmds));
	}

	/* lcd pinctrl init */
	ret = pinctrl_cmds_tx(pdev, jdi_lcd_pinctrl_init_cmds,
		ARRAY_SIZE(jdi_lcd_pinctrl_init_cmds));
	if (ret != 0) {
	        K3_FB_ERR("Init pinctrl failed, defer\n");
		goto err_return;
	}

	/* alloc panel device data */
	ret = platform_device_add_data(pdev, &jdi_panel_data,
		sizeof(struct k3_fb_panel_data));
	if (ret) {
		K3_FB_ERR("platform_device_add_data failed!\n");
		goto err_device_put;
	}

	this_dev = k3_fb_add_device(pdev);

	BUG_ON(this_dev == NULL);
	g_k3fd = platform_get_drvdata(this_dev);
	BUG_ON(g_k3fd == NULL);
	/* for cabc */
	sema_init(&ct_sem, 1);
	g_csc_value[0] = 0;
	g_csc_value[1] = 0;
	g_csc_value[2] = 0;
	g_csc_value[3] = 0;
	g_csc_value[4] = 0;
	g_csc_value[5] = 0;
	g_csc_value[6] = 0;
	g_csc_value[7] = 0;
	g_csc_value[8] = 0;
	g_is_csc_set = 0;
	lcd_props.type = TFT;
	lcd_props.default_gamma = GAMMA25;
	p_tuning_dev = lcd_tuning_dev_register(&lcd_props, &sp_tuning_ops, (void *)this_dev);
	if (IS_ERR(p_tuning_dev)) {
		K3_FB_ERR("lcd_tuning_dev_register failed!\n");
		return -1;
	}
	k3_lcd_class = class_create(THIS_MODULE, "k3_lcd");
	if (IS_ERR(k3_lcd_class)) {
		K3_FB_ERR("k3_lcd class create failed\n");
		return 0;
	}
	k3_lcd_class->dev_attrs = k3_lcd_class_attrs;

	if (IS_ERR(device_create(k3_lcd_class, &pdev->dev, 0, NULL, "lcd_info"))) {
		K3_FB_ERR("lcd_info create failed\n");
		class_destroy(k3_lcd_class);
		return 0;
	}

	get_tp_color = get_mipi_jdi_NT35695_tp_color;

	K3_FB_DEBUG("-.\n");

	return 0;

err_device_put:
	platform_device_put(pdev);
err_return:
	return ret;
err_probe_defer:
	return -EPROBE_DEFER;

	return ret;
}

static const struct of_device_id k3_panel_match_table[] = {
	{
		.compatible = DTS_COMP_JDI_NT35695,
		.data = NULL,
	},
};
MODULE_DEVICE_TABLE(of, k3_panel_match_table);

static struct platform_driver this_driver = {
	.probe = mipi_jdi_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "mipi_jdi_NT35695",
		.of_match_table = of_match_ptr(k3_panel_match_table),
	},
};

static int __init mipi_jdi_panel_init(void)
{
	int ret = 0;

	if (read_lcd_type() != JDI_NT35695_LCD) {
		K3_FB_INFO("lcd type is not jdi_NT35695_LCD, return!\n");
		return ret;
	}

	ret = platform_driver_register(&this_driver);
	if (ret) {
		K3_FB_ERR("platform_driver_register failed, error=%d!\n", ret);
		return ret;
	}

	return ret;
}

module_init(mipi_jdi_panel_init);
