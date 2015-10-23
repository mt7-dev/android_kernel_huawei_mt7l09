/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  efuse.c
*
*   作    者 :  wuzechun
*
*   描    述 :  Efuse读取
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/
#include "osl_bio.h"
#include "soc_clk.h"
#include "bsp_memmap.h"
#include "hi_base.h"
#include "hi_syscrg_interface.h"
#include "hi_efuse.h"
#include "hi_syssc.h"
#include "bsp_pmu.h"
#include "time.h"
#include "boot/boot.h"
#include "efuse.h"

#define TIMEMAXDELAY        (0x1000000)       /*最大延迟时间*/
/*由于efuse每次烧写完成，重启后才能读到efuse数据，
* 无法一次启动中，验证该次烧写是否正确。目前测试在
* console.c中添加相应控制代码，调用efuse测试函数，
* 重启后验证烧写是否正确。
*/
#define EFUSE_TEST_ENABLE

typedef unsigned int (*punptr)(void);

/*********************************************************************************************
Function:           test_bit_value
Description:        测试寄存器中的某位，是否等于指定的值
Calls:              无
Data Accessed:      无
Data Updated:       无
Input:              addr：  寄存器地址
                    bitMask：需要检测的位置
                    bitValue：指定的值
Output:             无
Return:             成功则返回TRUE， 失败则返回FALSE
Others:             无
*********************************************************************************************/
int test_bit_value(punptr pfun,  u32 bitValue)
{
    u32 time;                       /*延时时间*/

    for(time = 0; time < TIMEMAXDELAY; time++)
    {
        if(bitValue == (*pfun)())
        {
            return OK;
        }
    }

    return ERROR;
}

/*****************************************************************************
* 函 数 名  : bsp_efuse_read
*
* 功能描述  : 按组读取EFUSE中的数据
*
* 输入参数  : group  起始group
*                   num  数组长度(word数,不超过512/4=128)
* 输出参数  : pBuf ：EFUSE中的数据
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int bsp_efuse_read(u32* pBuf, u32 group, u32 num)
{
    u32* pSh;
    u32 cnt;


    if((group + num > EFUSE_MAX_SIZE) || (NULL == pBuf))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);

        return ERROR;
    }

    /* efuse clk enable */
    hi_syscrg_efuse_clk_enble();

    /*判断是否处在power_down状态*/
    if(get_hi_efusec_status_pd_status())
    {
       /*退出powerd_down状态*/
        set_hi_efusec_cfg_pd_en(0);

        /* 等待powerd_down退出标志设置成功，否则超时返回错误 */
        if(OK != test_bit_value(get_hi_efusec_status_pd_status, 0))
        {
            cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
            goto efuse_err;
        }
    }

#ifdef HI_RF_STATUS_OFFSET
    /*设置efuse接口时序为内部产生*/
    set_hi_efusec_cfg_signal_sel(1);
#else
    /*选择信号为apb操作efuse*/
    set_hi_efusec_cfg_aib_sel(1);
#endif

    pSh = pBuf;

    /* 循环读取efuse值 */
    for(cnt = 0; cnt < num; cnt++)
    {
        /* 设置读取地址 */
        set_hi_efuse_group_efuse_group( group+cnt);

        /* 使能读 */
        set_hi_efusec_cfg_rden(1);

        /* 等待读完成 */
        if(OK != test_bit_value(get_hi_efusec_status_rd_status, 1))
        {
            cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
            goto efuse_err;
        }
        /* 读取数据 */
        *pSh = get_hi_efusec_data_efusec_data();
        pSh++;

    }
    /* efuse clk disable */
    hi_syscrg_efuse_clk_disable();
    return OK;

efuse_err:
    /* efuse clk disable */
    hi_syscrg_efuse_clk_disable();
    return ERROR;
}

/*****************************************************************************
* 函 数 名  : bsp_efuse_Write
*
* 功能描述  : 烧写Efsue
*
* 输入参数  : pBuf:待烧写的EFUSE值
*                 group,Efuse地址偏移
*                 len,烧写长度
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int bsp_efuse_write( u32 *pBuf, u32 group, u32 len)
{

    u32 u32Grp;
    u32 *pu32Value;
    u32 i;
    int iRet = BSP_PMU_OK;
    int volt = 1800000;

    if((group + len > EFUSE_MAX_SIZE) || (NULL == pBuf))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    iRet = bsp_pmu_efuse_volt_set(volt);
    if(BSP_PMU_OK != iRet)
    {
        cprintf(" efuse power voltage error!\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }
    iRet = bsp_pmu_efuse_power_switch(POWER_ON);
    if(BSP_PMU_OK != iRet)
    {
        cprintf(" efuse power on failed!\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    /* efuse clk enable */
    hi_syscrg_efuse_clk_enble();

    mdelay(1);

    /*判断是否处在power_down状态*/
    if(get_hi_efusec_status_pd_status())
    {
       /*退出powerd_down状态*/
        set_hi_efusec_cfg_pd_en(0);
        /* 等待powerd_down退出标志设置成功，否则超时返回错误 */
        if(OK != test_bit_value(get_hi_efusec_status_pd_status, 0))
        {
            cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
            goto efusee_err;
        }
    }

    /* 判断是否允许烧写,为1禁止烧写,直接退出 */
    if(get_hi_hw_cfg_hw_cfg() & 0x1)
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        goto efusee_err;
    }

#ifdef HI_RF_STATUS_OFFSET
    /*设置efuse接口时序为内部产生*/
    set_hi_efusec_cfg_signal_sel(1);
#else
    /*选择信号为apb操作efuse*/
    set_hi_efusec_cfg_aib_sel(1);
#endif

    /* 配置时序要求 */
    set_hi_efusec_count_efusec_count(EFUSE_COUNT_CFG);
    set_hi_pgm_count_pgm_count(PGM_COUNT_CFG);

    /* 使能预烧写 */
    set_hi_efusec_cfg_pre_pg(1);

    /*查询是否使能置位 */
    if(OK != test_bit_value(get_hi_efusec_status_pgenb_status, 1))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        goto efusee_err;
    }

    /* 循环烧写 */
    u32Grp = group;
    pu32Value = pBuf;
    for (i = 0; i < len; i++)
    {
        /* 设置group */
        set_hi_efuse_group_efuse_group(u32Grp);

        /* 设置value */
        set_hi_pg_value_pg_value(*pu32Value);

        /* 使能烧写 */
        set_hi_efusec_cfg_pgen(1);

        /* 查询烧写完成 */
        if(OK != test_bit_value(get_hi_efusec_status_pg_status, 1))
        {
            cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
            goto efusee_err;
        }

        /* 烧写下一组 */
        u32Grp++;
        pu32Value++;
    }

    /*去使能预烧写*/
    set_hi_efusec_cfg_pre_pg(0);

    /* efuse clk disable */
    hi_syscrg_efuse_clk_disable();

    bsp_pmu_efuse_power_switch(POWER_OFF);

    return OK;
    
efusee_err:
    /* efuse clk disable */
    hi_syscrg_efuse_clk_disable();

    bsp_pmu_efuse_power_switch(POWER_OFF);

    return ERROR;
}

void efuse_info_show(void)
{
    u32 i = 0;
    u32 value = 0;

    for(i = 0;i < EFUSE_MAX_SIZE;i++)
    {
        if(OK == bsp_efuse_read(&value, i, 1))
        {
            cprintf("efuse group%d value = 0x%x.\n ", i,value);
        }
        else
        {
            cprintf("Line %d, FUNC %s , efuse group%d read fail.\n", (int)__LINE__, (int)__FUNCTION__, i);
            return;
        }
    }

}

/*efuse测试函数会烧写group0~15组数据，请慎重调用*/
#ifdef EFUSE_TEST_ENABLE

u32 efuseTestWriteData[] =
{
    /* random value */
    0x73c6200a, 0x41987331, 0x7b63350e, 0x25b8e5c5, 0x4aec85e2,
    0x73c6200a, 0x41987331, 0x7b63350e, 0x25b8e5c5, 0x4aec85e2,
    0x73c6200a, 0x41987331, 0x7b63350e, 0x25b8e5c5, 0x4aec85e2,
    0x0000FF00
};

u32 efuseTestReadData[EFUSE_MAX_SIZE] = {0};

int efuse_test_separate_write(void)
{
    int i =0;
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        if(ERROR == bsp_efuse_write(&(efuseTestWriteData[i]), i, 1))
        {
            cprintf(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\n", (int)__LINE__, (int)__FUNCTION__, i, efuseTestWriteData[i]);
            return ERROR;
        }

    }

    return OK;
}

int efuse_test_separate_read(void)
{
    int i =0;

    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        efuseTestReadData[i] = 0;
        if(ERROR == bsp_efuse_read(&(efuseTestReadData[i]), i, 1))
        {
            cprintf(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\n", (int)__LINE__, (int)__FUNCTION__, i, efuseTestReadData[i]);
            return ERROR;
        }
    }

    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        if(efuseTestWriteData[i] != efuseTestReadData[i])
        {
            cprintf(" Line %d, FUNC %s , efuseTestWriteData == efuseTestReadData fail .\n", (int)__LINE__, (int)__FUNCTION__);
            return ERROR;
        }
    }

    return OK;

}

int efuse_test_continue_write(void)
{
    if(ERROR == bsp_efuse_write(efuseTestWriteData, 0, EFUSE_MAX_SIZE))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    return OK;
}

int efuse_test_continue_read(void)
{
    int i = 0;
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        efuseTestReadData[i] = 0;
    }

    if(ERROR == bsp_efuse_read(efuseTestReadData, 0, EFUSE_MAX_SIZE))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        if(efuseTestWriteData[i] != efuseTestReadData[i])
        {
            cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
            return ERROR;
        }
    }

    return OK;

}

#ifndef HI_RF_STATUS_OFFSET
int efuse_test_auto_read()
{
    u32 hwCfgValue = 0;
    u32 tsensorTrimValue = 0;
    u32 efuseGroup14Value = 0;
    u32 efuseGroup15Value = 0;

    if(ERROR == bsp_efuse_read(&efuseGroup15Value, 15, 1))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    hwCfgValue = get_hi_hw_cfg_hw_cfg();

    if(hwCfgValue != efuseGroup15Value)
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    if(ERROR == bsp_efuse_read(&efuseGroup14Value, 14, 1))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;

    }

    tsensorTrimValue = readl(HI_SC_STAT86_OFFSET);

    if(efuseGroup14Value != tsensorTrimValue)
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    return OK;
}
#endif

int efuse_test_repeat_write(void)
{
    int i =0;
    u32 efuseValue = 0;
    for(i = 0; i < EFUSE_MAX_SIZE; i++)
    {
        efuseValue = efuseTestWriteData[i];
        if(ERROR == bsp_efuse_write(&efuseValue, i, 1))
        {
            cprintf(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\n", (int)__LINE__, (int)__FUNCTION__, i, efuseValue);
            return ERROR;
        }

        efuseValue = ~(efuseTestWriteData[i]);

        if(ERROR == bsp_efuse_write(&efuseValue, i, 1))
        {
            cprintf(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\n", (int)__LINE__, (int)__FUNCTION__, i, efuseValue);
            return ERROR;
        }

        if(ERROR == bsp_efuse_read(&efuseValue, i, 1))
        {
            cprintf(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\n", (int)__LINE__, (int)__FUNCTION__, i, efuseValue);
            return ERROR;
        }

        if(0xFFFFFFFF != efuseValue)
        {
            cprintf(" Line %d, FUNC %s, group %d, efuse data 0x%x fail .\n", (int)__LINE__, (int)__FUNCTION__, i, efuseValue);
            return ERROR;
        }

    }

    return OK;
}

int efuse_write_test(void)
{
    u32 efuseValue[] =
    {
        /* ROOT Key Hash value */
        0x45003997, 0x68229d1a, 0xec67902c, 0xf4240b7a
    };

    if(ERROR == bsp_efuse_write(efuseValue, 0, sizeof(efuseValue)/sizeof(efuseValue[0])))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    return OK;
}

int efuse_read_test(void)
{
    u32 efuseValue[4] = {0};

    if(ERROR == bsp_efuse_read(0, efuseValue, 4))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    if(0x45003997 !=efuseValue[0] || (0x68229d1a !=efuseValue[1]) || (0xec67902c !=efuseValue[2])
      || (0xf4240b7a !=efuseValue[3]))
    {
        cprintf(" Line %d, FUNC %s fail .\n", (int)__LINE__, (int)__FUNCTION__);
        return ERROR;
    }

    return OK;
}

#endif

