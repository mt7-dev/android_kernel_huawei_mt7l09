


#ifndef _VIC_BALONG_H_
#define _VIC_BALONG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc_interrupts.h"
#include "bsp_memmap.h"
#include "osl_module.h"
#include "bsp_vic.h"

#define INT_VIC_MAX     32
#define BALONG_VIC_NUM  2

#ifdef __KERNEL__
#define INT_LVL_VIC      INT_LVL_EXT12
#define INT_GU_LVL_VIC   INT_LVL_EXT14
#define BSP_VIC_BASE     HI_APP_VIC_BASE_ADDR_VIRT
#define BSP_GU_VIC_BASE  HI_APP_GU_VIC_BASE_ADDR_VIRT
#else
#define INT_LVL_VIC      INT_LVL_EXT13
#define INT_GU_LVL_VIC   INT_LVL_EXT15
#define BSP_VIC_BASE     HI_MDM_VIC_BASE_ADDR_VIRT
#define BSP_GU_VIC_BASE  HI_MDM_GU_VIC_BASE_ADDR_VIRT
#endif

#ifndef OK
#define OK      (0)
#endif

#ifndef ERROR
#define ERROR   (-1)
#endif

typedef struct {
    vicfuncptr	routine;
    s32	arg;
    } bsp_victable_st;

typedef struct {
	u32 vic_base_addr;
	u32 vic_irq_num;
	u32 vic_inten;
	u32 vic_intdisable;
	u32 vic_final_status;
    bsp_victable_st victable[INT_VIC_MAX];
    } bsp_vic_st;


#ifdef __cplusplus
}
#endif

#endif /* _VIC_BALONG_H_ */

