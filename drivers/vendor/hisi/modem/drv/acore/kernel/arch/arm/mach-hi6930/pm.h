

#ifndef __PM_H__
#define __PM_H__

#include <bsp_sram.h>
#include <bsp_memmap.h>
#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#include <bsp_dump_drv.h>

#define PM_ERROR    (-1)
#define PM_OK       (0)
#define PM_TEST_WSRC
#define pm_printk(level, fmt, ...)      (bsp_trace(level, BSP_MODU_PM, "[pm]:<%s> <%d> "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__))

#define _asm_function_called_from_c(a) \
    .globl a ;\
    .code  32 ;\
    .balign 4 ;\
a:

#ifndef __ASSEMBLY__
extern int pm_create_idmap(void);
extern void pm_asm_boot_code_begin(void);
extern void pm_asm_cpu_go_sleep(void);
extern void pm_asm_sleep_end(void);
static inline void WFI(void)
{
	asm volatile ( "dsb;" );
    asm volatile ( "isb;");
	asm volatile ( "wfi;");
	asm volatile ( "nop;");
	asm volatile ( "nop;");
}

/* 动态设置唤醒源 发送  icc */
typedef enum tagPmType
{
	PM_TYEP_ACORE_SLEEP = 0,
	PM_TYEP_ACORE_WAKE = 1,
	PM_TYEP_CCORE_SLEEP = 2,
	PM_TYEP_CCORE_WAKE = 3,
	PM_TYEP_ASET_WSRC = 4,
	PM_TYEP_CSET_WSRC = 5,
	PM_TYEP_AUNSET_WSRC = 6,
	PM_TYEP_CUNSET_WSRC = 7,
	PM_TYEP_BUTTOM = 8
}T_PM_TYPE;
typedef struct
{
	T_PM_TYPE      type;
	u32            addr;
}T_PM_MAIL;

#endif

#define PM_MEM_V2P(addr)            SHD_DDR_ASM_V2P(addr)
#define PM_STAMP_ADDR       (TIMER_STAMP_ADDR)
#define PM_STAMP_ADDR_PHY   (TIMER_STAMP_ADDR_PHY)

#define DEBUG_PM_STAMP
#ifdef DEBUG_PM_STAMP
    #define _asm_debug_pm_stamp(macro_addr)\
        LDR    r0, =(macro_addr) ;\
        LDR    r1, =(PM_STAMP_ADDR) ;\
        LDR    r2, [r1] ;\
        STR    r2, [r0]
    #define _asm_debug_pm_stamp_v2p(macro_addr)\
        LDR    r0, =PM_MEM_V2P(macro_addr) ;\
        LDR    r1, =(PM_STAMP_ADDR_PHY) ;\
        LDR    r2, [r1] ;\
        STR    r2, [r0]
#else
    #define _asm_debug_pm_stamp(macro_addr)
    #define _asm_debug_pm_stamp_v2p(macro_addr)
#endif

//#define DEBUG_PM_PRINT
#ifdef DEBUG_PM_PRINT
    #define _asm_debug_pm_print_virt(char)\
    LDR r0, =HI_UART1_REGBASE_ADDR_VIRT ;\
    LDR r1, =char ;\
    STR r1, [r0]
    #define _asm_debug_pm_print_phy(char)\
    LDR r0, =HI_UART1_REGBASE_ADDR ;\
    LDR r1, =char ;\
    STR r1, [r0]
#else
    #define _asm_debug_pm_print_virt(char)
    #define _asm_debug_pm_print_phy(char)
#endif



/* boot addr after WFI & powerdown */
#define PM_MEM_ACORE_BASE_ADDR          (SHM_MEM_APPA9_PM_BOOT_ADDR)
#define PM_ASM_CODE_COPY_OFFSET 0x200  /* data-section in asm*/

#define	PM_VIRT_PHY_FOR_IDMAP				(DDR_SHARED_MEM_VIRT_ADDR-DDR_SHARED_MEM_ADDR) /*PM MEM (virt_addr-phy_addr) */

#define DSARM0_BASE_ADDR                (PM_MEM_ACORE_BASE_ADDR + 0x1000)
#define DSRAM0_DEEPSLEEP_ADDR           (DSARM0_BASE_ADDR + 0x200)

/* in asm save regs(arm,arm-corp,mmu), when powerup restore them */
/*  save r0-r14 */
#define BAK_ARM_REG_ADDR        (DSRAM0_DEEPSLEEP_ADDR + 0x0)  // (14+1) = 0x38
#define BAK_ARM_REG_SIZE        (0x50)
/* store cor_reg */
#define BAK_ARM_CO_REG_ADDR     (BAK_ARM_REG_ADDR + BAK_ARM_REG_SIZE)  //(21+1) = 0x58
#define BAK_ARM_CO_REG_SIZE     (0xD0)
/* store mmu */
#define BAK_MMU_REG_ADDR        (BAK_ARM_CO_REG_ADDR + BAK_ARM_CO_REG_SIZE)  // (12+1) = 0x34
#define BAK_MMU_REG_SIZE        (0x50)

/* STAMP */
#define STAMP_START_ADDR            DUMP_EXT_OM_DRX_ACORE_ADDR/*(DSRAM0_DEEPSLEEP_ADDR+0x700)*/
/*start form pm_suspend()*/
#define STAMP_PM_SUSPEND_START      (4+STAMP_START_ADDR)
#define STAMP_AFTER_DPM_SUSPEND     (4+STAMP_PM_SUSPEND_START)  /* dpm_suspend may fail and goto resume without being stamped.*/
#define STAMP_AFTER_UART_SUSPEND    (4+STAMP_AFTER_DPM_SUSPEND)
/* pm enter */
#define STAMP_PM_ENTER_START        (4+STAMP_AFTER_UART_SUSPEND)
#define STAMP_AFTER_DISABLE_GIC     (4+STAMP_PM_ENTER_START)
#define STAMP_AFTER_BAK_GIC         (4+STAMP_AFTER_DISABLE_GIC)

/*in sleep ASM power down*/
#define STAMP_SLEEP_ASM_ENTER       (4+STAMP_AFTER_BAK_GIC)
#define STAMP_BAK_COREG_BEGIN       (4+STAMP_SLEEP_ASM_ENTER)
#define STAMP_BAK_COREG_END         (4+STAMP_BAK_COREG_BEGIN)
#define STAMP_BAK_MMUREG_BEGIN      (4+STAMP_BAK_COREG_END)
#define STAMP_BAK_MMUREG_END        (4+STAMP_BAK_MMUREG_BEGIN)
#define STAMP_BEFORE_SEND_IPC       (4+STAMP_BAK_MMUREG_END)
#define STAMP_AFTER_SEND_IPC        (4+STAMP_BEFORE_SEND_IPC)

#define STAMP_AFTER_WFI_NOP         (4+STAMP_AFTER_SEND_IPC)   /* after wfi, should not be stamped */

/*in sleep ASM power up*/
#define STAMP_PWRUP_CODE_BEGIN      (4+STAMP_AFTER_WFI_NOP)
#define STAMP_RSTR_MMUREG_BEGIN     (4+STAMP_PWRUP_CODE_BEGIN)
#define STAMP_RSTR_MMUREG_END       (4+STAMP_RSTR_MMUREG_BEGIN)
#define STAMP_RSTR_COREG_BEGIN      (4+STAMP_RSTR_MMUREG_END)
#define STAMP_RSTR_COREG_END        (4+STAMP_RSTR_COREG_BEGIN)
/* out from sleep ASM,in pm enter */
#define STAMP_SLEEP_ASM_OUT         (4+STAMP_RSTR_COREG_END)
#define STAMP_AFTER_RSTR_GIC        (4+STAMP_SLEEP_ASM_OUT)
#define STAMP_AFTER_ENABLE_GIC      (4+STAMP_AFTER_RSTR_GIC)
#define STAMP_PM_ENTER_END          (4+STAMP_AFTER_ENABLE_GIC)
/* out from pm enter, in pm suspend*/
#define STAMP_AFTER_UART_RESUME     (4+STAMP_PM_ENTER_END)
#define STAMP_AFTER_DPM_RESUME      (4+STAMP_AFTER_UART_RESUME) /* after dpm_resume, pm_suspend return */

/* MMU  idmap */
#define STORE_REMAP_ADDR1           (DSRAM0_DEEPSLEEP_ADDR + 0x794)
#define STORE_REMAP_ADDR2           (DSRAM0_DEEPSLEEP_ADDR + 0x798)
#define STORE_REMAP_ADDR3           (DSRAM0_DEEPSLEEP_ADDR + 0x79C)
#define STORE_REMAP_ADDR4           (DSRAM0_DEEPSLEEP_ADDR + 0x7A0)
#define STORE_CTRL_REG_BASE          (DSRAM0_DEEPSLEEP_ADDR + 0x800)

/*GIC基址*/
#define CARM_GIC_DIST_BASE              (HI_APP_GIC_BASE_ADDR_VIRT+0x1000)

#define CARM_GIC_ICDISER                (CARM_GIC_DIST_BASE + 0x100)
#define CARM_GIC_ICDICER                (CARM_GIC_DIST_BASE + 0x180)
#define CARM_GIC_ICDISPR                (CARM_GIC_DIST_BASE + 0x300)

/*GIC*/
#define CARM_GIC_ICDABR_NUM             (7)
#define CARM_GIC_ICDISPR_OFFSET(x)      (CARM_GIC_ICDISPR  + ((x)*0x4))

#define DM_ACC_UNCTRL               (0xFFFFFFFF)

/* CPSR config bit . */;
#ifndef I_BIT
#define I_BIT       (1 << 7)
#endif
#ifndef F_BIT
#define F_BIT       (1 << 6)
#endif

/* mode bits */
#ifndef MODE_MASK
#define MODE_MASK       0x1F
#endif
/* masks for getting bits from PSR */
#ifndef MASK_MODE
#define MASK_MODE       0x0000003F
#endif
/* mode bits */
#define MODE_SYSTEM32   0x1F
#define MODE_UNDEF32    0x1B
#define MODE_ABORT32    0x17
#define MODE_SVC32      0x13
#define MODE_IRQ32      0x12
#define MODE_FIQ32      0x11
#define MODE_USER32     0x10



/* cpsr control bits */
#define CTRL_MMU_BIT            (0x01)
#define CTRL_ICACHE_BIT         (1<<12)
#define CTRL_DCACHE_BIT         (1<<2)
#define CTRL_CACHE_BIT          CTRL_DCACHE_BIT | CTRL_ICACHE_BIT
#define CTRL_MMU_CACHE          CTRL_MMU_BIT | CTRL_CACHE_BIT


/*ID map---*/
/*first level section address mask*/
#define SEC_ADDR_MASK           (0xFFF00000)     /*bit 31 ~ 20 mask*/

#define PMD_SECT_ARMA9        	PMD_TYPE_SECT | PMD_SECT_BUFFERABLE \
								| PMD_SECT_CACHEABLE|PMD_SECT_AP_WRITE| PMD_SECT_AP_READ

//#define PMD_SECT_ARMA9        	(PMD_TYPE_SECT | PMD_SECT_BUFFERABLE | PMD_SECT_CACHEABLE|PMD_SECT_AP_WRITE| PMD_SECT_AP_READ)
/*TTB register transfer bit0~13 MASK*/
#define TRANS_BASE_MASK         (0x3FFF)

#endif

