/*************************************************************************
*   版权所有(C) 1987-2009, 深圳华为技术有限公司.
*
*   文 件 名 :  sci_balong.h
*
*   作    者 :  Y00171698
*
*   描    述 :  本文件命名为"sci_balong.h", 实现A core SIM 卡热拔插功能
*
*   修改记录 :
*************************************************************************/
#ifndef _SCI_BALONG_H_
#define _SCI_BALONG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc_interrupts.h"
#include "product_config.h"
#include "bsp_memmap.h"
#include "osl_module.h"
#include "bsp_pmu.h"


/* add if you need */
#define BALONG_SIM_NUM  2

/* use raise int only */
#define INT_LVL_SIM0_PMU_IN      (PMU_INT_SIM0_IN_FALL)
#define INT_LVL_SIM0_PMU_OUT     (PMU_INT_SIM0_OUT_FALL)
#define INT_LVL_SIM1_PMU_IN      (PMU_INT_SIM1_IN_FALL)
#define INT_LVL_SIM1_PMU_OUT     (PMU_INT_SIM1_OUT_FALL)

/* sim detect pin */
#define SIM0_GPIO_DETECT         (GPIO_0_19)

#define DRIVER_NAME_SIM			"sim_io"
#define DETECT_NAME_SIM 		"sim_detect"

#define PMU_HPD_DEBOUNCE_TIME    (400)


#ifndef OK
#define OK      (0)
#endif

#ifndef ERROR
#define ERROR   (-1)
#endif

#ifdef __KERNEL__
#define SCI_PRINT   printk
#endif


typedef enum
{
    SIM0_CARD_OUT_EVENT = 0,
    SIM0_CARD_IN_EVENT  = 1,
    SIM1_CARD_OUT_EVENT = 0,
    SIM1_CARD_IN_EVENT  = 1
} sci_in_out_enum;


typedef enum
{
    SIM_CARD_STAUTS_OUT = 0,
    SIM_CARD_STAUTS_IN  = 1
} sci_status_enum;


typedef enum
{
    SIM_CARD_DETECT_LOW = 0,
    SIM_CARD_DETECT_HIGH  = 1
} sci_detect_level_enum;


typedef enum
{
    SIM_CARD_PMU_HPD_TRUE = 0,
    SIM_CARD_PMU_HPD_FALSE  = 1
} sci_pmu_hpd_enum;


typedef struct {
	bool sci_init_flag;
	u32  sci0_card_satus;
	u32  sci0_detect_level;
	bool  sci0_pmu_hpd;
	u32  sci1_card_satus;
	u32  sci1_detect_level;
	bool  sci1_pmu_hpd;
} bsp_sci_st;

typedef struct {
	u32 sci0_init_cnt;
	u32 sci0_init_err_cnt;
	u32 sci0_init_request_cnt;
	u32 sci0_detect_cnt;
	u32 sci0_detect_low_cnt;
	u32 sci0_detect_high_cnt;
	u32 sci0_detect_err_cnt;
    u32 sci0_pmu_hpd_in_enter_cnt;
	u32 sci0_pmu_hpd_in_cnt;
    u32 sci0_pmu_hpd_out_enter_cnt;
	u32 sci0_pmu_hpd_out_cnt;
	u32 sci0_pmu_hpd_in_err_cnt;
	u32 sci0_pmu_hpd_out_err_cnt;
	u32 sci0_detect_get_err_cnt;
}sci_debug_info_st;


#ifdef __cplusplus
}
#endif

#endif /* _VIC_BALONG_H_ */

