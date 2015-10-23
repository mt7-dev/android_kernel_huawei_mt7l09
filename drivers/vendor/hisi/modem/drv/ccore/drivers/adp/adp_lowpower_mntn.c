/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  adp_lowpower_mntn.c
*
*   作    者 :  xujingcui
*
*   描    述 : 本文件用于接口适配
*
*   修改记录 :  2013年6月10日  v1.00 xujingcui创建
*************************************************************************/
#include <bsp_lowpower_mntn.h>
#include <bsp_wakelock.h>
#include <string.h>
#include <vxWorks.h>
#include <mmuLib.h>
#include <errno.h>
#include <cacheLib.h>
#include <private/classLibP.h>
#include <private/objLibP.h>
#include <vmLib.h>
#include <cacheLib.h>
#include <private/memPartLibP.h>
#include <private/vmLibP.h>
#include <stdlib.h>

#define  mntn_printf(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_WAKELOCK, "[Lowpower_mntn]: <%s> "fmt"", __FUNCTION__, ##__VA_ARGS__))

struct lock_info hi6930_lockinfo;

/*低功耗控制和DEBUG NV --NV_ID_DRV_NV_PWC_SWITCH */
ST_PWC_SWITCH_STRU g_nv_pwc_switch = {0};  

/*记录低功耗共享地址*/
unsigned int g_lowpower_shared_addr = 0;

extern MMU_LIB_FUNCS	mmuLibFuncs;		  /* initialized by mmuLib.c */
#define MMU_ATTR_NORMAL_NONCACHEABLE_MSK (MMU_ATTR_SPL_MSK | MMU_ATTR_CACHE_MSK) /*mmuArmArch6PalLib.h*/
#define MMU_ATTR_NORMAL_NONCACHEABLE     (MMU_ATTR_SPL_0 | MMU_ATTR_CACHE_OFF) /*mmuArmArch6PalLib.h*/

void* dmem_malloc(unsigned int bytes, unsigned int *refcount)
{	
	if(refcount == NULL){		
		 mntn_printf("you input refcount = NULL!\n");
		 return NULL;
	}
	*refcount = 0;
	return KMEM_ALIGNED_ALLOC(ROUND_UP(bytes, MMU_PAGE_SIZE_GET()), MMU_PAGE_SIZE_GET());
}

void dmem_free(void* p)
{	
	free(p);
}

void* dmem_cache_dma_malloc(unsigned int bytes, unsigned int *refcount)
{
	STATUS state =0;
	unsigned long flags = 0;
	unsigned int alloc_size = ((bytes +  MMU_PAGE_SIZE_GET() -1) & ~(MMU_PAGE_SIZE_GET() -1));
	void* p = KMEM_ALIGNED_ALLOC(alloc_size, MMU_PAGE_SIZE_GET());	
	if(refcount == NULL){		
		 mntn_printf("you input refcount = NULL!\n");
		 return NULL;
	}
	if (p)
	{
		local_irq_save(flags);		
		*refcount = 0;
		state = VM_STATE_SET(NULL, p, alloc_size
			, MMU_ATTR_VALID_MSK | MMU_ATTR_SPL_MSK | MMU_ATTR_PROT_MSK | MMU_ATTR_NORMAL_NONCACHEABLE_MSK
			, MMU_ATTR_VALID | MMU_ATTR_NO_BLOCK | MMU_ATTR_SUP_RW | MMU_ATTR_NORMAL_NONCACHEABLE);		
		local_irq_restore(flags);
		if (state != OK )
		{
			free(p);
			return NULL;
		}
	}
	return p;
}

STATUS dmem_cache_dma_free(void* p)
{	
	STATUS state = OK;	
	unsigned long flags = 0;
	if(NULL != p){
		
		local_irq_save(flags);	
		state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p), MMU_ATTR_CACHE_MSK | MMU_ATTR_PROT_MSK, MMU_ATTR_CACHE_DEFAULT | MMU_ATTR_SUP_RW);		
		local_irq_restore(flags);
	}
	free(p);	
	return state;
}

STATUS dmem_lock(void* p, unsigned int *refcount)
{
	STATUS state = OK;
	unsigned long flags = 0;
	if(p == NULL){
		mntn_printf("you input p = NULL!\n");
		return ERROR;
	}	
	if(refcount == NULL){
		 mntn_printf("you input refcount = NULL!\n");
		 return ERROR;
	}
	local_irq_save(flags);
	if((*refcount) == 1){
		state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p), MMU_ATTR_PROT_MSK, MMU_ATTR_PROT_SUP_READ);
	}
	if((*refcount) == 0){
		mntn_printf("please call dmem_unlock first!\n");
		local_irq_restore(flags);		
		return state;
	}
	(*refcount)--;
	local_irq_restore(flags);
	return state;
}

STATUS dmem_unlock(void* p, unsigned int *refcount)
{
	STATUS state = OK;	
	unsigned long flags = 0;
	if(p == NULL){
		mntn_printf("you input p = NULL!\n");
		return ERROR;
	}	
	if(refcount == NULL){		
		 mntn_printf("you input refcount = NULL!\n");
		 return ERROR;
	}
	local_irq_save(flags);
	if((*refcount) == 0){
		state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p), MMU_ATTR_PROT_MSK, MMU_ATTR_SUP_RWX);
	}
	(*refcount)++;
	local_irq_restore(flags);
	return state;
}



void update_awake_time_stamp(void)
{
    return;
}
void check_awake_time_limit(void)
{
    return;
}
void bsp_lowpower_mntn_init(void)
{
    int ret = 0;
	char* dump_base = NULL;
	unsigned int dump_length = 0;

	/*获取DUMP 低功耗共享内存地址*/
	if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_DRX_CCORE, &dump_base, &dump_length)){
		mntn_printf("*******!get dump buffer failed!******* \n\n");
		return;
	}
    g_lowpower_shared_addr = (unsigned int)dump_base;
    mntn_printf("pm shared mem addr: g_lowpower_shared_addr = 0x %x!\n",g_lowpower_shared_addr);
    ret = bsp_nvm_read(NV_ID_DRV_NV_PWC_SWITCH, (u8 *)(&g_nv_pwc_switch), sizeof(ST_PWC_SWITCH_STRU));
    if(ret!=0)
    {
        logMsg("NV_ID_DRV_NV_PWC_SWITCH:D10B read failure!\n",0,0,0,0,0,0);
    }else{
        logMsg("NV_ID_DRV_NV_PWC_SWITCH:D10B read success!\n",0,0,0,0,0,0);
    }
    bsp_adp_dpm_debug_init();
    return;
}
void bsp_save_lowpower_status(void)
{
	return;
}
void bsp_dump_lowpower_status(void)
{
	return;
}
void bsp_show_lowpower_status(void)
{
	return;
}
void bsp_dump_bus_error_status(void)
{
	return;
}

