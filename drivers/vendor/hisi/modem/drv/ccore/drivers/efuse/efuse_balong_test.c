/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  efuse_balong_test.c
*
*   作    者 :  
*
*   描    述 :  Efuse test
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/
#include <vxWorks.h>
#include <logLib.h>

#include "osl_bio.h"
#include "hi_base.h"
#include "hi_efuse.h"
#include "hi_syssc.h"
#include "efuse_balong.h"

#define EFUSE_OK               (0)
#define EFUSE_ERROR            (-1)

#ifdef HI_RF_STATUS_OFFSET
#define EFUSE_MAX_SIZE         (128)         /* P531 Efuse大小(单位为word) */
#else
#define EFUSE_MAX_SIZE         (16)         /* V7R2 Efuse大小(单位为word) */
#endif

/*由于efuse每次烧写完成，重启后才能读到efuse数据，
* 无法一次启动中，验证该次烧写是否正确。目前测试在
* console.c中添加相应控制代码，调用efuse测试函数，
* 重启后验证烧写是否正确。
*/

/*efuse测试函数会烧写group0~15组数据，请慎重调用*/

u32 efuseTestWriteData[] = 
{
    /* random value */
    0x73c6200a, 0x41987331, 0x7b63350e, 0x25b8e5c5, 0x4aec85e2,
    0x73c6200a, 0x41987331, 0x7b63350e, 0x25b8e5c5, 0x4aec85e2,
    0x73c6200a, 0x41987331, 0x7b63350e, 0x25b8e5c5, 0x4aec85e2,
    0x0000FF00
};

u32 efuseTestReadData[EFUSE_MAX_SIZE] = {0};

int efuse_test_separate_write()
{
    int i =0;
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        if(EFUSE_ERROR == bsp_efuse_write(&(efuseTestWriteData[i]), i, 1))
        {
            logMsg(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0, i, efuseTestWriteData[i]);
            return EFUSE_ERROR;
        }

    }

    return EFUSE_OK;
}

int efuse_test_separate_read()
{
    int i =0;
    
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        efuseTestReadData[i] = 0;
        if(EFUSE_ERROR == bsp_efuse_read( &(efuseTestReadData[i]), i,1))
        {
            logMsg(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0, i, efuseTestReadData[i]);
            return EFUSE_ERROR;
        }
    }
    
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        if(efuseTestWriteData[i] != efuseTestReadData[i])
        {
            logMsg(" Line %d, FUNC %s , efuseTestWriteData == efuseTestReadData fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
            return EFUSE_ERROR;
        }
    }

    return EFUSE_OK;

}

int efuse_test_continue_write()
{
    if(EFUSE_ERROR == bsp_efuse_write(efuseTestWriteData, 0, EFUSE_MAX_SIZE))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }

    return EFUSE_OK;
}

int efuse_test_continue_read()
{
    int i = 0;
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        efuseTestReadData[i] = 0;
    }

    if(EFUSE_ERROR == bsp_efuse_read(efuseTestReadData, 0, EFUSE_MAX_SIZE))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }

    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        if(efuseTestWriteData[i] != efuseTestReadData[i])
        {
            logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
            return EFUSE_ERROR;
        }
    }

    return EFUSE_OK;

}

#ifndef HI_RF_STATUS_OFFSET
int efuse_test_auto_read()
{
    u32 hwCfgValue = 0;
    u32 tsensorTrimValue = 0;
    u32 efuseGroup14Value = 0;
    u32 efuseGroup15Value = 0;
    
    if(EFUSE_ERROR == bsp_efuse_read(&efuseGroup15Value, 15, 1))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }
    
    hwCfgValue = get_hi_hw_cfg_hw_cfg();

    if(hwCfgValue != efuseGroup15Value)
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }
    
    if(EFUSE_ERROR == bsp_efuse_read(&efuseGroup14Value, 14, 1))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;

    }
    
    tsensorTrimValue = readl(HI_SC_STAT86_OFFSET);

    if(efuseGroup14Value != tsensorTrimValue)
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }

    return EFUSE_OK;
}
#endif

int efuse_test_repeat_write()
{
    int i =0;
    u32 efuseValue = 0;
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        efuseValue = efuseTestWriteData[i];
        if(EFUSE_ERROR == bsp_efuse_write(&efuseValue, i, 1))
        {
            logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
            return EFUSE_ERROR;
        }

        efuseValue = ~(efuseTestWriteData[i]);
        
        if(EFUSE_ERROR == bsp_efuse_write(&efuseValue, i, 1))
        {
            logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
            return EFUSE_ERROR;
        }

        if(EFUSE_ERROR == bsp_efuse_read(&efuseValue, i, 1))
        {
            logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
            return EFUSE_ERROR;
        }

        if(0xFFFFFFFF != efuseValue)
        {
            logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
            return EFUSE_ERROR;
        }
        
    }

    return EFUSE_OK;
}

int efuse_write_test()
{
    u32 efuseValue[] = 
    {
        /* ROOT Key Hash value */
        0x45003997, 0x68229d1a, 0xec67902c, 0xf4240b7a
    };

    if(EFUSE_ERROR == bsp_efuse_write(efuseValue, 0, sizeof(efuseValue)/sizeof(efuseValue[0])))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }

    return EFUSE_OK;
}

int efuse_read_test()
{
    u32 efuseValue[4] = {0};

    if(EFUSE_ERROR == bsp_efuse_read(efuseValue, 0, 4))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }

    if(0x45003997 !=efuseValue[0] || (0x68229d1a !=efuseValue[1]) || (0xec67902c !=efuseValue[2])
      || (0xf4240b7a !=efuseValue[3]))
    {
        logMsg(" Line %d, FUNC %s fail .\r\n", (int)__LINE__, (int)__FUNCTION__,0,0,0,0);
        return EFUSE_ERROR;
    }
    
    return EFUSE_OK;
}

int test_bsp_efuse_read_para_error()
{
    if(EFUSE_ERROR == bsp_efuse_read(NULL, 0, 1))
    {
        return EFUSE_OK;
    }

    return EFUSE_ERROR;
}

int test_bsp_efuse_write_para_error()
{
    if(EFUSE_ERROR == bsp_efuse_write(NULL, 0, 1))
    {
        return EFUSE_OK;
    }

    return EFUSE_ERROR;
}

int bsp_efuse_write_zero()
{
    u32 value = 0;
    if(EFUSE_ERROR == bsp_efuse_write(&value, 0, 1))
    {
        return EFUSE_ERROR;
    }

    return EFUSE_OK;    
}

extern void bsp_efuse_show();
int bsp_efuse_test(void)
{
    test_bsp_efuse_read_para_error();
    test_bsp_efuse_write_para_error();
    
	bsp_efuse_show();
    bsp_efuse_write_zero();
	
	return EFUSE_OK;
}




