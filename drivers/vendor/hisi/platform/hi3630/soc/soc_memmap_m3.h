

#ifndef __SOC_MEMMAP_M3_H__
#define __SOC_MEMMAP_M3_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "product_config.h"

/************************************************************************
                * size *
************************************************************************/
#ifndef SZ_512
#define SZ_512                          (0x00000200)
#endif

#ifndef SZ_1K
#define SZ_1K                            (0x00000400)
#endif

#ifndef SZ_2K
#define SZ_2K                            (0x00000800)
#endif

#ifndef SZ_4K
#define SZ_4K                            (0x00001000)
#endif

#ifndef SZ_8K
#define SZ_8K                            (0x00002000)
#endif

#ifndef SZ_16K
#define SZ_16K                           (0x00004000)
#endif

#ifndef SZ_20K
#define SZ_20K                           (0x00005000)
#endif

#ifndef SZ_32K
#define SZ_32K                           (0x00008000)
#endif

#ifndef SZ_64K
#define SZ_64K                           (0x00010000)
#endif

#ifndef SZ_80K
#define SZ_80K                           (0x00014000)
#endif

#ifndef SZ_96K
#define SZ_96K                           (0x00018000)
#endif

#ifndef SZ_128K
#define SZ_128K                          (0x00020000)
#endif

#ifndef SZ_256K
#define SZ_256K                          (0x00040000)
#endif

#ifndef SZ_512K
#define SZ_512K                          (0x00080000)
#endif

#ifndef SZ_1M
#define SZ_1M                            (0x00100000)
#endif

#ifndef SZ_2M
#define SZ_2M                            (0x00200000)
#endif

#ifndef SZ_3M
#define SZ_3M                            (0x00300000)
#endif

#ifndef SZ_4M
#define SZ_4M                            (0x00400000)
#endif

#ifndef SZ_8M
#define SZ_8M                            (0x00800000)
#endif

#ifndef SZ_16M
#define SZ_16M                           (0x01000000)
#endif

#ifndef SZ_32M
#define SZ_32M                           (0x02000000)
#endif

#ifndef SZ_64M
#define SZ_64M                           (0x04000000)
#endif

#ifndef SZ_128M
#define SZ_128M                          (0x08000000)
#endif

#ifndef SZ_256M
#define SZ_256M                          (0x10000000)
#endif

#ifndef SZ_512M
#define SZ_512M                          (0x20000000)
#endif


/************************************************************************
                * IP BASE ADDR *
************************************************************************/

/* SOCP */
#define HI_SOCP_REGBASE_ADDR              (0xBF030000)
#define HI_SOCP_REG_SIZE                  (SZ_4K)

/* I2C1 */
#define HI_I2C1_REGBASE_ADDR              (0xBDF0AFFF)
#define HI_I2C1_REG_SIZE                  (SZ_4K)

/* I2C0 */
#define HI_I2C0_REGBASE_ADDR              (0xBDF09FFF)
#define HI_I2C0_REG_SIZE                  (SZ_4K)

/*SPI Master 0*/
#define HI_SPI_MST0_REGBASE_ADDR          (0xBDF07FFF)
#define HI_SPI_MST0_REG_SIZE              (SZ_4K)

/*SPI Master 1*/
#define HI_SPI_MST1_REGBASE_ADDR          (0xBDF08FFF)
#define HI_SPI_MST1_REG_SIZE              (SZ_4K)

/* UART3 */
#define HI_UART3_REGBASE_ADDR             (0xBDF04FFF)
#define HI_UART3_REG_SIZE                 (SZ_4K)

/* UART2 */
#define HI_UART2_REGBASE_ADDR             (0xBDF03FFF)
#define HI_UART2_REG_SIZE                 (SZ_4K)

/* UART0 */
#define HI_APB_CFG_UART0_BASE_ADDR        (0xBDF02FFF)
#define HI_APB_CFG_UART0_SIZE             (SZ_4K)

/* UART1 */
#define HI_UART1_REGBASE_ADDR             (0xBDF00FFF)
#define HI_UART1_REG_SIZE                 (SZ_4K)

/*********************** APB1_CFG  definition ***********************/
/* GPIO3 */
#define HI_GPIO3_REGBASE_ADDR             (0xA8A0D000)
#define HI_GPIO3_REG_SIZE                 (SZ_4K)

/* GPIO2 */
#define HI_GPIO2_REGBASE_ADDR             (0xA8A0D000)
#define HI_GPIO2_REG_SIZE                 (SZ_4K)

/* GPIO1 */
#define HI_GPIO1_REGBASE_ADDR             (0xA8A0C000)
#define HI_GPIO1_REG_SIZE                 (SZ_4K)

/* GPIO0 */
#define HI_GPIO0_REGBASE_ADDR             (0xA8A0B000)
#define HI_GPIO0_REG_SIZE                 (SZ_4K)

/* PCTRL */
#define HI_PCTRL_REGBASE_ADDR             (0xA8A09000)
#define HI_PCTRL_REG_SIZE                 (SZ_4K)

/* todo: PWM1 */
#define HI_PWM1_REGBASE_ADDR              (0xA8A05000)
#define HI_PWM1_REG_SIZE                  (SZ_4K)

/* todo: PWM0 */
#define HI_PWM0_REGBASE_ADDR              (0xA8A04000)
#define HI_PWM0_REG_SIZE                  (SZ_4K)

/* HKADC SSI */
#define HI_HKADCSSI_REGBASE_ADDR          (0xA82B8000)
#define HI_HKADCSSI_REG_SIZE              (SZ_4K)

/* APP GIC */
#define HI_APP_GIC_BASE_ADDR              (0xA82B0000)
#define HI_APP_GIC_SIZE                   (SZ_32K)

/*********************** MODEM_SUBSYS  definition ***********************/
#define HI_MODEM_BASE_ADDR                (0XA0000000)
#define HI_MODEM_SIZE                     (SZ_128M)

/* A9 L2 reg */
#define HI_MDMA9_L2_REGBASE_ADDR          (0XA0000000)
#define HI_MDMA9_L2_REG_SIZE              (SZ_1M)

/* MDM GIC */
#define HI_MDM_GIC_BASE_ADDR              (0XA0100000)
#define HI_MDM_GIC_SIZE                   (SZ_8K)

/* system controller */
#define HI_SYSCTRL_BASE_ADDR              (0XA0200000)
#define HI_SYSCTRL_REG_SIZE               (SZ_4K)

#define HI_SYSCRG_BASE_ADDR               HI_SYSCTRL_BASE_ADDR
#define HI_SYSSC_BASE_ADDR                HI_SYSCTRL_BASE_ADDR
#define HI_PWRCTRL_BASE_ADDR              HI_SYSCTRL_BASE_ADDR

/* watchdog(WDT) */
#define HI_WDT_BASE_ADDR                  (0XA0201000)
#define HI_WDT_REG_SIZE                   (SZ_4K)

/* timer: AO timer 0-3 ,APB 4-7 */
#define HI_TIMER_00_REGBASE_ADDR          (0x40200000)

#define HI_TIMER_01_REGBASE_ADDR          (0x40201000)
#define HI_TIMER_02_REGBASE_ADDR          (0x40202000)
#define HI_TIMER_03_REGBASE_ADDR          (0x40203000)
#define HI_TIMER_04_REGBASE_ADDR          (0xA8A00000)
#define HI_TIMER_05_REGBASE_ADDR          (0xA8A01000)
#define HI_TIMER_06_REGBASE_ADDR          (0xA8A02000)
#define HI_TIMER_07_REGBASE_ADDR          (0xA8A03000)
#define HI_TIMER_REG_SIZE                 (SZ_4K)

/* UART0 */
#define HI_UART0_REGBASE_ADDR             (0XA0203000)
#define HI_UART0_REG_SIZE                 (SZ_4K)

/* EDMA CH16 */
#define HI_EDMA_REGBASE_ADDR              (0XA0204000)
#define HI_EDMA_REG_SIZE                  (SZ_4K)

/* DramTest/ASHB */
#define HI_ASHB_REGBASE_ADDR              (0XA0205000)
#define HI_ASHB_REG_SIZE                  (SZ_4K)

/* UICC */
#define HI_UICC_REGBASE_ADDR              (0XA0400000)
#define HI_UICC_REG_SIZE                  (SZ_256K)

/* CICOM0 */
#define HI_CICOM0_REGBASE_ADDR            (0XA0440000)
#define HI_CICOM0_REG_SIZE                (SZ_64K)

/* CICOM1 */
#define HI_CICOM1_REGBASE_ADDR            (0XA0450000)
#define HI_CICOM1_REG_SIZE                (SZ_64K)

/* CIPHER */
#define HI_CIPHER_BASE_ADDR               (0XA0470000)
#define HI_CIPHER_REG_SIZE                (SZ_4K)

/* UPACC */
#define HI_UPACC_BASE_ADDR                (0XA0471000)
#define HI_UPACC_REG_SIZE                 (SZ_4K)

/* IPF */
#define HI_IPF_REGBASE_ADDR               (0XA0472000)
#define HI_IPF_REG_SIZE                   (SZ_4K)

/* HS UART */
#define HI_HSUART_REGBASE_ADDR            (0XA0473000)
#define HI_HSUART_REG_SIZE                (SZ_4K)

/* BBE16 VIC */
#define HI_BBE16VIC_REGBASE_ADDR          (0XA0474000)
#define HI_BBE16VIC_REG_SIZE              (SZ_4K)

/* IPCM */
#define HI_IPCM_REGBASE_ADDR              (0XA0475000)
#define HI_IPCM_REG_SIZE                  (SZ_4K)

/* todo: AMON_CPUSLOW */
#define HI_AMON_SOC_REGBASE_ADDR          (0XA0476000)
#define HI_AMON_SOC_REG_SIZE              (SZ_16K)

/* todo: AMON_CPUFAST */
#define HI_AMON_CPUFAST_REGBASE_ADDR      (0XA0476000)
#define HI_AMON_CPUFAST_REG_SIZE          (SZ_16K)

/* SRAM(AXI memroy) */
#define HI_SRAM_MEM_BASE_ADDR             (0XA0800000)
#define HI_SRAM_MEM_SIZE                  (SZ_96K)

/* dsp subsystem */
#define HI_DSP_SUBSYSTEM_BASE_ADDR        (0XA1000000)
#define HI_DSP_SUBSYSTEM_SIZE             (SZ_32M)

/* Sec RAM */
#define HI_SECRAM_BASE_ADDR               (0x60000000)
#define HI_SECRAM_SIZE                    (SZ_256K)

/* PMUSSI0 */
#define HI_PMUSSI0_REGBASE_ADDR           (0x40234000)
#define HI_PMUSSI0_REG_SIZE               (SZ_4K)

/* PERI_CRG  */
#define HI_LP_PERI_CRG_REG_ADDR           (0x40235000)
#define HI_LP_PERI_CRG_REG_SIZE           (SZ_4K)

/* PMC */
#define HI_PMC_REGBASE_ADDR           (0x40231000)
#define HI_PMC_REG_SIZE               (SZ_4K)

/* NOC BUS */
#define HI_NOC_BUS_REGBASE_ADDR           (0xA8080000)
#define HI_NOC_BUS_REG_SIZE               (SZ_64K)

/* Tsensor */
#define HI_TSENSOR_REGBASE_ADDR           (0x40230000)
#define HI_TSENSOR_REG_SIZE               (SZ_4K)

/* MDDRC */
#define HI_MDDRC_REGBASE_ADDR             (0x40220000)
#define HI_MDDRC_REG_SIZE                 (SZ_64K)

/* efusec */
#define HI_EFUSE_REGBASE_ADDR             (0x40210000)
#define HI_EFUSE_REG_SIZE                 (SZ_4K)

/*SCI1*/
#define HI_SCI1_REGBASE_ADDR              (0x40207000)
#define HI_SCI1_REG_SIZE                  (SZ_4K)

/*SCI0*/
#define HI_SCI0_REGBASE_ADDR              (0x40206000)
#define HI_SCI0_REG_SIZE                  (SZ_4K)

/* RTC1 */
#define HI_RTC1_BASE_ADDR                 (0x40205000)
#define HI_RTC1_SIZE                      (SZ_4K)

/* RTC0 */
#define HI_RTC0_BASE_ADDR                 (0x40204000)
#define HI_RTC0_SIZE                      (SZ_4K)

/* todo: CS_SYS */
#define HI_CS_SYS_REGBASE_ADDR            (0x40100000)
#define HI_CS_SYS_REG_SIZE                (SZ_1M)

/* todo: NANDC reg */
#define HI_NANDC_REGBASE_ADDR             (0x40020000)
#define HI_NANDC_REG_SIZE                 (SZ_128K)

/* todo: NANDC buffer */
#define HI_NAND_MEM_BUFFER_ADDR           (0x40000000)
#define HI_NAND_MEM_BUFFER_SIZE           (SZ_128K)


/* 经过滑动窗口后的DDR的基址，实际对应由滑动窗口的配置决定 */
#define HI_DDR_BASE_ADDR                  (0x10000000)
#define HI_DDR_SIZE                       (SZ_256M)

/* shared  DDR */
#define HI_SHARED_DDR_BASE_ADDR           (DDR_SHARED_MEM_ADDR - DDR_MEM_ADDR + HI_DDR_BASE_ADDR)
#define HI_SHARED_DDR_SIZE                (DDR_SHARED_MEM_SIZE)

/* BOOTROM */
#define HI_BOOTROM_REGBASE_ADDR           (0x00060000)
#define HI_BOOTROM_REG_SIZE               (SZ_64K)

/* LP M3 TCM */
/* M3 TCM1 only valid in LPMCU. PD region*/
#define HI_M3TCM1_MEM_ADDR                (0x00030000)
#define HI_M3TCM1_MEM_SIZE                (SZ_32K)

/* M3 TCM0 only valid in LPMCU. AO region*/
#define HI_M3TCM0_MEM_ADDR                (00020000)
#define HI_M3TCM0_MEM_SIZE                (SZ_64K)

/*******************************************************************************/

#define HI_BBPMASTER_REG_BASE_ADDR            HI_WBBP_REG_BASE_ADDR
#define HI_ZSP_AHB_REG_BASE_ADDR              0xFFFFFFFF            /* not found */
#define HI_ZSP_DTCM_REG_BASE_ADDR             0xFFFFFFFF            /* not found */

/*******************************************************************************/
#define HI_BBP_SRC_BASE_ADDR                  (0xA1000000)
#define HI_BBP_SRC_SIZE                        SZ_1M

#define HI_BBP_DMA_BASE_ADDR                  (0xA1200000)
#define HI_BBP_DMA_SIZE                        SZ_1M

#define HI_BBP_DBG_BASE_ADDR                  (0xA1300000)
#define HI_BBP_DBG_SIZE                        SZ_1M

#define HI_BBP_INT_BASE_ADDR                  (0xA1700000)
#define HI_BBP_INT_SIZE                        SZ_4K

#define HI_BBP_STU_BASE_ADDR                  (0xA170e000)
#define HI_BBP_STU_SIZE                        SZ_4K

#define HI_GBBP_REG_BASE_ADDR                 (0xA1800000)
#define HI_GBBP_REG_SIZE                       SZ_512K

#define HI_GBBP1_REG_BASE_ADDR	              (0xA1880000)
#define HI_GBBP1_REG_SIZE                      SZ_512K

#define HI_WBBP_REG_BASE_ADDR                 (0xA1900000)
#define HI_WBBP_REG_REG_SIZE                   SZ_1M

#define HI_BBP_TDSINT_BASE_ADDR               (0xA1d00000)
#define HI_BBP_TDSINT_SIZE                     SZ_8K

#define HI_CTU_BASE_ADDR                      (0xA1f80000)
#define HI_CTU_SIZE                            SZ_32K

#define HI_ABB_REG_BASE_ADDR                  (0xA1F8C000)
#define HI_ABB_REG_SIZE                       (0x2000)


#define HI_AP_SYSCTRL_BASE_ADDR               (0X4020A000)
#define HI_AP_SYSCTRL_REG_SIZE                (SZ_4K)
#define HI_AP_SYS_CNT_BASE_ADDR              (0x40208000)
#define HI_AP_SYS_CNT_SIZE                   (SZ_8K)
#ifdef BSP_CONFIG_BOARD_SFT
#define HI_BBP_SYSTIME_BASE_ADDR              (HI_AP_SYS_CNT_BASE_ADDR)
#define HI_BBP_SYSTIME_SIZE                    HI_AP_SYS_CNT_SIZE
#else
/*对应memmory map 中的 SYS CTRL项*/
#define HI_BBP_SYSTIME_BASE_ADDR              (0X4020A000)
#define HI_BBP_SYSTIME_SIZE                    SZ_4K
#endif

/* DRX */
/*bbp lte drx*/
#define HI_BBP_LTEDRX_BASE_ADDR               (0x40212000)

#define HI_GBBP_DRX_REG_BASE_ADDR             (0x40212800)

#define HI_GBBP1_DRX_REG_BASE_ADDR	          (0x40212C00)

/*bbp tds drx*/
#define HI_BBP_TDSDRX_BASE_ADDR               (0x40213400)

/*COMM ON的基地址为(0x9002D800),根据luxiaohua要求，返回0x9002a000*/
#define HI_BBP_COMM_ON_BASE_ADDR              HI_APB_MIPI_REGBASE_ADDR

/*W DRX的基地址为(0x9002DC00),根据luxiaohua要求，返回0x9002a000*/
#define HI_WBBP_DRX_REG_BASE_ADDR	          HI_APB_MIPI_REGBASE_ADDR

#ifdef __cplusplus
}
#endif

#endif    /* __SOC_MEMMAP_M3_H__ */

