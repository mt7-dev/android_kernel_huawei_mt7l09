
/*test hi6551 volt enable/isenable*/
/*notice:找能够关闭的电源进行测试*/
#include <osl_bio.h>
#include <osl_thread.h>
#include <bsp_shared_ddr.h>
#if defined(__KERNEL__)
#include "dr_hi6551.h"
#endif
#include <pmu_balong.h>

#define PMIC_HI6551_TEST_TASK_PRI                (25)
#define PMIC_HI6551_TEST_TASK_STACK_SIZE    (0x1000)

int volt_onoff_test[]={PMIC_HI6551_BUCK2,PMIC_HI6551_BUCK5,PMIC_HI6551_LDO05,PMIC_HI6551_LDO13,PMIC_HI6551_LDO15,\
PMIC_HI6551_LDO16,PMIC_HI6551_LDO17,PMIC_HI6551_LDO18,PMIC_HI6551_LDO19,PMIC_HI6551_LDO20,PMIC_HI6551_LDO21,\
PMIC_HI6551_LVS02,PMIC_HI6551_LVS03,PMIC_HI6551_LVS06,PMIC_HI6551_LVS07,PMIC_HI6551_LVS08};
int volt_setget_test[]={PMIC_HI6551_LDO05,PMIC_HI6551_LDO13,PMIC_HI6551_LDO15,\
PMIC_HI6551_LDO16,PMIC_HI6551_LDO17,PMIC_HI6551_LDO18,PMIC_HI6551_LDO19,PMIC_HI6551_LDO20,PMIC_HI6551_LDO21,\
};
int mode_set_get_test[]={PMIC_HI6551_BUCK5,PMIC_HI6551_LDO09,PMIC_HI6551_LDO10,PMIC_HI6551_LDO11,PMIC_HI6551_LDO24};

/*para check func*/
static __inline__ int hi6551_volt_test_table_check(PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table)
{
    if((SHM_PMU_VOLTTABLE_MAGIC_START_DATA == hi6551_volt_table->magic_start) && \
        (SHM_PMU_VOLTTABLE_MAGIC_END_DATA == hi6551_volt_table->magic_end))
        return BSP_PMU_OK;
    else
        return BSP_PMU_ERROR;
}

int hi6551_volt_enable_test(void)
{
    int i = 0;
    int j = 0;
    int test_num = (sizeof(volt_onoff_test)/sizeof(int));

    for(j = 0;j < test_num; j++)
    {
        i = volt_onoff_test[j];
        (void)bsp_hi6551_volt_enable(i);
        if(0 == bsp_hi6551_volt_is_enabled(i))
        {
            pmic_print_error("\n volt %d: enable or is_enabled failed!\n", i);
            return i;
        }
    }
    pmic_print_info("hi6551_volt_enable_test ok!!\n",i);

    return BSP_PMU_OK;
}
/*test hi6551 volt disable/isenable*/
int hi6551_volt_disable_test(void)
{
    int i = 0;
    int j = 0;
    int test_num = (sizeof(volt_onoff_test)/sizeof(int));

    for(j = 0;j < test_num; j++)
    {
        i = volt_onoff_test[j];
        (void)bsp_hi6551_volt_disable(i);
        if(0 != bsp_hi6551_volt_is_enabled(i))
        {
           pmic_print_error( "\n volt  %d: disable or is_enabled failed!\n", i);
           return i;
        }
    }
    pmic_print_info("hi6551_volt_disable_test ok!!\n",i);
    return BSP_PMU_OK;
}
/*异常分支测试*/
/*enable/disable err branch*/
int hi6551_volt_onoff_err_test(void)
{
    int iret = 0;
    int result = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;

    /*异常id*/
    iret = bsp_hi6551_volt_enable(PMIC_HI6551_VOLT_MIN - 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_enable err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_volt_disable(PMIC_HI6551_VOLT_MAX + 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_disable err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_volt_is_enabled(PMIC_HI6551_VOLT_MIN - 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_is_enabled err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*电压表被破坏*/
    /*人为改写table前的魔幻数*/
    hi6551_volt_table->magic_start = 0xa0a0a0;
    iret = bsp_hi6551_volt_enable(mode_set_get_test[0]);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_enable err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    iret = bsp_hi6551_volt_disable(mode_set_get_test[0]);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_disable err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    iret = bsp_hi6551_volt_is_enabled(mode_set_get_test[0]);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_is_enabled err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*恢复table前的魔幻数*/
    hi6551_volt_table->magic_start = SHM_PMU_VOLTTABLE_MAGIC_START_DATA;

    return result;
}
/*test hi6551 volt set/get voltage,only for boost and ldo*/
/*notice: some volt cannot set too high,buck need test in other way*/
int hi6551_volt_set_get_voltage_test(void)
{
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;
    PMIC_HI6551_VLTGS_ATTR *hi6551_volt;
    unsigned selector = 0;
    int voltage = 0;
    int i,test_sub,iret = 0;
    u8 j = 0;
    int test_num = (sizeof(volt_setget_test)/sizeof(int));

    /*DDR中电压属性表安全性检查*/
    iret = hi6551_volt_test_table_check(hi6551_volt_table);
    if(BSP_PMU_OK != iret)
        return  BSP_PMU_VOLTTABLE_ERROR;

    for(test_sub = 0;test_sub < test_num; test_sub++)
    {
        i = volt_setget_test[test_sub];
        hi6551_volt = &hi6551_volt_table->hi6551_volt_attr[i];

        for(j = 0; j <= hi6551_volt->voltage_nums;j++)
        {
            voltage = hi6551_volt->voltage_list[j];
            (void)bsp_hi6551_volt_set_voltage(i,voltage,voltage,&selector);
            if(voltage != bsp_hi6551_volt_get_voltage(i))
            {
                pmic_print_error("id %d set or get voltage error!!\n",i);
                return i;
            }
            if(voltage != bsp_hi6551_volt_list_voltage(i,selector))
            {
                pmic_print_error("id %d set or list voltage error!!\n",selector);
                return i;
            }
        }
    }
    pmic_print_info("hi6551_volt_set_get_voltage_test ok!!\n",i);
    return BSP_PMU_OK;
}
extern int bsp_hi6551_volt_set_voltage_test(int volt_id, int min_uV, int max_uV);
int hi6551_volt_setget_err_test(void)
{
    int iret = 0;
    int result = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;

    /*异常id*/
    iret = bsp_hi6551_volt_set_voltage_test(PMIC_HI6551_VOLT_MIN - 1,0,0);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_set_voltage err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_volt_get_voltage(PMIC_HI6551_VOLT_MAX + 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_get_voltage err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_volt_list_voltage(PMIC_HI6551_VOLT_MIN - 1,0);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_is_enabled err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*电压表被破坏*/
    /*人为改写table后的魔幻数*/
    hi6551_volt_table->magic_end = 0xa0a0a0;
    iret = bsp_hi6551_volt_set_voltage_test(volt_setget_test[0],0,0);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_set_voltage err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    iret = bsp_hi6551_volt_get_voltage(volt_setget_test[0]);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_get_voltage err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    iret = bsp_hi6551_volt_list_voltage(volt_setget_test[0],0);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_list_voltage err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*恢复table后的魔幻数*/
    hi6551_volt_table->magic_end = SHM_PMU_VOLTTABLE_MAGIC_END_DATA;

    /*无效电压值*/
    iret = bsp_hi6551_volt_set_voltage_test(volt_setget_test[0],0,0);
    if(BSP_PMU_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_set_voltage err voltage test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*无效档位值*/
    iret = bsp_hi6551_volt_list_voltage(volt_setget_test[0],500);
    if(BSP_PMU_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_list_voltage err selector test error!\n");
        result = BSP_PMU_ERROR;
    }

    return result;
}
/*****************************************************************************
 函 数 名  : hi6551_force_mode_set_get_test
 功能描述  : 强制eco模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_force_mode_set_get_test(void)
{
    int id_test = 0;
    int i,iret = 0;
    int result = BSP_PMU_OK;
    pmu_mode_e  mode = PMU_MODE_BUTTOM;
    int test_num = (sizeof(mode_set_get_test)/sizeof(int));

    for(i = 0;i < test_num;i++)
    {
        id_test = mode_set_get_test[i];
        iret = bsp_hi6551_volt_set_mode(id_test,PMU_MODE_ECO_FORCE);
        if(BSP_PMU_OK != iret)
        {
            pmic_print_error("hi6551 volt %d set_force_mode error\n",id_test);
            result = BSP_PMU_ERROR;
            break;
        }
        else
        {
            mode = bsp_hi6551_volt_get_mode(id_test);
            if(PMU_MODE_ECO_FORCE != mode)
            {
                pmic_print_error("hi6551 volt %d force_mode_set_get_test error\n",id_test);
                result = BSP_PMU_ERROR;
                break;
            }
        }
    }

    return result;
}
/*****************************************************************************
 函 数 名  : hi6551_normal_mode_set_get_test
 功能描述  : normal模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_normal_mode_set_get_test(void)
{
    int id_test = 0;
    int i,iret = 0;
    int result = BSP_PMU_OK;
    pmu_mode_e  mode = PMU_MODE_BUTTOM;
    int test_num = (sizeof(mode_set_get_test)/sizeof(int));

    for(i = 0;i< test_num;i++)
    {
        id_test = mode_set_get_test[i];
        iret = bsp_hi6551_volt_set_mode(id_test,PMU_MODE_NORMAL);
        if(BSP_PMU_OK != iret)
        {
            pmic_print_error("hi6551 volt %d set_normal_mode error\n",id_test);
            result = BSP_PMU_ERROR;
            break;
        }
        else
        {
            mode = bsp_hi6551_volt_get_mode(id_test);
            if(PMU_MODE_NORMAL != mode)
            {
                pmic_print_error("hi6551_normal_mode_set_get_test error\n");
                result = BSP_PMU_ERROR;
                break;
            }
        }
    }

    return result;

}
/*****************************************************************************
 函 数 名  : hi6551_follow_mode_set_get_test
 功能描述  : follow模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_follow_mode_set_get_test(void)
{
    int id_test = 0;
    int i,iret = 0;
    int result = BSP_PMU_OK;
    pmu_mode_e  mode = PMU_MODE_BUTTOM;
    int test_num = (sizeof(mode_set_get_test)/sizeof(int));

    for(i = 0;i< test_num;i++)
    {
        id_test = mode_set_get_test[i];
        iret = bsp_hi6551_volt_set_mode(id_test,PMU_MODE_ECO_FOLLOW);
        if(BSP_PMU_OK != iret)
        {
            pmic_print_error("hi6551 volt %d set_follow_mode error\n",id_test);
            result = BSP_PMU_ERROR;
            break;
        }
        else
        {
            mode = bsp_hi6551_volt_get_mode(id_test);
            if(PMU_MODE_ECO_FOLLOW != mode)
            {
                pmic_print_error("hi6551_follow_mode_set_get_test error\n");
                result = BSP_PMU_ERROR;
                break;
            }
        }
    }

    return result;
}
/*****************************************************************************
 函 数 名  : hi6551_volt_mode_err_test
 功能描述  : 模式错误分支测试
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_volt_mode_err_test(void)
{
    int iret = 0;
    int result = BSP_PMU_OK;
    pmu_mode_e  mode = PMU_MODE_BUTTOM;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;

    /*异常id*/
    iret = bsp_hi6551_volt_set_mode(PMIC_HI6551_VOLT_MIN - 1,PMU_MODE_NORMAL);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_set_mode err para test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    mode = bsp_hi6551_volt_get_mode(PMIC_HI6551_VOLT_MAX + 1);/*lint !e415 !e831*/
    if(PMU_MODE_BUTTOM != mode)
    {
        pmic_print_error("bsp_hi6551_volt_get_mode err para test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*电压表被破坏*/
    /*人为改写table前的魔幻数*/
    hi6551_volt_table->magic_start = 0xa0a0a0;
    iret = bsp_hi6551_volt_set_mode(mode_set_get_test[0],PMU_MODE_NORMAL);/*lint !e415 !e831*/
    if(BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_set_mode err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    mode = bsp_hi6551_volt_get_mode(mode_set_get_test[0]);
    if(PMU_MODE_BUTTOM != mode)
    {
        pmic_print_error("bsp_hi6551_volt_get_mode err table test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*恢复table前的魔幻数*/
    hi6551_volt_table->magic_start = SHM_PMU_VOLTTABLE_MAGIC_START_DATA;

    /*模式错误*/
    iret = bsp_hi6551_volt_set_mode(mode_set_get_test[0],PMU_MODE_BUTTOM);
    if(BSP_PMU_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_volt_set_mode err mode test error!\n");
        result = BSP_PMU_ERROR;
    }

    return result;

}
/*ddr头被改写测试*/
int hi6551_volttable_magic_start_test(void)
{
    int iret;
    int result = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;

    /*人为改写table前的魔幻数*/
    hi6551_volt_table->magic_start = 0xa0a0a0;
    iret = bsp_hi6551_volt_is_enabled(20);
    if( BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("hi6551_volttable_magic_start_test error\n");
        result = BSP_PMU_ERROR;
    }
    else
        result = BSP_PMU_OK;

    /*恢复table前的魔幻数*/
    hi6551_volt_table->magic_start = SHM_PMU_VOLTTABLE_MAGIC_START_DATA;
    return result;
}
/*ddr尾被改写测试*/
int hi6551_volttable_magic_end_test(void)
{
    int iret;
    int result = BSP_PMU_OK;
    PMIC_HI6551_VLTGS_TABLE *hi6551_volt_table = (PMIC_HI6551_VLTGS_TABLE *)SHM_PMU_VOLTTABLE_ADDR;

    /*人为改写table后的魔幻数*/
    hi6551_volt_table->magic_end = 0x0a0a0a0a;
    iret = bsp_hi6551_volt_get_voltage(20);
    if( BSP_PMU_VOLTTABLE_ERROR != iret)
    {
        pmic_print_error("hi6551_volttable_magic_end_test error\n");
        result = BSP_PMU_ERROR;
    }
    else
        result = BSP_PMU_OK;

    /*恢复table后的魔幻数*/
    hi6551_volt_table->magic_end = SHM_PMU_VOLTTABLE_MAGIC_END_DATA;
    return result;
}

int bsp_hi6551_volt_onoff_test(u32 test_times)
{
    int fail_times[3] = {0,0,0};
    u32 test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_volt_enable_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }

        result = hi6551_volt_disable_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }

        result = hi6551_volt_onoff_err_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[2]++;
        }
    }

    if((0 != fail_times[0])||(0 != fail_times[1])||(0 != fail_times[2]))
    {
        pmic_print_error("test %d times ,enable %d times failed,disable %d times failed,err check %d times failed!!\n",test_times,fail_times[0],fail_times[1],fail_times[2]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }
}
int bsp_hi6551_volt_setget_test(u32 test_times)
{
    int fail_times[2] = {0,0};
    u32 test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_volt_set_get_voltage_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }
        result = hi6551_volt_setget_err_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }
    }

    if((0 != fail_times[0])||(0 != fail_times[1]))
    {
        pmic_print_error("test %d times ,setget %d times failed,err check %d failed!!\n",test_times,fail_times[0],fail_times[1]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }

}

int bsp_hi6551_volt_mode_test(u32 test_times)
{
    int fail_times[4] = {0,0,0,0};
    u32 test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_force_mode_set_get_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }
        result = hi6551_normal_mode_set_get_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }
        result = hi6551_follow_mode_set_get_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[2]++;
        }
        result = hi6551_volt_mode_err_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[3]++;
        }
    }

    if((0 != fail_times[0])||(0 != fail_times[1])||(0 != fail_times[2])||(0 != fail_times[3]))
    {
        pmic_print_error("test %d times ,force mode: %d times failed,normal mode: %d times failed,eco mode :%d times failed,err check:%d times failed!!\n",\
                        test_times,fail_times[0],fail_times[1],fail_times[2],fail_times[3]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }

}

/*DDR电源表安全性测试,测完需要再写正确，否则会影响后续测试*/
int bsp_hi6551_volttable_test(u32 test_times)
{
    int fail_times[2] = {0,0};
    u32 test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_volttable_magic_start_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }

        result = hi6551_volttable_magic_end_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }
    }

    if((0 != fail_times[0])||(0 != fail_times[1]))
    {
        pmic_print_error("test %d times ,magic start: %d times failed,magic end: %d times failed!!\n",\
                        test_times,fail_times[0],fail_times[1]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }
}

/*以下代码是为回片，验证硬件提供的优化reg配置的测试*/
typedef struct
{
    u16 regAddr;
    u8 regValue;
    u8 reserved;
}SMARTSTAR_CFG_S;
SMARTSTAR_CFG_S g_SmartStar_CFG_V03[] =
{
    { 0x117 ,0x02},
    { 0X5A  ,0x3c},
    { 0X61  ,0x00},
    { 0X5F  ,0x12},
    { 0X60  ,0x03},
    { 0X5D  ,0x20},
    { 0X5E  ,0x01},
    { 0X65  ,0x01},
    { 0X6C  ,0x00},
    { 0X6B  ,0x03},
    { 0X6A  ,0x12},
    { 0X69  ,0x11},
    { 0X71  ,0x01},
    { 0X76  ,0x12},
    { 0X78  ,0x01},
    { 0X77  ,0x03},
    { 0X74  ,0x00},
    { 0X75  ,0x01},
    { 0X7A  ,0x15},
    { 0X7C  ,0x01},
    { 0X81  ,0x12},
    { 0x80  ,0x03},
    { 0x83  ,0x00},
    { 0X82  ,0x03},
    { 0X7F  ,0x01},
    { 0X87  ,0x01},
    { 0X90  ,0x00},
    { 0X8E  ,0x12},
    { 0x8D  ,0x01},
    { 0X8F  ,0x03},
    { 0X92  ,0x13},
    { 0X95  ,0x01},
    { 0X0BA ,0X0E},
    { 0X0BB ,0X32},
    { 0X0BC ,0X3B},
    { 0X0B2 ,0X31},
    { 0x025 ,0xFE},
    { 0x022 ,0xFA},
    { 0x028 ,0xAA},
    { 0x02B ,0xEA},
};
SMARTSTAR_CFG_S g_SmartStar_CFG_V230[] =
{
    {0X5A,0x3c},
    {0X61,0x00},
    {0X5F,0x12},
    {0X60,0x03},
    {0X5D,0x20},
    {0X5E,0x01},
    {0X6C,0x00},
    {0X6B,0x03},
    {0X6A,0x12},
    {0X69,0x11},
    {0X76,0x12},
    {0X78,0x01},
    {0X77,0x03},
    {0X74,0x00},
    {0X75,0x01},
    {0X7A,0x13},
    {0X81,0x12},
    {0x80,0x03},
    {0x83,0x00},
    {0X82,0x03},
    {0X7F,0x01},
    {0X90,0x00},
    {0X8E,0x12},
    {0x8D,0x01},
    {0X8F,0x03},
    {0X92,0x13},
    {0x020,0x45},
    {0X0BA,0X0E},
    {0X0BB,0X32},
    {0X0BC,0X3B},
    {0x0B2,0x01},
};
/*****************************************************************************
 函 数 名  : bsp_hi6551_optreg_test
 功能描述  : hi6551 优化reg配置测试
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_hi6551_optreg_test(void)
{
    u32 m;
    u8 value;
    u32 test_num = 0;

    if(BSP_PMU_V230 == bsp_pmu_version_get())
    {
        test_num = sizeof(g_SmartStar_CFG_V230)/sizeof(SMARTSTAR_CFG_S);
        for(m = 0; m < test_num; m++)
        {
            bsp_hi6551_reg_read(g_SmartStar_CFG_V230[m].regAddr,&value);

            pmic_print_info("the value of addr %x is %x\n", g_SmartStar_CFG_V230[m].regAddr,value);

            if(g_SmartStar_CFG_V03[m].regValue != value)
            {
                pmic_print_error("the value of addr %x is different\n", g_SmartStar_CFG_V230[m].regAddr);
                pmic_print_error("read value is %x,should be %x\n", value,g_SmartStar_CFG_V230[m].regValue);
            }
        }
    }
    else if((BSP_PMU_V210 == bsp_pmu_version_get())||(BSP_PMU_V220 == bsp_pmu_version_get()))
    {
        test_num = sizeof(g_SmartStar_CFG_V03)/sizeof(SMARTSTAR_CFG_S);
        for(m = 0; m < test_num; m++)
        {
            bsp_hi6551_reg_read(g_SmartStar_CFG_V03[m].regAddr,&value);

            pmic_print_info("the value of addr %x is %x\n", g_SmartStar_CFG_V03[m].regAddr,value);

            if(g_SmartStar_CFG_V03[m].regValue != value)
            {
                pmic_print_error("the value of addr %x is different\n", g_SmartStar_CFG_V03[m].regAddr);
                pmic_print_error("read value is %x,should be %x\n", value,g_SmartStar_CFG_V03[m].regValue);
            }
        }
    }

    pmic_print_error("test ok!\n");
    return;
}

#ifdef __KERNEL__
/*dr测试*/
int hi6551_dr_enable_test(void)
{
    int id_test = 0;

    for(id_test = PMIC_HI6551_DR_MIN;id_test <= PMIC_HI6551_DR_MAX; id_test++)
    {
        /*fla模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_FLA_FLASH);
        (void)bsp_hi6551_dr_enable(id_test);
        if(0 == bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_FLA_FLASH:dr %d: enable or is_enabled failed!\n", id_test);
            return id_test;
        }
        /*bre模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_FLASH);
        (void)bsp_hi6551_dr_enable(id_test);
        if(0 == bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_BRE_FLASH:dr %d: enable or is_enabled failed!\n", id_test);
            return id_test;
        }
        /*fla模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_FLA_LIGHT);
        (void)bsp_hi6551_dr_enable(id_test);
        if(0 == bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_FLA_LIGHT:dr %d: enable or is_enabled failed!\n", id_test);
            return id_test;
        }
        /*bre模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE);
        (void)bsp_hi6551_dr_enable(id_test);
        if(0 == bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_BRE:dr %d: enable or is_enabled failed!\n", id_test);
            return id_test;
        }
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_LIGHT);
        (void)bsp_hi6551_dr_enable(id_test);
        if(0 == bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_BRE_LIGHT:dr %d: enable or is_enabled failed!\n", id_test);
            return id_test;
        }
    }
    pmic_print_info("test ok!!\n");

    return BSP_PMU_OK;
}
int hi6551_dr_disable_test(void)
{
    int id_test = 0;

    for(id_test = PMIC_HI6551_DR_MIN;id_test <= PMIC_HI6551_DR_MAX; id_test++)
    {
        /*fla模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_FLA_FLASH);
        (void)bsp_hi6551_dr_disable(id_test);
        if(0 != bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_FLA_FLASH:dr %d: disable or is_enabled failed!\n", id_test);
            return id_test;
        }
        /*bre模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_FLASH);
        (void)bsp_hi6551_dr_disable(id_test);
        if(0 != bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_BRE_FLASH:dr %d: disable or is_enabled failed!\n", id_test);
            return id_test;
        }
        /*fla模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_FLA_LIGHT);
        (void)bsp_hi6551_dr_disable(id_test);
        if(0 != bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_FLA_LIGHT:dr %d: disable or is_enabled failed!\n", id_test);
            return id_test;
        }
        /*bre模式*/
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE);
        (void)bsp_hi6551_dr_disable(id_test);
        if(0 != bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_BRE:dr %d: disable or is_enabled failed!\n", id_test);
            return id_test;
        }
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_LIGHT);
        (void)bsp_hi6551_dr_disable(id_test);
        if(0 != bsp_hi6551_dr_is_enabled(id_test))
        {
            pmic_print_error("PMU_DRS_MODE_BRE_LIGHT:dr %d: disable or is_enabled failed!\n", id_test);
            return id_test;
        }
    }
    pmic_print_info("test ok!!\n");

    return BSP_PMU_OK;
}
int hi6551_dr_onoff_err_test(void)
{
    int iret = 0;
    int result = BSP_PMU_OK;

    /*异常id*/
    iret = bsp_hi6551_dr_enable(PMIC_HI6551_DR_MIN - 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_enable err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_dr_disable(PMIC_HI6551_DR_MAX + 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_disable err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_dr_is_enabled(PMIC_HI6551_DR_MIN - 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_is_enabled err id test error!\n");
        result = BSP_PMU_ERROR;
    }

    return result;
}
extern PMIC_HI6551_DRS_ATTR hi6551_dr_attr[PMIC_HI6551_DR_MAX + 1];
int hi6551_dr_current_test(void)
{
    PMIC_HI6551_DRS_ATTR *hi6551_dr;
    int i,j = 0;
    unsigned selector = 0;
    int dr_current = 0;

    for(i = PMIC_HI6551_DR_MIN;i <= PMIC_HI6551_DR_MAX; i++)
    {
        hi6551_dr = &hi6551_dr_attr[i];
        for(j = 0; j < NUM_OF_DR_CURRENT;j++)
        {
            dr_current = hi6551_dr->current_list[j];
            (void)bsp_hi6551_dr_set_current(i,dr_current,dr_current,&selector);
            if(dr_current != bsp_hi6551_dr_get_current(i))
            {
                pmic_print_error("dr_id %d set or get current error!!\n",i);
                return i;
            }
            if(dr_current != bsp_hi6551_dr_list_current(i,selector))
            {
                pmic_print_error("dr_id %d set or list current error!!\n",selector);
                return i;
            }
        }
    }

    pmic_print_info("test ok!!\n",i);

    return BSP_PMU_OK;
}
extern int bsp_hi6551_dr_set_current_test(int dr_id, int min_uA, int max_uA);
int hi6551_dr_current_err_test(void)
{
    int iret = 0;
    int result = BSP_PMU_OK;

    /*异常id*/
    iret = bsp_hi6551_dr_set_current_test(PMIC_HI6551_DR_MIN - 1,0,0);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_set_current err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_dr_get_current(PMIC_HI6551_DR_MAX + 1);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_get_current err id test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*异常id*/
    iret = bsp_hi6551_dr_list_current(PMIC_HI6551_DR_MIN - 1,0);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_list_current err id test error!\n");
        result = BSP_PMU_ERROR;
    }

    /*无效电流值*/
    iret = bsp_hi6551_dr_set_current_test(2,100000,200000);
    if(BSP_PMU_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_set_current err current test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*无效档位值*/
    iret = bsp_hi6551_dr_list_current(3,500);
    if(BSP_PMU_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_list_current err selector test error!\n");
        result = BSP_PMU_ERROR;
    }

    return result;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_bre_mode_test
 功能描述  : normal模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_dr_bre_mode_test(void)
{
    int id_test = 0;
    int result = BSP_PMU_OK;
    dr_mode_e  mode = PMU_DRS_MODE_BUTTOM;

    for(id_test = PMIC_HI6551_DR_MIN;id_test < PMIC_HI6551_DR03; id_test++)
    {
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_FLASH);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_BRE_FLASH != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_LIGHT);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_BRE_LIGHT != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
    }
    for(id_test = PMIC_HI6551_DR03;id_test <= PMIC_HI6551_DR_MAX; id_test++)
    {
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_BRE != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_FLASH);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_BRE != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_BRE_LIGHT);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_BRE != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
    }

    return result;
}
/*****************************************************************************
 函 数 名  : hi6551_dr_fla_mode_test
 功能描述  : fla模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_dr_fla_mode_test(void)
{
    int id_test = 0;
    int result = BSP_PMU_OK;
    dr_mode_e  mode = PMU_DRS_MODE_BUTTOM;

    for(id_test = PMIC_HI6551_DR_MIN;id_test <= PMIC_HI6551_DR_MAX; id_test++)
    {
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_FLA_FLASH);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_FLA_FLASH != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
        (void)bsp_hi6551_dr_set_mode(id_test,PMU_DRS_MODE_FLA_LIGHT);
        mode = bsp_hi6551_dr_get_mode(id_test);
        if(PMU_DRS_MODE_FLA_LIGHT != mode)
        {
            pmic_print_error("test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
    }
    return result;
}

/*****************************************************************************
 函 数 名  : hi6551_dr_err_mode_test
 功能描述  : 模式错误分支测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6551_dr_mode_err_test(void)
{
    int iret = 0;
    int result = BSP_PMU_OK;
    dr_mode_e  mode = PMU_DRS_MODE_BUTTOM;

    /*模式错误*/
    iret = bsp_hi6551_dr_set_mode(PMIC_HI6551_DR_MIN,PMU_DRS_MODE_BUTTOM);
    if(BSP_PMU_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_set_mode err mode test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*无效id*/
    iret = bsp_hi6551_dr_set_mode(PMIC_HI6551_DR_MAX + 1,PMU_DRS_MODE_BRE);/*lint !e415 !e831*/
    if(BSP_PMU_PARA_ERROR != iret)
    {
        pmic_print_error("bsp_hi6551_dr_set_mode err para test error!\n");
        result = BSP_PMU_ERROR;
    }
    /*无效id*/
    mode = bsp_hi6551_dr_get_mode(PMIC_HI6551_DR_MAX + 1);/*lint !e415 !e831*/
    if(PMU_DRS_MODE_BUTTOM != mode)
    {
        pmic_print_error("bsp_hi6551_dr_get_mode err para test error!\n");
        result = BSP_PMU_ERROR;
    }

    return result;
}
int bsp_hi6551_dr_onoff_test(unsigned test_times)
{
    unsigned fail_times[3] = {0,0,0};
    unsigned test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_dr_enable_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }

        result = hi6551_dr_disable_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }

        result = hi6551_dr_onoff_err_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[2]++;
        }
    }


    if((0 != fail_times[0])||(0 != fail_times[1])||(0 != fail_times[2]))
    {
        pmic_print_error("test %d times ,enable %d times failed,disable %d times failed,err check %d times failed!!\n",test_times,fail_times[0],fail_times[1],fail_times[2]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }
}
int bsp_hi6551_dr_setget_test(unsigned test_times)
{
    unsigned fail_times[2] = {0,0};
    unsigned test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_dr_current_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }
        result = hi6551_dr_current_err_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }
    }

    if((0 != fail_times[0])||(0 != fail_times[1]))
    {
        pmic_print_error("test %d times ,current setget %d times failed,err check %d times failed!!\n",test_times,fail_times[0],fail_times[1]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }

}
int bsp_hi6551_dr_mode_test(unsigned test_times)
{
    unsigned fail_times[3] = {0,0,0};
    unsigned test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6551_dr_bre_mode_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }
        result = hi6551_dr_fla_mode_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }
        result = hi6551_dr_mode_err_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[2]++;
        }
    }

    if((0 != fail_times[0])||(0 != fail_times[1])||(0 != fail_times[2]))
    {
        pmic_print_error("test %d times ,bre mode: %d times failed,fla mode: %d times failed,mode err :%d times failed!!\n",\
                        test_times,fail_times[0],fail_times[1],fail_times[2]);
        return BSP_PMU_ERROR;
    }
    else
    {
        pmic_print_error("test %d times ok!!\n",test_times);
        return BSP_PMU_OK;
    }
}
#endif

/*将测试函数注册进测试框架*/
/*****************************************************************************
 函 数 名  : bsp_hi6551_test_init
 功能描述  : hi6551 test模块的初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_hi6551_test_init(void)
{
    int iret = BSP_PMU_OK;

    struct pmu_test_ops hi6551_test_ops = {
        .pmu_volt_onoff_test = bsp_hi6551_volt_onoff_test,
        .pmu_volt_setget_test = bsp_hi6551_volt_setget_test,
        .pmu_volt_mode_test = bsp_hi6551_volt_mode_test,
        .pmu_volt_table_test = bsp_hi6551_volttable_test,
#ifdef __KERNEL__
        .pmu_dr_onoff_test = bsp_hi6551_dr_onoff_test,
        .pmu_dr_setget_test = bsp_hi6551_dr_setget_test,
        .pmu_dr_mode_test = bsp_hi6551_dr_mode_test,
#endif
    };

    iret = bsp_pmu_test_register(PMIC_HI6551,hi6551_test_ops);

    if(BSP_PMU_OK != iret)
        pmic_print_error("bsp_pmu_test_register error!\n");
    else
        pmic_print_info("bsp_hi6551_test_init ok!\n");

    return iret;
}


