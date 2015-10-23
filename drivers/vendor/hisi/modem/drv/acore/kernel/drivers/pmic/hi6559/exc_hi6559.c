/*
 * pmu_hi6559.c -- simple synchronous userspace interface to SSI devices
 *
 * Copyright (C) 2006 SWAPP
 *  Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*lint --e{537,958}*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <drv_comm.h>
#include <asm/uaccess.h>
//#include <linux/workqueue.h>
#include <osl_bio.h>
#include <bsp_ipc.h>
#include <hi_bbstar.h>
#include <bsp_memmap.h>
//#include <bsp_om.h>
#include <bsp_dump.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>
#include <bsp_pa_rf.h>
#include <bsp_icc.h>
#include <bsp_shared_ddr.h>
#include <osl_thread.h>
#include <osl_sem.h>
#include <osl_spinlock.h>

#include <pmu_balong.h>
#include <irq_hi6559.h>
#include <common_hi6559.h>
#include "exc_hi6559.h"

/* 异常状态记录数组，保存0x15~0x0\过温、欠压过压; 1 \过流; 2\rtc; 3\buck短路; 4~11\过流 */
u8 pmu_hi6559_om_data[HI6559_NP_RECORD_REG_NUM] = {0};    /* 当前状态 *//* 中断寄存器,待思考 */
u8 pmu_hi6559_om_boot[HI6559_NP_RECORD_REG_NUM] = {0};    /* 上次状态遗留 *//* 非下电中断reg */

/* reg 0x18 */ 
struct hi6559_reg_cont  reg_np_scp_record1_cont[HI6559_NP_SCP_RECORD1_CONT_NUM] = 
{
    {2, PMIC_HI6559_BUCK3, "BUCK3"}, 
    {3, PMIC_HI6559_BUCK4, "BUCK4"}, 
    {5, PMIC_HI6559_BUCK6, "BUCK6"}
};

/* reg 0x19 */ 
struct hi6559_reg_cont  reg_np_ocp_record1_cont[HI6559_NP_OCP_RECORD1_CONT_NUM] = 
{
    {3, PMIC_HI6559_BUCK0, "BUCK0"},
    {4, PMIC_HI6559_BUCK3, "BUCK3"},
    {5, PMIC_HI6559_BUCK4, "BUCK4"},
    {6, PMIC_HI6559_BUCK5, "BUCK5"},
    {7, PMIC_HI6559_BUCK6, "BUCK6"}
};

/* reg 0x1A */
struct hi6559_reg_cont  reg_np_ocp_record2_cont[HI6559_NP_OCP_RECORD2_CONT_NUM] = 
{
    {0, PMIC_HI6559_LDO01, "LDO01"},
    {2, PMIC_HI6559_LDO03, "LDO03"},
    {5, PMIC_HI6559_LDO06, "LDO06"},
    {6, PMIC_HI6559_LDO07, "LDO07"},
    {7, PMIC_HI6559_LDO08, "LDO08"}
};

static bool pmu_om_flag = false;    /* 异常状态是否已经记录 */

PMU_EXC_PRO_NV_STRU pmu_exc_pro; /* nv配置 */

struct hi6559_exc_data g_hi6559_exc_st;

/*****************************************************************************
 函 数 名  : hi6559_om_wk_handler
 功能描述  : 将非下电状态记录寄存器写入log文件，并判断此次启动是否由PMU引起的重启
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 备注说明  : 无
*****************************************************************************/
void hi6559_om_wk_handler(void)
{
    s32 i, j = 0;

    /* 保存非下电寄存器值 */
    for(j = 0; j < HI6559_NP_RECORD_REG_NUM; j++)
    {
        print2file(PMU_OM_LOG,"pmu_hi6559:np%d:%x!\n", j + 1, pmu_hi6559_om_boot[j]);
    }

    /* 欠压2.5V */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P5_OFFSET ) )    
    {
        print2file(PMU_OM_LOG,"pmu_hi6559:vsys under 2.5v last time!\n");
    }
    
    /* 欠压2.85/3.0V */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P85_OFFSET ) )   
    {
        print2file(PMU_OM_LOG,"pmu_hi6559:vsys under 2.85/3.0v last time!\n");
    }    
    
    /* 过压6.0V */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_OVER_6P0_OFFSET ))      
    {
        print2file(PMU_OM_LOG,"pmu_hi6559:vsys over 6.0v last time!\n");
    }    
    
    /* 过温125度 */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_OTMP_125_OFFSET))            
    {
        print2file(PMU_OM_LOG,"pmu_hi6559:temperature over 125℃ last time!\n");
    }  
    
    /* 过温150度 */
    if(pmu_hi6559_om_boot[1] & (0x01 << HI6559_OTMP_150_OFFSET ) )          
    {
        print2file(PMU_OM_LOG,"pmu_hi6559:temperature over 150℃ last time!\n");
    }    
    
    /* buck短路，寄存器0x18 */
    for (j = 0; j < HI6559_NP_SCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[3] & (u8)((u32)0x1 << reg_np_scp_record1_cont[j].bit_ofs))
        {
            print2file(PMU_OM_LOG,"pmu_hi6559: %s short last time!\n", reg_np_scp_record1_cont[j].cont);
        }
    }
    
    /* buck过流，寄存器0x19 */
    for (j = 0; j < HI6559_NP_OCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[4] & (u8)((u32)0x1 << reg_np_ocp_record1_cont[j].bit_ofs))
        {
            print2file(PMU_OM_LOG,"pmu_hi6559: %s overflow last time!\n", reg_np_ocp_record1_cont[j].cont);
        }
    }
    
    /* ldo1~8过流，寄存器0x1A */
    for (j = 0; j < HI6559_NP_OCP_RECORD2_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[5] & (u8)((u32)0x1 << reg_np_ocp_record2_cont[j].bit_ofs))
        {
            print2file(PMU_OM_LOG,"pmu_hi6559: %s overflow last time!\n", reg_np_ocp_record2_cont[j].cont);
        }
    }
    
    /* ldo9~14过流，寄存器0x1B */   
    for (j = 0; j < HI6559_NP_OCP_RECORD3_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[6] & (u8)((u32)0x1 << j))
        {
            print2file(PMU_OM_LOG,"pmu_hi6559: LDO%d overflow last time!\n", j + 9);
        }
    }
    
    /* ldo22~24过流，寄存器0x1C */
    for(j = 0; j < HI6559_NP_OCP_RECORD4_CONT_NUM; j++)        
    {
        if (pmu_hi6559_om_boot[7] & (u8)((u32)0x1 << j + 5))
        {
            print2file(PMU_OM_LOG,"pmu_hi6559: LDO%d overflow last time!\n", j + 22);
        }
    }
    
    /* lvs7过流，寄存器0x1D */
    if (pmu_hi6559_om_boot[8] & (u8)((u32)0x1 << HI6559_LVS07_BIT_OFFSET))    
    {
        print2file(PMU_OM_LOG,"pmu_hi6559: LVS7 overflow last time!\n");
    }
    
    /* lvs9过流，寄存器0x1E */
    if (pmu_hi6559_om_boot[9] & (u8)((u32)0x1 << HI6559_LVS09_BIT_OFFSET))         
    {
        print2file(PMU_OM_LOG,"pmu_hi6559: LVS9 overflow last time!\n");
    }    

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_boot_om_log_save
 功能描述  : 将非下电状态记录寄存器保存到全局变量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 备注说明  : 无
*****************************************************************************/
void hi6559_boot_om_log_save(void)
{
    u32 timeout = 0;
    u16 j = 0;

    if(true != pmu_om_flag)
    {
        for(j = 0; j < HI6559_NP_RECORD_REG_NUM; j++)
        {
            bsp_hi6559_reg_read((HI6559_NP_IRQ1_RECORD_OFFSET + j), &pmu_hi6559_om_boot[j]);
        }
        pmu_om_flag = true;
    }

    /* 记录重启后，pmu的最初状态，分析重启原因,记录日志时间长，采用工作队列完成 */
    g_hi6559_exc_st.hi6559_om_wq = create_singlethread_workqueue("bsp_hi6559_om");
    if (!g_hi6559_exc_st.hi6559_om_wq)
    {
        pmic_print_error("%s: create_singlethread_workqueue fail\n", __FUNCTION__);
        return;
    }
    
    INIT_DELAYED_WORK(&g_hi6559_exc_st.hi6559_om_wk, (void *)hi6559_om_wk_handler);
    queue_delayed_work(g_hi6559_exc_st.hi6559_om_wq, &g_hi6559_exc_st.hi6559_om_wk, timeout);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_om_log_save
 功能描述  : 记录hi6559的异常信息
 输入参数  : @hi6559_om_log_flag Log类型
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_om_log_save(pmu_om_log_e hi6559_om_log_flag)
{
    switch(hi6559_om_log_flag)
    {
        case PMU_OM_LOG_START:
            print2file(PMU_OM_LOG, "system start....\n");
            break;
        case PMU_OM_LOG_EXC:
            break;
        default:
            break;
    }

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_get_boot_state
 功能描述  : 系统启动时检查pmu寄存器状态，确认是否是由pmu引起的重启
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : 0 - 非PMU引起的重启， else - PMU引起的重启原因
*****************************************************************************/
s32 hi6559_get_boot_state(void)
{
    s32 iret = PMU_STATE_OK;
    u16 j = 0;

    if(true != pmu_om_flag) /* 如果没有读取过非下电寄存器，还没清除，则读取到全局变量里 */
    {
        for(j = 0; j < HI6559_NP_RECORD_REG_NUM; j++)
        {
            bsp_hi6559_reg_read((HI6559_NP_IRQ1_RECORD_OFFSET + j), &pmu_hi6559_om_boot[j]);
        }
        pmu_om_flag = true;
    }
    
    /* 欠压 */
    if((pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P5_OFFSET ))||
       (pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_UNDER_2P85_OFFSET )))
    {
        iret|= PMU_STATE_UNDER_VOL;
    }

    /* 过压 */
    if(pmu_hi6559_om_boot[0] & (0x01 << HI6559_VSYS_OVER_6P0_OFFSET ))
    {
        iret|=  PMU_STATE_OVER_VOL;
    }
    
    /* 过温 */
    if(pmu_hi6559_om_boot[1] & (0x01 << HI6559_OTMP_150_OFFSET ) )
    {
        iret|=  PMU_STATE_OVER_TEMP;
    }
    
    /* buck短路，寄存器0x18 */
    for (j = 0; j < HI6559_NP_SCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[3] & (u8)((u32)0x1 << reg_np_scp_record1_cont[j].bit_ofs))
        {
            iret |=  PMU_STATE_OVER_CUR;        
        }
    }
    
    /* buck过流，寄存器0x19 */
    for (j = 0; j < HI6559_NP_OCP_RECORD1_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[4] & (u8)((u32)0x1 << reg_np_ocp_record1_cont[j].bit_ofs))
        {
            iret |=  PMU_STATE_OVER_CUR;        
        }
    }
    
    /* ldo1~8过流，寄存器0x1A */
    for (j = 0; j < HI6559_NP_OCP_RECORD2_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[5] & (u8)((u32)0x1 << reg_np_ocp_record2_cont[j].bit_ofs))
        {
            iret |=  PMU_STATE_OVER_CUR;        
        }
    }
    
    /* ldo9~14过流，寄存器0x1B */
    for (j = 0; j < HI6559_NP_OCP_RECORD3_CONT_NUM; j++)       
    {
        if (pmu_hi6559_om_boot[6] & (u8)((u32)0x1 << j))
        {
            iret |=  PMU_STATE_OVER_CUR;        
        }
    }
    
    /* ldo22~24过流，寄存器0x1C */
    for(j = 0; j < HI6559_NP_OCP_RECORD4_CONT_NUM; j++)        
    {
        if (pmu_hi6559_om_boot[7] & (u8)((u32)0x1 << j + 5))
        {
            iret |=  PMU_STATE_OVER_CUR;        
        }
    }
    
    /* lvs7过流，寄存器0x1D */
    if (pmu_hi6559_om_boot[8] & (u8)((u32)0x1 << HI6559_LVS07_BIT_OFFSET))    
    {
        iret|=  PMU_STATE_OVER_CUR;        
    }
    
    /* lvs9过流，寄存器0x1E */
    if (pmu_hi6559_om_boot[9] & (u8)((u32)0x1 << HI6559_LVS09_BIT_OFFSET))         
    {
        iret|=  PMU_STATE_OVER_CUR;        
    }   
    
    return iret;
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_otp_threshold_set
 功能描述  : 系统启动时设置过温警告阈值
 输入参数  : @threshold 要设置的温度(摄氏度)
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_otp_threshold_set(s32 threshold)
{
    hi6559_excflags_t exc_flags = 0;
    
    /*lint --e{746,718}*/
    spin_lock_irqsave(&g_hi6559_exc_st.lock,exc_flags);
    
    switch(threshold)
    {
        case 105:
            bsp_hi6559_reg_write_mask(HI6559_BANDGAP_THSD_ADJ2_OFFSET, 0x00,0x30);
            break;
        case 115:
            bsp_hi6559_reg_write_mask(HI6559_BANDGAP_THSD_ADJ2_OFFSET, 0x10,0x30);
            break;
        case 125:
            bsp_hi6559_reg_write_mask(HI6559_BANDGAP_THSD_ADJ2_OFFSET, 0x20,0x30);;
            break;
        case 135:
            bsp_hi6559_reg_write_mask(HI6559_BANDGAP_THSD_ADJ2_OFFSET, 0x30,0x30);;
            break;
        default:    /* 不处理，采用寄存器默认值125°*/
            break;            
    }
    
    spin_unlock_irqrestore(&g_hi6559_exc_st.lock,exc_flags);

    return;
}

/*****************************************************************************
 函 数 名  : bsp_hi6559_uvp_threshold_set
 功能描述  : 系统启动时设置欠压警告阈值
 输入参数  : @threshold 要设置的电压(mV)
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void bsp_hi6559_uvp_threshold_set(s32 threshold)
{
    hi6559_excflags_t exc_flags = 0;

    spin_lock_irqsave(&g_hi6559_exc_st.lock, exc_flags);
    switch(threshold)
    {
        case 3000:
            bsp_hi6559_reg_write_mask(HI6559_NP_REG_ADJ_OFFSET, 0x10,0x10);
            break;
        case 2850:
            bsp_hi6559_reg_write_mask(HI6559_NP_REG_ADJ_OFFSET, 0x00,0x10);
            break;
        default:            /* 不处理，采用默认值2.85V */
            break;
    }
    spin_unlock_irqrestore(&g_hi6559_exc_st.lock, exc_flags);

    return;
}

/*****************************************************************************
 函 数 名  : hi6559_ocp_scp_handle
 功能描述  : 过流处理函数
 输入参数  : @para 参数指针
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_ocp_scp_handle(void* para)
{
    u32 volt_need_off = 0;
    u32 *pmu_ocp_flag = (u32 *)SHM_PMU_OCP_INFO_ADDR;   /* 过流，需要关闭的过流源 */
    s32 ret = ERROR;
    u8 a_ucRecordReg[HI6559_NP_OCP_SCP_REG_NUM] = {0};  /* 过流寄存器读取值 */
    u8 i = 0;
    u8 j = 0;
    
    /*lint --e{690,831}*/
    para = para;

    pmic_print_error("******* hi6559_current_overflow! **********\n");

    for(i = 0; i < HI6559_NP_OCP_SCP_REG_NUM; i++)
    {
        bsp_hi6559_reg_read((HI6559_NP_SCP_RECORD1_OFFSET + i), &a_ucRecordReg[i]);
        
        /* 在中断处理任务中，是否需要加锁? */
        pmu_hi6559_om_data[i + 3] |= a_ucRecordReg[i];
    }
    
    /* 检查所有SCP_RECORD和OCP_RECORD中的状态, 根据nv配置进行安全处理 */
    /* BUCK短路检查，0x18寄存器 */
    for (j = 0; j < HI6559_NP_SCP_RECORD1_CONT_NUM; j++)
    {
         if (a_ucRecordReg[0] & (u8)((u32)0x1 << reg_np_scp_record1_cont[j].bit_ofs))  /*lint !e690,!e831*/
         {
            volt_need_off |= (0x1 << reg_np_scp_record1_cont[j].volt_id);
            *pmu_ocp_flag |= (0x1 << reg_np_scp_record1_cont[j].volt_id);

            print2file(PMU_OM_LOG,"pmu_hi6559:BUCK %d short !\n", reg_np_scp_record1_cont[j].volt_id);
         }
    }
    /* BUCK过流检查，0x19寄存器 */
    for (j = 0; j < HI6559_NP_OCP_RECORD1_CONT_NUM; j++)
    {
         if (a_ucRecordReg[1] & (u8)((u32)0x1 << reg_np_ocp_record1_cont[j].bit_ofs))  /*lint !e690,!e831*/
         {
            volt_need_off |= (0x1 << reg_np_ocp_record1_cont[j].volt_id);
            *pmu_ocp_flag |= (0x1 << reg_np_ocp_record1_cont[j].volt_id);
            print2file(PMU_OM_LOG,"pmu_hi6559:BUCK %d overflow !\n", reg_np_ocp_record1_cont[j].volt_id);
         }
    }
    
    /* LDO1~8过流检查，0x1A寄存器 */
    for (j = 0; j < HI6559_NP_OCP_RECORD2_CONT_NUM; j++)
    {
         if (a_ucRecordReg[2] & (u8)((u32)0x1 << reg_np_ocp_record2_cont[j].bit_ofs))  /*lint !e690,!e831*/
         {
            volt_need_off |= (0x1 << reg_np_ocp_record2_cont[j].volt_id);
            *pmu_ocp_flag |= (0x1 << reg_np_ocp_record2_cont[j].volt_id);
            print2file(PMU_OM_LOG,"pmu_hi6559:LDO %d overflow !\n", reg_np_ocp_record2_cont[j].volt_id);
         }
    }    

    /* ldo9~14过流，寄存器0x1B */
    for (j = 0; j < HI6559_NP_OCP_RECORD3_CONT_NUM; j++)       
    {
         if (a_ucRecordReg[3] & (u8)((u32)0x1 << j))    /*lint !e690,!e831*/
         {
            volt_need_off |= (0x1 << PMIC_HI6559_LDO09 + j);
            *pmu_ocp_flag |= (0x1 << PMIC_HI6559_LDO09 + j);
            print2file(PMU_OM_LOG,"pmu_hi6559:LDO %d overflow !\n", PMIC_HI6559_LDO09 + j);
         }
    }
    
    /* ldo22~24过流，寄存器0x1C */
    for(j = 0; j < HI6559_NP_OCP_RECORD4_CONT_NUM; j++)        
    {
         if (a_ucRecordReg[4] & (u8)((u32)0x1 << j + 5))    /*lint !e690,!e831*/
         {
            volt_need_off |= (0x1 << PMIC_HI6559_LDO22 + j);
            *pmu_ocp_flag |= (0x1 << PMIC_HI6559_LDO22 + j);
            print2file(PMU_OM_LOG,"pmu_hi6559:LDO %d overflow !\n", PMIC_HI6559_LDO22 + j);
         }
    }
    
    /* lvs7过流，寄存器0x1D */
    if(a_ucRecordReg[5] & (u8)((u32)0x1 << HI6559_LVS07_BIT_OFFSET))    
    {
        volt_need_off |= (0x1 << PMIC_HI6559_LVS07);
        *pmu_ocp_flag |= (0x1 << PMIC_HI6559_LVS07);
        print2file(PMU_OM_LOG,"pmu_hi6559:LDO %d overflow !\n", PMIC_HI6559_LVS07);
    }
    
    /* lvs9过流，寄存器0x1E */
    if(a_ucRecordReg[6] & (u8)((u32)0x1 << HI6559_LVS09_BIT_OFFSET))         
    {
        volt_need_off |= (0x1 << PMIC_HI6559_LVS09);
        *pmu_ocp_flag |= (0x1 << PMIC_HI6559_LVS09);
        print2file(PMU_OM_LOG,"pmu_hi6559:LDO %d overflow !\n", PMIC_HI6559_LVS09);
    } 

    /* 异常信息发给C核 */
    if(volt_need_off)
    {
        ret = bsp_icc_send(ICC_CPU_MODEM, PA_RF_ICC_CHN_ID, &volt_need_off, sizeof(u32));
        if(sizeof(u32) != ret)
        {
            pmic_print_error("bsp_icc_send failed, ret = %d!\n", ret);
        }
    }
    
    /* 根据NV值确认是否需要关闭过流的电源 */
    for(j = PMIC_HI6559_VOLT_MIN; j <= PMIC_HI6559_VOLT_MAX; j++)
    {
        if(pmu_exc_pro.ulOcpIsOff)
        {
            if(volt_need_off & (0x1 << j))
            {
                pmic_print_error("hi6559:volt id %d overflow !\n", j);

                /* 需要关闭过流的电压源，则关闭 */
                if(pmu_exc_pro.VoltProConfig[j].VoltOcpIsOff)
                {
                    pmic_print_error("hi6559:volt id %d will be closed !\n", j);
                    bsp_hi6559_volt_disable(j);
                }

                /* 过流需要重启，则重启 */
                if(pmu_exc_pro.VoltProConfig[j].VoltOcpIsRst)
                {
                    pmic_print_error("hi6559:system will be restart!\n");
                    system_error(DRV_ERRNO_PMU_OVER_CUR, PMU_STATE_OVER_CUR, j, (char *)&volt_need_off, (u32)sizeof(volt_need_off));
                }
            }
        }
        else
        {
            if(volt_need_off & (0x1 << j))
            {
                pmic_print_error("hi6559:volt id %d overflow !\n", j);
            }
        }
    }

    /*清除非下电中断状态寄存器*/
    for(i = 0; i < HI6559_NP_OCP_SCP_REG_NUM; i++)
    {
        bsp_hi6559_reg_write((HI6559_NP_SCP_RECORD1_OFFSET + i), a_ucRecordReg[i]); /*lint !e661*/
    }

    return;
}/*lint !e438*/

/*****************************************************************************
 函 数 名  : hi6559_otp_handle
 功能描述  : 过温处理函数
 输入参数  : @para 参数指针
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_otp_handle(void* para)
{
    s32 i = 0;

    para = para;

    pmic_print_error("hi6559:temperature overflow (>125℃)!\n");

    /*由nv控制过温是否关闭*/
    pmu_hi6559_om_data[0] |= (0x01 << HI6559_OTMP_125_OFFSET);
    
    /*根据nv配置，决定是否关闭非核心电源,后续增加产品形态相关特殊处理*/
    if(pmu_exc_pro.ulOtpCurIsOff)
    {
        for(i = PMIC_HI6559_VOLT_MIN; i < PMIC_HI6559_VOLT_MAX; i++)
        {
            if(pmu_exc_pro.VoltProConfig[i].VoltOtpIsOff)
            {
                pmic_print_error("hi6559:volt id %d will be closed !\n",i);
                bsp_hi6559_volt_disable(i);
            }
        }
    }
    
    if(pmu_exc_pro.ulOtpIsRst)
    {
        pmic_print_error("hi6559:system will be restart!\n");
        system_error(DRV_ERRNO_PMU_OVER_TEMP, PMU_STATE_OVER_TEMP, 0, NULL, 0);
    }

    /* 记录om log */
    /* hi6559_om_log_save(PMU_OM_LOG_EXC); */
    print2file(PMU_OM_LOG, "pmu_hi6559:temperature overflow (>125℃)!\n");
    
    /*clear np int */
    bsp_hi6559_reg_write(HI6559_NP_SCP_RECORD1_OFFSET, (0x01 << HI6559_OTMP_125_OFFSET));
    pmic_print_error("\n**********hi6559_otp_isr**********\n");

    return;
}/*lint !e438*/

/*****************************************************************************
 函 数 名  : hi6559_ovp_handle
 功能描述  : 过压处理函数
 输入参数  : @para 参数指针
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void hi6559_ovp_handle(void* para)
{
    para = para;

    pmu_hi6559_om_data[0] |= (0x01 << HI6559_VSYS_OVER_6P0_OFFSET);

    /* 记录om log */
    /* hi6559_om_log_save(PMU_OM_LOG_EXC); */
    print2file(PMU_OM_LOG,"pmu_hi6559:vsys over 6.0v last time!\n");

    /* 清除非下电reg */
    bsp_hi6559_reg_write(HI6559_NP_IRQ1_RECORD_OFFSET, (0x01 << HI6559_VSYS_OVER_6P0_OFFSET));
    pmic_print_error("\n**********hi6559_ovp_isr**********\n");

    return;
}/*lint !e438*/

/*****************************************************************************
 函 数 名  : hi6559_uvp_warning_handle
 功能描述  : 欠压处理函数2.85/3.0，电压低于设定的告警阈值，所要进行的处理
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  : bbstar中断处理任务
*****************************************************************************/
void hi6559_uvp_warning_handle(void* para)
{
    para = para;
    pmu_hi6559_om_data[0] |= (0x01 << HI6559_VSYS_UNDER_2P85_OFFSET);

    /* 记录om log */
    /* hi6559_om_log_save(PMU_OM_LOG_EXC); */
    print2file(PMU_OM_LOG,"pmu_hi6559:vsys under 2.85/3.0v !\n");

    /* 根据nv配置确定是否重启 */
    if(pmu_exc_pro.ulUvpIsRst)
    {
        pmic_print_error("hi6559:system will be restart!\n");
        system_error(DRV_ERRNO_PMU_UNDEF_VOL, PMU_STATE_UNDER_VOL, 0, NULL, 0);
    }

    /*清除非下电reg，write 1 clear*/
    bsp_hi6559_reg_write(HI6559_NP_IRQ1_RECORD_OFFSET, (0x01 << HI6559_VSYS_UNDER_2P85_OFFSET));
    pmic_print_error("**********hi6559_uvp_2p85_isr**********\n");

    return;
}/*lint !e438*/

/*****************************************************************************
 函 数 名  : hi6559_uvp_die_handle
 功能描述  : 欠压2.5V处理函数，电压低于PMU所能容忍的阈值时所要进行的处理(按道
             理本函数调用到，系统就会挂)
 输入参数  : @para 参数指针
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : bbstar中断处理任务
*****************************************************************************/
void hi6559_uvp_die_handle(void* para)
{
    para = para;

    pmu_hi6559_om_data[0] |= (0x01 << HI6559_VSYS_UNDER_2P5_OFFSET);
    /*记录om log */
    /* hi6559_om_log_save(PMU_OM_LOG_EXC); */
    print2file(PMU_OM_LOG, "pmu_hi6559:vsys under 2.5v !\n");

    /* 清除非下电reg */
    bsp_hi6559_reg_write(HI6559_NP_IRQ1_RECORD_OFFSET, (0x01 << HI6559_VSYS_UNDER_2P5_OFFSET));
    pmic_print_error("\n**********hi6559_uvp_2p5_isr**********\n");

    return;
}/*lint !e438*/

/*****************************************************************************
 函 数 名  : bsp_hi6559_exc_init
 功能描述  : 异常处理模块初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
s32 bsp_hi6559_exc_init(void)
{
    hi6559_excflags_t exc_flags = 0;
    u32 *volt_need_off = (u32 *)SHM_PMU_OCP_INFO_ADDR;  /*过流，需要关闭的过流源*/
    s32 iret = 0;
    s32 ret = 0;
    u16 i = 0;
    u8 reg_temp = 0;

    /*init default nv config*/
    memset((void*)&pmu_exc_pro, 0, sizeof(PMU_EXC_PRO_NV_STRU));
    pmu_exc_pro.ulOcpIsOn = 0;      /* 过流的电源可以重新打开 */
    pmu_exc_pro.ulOcpIsOff = 1;     /* 过流的电源需要关闭 */
    pmu_exc_pro.ulOtpCurIsOff = 1;  /* 发生过温后需要关闭非核心电源 */
    pmu_exc_pro.ulOtpIsRst = 0;     /* 发生过温后系统不重启 */
    pmu_exc_pro.ulOtpIsOff = 1;     /* 过温150度后PMU下电 */
    pmu_exc_pro.ulOtpLimit = 125;   /* 温度预警阈值125度 */
    pmu_exc_pro.ulUvpIsRst = 1;     /* 发生欠压后系统重启 */
    pmu_exc_pro.ulUvpLimit = 3000;  /* 欠压预警阈值3000mV */

    spin_lock_init(&g_hi6559_exc_st.lock);  /* 中断只在Acore实现，多core互斥锁 */
    
    /* 读NV */
    iret = (s32)bsp_nvm_read(NV_ID_DRV_NV_PMU_EXC_PRO,(u8 *)&pmu_exc_pro,sizeof(PMU_EXC_PRO_NV_STRU));
    if(NV_OK != iret)
    {
        pmic_print_error("ERROR: read nv failed, ret %d!\n", iret);
        ret |= iret;
    }
    else
    {
        /* 设置过温预警阈值 */
        bsp_hi6559_otp_threshold_set((int)pmu_exc_pro.ulOtpLimit);
        /* 设置欠压预警阈值 */
        bsp_hi6559_uvp_threshold_set((int)pmu_exc_pro.ulUvpLimit);
    }

    /* 写start flag到log文件 */
    hi6559_om_log_save(PMU_OM_LOG_START);
    
    /* 保存非下电寄存器 */
    hi6559_boot_om_log_save();

    /* 清除非下电寄存器 */
    for(i = 0; i < HI6559_NP_RECORD_REG_NUM; i++)
    {
        spin_lock_irqsave(&g_hi6559_exc_st.lock,exc_flags);
        bsp_hi6559_reg_read((HI6559_NP_IRQ1_RECORD_OFFSET+i), &reg_temp);
        bsp_hi6559_reg_write((HI6559_NP_IRQ1_RECORD_OFFSET+i), reg_temp);
        spin_unlock_irqrestore(&g_hi6559_exc_st.lock,exc_flags);
    }

    /* 初始化过流标志位 */
    *volt_need_off = 0;

    /* 注册中断处理函数 */
    iret = hi6559_irq_callback_register(IRQ_HI6559_OCP_SCP, hi6559_ocp_scp_handle, NULL);
    if (iret) {
        pmic_print_error("hi6559 irq register ocp interrupt failed!\n");
        ret |= iret;
    }
    iret = hi6559_irq_callback_register(IRQ_HI6559_OTMP, hi6559_otp_handle, NULL);
    if (iret) {
        pmic_print_error("hi6559 irq register otmp interrupt failed!\n");
        ret |= iret;
    }
    iret = hi6559_irq_callback_register(IRQ_HI6559_VSYS_UNDER_2P5, hi6559_uvp_die_handle, NULL);
    if (iret) {
        pmic_print_error("hi6559 irq register ocp interrupt failed!\n");
        ret |= iret;
    }
    iret = hi6559_irq_callback_register(IRQ_HI6559_VSYS_UNDER_2P85, hi6559_uvp_warning_handle, NULL);
    if (iret) {
        pmic_print_error("hi6559 irq register otmp interrupt failed!\n");
        ret |= iret;
    }
    iret = hi6559_irq_callback_register(IRQ_HI6559_VSYS_OVER_6P0, hi6559_ovp_handle, NULL);
    if (iret) {
        pmic_print_error("hi6559 irq register otmp interrupt failed!\n");
        ret |= iret;
    }

    pmic_print_error("hi6559 exc init ok!\n");

    return ret;
}

module_init(bsp_hi6559_exc_init);
static void __exit bsp_hi6559_exc_exit(void)
{
}
module_exit(bsp_hi6559_exc_exit);

