/*
 * bsp_uart.h 
 *
 * Copyright (C) 2012 Huawei Corporation
 * 
 *author:wangxiandong
 */
#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include <product_config.h>

#ifdef CONFIG_CCORE_PM
int balongv7r2_uart_suspend();
int balongv7r2_uart_resume();
#else
static inline int balongv7r2_uart_suspend() {return 0;}
static inline int balongv7r2_uart_resume() {return 0;}
#endif
int balongv7r2_m_uart_suspend();
int balongv7r2_m_uart_resume();
void printksync(const char *fmt, ...);
#endif

