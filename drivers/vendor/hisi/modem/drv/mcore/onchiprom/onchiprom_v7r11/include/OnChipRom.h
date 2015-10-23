/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  OnChipRom.h
*
*   作    者 :  wuzechun
*
*   描    述 :  OnChipRom通用头文件
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __ONCHIPROM_H__
#define __ONCHIPROM_H__


#include "config.h"

/* 平台类型定义，实际的平台类型由Makefile传入 */
#define PLATFORM_ASIC      1
#define PLATFORM_PORTING   2
#define PLATFORM_EDA       3

#define YES                1
#define NO                 0

#define DOWNLOAD_BL_LEN_INDEX   (44)      /*bootrom镜像长度偏移*/
#define BL_LEN_INDEX            (144 * 4)    /* bootload.bin文件长度(Byte)保存在bootload.bin文件中的偏移字节数 */
#define ROOT_CA_INDEX           (BL_LEN_INDEX + 4)   /* 根CA在Image中位置 */
#define BL_SEC_INDEX            (ROOT_CA_INDEX)

#define WORD_SIZE   (sizeof(unsigned int)/sizeof(char))

#define TIMEMAXDELAY    0x1000000       /*最大延迟时间*/
#define SPI_MAX_DELAY_TIMES 0x10000

#define INTEGRATOR_DELAYFORSLAVE_VALUE 0x10100

/*判断镜像是否烧写*/
#define BL_CHECK_ADDR_OFFSET    0x368  /*判断镜像固定字符的偏移地址*/
#define BL_CHECK_INSTRUCTION    0x79706F43 /*用来判断镜像是否烧写的固定字符指令 */
#define BL_CHECK_ADDR (M3_TCM_BL_ADDR + BL_CHECK_ADDR_OFFSET) /*判断镜像中固定字符的绝对地址*/

/* EFUSE空间划分*/
#define EFUSE_GRP_ROOT_CA   0x00
#define EFUSE_GRP_CFG       (EFUSE_MAX_SIZE / WORD_SIZE - 1)



/*
 * System Ctrl module register define
 */


/*系统控制器*/
#define INTEGRATOR_SC_BASE              0x90000000
#define INTEGRATOR_SC_SIZE              (4*0x400)   /* 4K */

#define SC_CTRL3        (INTEGRATOR_SC_BASE+0x40C)  /*外设配置寄存器*/
#define SC_CRG_CTRL0    (INTEGRATOR_SC_BASE+0x00)
#define SC_CRG_CTRL3    (INTEGRATOR_SC_BASE+0x0C)  /* 时钟使能2 */
#define SC_CRG_CTRL6    (INTEGRATOR_SC_BASE+0x18)  /* 时钟使能3 */
#define SC_CRG_CTRL9    (INTEGRATOR_SC_BASE+0x24)  /* 时钟使能4 */
#define SC_CRG_CTRL12    (INTEGRATOR_SC_BASE+0x30)  /* 时钟选择配置1 */
#define SC_CRG_CTRL13    (INTEGRATOR_SC_BASE+0x34)  /* 时钟选择配置2 */
#define SC_CRG_CTRL14    (INTEGRATOR_SC_BASE+0x38)  /* 软复位请求1 */
#define SC_CRG_CTRL15    (INTEGRATOR_SC_BASE+0x3C)  /* 软复位请求2 */
#define SC_CRG_CTRL18    (INTEGRATOR_SC_BASE+0x48)  /* 软复位请求2 */
#define SC_CRG_CTRL20   (INTEGRATOR_SC_BASE+0x50)  /* A9 DFS配置1 */
#define SC_CRG_CTRL21   (INTEGRATOR_SC_BASE+0x54)  /* A9 DFS配置2 */
#define SC_CRG_CTRL22   (INTEGRATOR_SC_BASE+0x58)  /* A9 DFS配置3 */
#define SC_CRG_CTRL25   (INTEGRATOR_SC_BASE+0x64)  /* DSP DFS配置1 */
#define SC_CRG_CTRL28   (INTEGRATOR_SC_BASE+0x70)  /* BBP1 DFS配置3 */
#define SC_CRG_CTRL31   (INTEGRATOR_SC_BASE+0x7C)  /* BBP2 DFS配置2 */
#define SC_CRG_CTRL34   (INTEGRATOR_SC_BASE+0x88)  /* PERI DFS配置2 */
#define SC_CRG_CTRL37   (INTEGRATOR_SC_BASE+0x94)  /* USB DFS配置3 */
#define SC_CRG_DIV3     (INTEGRATOR_SC_BASE+0x108) /* CLK DIV 3 */
#define SC_CRG_MMC_CTRL (INTEGRATOR_SC_BASE+0x114) /* EMMC配置 */

#define SSP1_CTRL1      (1<<2)
#define SC_IOS_CTRL81   (INTEGRATOR_SC_BASE+0x944)  /* SSP0/SSP1接口管脚配置寄存器 */
#define SSP0_WIRE_4 (1<<14)
#define SSP0_CS1_EN (1<<15)
#define SSP1_WIRE_4 (1<<30)
#define SSP1_CS1_EN (1UL<<31)

#define SC_IOS_CTRL98   (INTEGRATOR_SC_BASE+0x988)  /* IO复用管脚配置 */
#define SSP0_CTRL       (1<<11)
#define SSP1_CTRL       (1<<15)
#define MMC1_CTRL       (1<<22)
#define JTAG1_CTRL      (1<<1)
#define MMC2_CTRL       (1<<25)
#define GMII_CTRL       (1<<21)
#define PTM_CTRL        (1<<23)
#define IOS_NAND_DATA8_15  (1<<2)

#define SC_STAT0        (INTEGRATOR_SC_BASE+0x0600)

#define SC_CTRL_REGBASE_ADDR    0x9000C000

#define SC_BOOTMODE_BITPOS          0
#define SC_BOOTMODE_BITWIDTH        3
#define SC_BOOTMODE_BITMASK         (((1<<SC_BOOTMODE_BITWIDTH)-1)<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_EMMC              (0x01<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_NAND_BOOT         (0x02<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_NAND_EFUSE        (0x03<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_SPI               (0x04<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_AP_HSIC           (0x05<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_AP_HSUART         (0x06<<SC_BOOTMODE_BITPOS)
#define BOOT_MODE_NAND_ID           (0x00<<SC_BOOTMODE_BITPOS)

#define SC_AUTO_ENUM_EN_BITPOS      3
#define SC_AUTO_ENUM_EN_BITWIDTH    1
#define SC_AUTO_ENUM_EN_BITMASK     (((1<<SC_AUTO_ENUM_EN_BITWIDTH)-1)<<SC_AUTO_ENUM_EN_BITPOS)
#define SC_AUTO_ENUM_EN             (0<<SC_AUTO_ENUM_EN_BITPOS)

/*
 * Power Ctrl module register define
 */
#define PWR_CTRL_BASE   0x90000C00
#define PWR_CTRL0       (PWR_CTRL_BASE + 0x00)
#define PWR_CTRL13      (PWR_CTRL_BASE + 0x34)

/*
 * UART module register define
 */
#define UART0_BASE (INTEGRATOR_SC_BASE+0x7000)
#define UART_RBR   0x0
#define UART_THR   0x0
#define UART_DLL   0x0
#define UART_IER   0x4
#define UART_DLH   0x4
#define UART_IIR   0x8
#define UART_FCR   0x8
#define UART_LCR   0xC
#define UART_MCR   0x10
#define UART_LSR   0x14
#define UART_TXDEPTH 0x2C
#define UART_DEBUG 0x28
#define UART_FAR   0x70
#define UART_TFR   0x74
#define UART_RFW   0x78
#define UART_USR   0x7C
#define UART_TFL   0x80
#define UART_RFL   0x84
#define UART_HTX   0xA4
#define UART_DMASA 0xA8
#define UART_CPR   0xF4

/***********SPI*****************/
#define ECS_SPI0_BASE       (INTEGRATOR_SC_BASE + 0x00008000)
#define ECS_SPI0_SIZE		0x00001000

#define ECS_SPI1_BASE       (INTEGRATOR_SC_BASE + 0x00023000)
#define ECS_SPI1_SIZE		0x00001000

#if PLATFORM==PLATFORM_PORTING
/* OnChipRom E2PROM使用SPI0 CS0 */ /*need make sure*/
#define SPI_NO_DEV          0       /*SPI0*/
#define SPI_CS_EEPROM       (1<<0)   /* EEPROM使用SSI0 CS0 */
#define SPI_CS_SFLASH       (1<<1)   /* EEPROM使用SSI0 CS1 */
/*for porting test*/
//#define SPI_CS_DEV          SPI_CS_EEPROM
#define SPI_CS_DEV          SPI_CS_SFLASH
#else
/* ASIC OnChipRom E2PROM/SFlash使用SPI1 CS0 */
#define SPI_NO_DEV          1          /*SPI1*/
#define SPI_CS_DEV          (1<<0)     /* CS1 for EEPROM/SFlash */
#endif

/* PMU使用SPI0 CS0 */
#define SPI_NO_PMU          0
#define SPI_CS_PMU          (1<<0)     /* CS0,for eMMC/MMC/SD */

/*************************************************
*
*  NANDC   base address definition
*
*************************************************/
#define NANDC_BASE          0x900A4000
#define NANDC_SIZE          0x0001000

#define NANDC_CON           (NANDC_BASE + 0x00)
#define NANDC_OPMODE_OS     0  /* 1为Normal模式,0为Boot模式 */
#define NANDC_OPMODE_WIDTH  1
#define NANDC_OPMODE_MASK  (((1<<NANDC_OPMODE_WIDTH)-1)<<NANDC_OPMODE_OS)
#define NANDC_OPMODE_BOOT   (0<<NANDC_OPMODE_OS)  /* 1为Normal模式,0为Boot模式 */
#define NAND_ECC_TYPE_OS    9
#define NAND_ECC_TYPE_WIDTH 3
#define NAND_ECC_TYPE_MASK  (((1<<NAND_ECC_TYPE_WIDTH)-1)<<NAND_ECC_TYPE_OS)
#define NAND_ECC_TYPE_0     (0)
#define NAND_ECC_TYPE_1     (1<<NAND_ECC_TYPE_OS)
#define NAND_ECC_TYPE_4     (2<<NAND_ECC_TYPE_OS)
#define NAND_ECC_TYPE_24_512K    (3<<NAND_ECC_TYPE_OS)
#define NAND_ECC_TYPE_24_1K    (4<<NAND_ECC_TYPE_OS)

#define NANDC_PWIDTH        (NANDC_BASE + 0x04)

/* ECC错误检测,中断方式,不可靠 */
#define NANDC_INTEN         (NANDC_BASE + 0x24)
#define ECC_ERR_INVALID     (1<<6)
#define NANDC_INTS          (NANDC_BASE + 0x28)
/* ECC错误检测,使用本方式 */
#define NANDC_ECC_TEST      (NANDC_BASE + 0x5C)

/* WDT */
#define SC_WDTCTRL          (INTEGRATOR_SC_BASE + 0x0408)
#define WDT_REGBASE_ADDR    (INTEGRATOR_SC_BASE + 0x1000)

#define WDG_LOAD    (WDT_REGBASE_ADDR + 0x000)   /* 计数初值寄存器 */
#define WDG_VALUE   (WDT_REGBASE_ADDR + 0x004)   /* 计数器当前值寄存器 */
#define WDG_CONTROL (WDT_REGBASE_ADDR + 0x008)   /* 控制寄存器 */
#define WDG_INTCLR  (WDT_REGBASE_ADDR + 0x00C)   /* 中断清除寄存器 */
#define WDG_RIS     (WDT_REGBASE_ADDR + 0x010)   /* 原始中断寄存器 */
#define WDG_MIS     (WDT_REGBASE_ADDR + 0x014)   /* 屏蔽后中断寄存器 */
#define WDG_LOCK    (WDT_REGBASE_ADDR + 0xC00)   /* LOCK寄存器 */
#define WDT_LOCK_VALUE   0x1ACCE551

/*timer 0~7*/
#define TIMER_REGBASE_0TO7                0x90002000
#define TIMER0TO7_REG_SIZE                (0x14)

/* timer no */
#define CRG_CLK_SEL2            0x90000144
#define CRG_CLK_SEL2_TIMER0     0
#define CRG_CLK_SEL2_TIMER6     6
#define CRG_CLK_SEL2_TIMER5     5

#define EMMC_TIMER_NO           (0)
#define TIMER_NO_6              (6)

/* timer freq */
#define TIMER0_FREQ             (19200*1000) /* HZ */
#define TIMER6_FREQ             (19200*1000) /* HZ */

#define US_PER_SECEND           1000000
#define MS_TICKS                (TIMER0_FREQ/1000)

/* timer0 register */
#define TIMER0_REGOFF_LOAD	       (TIMER_REGBASE_0TO7 + 0x000)    /* Load (R/W) */
#define TIMER0_REGOFF_VALUE        (TIMER_REGBASE_0TO7 + 0x004)    /* Value (R/O) */
#define TIMER0_REGOFF_CTRL         (TIMER_REGBASE_0TO7 + 0x008)    /* Control (R/W) */
#define TIMER0_REGOFF_CLEAR        (TIMER_REGBASE_0TO7 + 0x00C)    /* Clear (W/O) */
#define TIMER0_REGOFF_INTSTATUS    (TIMER_REGBASE_0TO7 + 0x010)    /* INT STATUS (R/O) */

#define TIMER_CTRL_EN               (1<<0)
#define TIMER_CTRL_INTMASK          (1<<2)
#define TIMER_CTRL_EN_ACK           (1<<4)

#define TIMER_INS_INT               (1<<0)

/* timer6 register */
#define TIMER6_REGOFF_LOAD	       ((TIMER_REGBASE_0TO7+TIMER_NO_6*TIMER0TO7_REG_SIZE) + 0x000)    /* Load (R/W) */
#define TIMER6_REGOFF_VALUE        ((TIMER_REGBASE_0TO7+TIMER_NO_6*TIMER0TO7_REG_SIZE) + 0x004)    /* Value (R/O) */
#define TIMER6_REGOFF_CTRL         ((TIMER_REGBASE_0TO7+TIMER_NO_6*TIMER0TO7_REG_SIZE) + 0x008)    /* Control (R/W) */
#define TIMER6_REGOFF_CLEAR        ((TIMER_REGBASE_0TO7+TIMER_NO_6*TIMER0TO7_REG_SIZE) + 0x00C)    /* Clear (W/O) */
#define TIMER6_REGOFF_INTSTATUS    ((TIMER_REGBASE_0TO7+TIMER_NO_6*TIMER0TO7_REG_SIZE) + 0x010)    /* INT STATUS (R/O) */

#ifdef START_TIME_TEST
#define TIMER_NO_5              (5)
/* timer7 register */
#define TIMER5_REGOFF_LOAD	       ((TIMER_REGBASE_0TO7+TIMER_NO_5*TIMER0TO7_REG_SIZE) + 0x000)    /* Load (R/W) */
#define TIMER5_REGOFF_VALUE        ((TIMER_REGBASE_0TO7+TIMER_NO_5*TIMER0TO7_REG_SIZE) + 0x004)    /* Value (R/O) */
#define TIMER5_REGOFF_CTRL         ((TIMER_REGBASE_0TO7+TIMER_NO_5*TIMER0TO7_REG_SIZE) + 0x008)    /* Control (R/W) */
#define TIMER5_REGOFF_CLEAR        ((TIMER_REGBASE_0TO7+TIMER_NO_5*TIMER0TO7_REG_SIZE) + 0x00C)    /* Clear (W/O) */
#define TIMER5_REGOFF_INTSTATUS    ((TIMER_REGBASE_0TO7+TIMER_NO_5*TIMER0TO7_REG_SIZE) + 0x010)    /* INT STATUS (R/O) */

#define TIMER5_INIT_VALUE 0xFFFFFFFF
#endif

#endif /* end of onchiprom.h */

