#ifndef __POWER_COM_H__
#define __POWER_COM_H__

#include "drv_onoff.h"

#define LOCAL_1 static

typedef enum {
    POWER_ON_START_REASON_CHARGE        = 0x504f4348,
    POWER_ON_START_REASON_POWER_KEY     = 0x504F504B,
    POWER_ON_START_REASON_WARM_RESET    = 0x504F5752,
    POWER_ON_START_REASON_BUTT          = 0xFFFFFFFF
}power_on_start_reason;

typedef enum {
    POWER_REBOOT_CMD_WARMRESET        = 0x52454348,
    POWER_REBOOT_CMD_BOOTLOADER       = 0x5245424F,
    POWER_REBOOT_CMD_RECOVERY         = 0x52455245,
    POWER_REBOOT_CMD_FACTORY          = 0x52455D51,
    POWER_REBOOT_CMD_UPDATE           = 0x52455344,
    POWER_REBOOT_CMD_BUTT              = 0xFFFFFFFF
}power_reboot_cmd;

typedef enum {
    POWER_OFF_REASON_NORMAL         = 0x504F535E,
    POWER_OFF_REASON_BAD_BATTERY   = 0x504F4242,
    POWER_OFF_REASON_LOW_BATTERY   = 0x504F424C,
    POWER_OFF_REASON_OVER_TEMP     = 0x504F4F54,
    POWER_OFF_REASON_RM_CHARGE     = 0x504F4235,
    POWER_OFF_REASON_UPDATE         = 0x504F4F55,
    POWER_OFF_REASON_RESET          = 0x504F4D56,
    POWER_OFF_REASON_INVALID        = 0xFFFFFFFF
}power_off_reboot_flag;                        
 
/* power on off info saved in axi memory */
typedef struct
{
    unsigned int last_shut_reason;
    unsigned int power_on_reason;
    unsigned int wdg_rst_cnt;                                       
    unsigned int reboot_cmd;
    unsigned int dload_complt_magic;
    unsigned int c_power_state;              /* 标识C核启动状态 */
    unsigned int a_power_state;
}power_info_s;

typedef enum {
    POWER_NO_START         =0,
    POWER_OS_OK,          
    POWER_BSP_OK,         
    POWER_MSP_OK,        
    POWER_DSP_OK,           /* 升级阶段保证C核 MSP启动OK(AXI值大于等于POWER_MSP_OK)，才认为升级成功 */
    POWER_PS_OK,
    POWER_ST_BUILT
}c_power_st_e;

typedef enum {
    POWER_NORMAL          = 0x50127895,
    POWER_RECOVERY_A      = 0x504F4F33,
    POWER_RECOVERY_B        = 0x504F4E54
}a_kernel_st_e;                 /* fastboot启动模式状态机，只在fastboot中根据启动状态设置 */

/* 操作类型 */
typedef enum _tagePowType
{
    E_POWER_ON_MODE_GET= 0,
    E_POWER_SHUT_DOWN,      /* 关机，带关机原因 */
    E_POWER_POWER_OFF,      /* 下电 */
    E_POWER_POWER_REBOOT,   /* 重启 */
    E_POWER_BUILT
}ePowCrtlType;

typedef struct _tagCtrlMsg
{
    ePowCrtlType                pwr_type;
    DRV_START_MODE              mode;
    DRV_SHUTDOWN_REASON_ENUM    reason;
}stCtrlMsg;

void balong_power_restart(char mode, const char *cmd);
void balong_power_off(void);

#if defined(__KERNEL__)
DRV_CHARGING_MODE get_pd_charge_flag(void);
DRV_RUN_MODE get_run_mode(void);
DRV_SOFT_VER_TYPE get_version_type(void);
#endif

#endif
