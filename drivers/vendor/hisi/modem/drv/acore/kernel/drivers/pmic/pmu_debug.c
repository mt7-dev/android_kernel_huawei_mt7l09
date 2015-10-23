#include "pmu_balong.h"

struct pmu_dbg g_pmu_dbg;

/*函数声明*/
void bsp_pmu_dbg_enable(void);
void bsp_pmu_dbg_disable(void);
void bsp_pmu_volt_state(void);
void bsp_pmu_om_data_show(void);
void bsp_pmu_om_boot_show(void);
void bsp_pmu_exc_state(void);
void bsp_pmu_pmic_info(void);

/*函数实现*/
/*****************************************************************************
 函 数 名  : bsp_pmu_dbg_enable
 功能描述  :打开pmu模块dbg信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_pmu_dbg_enable(void)
{
    g_pmu_dbg.info_print_sw = 1;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_dbg_disable
 功能描述  :关闭pmu模块dbg信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :异常定位
*****************************************************************************/
void bsp_pmu_dbg_disable(void)
{
    g_pmu_dbg.info_print_sw = 0;
}
/*****************************************************************************
 函 数 名  : bsp_pmu_dbg_register
 功能描述  : pmu模块dbg信息注册
 输入参数  : void
 输出参数  : 无
 返 回 值  : 注册成功或失败
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_pmu_dbg_register(pmic_id_e pmic_id,struct pmu_dbg_ops ops_func)
{
    if(PMIC_BUTTOM <= pmic_id)
    {
        pmic_print_info("*****bsp_pmu_all_volt_state*****\n");
        return  BSP_PMU_PARA_ERROR;
    }

    g_pmu_dbg.dbg_ops[pmic_id]= ops_func;

    return  BSP_PMU_OK;

}
/*****************************************************************************
 函 数 名  : bsp_pmu_volt_state
 功能描述  : 显示当前PMIC的所有路电源的使用信息(包括开关，电压，)
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :异常定位
*****************************************************************************/
void bsp_pmu_volt_state(void)
{
    int i = 0;

    pmic_print_info("*****bsp_pmu_all_volt_state*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_dbg.dbg_ops[i].pmu_volt_state)
            g_pmu_dbg.dbg_ops[i].pmu_volt_state();
    }
}
/*****************************************************************************
 函 数 名  : bsp_pmu_om_data_show
 功能描述  : 显示此次运行过程中当前PMU状态的异常信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_pmu_om_data_show(void)
{
    int i = 0;

    pmic_print_info("*****bsp_pmu_om_data_show*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_dbg.dbg_ops[i].pmu_om_data_show)
            g_pmu_dbg.dbg_ops[i].pmu_om_data_show();
    }
}
/*****************************************************************************
 函 数 名  : bsp_pmu_boot_om_show
 功能描述  : 显示此次运行过程中当前PMU状态的异常信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_pmu_om_boot_show(void)
{
    int i = 0;

    pmic_print_info("*****bsp_pmu_boot_om_show*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_dbg.dbg_ops[i].pmu_om_boot_show)
            g_pmu_dbg.dbg_ops[i].pmu_om_boot_show();
    }
}
/*****************************************************************************
 函 数 名  : bsp_pmu_exc_state
 功能描述  : 显示当前所有PMIC的所有异常信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :异常定位
*****************************************************************************/
void bsp_pmu_exc_state(void)
{
    int i = 0;

    pmic_print_info("*****bsp_pmu_exc_state*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_dbg.dbg_ops[i].pmu_exc_state)
            g_pmu_dbg.dbg_ops[i].pmu_exc_state();
    }
}
/*****************************************************************************
 函 数 名  : bsp_pmu_pmic_info
 功能描述  : 显示当前所有PMIC的版本信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_pmu_pmic_info(void)
{
    int i = 0;

    pmic_print_info("*****bsp_pmu_pmic_info*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_dbg.dbg_ops[i].pmu_pmic_info)
            g_pmu_dbg.dbg_ops[i].pmu_pmic_info();
    }
}

