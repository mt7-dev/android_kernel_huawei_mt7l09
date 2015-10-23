#ifndef __BSP_AMON_H__
#define __BSP_AMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "osl_common.h"

#ifdef ENABLE_BUILD_AMON
s32 bsp_amon_init(void);

#else
static s32 inline bsp_amon_init(void)
{
    return 0;
}

#endif


#ifdef __cplusplus
}
#endif


#endif

