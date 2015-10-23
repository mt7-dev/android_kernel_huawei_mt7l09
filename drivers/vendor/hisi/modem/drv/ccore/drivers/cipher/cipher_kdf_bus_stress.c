
#include <vxWorks.h>
#include <semLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <logLib.h>
#include "cacheLib.h"
#include "product_config.h"
#include "DrvInterface.h"
#include "bsp_softtimer.h"
#include "bsp_busstress.h"
#include "cipher_balong_common.h"
#include "kdf_balong.h"
#include "acc_balong.h"
#include "cipher_balong.h"
#include "osl_irq.h"

//static BSP_U8 u8HerderLenTbl[CIPHER_HDR_BIT_TYPE_BUTT] = {0,1,1,2,1,1,2};
//static BSP_U8 u8APHLenTbl[CIPHER_HDR_BIT_TYPE_BUTT] = {0,1,1,2,1,1,2};

#define CIPHER_DEBUG_ENABLE (1)
/*
 * CIPHER_STRESS_TEST设置为0时，多通道并发测试会遍历所有cipher操作(除DMA外)，用于获取性能数据
 * CIPHER_STRESS_TEST设置为1时，多通道并发只测试callback方式提交，而且数据块变为60KB，同时不打印日志，用于压力测试
 * 该配置下专门用于总线压力测试
 */
#define CIPHER_STRESS_TEST  (1)

#define cipherDebugMsg(str,p0,p1,p2,p3,p4) logMsg("TEST-(line:%d)"str,__LINE__,p0,p1,p2,p3,p4)

#define NAS_CHN             (0x1)
#define PDCP_SIGNAL_CHN     (1)
#define PDCP_DATA_CHN_1     (2)
#define PDCP_DATA_CHN_2     (3)

#define CHAIN_MAX_BLOCK_NUM          (4)

#define MAC_SIZE_SHORT               (2)
#define MAC_SIZE_LONG                (4)
#define MAC_SIZE                    (4)

#define SEQNUM_SIZE                (1)
#define PH_SIZE                      (1)
#define MIN_CALLBACK_OPT_ID          (1)  

#define BLOCK_MAX_SIZE               (64*1024 + 4 + 20)

#define UP_LINK   (0)
#define DOWN_LINK (1)
#define PDCP_DATA_TASK_NUM (2)
#define TASK_STACK 0x8000

#define DEFAULT_THRESHOLD  (1024)
#define MIN_THRESHOLD      (0)
#define MAX_THRESHOLD      (8192)

#define CONTINUE_SUBM_MAX_SCP_NUM   (32)

#define TEST_TASK_PRIORITY     (150)
#define TEST_TASK_STACK_SIZE   (10000)

typedef struct tag_cipher_data_block
{
    BSP_U32 u32DataAddr;
    BSP_U32 u32DataLen;
    BSP_U32 pNextBlock;
}CIPHER_DATA_BLOCK;

static BSP_BOOL   g_cipher_test_inited = BSP_FALSE;
static CIPHER_DATA_BLOCK    cipherInBlock[CIPHER_MAX_CHN];
static CIPHER_DATA_BLOCK    cipherTmpBlock[CIPHER_MAX_CHN];
static CIPHER_DATA_BLOCK    cipherOutBlock[CIPHER_MAX_CHN];
static SEM_ID               cipherChnCbSem[CIPHER_MAX_CHN];
static CIPHER_NOTIFY_STAT_E g_notify_stat[CIPHER_MAX_CHN] = {0};
static CIHPER_SINGLE_CFG_S         g_cipher_uplink_cfg[CIPHER_MAX_CHN];
static CIHPER_SINGLE_CFG_S         g_cipher_downlink_cfg[CIPHER_MAX_CHN];
#if 0
static BSP_U8               g_iv[16] = {
                     0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
                     };
static BSP_U8               g_key[32] = {
                     0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
                     0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                     0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f                     
                     };
static BSP_U8               g_key2[32] = {
                     0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                     0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,                     
                     0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
                     };
static BSP_U32 g_trans_err[CIPHER_MAX_CHN] = {0};
#endif
static BSP_U32 g_check_err[CIPHER_MAX_CHN] = {0};
static BSP_U32 g_FreeInMem_Num = 0;
static BSP_U32 g_FreeOutMem_Num = 0;
#if 0
static BSP_BOOL g_bUseFileSaveResult = BSP_FALSE;
static BSP_BOOL g_bEnableFluxTest = BSP_FALSE;
static BSP_BOOL g_bCheckOptIdPass[CIPHER_MAX_CHN] = {BSP_TRUE,BSP_TRUE,BSP_TRUE,BSP_TRUE};
static BSP_S32 nas_task_id = 0;
static BSP_S32 pdcp_signal_task_id = 0;
static BSP_S32 pdcp_data_1_task_id = 0;
static BSP_S32 pdcp_data_2_task_id = 0;
static BSP_BOOL g_bPurgeTestTaskDone = BSP_FALSE;
static BSP_BOOL g_bEnablePurgeStubTest = BSP_FALSE;
static BSP_BOOL g_local_purge_test = 0;
static BSP_S32 nas_purge_test_task_id = 0;
static BSP_S32 pdcp_signal_purge_test_task_id = 0;
static BSP_S32 pdcp_data_1_purge_test_task_id = 0;
static BSP_S32 pdcp_data_2_purge_test_task_id = 0;
static BSP_U32 g_u32TickStart[CIPHER_MAX_CHN][2]      = {{0},{0},{0},{0}};
static BSP_U32 g_u32TickEnd[CIPHER_MAX_CHN][2]        = {{0},{0},{0},{0}};
static BSP_U32 g_u32RealTotalTimes[CIPHER_MAX_CHN][2] = {{0},{0},{0},{0}};
static double  g_dRate[CIPHER_MAX_CHN][2]             = {{0.0f},{0.0f},{0.0f},{0.0f}};
static BSP_U32 g_test_times[2]                        = {0};
static BSP_S32 nas_perf_test_task_id = 0;
static BSP_S32 pdcp_signal_perf_test_task_id = 0;
static BSP_S32 pdcp_data1_perf_test_task_id = 0;
static BSP_S32 pdcp_data2_perf_test_task_id = 0;
static BSP_S32 flux_switch_task_id_1 = 0;
static BSP_S32 flux_switch_task_id_2 = 0;
static BSP_S32 nas_subm_none_task_id = 0;
static BSP_S32 other_subm_none_task_id_1 = 0;
static BSP_S32 other_subm_none_task_id_2 = 0;
static BSP_S32 other_subm_none_task_id_3 = 0;
#endif
static BSP_S32 s32LoopforMulti = 0;

#if CIPHER_DEBUG_ENABLE
static BSP_U32 pMallocAddrBak[3][CIPHER_MAX_CHN];
#endif

static struct softtimer_list  sft_timer_list ;
static SEM_ID cipher_send_sem = (SEM_ID)NULL;

static void cipherCbFunc(BSP_U32 u32ChNum, BSP_U32 u32SourAddr, BSP_U32 u32DestAddr,
               CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private);
static void cipherInBufFree(BSP_VOID* pMemAddr);
static void cipherOutBufFree(BSP_VOID* pMemAddr);

static void cipherTestInit()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret = 0;

    if(g_cipher_test_inited)
    {
       return;
    }        

    for(i = 0; i < CIPHER_MAX_CHN; i++)
    {
       memset((void*)&(g_cipher_uplink_cfg[i]),0,sizeof(CIHPER_SINGLE_CFG_S));
       memset((void*)&(g_cipher_downlink_cfg[i]),0,sizeof(CIHPER_SINGLE_CFG_S));
    }

    for(i = 0; i < CIPHER_MAX_CHN; i++)
    {
        memset((void*)&cipherInBlock[i],0,sizeof(CIPHER_DATA_BLOCK));
        memset((void*)&cipherTmpBlock[i],0,sizeof(CIPHER_DATA_BLOCK));
        memset((void*)&cipherOutBlock[i],0,sizeof(CIPHER_DATA_BLOCK));
    }

    for(i = 0; i < CIPHER_MAX_CHN; i++)
    {
        cipherInBlock[i].u32DataAddr = (BSP_U32)cacheDmaMalloc(BLOCK_MAX_SIZE);
        if(cipherInBlock[i].u32DataAddr == 0)
        {
            cipherDebugMsg("%s: cipherInBlock[%d].u32DataAddr malloc fail, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherInBlock[i].u32DataAddr,0,0);
            return;
        }
        else
        {
            /*cipherDebugMsg("%s: cipherInBlock[%d].u32DataAddr malloc sucess, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherInBlock[i].u32DataAddr,0,0);*/
        }
        pMallocAddrBak[0][i] = cipherInBlock[i].u32DataAddr;

        cipherTmpBlock[i].u32DataAddr = (BSP_U32)cacheDmaMalloc(BLOCK_MAX_SIZE);
        if(cipherTmpBlock[i].u32DataAddr == 0)
        {
            cipherDebugMsg("%s: cipherTmpBlock[%d].u32DataAddr malloc fail, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherTmpBlock[i].u32DataAddr,0,0);
            return;
        }
        else
        {
            /*cipherDebugMsg("%s: cipherTmpBlock[%d].u32DataAddr malloc sucess, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherTmpBlock[i].u32DataAddr,0,0);*/
        }
        pMallocAddrBak[1][i] = cipherTmpBlock[i].u32DataAddr;

        cipherOutBlock[i].u32DataAddr = (BSP_U32)cacheDmaMalloc(BLOCK_MAX_SIZE);
        if(cipherOutBlock[i].u32DataAddr == 0)
        {
            cipherDebugMsg("%s: cipherOutBlock[%d].u32DataAddr malloc fail, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherOutBlock[i].u32DataAddr,0,0);
            return;
        }
        else
        {
            /*cipherDebugMsg("%s: cipherOutBlock[%d].u32DataAddr malloc sucess, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherOutBlock[i].u32DataAddr,0,0);*/
        }
        pMallocAddrBak[2][i] = cipherOutBlock[i].u32DataAddr;
    }
    
    for(i = 0; i < CIPHER_MAX_CHN; i++)
    {
        cipherChnCbSem[i] = NULL;
        cipherChnCbSem[i] = semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);
        if(cipherChnCbSem[i] == NULL)
        {
            cipherDebugMsg("Sem create faile,ret\n",0,0,0,0,0);
            return;            
        }
    }   
    s32Ret = cipher_init();
    if(s32Ret != CIPHER_SUCCESS)
    {
        cipherDebugMsg("CIPHER_Init faile,ret:0x%x\n",s32Ret,0,0,0,0);
    }

    s32Ret = BSP_CIPHER_RegistNotifyCB(cipherCbFunc);
    if(s32Ret != BSP_OK)
    {
        cipherDebugMsg("BSP_CIPHER_RegistNotifyCB faile,ret:0x%x\n",s32Ret,0,0,0,0);
    }

    for(i = CIPHER_MIN_CHN; i < CIPHER_MAX_CHN; i++)
    {
        s32Ret = BSP_CIPHER_RegistFreeMemCB(i, 0, cipherInBufFree);
        if(s32Ret != BSP_OK)
        {
            cipherDebugMsg("BSP_CIPHER_RegistNotifyCB faile,ret:0x%x\n",s32Ret,0,0,0,0);
        }

        s32Ret = BSP_CIPHER_RegistFreeMemCB(i, 1, cipherOutBufFree);
        if(s32Ret != BSP_OK)
        {
            cipherDebugMsg("BSP_CIPHER_RegistNotifyCB faile,ret:0x%x\n",s32Ret,0,0,0,0);
        }
    }

    for(i = CIPHER_MIN_CHN; i < CIPHER_MAX_CHN; i++)
    {
        s32Ret = BSP_CIPHER_SetDataBufPara(i, 0x0, 0x0, 0x4, 0x8);
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SetDataBufPara faile,ret:0x%x\n",s32Ret,0,0,0,0);
        }

        s32Ret = BSP_CIPHER_SetDataBufPara(i, 0x1, 0x0, 0x4, 0x8);
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SetDataBufPara faile,ret:0x%x\n",s32Ret,0,0,0,0);
        }
    }
    
/*    g_ftp_record_file_is_created = BSP_FALSE;*/
    g_cipher_test_inited = BSP_TRUE;
}

static void cipherCbFunc(BSP_U32 u32ChNum, BSP_U32 u32SourAddr, BSP_U32 u32DestAddr,
               CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private)
{
    g_notify_stat[u32ChNum] = enStatus;
    
    if(enStatus == CIPHER_STAT_CHECK_ERR)
    {
           g_check_err[u32ChNum]++;
    }
    semGive(cipherChnCbSem[u32ChNum]);
/*
    logMsg("u32ChNum = 0x%x, u32SourAddr = 0x%x, u32DestAddr = 0x%x, enStatus = 0x%x, u32Private = 0x%x\n",
                   u32ChNum, u32SourAddr, u32DestAddr, enStatus, u32Private, 0);
    logMsg("tick:%d-chn(%d)-optId(%d)-status(%d)-transErr(%d)-checkErr(%d)\n",tickGet(),u32ChNum,u32OptId,enStatus,g_trans_err[u32ChNum],g_check_err[u32ChNum]);
*/
}

static void cipherCbFunc_forMulti(BSP_U32 u32ChNum, BSP_U32 u32SourAddr, BSP_U32 u32DestAddr,
               CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private)
{
     g_notify_stat[u32ChNum] = enStatus;
       
     if(enStatus == CIPHER_STAT_CHECK_ERR)
     {
            g_check_err[u32ChNum]++;
     }

     if((s32LoopforMulti >= 0x11)||(s32LoopforMulti == 0))
     {
         semGive(cipherChnCbSem[u32ChNum]);
     }
     s32LoopforMulti++;
}


static void cipherInBufFree(BSP_VOID* pMemAddr)
{   

   g_FreeInMem_Num++;
    
    /*logMsg("tick:%d-chn(%d)-optId(%d)-status(%d)-transErr(%d)-checkErr(%d)\n",tickGet(),u32ChNum,u32OptId,enStatus,g_trans_err[u32ChNum],g_check_err[u32ChNum]);*/
}

static void cipherOutBufFree(BSP_VOID* pMemAddr)
{   

   g_FreeOutMem_Num++;
    
    /*logMsg("tick:%d-chn(%d)-optId(%d)-status(%d)-transErr(%d)-checkErr(%d)\n",tickGet(),u32ChNum,u32OptId,enStatus,g_trans_err[u32ChNum],g_check_err[u32ChNum]);*/
}

static BSP_S32 cipher_memcmp_ext(const void* mem1,const void* mem2,int size)
{
	BSP_S32 i = 0;

	for(i = 0; i < size; i++)
	{
		if(*((char*)mem1+i) != *((char*)mem2+i))
		{
			logMsg("diff_pos:%d,addr1(0x%x):%x,addr2(0x%x):%x\n",i,(int)((char*)mem1+i),(int)*((char*)mem1+i),(int)((char*)mem2+i),(int)*((char*)mem2+i),0);
			return BSP_ERROR;
		}
	}
	return BSP_OK;
}

static BSP_S32 cipherDataCmp(CIPHER_DATA_BLOCK* pBlk1,CIPHER_DATA_BLOCK* pBlk2)
{
    BSP_S32 s32CmpResult = BSP_ERROR;

    CIPHER_DATA_BLOCK* pTempBlk1 = pBlk1;
    CIPHER_DATA_BLOCK* pTempBlk2 = pBlk2;

    if((pBlk1 == NULL)||(pBlk2 == NULL))
    {
        return BSP_ERROR;
    }
    
    do
    {
        s32CmpResult = cipher_memcmp_ext((void*)pTempBlk1->u32DataAddr,(void*)pTempBlk2->u32DataAddr,pTempBlk1->u32DataLen);
        if(s32CmpResult != BSP_OK)
        {
            return BSP_ERROR;
        }
        else
        {
            pTempBlk1 = (CIPHER_DATA_BLOCK*)pTempBlk1->pNextBlock;
            pTempBlk2 = (CIPHER_DATA_BLOCK*)pTempBlk2->pNextBlock;
        }
    }while((pTempBlk1 != BSP_NULL) && (pTempBlk2!=BSP_NULL));
    
    return BSP_OK;
}

static BSP_S32 build_blk_chain(CIPHER_DATA_BLOCK* pCipherInBlock,CIPHER_DATA_BLOCK CipherBlkChain[],BSP_U32 u32TestLen,BSP_U32 blk_num,BSP_U32 blk_len[])
{
    BSP_U32 total_len = 0;
    BSP_S32 i;
    BSP_U32 tmpAddr = 0;

    for(i = 0; i <(BSP_S32) blk_num; i++)
    {
        total_len += blk_len[i];
    }
    if(u32TestLen != total_len)
    {
        cipherDebugMsg("chain total len(%d) not equal to test len(%d).\n", total_len, u32TestLen, 0, 0,0);
        return BSP_ERROR;
    }
    tmpAddr = pCipherInBlock->u32DataAddr;
    for(i = 0; i <(BSP_S32) blk_num; i++)
    {
        CipherBlkChain[i].u32DataAddr = tmpAddr;
        CipherBlkChain[i].u32DataLen  = blk_len[i];
        if(i == (BSP_S32)(blk_num -1))
        {
             CipherBlkChain[i].pNextBlock = 0;
        }
        else
        {
           CipherBlkChain[i].pNextBlock  = (BSP_U32)&CipherBlkChain[i+1];
           tmpAddr += blk_len[i];
        }        
    }
    return BSP_OK;
}


static BSP_S32 chain_encrypt_decrypt_multitype(BSP_U8* caseName,BSP_U32 chnid,
        BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],CIPHER_ALGORITHM_E alg,
                     CIPHER_KEY_LEN_E keyLen, CIPHER_SUBMIT_TYPE_E enSubmit)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;

    CIPHER_DATA_BLOCK* blkChain  = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
                        18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }    

    /*cipherTestInit();*/

    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = 0;

    blkChain  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    blkChain2 = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    
    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = dataTestLen;
    cipherOutBlock[chnid].u32DataLen = dataTestLen;
  
    cipherTmpBlock[chnid].pNextBlock = 0;
    cipherOutBlock[chnid].pNextBlock = 0;
#if 1   
    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);
#endif
    for(i = 0; i <(BSP_S32)cipherInBlock[chnid].u32DataLen; i++)
    {
        *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = (BSP_U8)i;
    }

    s32Ret = BSP_CIPHER_SetKey(u8Key, keyLen, u32KeyIndex);
    if(s32Ret != CIPHER_SUCCESS)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey fail,ret:0x%x\n",s32Ret,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }

    /*完成上行业务*/
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_ENCRYPT;
    g_cipher_uplink_cfg[chnid].u8BearId = 0;/*5bit*/
    g_cipher_uplink_cfg[chnid].u8Direction = 0;/*0表示上行*/
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
     
    s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
             &(g_cipher_uplink_cfg[chnid]), enSubmit, u32Private);             

    if(enSubmit == CIPHER_SUBM_CALLBACK)
    {        
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
        semTake(cipherChnCbSem[chnid],WAIT_FOREVER);
    }
    else if(enSubmit == CIPHER_SUBM_NONE)
    {
        /*taskDelay(5);*/
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }    
    }
    else
    {
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }    
    } 

    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_OPT_DECRYPT;
    g_cipher_downlink_cfg[chnid].u8BearId = 0;/*5bit*/
    g_cipher_downlink_cfg[chnid].u8Direction = 0;/*0表示上行*/
    g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].u32Count = 0;
    g_cipher_downlink_cfg[chnid].bMemBlock = 0;
    g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
    
    /*构造加密后的数据链表*/
    s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    g_notify_stat[chnid] = CIPHER_STAT_OK;
        
    s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain2,&cipherOutBlock[chnid],
             &(g_cipher_downlink_cfg[chnid]),enSubmit, u32Private);


    if(enSubmit == CIPHER_SUBM_CALLBACK)
    {        
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
        semTake(cipherChnCbSem[chnid],WAIT_FOREVER);
    }
#if 1
    else if(enSubmit == CIPHER_SUBM_NONE)
    {
        /*taskDelay(5);*/
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }    
    }
#endif
    else
    {
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }  
    }

    if(g_notify_stat[chnid] != CIPHER_STAT_OK)
    {
        cipherDebugMsg("CIPHER_STAT:%x\n",g_notify_stat[chnid],0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;        
    } 
#if 1
    if((enSubmit == CIPHER_SUBM_CALLBACK) || (enSubmit == CIPHER_SUBM_BLK_HOLD))
    {
        s32Ret = cipherDataCmp(&cipherInBlock[chnid],&cipherOutBlock[chnid]);
        if(s32Ret != BSP_OK)
        {
            cipherDebugMsg("data cmp fail.\n",0,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
    }
#endif
clearup:

    free(blkChain);
    free(blkChain2);
    if(s32TestStatus == BSP_OK)
    {
        /*cipherDebugMsg("test pass.\n",0,0,0,0,0);*/
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

static BSP_S32 chain_encrypt_decrypt_multiTask(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen, CIPHER_SUBMIT_TYPE_E enSubmit)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;

    CIPHER_DATA_BLOCK* blkChain  = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;

    /*cipherTestInit();*/

    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = 0;

    blkChain  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    blkChain2 = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    
    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = dataTestLen;
    cipherOutBlock[chnid].u32DataLen = dataTestLen;
  
    cipherTmpBlock[chnid].pNextBlock = 0;
    cipherOutBlock[chnid].pNextBlock = 0;
#if 1   
    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);
#endif
    for(i = 0; i < (BSP_S32)cipherInBlock[chnid].u32DataLen; i++)
    {
        *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = (BSP_U8)i;
    }

    /*完成上行业务*/
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_ENCRYPT;
    g_cipher_uplink_cfg[chnid].u8BearId = 0;/*5bit*/
    g_cipher_uplink_cfg[chnid].u8Direction = 0;/*0表示上行*/
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
     
    s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
             &(g_cipher_uplink_cfg[chnid]), enSubmit, u32Private);             

    if(enSubmit == CIPHER_SUBM_CALLBACK)
    {        
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
        semTake(cipherChnCbSem[chnid],WAIT_FOREVER);
    }
    else
    {
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }    
    }

    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_OPT_DECRYPT;
    g_cipher_downlink_cfg[chnid].u8BearId = 0;/*5bit*/
    g_cipher_downlink_cfg[chnid].u8Direction = 0;/*0表示上行*/
    g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].u32Count = 0;
    g_cipher_downlink_cfg[chnid].bMemBlock = 0;
    g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
    
    /*构造加密后的数据链表*/
    s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    g_notify_stat[chnid] = CIPHER_STAT_OK;
        
    s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain2,&cipherOutBlock[chnid],
             &(g_cipher_downlink_cfg[chnid]), enSubmit, u32Private);

    if(enSubmit == CIPHER_SUBM_CALLBACK)
    {        
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
        semTake(cipherChnCbSem[chnid],WAIT_FOREVER);
    }
    else
    {
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }    
    }

    if(g_notify_stat[chnid] != CIPHER_STAT_OK)
    {
        cipherDebugMsg("CIPHER_STAT:%x\n",g_notify_stat[chnid],0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;        
    } 
    
    s32Ret = cipherDataCmp(&cipherInBlock[chnid],&cipherOutBlock[chnid]);
    if(s32Ret != BSP_OK)
    {
        cipherDebugMsg("data cmp fail.\n",0,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }

clearup:

    free(blkChain);
    free(blkChain2);
    if(s32TestStatus == BSP_OK)
    {
        /*cipherDebugMsg("test pass.\n",0,0,0,0,0);*/
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

static void cipher_hold_taskEntry_longtime_bus_stress(BSP_BOOL* pbDone)
{
	BSP_U32 blk_num = 1;
	BSP_U32 blk_len[] = {1500};
	BSP_U32 dataTestLen = 1500;
	BSP_S32 s32Ret;

	while(get_test_switch_stat(CIPHER))
	{
		if (semTake(cipher_send_sem, WAIT_FOREVER) == ERROR)
		{
			logMsg(">>>line %d FUNC %s fail\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
			break;
		}
		s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
		    dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, CIPHER_SUBM_BLK_HOLD);
		if(BSP_OK != s32Ret)
		{
			cipherDebugMsg("cipher_hold_taskEntry_longtime_bus_stress fail,ret:0x%x\n",s32Ret,0,0,0,0);
			break;
		}
	}
	*pbDone = BSP_TRUE;   
}

static void cipher_callback_taskEntry_longtime_bus_stress(BSP_U32 chnid,BSP_U32 test_times,BSP_BOOL* pbDone,
                                                  CIPHER_SUBMIT_TYPE_E enSubmit)
{
	BSP_U32 blk_num = 1;
	BSP_U32 blk_len[] = {1500};
	BSP_U32 dataTestLen = 1500;
	BSP_S32 s32Ret;

	while(get_test_switch_stat(CIPHER))
	{
		if (semTake(cipher_send_sem, WAIT_FOREVER) == ERROR)
		{
			logMsg(">>>line %d FUNC %s fail\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
			break;
		}
		s32Ret = chain_encrypt_decrypt_multiTask((BSP_U8*)__FUNCTION__, chnid, dataTestLen,blk_num,blk_len,
		                CIPHER_ALG_AES_128, CIPHER_KEY_L128, enSubmit);
		if(BSP_OK != s32Ret)
		{
		    cipherDebugMsg("cipher_callback_taskEntry_longtime_bus_stress fail,ret:0x%x\n",s32Ret,0,0,0,0);
		    break;
		}	    
	}
	*pbDone = BSP_TRUE;   	
}

static void cipher_timer_event(BSP_U32 param)
{
	if(NULL != cipher_send_sem)
	{
		semGive(cipher_send_sem);
	}
	bsp_softtimer_add(&sft_timer_list);
}

BSP_S32 cipher_stress_test_start(BSP_S32 task_priority,BSP_S32 test_rate)
{
	BSP_S32 i = 0,j = 0;
	int    taskId[3] = {0};
	int    test_times = 1000;
	BSP_S32 testStatus = BSP_OK;
	BSP_BOOL bDone[] = {BSP_FALSE,BSP_FALSE,BSP_FALSE};
	BSP_U32 chnid[] = {PDCP_SIGNAL_CHN, PDCP_DATA_CHN_1, PDCP_DATA_CHN_2};
	BSP_U8* taskName[] = {(BSP_U8*)"ltask1",(BSP_U8*)"ltask2",(BSP_U8*)"ltask3"};
	CIPHER_SUBMIT_TYPE_E enSubmit = CIPHER_SUBM_CALLBACK;
	sft_timer_list.func = cipher_timer_event;
	sft_timer_list.para = 0;
	sft_timer_list.timeout = test_rate;
	sft_timer_list.wake_type = SOFTTIMER_WAKE;

	set_test_switch_stat(CIPHER, TEST_RUN);
   
	/*创建控制发送速率的信号量*/
	cipher_send_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	if (cipher_send_sem == NULL)
	{
		printf("semBCreate cipher_send_sem fail.\n");
		return BSP_ERROR; 
	} 

	/*创建控制发送速率 的软timer*/
	if(bsp_softtimer_create(&sft_timer_list))
	{
		printf("SOFTTIMER_CREATE_TIMER fail.\n");
		semDelete(cipher_send_sem);
		return BSP_ERROR; 
	}
	
	if(!g_cipher_test_inited)
	{
	   cipherTestInit();
	}

	for(i = 0; i < 0x3; i++)
	{
		if(0x0 == i)
		{
			taskId[i] = taskSpawn((char*)taskName[i], (int)task_priority, (int)VX_FP_TASK,
			            20000, (FUNCPTR)cipher_hold_taskEntry_longtime_bus_stress,
			                                        (int)&bDone[i], 0,0,0, 0, 0, 0, 0, 0, 0);
		}

		if((1 == i) || (2 == i))
		{
			taskId[i] = taskSpawn((char*)taskName[i], (int)task_priority, (int)VX_FP_TASK,
			            20000, (FUNCPTR)cipher_callback_taskEntry_longtime_bus_stress, 
			                     (int)chnid[i],(int)test_times, (int)&bDone[i], (int)enSubmit, 0, 0, 0, 0, 0, 0);
		}


		if(taskId[i] == 0)
		{
			testStatus = BSP_ERROR;
			for(j = 0; j < i; j++)
			{
			 	taskDelete(taskId[j]);
			}          
		}
	}
	bsp_softtimer_add(&sft_timer_list);
	return testStatus;
}

BSP_S32 cipher_stress_test_stop()
{
	BSP_U8* taskName[] = {(BSP_U8*)"ltask1",(BSP_U8*)"ltask2",(BSP_U8*)"ltask3"};
	BSP_S32 taskId = ERROR;
	BSP_S32 i = 0;

	set_test_switch_stat(CIPHER, TEST_STOP);
	
	for(i = 0; i < 3; i++)
	{
		taskId = taskNameToId((char*)taskName[i]);
		while(BSP_OK == taskIdVerify(taskId))
		{
			taskDelay(100);
		}
	}

	if (NULL != cipher_send_sem)
	{
		semDelete(cipher_send_sem);
	}

	if(sft_timer_list.init_flags == TIMER_INIT_FLAG)
	{
		bsp_softtimer_delete_sync(&sft_timer_list);
		bsp_softtimer_free(&sft_timer_list);
	}
	return OK;	
}

/***********************************************************
KDF stress test code
************************************************************/
#define KEY_TOTAL_SIZE (32*16)
#define KDF_SHA_S_LENGTH      0x7
#define KDF_DESTKEY_OFF       0x10
#define KDF_DESTKEY_LENGTH    0x10

static BSP_U8 u32GoldenKey[KDF_SHA_KEY_LENGTH] = 
{
    0xbd, 0xef, 0x4e, 0xd7, 0x1a, 0x1d, 0x3a, 0x15, 
    0x00, 0x90, 0x4d, 0x83, 0xa5, 0xe8, 0x32, 0xf7, 
    0xe3, 0xe0, 0xfb, 0x1c, 0x43, 0x07, 0xcd, 0x94, 
    0xc6, 0xe6, 0x2d, 0x6a, 0x63, 0x7b, 0x76, 0x41 
};

static BSP_U8 u32GoldenS1[KDF_SHA_S_LENGTH] = 
{
    0x15,0x05,0x00,0x01,0x02,0x00,0x01
};
#if 0
static BSP_U8 u32GoldenS2[KDF_SHA_S_LENGTH] = 
{
    0x15,0x04,0x00,0x01,0x02,0x00,0x01
};

static BSP_U8 u32GoldenS3[KDF_SHA_S_LENGTH] = 
{
    0x15,0x03,0x00,0x01,0x02,0x00,0x01
};
#endif
static BSP_U8 u32GoldenResult1[KDF_DESTKEY_LENGTH] = 
{
    0xf8,0xc0,0x2a,0x2f,0x03,0x11,0x02,0x2f, 
    0x8d,0x37,0xec,0xe9,0xf2,0x0c,0x07,0x11
};

static BSP_S32 g_Kdf_TaskId = ERROR;
static struct softtimer_list kdf_timer;
static SEM_ID kdf_send_sem = (SEM_ID)NULL;

static BSP_S32 kdf_stress_routine()
{
	BSP_S32 i;
	BSP_S32 s32Ret = BSP_OK;
	BSP_VOID *pDestAddr;
	BSP_VOID *pTempAddr = BSP_NULL;
	BSP_U32 u32Length = 0;
	BSP_U8 u8Key[32] = {0};

	KEY_CONFIG_INFO_S stKeyInfo = {0,0,0};
	S_CONFIG_INFO_S   stSInfo   = {0,0,0,0};
	KEY_MAKE_S stKeyMake        = {0,{0,0,0}};
	BSP_U32 u32DIndex           = 0x3;

	stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_KEYRAM;
	stKeyInfo.u32ShaKeyIndex = 0x0;
	stKeyInfo.pKeySourceAddr = BSP_NULL;

	stSInfo.enShaSSource = SHA_S_SOURCE_KEYRAM;
	stSInfo.u32ShaSIndex = 0x1;
	stSInfo.u32ShaSLength = 0x7;
	stSInfo.pSAddr       = BSP_NULL;

	stKeyMake.enKeyOutput = CIPHER_KEY_OUTPUT;
	stKeyMake.stKeyGet.enKeyLen = CIPHER_KEY_L256;
	stKeyMake.stKeyGet.penOutKeyLen = &u32Length;
	stKeyMake.stKeyGet.pKeyAddr     = (BSP_VOID *)u8Key;


	pTempAddr = (BSP_VOID *)CIPHER_MALLOC(KDF_DESTKEY_LENGTH);

	pDestAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + \
	             CIPHER_KEYRAM_OFFSET +
	             u32DIndex * CIPHER_KEY_LEN + KDF_DESTKEY_OFF);

	for(i=0; i<KDF_SHA_KEY_LENGTH/4; i++)
	{        
	    *((BSP_U32 *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stKeyInfo.u32ShaKeyIndex) * CIPHER_KEY_LEN + 4*i))  \
	    = (((BSP_U32)u32GoldenKey[4*i])<<24) | (((BSP_U32)u32GoldenKey[4*i+1])<<16) \
	      | (((BSP_U32)u32GoldenKey[4*i+2])<<8) | ((BSP_U32)u32GoldenKey[4*i+3]);
	}

	for(i=0; i<KDF_SHA_S_LENGTH/4; i++)
	{
	    *((BSP_U32 *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stSInfo.u32ShaSIndex) * CIPHER_KEY_LEN + 4*i))  \
	        = (((BSP_U32)u32GoldenS1[4*i])<<24) | (((BSP_U32)u32GoldenS1[4*i+1])<<16) \
	        | (((BSP_U32)u32GoldenS1[4*i+2])<<8) | ((BSP_U32)u32GoldenS1[4*i+3]);
	}
	if((KDF_SHA_S_LENGTH%4)==1)
	{
	    *((BSP_U32 *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stSInfo.u32ShaSIndex) * CIPHER_KEY_LEN + 4*i))  \
	        = (((BSP_U32)u32GoldenS1[4*i])<<24);
	}
	if((KDF_SHA_S_LENGTH%4)==2)
	{
	    *((BSP_U32 *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stSInfo.u32ShaSIndex) * CIPHER_KEY_LEN + 4*i))  \
	        = (((BSP_U32)u32GoldenS1[4*i])<<24) | (((BSP_U32)u32GoldenS1[4*i+1])<<16);
	}
	if((KDF_SHA_S_LENGTH%4)==3)
	{
	    *((BSP_U32 *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stSInfo.u32ShaSIndex) * CIPHER_KEY_LEN + 4*i))  \
	        = (((BSP_U32)u32GoldenS1[4*i])<<24) | (((BSP_U32)u32GoldenS1[4*i+1])<<16) | (((BSP_U32)u32GoldenS1[4*i+2])<<8);
	}

	while(get_test_switch_stat(KDF))
	{
		if (semTake(kdf_send_sem, WAIT_FOREVER) == ERROR)
		{
			logMsg(">>>line %d FUNC %s fail\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
			break;
		}
		
		s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
		if(BSP_OK != s32Ret)
		{
		    printf("BSP_KDF_KeyMake ERROR! \r\n");
		    CIPHER_FREE(pTempAddr);
		    return BSP_ERROR;
		}

		s32Ret = cipher_memcmp_ext((void *)(&u8Key[16]), \
		                           (void *)u32GoldenResult1,\
		                           (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));

		if(BSP_OK != s32Ret)
		{
		    printf("cipher_memcmp_ext Error\r\n");
		    CIPHER_FREE(pTempAddr);
		    return s32Ret;
		}
	}
	CIPHER_FREE(pTempAddr);
	return s32Ret;   
}


static void kdf_timer_event(BSP_U32 param)
{
	if(NULL != kdf_send_sem)
	{
		semGive(kdf_send_sem);
	}
	bsp_softtimer_add(&kdf_timer);
}
BSP_S32 kdf_stress_test_start(BSP_S32 task_priority,BSP_S32 test_rate)
{
	kdf_timer.func = kdf_timer_event;
	kdf_timer.para = 0;
	kdf_timer.timeout = test_rate;
	kdf_timer.wake_type = SOFTTIMER_WAKE;

	set_test_switch_stat(KDF, TEST_RUN);

	/*创建控制发送速率的信号量*/
	kdf_send_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	if (kdf_send_sem == NULL)
	{
		printf("semBCreate cipher_send_sem fail.\n");
		return BSP_ERROR; 
	} 

	/*创建控制发送速率 的软timer*/
	if(bsp_softtimer_create(&kdf_timer))
	{
		printf("SOFTTIMER_CREATE_TIMER fail.\n");
		semDelete(kdf_send_sem);
		return BSP_ERROR; 
	}
	
	cipherTestInit();
	
	g_Kdf_TaskId = taskSpawn("tKdfTask", task_priority, 0, 20000, (FUNCPTR)kdf_stress_routine, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if(0 == g_Kdf_TaskId)		
	{
		return ERROR;
	}

	bsp_softtimer_add(&kdf_timer);
	return OK;
}

BSP_S32 kdf_stress_test_stop()
{
	set_test_switch_stat(KDF, TEST_STOP);
	while(OK == taskIdVerify(g_Kdf_TaskId))
	{
		taskDelay(100);
	}

	if (NULL != kdf_send_sem)
	{
		semDelete(kdf_send_sem);
	}

	if( kdf_timer.init_flags == TIMER_INIT_FLAG)
	{
		bsp_softtimer_delete_sync(&kdf_timer);
		bsp_softtimer_free(&kdf_timer);
	}
	return OK;	
}
