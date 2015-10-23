#ifndef __PREBOOT_H__
#define __PREBOOT_H__

#include <config.h>
#include <balongv7r2/gpio.h>
#include <balongv7r2/types.h>

/* the struct for misc data, should be the same as bootloader_message in recovery */
typedef struct  {
    char command[32];               /*启动命令*/
    char status[32];
} misc_message_type;

typedef enum {
	MISC_OK = 0,
	MISC_ERROR,
}misc_return_type;

typedef enum {
	NO_MISC_INFO = 0,
	EXIST_MISC_INFO,
	REDA_MISC_ERROR,
} misc_info;

typedef enum {
	BOOT_ON_RECOVERY_MODE = 0,
	BOOT_ON_NORMAL_MODE,
	BOOT_ON_FASTBOOT_MODE
}boot_on_mode;

typedef enum {
	WARM_RECOVERY = 0,
	WARM_NORMAL
}warm_up_e;

typedef enum {
    NORMAL_SW_TYPE = 0,
    FACTORY_SW_TYPE
}sw_type;

typedef enum {
    NORMAL_RST_TYPE = 0,
    ABNORMAL_RST_TYPE
}reset_type;

typedef struct{
	u8 name[16];
	u32 num;
}boot_cmdword;

#define  BOOT_GPIO_FASTBOOT_ID              (GPIO_2_29)
#define  BOOT_GPIO_RESET_ID                 (GPIO_0_18)

#define  BOOT_POWER_KEY_TIMEOUT 	          (2 * FASTBOOT_TIME_HZ)             /* 默认值 2s */
#define  BOOT_FORCE_UPDATE_TIMEOUT            (1000 * FASTBOOT_TIME_HZ / 1000)   /* 默认值 1s */
#define  STARTUP_TRY_TIMES                    (3)

#define pr_dbg  cprintf

int fastboot_preboot(void);
int get_pw_charge_flag(void);
int get_soft_version_flag(void);
int get_pw_mode_flag(void);
int get_abnormal_reset_flag(void);

#endif
