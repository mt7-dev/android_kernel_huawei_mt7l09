#ifndef __FASTBOOT_CONFIG_H__
#define __FASTBOOT_CONFIG_H__

#include <bsp_sram.h>
#include <product_config.h>
#include <ptable_com.h>

#define SECTION(x)						__attribute__((__section__(x)))

/* kernel */
#define CFG_ARCH_TYPE					3339
#define CFG_DEF_CMDLINE					"mem=50M console=ttyAMA0,115200"
#define	PAGE_SIZE						4096

/* ddr */
#define CFG_DDR_PHYS					(DDR_MEM_ADDR)
#define CFG_DDR_SIZE					(DDR_MEM_SIZE)

#define HI_MDDRC_REGBASE_BASE			0x90020000

/* sysctrl */
#define HI_SC_REGBASE_ADDR				0x90000000
#define HI_CRG_CTRL14_REG				(0x38)  /* 软复位请求寄存器1 */
#define HI_SYSCTRL_SC_CTRL1_REG			(0x404)
#define SC_CRG_CTRL18                   0x90000048
#define SC_CRG_CTRL20                   0x90000050
#define SC_CRG_CTRL22                   0x90000058
#define SC_CRG_CTRL25                   0x90000064
#define SC_CRG_CTRL26                   0x90000068
#define SC_CRG_CTRL27                   0x9000006C
#define SC_CRG_CTRL28                   0x90000070
#define SC_CRG_CTRL34                   0x90000088
#define SC_CRG_CTRL42                   0x90000224
#define PWR_CTRL0                       0x90000C00
#define SC_CTRL6						(0x418)


/* timer */
#ifdef CONFIG_USE_TIMER_STAMP
#define FASTBOOT_TIMER_BASE             0x90002064
#define FASTBOOT_TIMER_CURRENTVALUE		0x4
#else
#define FASTBOOT_TIMER_BASE 			0x90000000
#define FASTBOOT_TIMER_CURRENTVALUE 	0x640
#endif 

#define FASTBOOT_TIMER_LOADCOUNT		0x0
#define FASTBOOT_TIMER_CONTROLREG		0x8


#define FASTBOOT_TIME_HZ                  (32000)           /* 用FASTBOOT TIMER 32K时钟 */
#define UDELAY_TIMER_BASE             0x90003050/*timer12*/

#define WAIT_TIMER_CHANGE  				4000   		/*set the cycle counter for TIMER0VALUE to change*/
#define MHZ 							(1000*1000)
#ifdef BSP_CONFIG_V7R2_SFT
#define CFG_CLK_CPU_BASE 				50
#else
#define CFG_CLK_CPU_BASE 				333			/* fix me ,FPGA平台上CPU频率为90MHZ。*/
#endif

#define CFG_CLK_CPU 					(CFG_CLK_CPU_BASE*MHZ)

#define A9_CORE_BASE           			0xFC000000
#define A9_GLB_TIMER_BASE 				(A9_CORE_BASE+0x200)

#define A9_GLB_TIMER_CNTL    			0x00
#define A9_GLB_TIMER_CNTH   			0x04
#define A9_GLB_TIMER_CTL      			0x08

/* ptable */
#define AXI_MEM_ADDR					(HI_SRAM_MEM_ADDR)	  /* (0x4fe00000) */
#define PTABLE_NAND_OFFSET              (FLASH_PTABLE_OFFSET) /*  (0x20000)  */ 	/*分区表在flash存储空间的偏移值*/

/* nand */
#define NANDC_BUF_BASE_ADDR           	(HI_NAND_MEM_BUFFER_ADDR)
#define ADDR_TAGS    					(PRODUCT_KERNEL_PARAMS_PHYS)

/* usb */
#define CFG_USB_DOWNLOAD_ADDR			CFG_DDR_PHYS
#define CFG_USB_DOWNLOAD_SIZE			(64*1024*1024)

/* uart load */
#define CFG_UART_DOWNLOAD_ADDR			CFG_DDR_PHYS
#define CFG_UART_DOWNLOAD_SIZE			(100*1024*1024)

/* init */
#define PBXA9_PERIPHBASE                (0x4000000)
#define PBXA9_SCU_BASE                  (PBXA9_PERIPHBASE + 0x0)
#define PBXA9_SCU_CTRL                  (PBXA9_SCU_BASE + 0x00)

#define PBXA9_GIC_BASE                  (PBXA9_PERIPHBASE)
#define PBXA9_GIC_CPU_CONTROL           (PBXA9_GIC_BASE + 0x0100)

#define BOOT_COLD			    		0x02

#define BSS_START                       BOOTLOADER_BSS
#define BSS_END                         BOOTLOADER_BSS_END
#define CFG_FASTBOOT_NAND_BASE          (NANDC_BUF_BASE_ADDR + 0x40000)
#define CFG_BOOTLOADER_SIZE             BOOTLOADER_SIZE


/* MMU Control Register bit allocations */
#define MMUCR_M_ENABLE	 (1<<0)  /* MMU enable */
#define MMUCR_A_ENABLE	 (1<<1)  /* Address alignment fault enable */
#define MMUCR_C_ENABLE	 (1<<2)  /* (data) cache enable */
#define MMUCR_W_ENABLE	 (1<<3)  /* write buffer enable */
#define MMUCR_PROG32	 (1<<4)  /* PROG32 */
#define MMUCR_DATA32	 (1<<5)  /* DATA32 */
#define MMUCR_L_ENABLE	 (1<<6)  /* Late abort on earlier CPUs */
#define MMUCR_BIGEND	 (1<<7)  /* Big-endian (=1), little-endian (=0) */
#define MMUCR_SYSTEM	 (1<<8)  /* System bit, modifies MMU protections */
#define MMUCR_ROM	 (1<<9)  /* ROM bit, modifies MMU protections */
#define MMUCR_F		 (1<<10) /* Should Be Zero */
#define MMUCR_Z_ENABLE	 (1<<11) /* Branch prediction enable on 810 */
#define MMUCR_I_ENABLE	 (1<<12) /* Instruction cache enable */
#define MMUCR_V_ENABLE	 (1<<13) /* Exception vectors remap to 0xFFFF0000 */
#define MMUCR_ALTVECT    MMUCR_V_ENABLE /* alternate vector select */
#define MMUCR_RR_ENABLE	 (1<<14) /* Round robin cache replacement enable */
#define MMUCR_ROUND_ROBIN MMUCR_RR_ENABLE  /* round-robin placement */
#define MMUCR_DISABLE_TBIT   (1<<15) /* disable TBIT */
#define MMUCR_ENABLE_DTCM    (1<<16) /* Enable Data TCM */
#define MMUCR_ENABLE_ITCM    (1<<18) /* Enable Instruction TCM */
#define MMUCR_UNALIGNED_ENABLE (1<<22) /* Enable unaligned access */
#define MMUCR_EXTENDED_PAGE (1<<23)  /* Use extended PTE format */
#define MMUCR_VECTORED_INTERRUPT (1<<24) /* Enable VIC Interface */

#define MMUCR_MODE MMUCR_ROM


#define MMU_MUST_SET_VALUE  (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			     MMUCR_W_ENABLE | MMUCR_ENABLE_DTCM | \
			     MMUCR_ENABLE_ITCM | MMUCR_UNALIGNED_ENABLE | \
			     MMUCR_EXTENDED_PAGE)
#define MMU_INIT_VALUE	(MMU_MUST_SET_VALUE)

#define ARM_IMM #
#define ARM_LOC_MASK #0x7000000     /* level of coherency mask of CLIDR */

#define CMD_LINE_SIZE  192

#endif
