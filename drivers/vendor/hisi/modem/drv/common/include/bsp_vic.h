


#ifndef _BSP_VIC_H_
#define _BSP_VIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "osl_module.h"
#include "osl_types.h"

#define INT_GU_VIC_BASE_LVL     32

typedef void (*vicfuncptr)(s32);

s32  bsp_vic_init(void);
s32 bsp_vic_enable(int level);
s32 bsp_vic_disable (int level);
s32 bsp_vic_connect(int level, vicfuncptr routine, s32 parameter);
s32 bsp_vic_disconnect(int level);

#ifdef __cplusplus
}
#endif

#endif /*  _BSP_VIC_H_*/

