/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_onoff.hdrv_reset.h
*
*   作    者 :
*
*   描    述 :  以下接口如果在C核调用，需要经过核间调用，C核启动的时候不能调用。
*
*   修改记录 :
*************************************************************************/

#ifndef __DRV_ONOFF_H__
#define __DRV_ONOFF_H__

#include "product_config.h"
#include "drv_comm.h"

#ifdef __KERNEL__
#include <osl_list.h>

struct bsp_onoff_callback {
    struct list_head node;
    void (*fn)(void);
};

void bsp_reboot_callback_register(void (*hook)(void));
#endif

/* TAF需要依赖这个接口，正常开机返回1 */
int  bsp_start_mode_get( void );
#define DRV_START_MODEGET(void) bsp_start_mode_get(void)

typedef enum
{
    DRV_START_MODE_CHARGING = 0, /* 充电开机模式, ps 等业务不启动  */
    DRV_START_MODE_NORMAL ,     /* 正常开机模式，所有业务启动     */
    DRV_START_MODE_UPDATE,      /* 升级模式 */
    DRV_START_MODE_BUILT
}DRV_START_MODE;

void bsp_drv_power_off(void);
#define DRV_POWER_OFF(void) bsp_drv_power_off(void)

void bsp_drv_power_reboot( void );
void bsp_drv_power_reboot_direct( void );

typedef enum DRV_SHUTDOWN_REASON_tag_s
{
    DRV_SHUTDOWN_POWER_KEY,             /* 长按 Power 键关机          */
    DRV_SHUTDOWN_BATTERY_ERROR,         /* 电池异常                   */
    DRV_SHUTDOWN_LOW_BATTERY,           /* 电池电量低                 */
    DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* 过温保护关机               */
    DRV_SHUTDOWN_CHARGE_REMOVE,         /* 关机充电模式下，拔除充电器 */
    DRV_SHUTDOWN_UPDATE,                /* 关机并进入升级模式         */
    DRV_SHUTDOWN_RESET,                 /* 系统软复位                 */
    DRV_SHUTDOWN_BUTT
}DRV_SHUTDOWN_REASON_ENUM;

typedef enum {
    DRV_START_REASON_CHARGE = 0,
    DRV_START_REASON_POWER_KEY ,
    DRV_START_REASON_WARM_RESET ,
    DRV_START_REASON_BUTT
}DRV_START_REASON;

/*****************************************************************************
 函 数 名  : DRV_SHUT_DOWN
 功能描述  : 单板关机
 输入参数  : 无
 输出参数  : eReason：        关机原因
 返 回 值  : 无
*****************************************************************************/
void drv_shut_down( DRV_SHUTDOWN_REASON_ENUM eReason );
#define DRV_SHUT_DOWN(eReason) drv_shut_down(eReason)
#define drvShutdown(eReason)   drv_shut_down(eReason)

typedef enum
{
    NORMAL_VERTYPE_FLAG = 0,       /* 当前版本为正常版本 */
    FACTORY_VERTYPE_FLAG,          /* 当前版本为工厂版本 */
    INVALID_VERTYPE_FLAG
}DRV_SOFT_VER_TYPE;

typedef enum
{
    NORMAL_CHARGING_MODE = 0,          /* 开机充电模式 */
    POWER_DOWN_CHARGING_MODE,         /* 关机充电模式 */
    INVALID_CHARGING_MODE
}DRV_CHARGING_MODE;

typedef enum
{
    NORMAL_RESET_FLAG = 0,         /* 正常复位版本 */
    ABNORMAL_RESET_FLAG,           /* 异常复位版本 */
    INVALID_RESET_FLAG
}DRV_RESET_TYPE;

typedef enum
{
    RUN_MODE_NORMAL = 0,         /* 正常版本 */
    RUN_MODE_RECOVERY,           /* RECOVERY */
    RUN_MODE_INVALID
}DRV_RUN_MODE;

#endif

