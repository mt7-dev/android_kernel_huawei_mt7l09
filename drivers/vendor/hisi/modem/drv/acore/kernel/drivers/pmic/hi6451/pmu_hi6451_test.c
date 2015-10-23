/*test hi6451 volt enable/isenable*/
/*notice:找能够关闭的电源进行测试*/
/*buck3/4/6不可测试,ldo6/12不可测试*/
#include <osl_bio.h>
#include <osl_thread.h>
#include <pmu_balong.h>

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#define BSP_PMIC_HI6451_TEST_ERROR  -1
#define BSP_PMIC_HI6451_TEST_OK     0
#define PMIC_HI6451_TEST_TASK_PRI                (25)
#define PMIC_HI6451_TEST_TASK_STACK_SIZE    (0x1000)

extern const PMIC_HI6451_VLTGS_ATTR  hi6451_volt_attr[PMIC_HI6451_VOLT_MAX + 1];

/*int hi6451_volt_onoff_untest[]={PMIC_HI6451_BUCK3,PMIC_HI6451_BUCK4,PMIC_HI6451_BUCK6,PMIC_HI6451_LDO6,PMIC_HI6451_LDO12};
int hi6451_volt_set_untest[]={PMIC_HI6451_BUCK3,PMIC_HI6451_BUCK6,PMIC_HI6451_LDO6,PMIC_HI6451_LDO12};*/
int hi6451_volt_onoff_test[]={PMIC_HI6451_BUCK1,PMIC_HI6451_BUCK2,PMIC_HI6451_BUCK5,PMIC_HI6451_LDO1,PMIC_HI6451_LDO2,\
PMIC_HI6451_LDO3,PMIC_HI6451_LDO5,PMIC_HI6451_LDO8,PMIC_HI6451_LDO9,PMIC_HI6451_LDO15};
int hi6451_volt_set_test[]={PMIC_HI6451_BUCK1,PMIC_HI6451_BUCK2,PMIC_HI6451_BUCK5,PMIC_HI6451_LDO1,PMIC_HI6451_LDO2,\
PMIC_HI6451_LDO3,PMIC_HI6451_LDO5,PMIC_HI6451_LDO8,PMIC_HI6451_LDO9,PMIC_HI6451_LDO15};
int hi6451_mode_set_get_test[]={PMIC_HI6451_BUCK1,PMIC_HI6451_BUCK2,PMIC_HI6451_BUCK5,PMIC_HI6451_LDO1,PMIC_HI6451_LDO2,\
    PMIC_HI6451_LDO3,PMIC_HI6451_LDO4,PMIC_HI6451_LDO5,PMIC_HI6451_LDO7,PMIC_HI6451_LDO8,PMIC_HI6451_LDO9,\
    PMIC_HI6451_LDO10,PMIC_HI6451_LDO11,PMIC_HI6451_LDO13,PMIC_HI6451_LDO14,PMIC_HI6451_LDO15};

int hi6451_volt_enable_test(void)
{
    int i = 0;
    int j = 0;
    int test_num = ARRAY_SIZE(hi6451_volt_onoff_test);

    for(j = 0;j < test_num; j++)
    {
        i = hi6451_volt_onoff_test[j];
        bsp_hi6451_volt_enable(i);
        if(0 == bsp_hi6451_volt_is_enabled(i))
        {
            pmic_print_error("\n volt %d: enable or is_enabled failed!\n", i);
            return i;
        }
    }
    pmic_print_info("hi6451_volt_enable_test ok!!\n",i);

    return BSP_PMU_OK;
 }
/*test hi6451 volt disable/isenable*/
int hi6451_volt_disable_test(void)
{
      int i = 0;
    int j = 0;
    int test_num = ARRAY_SIZE(hi6451_volt_onoff_test);

    for(j = 0;j < test_num; j++)
    {
        i = hi6451_volt_onoff_test[j];
        bsp_hi6451_volt_disable(i);
        if(0 != bsp_hi6451_volt_is_enabled(i))
        {
            pmic_print_error("\n volt %d: disable or is_enabled failed!\n", i);
            return i;
        }
    }
    pmic_print_info("hi6451_volt_disable_test ok!!\n",i);

    return BSP_PMU_OK;
}

/*test hi6451 volt set/get voltage*/
/*notice: some volt cannot set too high*/
int hi6451_volt_set_get_voltage_test(void)
{
    unsigned selector = 0;
    int voltage = 0;
    int flag = 0;
    int i,test_sub ,k=0;
    int test_num = ARRAY_SIZE(hi6451_volt_set_test);

    for(test_sub = 0;test_sub < test_num; test_sub++)
    {
        i = hi6451_volt_set_test[test_sub];

        if(hi6451_volt_attr[i].voltage_list != NULL)
        {
            for(k = 0; k < hi6451_volt_attr[i].voltage_nums;k++)
            {
                voltage = hi6451_volt_attr[i].voltage_list[k];
                bsp_hi6451_volt_set_voltage(i,voltage,voltage,&selector);
                if(voltage != bsp_hi6451_volt_get_voltage(i))
                {
                    pmic_print_error("id %d set or get voltage error!!\n",i);
                    return i;
                }
                if(voltage != bsp_hi6451_volt_list_voltage(i,selector))
                {
                    pmic_print_error("id %d set or list voltage error!!\n",selector);
                    return i;
                }
            }
        }
        else
        {
            pmic_print_error("hi6551_volt_attr[%d].voltage_list is NULL,need check!!\n",i);
            return i;
        }
    }

    pmic_print_info("hi6451_volt_set_get_voltage_test ok!!\n",i);

    return BSP_PMU_OK;
}
/*****************************************************************************
 函 数 名  : hi6451_eco_mode_set_get_test
 功能描述  : eco模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6451_eco_mode_set_get_test(void)
{
    int id_test = 0;
    int i,iret = 0;
    int result = BSP_PMU_OK;
    pmu_mode_e  mode = PMU_MODE_BUTTOM;

    for(i = 0;i< ARRAY_SIZE(hi6451_mode_set_get_test);i++)
    {
        id_test = hi6451_mode_set_get_test[i];
        iret = bsp_hi6451_volt_set_mode(id_test,PMU_MODE_ECO);
        mode = bsp_hi6451_volt_get_mode(id_test);
        if(PMU_MODE_ECO != mode)
        {
            pmic_print_error("hi6451_eco_mode_set_get_test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
    }

    return result;
}
/*****************************************************************************
 函 数 名  : hi6451_normal_mode_set_get_test
 功能描述  : normal模式设置测试
 输入参数  : irq
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 备注说明  :
*****************************************************************************/
int hi6451_normal_mode_set_get_test(void)
{
    int id_test = 0;
    int i,iret = 0;
    int result = BSP_PMU_OK;
    pmu_mode_e  mode = PMU_MODE_BUTTOM;

    for(i = 0;i< ARRAY_SIZE(hi6451_mode_set_get_test);i++)
    {
        id_test = hi6451_mode_set_get_test[i];
        iret = bsp_hi6451_volt_set_mode(id_test,PMU_MODE_NORMAL);
        mode = bsp_hi6451_volt_get_mode(id_test);
        if(PMU_MODE_NORMAL != mode)
        {
            pmic_print_error("hi6451_normal_mode_set_get_test error\n");
            result = BSP_PMU_ERROR;
            break;
        }
    }

    return result;

}
/*互斥测试*/
#if 0
/*核内互斥*/
int hi6451_volt_mutex_onecore_test(void)
{
    int i = 0;
    u32 pmu_hi6451_test_pri[30] = {10,12,15,17,19,20,25,28,30,32,34,36,40,42,45,48,50,53,55,58,60,65,72,83,85,87,89,92,95,99};
    u32 task_id[30] = {0};
    u8 name[30][30] = {0};

    for(i = 0;i < 30;i++)
    {
        sprintf((char*)name[i], "pmu_onecore_test_task%d", i);
        if(-1 == osl_task_init((char*)name[i], pmu_hi6451_test_pri[i], PMIC_HI6451_TEST_TASK_STACK_SIZE,
            (void *)hi6451_volt_enable_test, NULL, &task_id[i]))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU,"create test task %d error\n",i);
            return BSP_PMIC_HI6451_TEST_ERROR;
        }
    }
    pmic_print_info("hi6451_volt_mutex_onecore_test ok!!\n",i);
    return 0;
}
/*核间互斥*/
int hi6451_volt_mutex_cores_test(void)
{
#ifdef __KERNEL__
    int i = 0;
    u32 pmu_hi6451_test_pri[30] = {10,12,15,17,19,20,25,28,30,32,34,36,40,42,45,48,50,53,55,58,60,65,72,83,85,87,89,92,95,99};
    u32 task_id[30] = {0};
    u8 name[30][30] = {0};

    for(i = 0;i < 30;i++)
    {
        sprintf((char*)name[i], "pmu_muticores_test_task%d", i);
        if(-1 == osl_task_init((char*)name[i], pmu_hi6451_test_pri[i], PMIC_HI6451_TEST_TASK_STACK_SIZE,
            (void *)hi6451_volt_disable_test, NULL, &task_id[i]))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU,"create test task %d error\n",i);
            return BSP_PMIC_HI6451_TEST_ERROR;
        }
    }
#elif defined(__VXWORKS__)
    int i = 0;
    u32 pmu_hi6451_test_pri[30] = {10,12,15,17,19,20,25,28,30,32,34,36,40,42,45,48,50,53,55,58,60,65,72,83,85,87,89,92,95,99};
    u32 task_id[30] = {0};
    u8 name[30][30] = {0};

    for(i = 0;i < 30;i++)
    {
        sprintf((char*)name[i], "pmu_muticores_test_task%d", i);
        if(-1 == osl_task_init((char*)name[i], pmu_hi6451_test_pri[i], PMIC_HI6451_TEST_TASK_STACK_SIZE,
            (void *)hi6451_volt_enable_test, NULL, &task_id[i]))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU,"create test task %d error\n",i);
            return BSP_PMIC_HI6451_TEST_ERROR;
        }
    }
#endif
    pmic_print_info("hi6451_volt_mutex_cores_test ok!!\n",i);
    return 0;
}
#endif
/*压力测试*/
int bsp_hi6451_volt_onoff_test(u32 test_times)
{
    int fail_times[2] = {0,0};
    int test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6451_volt_enable_test();
        if(BSP_PMU_OK != result)
        {
            pmic_print_error("hi6451_volt_enable_test failed!!\n");
            fail_times[0]++;
        }
        result = hi6451_volt_disable_test();
        if(BSP_PMU_OK != result)
        {
            pmic_print_error("hi6451_volt_disable_test failed!!\n");
            fail_times[1]++;
        }
    }

    pmic_print_error("test %d times ,enable %d times failed,disable %d times failed!!\n",test_times,fail_times[0],fail_times[1]);

    if((0 != fail_times[0])||(0 != fail_times[1]))
        return BSP_PMU_ERROR;
    else
        return BSP_PMU_OK;

}
int bsp_hi6451_volt_setget_test(u32 test_times)
{
    int fail_times = 0;
    int test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6451_volt_set_get_voltage_test();
        if(BSP_PMU_OK != result)
        {
            pmic_print_error("hi6451_volt_set_get_voltage_test failed!!\n");
            fail_times++;
        }
    }

    pmic_print_error("test %d times ,%d times failed!!\n",test_times,fail_times);

    if(0 != fail_times)
        return BSP_PMU_ERROR;
    else
        return BSP_PMU_OK;

}
/*设置eco模式测试*/
int bsp_hi6451_mode_test(u32 test_times)
{
    int fail_times[2] = {0,0,0};
    int test_counts = 0;
    int result = BSP_PMU_OK;

    for(test_counts = 0; test_counts < test_times ; test_counts++)
    {
        result = hi6451_eco_mode_set_get_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[0]++;
        }
        result = hi6451_normal_mode_set_get_test();
        if(BSP_PMU_OK != result)
        {
            fail_times[1]++;
        }
    }

    pmic_print_error("test %d times ,force mode: %d times failed,normal mode: %d times failed,eco mode :%d times failed!!\n",\
                        test_times,fail_times[0],fail_times[1],fail_times[2]);

    if((0 != fail_times[0])||(0 != fail_times[1]))
        return BSP_PMU_ERROR;
    else
        return BSP_PMU_OK;
}
/*将测试函数注册进测试框架*/
/*****************************************************************************
 函 数 名  : bsp_hi6451_test_init
 功能描述  : hi6451 test模块的初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
int bsp_hi6451_test_init(void)
{
    int iret = BSP_PMU_OK;

    struct pmu_test_ops hi6451_test_ops = {
        .pmu_volt_onoff_test = bsp_hi6451_volt_onoff_test,
        .pmu_volt_setget_test = bsp_hi6451_volt_setget_test,
        .pmu_volt_mode_test = bsp_hi6451_mode_test,
    };

    iret = bsp_pmu_test_register(PMIC_HI6451,hi6451_test_ops);

    if(BSP_PMU_OK != iret)
        pmic_print_error("bsp_pmu_dbg_register error!\n");
    else
        pmic_print_error("bsp_hi6451_debug_init ok!\n");

    return iret;
}

#ifdef __cplusplus /* __cplusplus */
}
#endif             /* __cplusplus */


