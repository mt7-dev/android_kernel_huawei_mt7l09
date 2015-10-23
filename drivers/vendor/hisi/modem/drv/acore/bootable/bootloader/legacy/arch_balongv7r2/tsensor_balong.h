#ifndef	_TSENSOR_BALONG_H_
#define	_TSENSOR_BALONG_H_
#include "product_config.h"

#define TSENSOR_OK      0
#define TSENSOR_ERROR   -1

typedef enum {    
    TSENS_REGION_0   = 0,
    TSENS_REGION_1   = 1,
    TSENS_REGION_2   = 2,
    MAX_TSENS_REGION
} TSENSOR_REGION;


#ifdef CONFIG_TSENSOR

/*tsensor初始化设置*/
int tsensor_init();

/*tsensor获取芯片内部温度码值*/
int tsens_tem_get(TSENSOR_REGION region);

int chip_tem_get();

#ifndef __CMSIS_RTOS
/* chip温度获取，如果超过130℃，请丢弃。*/
int chip_tem_get(void);
#endif

#else
static inline int tsensor_init(void)
{
    return 0;
}

static inline int tsens_tem_get(TSENSOR_REGION region)
{
    return 60;
}

int chip_tem_get()
{
    return 60;
}

#endif

#endif

