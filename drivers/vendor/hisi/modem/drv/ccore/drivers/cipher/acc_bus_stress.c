
#include "memLib.h"
#include "semLib.h"
#include <stdlib.h>
#include <taskLib.h>
#include <logLib.h>
#include <stdio.h>
#include "product_config.h"
#include "drv_cipher.h"
#include "cipher_balong_common.h"
#include "bsp_busstress.h"
#include "bsp_softtimer.h"


#define ACC_CHN_NUM 0
static BSP_U32  ACC_APH_LEN = 0;
//static SEM_ID g_UsrClkSendCountSem = NULL;
static BSP_U32 g_AccInitCmp = FALSE;
static BSP_U32 g_ChnBusy = 0;
static BSP_U32 g_keyRam[4] = {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c};
static struct softtimer_list acc_timer_id ;
static SEM_ID acc_test_sem = (SEM_ID)NULL;
static ACC_SINGLE_CFG_S *pstCfg = NULL;
static CIHPER_SINGLE_CFG_S *pstCipherCfg = NULL;
static void *pInAddr = NULL;
static void *pOutAddr = NULL;
static BSP_S32 acc_taskid1 = 0,acc_taskid2 = 0,acc_taskid3 = 0;
/*****************************************************************************
* 函 数 名  : Acc_CipherCfg
*
* 功能描述  :配置cipher配置信息
*
* 输入参数  :void* mem1 :输入数据块首地址
                           void* mem2 :输出数据块首地址
                           BSP_U32 size :数据长度
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  :
*
*****************************************************************************/
static void Acc_CipherCfg(ACC_SINGLE_CFG_S *pstCfg, BSP_U32 CfgType)
{
    /*配置pstCfg信息*/
    switch(CfgType)
    {
        case 0: 
            BSP_CIPHER_SetKey(g_keyRam , 0, 0);
            pstCfg->stAlgKeyInfo.u32KeyIndexSec = 0;                         /*加密操作所需要key的序号*/
            pstCfg->u32AppdHeaderLen = 0;                                  /*附加包头的长度*/
            pstCfg->u32HeaderLen = 0;                                        /*包头packer head的长度*/
            pstCfg->stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;                               /*加密操作配置SNOW3G*/
            pstCfg->stAlgKeyInfo.enAlgIntegrity= 0; 
            pstCfg->u32BearId = 12;    
            pstCfg->u32Aph = 0x512;  
            pstCfg->u32Count = 0x512;
            break;
            
        case 1:
            BSP_CIPHER_SetKey(g_keyRam , 0, 0);
            pstCfg->stAlgKeyInfo.u32KeyIndexSec = 0;                         /*加密操作所需要key的序号*/
            pstCfg->u32AppdHeaderLen = 0;                                  /*附加包头的长度*/
            pstCfg->u32HeaderLen = 0;                                        /*包头packer head的长度*/
            pstCfg->stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;                               /*加密操作配置AES192*/
            pstCfg->stAlgKeyInfo.enAlgIntegrity= 0; 
            pstCfg->u32BearId = 12;    
            pstCfg->u32Aph = 0x56432F;  
            pstCfg->u32Count = 0x56432F;
            break;
            
        case 2:
            BSP_CIPHER_SetKey(g_keyRam , 0, 0);
            pstCfg->stAlgKeyInfo.u32KeyIndexSec = 0;                         /*加密操作所需要key的序号*/
            pstCfg->u32AppdHeaderLen = 1;                                  /*附加包头的长度*/
            pstCfg->u32HeaderLen = 0;                                        /*包头packer head的长度*/
            pstCfg->stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;                               /*加密操作配置AES128*/
            pstCfg->stAlgKeyInfo.enAlgIntegrity= 0; 
            pstCfg->u32BearId = 12;    
            pstCfg->u32Aph = 0x56432F;  
            pstCfg->u32Count = 0x56432F;
            break;
            
        case 3:
            BSP_CIPHER_SetKey(g_keyRam , 0, 0);
            pstCfg->stAlgKeyInfo.u32KeyIndexSec = 0;                         /*加密操作所需要key的序号*/
            pstCfg->u32AppdHeaderLen = 2;                                  /*附加包头的长度*/
            pstCfg->u32HeaderLen = 0;                                        /*包头packer head的长度*/
            pstCfg->stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;                               /*加密操作配置AES256*/
            pstCfg->stAlgKeyInfo.enAlgIntegrity= 0; 
            pstCfg->u32BearId = 3;    
            pstCfg->u32Aph = 256;  
            pstCfg->u32Count = 256;
            break;
        case 4:
            BSP_CIPHER_SetKey(g_keyRam , 0, 0);
            pstCfg->stAlgKeyInfo.u32KeyIndexSec = 0;                         /*加密操作所需要key的序号*/
            pstCfg->u32AppdHeaderLen = 2;                                  /*附加包头的长度*/
            pstCfg->u32HeaderLen = 2;                                        /*包头packer head的长度*/
            pstCfg->stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;                               /*加密操作配置NULL*/
            pstCfg->stAlgKeyInfo.enAlgIntegrity= 0; 
            pstCfg->u32BearId = 3;    
            pstCfg->u32Aph = 256;  
            pstCfg->u32Count = 256;
            break;            
    }
    return;
}

BSP_S32 acc_ddr_data_init(BSP_U32 u32BlockLen, BSP_U32 u32PakNums, void **pInAddr, void **pOutAddr,
                        ACC_SINGLE_CFG_S ** pstCfg,CIHPER_SINGLE_CFG_S **pstCipherCfg)
{
    BSP_S32 i;
    char * ptempIn;

    *pstCipherCfg = (CIHPER_SINGLE_CFG_S *) CIPHER_MALLOC(sizeof(CIHPER_SINGLE_CFG_S));
    if(!*pstCipherCfg)
    {
        logMsg(">>>line %d FUNC %s fail, malloc failed !\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
        return ERROR;
    }

    *pstCfg = (ACC_SINGLE_CFG_S *)CIPHER_MALLOC(sizeof(ACC_SINGLE_CFG_S));
    if(!*pstCfg)
    {
        CIPHER_FREE(*pstCipherCfg);
        logMsg(">>>line %d FUNC %s fail, malloc failed !\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
        return ERROR;
    }
    
    *pInAddr = (void *)CIPHER_MALLOC(u32BlockLen * u32PakNums + ACC_APH_LEN* u32PakNums);
    if(NULL == *pInAddr)
    {
        CIPHER_FREE(*pstCipherCfg);
        CIPHER_FREE(*pstCfg);
        logMsg(">>>line %d FUNC %s fail, malloc failed !\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
        return ERROR;
    }
    
    *pOutAddr = (void *)CIPHER_MALLOC(u32BlockLen * u32PakNums + ACC_APH_LEN* u32PakNums);
    if(NULL == *pInAddr)
    {
        CIPHER_FREE(*pstCipherCfg);
        CIPHER_FREE(*pstCfg);
        CIPHER_FREE(*pInAddr);
        logMsg(">>>line %d FUNC %s fail, malloc failed !\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
        return ERROR;
    }

    ptempIn = *pInAddr;
    for(i = 0; i <(BSP_S32) (u32BlockLen * u32PakNums + ACC_APH_LEN* u32PakNums ); i ++)
    {
        *ptempIn = (char)0xDD;
        ptempIn++;
    }
    memset(*pOutAddr, 0, u32BlockLen * u32PakNums + ACC_APH_LEN* u32PakNums);
    
    return OK;
}

void acc_timer_event(BSP_U32 parm)
{
	if(NULL != acc_test_sem)
	{
		semGive(acc_test_sem);
	}
	bsp_softtimer_add(&acc_timer_id);
}

BSP_S32 acc_stress_test_routine(BSP_U32 u32BlockLen, BSP_U32 u32PakNums,    void *pInAddr, void *pOutAddr, 
                    ACC_SINGLE_CFG_S *pstCfg, CIHPER_SINGLE_CFG_S *pstCipherCfg)
{
    BSP_S32 s32ret;
    BSP_U32 u32BdFifoAddr;
    void *pTempInAddr = NULL;
    void *pTempOutAddr = NULL;
    int i = 0;
    
    while (get_test_switch_stat(ACC))
    {
        pTempInAddr = pInAddr;
        pTempOutAddr = pOutAddr;
        if (semTake(acc_test_sem, WAIT_FOREVER) == ERROR)
        {
            logMsg(">>>line %d FUNC %s fail\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
           return ERROR;
        }
       
        /*获取可用FIFO首地址*/
        u32BdFifoAddr = BSP_Accelerator_GetBdFifoAddr();
        if((BSP_U32)CIPHER_UNKNOWN_ERROR == u32BdFifoAddr )
        {
            logMsg(">>>line %d FUNC %s fail，result = 0x%x\n", (int)__LINE__, (int)__FUNCTION__, u32BdFifoAddr, 0, 0, 0);
            return ERROR;
        }

        /*配置Cipher加密方式*/ 
        for(i = 0; i <(BSP_S32)u32PakNums; i++)
        {
            s32ret = BSP_Accelerator_DMA(u32BdFifoAddr, pTempInAddr, pTempOutAddr, pstCfg);
            if(OK != s32ret)
            {
                logMsg(">>>line %d FUNC %s fail，return = 0x%x\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
                return ERROR;
            }
            pTempInAddr = (char *)((BSP_U32)pTempInAddr +u32BlockLen);
            pTempOutAddr = (char *)((BSP_U32)pTempOutAddr + u32BlockLen);
                
            s32ret = BSP_Accelerator_Cipher(u32BdFifoAddr, pTempInAddr, pTempOutAddr, pstCfg);
            if(OK != s32ret)
            {
                logMsg(">>>line %d FUNC %s fail，return = 0x%x\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
                return ERROR;
            }
            pTempInAddr = (char *)((BSP_U32)pTempInAddr +u32BlockLen);
            pTempOutAddr = (char *)((BSP_U32)pTempOutAddr + u32BlockLen);
        }    

        /*获取当前通道状态，空闲则使能组包加速*/
        while(BSP_Accelerator_GetStatus() != CHN_FREE)
        {
            g_ChnBusy ++;
        }
	
        s32ret = BSP_Accelerator_Enable(u32BdFifoAddr);
        if(CIPHER_UNKNOWN_ERROR == s32ret)
        {
            logMsg(">>>line %d FUNC %s fail，return = 0x%x\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
            return ERROR;
        } 	
    }
  
    return OK;       
}

BSP_S32 acc_stress_test_start(BSP_S32 task_priority,BSP_S32 test_type,BSP_S32 test_rate)
{
	BSP_U32 u32BlockLen = 1500;
	BSP_U32 u32PakNums = 63;
	BSP_U32 u32Offset = 0;
	BSP_U32 CfgType = 1;
	BSP_S32 s32ret = 0;
	acc_timer_id.func = acc_timer_event;
	acc_timer_id.para = 0;
	acc_timer_id.timeout = test_rate;
	acc_timer_id.wake_type = SOFTTIMER_WAKE;

	pstCfg = NULL;
	pstCipherCfg = NULL;
	pInAddr = NULL;
	pOutAddr = NULL;
	acc_taskid1 = 0;
	acc_taskid2 = 0;
	acc_taskid3 = 0;
	acc_test_sem = (SEM_ID)NULL;


	if(cipher_init())
	{
	    logMsg(">>>line %d FUNC %s fail, Cipher Init Failed! return = 0x%x\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
	    return ERROR;
	}
	
	if((DDR == test_type) ||(DDR_BBE16_AXIMEM == test_type))
	{
		s32ret = acc_ddr_data_init(u32BlockLen, 2*u32PakNums, &pInAddr, &pOutAddr,&pstCfg,&pstCipherCfg);
		if(ERROR == s32ret)
		{
		    return ERROR;
		}

		/*配置pstCfg信息*/
		Acc_CipherCfg(pstCfg,CfgType);
		pstCfg->u32BlockLen = u32BlockLen;
		pstCfg->u32Offset = u32Offset;
		pstCfg->u32OutLen = u32BlockLen - u32Offset;
	}

	/*初始化组包加速*/
	if(FALSE == g_AccInitCmp)
	{
		s32ret = bsp_acc_init();
	    if(CIPHER_NO_MEM == s32ret)
	    {
	        logMsg(">>>line %d FUNC %s fail, malloc failed !\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
	        return ERROR;
	    }
	     g_AccInitCmp = TRUE;
	}


	set_test_switch_stat(ACC, TEST_RUN);
	
	/*创建控制发送速率的信号量*/
	acc_test_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	if (acc_test_sem == NULL)
	{
		printf("semBCreate gmac_send_sem fail.\n");
		return BSP_ERROR; 
	} 

	/*创建控制发送速率 的软timer*/
	 
	if(bsp_softtimer_create(&acc_timer_id))
	{
		printf("SOFTTIMER_CREATE_TIMER fail.\n");
		return BSP_ERROR; 
	}

	set_test_switch_stat(ACC, TEST_RUN);
	if((DDR == test_type) ||(DDR_BBE16_AXIMEM == test_type))
	{
	    acc_taskid1 = taskSpawn ("accTestInDdr", task_priority, 0, 20000, (FUNCPTR)acc_stress_test_routine, (int)u32BlockLen,
	                                                (int)u32PakNums, (int)pInAddr, (int)pOutAddr,(int)pstCfg, (int)pstCipherCfg, 0, 0, 0, 0);
	        
	    if (ERROR == s32ret)
	    {
	        logMsg(">>>line %d FUNC %s fail，s32ret = %d\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
	        return ERROR;
	    }
	}

	bsp_softtimer_add(&acc_timer_id);
    return OK;    
}

BSP_S32 acc_stress_test_stop()
{
	set_test_switch_stat(ACC, TEST_STOP);
	while((OK == taskIdVerify(acc_taskid1)) || (OK == taskIdVerify(acc_taskid2))||(OK == taskIdVerify(acc_taskid3)))
	{
		taskDelay(100);
	}
	if(NULL != acc_test_sem)
	{
		semDelete(acc_test_sem);
	}
	if(acc_timer_id.init_flags == TIMER_INIT_FLAG)
	{
		bsp_softtimer_delete_sync(&acc_timer_id);
		bsp_softtimer_free(&acc_timer_id);
	}
	if(NULL != pstCipherCfg)
	{
		CIPHER_FREE(pstCipherCfg);
	}
	if(NULL != pstCfg)
	{
		CIPHER_FREE(pstCfg);
	}
	if(NULL != pInAddr)
	{
		CIPHER_FREE(pInAddr);
	}
	if(NULL != pOutAddr)
	{
		CIPHER_FREE(pOutAddr);
	}
	return OK;
}
