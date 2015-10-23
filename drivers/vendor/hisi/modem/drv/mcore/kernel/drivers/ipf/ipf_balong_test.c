#include "bsp_ipf.h"

u32* g_u32_ipf_setting_bak_addr_test = (u32*)IPF_PWRCTL_INFO_ADDR;
u32* g_u32_ipf_adq_rptr_bak_addr_test = (u32*)IPF_PWRCTL_ADQ_WPTR_INFO_ADDR;
u32* g_u32_ipf_adq_wptr_chctrl_bak_addr_test = (u32*)IPF_PWRCTL_ADQ_RPTR_CHCTRL_ADDR;

u32 g_ipf_setting_table_test[] =
    {
        HI_IPF_CTRL_OFFSET,
        HI_IPF_GATE_OFFSET,
        HI_IPF_DMA_CTRL1_OFFSET,
        HI_IPF_EF_BADDR_OFFSET,

        HI_IPF_CH0_CTRL_OFFSET,
        HI_IPF_CH1_CTRL_OFFSET,		
			
        HI_IPF_CH0_BDQ_SIZE_OFFSET,
        HI_IPF_CH1_BDQ_SIZE_OFFSET,
        
        HI_IPF_CH0_RDQ_SIZE_OFFSET,
        HI_IPF_CH1_RDQ_SIZE_OFFSET,
        
        HI_IPF_CH0_BDQ_BADDR_OFFSET,
        HI_IPF_CH1_BDQ_BADDR_OFFSET,
        
        HI_IPF_CH0_RDQ_BADDR_OFFSET,
        HI_IPF_CH1_RDQ_BADDR_OFFSET,
        
        HI_IPF_CH0_ADQ0_BASE_OFFSET, 
        HI_IPF_CH0_ADQ1_BASE_OFFSET, 
        
        HI_IPF_CH1_ADQ0_BASE_OFFSET,  
        HI_IPF_CH1_ADQ1_BASE_OFFSET, 
        
        HI_IPF_TIME_OUT_OFFSET,
        HI_IPF_PKT_LEN_OFFSET
    };
u32 g_ipf_adq_rptr_table_test[] =
    {
        HI_IPF_CH0_ADQ0_RPTR_OFFSET, 
        HI_IPF_CH0_ADQ1_RPTR_OFFSET, 
        HI_IPF_CH1_ADQ0_RPTR_OFFSET, 
        HI_IPF_CH1_ADQ1_RPTR_OFFSET, 
        
        HI_IPF_CH0_ADQ0_STAT_OFFSET, 
        HI_IPF_CH0_ADQ1_STAT_OFFSET, 
        HI_IPF_CH1_ADQ0_STAT_OFFSET, 
        HI_IPF_CH1_ADQ1_STAT_OFFSET
    };
u32 g_ipf_adq_wptr_chctrl_table_test[] =
    {
        HI_IPF_CH0_ADQ0_WPTR_OFFSET, 
        HI_IPF_CH0_ADQ1_WPTR_OFFSET, 
        HI_IPF_CH1_ADQ0_WPTR_OFFSET, 
        HI_IPF_CH1_ADQ1_WPTR_OFFSET, 

        HI_IPF_CH0_ADQ_CTRL_OFFSET,
        HI_IPF_CH1_ADQ_CTRL_OFFSET,
        HI_IPF_INT_MASK0_OFFSET,
        HI_IPF_INT_MASK1_OFFSET,		
        HI_IPF_CH_EN_OFFSET
    };


BSP_VOID PrintMatchInfo(IPF_MATCH_INFO_S * pu32MatchInfo)
{
    BSP_U32 i = 0;
    BSP_U32* p;
	
    BSP_U32 u32Len = sizeof(IPF_MATCH_INFO_S);
    p = (BSP_U32 *)(pu32MatchInfo);
    u32Len = u32Len/4;
	
    for(i=0; i < u32Len; i++)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "0x%08x    ", *(p+i));
        if((i%4) == 0)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\n");
        }
    }
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\n");
}

BSP_S32 CmpMatchInfo(IPF_MATCH_INFO_S * p1t,IPF_MATCH_INFO_S * p2t)
{
    BSP_U32 i = 0;
    BSP_U32 *u32p1 = (BSP_U32 *)p1t;
    BSP_U32 *u32p2 = (BSP_U32 *)p2t;
    BSP_U32 u32Len = 0; 
    u32Len = sizeof(IPF_MATCH_INFO_S);
    u32Len = u32Len/4;
	
    for(i=0; i<u32Len; i++)
    {
        if(*(u32p1 + i) != *(u32p2 + i))
        {
            return ERROR;
        }
    }
    return 0;
}

s32 check_filter_restore_status(void)
{
    s32 ret;
    u32 i,j,k,filter_serial;
    IPF_MATCH_INFO_S* hard_filter_info;
    u32 *hard_filter_info_addr;
    IPF_MATCH_INFO_S* backup_filter_info;
    u32* backup_info_addr = NULL;
    hard_filter_info = (IPF_MATCH_INFO_S*)malloc(sizeof(IPF_MATCH_INFO_S));
    if(NULL == hard_filter_info)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r pstMatchInfo malloc ERROR! \n");
        return IPF_ERROR;
    }
    hard_filter_info_addr = (u32*)hard_filter_info;

    for(i=0 ; i < IPF_MODEM_MAX; i++)
    {
        backup_info_addr = (u32 *)(IPF_PWRCTL_BASIC_FILTER_ADDR + i*sizeof(IPF_MATCH_INFO_S));
        backup_filter_info = (IPF_MATCH_INFO_S*)backup_info_addr;
        filter_serial = i;
        k = 0;
        do
        {
            if(filter_serial < IPF_BF_NUM)
            {
                ipf_writel(filter_serial, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 
                for(j=0; j<(sizeof(IPF_MATCH_INFO_S)/4); j++)
                {
                    *hard_filter_info_addr = ipf_readl((HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET+j*4)); 
                    hard_filter_info_addr++;
                }
                k++;
                ret = CmpMatchInfo(backup_filter_info, hard_filter_info);
                if(ret != 0)
                {
                    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "chainNum = %d \n",i);
                    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "filterNum = %d \n",filter_serial);
                    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "========BACKUPED_FILTER_INFO=======\n");
                    PrintMatchInfo(backup_filter_info);
                    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "========FILTER_ACTUAL_INFO=======\n");
                    PrintMatchInfo(hard_filter_info);
                }
            hard_filter_info_addr = (u32*)hard_filter_info;
            }
            filter_serial = backup_filter_info->unFltChain.Bits.u16NextIndex;
            backup_filter_info = (IPF_MATCH_INFO_S*)(IPF_PWRCTL_BASIC_FILTER_ADDR + filter_serial*sizeof(IPF_MATCH_INFO_S));
            backup_info_addr = (u32 *)backup_filter_info;
            if(k >= IPF_BF_NUM)
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "Filter end error!\n");
                return IPF_ERROR;
            }
        }while(filter_serial != IPF_TAIL_INDEX);
    }
	return IPF_SUCCESS;
}

s32 compare_reg_table(u32* reg_table, u32* backup_space, u32 reg_table_size)
{
	u32 i;
	u32* backup_info;
	u32 reg_info;
	
	backup_info = backup_space;
	for(i=0; i<reg_table_size/4; i++)
	{	
		reg_info = ipf_readl(*(reg_table+i) + HI_IPF_REGBASE_ADDR_VIRT);
		if(*backup_info != reg_info)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "compare_reg_table error!\n");
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "backup_info = 0x%x\n", *backup_info);
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "reg_info = 0x%x\n",reg_info);
			return IPF_ERROR;
		}
		backup_info ++;

	}
	return IPF_SUCCESS;
}

	/*低功耗上下电测试*/
	 /*低功耗寄存器备份测试*/
	BSP_S32 BSP_IPF_ST_401()
	{
		s32 result = 0;
		u32 reg_table_size =0;
		result = ipf_drx_bak_reg();
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_401 backup reg fail!\n");
			return IPF_ERROR;
		}
		
		reg_table_size = sizeof(g_ipf_setting_table_test);
		result = compare_reg_table(g_ipf_setting_table_test, (u32*)IPF_PWRCTL_INFO_ADDR, reg_table_size);
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 backup g_ipf_setting_table_test fail!\n");
			return IPF_ERROR;
		}

		reg_table_size = sizeof(g_ipf_adq_rptr_table_test);
		result = compare_reg_table(g_ipf_adq_rptr_table_test, (u32*)IPF_PWRCTL_ADQ_WPTR_INFO_ADDR, reg_table_size);
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 backup g_ipf_setting_table_test fail!\n");
			return IPF_ERROR;
		}

		reg_table_size = sizeof(g_ipf_adq_wptr_chctrl_table_test);		
		result = compare_reg_table(g_ipf_adq_wptr_chctrl_table_test, (u32*)IPF_PWRCTL_ADQ_RPTR_CHCTRL_ADDR, reg_table_size);
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 backup g_ipf_setting_table_test fail!\n");
			return IPF_ERROR;
		}

		return IPF_SUCCESS;
	}


	
	 /*低功耗寄存器恢复测试*/

	BSP_S32 BSP_IPF_ST_402()
	{
		s32 result = 0;
		u32 reg_table_size =0;
		
		result = ipf_drx_bak_reg();
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 backup reg fail!\n");
			return IPF_ERROR;
		}
		
		/*使用syssc reset ipf，根据mengbiao，这会使所有寄存器清0*/



		ipf_drx_restore_reg();
		
		reg_table_size = sizeof(g_ipf_setting_table_test);
		result = compare_reg_table(g_ipf_setting_table_test, (u32*)IPF_PWRCTL_INFO_ADDR, reg_table_size);
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 restore g_ipf_setting_table_test fail!\n");
			return IPF_ERROR;
		}

		reg_table_size = sizeof(g_ipf_adq_rptr_table_test);
		result = compare_reg_table(g_ipf_adq_rptr_table_test, (u32*)IPF_PWRCTL_ADQ_WPTR_INFO_ADDR, reg_table_size);
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 restore g_ipf_setting_table_test fail!\n");
			return IPF_ERROR;
		}

		reg_table_size = sizeof(g_ipf_adq_wptr_chctrl_table_test);		
		result = compare_reg_table(g_ipf_adq_wptr_chctrl_table_test, (u32*)IPF_PWRCTL_ADQ_RPTR_CHCTRL_ADDR, reg_table_size);
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 restore g_ipf_setting_table_test fail!\n");
			return IPF_ERROR;
		}


		
		return IPF_SUCCESS;
	}



	/*Check of ipf filter restore success or not test*/
	BSP_S32 BSP_IPF_ST_403()
	{
	    s32 result = 0;
		    
		result = ipf_drx_bak_reg();
		if(IPF_SUCCESS != result)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ST_402 backup reg fail!\n");
			return IPF_ERROR;
		}
		
		ipf_drx_restore_reg();


		return check_filter_restore_status();

	}


	

