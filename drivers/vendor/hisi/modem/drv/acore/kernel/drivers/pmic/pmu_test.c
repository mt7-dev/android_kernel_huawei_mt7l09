
#include "pmu_balong.h"

struct pmu_test g_pmu_test;

/*****************************************************************************
 函 数 名  : bsp_pmu_test_enable
 功能描述  : 打开pmu模块test功能
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_pmu_test_enable(void)
{
    g_pmu_test.test_sw = 1;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_test_disable
 功能描述  :关闭pmu模块test功能
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :异常定位
*****************************************************************************/
void bsp_pmu_test_disable(void)
{
    g_pmu_test.test_sw = 0;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_test_register
 功能描述  : pmu模块test信息注册
 输入参数  : void
 输出参数  : 无
 返 回 值  : 注册成功或失败
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_test_register(pmic_id_e pmic_id,struct pmu_test_ops ops_func)
{
    if(PMIC_BUTTOM <= pmic_id)
    {
        pmic_print_error("platform doesn't support this PMIC!\n");
        return  BSP_PMU_PARA_ERROR;
    }

    g_pmu_test.test_ops[pmic_id]= ops_func;

    return  BSP_PMU_OK;

}
/*****************************************************************************
 函 数 名  : bsp_pmu_volt_onoff_test
 功能描述  : 测试pmu模块电压源开关接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_volt_onoff_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_onoff_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_onoff_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_onoff_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_volt_setget_test
 功能描述  : 测试pmu模块电压源设置电压接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_volt_setget_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_setget_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_setget_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_volt_mode_test
 功能描述  : 测试pmu模块电压源设置模式接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_volt_mode_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_mode_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_mode_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_mode_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_mode_setget_test
 功能描述  : 测试pmu模块电压源电压表保护
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_volt_table_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_table_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_table_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_table_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_test_init
 功能描述  : 初始化pmu test模块，将各pmic测试函数注册到测试框架
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_test_init(void)
{
    int iret = BSP_PMU_OK;

#ifdef CONFIG_PMIC_HI6451
    iret = bsp_hi6451_test_init();
#endif
#ifdef CONFIG_PMIC_HI6551
    iret |= bsp_hi6551_test_init();
#endif

#ifdef CONFIG_PMIC_HI6559
    iret |= bsp_hi6559_test_init();
#endif

    if(iret)
        pmic_print_error("init error!\n");
    else
        pmic_print_info("init ok!\n");

    return iret;
}
/*pmu adp接口的测试*/
#ifdef __KERNEL__
/*电流源部分接口*/
/*****************************************************************************
 函 数 名  : bsp_pmu_dr_onoff_test
 功能描述  : 测试pmu模块电流源开关接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 自动化测试
*****************************************************************************/
int bsp_pmu_dr_onoff_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_dr_onoff_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_dr_onoff_test)
            test_result |= g_pmu_test.test_ops[i].pmu_dr_onoff_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_dr_setget_test
 功能描述  : 测试pmu模块电流源设置电流接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_dr_setget_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_dr_setget_test)
            test_result |= g_pmu_test.test_ops[i].pmu_dr_setget_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_dr_mode_test
 功能描述  : 测试pmu模块电流源设置模式接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_dr_mode_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_mode_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_dr_mode_test)
            test_result |= g_pmu_test.test_ops[i].pmu_dr_mode_test(uctimes);
    }
    return test_result;
}
/*背光灯及三色灯功能测试用例*/
/*****************************************************************************
 函 数 名  : bsp_dr_fla_time_set_test
 功能描述  : 测试dr模块闪烁时间配置接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 系统测试
*****************************************************************************/
int bsp_dr_fla_time_set_test(unsigned on_us,unsigned off_us)
{
    DR_FLA_TIME fla_time;
    int iret;

    fla_time.fla_on_us = on_us;
    fla_time.fla_off_us = off_us;

    iret = bsp_dr_fla_time_set(&fla_time);
    return  iret;
}
/*****************************************************************************
 函 数 名  : bsp_dr_fla_time_set_test
 功能描述  : 测试dr模块呼吸时间配置接口
 输入参数  : uctimes:测试次数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 系统测试
*****************************************************************************/
int bsp_dr_bre_time_set_test(dr_id_e dr_id,unsigned on_ms,unsigned off_ms,unsigned rise_ms,unsigned fall_ms)
{
    DR_BRE_TIME bre_time;
    int iret;

    bre_time.bre_on_ms = on_ms;
    bre_time.bre_off_ms = off_ms;
    bre_time.bre_rise_ms = rise_ms;
    bre_time.bre_fall_ms = fall_ms;

    iret = bsp_dr_bre_time_set(dr_id,&bre_time);
    return  iret;
}

/*闪烁模式*/
int bsp_dr_fla_test_001()
{
    return bsp_dr_fla_time_set_test(DR_VALUE_INVALIED,DR_VALUE_INVALIED);
}
#endif
