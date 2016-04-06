/*
This file contains I/O address and related constants for the balongv7r2 clk .
*/

#ifndef __SOC_CLK_H
#define __SOC_CLK_H
#include <product_config.h>
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
                  * soc clk *
***************************************************************************/

#define HI_TCXO_CLK                 (32*1024)
#ifdef BSP_CONFIG_BOARD_SFT
#define HI_UART_CLK                 (20000000)
#elif defined(BSP_CONFIG_K3V3_ASIC)
#define HI_UART_CLK                 (19200000)
#endif
#define HI_HSUART_CLK               (48000000)
#define HI_TIMER_CLK                (19200000)
#define HI_TIMER_TCXO_CLK           TCXO_CLK
#define HI_TIMER_STAMP_CLK           (32*1024)
#define HI_SCI0_CLK                 (30000000)
#define HI_SCI1_CLK                 (30000000)

/*#define HI_APB_CLK                  (25000000)*/
#ifdef BSP_CONFIG_V7R2_PORTING
#define HI_TIMER0_CLK               (19200000)
#define HI_TIMER1_CLK               (19200000)
#define HI_TIMER2_CLK               (19200000)
#define HI_TIMER3_CLK               (19200000)
#define HI_TIMER4_CLK               (19200000)
#define HI_TIMER5_CLK               (19200000)
#define HI_TIMER6_CLK               (19200000)
#define HI_TIMER7_CLK               (32*1024)
#define HI_TIMER8_CLK               (19200000)
#define HI_TIMER9_CLK               (19200000)
#define HI_TIMER10_CLK              (19200000)
#define HI_TIMER11_CLK              (19200000)
#define HI_TIMER12_CLK              (19200000)
#define HI_TIMER13_CLK              (19200000)
#define HI_TIMER14_CLK              (19200000)
#define HI_TIMER15_CLK              (19200000)
#define HI_TIMER16_CLK              (19200000)
#define HI_TIMER17_CLK              (19200000)
#define HI_TIMER18_CLK              (19200000)
#define HI_TIMER19_CLK              (19200000)
#define HI_TIMER20_CLK              (19200000)
#define HI_TIMER21_CLK              (19200000)
#define HI_TIMER22_CLK              (19200000)
#define HI_TIMER23_CLK              (19200000)
#elif defined(BSP_CONFIG_V7R2_SFT)
#define HI_TIMER0_CLK               (32*1024)
#define HI_TIMER1_CLK               (32*1024)
#define HI_TIMER2_CLK               (32*1024)
#define HI_TIMER3_CLK               (32*1024)
#define HI_TIMER4_CLK               (32*1024)
#define HI_TIMER5_CLK               (32*1024)
#define HI_TIMER6_CLK               (19200000)
#define HI_TIMER7_CLK               (19200000)
#define HI_TIMER8_CLK               (19200000)
#define HI_TIMER9_CLK               (19200000)
#define HI_TIMER10_CLK              (32*1024)
#define HI_TIMER11_CLK              (32*1024)
#define HI_TIMER12_CLK              (32*1024)
#define HI_TIMER13_CLK              (19200000)
#define HI_TIMER14_CLK              (19200000)
#define HI_TIMER15_CLK              (19200000)
#define HI_TIMER16_CLK              (19200000)
#define HI_TIMER17_CLK              (19200000)
#define HI_TIMER18_CLK              (32*1024)
#define HI_TIMER19_CLK              (32*1024)
#define HI_TIMER20_CLK              (32*1024)
#define HI_TIMER21_CLK              (19200000)
#define HI_TIMER22_CLK              (32*1024)
#define HI_TIMER23_CLK              (32*1024)
#elif defined(BSP_CONFIG_V7R2_ASIC)
#define HI_TIMER0_CLK               (32*1024)
#define HI_TIMER1_CLK               (32*1024)
#define HI_TIMER2_CLK               (32*1024)
#define HI_TIMER3_CLK               (32*1024)
#define HI_TIMER4_CLK               (32*1024)
#define HI_TIMER5_CLK               (32*1024)
#define HI_TIMER6_CLK               (19200000)
#define HI_TIMER7_CLK               (19200000)
#define HI_TIMER8_CLK               (19200000)
#define HI_TIMER9_CLK               (32*1024)
#define HI_TIMER10_CLK              (32*1024)
#define HI_TIMER11_CLK              (32*1024)
#define HI_TIMER12_CLK              (19200000)
#define HI_TIMER13_CLK              (19200000)
#define HI_TIMER14_CLK              (19200000)
#define HI_TIMER15_CLK              (19200000)
#define HI_TIMER16_CLK              (19200000)
#define HI_TIMER17_CLK              (32*1024)
#define HI_TIMER18_CLK              (32*1024)
#define HI_TIMER19_CLK              (32*1024)
#define HI_TIMER20_CLK              (32*1024)
#define HI_TIMER21_CLK              (32*1024)
#define HI_TIMER22_CLK              (32*1024)
#define HI_TIMER23_CLK              (32*1024)

#endif
/*v7r2porting M3 cpu frequency 36MHZ*/
#define CFG_M3_CLK_CPU              (36000000)

#ifdef __cplusplus
}
#endif

#endif    /* __SOC_CLK_H */
