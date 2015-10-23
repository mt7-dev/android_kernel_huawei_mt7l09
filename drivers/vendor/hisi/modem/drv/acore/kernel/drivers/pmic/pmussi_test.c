
#include "osl_types.h"
#include "osl_bio.h"
#include <bsp_om.h>
#include <bsp_memmap.h>
#include <bsp_hardtimer.h>
#include <pmu_balong.h>
#include <bsp_pmu.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u16 regAddr;
    u8 regValue;

}PMUSSI_CFG_S;

PMUSSI_CFG_S g_SmartStar_CFG_V00[] =
{
    {0x40   ,   0x0A},
    {0x41   ,   0x13},
    {0x42   ,   0x0a},
    {0x43   ,   0x0d},
    {0x44   ,   0x0a},
    {0x45   ,   0x04},
    {0x46   ,   0x03},
    {0x47   ,   0x05},
    {0x9d   ,   0x04},
    {0x9e   ,   0x07},
    {0x9f   ,   0x06},
    {0xa0   ,   0x05},
    {0xa1   ,   0x03},
    {0xa2   ,   0x02},
    {0xa3   ,   0x01},
    {0xa4   ,   0x00},
    {0xa5   ,   0x01},
    {0xa6   ,   0x02},
    {0xa7   ,   0x03},
    {0xa8   ,   0x04},
    {0xa9   ,   0x05},
    {0xaa   ,   0x06},
    {0xab   ,   0x07},
    {0xac   ,   0x02},
    {0xad   ,   0x04},
    {0xae   ,   0x05},
    {0xaf   ,   0x06},
    {0xb0   ,   0x05},
    {0xb1   ,   0x07},
    {0xb2   ,   0x03},
    {0xb3   ,   0x02},
    {0xb4   ,   0x01},
};
PMUSSI_CFG_S g_SmartStar_CFG_V01[] =
{
    {0x9d   ,   0x05},
    {0x9e   ,   0x03},
    {0x9f   ,   0x01},
    {0xa0   ,   0x03},
    {0xa1   ,   0x03},
    {0xa2   ,   0x02},
    {0xa3   ,   0x07},
    {0xa4   ,   0x03},
    {0xa5   ,   0x01},
    {0xa6   ,   0x02},
    {0xa7   ,   0x05},
    {0xa8   ,   0x00},
    {0xa9   ,   0x03},
    {0xaa   ,   0x00},
    {0xab   ,   0x02},
    {0xac   ,   0x00},
    {0xad   ,   0x01},
    {0xae   ,   0x03},
    {0xaf   ,   0x07},
    {0xb0   ,   0x00},
    {0xb1   ,   0x05},
    {0xb2   ,   0x03},
    {0xb3   ,   0x04},
    {0xb4   ,   0x02},
};
/*----------------------------------基本寄存器操作接口---------------------------------------*/
u8 bsp_pmussi_read(u16 addr)
{
    u8 val = 0;

    bsp_hi6551_reg_read(addr, &val);

    return val;
}

void bsp_pmussi_write(u16 addr, u8 value)
{
    bsp_hi6551_reg_write(addr,value);
}

int bsp_pmussi_test(unsigned int count)
{
    int i, m;
    u8 value;

    for(i = 0; i <= count; i++)
    {
        for(m = 0; m < sizeof(g_SmartStar_CFG_V00)/sizeof(g_SmartStar_CFG_V00[0]); m++)
        {
            bsp_pmussi_write(g_SmartStar_CFG_V00[m].regAddr,g_SmartStar_CFG_V00[m].regValue);
        }

        for(m = 0; m < sizeof(g_SmartStar_CFG_V00)/sizeof(g_SmartStar_CFG_V00[0]); m++)
        {
            value = bsp_pmussi_read(g_SmartStar_CFG_V00[m].regAddr);

        if(g_SmartStar_CFG_V00[m].regValue != value)
            {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "g_SmartStar_CFG_V00:the value of addr %x is different\n", g_SmartStar_CFG_V00[m].regAddr);
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "g_SmartStar_CFG_V00:read value is %x,should be %x\n", value,g_SmartStar_CFG_V00[m].regValue);
            }
        }

        for(m = 0; m < sizeof(g_SmartStar_CFG_V01)/sizeof(g_SmartStar_CFG_V01[0]); m++)
        {
            bsp_pmussi_write(g_SmartStar_CFG_V01[m].regAddr,g_SmartStar_CFG_V01[m].regValue);
        }

        for(m = 0; m < sizeof(g_SmartStar_CFG_V01)/sizeof(g_SmartStar_CFG_V01[0]); m++)
        {
            value = bsp_pmussi_read(g_SmartStar_CFG_V01[m].regAddr);

            if(g_SmartStar_CFG_V01[m].regValue != value)
            {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "g_SmartStar_CFG_V01:the value of addr %x is different\n", g_SmartStar_CFG_V01[m].regAddr);
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "g_SmartStar_CFG_V01:read value is %x,should be %x\n", value,g_SmartStar_CFG_V01[m].regValue);
            }
        }
    }

    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "test over!\n");
}
int bsp_pmussi_run_time_test()
{
    s32 cur_time = 0;
    s32 run_time = 0;
    int m;
    u8 value;
    struct bsp_hardtimer_control pmu_ssi_timer = {23,1,0xffffffff,NULL,0};

    /*alloc timer*/
    if( 0 != bsp_hardtimer_alloc(&pmu_ssi_timer))
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "bsp_hardtimer_alloc fail!\n");
    bsp_hardtimer_enable(23);
    cur_time = bsp_get_timer_current_value(23);

    for(m = 0; m < sizeof(g_SmartStar_CFG_V00)/sizeof(g_SmartStar_CFG_V00[0]); m++)
    {
        bsp_pmussi_write(g_SmartStar_CFG_V00[m].regAddr,g_SmartStar_CFG_V00[m].regValue);
    }
    run_time = (cur_time - bsp_get_timer_current_value(23))/150;
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "write %d times time is  %d\n", sizeof(g_SmartStar_CFG_V00)/sizeof(g_SmartStar_CFG_V00[0]),run_time);

    cur_time = bsp_get_timer_current_value(23);
    for(m = 0; m < sizeof(g_SmartStar_CFG_V00)/sizeof(g_SmartStar_CFG_V00[0]); m++)
    {
        value = bsp_pmussi_read(g_SmartStar_CFG_V00[m].regAddr);
    }
    run_time = (cur_time - bsp_get_timer_current_value(23))/150;
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU, "read %d times time is  %d\n", sizeof(g_SmartStar_CFG_V00)/sizeof(g_SmartStar_CFG_V00[0]),run_time);
}
