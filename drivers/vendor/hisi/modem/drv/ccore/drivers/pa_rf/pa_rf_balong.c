
#ifdef __cplusplus
extern "C"
{
#endif
#include <drv_comm.h>
#include <bsp_shared_ddr.h>
#include <bsp_pa_rf.h>
#include <bsp_regulator.h>
#include <bsp_pmu_hi6561.h>
#include <bsp_pmu.h>
#include <bsp_icc.h>

#ifndef OK
#define OK      0
#endif

#ifndef ERROR
#define ERROR   -1
#endif

/* print func */
#define parf_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PARF, "[pa_rf]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define parf_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_PARF, "[pa_rf]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define parf_print_always(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PARF, "[pa_rf]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

#if defined(CONFIG_PASTAR)

#elif defined(CONFIG_PMIC_HI6559)

#define PA_RF_VOLT_ID_MASK  ((1 << PMIC_HI6559_BUCK0) | (1 << PMIC_HI6559_BUCK5) | (1 << PMIC_HI6559_LDO12) |\
                             (1 << PMIC_HI6559_LDO13) | (1 << PMIC_HI6559_LVS07))

typedef void (*pFUNC)(u32);
pFUNC g_pfunc_exc_handle = NULL;  
u32 g_pmu_exc_type = 0;    /* PA/RF供电异常记录，0表示没有异常，非0表示有异常 */


#define PA_RF_VOLT_NUM  MODEM_PA1
struct regulator *regulator_pmu[PA_RF_VOLT_NUM]; /* 给PA/RF供电。0/1给PA用,2/3/4给RF用 */

/*
 * 以下是针对PMU芯片的适配
 */
#if 0
/*****************************************************************************
 函数 	: drv_pa_rf_exc_callback_register
 功能	: 上层提供的回调函数注册
 输入	: 无
 输出	: 无
 返回   : 无
*****************************************************************************/
void drv_pa_rf_exc_callback_register(pFUNC p)
{
    g_pfunc_exc_handle = p;
}
#endif
/*****************************************************************************
 函数 	: parf_exc_handler
 功能	: pa/rf异常处理
 输入	: 
 输出	: 无
 返回   : 无
*****************************************************************************/
s32 parf_exc_handler(u32 channel_id , u32 len, void* context)
{
    s32 ret = ERROR;

    /* 接收ICC消息 */
	ret = bsp_icc_read(PA_RF_ICC_CHN_ID, (u8 *)&g_pmu_exc_type, sizeof(u32));
	if(sizeof(u32) != ret)
	{
        parf_print_error("ERROR: bsp_icc_read failed %d\n", ret);
		return ret;
	}

    /* 若不是pa/rf的电源异常，则直接返回 */
    g_pmu_exc_type = g_pmu_exc_type & PA_RF_VOLT_ID_MASK;
    if(!g_pmu_exc_type)
    {
        return OK;
    }

    parf_print_error("ERROR: PA/RF power error, volt id %d\n", g_pmu_exc_type);

#if 0
    /* 上报异常信息，DSP尚未确认此处如何交互，待确认后修改 */ /* 如果是向HIDS打印信息，需要起一个小任务 */
    if(g_pfunc_exc_handle)
    {
        (*g_pfunc_exc_handle)(g_pmu_exc_type);
    }
#endif
    return OK;
}

/*****************************************************************************
 函 数 名  : bsp_pa_rf_init
 功能描述  : 获取regulator
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0:成功；else - 失败。失败时返回值的bit0~bit4对应获取失败的regulator_pmu id
*****************************************************************************/
s32 bsp_pa_rf_init(void)
{
    s32 i = 0;
    s32 ret = OK;
    
    /* 获取regulator */
    regulator_pmu[MODEM_PA0] = regulator_get(NULL, VOLT_NAME_PA_VCC);                        /* BUCK0 */
    regulator_pmu[MODEM_RFIC0_ANALOG0] = regulator_get(NULL, VOLT_NAME_RFIC_FRONT_VCC);      /* BUCK5 */
    regulator_pmu[MODEM_RFIC0_ANALOG1] = regulator_get(NULL, VOLT_NAME_RFIC_VCC);            /* LVS07 */
    regulator_pmu[MODEM_FEM0] = regulator_get(NULL, VOLT_NAME_RF_FEM_VCC);                   /* LDO13 */
    regulator_pmu[MODEM_PA_VBIAS0] = regulator_get(NULL, VOLT_NAME_PA_VBIA2_VCC);           /* LDO12 */

    for(i = 0; i < PA_RF_VOLT_NUM; i++)
    {
        if(NULL == regulator_pmu[i])
        {
            parf_print_error("ERROR: regulator_pmu%d get regulator failed %d\n", i);
            ret |= (1 << i);
        }
    }

    if(ret != OK)
    {
        return ret;
    }

    /* 异常处理注册*/
    ret = bsp_icc_event_register(PA_RF_ICC_CHN_ID, parf_exc_handler, NULL, NULL, NULL);
	if(ret != OK)
	{
        parf_print_error("ERROR: register icc event failed, ret = 0x%x\n", ret);
        return ret;
	}
    
    parf_print_always("pa_rf init OK\n");
    
    return ret;
}


/*****************************************************************************
 函 数 名  : bsp_pmu_get_pa_powerstatus
 功能描述  : 获取pa上下电状态
 输入参数  : @modem_id - 主卡or副卡(6559只支持单卡，此参数无用)
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
PWC_COMM_STATUS_E bsp_pmu_get_pa_powerstatus(PWC_COMM_MODEM_E modem_id)
{
    s32 status1 = 0, status2 = 0;
    s32 result = 0;

    /* bbstar只支持modem0 */
    if(modem_id != PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem id %d\n", modem_id);
        return PWC_COMM_STATUS_BUTT;
    }

    status1 = regulator_is_enabled(regulator_pmu[MODEM_PA0]);
    status2 = regulator_is_enabled(regulator_pmu[MODEM_PA_VBIAS0]);

	if(status1 && status2)
    {
		result = PWC_COMM_MODEM_ON;
	}
	else if(0 == (status1 | status2))
    {
		result = PWC_COMM_MODEM_OFF;
	}
	else
    {
		result = PWC_COMM_STATUS_BUTT;
	}

	return result;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_pa_poweron
 功能描述  : 给pa上电
 输入参数  : @modem_id - 主卡or副卡(6559只支持单卡，此参数无用)
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_pmu_pa_poweron(PWC_COMM_MODEM_E modem_id)
{
    s32 ret = OK;
    
    /* bbstar只支持modem0 */
    if(modem_id != PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem id %d\n", modem_id);
        return BSP_PMU_ERROR;
    }
    
    /* 若已经打开，则不重复打开 */
    if(PWC_COMM_MODEM_ON == bsp_pmu_get_pa_powerstatus(modem_id))
    {
        return ret;
    }

    ret = regulator_enable(regulator_pmu[MODEM_PA0]);
    if(ret)
    {
        parf_print_error("ERROR: enable regulator_pmu%d failed, ret %d \n", MODEM_PA0, ret);
        return ret;
    }    

    ret = regulator_enable(regulator_pmu[MODEM_PA_VBIAS0]);
    if(ret)
    {
        parf_print_error("ERROR: enable regulator_pmu%d failed, ret %d \n", MODEM_PA_VBIAS0, ret);
        return ret;
    } 

    return ret;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_pa_poweroff
 功能描述  : 给pa下电
 输入参数  : @modem_id - 主卡or副卡(6559只支持单卡，此参数无用)
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_pmu_pa_poweroff(PWC_COMM_MODEM_E modem_id)
{
    s32 ret = OK;

    /* bbstar只支持modem0 */
    if(modem_id != PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem id %d\n", modem_id);
        return BSP_PMU_ERROR;
    }    

    if(PWC_COMM_MODEM_OFF == bsp_pmu_get_pa_powerstatus(modem_id))
    {
        return ret;
    }
    
    ret = regulator_disable(regulator_pmu[MODEM_PA0]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_disable %d failed, ret %d \n", MODEM_PA0, ret);
        return ret;
    }
#if 0
    /* 711 LDO12与USB共用，此处不能关掉 */
    ret = regulator_disable(regulator_pmu[MODEM_PA_VBIAS0]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_disable %d failed, ret %d \n", MODEM_PA_VBIAS0, ret);
        return ret;
    }
#endif
    return ret;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_get_rf_powerstatus
 功能描述  : 获取rf上下电状态
 输入参数  : @modem_id - 主卡or副卡(6559只支持单卡，此参数无用)
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
PWC_COMM_STATUS_E bsp_pmu_get_rf_powerstatus(PWC_COMM_MODEM_E modem_id)
{    
    s32 status1 = 0, status2 = 0, status3 = 0;
    s32 result = 0;
    
    /* bbstar只支持modem0 */
    if(modem_id != PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem id %d\n", modem_id);
        return PWC_COMM_STATUS_BUTT;
    }
    
    status1 = regulator_is_enabled(regulator_pmu[MODEM_RFIC0_ANALOG0]);
    status2 = regulator_is_enabled(regulator_pmu[MODEM_RFIC0_ANALOG1]);
    status3 = regulator_is_enabled(regulator_pmu[MODEM_FEM0]);

	if(status1 && status2 && status3)
    {
		result = PWC_COMM_MODEM_ON;
	}
	else if(0 == (status1 | status2 | status3))
    {
		result = PWC_COMM_MODEM_OFF;
	}
	else
    {
		result = PWC_COMM_STATUS_BUTT;
	}

	return result;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_rf_poweron
 功能描述  : 给rf上电
 输入参数  : @modem_id - 主卡or副卡(6559只支持单卡，此参数无用)
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_pmu_rf_poweron(PWC_COMM_MODEM_E modem_id)
{
    s32 ret = OK;

    /* bbstar只支持modem0 */
    if(modem_id != PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem id %d\n", modem_id);
        return BSP_PMU_ERROR;
    }
    
    if(PWC_COMM_MODEM_ON == bsp_pmu_get_rf_powerstatus(modem_id))
    {
        return ret;
    }
    
    ret = regulator_enable(regulator_pmu[MODEM_RFIC0_ANALOG0]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_enable %d failed, ret %d \n", MODEM_RFIC0_ANALOG0, ret);
        return ret;
    }    

    ret = regulator_enable(regulator_pmu[MODEM_RFIC0_ANALOG1]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_enable %d failed, ret %d \n", MODEM_RFIC0_ANALOG1, ret);
        return ret;
    }  
    
    ret = regulator_enable(regulator_pmu[MODEM_FEM0]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_enable %d failed, ret %d \n", MODEM_FEM0, ret);
        return ret;
    }   

    return ret;
}

/*****************************************************************************
 函 数 名  : bsp_pmu_rf_poweroff
 功能描述  : 给rf下电
 输入参数  : @modem_id - 主卡or副卡(6559只支持单卡，此参数无用)
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_pmu_rf_poweroff(PWC_COMM_MODEM_E modem_id)
{
    s32 ret = OK;

    /* bbstar只支持modem0 */
    if(modem_id != PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem id %d\n", modem_id);
        return BSP_PMU_ERROR;
    }
    
    if(PWC_COMM_MODEM_OFF == bsp_pmu_get_rf_powerstatus(modem_id))
    {
        return ret;
    }
    
    ret = regulator_disable(regulator_pmu[MODEM_RFIC0_ANALOG0]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_disable %d failed, ret %d \n", MODEM_RFIC0_ANALOG0, ret);
        return ret;
    }
    
    ret = regulator_disable(regulator_pmu[MODEM_RFIC0_ANALOG1]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_disable %d failed, ret %d \n", MODEM_RFIC0_ANALOG1, ret);
        return ret;
    }

    ret = regulator_disable(regulator_pmu[MODEM_FEM0]);
    if(ret)
    {
        parf_print_error("ERROR: regulator_disable %d failed, ret %d \n", MODEM_FEM0, ret);
        return ret;
    }    

    return ret;
}

/*****************************************************************************
 函 数 名  : drv_pmu_parf_exc_check
 功能描述  : 通信模块检查PASTAR是否有异常接口
 输入参数  : modem_id       卡号
 输出参数  : 无
 返 回 值  : 0          没有异常
             else       存在异常
*****************************************************************************/
s32 drv_pmu_parf_exc_check(PWC_COMM_MODEM_E modem_id)
{
    /* 参数检查 */
    if(modem_id > PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem_id %d\n", modem_id);
        return ERROR;
    }

    if(!(g_pmu_exc_type & PA_RF_VOLT_ID_MASK))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*****************************************************************************
 函 数 名  : drv_pmu_modem_voltage_set
 功能描述  : 通信模块设置电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      设置的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          设置成功
             else       设置失败
*****************************************************************************/
s32 drv_pmu_modem_voltage_set(EM_MODEM_CONSUMER_ID consumer_id, u32 voltage_mv )
{
    s32 ret = ERROR;
    
    /* 参数检查 */
    if(consumer_id > MODEM_PA_VBIAS0)
    {
        parf_print_error("ERROR: invalid consumer_id %d\n", consumer_id);
        return ERROR;
    }

    ret = regulator_set_voltage(regulator_pmu[consumer_id], voltage_mv * 1000, voltage_mv * 1000);
    if(ret)
    {
        parf_print_error("ERROR: regulator_set_voltage failed, ret = %d\n", ret);
        return ret;
    }

    return OK;
}

/*****************************************************************************
 函 数 名  : drv_pmu_modem_voltage_get
 功能描述  : 通信模块获取电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      获得的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          获取成功
             else       获取失败
*****************************************************************************/
s32 drv_pmu_modem_voltage_get( EM_MODEM_CONSUMER_ID consumer_id, u32 *voltage_mv )
{
    /* 参数检查 */
    if(consumer_id > MODEM_PA_VBIAS0)
    {
        parf_print_error("ERROR: invalid consumer_id %d\n", consumer_id);
        return ERROR;
    }

    if(!voltage_mv)
    {
        parf_print_error("ERROR: null pointer of vlotage_mv\n");
        return ERROR;
    }

    /* 获取电压值，并转换为mv */
    *voltage_mv = (u32)regulator_get_voltage(regulator_pmu[consumer_id]) / 1000;

    return OK;
}

/*****************************************************************************
 函 数 名  : drv_pmu_modem_voltage_list
 功能描述  : 通信模块获取电压设置范围接口
 输入参数  : consumer_id     用户id
             list            电压范围数组
             size            数组大小
 输出参数  : 无
 返 回 值  : 0          获取成功
             else       获取失败
*****************************************************************************/
s32 drv_pmu_modem_voltage_list(EM_MODEM_CONSUMER_ID consumer_id,u16 **list, u32 *size)
{
    /* 上层没有调用，此处直接返回ERROR */
    return ERROR;
}

/*****************************************************************************
 函数	: drv_pmu_modem_apt_enable
 功能	: 通信模块使能APT状态接口
 输入	: modem_id       卡号
          mode_id        模式
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 drv_pmu_modem_apt_enable(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    /* 参数检查 */
    if(modem_id > PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem_id %d\n", modem_id);
        return ERROR;
    }
    
    if(mode_id >= PWC_COMM_MODE_BUTT)
    {
        parf_print_error("ERROR: invalid mode_id %d\n", mode_id);
        return ERROR;
    }

    return bsp_pmu_apt_enable();
}

/*****************************************************************************
 函数	: drv_pmu_modem_apt_disable
 功能	: 通信模块去使能APT状态接口
 输入	: modem_id       卡号
          mode_id        模式
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 drv_pmu_modem_apt_disable(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    /* 参数检查 */
    if(modem_id > PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem_id %d\n", modem_id);
        return ERROR;
    }
    
    if(mode_id >= PWC_COMM_MODE_BUTT)
    {
        parf_print_error("ERROR: invalid mode_id %d\n", mode_id);
        return ERROR;
    }
    
    return bsp_pmu_apt_disable();
}

/*****************************************************************************
 函数	: drv_pmu_modem_apt_status_get
 功能	: 通信模块获取当前APT状态接口
 输入	: modem_id       卡号
          mode_id        模式
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 drv_pmu_modem_apt_status_get(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    /* 参数检查 */
    if(modem_id > PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem_id %d\n", modem_id);
        return ERROR;
    }
    
    if(mode_id >= PWC_COMM_MODE_BUTT)
    {
        parf_print_error("ERROR: invalid mode_id %d\n", mode_id);
        return ERROR;
    }
    
    return bsp_pmu_apt_status_get();
}

/*****************************************************************************
 函数 	: drv_pmu_modem_mode_config
 功能	: 通信模块配置G模或W模接口
 输入	: modem_id       卡号
          mode_id        模式
 输出	: 无
 返回  : 0         配置成功/  else      配置失败
*****************************************************************************/
s32 drv_pmu_modem_mode_config(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    s32 ret = ERROR;
    
    /* 参数检查 */
    if(modem_id > PWC_COMM_MODEM_0)
    {
        parf_print_error("ERROR: invalid modem_id %d\n", modem_id);
        return ERROR;
    }
    
    if(mode_id >= PWC_COMM_MODE_BUTT)
    {
        parf_print_error("ERROR: invalid mode_id %d\n", mode_id);
        return ERROR;
    }
    
	switch(mode_id){
		case PWC_COMM_MODE_LTE :
    	case PWC_COMM_MODE_TDS :
		case PWC_COMM_MODE_WCDMA :
			/* 配置buck pa的电压为3.3v */
			ret|= regulator_set_voltage(regulator_pmu[MODEM_PA0],3300000,3300000);

			/* 关闭APT */
			ret |= bsp_pmu_apt_disable();
			break;
		case PWC_COMM_MODE_GSM:

			/* 配置buck pa的电压为3.7v */
			ret|= regulator_set_voltage(regulator_pmu[MODEM_PA0],3700000,3700000);

			/* 关闭APT */
			ret |= bsp_pmu_apt_disable();
			break;

		default:
			;

	}

	return ret;
}

#else

/*
 * 以下是打桩接口，只有当PASTAR和PMU都不编译时，才会用到这些接口
 */

/*****************************************************************************
 函 数 名  : dummy_modem_exc_check
 功能描述  : 通信模块检查PASTAR是否有异常接口打桩
 输入参数  : modem_id       卡号
 输出参数  : 无
 返 回 值  : 0          没有异常
             else       存在异常
*****************************************************************************/
s32 dummy_modem_exc_check(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_voltage_set
 功能描述  : 通信模块设置电压接口打桩
 输入参数  : consumer_id     用户id
             voltage_mv      设置的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          设置成功
             else       设置失败
*****************************************************************************/
s32 dummy_modem_voltage_set( EM_MODEM_CONSUMER_ID consumer_id, u32 voltage_mv )
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_voltage_get
 功能描述  : 通信模块获取电压接口打桩
 输入参数  : consumer_id     用户id
             voltage_mv      获得的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          获取成功
             else       获取失败
*****************************************************************************/
s32 dummy_modem_voltage_get( EM_MODEM_CONSUMER_ID consumer_id, u32 *voltage_mv )
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_voltage_list
 功能描述  : 通信模块获取电压设置范围接口打桩
 输入参数  : consumer_id     用户id
             list            电压范围数组
             size            数组大小
 输出参数  : 无
 返 回 值  : 0          获取成功
             else       获取失败
*****************************************************************************/
s32 dummy_modem_voltage_list(EM_MODEM_CONSUMER_ID consumer_id,u16 **list, u32 *size)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_apt_enable
 功能描述  : 通信模块使能APT接口打桩
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 0         设置成功
             else      设置失败
*****************************************************************************/
s32 dummy_modem_apt_enable(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_apt_disable
 功能描述  : 通信模块去使能APT接口打桩
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 0         设置成功
             else      设置失败
*****************************************************************************/
s32 dummy_modem_apt_disable(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_apt_status_get
 功能描述  : 通信模块获取当前APT状态接口打桩
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 1     APT使能
             		   0    APT未使能
             		  -1    获取失败
*****************************************************************************/
s32 dummy_modem_apt_status_get(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_modem_mode_config
 功能描述  : 通信模块配置G模或W模接口打桩
 输入参数  : modem_id       卡号
             mode_id        通信模式
 输出参数  : 无
 返 回 值  : 0         配置成功
             else      配置失败
*****************************************************************************/
s32 dummy_modem_mode_config(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
    return (s32)ERROR;
}

/*****************************************************************************
 函 数 名  : dummy_pmu_pa_poweron
 功能描述  : PA 上电打桩
 输入参数  : 无

 输出参数  : 无
 返 回 值  : pa电源开关状态
*****************************************************************************/
int dummy_pmu_pa_poweron(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return OK;
}

/*****************************************************************************
 函 数 名  : dummy_pmu_lpa_poweroff
 功能描述  : PA 下电打桩
 输入参数  : 无

 输出参数  : 无
 返 回 值  : pa 电源开关状态
*****************************************************************************/
int dummy_pmu_pa_poweroff(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return OK;
}

/*****************************************************************************
 函 数 名  : dummy_pmu_rf_poweroff
 功能描述  : RF 下电打桩
 输入参数  : 无

 输出参数  : 无
 返 回 值  : rf电源开关状态
*****************************************************************************/
int dummy_pmu_rf_poweroff(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return OK;
}

/*****************************************************************************
 函数	: dummy_pmu_rf_poweron
 功能	: RF 上电打桩
 输入	: 无
 输出	: 无
 返回 	: rf电源开关状态
*****************************************************************************/
int dummy_pmu_rf_poweron(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return OK;
}

/*****************************************************************************
 函 数 名  : dummy_pmu_get_rf_powerstatus
 功能描述  : RF电源状态查询打桩
 输入参数  : 
 输出参数  :
 返 回 值  : rf电源开关状态
*****************************************************************************/
PWC_COMM_STATUS_E dummy_pmu_get_rf_powerstatus(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return PWC_COMM_MODE_BUTT;
}

/*****************************************************************************
 函 数 名  :dummy_pmu_get_rf_powerstatus
 功能描述  :RF电源状态查询打桩
 输入参数  :
 输出参数  :
 返 回 值  : pa电源开关状态
*****************************************************************************/
PWC_COMM_STATUS_E dummy_pmu_get_pa_powerstatus(PWC_COMM_MODEM_E modem_id)
{
	parf_print_error("ERROR: pa_rf didn't build all function!\n");
	return PWC_COMM_MODE_BUTT;
}

#endif

struct pmu_parf_ops pmu_parf_ops = { 
#if defined(CONFIG_PASTAR)  /* pastar芯片应该走这个分支 */
    .pmu_pa_power_on = bsp_pmu_hi6561_pa_poweron,
    .pmu_pa_power_off = bsp_pmu_hi6561_pa_poweroff,
    .pmu_pa_power_status = bsp_pmu_hi6561_get_pa_powerstatus,
    .pmu_rf_power_on = bsp_pmu_hi6561_rf_poweron,
    .pmu_rf_power_off = bsp_pmu_hi6561_rf_poweroff,
    .pmu_rf_power_status = bsp_pmu_hi6561_get_rf_powerstatus,
    .drv_pastar_exc_check = drv_pmu_hi6561_exc_check,
    .drv_modem_voltage_set = drv_pmu_hi6561_voltage_set,
    .drv_modem_voltage_get = drv_pmu_hi6561_voltage_get,
    .drv_modem_voltage_list = drv_pmu_hi6561_voltage_list,
    .drv_modem_apt_enable = drv_pmu_hi6561_apt_enable,
    .drv_modem_apt_disable = drv_pmu_hi6561_apt_disable,
    .drv_modem_apt_status_get = drv_pmu_hi6561_apt_status_get,
    .drv_modem_mode_config = drv_pmu_hi6561_mode_config,

#elif defined(CONFIG_PMIC_HI6559) /* PMU芯片应该走这个分支 */
    .pmu_pa_power_on = bsp_pmu_pa_poweron,
    .pmu_pa_power_off = bsp_pmu_pa_poweroff,
    .pmu_pa_power_status = bsp_pmu_get_pa_powerstatus,
    .pmu_rf_power_on = bsp_pmu_rf_poweron,
    .pmu_rf_power_off = bsp_pmu_rf_poweroff,
    .pmu_rf_power_status = bsp_pmu_get_rf_powerstatus,
    .drv_pastar_exc_check = drv_pmu_parf_exc_check,
    .drv_modem_voltage_set = drv_pmu_modem_voltage_set,
    .drv_modem_voltage_get = drv_pmu_modem_voltage_get,
    .drv_modem_voltage_list = drv_pmu_modem_voltage_list,
    .drv_modem_apt_enable = drv_pmu_modem_apt_enable,
    .drv_modem_apt_disable = drv_pmu_modem_apt_disable,
    .drv_modem_apt_status_get = drv_pmu_modem_apt_status_get,
    .drv_modem_mode_config = drv_pmu_modem_mode_config,

#else /* 没有pastar也没有pmu，才走这个分支 */
    .pmu_pa_power_on = dummy_pmu_pa_poweron,
    .pmu_pa_power_off = dummy_pmu_pa_poweroff,
    .pmu_pa_power_status = dummy_pmu_get_pa_powerstatus,
    .pmu_rf_power_on = dummy_pmu_rf_poweron,
    .pmu_rf_power_off = dummy_pmu_rf_poweroff,
    .pmu_rf_power_status = dummy_pmu_get_rf_powerstatus,
    .drv_pastar_exc_check = dummy_modem_exc_check,
    .drv_modem_voltage_set = dummy_modem_voltage_set,
    .drv_modem_voltage_get = dummy_modem_voltage_get,
    .drv_modem_voltage_list = dummy_modem_voltage_list,
    .drv_modem_apt_enable = dummy_modem_apt_enable,
    .drv_modem_apt_disable = dummy_modem_apt_disable,
    .drv_modem_apt_status_get = dummy_modem_apt_status_get,
    .drv_modem_mode_config = dummy_modem_mode_config,

#endif
};

/*
 * 以下是对外提供的接口，包括DRV接口和dpm流程接口
 */

/*****************************************************************************
 函 数 名  : DRV_PASTAR_EXC_CHECK
 功能描述  : 通信模块检查PASTAR是否有异常接口
 输入参数  : modem_id       卡号
 输出参数  : 无
 返 回 值  : 0          没有异常
             else       存在异常
*****************************************************************************/
s32 DRV_PASTAR_EXC_CHECK(PWC_COMM_MODEM_E modem_id)
{
    return pmu_parf_ops.drv_pastar_exc_check(modem_id);
}

/*****************************************************************************
 函 数 名  : DRV_MODEM_VOLTAGE_SET
 功能描述  : 通信模块设置电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      设置的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          设置成功
             else       设置失败
*****************************************************************************/
s32 DRV_MODEM_VOLTAGE_SET( EM_MODEM_CONSUMER_ID consumer_id, u32 voltage_mv )
{
    return pmu_parf_ops.drv_modem_voltage_set(consumer_id,voltage_mv);
}

/*****************************************************************************
 函 数 名  : DRV_MODEM_VOLTAGE_GET
 功能描述  : 通信模块获取电压接口
 输入参数  : consumer_id     用户id
             voltage_mv      获得的电压值毫伏
 输出参数  : 无
 返 回 值  : 0          获取成功
             else       获取失败
*****************************************************************************/
s32 DRV_MODEM_VOLTAGE_GET( EM_MODEM_CONSUMER_ID consumer_id, u32 *voltage_mv )
{
    return pmu_parf_ops.drv_modem_voltage_get(consumer_id, voltage_mv);
}

/*****************************************************************************
 函 数 名  : DRV_MODEM_VOLTAGE_LIST
 功能描述  : 通信模块获取电压设置范围接口
 输入参数  : consumer_id     用户id
             list            电压范围数组
             size            数组大小
 输出参数  : 无
 返 回 值  : 0          获取成功
             else       获取失败
*****************************************************************************/
s32 DRV_MODEM_VOLTAGE_LIST(EM_MODEM_CONSUMER_ID consumer_id,u16 **list, u32 *size)
{
    return pmu_parf_ops.drv_modem_voltage_list(consumer_id, list, size);
}

/*****************************************************************************
 函数	: DRV_MODEM_APT_STATUS_GET
 功能	: 通信模块使能APT状态接口
 输入	: modem_id       卡号
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 DRV_MODEM_APT_ENABLE(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    return pmu_parf_ops.drv_modem_apt_enable(modem_id, mode_id);
}

/*****************************************************************************
 函数	: DRV_MODEM_APT_STATUS_GET
 功能	: 通信模块去使能APT状态接口
 输入	: modem_id       卡号
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 DRV_MODEM_APT_DISABLE(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    return pmu_parf_ops.drv_modem_apt_disable(modem_id, mode_id);
}

/*****************************************************************************
 函数	: DRV_MODEM_APT_STATUS_GET
 功能	: 通信模块获取当前APT状态接口
 输入	: modem_id       卡号
 输出	: 无
 返回	: 1  APT使能/   0    APT未使能/  -1    获取失败
*****************************************************************************/
s32 DRV_MODEM_APT_STATUS_GET(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    return pmu_parf_ops.drv_modem_apt_status_get(modem_id, modem_id);
}

/*****************************************************************************
 函数 	: bsp_pmu_hi6561_pa_poweron
 功能	: 通信模块配置G模或W模接口
 输入	: modem_id       卡号
 输出	: 无
 返回  : 0         配置成功/  else      配置失败
*****************************************************************************/
s32 DRV_MODEM_MODE_CONFIG(PWC_COMM_MODEM_E modem_id, PWC_COMM_MODE_E mode_id)
{
    return pmu_parf_ops.drv_modem_mode_config(modem_id, mode_id);
}

/*****************************************************************************
 函 数 名  : bsp_pa_poweron
 功能描述  : 给pa上电
 输入参数  : @modem_id - 0:主卡 1:副卡
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_pa_poweron(PWC_COMM_MODEM_E modem_id)
{
    return  pmu_parf_ops.pmu_pa_power_on(modem_id); 
}

/*****************************************************************************
 函 数 名  : bsp_pa_poweroff
 功能描述  : 给pa下电
 输入参数  : @modem_id - 0:主卡 1:副卡
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_pa_poweroff(PWC_COMM_MODEM_E modem_id)
{
    return  pmu_parf_ops.pmu_pa_power_off(modem_id); 
}

/*****************************************************************************
 函 数 名  : bsp_get_pa_powerstatus
 功能描述  : 获取pa上下电状态
 输入参数  : @modem_id - 0:主卡 1:副卡
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
PWC_COMM_STATUS_E bsp_get_pa_powerstatus(PWC_COMM_MODEM_E modem_id)
{
    return  pmu_parf_ops.pmu_pa_power_status(modem_id); 
}

/*****************************************************************************
 函 数 名  : bsp_rf_poweron
 功能描述  : 给rf上电
 输入参数  : @modem_id - 0:主卡 1:副卡
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_rf_poweron(PWC_COMM_MODEM_E modem_id)
{
    return  pmu_parf_ops.pmu_rf_power_on(modem_id); 
}

/*****************************************************************************
 函 数 名  : bsp_rf_poweroff
 功能描述  : 给rf下电
 输入参数  : @modem_id - 0:主卡 1:副卡
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
s32 bsp_rf_poweroff(PWC_COMM_MODEM_E modem_id)
{
    return  pmu_parf_ops.pmu_rf_power_off(modem_id); 
}

/*****************************************************************************
 函 数 名  : bsp_get_rf_powerstatus
 功能描述  : 获取rf上下电状态
 输入参数  : @modem_id - 0:主卡 1:副卡
 输出参数  : 无
 返 回 值  : 0 - 成功，非0 - 失败
*****************************************************************************/
PWC_COMM_STATUS_E bsp_get_rf_powerstatus(PWC_COMM_MODEM_E modem_id)
{
    return  pmu_parf_ops.pmu_rf_power_status(modem_id); 
}

#ifdef __cplusplus
}
#endif
