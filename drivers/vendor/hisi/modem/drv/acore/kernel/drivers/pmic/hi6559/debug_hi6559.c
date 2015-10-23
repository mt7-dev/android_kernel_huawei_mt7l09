
#ifdef __KERNEL__
#include "exc_hi6559.h"
#endif
#include "pmu_balong.h"
#include "common_hi6559.h"

/*电源名称*/
char* hi6559_volt_name[PMIC_HI6559_VOLT_MAX + 1] =
{   
    "BUCK0","BUCK3","BUCK4","BUCK5","BUCK6","LDO01","LDO03",
    "LDO06","LDO07","LDO08","LDO09","LDO10","LDO11","LDO12",
    "LDO13","LDO14","LDO22","LDO23","LDO24","LVS07","LVS09"
};
char* hi6559_mode_name[PMU_MODE_BUTTOM+1] =
{   
    "NORMAL",
    "MODE_ECO",
    "FORCE_ECO",
    "FOLLOW_ECO",
    "NONE_MODE",
    "MODE_INVALID"
};

void bsp_hi6559_volt_show(s32  volt_id);
void bsp_hi6559_volt_state(void);
void bsp_hi6559_version_get(void);
extern s32 hi6559_volt_para_check(s32 volt_id);

#ifdef __KERNEL__
void bsp_hi6559_om_data_show(void);
void bsp_hi6559_om_boot_show(void);
void bsp_hi6559_exc_state(void);extern struct hi6559_reg_cont  reg_np_scp_record1_cont[HI6559_NP_SCP_RECORD1_CONT_NUM]; 
extern struct hi6559_reg_cont  reg_np_ocp_record1_cont[HI6559_NP_OCP_RECORD1_CONT_NUM];
extern struct hi6559_reg_cont  reg_np_ocp_record2_cont[HI6559_NP_OCP_RECORD2_CONT_NUM];
#endif

int bsp_hi6559_volt_set_voltage_test(int volt_id, int min_uV, int max_uV)
{
    unsigned selector = 0;
    int iret = BSP_PMU_OK;

    iret = bsp_hi6559_volt_set_voltage(volt_id,min_uV,max_uV,&selector);
    if(iret)
        return iret;
    else
    {
        pmic_print_info("selector value is %d!!",selector);
        return (int)selector;
    }
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_show
 功能描述  : 显示当前所需路电源的使用信息(包括开关，电压，)
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_volt_show(s32  volt_id)
{
    pmu_mode_e mode;            /* 模式 */
    s32  sw;                    /* 开关状态 */
    s32  voltage;               /* 电压 */
    s32  iret;
    char* onoff = NULL;         /* 开关名称 */
    char* name = NULL;          /* 电压源名称 */
    char* mod_name = NULL;

    iret = hi6559_volt_para_check(volt_id);
    if(iret)
    {
        return;
    }

    sw = bsp_hi6559_volt_is_enabled(volt_id);
    voltage = bsp_hi6559_volt_get_voltage(volt_id);
    mode = bsp_hi6559_volt_get_mode(volt_id);
    name = hi6559_volt_name[volt_id];
    if(!sw)
    {
        onoff = "OFF";
    }
    else
    {
        onoff = "ON";
    }

    mod_name = hi6559_mode_name[mode];

    pmic_print_error("id:%d, name:%s, sw:%s, voltage:%duV, mode:%s\n",volt_id, name, onoff, voltage, mod_name);

    return;
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_volt_state
 功能描述  : 显示当前所有路电源的使用信息(包括开关，电压，)
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_volt_state(void)
{
    s32  i = 0;
    pmic_print_error("***********bsp_hi6559_all_volt_state*****************\n");
    for(i= PMIC_HI6559_VOLT_MIN;i <= PMIC_HI6559_VOLT_MAX;i++)
    {
        bsp_hi6559_volt_show(i);
    }

    return;
}

#ifdef __KERNEL__
/*****************************************************************************
 函 数 名  : bsp_hi6559_om_data_show
 功能描述  : 显示此次运行过程中当前PMU状态的异常信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_om_data_show(void)
{
    s32 i,j = 0;
    
    /* 过温125度 */
    if(pmu_hi6559_om_data[0] & ((u32)0x01 << HI6559_OTMP_125_OFFSET))
    {
        pmic_print_error("pmu_hi6559:temperature over 125℃ flow !\n");
    }

    /* buck短路，寄存器0x18 */
    for (j = 0; j < HI6559_NP_SCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_data[3] & (u8)((u32)0x1 << reg_np_scp_record1_cont[j].bit_ofs))
        {
            pmic_print_error("pmu_hi6559:BUCK %d short !\n",j+1);
        }
    }

    /* buck过流，寄存器0x19 */
    for (j = 0; j < HI6559_NP_OCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_data[4] & (u8)((u32)0x1 << reg_np_ocp_record1_cont[j].bit_ofs))
        {
            pmic_print_error("pmu_hi6559:BUCK %d overflow !\n",j-1);
        }
    }

    /* ldo1~8过流，寄存器0x1A */
    for (j = 0; j < HI6559_NP_OCP_RECORD2_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_data[5] & (u8)((u32)0x1 << reg_np_ocp_record2_cont[j].bit_ofs))
        {
            pmic_print_error("pmu_hi6559:LDO%d overflow !\n" ,reg_np_ocp_record2_cont[j].cont);
        }
    }

    /* ldo9~14过流，寄存器0x1B */
    for (j = 0; j < HI6559_NP_OCP_RECORD3_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_data[6] & (u8)((u32)0x1 << j))
        {
            pmic_print_error("pmu_hi6559:LDO%d overflow !\n" , j + 9);
        }
    }
    
    /* ldo22~24过流，寄存器0x1C */
    for(j = 0; j < HI6559_NP_OCP_RECORD4_CONT_NUM; j++)        
    {
        if (pmu_hi6559_om_data[7] & (u8)((u32)0x1 << (j + 5)))
        {
            pmic_print_error("pmu_hi6559:LDO%d overflow !\n" , j + 22);
        }
    }
    
    /* lvs7过流，寄存器0x1D */
    if (pmu_hi6559_om_data[8] & (u8)((u32)0x1 << HI6559_LVS07_BIT_OFFSET))    
    {
        pmic_print_error("pmu_hi6559: LVS7 overflow last time!\n");
    }
    
    /* lvs9过流，寄存器0x1E */
    if (pmu_hi6559_om_data[9] & (u8)((u32)0x1 << HI6559_LVS09_BIT_OFFSET))         
    {
        pmic_print_error("pmu_hi6559: LVS9 overflow last time!\n");
    } 

    return;
}
/*****************************************************************************
 函 数 名  : bsp_hi6559_boot_om_show
 功能描述  : 显示当前PMU启动时记录的异常信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_hi6559_om_boot_show(void)
{
    s32 j = 0;

    for(j = 0; j < HI6559_NP_RECORD_REG_NUM; j++)
    {
        pmic_print_error("pmu_hi6559:reg_addr: 0x%x, reg_val 0x%x!\n", 0x15 + j, pmu_hi6559_om_boot[j]);
    }

    /* 欠压2.5V */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P5_OFFSET ) )
    {
        pmic_print_error("pmu_hi6559:vsys under 2.5v last time!\n");
    }
    
    /* 欠压2.85V */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P85_OFFSET ) )
    {
        pmic_print_error("pmu_hi6559:vsys under 2.85/3.0v last time!\n");
    }

    /* 过压6.0V */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_OVER_6P0_OFFSET ))
    {
        pmic_print_error("pmu_hi6559:vsys over 6.0v last time!\n");
    }

    /* 过温125度 */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_OTMP_125_OFFSET))
    {
        pmic_print_error("pmu_hi6559:temperature over 125℃ last time!\n");
    }
    
    /* 过温150度 */
    if(pmu_hi6559_om_boot[1] & (0x01 << HI6559_OTMP_150_OFFSET ) )
    {
        pmic_print_error("pmu_hi6559:temperature over 150℃ last time!\n");
    }

    /* buck短路，寄存器0x18 */
    for (j = 0; j < HI6559_NP_SCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[3] & (u8)((u32)0x1 << reg_np_scp_record1_cont[j].bit_ofs))
        {
            pmic_print_error("pmu_hi6559:BUCK %d short !\n",j+1);
        }
    }

    /* buck过流，寄存器0x19 */
    for (j = 0; j < HI6559_NP_OCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[4] & (u8)((u32)0x1 << reg_np_ocp_record1_cont[j].bit_ofs))
        {
            pmic_print_error("pmu_hi6559:BUCK %d overflow !\n",j-1);
        }
    }

    /* ldo1~8过流，寄存器0x1A */
    for (j = 0; j < HI6559_NP_OCP_RECORD2_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[5] & (u8)((u32)0x1 << reg_np_ocp_record2_cont[j].bit_ofs))
        {
            pmic_print_error("pmu_hi6559:LDO%d overflow !\n" ,reg_np_ocp_record2_cont[j].cont);
        }
    }

    /* ldo9~14过流，寄存器0x1B */
    for (j = 0; j < HI6559_NP_OCP_RECORD3_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[6] & (u8)((u32)0x1 << j))
        {
            pmic_print_error("pmu_hi6559:LDO%d overflow !\n" , j + 9);
        }
    }
    
    /* ldo22~24过流，寄存器0x1C */
    for(j = 0; j < HI6559_NP_OCP_RECORD4_CONT_NUM; j++)        
    {
        if (pmu_hi6559_om_boot[7] & (u8)((u32)0x1 << (j + 5)))
        {
            pmic_print_error("pmu_hi6559:LDO%d overflow !\n" , j + 22);
        }
    }
    
    /* lvs7过流，寄存器0x1D */
    if (pmu_hi6559_om_boot[8] & (u8)((u32)0x1 << HI6559_LVS07_BIT_OFFSET))    
    {
        pmic_print_error("pmu_hi6559: LVS7 overflow last time!\n");
    }
    
    /* lvs9过流，寄存器0x1E */
    if (pmu_hi6559_om_boot[9] & (u8)((u32)0x1 << HI6559_LVS09_BIT_OFFSET))         
    {
        pmic_print_error("pmu_hi6559: LVS9 overflow last time!\n");
    } 

    return;
}
/*****************************************************************************
 函 数 名  : bsp_hi6559_exc_state
 功能描述  : 显示当前PMU的异常信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_hi6559_exc_state(void)
{
    bsp_hi6559_om_boot_show();
    bsp_hi6559_om_data_show();

    pmic_print_error("bsp_hi6559_exc_state over!\n");
}
#endif
/*****************************************************************************
 函 数 名  : bsp_hi6559_version_get
 功能描述  : 显示当前PMIC的版本信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_version_get(void)
{
    u8 version = 0;

    version = hi6559_version_get();
    pmic_print_error("PMIC HI6559 version is 0x%x!\n",version);
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_debug_init
 功能描述  : hi6559 debug模块的初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 异常定位
*****************************************************************************/
void bsp_hi6559_debug_init(void)
{
    struct pmu_dbg_ops hi6559_dbg_ops;
    s32  iret = BSP_PMU_OK;

    hi6559_dbg_ops.pmu_volt_show = bsp_hi6559_volt_show;
    hi6559_dbg_ops.pmu_volt_state = bsp_hi6559_volt_state;
    hi6559_dbg_ops.pmu_pmic_info = bsp_hi6559_version_get;
#ifdef __KERNEL__
    hi6559_dbg_ops.pmu_om_data_show = bsp_hi6559_om_data_show;
    hi6559_dbg_ops.pmu_om_boot_show = bsp_hi6559_om_boot_show;
    hi6559_dbg_ops.pmu_exc_state = bsp_hi6559_exc_state;
#endif

    iret = bsp_pmu_dbg_register(PMIC_HI6559, hi6559_dbg_ops);

    if(BSP_PMU_OK != iret)
    {
        pmic_print_error("bsp_pmu_dbg_register error!\n");
    }
    else
    {
        pmic_print_error("bsp_hi6559_debug_init ok!\n");
    }
    
    return;
}


