#ifndef __M3_PM_H__
#define __M3_PM_H__

#include "osl_types.h"
#include "bsp_sram.h"
#include <irq.h>

#define PM_SLEEP_VOTE_MASK    \
       ((0x1 << PM_PWR_VOTE_ACORE) | (0x1 << PM_PWR_VOTE_CCORE) | (0x1 << PM_PWR_VOTE_HIFI))


typedef enum tagPwrVote
{
	PM_PWR_VOTE_ACORE = 0,
	PM_PWR_VOTE_CCORE = 1,
	PM_PWR_VOTE_HIFI  = 2
}T_PM_PWR_VOTE;

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
	PM_TYEP_HIFI_SLEEP = 8,
	PM_TYEP_HIFI_WAKE = 9,
	PM_TYEP_BUTTOM = 10
}T_PM_TYPE;

typedef struct
{
	T_PM_TYPE      type;
	u32            addr;
}T_PM_MAIL;

typedef struct
{
	u32 u32VoteMap;
	u32 u32VoteMask;
	u32 u32SleepFlag;
	u32 u32AcoreWsrc[4];
	u32 u32CcoreWsrc[4];
	u32 u32McoreWsrc[4];
    u32 u32HiFiWsrc[4];
}T_PM_ST;

typedef struct
{
	u32 u32ClkDiv;
	u32 u32NvicEnable[8];
}T_PM_BAKEUP_ST;

typedef struct
{
	u32 u32NormalWfiTimes;
	u32 u32DeepSleepTimes;
	u32 u32McuTimerIntTimes;
	u32 u32AcoreSleepTimes;
	u32 u32CcoreSleepTimes;
    u32 u32HiFiSleepTimes;
	u32 u32PwrRegbakFlag;
	u32 u32WakeupDebugFlag;
	u32 u32PrintIrqWsrcFlag;
}T_PM_COUNT_ST;

typedef struct
{
	u32 u32DpmSuspendErrTimes;
	u32 u32DpmResumeErrTimes;
	u32 u32AxiBakeupErrTimes;
	u32 u32AxiRestoreErrTimes;
}T_PM_ERROR_ST;


extern void pm_init(void);
extern void pm_appa9_wsrc_set(u32 lvl);
extern void pm_mdma9_wsrc_set(u32 lvl);
extern void pm_hifi_wsrc_set(u32 lvl);
extern void pm_mcu_wsrc_set(u32 lvl);
extern void pm_appa9_wsrc_unset(u32 lvl);
extern void pm_mdma9_wsrc_unset(u32 lvl);
extern void pm_hifi_wsrc_unset(u32 lvl);
extern void pm_mcu_wsrc_unset(u32 lvl);
extern void pm_print_debug(void);
extern u32 pm_get_deepsleep_status(void);
extern u32 pm_mcu_uart_wsrc_flag(void);
extern void pm_vote(u32 vote, u32 flag);

/**************************************************************/
/* sram debug addr */
#define PWR_SRAM_DEBUG_ADDR         SRAM_MCU_RESERVE_ADDR
#define PWR_SRAM_DEBUG_SIZE         SRAM_MCU_RESERVE_SIZE

#define PWR_SRAM_TIMER0            (PWR_SRAM_DEBUG_ADDR)
#define PWR_SRAM_TIMER1            (PWR_SRAM_DEBUG_ADDR + 0x10)
#define PWR_SRAM_TIMER2            (PWR_SRAM_DEBUG_ADDR + 0x20)
#define PWR_SRAM_TIMER3            (PWR_SRAM_DEBUG_ADDR + 0x30)
#define PWR_SRAM_TIMER4            (PWR_SRAM_DEBUG_ADDR + 0x40)
#define PWR_SRAM_TIMER5            (PWR_SRAM_DEBUG_ADDR + 0x50)
#define PWR_SRAM_TIMER6            (PWR_SRAM_DEBUG_ADDR + 0x60)
#define PWR_SRAM_TIMER7            (PWR_SRAM_DEBUG_ADDR + 0x70)
#define PWR_SRAM_TIMER8            (PWR_SRAM_DEBUG_ADDR + 0x80)
#define PWR_SRAM_TIMER9            (PWR_SRAM_DEBUG_ADDR + 0x90)
#define PWR_SRAM_TIMER10           (PWR_SRAM_DEBUG_ADDR + 0xa0)

#define PWR_SRAM_PWRCTRL_REG       (PWR_SRAM_DEBUG_ADDR + 0x100)
#define PWR_SRAM_WAKEUP_NUM_REG    (PWR_SRAM_DEBUG_ADDR + 0x200)
#define PWR_SRAM_WAKEUP_REG        (PWR_SRAM_DEBUG_ADDR + 0x210)

#define PWR_SRAM_CRG_REG           (PWR_SRAM_DEBUG_ADDR + 0x300)

#define PWR_SRAM_TIMER21           (PWR_SRAM_DEBUG_ADDR + 0x24)
#define PWR_SRAM_TIMER22           (PWR_SRAM_DEBUG_ADDR + 0x28)

#define PWR_SRAM_TIMER41           (PWR_SRAM_DEBUG_ADDR + 0x44)
#define PWR_SRAM_TIMER42           (PWR_SRAM_DEBUG_ADDR + 0x48)

#define PWR_SRAM_TIMER51           (PWR_SRAM_DEBUG_ADDR + 0x54)
#define PWR_SRAM_TIMER52           (PWR_SRAM_DEBUG_ADDR + 0x58)
#define PWR_SRAM_TIMER53           (PWR_SRAM_DEBUG_ADDR + 0x5c)

#define PWR_SRAM_TIMER61           (PWR_SRAM_DEBUG_ADDR + 0x64)
#define PWR_SRAM_TIMER62           (PWR_SRAM_DEBUG_ADDR + 0x68)
#define PWR_SRAM_TIMER63           (PWR_SRAM_DEBUG_ADDR + 0x6C)

#define PWR_SRAM_TIMER71           (PWR_SRAM_DEBUG_ADDR + 0x74)
#define PWR_SRAM_TIMER72           (PWR_SRAM_DEBUG_ADDR + 0x78)

#define PWR_SRAM_TIMER81           (PWR_SRAM_DEBUG_ADDR + 0x84)

#define DPM_SRAM_DEBUG_ADDR (PWR_SRAM_DEBUG_ADDR+ SRAM_MCU_RESERVE_SIZE)
#define DPM_SRAM_TIMER1           (DPM_SRAM_DEBUG_ADDR - 0x4)
#define DPM_SRAM_TIMER2           (DPM_SRAM_DEBUG_ADDR - 0x8)
#define DPM_SRAM_TIMER3           (DPM_SRAM_DEBUG_ADDR - 0xC)
#define DPM_SRAM_TIMER4           (DPM_SRAM_DEBUG_ADDR - 0x10)
#define DPM_SRAM_TIMER5           (DPM_SRAM_DEBUG_ADDR - 0x14)
#define DPM_SRAM_TIMER6          (DPM_SRAM_DEBUG_ADDR - 0x18)
#define DPM_SRAM_TIMER7          (DPM_SRAM_DEBUG_ADDR - 0x1C)
#define DPM_SRAM_TIMER8           (DPM_SRAM_DEBUG_ADDR - 0x20)
#define DPM_SRAM_TIMER9           (DPM_SRAM_DEBUG_ADDR - 0x24)
#define DPM_SRAM_TIMER10           (DPM_SRAM_DEBUG_ADDR - 0x28)
#define DPM_SRAM_TIMER11           (DPM_SRAM_DEBUG_ADDR - 0x2C)
#define DPM_SRAM_TIMER12          (DPM_SRAM_DEBUG_ADDR - 0x30)
#define DPM_SRAM_TIMER13          (DPM_SRAM_DEBUG_ADDR - 0x34)
#define DPM_SRAM_TIMER14           (DPM_SRAM_DEBUG_ADDR - 0x38)
#define DPM_SRAM_TIMER15           (DPM_SRAM_DEBUG_ADDR - 0x3C)

#define RFIC0_RSTN_OFFSET          (0x2c)
#endif
