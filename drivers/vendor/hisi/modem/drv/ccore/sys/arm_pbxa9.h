/* arm_pbxa9.h - ARM RealView PBX-A9 board header file */

/*
 * Copyright (c) 2009 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,20oct09,z_l  created
*/

/*
This file contains I/O address and related constants for the
ARM RealView PBX-A9 board.
*/

#ifndef __INCarm_pbxa9h
#define __INCarm_pbxa9h

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_memmap.h"

#define ARMA9CTX_REGISTER_READ(reg) \
    *(volatile UINT32 *)(reg)

#define ARMA9CTX_REGISTER_WRITE(reg, data)\
    *(volatile UINT32 *)(reg) = (data)

/*
 * Resistors R3-R20 soldered onto the tile specify bits [31:14] of PERIPHBASE. 
 * By default R13, R14, R15, R16, R17 are present so the address is 0x1F000000.
 */
 
#define PBXA9_PERIPHBASE      HI_MDM_GIC_BASE_ADDR
//#define PBXA9_PERIPHBASE_SIZE SZ_4M

/* Snoop Control Unit */

#define PBXA9_SCU_BASE              (PBXA9_PERIPHBASE + 0x0)

#define PBXA9_SCU_CTRL              (PBXA9_SCU_BASE + 0x00)
#define PBXA9_SCU_CONFIG            (PBXA9_SCU_BASE + 0x04)
#define PBXA9_SCU_CPU_STATUS        (PBXA9_SCU_BASE + 0x08)
#define PBXA9_SCU_INVALIDATE_ALL    (PBXA9_SCU_BASE + 0x0C)
#define PBXA9_SCU_PM_CR             (PBXA9_SCU_BASE + 0x10)
#define PBXA9_SCU_PM_CNT_EVT0       (PBXA9_SCU_BASE + 0x14)
#define PBXA9_SCU_PM_CNT_EVT1       (PBXA9_SCU_BASE + 0x18)
#define PBXA9_SCU_PM_COUNT0         (PBXA9_SCU_BASE + 0x1C)
#define PBXA9_SCU_PM_COUNT1         (PBXA9_SCU_BASE + 0x20)
#define PBXA9_SCU_PM_COUNT2         (PBXA9_SCU_BASE + 0x24)
#define PBXA9_SCU_PM_COUNT3         (PBXA9_SCU_BASE + 0x28)
#define PBXA9_SCU_PM_COUNT4         (PBXA9_SCU_BASE + 0x2C)
#define PBXA9_SCU_PM_COUNT5         (PBXA9_SCU_BASE + 0x30)
#define PBXA9_SCU_PM_COUNT6         (PBXA9_SCU_BASE + 0x34)
#define PBXA9_SCU_PM_COUNT7         (PBXA9_SCU_BASE + 0x38)

/* Snoop Control Unit - control register masks */

#define PBXA9_SCU_CTRL_EN           (0x00000001)
#if 0
/* system controller */

#define PBXA9_SC_BASE            (0x10001000)
#define PBXA9_SC_SYSCTRL         (PBXA9_SC_BASE + 0)

#define SC_RESERVED              (0xFFF07CFF)
#define SC_REMAP_CLEAR           (0x00000100)
#define SC_REMAP_STATUS          (0x00000200)
#define SC_TIMER0_ENABLE_TIMCLK  (0x00008000)
#define SC_TIMER0_OVERRIDE       (0x00010000)
#define SC_TIMER1_ENABLE_TIMCLK  (0x00020000)
#define SC_TIMER1_OVERRIDE       (0x00040000)
#define SC_TIMER2_ENABLE_TIMCLK  (0x00080000)
#define SC_TIMER2_OVERRIDE       (0x00100000)
#define SC_TIMER3_ENABLE_TIMCLK  (0x00200000)
#define SC_TIMER3_OVERRIDE       (0x00400000)
#define SC_WATCHDOG_OVERRIDE     (0x00800000)

/* real time clock  */

#define PBXA9_RTC_DR_OFFSET   (0x00)  /* Data register */
#define PBXA9_RTC_MR_OFFSET   (0x04)  /* Match register */
#define PBXA9_RTC_LR_OFFSET   (0x08)  /* Load register */
#define PBXA9_RTC_CR_OFFSET   (0x0C)  /* Control register */
#define PBXA9_RTC_IMSC_OFFSET (0x10)  /* Interrupt mask set/clr register */
#define PBXA9_RTC_RIS_OFFSET  (0x14)  /* Raw Interrupt status register */
#define PBXA9_RTC_MIS_OFFSET  (0x18)  /* Masked interrupt status register */
#define PBXA9_RTC_ICR_OFFSET  (0x1C)  /* Interrupt clear register */

#define PBXA9_RTC_BASE        (0x10017000)
#define PBXA9_RTC_SIZE         SZ_4K

#define PBXA9_RTC_DR          (PBXA9_RTC_BASE + PBXA9_RTC_DR_OFFSET)
#define PBXA9_RTC_MR          (PBXA9_RTC_BASE + PBXA9_RTC_MR_OFFSET)
#define PBXA9_RTC_LR          (PBXA9_RTC_BASE + PBXA9_RTC_LR_OFFSET)
#define PBXA9_RTC_CR          (PBXA9_RTC_BASE + PBXA9_RTC_CR_OFFSET)
#define RTC_CR_START          (0x00000001)
#define PBXA9_RTC_IMSC        (PBXA9_RTC_BASE + PBXA9_RTC_IMSC_OFFSET)
#define PBXA9_RTC_RIS         (PBXA9_RTC_BASE + PBXA9_RTC_RIS_OFFSET)
#define PBXA9_RTC_MIS         (PBXA9_RTC_BASE + PBXA9_RTC_MIS_OFFSET)
#define PBXA9_RTC_ICR         (PBXA9_RTC_BASE + PBXA9_RTC_ICR_OFFSET)
#endif
/*
 * Generic Interrupt Controller
 * Note: FIQ is not handled within VxWorks so this is just IRQ
 */

#define PBXA9_GIC1_BASE             HI_MDM_GIC_BASE_ADDR

#define PBXA9_GIC_CPU_CONTROL       (PBXA9_GIC1_BASE + 0x0100)
#define PBXA9_GIC_CPU_PRIORITY      (PBXA9_GIC1_BASE + 0x0104)
#define PBXA9_GIC_CPU_POINT         (PBXA9_GIC1_BASE + 0x0108)
#define PBXA9_GIC_CPU_ACK           (PBXA9_GIC1_BASE + 0x010C)
#define PBXA9_GIC_CPU_END_INTR      (PBXA9_GIC1_BASE + 0x0110)
#define PBXA9_GIC_CPU_RUNNING       (PBXA9_GIC1_BASE + 0x0114)
#define PBXA9_GIC_CPU_PENDING       (PBXA9_GIC1_BASE + 0x0118)


/* 96 is the maximum interrupt number. It covers SGI, PPI and SPI */

//#define SYS_INT_LEVELS_MAX  (96)

/* interrupt distributor */

#define PBXA9_GIC_DIST_CONTROL      (0x1000)
#define PBXA9_GIC_DIST_CTRL_TYPE    (0x1004)
#define PBXA9_GIC_DIST_ENABLE_SET1  (0x1100)
#define PBXA9_GIC_DIST_ENABLE_SET2  (0x1104)
#define PBXA9_GIC_DIST_ENABLE_SET3  (0x1108)
#define PBXA9_GIC_DIST_ENABLE_CLR1  (0x1180)
#define PBXA9_GIC_DIST_ENABLE_CLR2  (0x1184)
#define PBXA9_GIC_DIST_ENABLE_CLR3  (0x1188)
#define PBXA9_GIC_DIST_PEND_SET1    (0x1200)
#define PBXA9_GIC_DIST_PEND_SET2    (0x1204)
#define PBXA9_GIC_DIST_PEND_SET3    (0x1208)
#define PBXA9_GIC_DIST_PEND_CLR1    (0x1280)
#define PBXA9_GIC_DIST_PEND_CLR2    (0x1284)
#define PBXA9_GIC_DIST_PEND_CLR3    (0x1288)
#define PBXA9_GIC_DIST_ACTIVE1      (0x1300)
#define PBXA9_GIC_DIST_ACTIVE2      (0x1304)
#define PBXA9_GIC_DIST_ACTIVE3      (0x1308)
#define PBXA9_GIC_DIST_PRIORITY1    (0x1400) /* Priority reg  0- 3 */
#define PBXA9_GIC_DIST_PRIORITY2    (0x1404) /* Priority reg  4- 7 */
#define PBXA9_GIC_DIST_PRIORITY3    (0x1408) /* Priority reg  8-11 */
#define PBXA9_GIC_DIST_PRIORITY4    (0x140C) /* Priority reg 12-15 */
#define PBXA9_GIC_DIST_PRIORITY5    (0x1410) /* Priority reg 16-19 */
#define PBXA9_GIC_DIST_PRIORITY6    (0x1414) /* Priority reg 20-23 */
#define PBXA9_GIC_DIST_PRIORITY7    (0x1418) /* Priority reg 24-27 */
#define PBXA9_GIC_DIST_PRIORITY8    (0x141C) /* Priority reg 28-31 */
#define PBXA9_GIC_DIST_PRIORITY9    (0x1420) /* Priority reg 32-35 */
#define PBXA9_GIC_DIST_PRIORITY10   (0x1424) /* Priority reg 36-39 */
#define PBXA9_GIC_DIST_PRIORITY11   (0x1428) /* Priority reg 40-43 */
#define PBXA9_GIC_DIST_PRIORITY12   (0x142C) /* Priority reg 44-47 */
#define PBXA9_GIC_DIST_PRIORITY13   (0x1430) /* Priority reg 48-51 */
#define PBXA9_GIC_DIST_PRIORITY14   (0x1434) /* Priority reg 52-55 */
#define PBXA9_GIC_DIST_PRIORITY15   (0x1438) /* Priority reg 56-59 */
#define PBXA9_GIC_DIST_PRIORITY16   (0x143C) /* Priority reg 60-63 */
#define PBXA9_GIC_DIST_TARG         (0x1800)
#define PBXA9_GIC_DIST_CONFIG1      (0x1C00)
#define PBXA9_GIC_DIST_CONFIG2      (0x1C04)
#define PBXA9_GIC_DIST_CONFIG3      (0x1C08)
#define PBXA9_GIC_DIST_CONFIG4      (0x1C0C)
#define PBXA9_GIC_DIST_CONFIG5      (0x1C10)
#define PBXA9_GIC_DIST_CONFIG6      (0x1C14)
#define PBXA9_GIC_DIST_SOFTWARE     (0x1F00)
#define PBXA9_GIC_DIST_PERIPH_ID    (0x1FD0)
#define PBXA9_GIC_DIST_CELL_ID      (0x1FFC)

//#define PBXA9_SMC_ENET_BASE       (0x4E000000)  /* ENET base */
//#define PBXA9_SMC_ENET_SIZE       SZ_16M
#if 0
/* signals generated from various clock generators */

#define PBXA9_OSCCLK0         (25000000)    /* System bus clock (AHB) */
#define PBXA9_OSCCLK1             
#define PBXA9_OSCCLK2             
#define PBXA9_OSCCLK3             
#define PBXA9_OSCCLK4           
#define PBXA9_REFCLK24MHZ     (24000000)
#define PBXA9_REFCLK32K       (32786)

#define PBXA9_ETHERNET_CLK    (25000000)          /* Fixed 25MHz */
#define PBXA9_PCI_CLK         (66000000)          /* PCI clock */
#define PBXA9_RTC_CLK         PBXA9_REFCLK32K     /* Real time clock */
#define PBXA9_SSP_CLK         PBXA9_REFCLK24MHZ   /* Sync Serial Port */
#define PBXA9_SCI_CLK         PBXA9_REFCLK24MHZ   /* Smart Card */
#define PBXA9_SMC_CLK         PBXA9_OSCCLK0       /* Static mem ctrl */
#define PBXA9_TIMERS_CLK      (1000000)           /* Real time clock */
#define PBXA9_UART_CLK        PBXA9_REFCLK24MHZ   /* UART clock */
#define PBXA9_WD_CLK          PBXA9_REFCLK24MHZ   /* Watchdog timer */

/* definitions for the AMBA UART */

#define UART_XTAL_FREQ        PBXA9_UART_CLK
#define N_SIO_CHANNELS        N_AMBA_UART_CHANNELS
#define N_UART_CHANNELS       N_AMBA_UART_CHANNELS
#define N_AMBA_UART_CHANNELS  (2)

#define UART_0_BASE_ADR       (0x10009000)    /* UART 0 base address */
#define UART_1_BASE_ADR       (0x1000A000)    /* UART 1 base address */

#endif
#define N_SIO_CHANNELS        (1)

/* definitions for the AMBA Timer */
#if 0
#define AMBA_TIMER_BASE        (0x10011000)  /* start of timer 0 */
#define AMBA_TIMER_REGS_SIZE   (2 * SZ_4K)   /* two dual timers 0,1 & 2,3 */

#define AMBA_TIMER0_BASE     (0x10011000)
#define AMBA_TIMER1_BASE     (0x10011020)
#define AMBA_TIMER2_BASE     (0x10012000)
#define AMBA_TIMER3_BASE     (0x10012020)

#define SYS_TIMER_BASE       AMBA_TIMER3_BASE
#define AUX_TIMER_BASE       AMBA_TIMER0_BASE

#define SYS_TIMER_INT_LVL    (INT_LVL_TIMER_2_3)
#define AUX_TIMER_INT_LVL    (INT_LVL_TIMER_0_1)

/* add corresponding INT_VEC */

#define SYS_TIMER_INT_VEC (INT_VEC_TIMER_0_1)
#define AUX_TIMER_INT_VEC (INT_VEC_TIMER_2_3)

/* frequency of counter/timers */

#define SYS_TIMER_CLK    (PBXA9_TIMERS_CLK)
#define AUX_TIMER_CLK    (PBXA9_TIMERS_CLK)

#define SYS_CLK_RATE_MIN (10)
#define SYS_CLK_RATE_MAX (8000)

#define AUX_CLK_RATE_MIN (10)
#define AUX_CLK_RATE_MAX (8000)
#endif
/* MPCore Tile Interrupt registers */

//#define PBXA9_MPCORE_CPU_INTERFACE   (PBXA9_GIC1_BASE)
//#define PBXA9_MPCORE_CONTROL_REG     (0x00)
//#define PBXA9_MPCORE_PRIO_MASK_REG   (0x04)

/* PCI definitions */

//#define BUS        BUS_TYPE_PCI

#if 0
/*
 * Max number of END devices we support - we currently set to three for
 * 2 PCI slots + 1 integrated END
 */

#define PBXA9_MAX_END_DEVS (3)

/* Bit field definitions */

#define BIT0                            (0x00000001)
#define BIT1                            (0x00000002)
#define BIT2                            (0x00000004)
#define BIT3                            (0x00000008)
#define BIT4                            (0x00000010)
#define BIT5                            (0x00000020)
#define BIT6                            (0x00000040)
#define BIT7                            (0x00000080)
#define BIT8                            (0x00000100)
#define BIT9                            (0x00000200)
#define BIT10                           (0x00000400)
#define BIT11                           (0x00000800)
#define BIT12                           (0x00001000)
#define BIT13                           (0x00002000)
#define BIT14                           (0x00004000)
#define BIT15                           (0x00008000)
#define BIT16                           (0x00010000)
#define BIT17                           (0x00020000)
#define BIT18                           (0x00040000)
#define BIT19                           (0x00080000)
#define BIT20                           (0x00100000)

/* Handy sizes */

#define SZ_1K                           (0x00000400)
#define SZ_4K                           (0x00001000)
#define SZ_8K                           (0x00002000)
#define SZ_16K                          (0x00004000)
#define SZ_64K                          (0x00010000)
#define SZ_128K                         (0x00020000)
#define SZ_256K                         (0x00040000)
#define SZ_512K                         (0x00080000)

#define SZ_1M                           (0x00100000)
#define SZ_2M                           (0x00200000)
#define SZ_4M                           (0x00400000)
#define SZ_8M                           (0x00800000)
#define SZ_16M                          (0x01000000)
#define SZ_32M                          (0x02000000)
#define SZ_64M                          (0x04000000)
#define SZ_128M                         (0x08000000)
#define SZ_256M                         (0x10000000)
#define SZ_512M                         (0x20000000)

#define SZ_1G                           (0x40000000)
#define SZ_2G                           (0x80000000)

#define SCTLR_BE  (0x02000000)

#define ARM_IMM #

#define ARM_LOC_MASK #0x7000000     /* level of coherency mask of CLIDR */

#define CACHE_DC_ENABLE   (1<<2)    /* (data) cache enable */
#define CACHE_IC_ENABLE   (1<<12)   /* Instruction cache enable */

/* Auxiliary Control Register */

#define AUX_CTL_REG_FW        (0x00000001) /* cache and tlb maintenance broadcast */
#define AUX_CTL_REG_L1_PRE_EN (0x00000004) /* L1 prefetch enable */
#endif
/* 
 * Common code for cache operations on entire data/unified caches,
 * performed by set/way to the point of coherency (PoC).
 * This code is based on 'Example code for cache maintenance operations'
 * provided in "ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition 
 * (ARM DDI 0406)" .
 * 
 * Registers used: r0-r8. Also note that r0 is 0 when this code completes.
 */

#define _CORTEX_AR_ENTIRE_DATA_CACHE_OP(crm) \
                    \
    MRC    p15, 1, r0, c0, c0, 1    /* r0 = Cache Lvl ID register info */;\
    ANDS   r3, r0, ARM_LOC_MASK     /* get level of coherency (LoC) */;\
    MOV    r3, r3, LSR ARM_IMM 23   /* r3 = LoC << 1 */;\
    BEQ    5f            ;\
                    \
    MOV    r7, ARM_IMM 0            /* r7 = cache level << 1; start at 0 */;\
                    \
1:                    ;\
    AND    r1, r0, ARM_IMM 0x7      /* r1 = cache type(s) for this level */;\
    CMP    r1, ARM_IMM 2        ;\
    BLT    4f                       /* no data cache at this level */;\
                    \
    MCR    p15, 2, r7, c0, c0, 0    /* select the Cache Size ID register */;\
    MCR    p15, 0, r7, c7, c5, 4    /* ISB: sync change to Cache Size ID */;\
    MRC    p15, 1, r1, c0, c0, 0    /* r1 = current Cache Size ID info */;\
    AND    r2, r1, ARM_IMM 0x7      /* r2 = line length */;\
    ADD    r2, r2, ARM_IMM 4        /* add line length offset = log2(16 bytes) */;\
    LDR    r4, =0x3FF        ;\
    ANDS   r4, r4, r1, LSR ARM_IMM 3 /* r4 = (# of ways - 1); way index */;\
    CLZ    r5, r4                    /* r5 = bit position of way size increment */;\
    LDR    r6, =0x00007FFF        ;\
    ANDS    r6, r6, r1, LSR ARM_IMM 13/* r6 = (# of sets - 1); set index */;\
                    \
2:                    ;\
    MOV    r1, r4                 /* r1 = working copy of way number */;\
3:                    ;\
    ORR    r8, r7, r1, LSL r5     /* r8 = set/way operation data word: */;\
    ORR    r8, r8, r6, LSL r2     /* cache level, way and set info */;\
                    \
    MCR    p15, 0, r8, c7, crm, 2 /* dcache operation by set/way */;\
                    \
    SUBS   r1, r1, ARM_IMM 1      /* decrement the way index */;\
    BGE    3b            ;\
    SUBS   r6, r6, ARM_IMM 1      /* decrement the set index */;\
    BGE    2b            ;\
                    \
4:                    ;\
    ADD    r7, r7, ARM_IMM 2      /* increment cache index = level << 1 */;\
    CMP    r3, r7                 /* done when LoC is reached */;\
    MOVGT  r0, r0, LSR ARM_IMM 3  /* rt-align type of next cache level */;\
    BGT    1b            ;\
                    \
5:                    ;\


#ifdef __cplusplus
}
#endif

#endif    /* __INCarm_pbxa9h */
