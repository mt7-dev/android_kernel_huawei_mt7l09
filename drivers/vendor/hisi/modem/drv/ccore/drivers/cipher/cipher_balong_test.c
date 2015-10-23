/*************************************************************************
*   版权所有(C) 2008-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  cipher_balong_test.h
*
*   作    者 :  wangxiandong
*
*   描    述 :  cipher功能实现头文件
*
*   修改记录 :  2013年03月12日  v1.00  wangxiandong 修改
*************************************************************************/
#include <vxWorks.h>
#include <semLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <cacheLib.h>
#include "product_config.h"
#include "cipher_balong_common.h"
#include "drv_cipher.h"
#include "kdf_balong.h"
#include "acc_balong.h"



static BSP_U8 u8HerderLenTbl[CIPHER_HDR_BIT_TYPE_BUTT] = {0,1,1,2,1,1,2};
static BSP_U8 u8APHLenTbl[CIPHER_HDR_BIT_TYPE_BUTT] = {0,1,1,2,1,1,2};

#define CIPHER_DEBUG_ENABLE (1)
/*
 * CIPHER_STRESS_TEST设置为0时，多通道并发测试会遍历所有cipher操作(除DMA外)，用于获取性能数据
 * CIPHER_STRESS_TEST设置为1时，多通道并发只测试callback方式提交，而且数据块变为60KB，同时不打印日志，用于压力测试
 * 该配置下专门用于总线压力测试
 * */
#define CIPHER_STRESS_TEST  (0)

#define cipherDebugMsg(str,p0,p1,p2,p3,p4) logMsg("TEST-(line:%d)"str,__LINE__,p0,p1,p2,p3,p4)
        
#define NAS_CHN             (0x1)
#define PDCP_SIGNAL_CHN     (1)
#define PDCP_DATA_CHN_1     (2)
#define PDCP_DATA_CHN_2     (3)

#define CHAIN_MAX_BLOCK_NUM          (4)

#define MAC_SIZE_SHORT               (2)
#define MAC_SIZE_LONG                (4)
#define MAC_SIZE                    (4)

/*#define IP_SIZE                    (2)*/
#define SEQNUM_SIZE                (1)
/*#define IP_SIZE(i)                   (u8HerderLenTbl(i))*/
#define PH_SIZE                      (1)
#define MIN_CALLBACK_OPT_ID          (1)  

/*#define BLOCK_MAX_SIZE               (64*1024 + IP_SIZE + 20)*/
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

typedef struct targ_cipher_ass_op_pdcp_arg
{
    CIHPER_RELA_CFG_S stRelaCfg;
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num;
    BSP_U32 *blk_len;
    BSP_U32 dataTestLen;
    CIPHER_KEY_LEN_E keyLen;
    CIPHER_HDR_E enHeaderLength; 
}CIPHER_ASS_OP_PDCP_ARG;

static BSP_BOOL   g_cipher_test_inited = BSP_FALSE;
static CIPHER_DATA_BLOCK    cipherInBlock[CIPHER_MAX_CHN];
static CIPHER_DATA_BLOCK    cipherTmpBlock[CIPHER_MAX_CHN];
static CIPHER_DATA_BLOCK    cipherOutBlock[CIPHER_MAX_CHN];
static SEM_ID               cipherChnCbSem[CIPHER_MAX_CHN];
static CIPHER_NOTIFY_STAT_E g_notify_stat[CIPHER_MAX_CHN] = {0};
static CIHPER_SINGLE_CFG_S         g_cipher_uplink_cfg[CIPHER_MAX_CHN];
static CIHPER_SINGLE_CFG_S         g_cipher_downlink_cfg[CIPHER_MAX_CHN];
/*CIHPER_SUBM_ATTR_S   g_subm_attr;*/
#if 0
static BSP_U8               g_iv[16] = {
                     0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
                     };
#endif
static BSP_U8               g_key[32] = {
                     0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
                     0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                     0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f                     
                     };
#if 0
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
static FILE*   g_file_id[CIPHER_MAX_CHN] = {0};
static BSP_BOOL g_bUseFileSaveResult = BSP_FALSE;
static BSP_U8* g_chnName[] = {(BSP_U8*)"NAS",(BSP_U8*)"PDCP_SIGNAL",(BSP_U8*)"PDCP_DATA_1",(BSP_U8*)"PDCP_DATA_2"};
static BSP_U8* g_resultFileName[] = {(BSP_U8*)"chn_nas_test_result.txt",
                              (BSP_U8*)"chn_pdcp_signal_test_result.txt",
                              (BSP_U8*)"chn_pdcp_data_1_test_result.txt",
                              (BSP_U8*)"chn_pdcp_data_2_test_result.txt"};
static BSP_BOOL g_bEnableFluxTest = BSP_FALSE;
static BSP_U8*  g_linkTypeName[] = {(BSP_U8*)"UPLINK",(BSP_U8*)"DOWNLINK"};
static BSP_BOOL g_bCheckOptIdPass[CIPHER_MAX_CHN] = {BSP_TRUE,BSP_TRUE,BSP_TRUE,BSP_TRUE};
static BSP_S32 nas_task_id = 0;
static BSP_S32 pdcp_signal_task_id = 0;
static BSP_S32 pdcp_data_1_task_id = 0;
static BSP_S32 pdcp_data_2_task_id = 0;
static BSP_BOOL g_bPurgeTestTaskDone = BSP_FALSE;
static BSP_BOOL g_bEnablePurgeStubTest = BSP_FALSE;
static BSP_BOOL g_local_purge_test = 0;
/*PURGE_TEST_STRUCT purgeTest;*/
static BSP_S32 nas_purge_test_task_id = 0;
static BSP_S32 pdcp_signal_purge_test_task_id = 0;
static BSP_S32 pdcp_data_1_purge_test_task_id = 0;
static BSP_S32 pdcp_data_2_purge_test_task_id = 0;
static BSP_U32 g_u32TickStart[CIPHER_MAX_CHN][2]      = {{0,0},{0,0},{0,0},{0,0}};
static BSP_U32 g_u32TickEnd[CIPHER_MAX_CHN][2]        = {{0},{0},{0},{0}};
static BSP_U32 g_u32RealTotalTimes[CIPHER_MAX_CHN][2] = {{0},{0},{0},{0}};
static double  g_dRate[CIPHER_MAX_CHN][2]             = {{0,0},{0,0,},{0,0},{0,0}};
static BSP_U32 g_test_times[2]                        = {0,0};
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
/*BSP_BOOL g_ftp_record_file_is_created = BSP_FALSE;*/

#if CIPHER_DEBUG_ENABLE
static BSP_U32 pMallocAddrBak[3][CIPHER_MAX_CHN];
#endif
/*
extern void BSP_CIPHER_SetPurgeUTStat(BSP_U32 u32UtStat);
extern VOID BSP_CIPHER_SetStrmThreshhld(BSP_U32 u32Threshhld);
extern int sysClkRateGet (void);
void cipherTestDeInit();
*/
void cipherCbFunc(BSP_U32 u32ChNum, BSP_U32 u32SourAddr, BSP_U32 u32DestAddr,
               CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private);
/*
void perf_test_entry_1(BSP_U32 chnid,BSP_BOOL bUseFtpFile);
void perf_test_entry_2(BSP_U32 chnid,BSP_BOOL bUseFtpFile);
*/
void cipherInBufFree(BSP_VOID* pMemAddr);
void cipherOutBufFree(BSP_VOID* pMemAddr);

void cipherTestInit()
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
/*            cipherDebugMsg("%s: cipherInBlock[%d].u32DataAddr malloc sucess, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherInBlock[i].u32DataAddr,0,0);*/
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
/*            cipherDebugMsg("%s: cipherTmpBlock[%d].u32DataAddr malloc sucess, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherTmpBlock[i].u32DataAddr,0,0);*/
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
/*            cipherDebugMsg("%s: cipherOutBlock[%d].u32DataAddr malloc sucess, addr:0x%x\n",(BSP_S32)__FUNCTION__,i,cipherOutBlock[i].u32DataAddr,0,0);*/
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
        cipherDebugMsg("cipher_init faile,ret:0x%x\n",s32Ret,0,0,0,0);
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

void cipherCbFunc(BSP_U32 u32ChNum, BSP_U32 u32SourAddr, BSP_U32 u32DestAddr,
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

void cipherCbFunc_forMulti(BSP_U32 u32ChNum, BSP_U32 u32SourAddr, BSP_U32 u32DestAddr,
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


void cipherInBufFree(BSP_VOID* pMemAddr)
{   

   g_FreeInMem_Num++;
    
    /*logMsg("tick:%d-chn(%d)-optId(%d)-status(%d)-transErr(%d)-checkErr(%d)\n",tickGet(),u32ChNum,u32OptId,enStatus,g_trans_err[u32ChNum],g_check_err[u32ChNum]);*/
}

void cipherOutBufFree(BSP_VOID* pMemAddr)
{   

   g_FreeOutMem_Num++;
    
    /*logMsg("tick:%d-chn(%d)-optId(%d)-status(%d)-transErr(%d)-checkErr(%d)\n",tickGet(),u32ChNum,u32OptId,enStatus,g_trans_err[u32ChNum],g_check_err[u32ChNum]);*/
}

BSP_S32 cipher_memcmp_ext(const void* mem1,const void* mem2,int size)
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

BSP_S32 cipherDataCmp(CIPHER_DATA_BLOCK* pBlk1,CIPHER_DATA_BLOCK* pBlk2)
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
    }while(/*(pTempBlk1 == BSP_NULL) ||(pTempBlk2==BSP_NULL) modified by wangxiandong*/(pTempBlk1 != BSP_NULL) && (pTempBlk2!=BSP_NULL));
    
    return BSP_OK;
}

BSP_S32 build_blk_chain(CIPHER_DATA_BLOCK* pCipherInBlock,CIPHER_DATA_BLOCK CipherBlkChain[],BSP_U32 u32TestLen,BSP_U32 blk_num,BSP_U32 blk_len[])
{
    BSP_U32 total_len = 0;
    BSP_S32 i;
    BSP_U32 tmpAddr = 0;

    for(i = 0; i < (BSP_S32)blk_num; i++)
    {
        total_len += blk_len[i];
    }
    if(u32TestLen != total_len)
    {
        cipherDebugMsg("chain total len(%d) not equal to test len(%d).\n", total_len, u32TestLen, 0, 0,0);
        return BSP_ERROR;
    }
    tmpAddr = pCipherInBlock->u32DataAddr;
    for(i = 0; i < (BSP_S32)blk_num; i++)
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


BSP_S32 chain_protect_check_pdcp_longmac(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen, CIPHER_HDR_E enHeaderlen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;
    CIPHER_DATA_BLOCK* blkChain = NULL;
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    BSP_U32 ip_size = 0;
    CIPHER_SUBMIT_TYPE_E enSubmit;

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;

    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].pNextBlock = 0;

    ip_size = u8HerderLenTbl[enHeaderlen];
    
    cipherInBlock[chnid].u32DataLen = ip_size + dataTestLen;
    blk_len[0] += ip_size;

    blkChain = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));

    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen+ip_size,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = MAC_SIZE_LONG;
    cipherTmpBlock[chnid].pNextBlock = 0;

    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);

    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_PROTECT_LMAC;
    g_cipher_uplink_cfg[chnid].u8BearId = 0;
    g_cipher_uplink_cfg[chnid].u8Direction = 0;
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = enHeaderlen;
    g_cipher_uplink_cfg[chnid].u32Count = 0;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
                 &(g_cipher_uplink_cfg[chnid]),enSubmit, u32Private);
    }

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

    /*将完整性校验值拷贝入数据末尾*/
/*    memcpy((void*)(cipherInBlock[chnid].u32DataAddr + dataTestLen),(void*)(cipherTmpBlock[chnid].u32DataAddr),MAC_SIZE);*/

    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_OPT_CHECK_POST_LMAC;
    g_cipher_downlink_cfg[chnid].u8BearId = 0;
    g_cipher_downlink_cfg[chnid].u8Direction = 0;
    g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].enHeaderLen = enHeaderlen;
    g_cipher_downlink_cfg[chnid].u32Count = 0;
    g_cipher_downlink_cfg[chnid].bMemBlock = 0;
    g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      


    /*当输入为链表时，进行完整性检查*/
    memcpy((void*)(blkChain[blk_num-1].u32DataAddr + blkChain[blk_num-1].u32DataLen),
           (void*)cipherTmpBlock[chnid].u32DataAddr,
           MAC_SIZE_LONG);
    blkChain[blk_num-1].u32DataLen = blkChain[blk_num-1].u32DataLen + MAC_SIZE_LONG;

    g_notify_stat[chnid] = CIPHER_STAT_OK;

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,NULL,
                 &(g_cipher_downlink_cfg[chnid]),enSubmit, u32Private);   
    }
    
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

    blkChain[blk_num-1].u32DataLen = blkChain[blk_num-1].u32DataLen - MAC_SIZE_LONG;
    
    if(g_notify_stat[chnid] != CIPHER_STAT_OK)
    {
        cipherDebugMsg("CIPHER_STAT:%x\n",g_notify_stat[chnid],0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;        
    } 

clearup:
    free(blkChain);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

#if 0
BSP_S32 chain_nas_associate(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg,CIPHER_ALG_MODE_E algMode,CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;

    CIPHER_DATA_BLOCK* blkChain  = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;

    //cipherTestInit();

    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = NULL;

    blkChain  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    blkChain2 = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    
    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    if(g_InitParam.astEndFlag[chnid].bUseEndFlag)
    {
       cipherTmpBlock[chnid].u32DataLen = dataTestLen + MAC_SIZE + 1; 
       cipherOutBlock[chnid].u32DataLen = dataTestLen + 1;
    }
    else
    {
       cipherTmpBlock[chnid].u32DataLen = dataTestLen + MAC_SIZE;
       cipherOutBlock[chnid].u32DataLen = dataTestLen;
    }    
    cipherTmpBlock[chnid].pNextBlock = NULL;
    cipherOutBlock[chnid].pNextBlock = NULL;
#if 1   
    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);
#endif
    for(i = 0; i < cipherInBlock[chnid].u32DataLen; i++)
    {
        *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = (BSP_U8)i;
    }

    /*完成上行业务*/
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_NAS_ENCY_PRTCT;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stAlgInfo.bAlgRenew = BSP_TRUE;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stAlgInfo.enAlg = alg;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stAlgInfo.enAlgMode = algMode;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stKeyInfo.bKeyRenew = BSP_TRUE;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stKeyInfo.enKeyLen = keyLen;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stKeyInfo.pIvAddr = (BSP_U8*)&g_iv;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.stKeyInfo.pKeyAddr = (BSP_U8*)&g_key;    

    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stAlgInfo.bAlgRenew = BSP_TRUE;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stAlgInfo.enAlg = alg;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stAlgInfo.enAlgMode = algMode;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.bKeyRenew = BSP_TRUE;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.enKeyLen = keyLen;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.pIvAddr = (BSP_U8*)&g_iv;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.pKeyAddr = (BSP_U8*)&g_key; 
    
    g_subm_attr.enSubmType =  CIPHER_SUBM_CALLBACK;  
    
    s32Ret = BSP_CIPHER_SubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
             &(g_cipher_uplink_cfg[chnid]),&g_subm_attr);             
    if(s32Ret < MIN_CALLBACK_OPT_ID)
    {
        cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }
    
    semTake(cipherChnCbSem[chnid],WAIT_FOREVER); 
#if 1
    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_NAS_CHCK_DECY;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stAlgInfo.bAlgRenew = BSP_TRUE;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stAlgInfo.enAlg = alg;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stAlgInfo.enAlgMode = algMode;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stKeyInfo.bKeyRenew = BSP_TRUE;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stKeyInfo.enKeyLen = keyLen;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stKeyInfo.pIvAddr = (BSP_U8*)&g_iv;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.stKeyInfo.pKeyAddr = (BSP_U8*)&g_key;  

    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stAlgInfo.bAlgRenew = BSP_TRUE;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stAlgInfo.enAlg = alg;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stAlgInfo.enAlgMode = algMode;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.bKeyRenew = BSP_TRUE;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.enKeyLen = keyLen;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.pIvAddr = (BSP_U8*)&g_iv;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo2.stKeyInfo.pKeyAddr = (BSP_U8*)&g_key; 
    
    /*构造加密后的数据链表*/
    if(g_InitParam.astEndFlag[chnid].bUseEndFlag)
    {
        blk_len[blk_num - 1] += (MAC_SIZE+1);
    }
    else
    {
        blk_len[blk_num - 1] += MAC_SIZE;
    }
    s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,cipherTmpBlock[chnid].u32DataLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    g_subm_attr.enSubmType =  CIPHER_SUBM_CALLBACK;  

    g_notify_stat[chnid] = CIPHER_STAT_OK;
        
    s32Ret = BSP_CIPHER_SubmitTask(chnid,(void*)blkChain2,&cipherOutBlock[chnid],
             &(g_cipher_downlink_cfg[chnid]),&g_subm_attr);
    if(s32Ret < MIN_CALLBACK_OPT_ID)
    {
        cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }
   
    semTake(cipherChnCbSem[chnid],WAIT_FOREVER);

    if(g_notify_stat[chnid] != CIPHER_STAT_OK)
    {
        cipherDebugMsg("CIPHER_STAT:%x\n",g_notify_stat[chnid],0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;        
    } 
    
    if(g_InitParam.astEndFlag[chnid].bUseEndFlag)
    {
        cipherOutBlock[chnid].u32DataLen = dataTestLen;
        blk_len[blk_num - 1] -= (MAC_SIZE+1);
    }
    else
    {
        blk_len[blk_num - 1] -= MAC_SIZE;
    }
    
    s32Ret = cipherDataCmp(&cipherInBlock[chnid],&cipherOutBlock[chnid]);
    if(s32Ret != BSP_OK)
    {
        cipherDebugMsg("data cmp fail.\n",0,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }
#endif
clearup:

    free(blkChain);
    free(blkChain2);
    if(s32TestStatus == BSP_OK)
    {
        //cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}
#endif

#if 1
/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_128进行完整性保护，包头长度为1Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128, CIPHER_HDR_BIT_TYPE_5); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_128进行完整性保护，包头长度为1Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_002()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_128进行完整性保护，包头长度为2Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_003()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128, CIPHER_HDR_BIT_TYPE_12); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_128进行完整性保护，包头长度为2Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_004()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);  
}


/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，包头长度为1Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_005()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);                    
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，包头长度为1Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_001_006()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，包头长度为2Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_007()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);                    
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，包头长度为2Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_008()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_192进行完整性保护，包头长度为1Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_009()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192, CIPHER_KEY_L192, CIPHER_HDR_BIT_TYPE_5); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_KEY_L192进行完整性保护，包头长度为1Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_010()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192,CIPHER_KEY_L192,CIPHER_HDR_BIT_TYPE_5);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_192进行完整性保护，包头长度为2Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_011()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192, CIPHER_KEY_L192, CIPHER_HDR_BIT_TYPE_12); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_192进行完整性保护，包头长度为2Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_012()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192,CIPHER_KEY_L192,CIPHER_HDR_BIT_TYPE_12);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_256进行完整性保护，包头长度为1Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_013()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256, CIPHER_KEY_L256, CIPHER_HDR_BIT_TYPE_5); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_256进行完整性保护，包头长度为1Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_014()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256,CIPHER_KEY_L256,CIPHER_HDR_BIT_TYPE_5);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_256进行完整性保护，包头长度为2Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_015()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256, CIPHER_KEY_L256, CIPHER_HDR_BIT_TYPE_12); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_256进行完整性保护，包头长度为2Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_016()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256,CIPHER_KEY_L256,CIPHER_HDR_BIT_TYPE_12);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，包头长度为1Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_017()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L128, CIPHER_HDR_BIT_TYPE_5); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，包头长度为1Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_018()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);  
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，包头长度为2Byte，
* 一个数据包，输入输出都为单块，数据量大小为200，
*/
BSP_S32 BSP_CIPHER_001_019()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L128, CIPHER_HDR_BIT_TYPE_12); 
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，包头长度为2Byte，
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_001_020()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);  
}

/*callback极限场景测试*/
BSP_S32 BSP_CIPHER_001_021()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {1};
   BSP_U32 dataTestLen = 1;
   BSP_S32 s32Ret = BSP_OK;
   s32Ret = chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);                   
   return s32Ret;                    
}

/*
* PDCP单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，包头长度为2Byte，
* 一个数据包，输入为17块，块大小分别为23、45、132,17,46,80,1,101...输出为单块
*/
BSP_S32 BSP_CIPHER_001_022()
{
   BSP_U32 blk_num = 17;
   BSP_U32 blk_len[] = {23, 45, 132, 17, 46, 80, 1, 100, 101, 10, 20, 30, 40, 6, 1023, 1024, 23};
   BSP_U32 dataTestLen = 2721;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);  
}

BSP_S32 BSP_CIPHER_001_023()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);                    
}

BSP_S32 BSP_CIPHER_001_024()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);  
}

BSP_S32 BSP_CIPHER_001_025()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);                    
}

BSP_S32 BSP_CIPHER_001_026()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_pdcp_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC,CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);  
}



/* 短MAC应用场景*/
BSP_S32 chain_protect_check_nas_shortmac(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;
    CIPHER_DATA_BLOCK* blkChain = NULL;
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;

    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = 0;

    blkChain = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));

    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = MAC_SIZE_SHORT;
    cipherTmpBlock[chnid].pNextBlock = 0;

    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);

    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_PROTECT_SMAC;
    g_cipher_uplink_cfg[chnid].u8BearId = 0;//5bit
    g_cipher_uplink_cfg[chnid].u8Direction = 0;//0表示上行
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
                 &(g_cipher_uplink_cfg[chnid]),enSubmit, u32Private);
    }

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
    
    /*将完整性校验值拷贝入数据末尾*/
/*    memcpy((void*)(cipherInBlock[chnid].u32DataAddr + dataTestLen),(void*)(cipherTmpBlock[chnid].u32DataAddr),MAC_SIZE);*/

    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_OPT_CHECK_POST_SMAC;
    g_cipher_downlink_cfg[chnid].u8BearId = 0;
    g_cipher_downlink_cfg[chnid].u8Direction = 0;
    g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].u32Count = 0;
    g_cipher_downlink_cfg[chnid].bMemBlock = 0;
    g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      


    /*当输入为链表时，进行完整性检查*/
    memcpy((void*)(blkChain[blk_num-1].u32DataAddr + 
    blkChain[blk_num-1].u32DataLen),(void*)cipherTmpBlock[chnid].u32DataAddr,MAC_SIZE_SHORT);
    blkChain[blk_num-1].u32DataLen = blkChain[blk_num-1].u32DataLen + MAC_SIZE_SHORT;

    g_notify_stat[chnid] = CIPHER_STAT_OK;

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,NULL,
                 &(g_cipher_downlink_cfg[chnid]),enSubmit, u32Private);   
    }
    
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

    blkChain[blk_num-1].u32DataLen = blkChain[blk_num-1].u32DataLen - MAC_SIZE_SHORT;
    
    if(g_notify_stat[chnid] != CIPHER_STAT_OK)
    {
        cipherDebugMsg("CIPHER_STAT:%x\n",g_notify_stat[chnid],0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;        
    } 

clearup:
    free(blkChain);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}


/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_128进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_002_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128); 
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_128进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_002_002()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128,CIPHER_KEY_L128);  
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_002_003()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128);                    
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_128进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_002_004()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128);  
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_192进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_002_005()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192, CIPHER_KEY_L192); 
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_192进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_002_006()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192,CIPHER_KEY_L192);  
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_256进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_002_007()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256, CIPHER_KEY_L256); 
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_AES_256进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_002_008()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256,CIPHER_KEY_L256);  
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_NULL进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_002_009()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L256); 
}

/*
* NAS单独操作典型场景，输出MAC长度为2Byte，使用CIPHER_ALG_NULL进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_002_010()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL,CIPHER_KEY_L256);  
}

BSP_S32 BSP_CIPHER_002_011()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC,CIPHER_KEY_L128);                    
}

BSP_S32 BSP_CIPHER_002_012()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_shortmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC,CIPHER_KEY_L128);  
}

/* Nas长Mac*/
BSP_S32 chain_protect_check_nas_longmac(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;
    CIPHER_DATA_BLOCK* blkChain = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;


    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = 0;

    blkChain = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    blkChain2  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));

    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = MAC_SIZE_LONG;
    cipherTmpBlock[chnid].pNextBlock = 0;

    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);

    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_PROTECT_LMAC;
    g_cipher_uplink_cfg[chnid].u8BearId = 0;//5bit
    g_cipher_uplink_cfg[chnid].u8Direction = 0;//0表示上行
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
                 &(g_cipher_uplink_cfg[chnid]),enSubmit, u32Private);
    }
    
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
    
    /*将完整性校验值拷贝入数据末尾*/
/*    memcpy((void*)(cipherInBlock[chnid].u32DataAddr + dataTestLen),(void*)(cipherTmpBlock[chnid].u32DataAddr),MAC_SIZE);*/

    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_OPT_CHECK_PRE_LMAC;
    g_cipher_downlink_cfg[chnid].u8BearId = 0;
    g_cipher_downlink_cfg[chnid].u8Direction = 0;
    g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].u32Count = 0;
    g_cipher_downlink_cfg[chnid].bMemBlock = 0;
    g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      

    memcpy((void*)(cipherTmpBlock[chnid].u32DataAddr + MAC_SIZE_LONG),
                   (void*)(cipherInBlock[chnid].u32DataAddr), dataTestLen);

    blk_len[0] += MAC_SIZE_LONG;

    s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,(MAC_SIZE_LONG + dataTestLen),blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }

    g_notify_stat[chnid] = CIPHER_STAT_OK;

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain2,NULL,
                 &(g_cipher_downlink_cfg[chnid]),enSubmit, u32Private);   
    }
    
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

clearup:
    free(blkChain);
    free(blkChain2);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_128进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_003_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128); 
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_128进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_003_002()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128,CIPHER_KEY_L128);  
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_003_003()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128);                    
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_SNOW3G进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_003_004()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G,CIPHER_KEY_L128);  
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_192进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_003_005()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192,CIPHER_KEY_L192);                    
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_192进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_003_006()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192,CIPHER_KEY_L192);  
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_256进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_003_007()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256,CIPHER_KEY_L256);                    
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_AES_256进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_003_008()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256,CIPHER_KEY_L256);  
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入输出为单块
*/
BSP_S32 BSP_CIPHER_003_009()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL,CIPHER_KEY_L256);                    
}

/*
* NAS单独操作典型场景，输出MAC长度为4Byte，使用CIPHER_ALG_NULL进行完整性保护，没有包头，没有附加包头
* 一个数据包，输入为3块，块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_003_010()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   return chain_protect_check_nas_longmac((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL,CIPHER_KEY_L256);  
}

/* 加密操作*/
BSP_S32 chain_encrypt_decrypt(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
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
    CIPHER_SUBMIT_TYPE_E enSubmit;

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;

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
    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
    g_cipher_uplink_cfg[chnid].u8BearId = 0;//5bit
    g_cipher_uplink_cfg[chnid].u8Direction = 0;//0表示上行
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
    g_cipher_downlink_cfg[chnid].u8BearId = 0;//5bit
    g_cipher_downlink_cfg[chnid].u8Direction = 0;//0表示上行
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
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}


/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，没有包头，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_004_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，没有包头，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_004_002()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_192加密，密钥长度192，没有包头，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_004_003()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192, CIPHER_KEY_L192);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_192加密，密钥长度192，没有包头，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_004_004()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_192,CIPHER_KEY_L192);

}

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_256加密，密钥长度256，没有包头，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_004_005()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256, CIPHER_KEY_L256);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_256加密，密钥长度256，没有包头，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_004_006()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_256, CIPHER_KEY_L256);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_SNOW3G加密，密钥长度128，没有包头，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_004_007()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G, CIPHER_KEY_L128);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_SNOW3G加密，密钥长度128，没有包头，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_004_008()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G, CIPHER_KEY_L128);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_NULL加密，密钥长度128，没有包头，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_004_009()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L128);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_NULL加密，密钥长度128，没有包头，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/

BSP_S32 BSP_CIPHER_004_010()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L128);
}

/*
* PDCP上下行频繁交替切换进行操作，使用AES进行加解密，密钥长度为192，一个数据包，数据量为200字节。
* 实现方式上行加密后数据，作为下行解密数据输入
*/
BSP_S32 BSP_CIPHER_004_011()
{
   BSP_S32 i = 0;
   BSP_S32 test_times = 100;
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;

   for(i = 0; i < test_times; i++)
   {
        chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                            CIPHER_ALG_AES_192, CIPHER_KEY_L192);                    
   }
   return BSP_OK;
}

BSP_S32 BSP_CIPHER_004_012()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128);
}

BSP_S32 BSP_CIPHER_004_013()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128);
}

#endif

BSP_S32 chain_encrypt_decrypt_header(BSP_U8* caseName,BSP_U32 chnid,
                     BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                     CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen,
                                            CIPHER_HDR_E enHeaderlen)
{
 BSP_S32 s32TestStatus = BSP_OK;
 BSP_S32 s32Ret = BSP_OK;
 BSP_S32 i = 0;
 BSP_U32 ip_size;

 CIPHER_DATA_BLOCK* blkChain  = NULL;
 CIPHER_DATA_BLOCK* blkChain2 = NULL;
 BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
                     18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
 BSP_U32 u32KeyIndex = 0x1;
 BSP_U32 u32Private = 0x0;
 CIPHER_SUBMIT_TYPE_E enSubmit;

 if(!g_cipher_test_inited)
 {
    cipherTestInit();
 }  

 enSubmit = CIPHER_SUBM_CALLBACK;

 /*cipherTestInit();*/
 
 cipherInBlock[chnid].pNextBlock = 0;
 cipherTmpBlock[chnid].pNextBlock = 0;
 cipherOutBlock[chnid].pNextBlock = 0;

 ip_size = u8HerderLenTbl[enHeaderlen];

 /*配置数据加解密缓冲区*/
 cipherInBlock[chnid].u32DataLen = ip_size + dataTestLen;
 blk_len[0] += ip_size;

 blkChain  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
 blkChain2 = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
 
 s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen+ip_size,blk_num,blk_len);
 if(s32Ret != BSP_OK)
 {
     s32TestStatus = BSP_ERROR;
     goto clearup;
 }    

 cipherTmpBlock[chnid].u32DataLen = dataTestLen + ip_size;
 cipherOutBlock[chnid].u32DataLen = dataTestLen + ip_size;

#if 1   
 memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
 memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
 memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);
#endif
 for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
/* g_cipher_uplink_cfg[chnid].u8BearId = 0x1f;5bit*/
 g_cipher_uplink_cfg[chnid].u8BearId = 0x0;/*5bit*/
 g_cipher_uplink_cfg[chnid].u8Direction = 0;/*0表示上行*/
 g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
 g_cipher_uplink_cfg[chnid].enHeaderLen = enHeaderlen;
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
 g_cipher_downlink_cfg[chnid].u8BearId = 0;//5bit
 g_cipher_downlink_cfg[chnid].u8Direction = 0;//0表示上行
 g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
 g_cipher_downlink_cfg[chnid].enHeaderLen = enHeaderlen;
 g_cipher_downlink_cfg[chnid].u32Count = 0;
 g_cipher_downlink_cfg[chnid].bMemBlock = 0;
 g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
 g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
 g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
 g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
 g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
 
 /*构造加密后的数据链表*/
 s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,dataTestLen + ip_size,blk_num,blk_len);
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
 cipherInBlock[chnid].u32DataLen -= ip_size;
 blk_len[0] -= ip_size;
 
 if(s32TestStatus == BSP_OK)
 {
     cipherDebugMsg("test pass.\n",0,0,0,0,0);
 }
 else
 {
     cipherDebugMsg("test fail.\n",0,0,0,0,0); 
 }
 return s32TestStatus;
}

 /*
 * PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，包头长度为1Byte，没有附加包头，
 * 一个数据包，输入输出都为单块，数据量大小为200
 */
 BSP_S32 BSP_CIPHER_007_001()
 {
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                     CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
 }
 
 /*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，包头长度为1Byte，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
 BSP_S32 BSP_CIPHER_007_002()
 {
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {23,45,132};
    BSP_U32 dataTestLen = 200;
    
    return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                     CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
 }

 /*
 * PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，包头长度为2Byte，没有附加包头，
 * 一个数据包，输入输出都为单块，数据量大小为200
 */
  BSP_S32 BSP_CIPHER_007_003()
 {
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                     CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
 }
 
 /*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，包头长度为2Byte，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
 BSP_S32 BSP_CIPHER_007_004()
 {
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {23,45,132};
    BSP_U32 dataTestLen = 200;
    
    return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                     CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
 }

 /*
 * PDCP上行加密典型场景，CIPHER_ALG_SNOW3G加密，密钥长度128，包头长度为1Byte，没有附加包头，
 * 一个数据包，输入输出都为单块，数据量大小为200
 */
 BSP_S32 BSP_CIPHER_007_005()
  {
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                      CIPHER_ALG_SNOW3G, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
  }
  
 /*
* PDCP上行加密典型场景，CIPHER_ALG_SNOW3G加密，密钥长度128，包头长度为1Byte，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
  BSP_S32 BSP_CIPHER_007_006()
  {
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {23,45,132};
     BSP_U32 dataTestLen = 200;
     
     return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                      CIPHER_ALG_SNOW3G, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
  }

/*
* PDCP上行加密典型场景，CIPHER_ALG_SNOW3G加密，密钥长度128，包头长度为2Byte，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_007_007()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_SNOW3G加密，密钥长度128，包头长度为2Byte，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_007_008()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_SNOW3G, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_NULL加密，密钥长度128，包头长度为1Byte，没有附加包头，
* 一个数据包，输入输出都为单块，数据量大小为200
*/
BSP_S32 BSP_CIPHER_007_009()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
}

/*
* PDCP上行加密典型场景，CIPHER_ALG_NULL加密，密钥长度128，包头长度为1Byte，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
BSP_S32 BSP_CIPHER_007_010()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_NULL, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
}

/*
 * PDCP上行加密典型场景，CIPHER_ALG_NULL加密，密钥长度128，包头长度为2Byte，没有附加包头，
 * 一个数据包，输入输出都为单块，数据量大小为200
 */
 BSP_S32 BSP_CIPHER_007_011()
  {
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                      CIPHER_ALG_NULL, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
  }
  
 /*
* PDCP上行加密典型场景，CIPHER_ALG_NULL加密，密钥长度128，包头长度为2Byte，没有附加包头，
* 块大小分别为23、45、132，输出为单块
*/
  BSP_S32 BSP_CIPHER_007_012()
  {
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {23,45,132};
     BSP_U32 dataTestLen = 200;
     
     return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                      CIPHER_ALG_NULL, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
  }

BSP_S32 BSP_CIPHER_007_013()
  {
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                      CIPHER_ALG_ZUC, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
  }
  
BSP_S32 BSP_CIPHER_007_014()
  {
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {23,45,132};
     BSP_U32 dataTestLen = 200;
     
     return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                      CIPHER_ALG_ZUC, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_5);
  }

BSP_S32 BSP_CIPHER_007_015()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {200};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
}

BSP_S32 BSP_CIPHER_007_016()
{
   BSP_U32 blk_num = 3;
   BSP_U32 blk_len[] = {23,45,132};
   BSP_U32 dataTestLen = 200;
   
   return  chain_encrypt_decrypt_header((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128,CIPHER_HDR_BIT_TYPE_12);
}

  BSP_S32 chain_encrypt_decrypt_aph(BSP_U8* caseName,BSP_U32 chnid,
                      BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                      CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen,
                                 CIPHER_HDR_E aphlen,BSP_U32 u32Count)
  {
      BSP_S32 s32TestStatus = BSP_OK;
      BSP_S32 s32Ret = BSP_OK;
      BSP_S32 i = 0;
      BSP_U32 ip_size = 0;
  
      CIPHER_DATA_BLOCK* blkChain  = NULL;
      CIPHER_DATA_BLOCK* blkChain2 = NULL;
      BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
                          18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
      BSP_U32 u32KeyIndex = 0x1;
      BSP_U32 u32Private = 0x0;
      CIPHER_SUBMIT_TYPE_E enSubmit;

      if(!g_cipher_test_inited)
      {
          cipherTestInit();
      }  

      enSubmit = CIPHER_SUBM_CALLBACK;
      
      cipherInBlock[chnid].pNextBlock = 0;
      cipherInBlock[chnid].u32DataLen = dataTestLen;
  
      blkChain  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
      blkChain2 = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
      
      s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
      if(s32Ret != BSP_OK)
      {
          s32TestStatus = BSP_ERROR;
          goto clearup;
      }    
  
      ip_size = u8APHLenTbl[aphlen];
  
      cipherTmpBlock[chnid].u32DataLen = dataTestLen + ip_size;
      cipherTmpBlock[chnid].pNextBlock = 0;
      cipherOutBlock[chnid].u32DataLen = dataTestLen;
      cipherOutBlock[chnid].pNextBlock = 0;
  
#if 1   
      memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
      memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
      memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);
#endif
      for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
      g_cipher_uplink_cfg[chnid].u8BearId = 0;//5bit
      g_cipher_uplink_cfg[chnid].u8Direction = 0;//0表示上行
      g_cipher_uplink_cfg[chnid].enAppdHeaderLen = aphlen;
      g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
      g_cipher_uplink_cfg[chnid].u32Count = u32Count;
      g_cipher_uplink_cfg[chnid].bMemBlock = 0;
      g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
      g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
      g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
      g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
      g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
       
      s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
               &(g_cipher_uplink_cfg[chnid]), CIPHER_SUBM_CALLBACK, u32Private);             


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
      g_cipher_downlink_cfg[chnid].u8BearId = 0;//5bit
      g_cipher_downlink_cfg[chnid].u8Direction = 0;//0表示上行
      g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
      g_cipher_downlink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
      g_cipher_downlink_cfg[chnid].u32Count = u32Count;
      g_cipher_downlink_cfg[chnid].bMemBlock = 0;
      g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
      g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
      g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
      g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
      g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
  
      cipherTmpBlock[chnid].u32DataAddr += ip_size;
      
      /*构造加密后的数据链表*/
      s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,dataTestLen,blk_num,blk_len);
      if(s32Ret != BSP_OK)
      {
          s32TestStatus = BSP_ERROR;
          goto clearup;
      }    
  
      g_notify_stat[chnid] = CIPHER_STAT_OK;
          
      s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain2,&cipherOutBlock[chnid],
               &(g_cipher_downlink_cfg[chnid]),CIPHER_SUBM_CALLBACK, u32Private);
      if(s32Ret != CIPHER_SUCCESS)
      {
          cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
          s32TestStatus = BSP_ERROR;
          goto clearup;
      }
     
      semTake(cipherChnCbSem[chnid],WAIT_FOREVER);
  
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
      cipherInBlock[chnid].u32DataLen -= ip_size;
      blk_len[0] -= ip_size;
      
      if(s32TestStatus == BSP_OK)
      {
          cipherDebugMsg("test pass.\n",0,0,0,0,0);
      }
      else
      {
          cipherDebugMsg("test fail.\n",0,0,0,0,0); 
      }
      return s32TestStatus;
  }

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，附加包头长度为1Byte，对应附加头的
* 内容是Count值的低5位，不参与保护，没有包头，一个数据包，输入输出都为单块，数据量大小为200
*/
 BSP_S32 BSP_CIPHER_008_001()
 {
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32count = 0x353526;
    
    return  chain_encrypt_decrypt_aph((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_APPEND_HDR_BIT_TYPE_5, u32count);
 }
 
/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，附加包头长度为1Byte，对应附加头的
* 内容是Count值的低5位, 不参与保护，没有包头，一个数据包，输入为3块，块大小分别为23、45、132，
* 输出为单块
*/
 BSP_S32 BSP_CIPHER_008_002()
 {
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {23,45,132};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32count = 0x353526;
    
    return  chain_encrypt_decrypt_aph((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_APPEND_HDR_BIT_TYPE_5, u32count);
 }

/*
* PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，附加包头长度为1Byte，对应附加头的
* 内容是Count值的低7位，不参与保护，没有包头，一个数据包，输入输出都为单块，数据量大小为200
*/
  BSP_S32 BSP_CIPHER_008_003()
 {
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32count = 0x353526;
    
    return  chain_encrypt_decrypt_aph((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_APPEND_HDR_BIT_TYPE_7,u32count);
 }
 
 /*
 * PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，附加包头长度为1Byte，对应附加头的
 * 内容是Count值的低7位, 不参与保护，没有包头，一个数据包，输入为3块，块大小分别为23、45、132，
 * 输出为单块
 */
 BSP_S32 BSP_CIPHER_008_004()
 {
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {23,45,132};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32count = 0x353526;
    
    return  chain_encrypt_decrypt_aph((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_APPEND_HDR_BIT_TYPE_7, u32count);
 }

 /*
 * PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，附加包头长度为2Byte，对应附加头的
 * 内容是Count值的低12位，不参与保护，没有包头，一个数据包，输入输出都为单块，数据量大小为200
 */
 BSP_S32 BSP_CIPHER_008_005()
 {
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32count = 0x353526;
    
    return  chain_encrypt_decrypt_aph((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_APPEND_HDR_BIT_TYPE_12,u32count);
 }
 
 /*
 * PDCP上行加密典型场景，CIPHER_ALG_AES_128加密，密钥长度128，附加包头长度为2Byte，对应附加头的
 * 内容是Count值的低12位, 不参与保护，没有包头，一个数据包，输入为3块，块大小分别为23、45、132，
 * 输出为单块
 */
 BSP_S32 BSP_CIPHER_008_006()
 {
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {23,45,132};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32count = 0x353526;
    
    return  chain_encrypt_decrypt_aph((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                CIPHER_ALG_AES_128, CIPHER_KEY_L128,CIPHER_APPEND_HDR_BIT_TYPE_12, u32count);
 }

 /*PDCP_SIGNAL*/
 BSP_S32 associate_operate_pdcp(BSP_U8* caseName,BSP_U32 chnid,
                     BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                     CIHPER_RELA_CFG_S cfg[],CIPHER_SUBMIT_TYPE_E submAttr[])
 {
     BSP_S32 s32TestStatus = BSP_OK;
     BSP_S32 s32Ret = BSP_OK;
     BSP_S32 i = 0;
     CIPHER_DATA_BLOCK* blkChain1  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
     CIPHER_DATA_BLOCK* blkChain2  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
     BSP_U32 ip_size = 0;
     BSP_U32 u32Private = 0;
     BSP_U32 u32ChangedHead = BSP_FALSE;
     BSP_U32 u32ChangedTail = BSP_FALSE;

     if(!g_cipher_test_inited)
     {
         cipherTestInit();
     }  
     
     cipherInBlock[chnid].pNextBlock = 0;
     cipherTmpBlock[chnid].pNextBlock = 0;
     cipherOutBlock[chnid].pNextBlock = 0;
       
     switch(cfg[0].enOpt)
     {
     /*涉及IP头的处理*/
     case CIPHER_PDCP_PRTCT_ENCY:
     case CIPHER_PDCP_DECY_CHCK:
         {
/*
           ip_size = IP_SIZE;
           ip_size = IP_SIZE(cfg[0].enHeaderLen); 
*/
             ip_size = u8HerderLenTbl[cfg[0].enHeaderLen];
         }
         break;
     default:
         cipherDebugMsg("enOpt fail\n",0,0,0,0,0);
         s32TestStatus = BSP_ERROR;
         goto clearup;
     }
 
     cipherInBlock[chnid].u32DataLen = ip_size + dataTestLen;
     blk_len[0] += ip_size;
     u32ChangedHead = BSP_TRUE;
     
     s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain1,ip_size + dataTestLen,blk_num,blk_len);
     if(s32Ret != BSP_OK)
     {
         s32TestStatus = BSP_ERROR;
         goto clearup;
     }
     
     cipherTmpBlock[chnid].u32DataLen = ip_size + dataTestLen + MAC_SIZE;
  
     memset((void*)cipherInBlock[chnid].u32DataAddr, 0,cipherInBlock[chnid].u32DataLen + 20);
     memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,cipherTmpBlock[chnid].u32DataLen + 20);
     memset((void*)cipherOutBlock[chnid].u32DataAddr,0,cipherOutBlock[chnid].u32DataLen + 20);
 
     for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
     {
         *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = (BSP_U8)i;
     }
     
     s32Ret = BSP_CIPHER_RelaSubmitTask(chnid,(void*)blkChain1,(void*)&cipherTmpBlock[chnid],
              &(cfg[0]), submAttr[0], u32Private);
     if(submAttr[0] == CIPHER_SUBM_CALLBACK)
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
     switch(cfg[1].enOpt)
     {
     /*涉及IP头的处理*/
     case CIPHER_PDCP_PRTCT_ENCY:
     case CIPHER_PDCP_DECY_CHCK:
         {
/*
              ip_size = IP_SIZE;
            ip_size = IP_SIZE(cfg[1].enHeaderLen); 
*/
             ip_size = u8HerderLenTbl[cfg[1].enHeaderLen]; 
         }
         break;
     default:
         
         s32TestStatus = BSP_ERROR;
         goto clearup;
     }
     
     blk_len[blk_num - 1] += MAC_SIZE;
     u32ChangedTail = BSP_TRUE;
     
     s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,(ip_size + dataTestLen + MAC_SIZE),blk_num,blk_len);
     if(s32Ret != BSP_OK)
     {
         s32TestStatus = BSP_ERROR;
         goto clearup;
     }
    
     cipherOutBlock[chnid].u32DataLen = ip_size + dataTestLen;
     
     g_notify_stat[chnid] = CIPHER_STAT_OK;
     
     s32Ret = BSP_CIPHER_RelaSubmitTask(chnid,(void*)blkChain2,(void*)&cipherOutBlock[chnid],
              &(cfg[1]),submAttr[1], u32Private);    
     if(submAttr[1] == CIPHER_SUBM_CALLBACK)
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
 
 /*还原blk_len[0]*/
     if(u32ChangedHead)
     {
         blk_len[0] -= ip_size;
     }
 
 /*还原blk_len[blk_num -1]*/
     if(u32ChangedTail)
     {
         blk_len[blk_num -1] -= MAC_SIZE;
     }
     
     free(blkChain1);
     free(blkChain2);
     if(s32TestStatus == BSP_OK)
     {
         cipherDebugMsg("test pass.\n",0,0,0,0,0);
     }
     else
     {
         cipherDebugMsg("test fail.\n",0,0,0,0,0); 
     }
     return s32TestStatus;   
 }


 /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
 BSP_S32 BSP_CIPHER_005_001()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_002()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_003()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
 /*    BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
 */
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_004()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
BSP_S32 BSP_CIPHER_005_005()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte.
 */
BSP_S32 BSP_CIPHER_005_006()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
* 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
*/
BSP_S32 BSP_CIPHER_005_007()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
* 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
*/
BSP_S32 BSP_CIPHER_005_008()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_SNOW3G加密，
* 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
*/
BSP_S32 BSP_CIPHER_005_009()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_010()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_011()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_012()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

 /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
 BSP_S32 BSP_CIPHER_005_013()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_014()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_015()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_016()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

 /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
 BSP_S32 BSP_CIPHER_005_017()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_018()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_019()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_020()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
 BSP_S32 BSP_CIPHER_005_021()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_022()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_023()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_024()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
 BSP_S32 BSP_CIPHER_005_025()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_026()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_027()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_028()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
BSP_S32 BSP_CIPHER_005_029()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_030()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_031()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_032()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为1Byte.
 */
BSP_S32 BSP_CIPHER_005_033()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }
 
 /*
  * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，
  * 密钥长度128，一个数据包，输入输出都为单块，数据量大小为200, 包头长为2Byte，
  */
 BSP_S32 BSP_CIPHER_005_034()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 1;
     BSP_U32 blk_len[] = {200};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

  /*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为1Byte，
 */
 BSP_S32 BSP_CIPHER_005_035()
 {
     BSP_S32 i = 0;
     BSP_S32 s32Ret;
     CIHPER_RELA_CFG_S cfg[2];
     CIPHER_SUBMIT_TYPE_E submAttr[2];
     BSP_U32 blk_num = 3;
     BSP_U32 blk_len[] = {132,45,23};
     BSP_U32 dataTestLen = 200;
     
     BSP_U32 u32KeyIndexInt = 0x0;
     BSP_U32 u32KeyIndexSec = 0x1;
     CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
     CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
     CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
     CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
 
     for(i=0; i<2; i++)
     {
         memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
         memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
     }
     
     cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
     cfg[UP_LINK].u8BearId = 0;
     cfg[UP_LINK].u8Direction = 0;
     cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[UP_LINK].enHeaderLen = enHeaderLength;
     cfg[UP_LINK].u32Count = 0;
     cfg[UP_LINK].bMemBlock = 0;
     cfg[UP_LINK].u32BlockLen = 0;
     cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
     cfg[DOWN_LINK].u8BearId = 0;
     cfg[DOWN_LINK].u8Direction = 0;
     cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
     cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
     cfg[DOWN_LINK].u32Count = 0;
     cfg[DOWN_LINK].bMemBlock = 0;
     cfg[DOWN_LINK].u32BlockLen = 0;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
     cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
     cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;
 
     submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
     submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;
 
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }
     s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
     if(CIPHER_SUCCESS != s32Ret)
     {
         cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
         return BSP_ERROR;
     }    
 
     return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
 }

/*
 * PDCP关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，
 * 密钥长度128，一个数据包，输入为3块，块大小分别为132、45、23，输出为单块, 包头长为2Byte，
 */
BSP_S32 BSP_CIPHER_005_036()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用AES完整性保护，使用AES加密，密钥长度192，一个数据包，输入输出都为单块，
* 数据量大小为200
*/
BSP_S32 BSP_CIPHER_005_037()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_192;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用AES完整性保护，使用AES加密，密钥长度192，
*  一个数据包，输入为3块，块大小分别为132、45、23，输出为单块
*/
BSP_S32 BSP_CIPHER_005_038()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_192;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用AES完整性保护，使用AES加密，密钥长度256，一个数据包，输入输出都为单块，
* 数据量大小为200
*/
BSP_S32 BSP_CIPHER_005_039()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_256;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用AES完整性保护，使用AES加密，密钥长度256，一个数据包，输入输出都为单块，
* 数据量大小为200
*/
BSP_S32 BSP_CIPHER_005_040()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_256;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用SNOW3G完整性保护，使用AES加密，密钥长度192，一个数据包，输入输出都为单块，
* 数据量大小为200
*/
BSP_S32 BSP_CIPHER_005_041()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用SNOW3G完整性保护，使用AES加密，密钥长度192，
*  一个数据包，输入为3块，块大小分别为132、45、23，输出为单块
*/
BSP_S32 BSP_CIPHER_005_042()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用SNOW3G完整性保护，使用AES加密，密钥长度256，一个数据包，输入输出都为单块，
* 数据量大小为200
*/
BSP_S32 BSP_CIPHER_005_043()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用SNOW3G完整性保护，使用AES加密，密钥长度256，
*  一个数据包，输入为3块，块大小分别为132、45、23，输出为单块
*/
BSP_S32 BSP_CIPHER_005_044()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用SNOW3G完整性保护，使用SNOW3G加密，密钥长度128，一个数据包，输入输出都为单块，
* 数据量大小为200
*/

/*
* PDCP上行信令典型场景，使用AES完整性保护，使用AES加密，密钥长度128，一个数据包，输入输出都为单块，
* 数据量大小为2048
*/
BSP_S32 BSP_CIPHER_005_045()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {2048};
    BSP_U32 dataTestLen = 2048;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* PDCP上行信令典型场景，使用AES完整性保护，使用AES加密，密钥长度128，
*  一个数据包，大小为2048，输入为3块，块大小分别为1023，999，26，输出为单块
*/
BSP_S32 BSP_CIPHER_005_046()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {1023,999,26};
    BSP_U32 dataTestLen = 2048;
    
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    for(i=0; i<2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }
    
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]  = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK]= CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


BSP_S32 bsp_cipher_ass_op_pdcp(CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg)
{
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2]={{0},{0}};
	
    cfg[UP_LINK].enOpt = CIPHER_PDCP_PRTCT_ENCY;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = stCipherAssOpPdcpArg.enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_PDCP_DECY_CHCK;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = stCipherAssOpPdcpArg.enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec;

    s32Ret = BSP_CIPHER_SetKey(g_key, stCipherAssOpPdcpArg.keyLen, stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, stCipherAssOpPdcpArg.keyLen, stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_pdcp((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,stCipherAssOpPdcpArg.dataTestLen,\
        stCipherAssOpPdcpArg.blk_num,stCipherAssOpPdcpArg.blk_len,cfg,stCipherAssOpPdcpArg.submAttr);
}

BSP_S32 BSP_CIPHER_005_047()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_048()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_049()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_050()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_051()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_052()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_053()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_054()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_055()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_056()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_057()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_058()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}
 
BSP_S32 BSP_CIPHER_005_059()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_060()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_061()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_062()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_063()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_064()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_065()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_066()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_067()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_005_068()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_CALLBACK;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0x0;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0x1;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_pdcp(stCipherAssOpPdcpArg);
}

/* Nas关联操作*/
BSP_S32 associate_operate_nas(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIHPER_RELA_CFG_S cfg[],CIPHER_SUBMIT_TYPE_E submAttr[])
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;
    CIPHER_DATA_BLOCK* blkChain1  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    CIPHER_DATA_BLOCK* blkChain2  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    BSP_U32 ip_size = 0;
    BSP_U32 u32Private = 0;
    BSP_U32 u32ChangedHead = BSP_FALSE;
    BSP_U32 u32ChangedMac = BSP_FALSE;

     if(!g_cipher_test_inited)
     {
         cipherTestInit();
     }
    
    cipherInBlock[chnid].pNextBlock = 0;
    cipherTmpBlock[chnid].pNextBlock = 0;
    cipherOutBlock[chnid].pNextBlock = 0;

    /* NAS关联操作数据前需要有Sequence Number，大小为1Byte, 这个Nuber就是包头*/
    switch(cfg[0].enOpt)
    {
        case CIPHER_NAS_ENCY_PRTCT:
        case CIPHER_NAS_CHCK_DECY:
        {
              ip_size = SEQNUM_SIZE;
        }
        break;
        default:
            cipherDebugMsg("enOpt fail\n",0,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
    }

    cipherInBlock[chnid].u32DataLen = ip_size + dataTestLen;
    blk_len[0] += ip_size;
    u32ChangedHead = BSP_TRUE;

    /* 构建PDCP结构体链表*/
    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain1,ip_size + dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }

    /* 上行输出长度为MAC_SIZE + Seq Num + Nas Data*/
    cipherTmpBlock[chnid].u32DataLen = MAC_SIZE + ip_size + dataTestLen;
 
    memset((void*)cipherInBlock[chnid].u32DataAddr, 0,cipherInBlock[chnid].u32DataLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,cipherTmpBlock[chnid].u32DataLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,cipherOutBlock[chnid].u32DataLen + 20);

    /* 设置输入数据，包括Sequnce Number 和 NAS Meg*/
    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
    {
        *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = (BSP_U8)i;
    }

    /* 提交操作*/
    s32Ret = BSP_CIPHER_RelaSubmitTask(chnid,(void*)blkChain1,(void*)&cipherTmpBlock[chnid],
             &(cfg[0]), submAttr[0], u32Private);
    if(submAttr[0] == CIPHER_SUBM_CALLBACK)
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
    switch(cfg[0].enOpt)
    {
    case CIPHER_NAS_ENCY_PRTCT:
    case CIPHER_NAS_CHCK_DECY:
        {
              ip_size = SEQNUM_SIZE;
        }
        break;
    default:
        cipherDebugMsg("enOpt fail\n",0,0,0,0,0);
        break;
    }
    
    blk_len[0] += MAC_SIZE;
    u32ChangedMac = BSP_TRUE;

    /* 构建PDCP结构体链表*/
    s32Ret = build_blk_chain(&cipherTmpBlock[chnid],blkChain2,(MAC_SIZE + ip_size + dataTestLen),blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }

    /* 设置下行数据输出长度，为Seq Number + NAS Meg*/
    cipherOutBlock[chnid].u32DataLen = ip_size + dataTestLen;
    
    g_notify_stat[chnid] = CIPHER_STAT_OK;
    
    s32Ret = BSP_CIPHER_RelaSubmitTask(chnid,(void*)blkChain2,(void*)&cipherOutBlock[chnid],
             &(cfg[1]),submAttr[1], u32Private);    
    if(submAttr[1] == CIPHER_SUBM_CALLBACK)
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
    /*还原blk_len[0]*/
    if(u32ChangedHead)
    {
        blk_len[0] -= ip_size;
    }
    /*还原blk_len[0]*/
    if(u32ChangedMac)
    {
        blk_len[0] -= MAC_SIZE;
    }
   
    free(blkChain1);
    free(blkChain2);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;   
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_001()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_002()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_003()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_004()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_005()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_006()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_007()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_008()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_009()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_010()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_011()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_012()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_013()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_014()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_015()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_016()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_017()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_018()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_019()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_NULL完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_020()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_NULL;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_021()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_022()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_023()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_024()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_025()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_026()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_027()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_128完整性保护，使用CIPHER_ALG_SNOW3G加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_028()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_128;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_SNOW3G;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_029()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_030()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_031()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_032()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_033()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_034()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_035()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_128加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_036()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_128;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_192完整性保护，使用CIPHER_ALG_AES_192加密，密钥长度128，一个数据包，
* 输入为单块,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_037()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_192;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexInt;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_192完整性保护，使用CIPHER_ALG_AES_192加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_038()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0xe;
    BSP_U32 u32KeyIndexSec = 0xf;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_192;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_256完整性保护，使用CIPHER_ALG_AES_256加密，密钥长度128，一个数据包，
* 输入为一块，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_039()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0xe;
    BSP_U32 u32KeyIndexSec = 0xf;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_256;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}


/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_256完整性保护，使用CIPHER_ALG_AES_256加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_040()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0xe;
    BSP_U32 u32KeyIndexSec = 0xf;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_AES_256;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_192加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_041()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexInt;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_256完整性保护，使用CIPHER_ALG_AES_256加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长2Byte
*/
BSP_S32 BSP_CIPHER_006_042()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0xe;
    BSP_U32 u32KeyIndexSec = 0xf;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_192;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_AES_256加密，密钥长度128，
* 一个数据包，输入为1块，块大小为200，输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_043()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
/*
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
*/
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0xe;
    BSP_U32 u32KeyIndexSec = 0xf;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK] = CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_AES_256完整性保护，使用CIPHER_ALG_AES_256加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_044()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {132,45,23};
    BSP_U32 dataTestLen = 200;
    BSP_U32 u32KeyIndexInt = 0xe;
    BSP_U32 u32KeyIndexSec = 0xf;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L256;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_AES_256;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5;
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    submAttr[UP_LINK]   = CIPHER_SUBM_CALLBACK;
    submAttr[DOWN_LINK] = CIPHER_SUBM_CALLBACK;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

/*
* NAS关联操作典型场景，使用CIPHER_ALG_SNOW3G完整性保护，使用CIPHER_ALG_NULL加密，密钥长度128，一个数据包，
* 输入为3块，块大小分别为132、45、23,输出为单块,提交类型为CIPHER_SUBM_BLK_HOLD模式,包头长1Byte
*/
BSP_S32 BSP_CIPHER_006_045()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2];
    CIPHER_SUBMIT_TYPE_E submAttr[2];
    BSP_U32 blk_num = 18;
    BSP_U32 blk_len[] = {23, 45, 132, 17, 46, 80, 1, 100, 101, 10, 20, 30, 40, 6, 1023, 1024, 23, 9};
    BSP_U32 dataTestLen = 2730;
    BSP_U32 u32KeyIndexInt = 0x0;
    BSP_U32 u32KeyIndexSec = 0x1;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L128;
    CIPHER_ALGORITHM_E enAlgIntegrity = CIPHER_ALG_SNOW3G;
    CIPHER_ALGORITHM_E enAlgSecurity = CIPHER_ALG_NULL;
    CIPHER_HDR_E enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 
    
    for(i = 0; i < 2; i++)
    {
        memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S));
        memset((void*)&submAttr[i],0,sizeof(CIPHER_SUBMIT_TYPE_E));
    }

    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndexSec;


    submAttr[UP_LINK]   = CIPHER_SUBM_BLK_HOLD;
    submAttr[DOWN_LINK] = CIPHER_SUBM_BLK_HOLD;

    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, keyLen, u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,dataTestLen,blk_num,blk_len,cfg,submAttr);
}

BSP_S32 bsp_cipher_ass_op_nas(CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg)
{
    BSP_S32 s32Ret;
    CIHPER_RELA_CFG_S cfg[2]={{0},{0}};

    /*memset((void*)&cfg[i],0,sizeof(CIHPER_RELA_CFG_S) * 2);*/
    
    cfg[UP_LINK].enOpt = CIPHER_NAS_ENCY_PRTCT;
    cfg[UP_LINK].u8BearId = 0;
    cfg[UP_LINK].u8Direction = 0;
    cfg[UP_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[UP_LINK].enHeaderLen = stCipherAssOpPdcpArg.enHeaderLength;
    cfg[UP_LINK].u32Count = 0;
    cfg[UP_LINK].bMemBlock = 0;
    cfg[UP_LINK].u32BlockLen = 0;
    cfg[UP_LINK].stAlgKeyInfo.enAlgIntegrity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity;
    cfg[UP_LINK].stAlgKeyInfo.enAlgSecurity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexInt = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt;
    cfg[UP_LINK].stAlgKeyInfo.u32KeyIndexSec = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec;

    cfg[DOWN_LINK].enOpt = CIPHER_NAS_CHCK_DECY;
    cfg[DOWN_LINK].u8BearId = 0;
    cfg[DOWN_LINK].u8Direction = 0;
    cfg[DOWN_LINK].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    cfg[DOWN_LINK].enHeaderLen = stCipherAssOpPdcpArg.enHeaderLength;
    cfg[DOWN_LINK].u32Count = 0;
    cfg[DOWN_LINK].bMemBlock = 0;
    cfg[DOWN_LINK].u32BlockLen = 0;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgIntegrity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity;
    cfg[DOWN_LINK].stAlgKeyInfo.enAlgSecurity = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexInt = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt;
    cfg[DOWN_LINK].stAlgKeyInfo.u32KeyIndexSec = stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec;

    s32Ret = BSP_CIPHER_SetKey(g_key, stCipherAssOpPdcpArg.keyLen, stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }
    s32Ret = BSP_CIPHER_SetKey(g_key, stCipherAssOpPdcpArg.keyLen, stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey2 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }    

    return associate_operate_nas((BSP_U8*)__FUNCTION__,PDCP_SIGNAL_CHN,stCipherAssOpPdcpArg.dataTestLen,\
        stCipherAssOpPdcpArg.blk_num,stCipherAssOpPdcpArg.blk_len,cfg,stCipherAssOpPdcpArg.submAttr);
}

BSP_S32 BSP_CIPHER_006_046()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_047()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_048()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_049()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_SNOW3G;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_050()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_051()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_052()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_053()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_054()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_055()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_056()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_057()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_058()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_059()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_060()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_061()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_062()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_063()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_064()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_065()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_NULL;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_066()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_067()
{
    BSP_U32 blk_len[] = {200};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 1;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_068()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_5; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

BSP_S32 BSP_CIPHER_006_069()
{
    BSP_U32 blk_len[] = {132,45,23};
    CIPHER_ASS_OP_PDCP_ARG stCipherAssOpPdcpArg;
    stCipherAssOpPdcpArg.blk_num = 3;
    stCipherAssOpPdcpArg.blk_len = blk_len;
    stCipherAssOpPdcpArg.submAttr[0]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.submAttr[1]=CIPHER_SUBM_BLK_HOLD;
    stCipherAssOpPdcpArg.dataTestLen = 200;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexInt = 0xe;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.u32KeyIndexSec = 0xf;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgIntegrity = CIPHER_ALG_ZUC;
    stCipherAssOpPdcpArg.stRelaCfg.stAlgKeyInfo.enAlgSecurity = CIPHER_ALG_AES_128;
    stCipherAssOpPdcpArg.keyLen = CIPHER_KEY_L128;
    stCipherAssOpPdcpArg.enHeaderLength = CIPHER_HDR_BIT_TYPE_12; 

    return bsp_cipher_ass_op_nas(stCipherAssOpPdcpArg);
}

/* Purge操作测试*/
BSP_S32 chain_purge_prepare(BSP_U8* caseName,BSP_U32 chnid,
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
    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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
/*        semTake(cipherChnCbSem[chnid],WAIT_FOREVER);*/
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
/*        semTake(cipherChnCbSem[chnid],WAIT_FOREVER);*/
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
#if 0    
    s32Ret = cipherDataCmp(&cipherInBlock[chnid],&cipherOutBlock[chnid]);
    if(s32Ret != BSP_OK)
    {
        cipherDebugMsg("data cmp fail.\n",0,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
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

/* 提交的操作类型为NONE时，进行Purge操作*/
BSP_S32 BSP_CIPHER_009_001()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {10240};
    BSP_U32 dataTestLen = 10240; 
    BSP_S32 test_times = 0xa;

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }

    submAttr = CIPHER_SUBM_NONE;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_purge_prepare((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    s32Ret = BSP_CIPHER_Purge(PDCP_SIGNAL_CHN);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return s32Ret;
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交的操作类型为NONE时，进行Purge操作*/
BSP_S32 BSP_CIPHER_009_002()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {4096};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {3200, 800, 6240};
    BSP_U32 dataTestLen = 10240; 
    BSP_S32 test_times = 0xa;
    
    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }

    submAttr = CIPHER_SUBM_NONE;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_purge_prepare((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_192, CIPHER_KEY_L192, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    s32Ret = BSP_CIPHER_Purge(PDCP_SIGNAL_CHN);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return s32Ret;
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交的操作类型为CallBack时，进行Purge操作*/
BSP_S32 BSP_CIPHER_009_003()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {10*1024};
    BSP_U32 dataTestLen = 10240; 
    BSP_S32 test_times = 0xa;

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }

    submAttr = CIPHER_SUBM_CALLBACK;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_purge_prepare((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    s32Ret = BSP_CIPHER_Purge(PDCP_SIGNAL_CHN);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return s32Ret;
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交的操作类型为CallBack时，进行Purge操作*/
BSP_S32 BSP_CIPHER_009_004()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {4096};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {240, 5000, 5000};
    BSP_U32 dataTestLen = 10240; 
    BSP_S32 test_times = 0x8;

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }    

    submAttr = CIPHER_SUBM_CALLBACK;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_purge_prepare((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_192, CIPHER_KEY_L192, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    s32Ret = BSP_CIPHER_Purge(PDCP_SIGNAL_CHN);
    if(CIPHER_SUCCESS != s32Ret)
    {
        cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return s32Ret;
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}


BSP_S32 chain_encrypt_decrypt_multitype(BSP_U8* caseName,BSP_U32 chnid,
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
    for(i = 0; i <(BSP_S32) cipherInBlock[chnid].u32DataLen; i++)
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


/* 提交多次阻塞类型的操作，使用CIPHER_ALG_AES_128加密*/
BSP_S32 BSP_CIPHER_010_001()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次阻塞类型的操作*/
BSP_S32 BSP_CIPHER_010_002()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {120, 20, 60};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次阻塞类型的操作，使用CIPHER_ALG_AES_192加密*/
BSP_S32 BSP_CIPHER_010_003()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_192, CIPHER_KEY_L192, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

BSP_S32 BSP_CIPHER_010_004()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {120, 20, 60};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_192, CIPHER_KEY_L192, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次阻塞类型的操作，使用CIPHER_ALG_AES_256加密*/
BSP_S32 BSP_CIPHER_010_005()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_256, CIPHER_KEY_L256, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

BSP_S32 BSP_CIPHER_010_006()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {120, 20, 60};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_256, CIPHER_KEY_L256, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}


/* 提交多次CIPHER_SUBM_NONE类型的操作*/
BSP_S32 BSP_CIPHER_010_007()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;
    BSP_U32 u32BDNumber = 0;
    BSP_U32 u32SourAddr = 0;
    BSP_U32 u32DestAddr = 0;
    CIPHER_NOTIFY_STAT_E enStatus = 0;
    BSP_U32 u32Private = 0;

    submAttr = CIPHER_SUBM_NONE;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    for(i=0; ;i++)
    {
        s32Ret = BSP_CIPHER_GetCmplSrcBuff(NAS_CHN, &u32SourAddr, &u32DestAddr, 
                                             &enStatus, &u32Private);
        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("InValid RD\n",0,0,0,0,0);
        }
        else
        {
            u32BDNumber++;
            if(enStatus != CIPHER_STAT_OK)
            {
                cipherDebugMsg("state Error 0x%x\n",enStatus,0,0,0,0);    
            }
        }
        
        if(0x10 == u32BDNumber)
        {
             break;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次CIPHER_SUBM_NONE类型的操作*/
BSP_S32 BSP_CIPHER_010_008()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {120, 20, 60};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;
    BSP_U32 u32BDNumber = 0;
    BSP_U32 u32SourAddr = 0;
    BSP_U32 u32DestAddr = 0;
    CIPHER_NOTIFY_STAT_E enStatus = 0;
    BSP_U32 u32Private = 0;

    submAttr = CIPHER_SUBM_NONE;

    
    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }
    
    for(i=0; ;i++)
    {
        s32Ret = BSP_CIPHER_GetCmplSrcBuff(NAS_CHN, &u32SourAddr, &u32DestAddr, 
                                             &enStatus, &u32Private);
        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("InValid RD\n",0,0,0,0,0);
        }
        else
        {
            u32BDNumber++;
            if(enStatus != CIPHER_STAT_OK)
            {
                cipherDebugMsg("state Error 0x%x\n",enStatus,0,0,0,0);    
            }
        }
        
        if(0x10 == u32BDNumber)
        {
             break;
        }
    }
    
    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次CIPHER_SUBM_CALLBACK类型的操作*/
BSP_S32 BSP_CIPHER_010_009()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_CALLBACK;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("CALLBACK Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次CIPHER_SUBM_CALLBACK类型的操作*/
BSP_S32 BSP_CIPHER_010_010()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {120, 20, 60};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    submAttr = CIPHER_SUBM_CALLBACK;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* NONE类型和CallBack类型混合提交*/
BSP_S32 BSP_CIPHER_010_011()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }

    s32LoopforMulti = 0;

    s32Ret = BSP_CIPHER_RegistNotifyCB(cipherCbFunc_forMulti);
    if(s32Ret != BSP_OK)
    {
        cipherDebugMsg("BSP_CIPHER_RegistNotifyCB faile,ret:0x%x\n",s32Ret,0,0,0,0);
    }

    submAttr = CIPHER_SUBM_NONE;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            BSP_CIPHER_RegistNotifyCB(cipherCbFunc);
            
            return s32Ret;
        }
    }

    submAttr = CIPHER_SUBM_CALLBACK;
    
    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            BSP_CIPHER_RegistNotifyCB(cipherCbFunc);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    BSP_CIPHER_RegistNotifyCB(cipherCbFunc);
    return BSP_OK;

}


/* NONE类型和CallBack类型混合提交*/
BSP_S32 BSP_CIPHER_010_012()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;
    BSP_U32 u32BDNumber = 0;
    BSP_U32 u32SourAddr = 0;
    BSP_U32 u32DestAddr = 0;
    CIPHER_NOTIFY_STAT_E enStatus = 0;
    BSP_U32 u32Private = 0;

    submAttr = CIPHER_SUBM_CALLBACK;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    submAttr = CIPHER_SUBM_NONE;
    
    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    for(i=0; ;i++)
    {
        s32Ret = BSP_CIPHER_GetCmplSrcBuff(NAS_CHN, &u32SourAddr, &u32DestAddr, 
                                             &enStatus, &u32Private);
        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("InValid RD\n",0,0,0,0,0);
        }
        else
        {
            u32BDNumber++;
        }
        
        if(0x10 == u32BDNumber)
        {
             break;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}

/* 提交多次CIPHER_SUBM_NONE类型的操作, 一个数据包，分成18块，输出为单块*/
BSP_S32 BSP_CIPHER_010_013()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 18;
    BSP_U32 blk_len[] = {23, 45, 132, 17, 46, 80, 1, 100, 101, 10, 20, 30, 40, 6, 1023, 1024, 23, 9};
    BSP_U32 dataTestLen = 2730; 
    BSP_S32 test_times = 0x8;
    BSP_U32 u32BDNumber = 0;
    BSP_U32 u32SourAddr = 0;
    BSP_U32 u32DestAddr = 0;
    CIPHER_NOTIFY_STAT_E enStatus = 0;
    BSP_U32 u32Private = 0;

    submAttr = CIPHER_SUBM_NONE;

    
    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }
    
    for(i=0; ;i++)
    {
        s32Ret = BSP_CIPHER_GetCmplSrcBuff(NAS_CHN, &u32SourAddr, &u32DestAddr, 
                                             &enStatus, &u32Private);
        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("InValid RD\n",0,0,0,0,0);
        }
        else
        {
            u32BDNumber++;
            if(enStatus != CIPHER_STAT_OK)
            {
                cipherDebugMsg("state Error 0x%x\n",enStatus,0,0,0,0);    
            }
        }
        
        if(0x10 == u32BDNumber)
        {
             break;
        }
    }
    
    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}


BSP_S32 chain_encrypt_decrypt_multiTask(BSP_U8* caseName,BSP_U32 chnid,
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
     /*   cipherDebugMsg("test pass.\n",0,0,0,0,0);*/
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

BSP_S32 chain_encrypt_decrypt_multiTask_onechn(BSP_U8* caseName,BSP_U32 chnid,
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
     
    s32Ret = BSP_CIPHER_SingleSubmitTask(PDCP_SIGNAL_CHN,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
             &(g_cipher_uplink_cfg[chnid]), enSubmit, u32Private);             

    
    if(enSubmit == CIPHER_SUBM_CALLBACK)
    {        
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
        semTake(cipherChnCbSem[PDCP_SIGNAL_CHN],WAIT_FOREVER);
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
        
    s32Ret = BSP_CIPHER_SingleSubmitTask(PDCP_SIGNAL_CHN,(void*)blkChain2,&cipherOutBlock[chnid],
             &(g_cipher_downlink_cfg[chnid]), enSubmit, u32Private);
    if(enSubmit == CIPHER_SUBM_CALLBACK)
    {        
        if(s32Ret != CIPHER_SUCCESS)
        {
            cipherDebugMsg("BSP_CIPHER_SubmitTask fail,ret:0x%x\n",s32Ret,0,0,0,0);
            s32TestStatus = BSP_ERROR;
            goto clearup;
        }
        semTake(cipherChnCbSem[PDCP_SIGNAL_CHN],WAIT_FOREVER);
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

clearup:

    free(blkChain);
    free(blkChain2);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}



void pdcp_data_taskEntry(BSP_U32 chnid,BSP_U32 test_times,BSP_BOOL* pbDone,
                                                  CIPHER_SUBMIT_TYPE_E enSubmit)
{
   BSP_S32 i = 0;
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {1500};
   BSP_U32 dataTestLen = 1500;

   for(i = 0; i < (BSP_S32)test_times; i++)
   {
       chain_encrypt_decrypt_multiTask((BSP_U8*)__FUNCTION__,chnid, dataTestLen,blk_num,blk_len,
                        CIPHER_ALG_AES_128, CIPHER_KEY_L128, enSubmit);
   }
   *pbDone = BSP_TRUE;
}

void pdcp_data_taskEntry_onechn(BSP_U32 chnid,BSP_U32 test_times,BSP_BOOL* pbDone, 
                                                            CIPHER_SUBMIT_TYPE_E enSubmit)
{
   BSP_S32 i = 0;
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {1500};
   BSP_U32 dataTestLen = 1500;
   BSP_S32 s32Ret;

   for(i = 0; i < (BSP_S32)test_times; i++)
   {
       s32Ret = chain_encrypt_decrypt_multiTask_onechn((BSP_U8*)__FUNCTION__,chnid, dataTestLen,blk_num,blk_len,
                        CIPHER_ALG_AES_128, CIPHER_KEY_L128, enSubmit);

       if(BSP_OK != s32Ret)
       {
           cipherDebugMsg("test fail.s32Ret = 0x%x\n",s32Ret,0,0,0,0);
           break;
       }
   }
   *pbDone = BSP_TRUE;
}


void pdcp_data_taskEntry2(BSP_U32 chnid,BSP_U32 test_times,BSP_BOOL* pbDone, CIPHER_SUBMIT_TYPE_E enSubmit)
{
   BSP_S32 i = 0;
   BSP_U32 blk_num = 1500;/*CD最多可以有512个，这都设定了这么多!!!!!*/
   BSP_U32 blk_len[1500] = {0};
   BSP_U32 dataTestLen = 1500;

   for(i = 0; i < (BSP_S32)blk_num; i++)
   {
       blk_len[i] = 1;
   }

   for(i = 0; i < (BSP_S32)test_times; i++)
   {
       chain_encrypt_decrypt_multiTask((BSP_U8*)__FUNCTION__,chnid, dataTestLen,blk_num,blk_len,
                        CIPHER_ALG_AES_128, CIPHER_KEY_L128, enSubmit);
   }
   if(pbDone != NULL)
   {
       *pbDone = BSP_TRUE;
   }
}


/*
* PDCP数据双通道双任务并发，每个通道交替进行加解密操作
*/
BSP_S32 BSP_CIPHER_011_001()
{
   BSP_S32 i = 0,j = 0;
   int    taskId[PDCP_DATA_TASK_NUM];
   int    test_times = 1000;
   BSP_S32 testStatus = BSP_OK;
   BSP_BOOL bDone[] = {BSP_FALSE,BSP_FALSE};
   BSP_U32 chnid[] = {PDCP_DATA_CHN_1,PDCP_DATA_CHN_2};
   BSP_U8* taskName[] = {(BSP_U8*)"pdcp_data_task1",(BSP_U8*)"pdcp_data_task2"};
   CIPHER_SUBMIT_TYPE_E enSubmit = CIPHER_SUBM_CALLBACK;

   if(!g_cipher_test_inited)
   {
       cipherTestInit();
   }

   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskId[i] = taskSpawn((char*)taskName[i], (int)TEST_TASK_PRIORITY, (int)VX_FP_TASK,
                        (int)TEST_TASK_STACK_SIZE, (FUNCPTR)pdcp_data_taskEntry, chnid[i],(int)test_times, (int)&bDone[i], enSubmit, 0, 0, 0, 0, 0, 0);
       if(taskId[i] == 0)
       {
          testStatus = BSP_ERROR;
          for(j = 0; j < i; j++)
          {
              taskDelete(taskId[j]);
          }          
       }
   }

   while(1)
   {
       taskDelay(10);
       if((bDone[0] == BSP_TRUE)&&(bDone[1] == BSP_TRUE))
       {
            break;
       }
   }
   
   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskDelete(taskId[i]);
   }
/*   printf("g_curOptId[PDCP_DATA_CHN_1]:%d\n",g_curOptId[PDCP_DATA_CHN_1]);
   printf("g_curOptId[PDCP_DATA_CHN_2]:%d\n",g_curOptId[PDCP_DATA_CHN_2]);   
*/
   return testStatus;
}

/*
* PDCP数据双通道双任务并发，每个通道交替进行加解密操作
*/
BSP_S32 BSP_CIPHER_012_001()
{
   BSP_S32 i = 0,j = 0;
   int    taskId[PDCP_DATA_TASK_NUM];
   int    test_times = 1000;
   BSP_S32 testStatus = BSP_OK;
   BSP_BOOL bDone[] = {BSP_FALSE,BSP_FALSE};
   BSP_U32 chnid[] = {PDCP_DATA_CHN_1,PDCP_DATA_CHN_2};
   BSP_U8* taskName[] = {(BSP_U8*)"pdcp_data_task1",(BSP_U8*)"pdcp_data_task2"};
   CIPHER_SUBMIT_TYPE_E enSubmit = CIPHER_SUBM_CALLBACK;

   if(!g_cipher_test_inited)
   {
       cipherTestInit();
   }   

   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskId[i] = taskSpawn((char*)taskName[i], (int)TEST_TASK_PRIORITY, (int)VX_FP_TASK,
                        (int)TEST_TASK_STACK_SIZE, (FUNCPTR)pdcp_data_taskEntry2, (int)chnid[i],(int)test_times, (int)&bDone[i], (int)enSubmit, 0, 0, 0, 0, 0, 0);
       if(taskId[i] == 0)
       {
          testStatus = BSP_ERROR;
          for(j = 0; j < i; j++)
          {
              taskDelete(taskId[j]);
          }          
       }
   }

   while(1)
   {
       taskDelay(10);
       if((bDone[0] == BSP_TRUE)||(bDone[1] == BSP_TRUE))
       {
            break;
       }
   }
   
   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskDelete(taskId[i]);
   }
  
   return testStatus;
}

/*
* PDCP数据单通道多任务并发，通道进行加解密操作
*/
BSP_S32 BSP_CIPHER_013_001()
{
   BSP_S32 i = 0,j = 0;
   int    taskId[PDCP_DATA_TASK_NUM];
   int    test_times = 1000;
   BSP_S32 testStatus = BSP_OK;
   BSP_BOOL bDone[] = {BSP_FALSE,BSP_FALSE};
   BSP_U32 chnid[] = {PDCP_SIGNAL_CHN,PDCP_DATA_CHN_1};
   BSP_U8* taskName[] = {(BSP_U8*)"pdcp_data_task1",(BSP_U8*)"pdcp_data_task2"};
   CIPHER_SUBMIT_TYPE_E enSubmit = CIPHER_SUBM_CALLBACK;

   if(!g_cipher_test_inited)
   {
       cipherTestInit();
   }

   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskId[i] = taskSpawn((char*)taskName[i], (int)TEST_TASK_PRIORITY, (int)VX_FP_TASK,
                        (int)TEST_TASK_STACK_SIZE, (FUNCPTR)pdcp_data_taskEntry_onechn, (int)chnid[i],(int)test_times, (int)&bDone[i], (int)enSubmit, 0, 0, 0, 0, 0, 0);
       if(taskId[i] == 0)
       {
          testStatus = BSP_ERROR;
          for(j = 0; j < i; j++)
          {
              taskDelete(taskId[j]);
          }          
       }
   }

   while(1)
   {
       taskDelay(10);
       if((bDone[0] == BSP_TRUE)&&(bDone[1] == BSP_TRUE))
       {
            break;
       }
   }
   
   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskDelete(taskId[i]);
   }
/*   printf("g_curOptId[PDCP_DATA_CHN_1]:%d\n",g_curOptId[PDCP_DATA_CHN_1]);
   printf("g_curOptId[PDCP_DATA_CHN_2]:%d\n",g_curOptId[PDCP_DATA_CHN_2]);   
*/
   return testStatus;
}

void cipher_hold_taskEntry_longtime(BSP_BOOL* pbDone)
{
   BSP_S32 i = 0;
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {1500};
   BSP_U32 dataTestLen = 1500;
   BSP_S32 s32Ret;

   for(i = 0; ; i++)
   {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, CIPHER_SUBM_BLK_HOLD);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("cipher_hold_taskEntry_longtime fail,ret:0x%x\n",s32Ret,0,0,0,0);
            break;
        }

        if(0x0 == (i % 0x10))
        {
            taskDelay(0);
        }
   }
   *pbDone = BSP_TRUE;   
}

void cipher_callback_taskEntry_longtime(BSP_U32 chnid,BSP_U32 test_times,BSP_BOOL* pbDone,
                                                  CIPHER_SUBMIT_TYPE_E enSubmit)
{
   BSP_S32 i = 0;
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {1500};
   BSP_U32 dataTestLen = 1500;
   BSP_S32 s32Ret;

   for(i = 0; ; i++)
   {
        s32Ret = chain_encrypt_decrypt_multiTask((BSP_U8*)__FUNCTION__, chnid, dataTestLen,blk_num,blk_len,
                        CIPHER_ALG_AES_128, CIPHER_KEY_L128, enSubmit);
        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("cipher_callback_taskEntry_longtime fail,ret:0x%x\n",s32Ret,0,0,0,0);
            break;
        }

        if(0x0 == (i % 0x10))
        {
            taskDelay(0);
        }
        
   }
   *pbDone = BSP_TRUE;
}

/* 压力测试*/
BSP_S32 BSP_CIPHER_014_001()
{
   BSP_S32 i = 0,j = 0;
   int    taskId[3];
   int    test_times = 1000;
   BSP_S32 testStatus = BSP_OK;
   BSP_BOOL bDone[] = {BSP_FALSE,BSP_FALSE,BSP_FALSE};
   BSP_U32 chnid[] = {PDCP_SIGNAL_CHN, PDCP_DATA_CHN_1, PDCP_DATA_CHN_2};
   BSP_U8* taskName[] = {(BSP_U8*)"ltask1",(BSP_U8*)"ltask2",(BSP_U8*)"ltask3"};
   CIPHER_SUBMIT_TYPE_E enSubmit = CIPHER_SUBM_CALLBACK;

   if(!g_cipher_test_inited)
   {
       cipherTestInit();
   }

   for(i = 0; i < 0x3; i++)
   {
       if(0x0 == i)
       {
           taskId[i] = taskSpawn((char*)taskName[i], TEST_TASK_PRIORITY, VX_FP_TASK,
                        TEST_TASK_STACK_SIZE, (FUNCPTR)cipher_hold_taskEntry_longtime,
                                                    (int)&bDone[i], 0,0,0, 0, 0, 0, 0, 0, 0);
       }
       
       if((1 == i) || (2 == i))
       {
           taskId[i] = taskSpawn((char*)taskName[i], TEST_TASK_PRIORITY, VX_FP_TASK,
                        TEST_TASK_STACK_SIZE, (FUNCPTR)cipher_callback_taskEntry_longtime, 
                                 chnid[i],(int)test_times, (int)&bDone[i], enSubmit, 0, 0, 0, 0, 0, 0);
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

   while(1)
   {
       taskDelay(10);
       if((bDone[0] == BSP_TRUE) || (bDone[1] == BSP_TRUE) || (bDone[2] == BSP_TRUE))
       {
            cipherDebugMsg("long time Test fail,ret:0x%x\n",0,0,0,0,0);
            break;
       }
   }

#if 0
   for(i = 0; i < PDCP_DATA_TASK_NUM; i++)
   {
       taskDelete(taskId[i]);
   }
/*
   printf("g_curOptId[PDCP_DATA_CHN_1]:%d\n",g_curOptId[PDCP_DATA_CHN_1]);
   printf("g_curOptId[PDCP_DATA_CHN_2]:%d\n",g_curOptId[PDCP_DATA_CHN_2]);
*/
#endif

   return testStatus;
}

/* 提交多次CIPHER_SUBM_CALLBACK类型的操作*/
BSP_S32 BSP_CIPHER_014_002()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }    

    submAttr = CIPHER_SUBM_CALLBACK;

    for(i=0; ; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("CALLBACK Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

}

/* 提交多次阻塞类型的操作*/
BSP_S32 BSP_CIPHER_014_003()
{
    BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
/*
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
*/
    BSP_U32 blk_num = 3;
    BSP_U32 blk_len[] = {120, 20, 60};
    BSP_U32 dataTestLen = 200;

    if(!g_cipher_test_inited)
    {
        cipherTestInit();
    }   

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; ; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}
#if 0
BSP_S32 build_blk_chain_001(CIPHER_DATA_BLOCK* pCipherInBlock,CIPHER_DATA_BLOCK CipherBlkChain[],BSP_U32 u32TestLen,BSP_U32 blk_num,BSP_U32 blk_len[])
{
    BSP_U32 total_len = 0;
    BSP_S32 i;
    BSP_U32 tmpAddr = 0;

    for(i = 0; i < blk_num; i++)
    {
        total_len += blk_len[i];
    }
    if(u32TestLen != total_len)
    {
        cipherDebugMsg("chain total len(%d) not equal to test len(%d).\n", total_len, u32TestLen, 0, 0,0);
        return BSP_ERROR;
    }
/*    tmpAddr = pCipherInBlock->u32DataAddr;*/
    tmpAddr = (pCipherInBlock->u32DataAddr + 1);
    for(i = 0; i < blk_num; i++)
    {
        CipherBlkChain[i].u32DataAddr = tmpAddr;
        CipherBlkChain[i].u32DataLen  = blk_len[i];
        if(i == (blk_num -1))
        {
             CipherBlkChain[i].pNextBlock = NULL;
        }
        else
        {
           CipherBlkChain[i].pNextBlock  = (BSP_U32)&CipherBlkChain[i+1];
           tmpAddr += blk_len[i];
        }        
    }
    return BSP_OK;
}


BSP_S32 chain_encrypt_decrypt_001(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;

    CIPHER_DATA_BLOCK* blkChain  = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;
    BSP_U8 u8Key[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                        16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;
    BSP_U8 u8Data[3] = {0x45,0,0}; 

    enSubmit = CIPHER_SUBM_BLK_HOLD;

    /*cipherTestInit();*/

    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = NULL;

    blkChain  = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    blkChain2 = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));
    
    s32Ret = build_blk_chain_001(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = dataTestLen;
    cipherOutBlock[chnid].u32DataLen = dataTestLen;
  
    cipherTmpBlock[chnid].pNextBlock = NULL;
    cipherOutBlock[chnid].pNextBlock = NULL;
#if 1   
    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);
#endif

    for(i = 0; i < cipherInBlock[chnid].u32DataLen; i++)
    {
        *((BSP_U8*)((cipherInBlock[chnid].u32DataAddr+1)+i)) = u8Data[i];
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

    cipherTmpBlock[chnid].u32DataAddr += 1;
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
    cipherTmpBlock[chnid].u32DataAddr -= 1;
    s32Ret = build_blk_chain_001(&cipherTmpBlock[chnid],blkChain2,dataTestLen,blk_num,blk_len);
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
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

BSP_S32 BSP_CIPHER_004_001_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {3};
   BSP_U32 dataTestLen = 3;
   
   return  chain_encrypt_decrypt_001((BSP_U8*)__FUNCTION__,0x3, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L128);
}

BSP_S32 BSP_CIPHER_004_001_002()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {3};
   BSP_U32 dataTestLen = 3;
   
   return  chain_encrypt_decrypt_001((BSP_U8*)__FUNCTION__,0x3, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L192);
}

BSP_S32 BSP_CIPHER_004_001_003()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {3};
   BSP_U32 dataTestLen = 3;
   
   return  chain_encrypt_decrypt_001((BSP_U8*)__FUNCTION__,0x3, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_AES_128, CIPHER_KEY_L256);
}
#endif
#define KEY_TOTAL_SIZE (32*16)

static int kdf_rand(int a,int b)
{
	double u_ran = 0.0;
	int uab_ran = 0;
    unsigned int seed = tickGet();
    
	srand(seed);
	u_ran = rand()/(double)RAND_MAX;
	uab_ran = (int)((b-a+1)*u_ran) + a;
	return uab_ran;
}

BSP_S32 Kdf_RamRead_test(BSP_U32 u32KeyIndex, BSP_U32 u32Length)
{
    BSP_S32 i;
    BSP_S32 s32Ret;
    BSP_U32 u32Value;
    BSP_VOID *pSourceAddr = BSP_NULL;
    BSP_VOID *pMallocAddr = BSP_NULL;
    BSP_VOID *pTempAddr = BSP_NULL;

    pTempAddr = (BSP_VOID *)CIPHER_MALLOC(u32Length);

    pSourceAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET \
                               + (u32KeyIndex * CIPHER_KEY_LEN));
    pMallocAddr = (BSP_VOID *)CIPHER_MALLOC(KEY_TOTAL_SIZE);

    for(i = 0; i < (BSP_S32)u32Length/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pSourceAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }
    
    s32Ret = BSP_KDF_KeyRamRead(pMallocAddr, u32KeyIndex, u32Length);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyRamRead Error, s32Ret = 0x%x\r\n", s32Ret);
        CIPHER_FREE(pMallocAddr);
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    s32Ret = cipher_memcmp_ext(pMallocAddr, pTempAddr,u32Length);
    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pMallocAddr);
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    CIPHER_FREE(pMallocAddr);
    CIPHER_FREE(pTempAddr);
    return BSP_OK;
}

BSP_S32 Kdf_RamWrite_test(BSP_U32 u32KeyIndex, BSP_U32 u32Length)
{
    BSP_S32 s32Ret;
    BSP_VOID *pSourceAddr = BSP_NULL;
    BSP_VOID *pMallocAddr = BSP_NULL;

    pSourceAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET \
                               + (u32KeyIndex * CIPHER_KEY_LEN));
    pMallocAddr = (BSP_VOID *)CIPHER_MALLOC(KEY_TOTAL_SIZE);

    memset(pMallocAddr, 0x5a, KEY_TOTAL_SIZE);
    
    s32Ret = BSP_KDF_KeyRamWrite(u32KeyIndex, pMallocAddr, u32Length);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyRamWrite Error, s32Ret = 0x%x\r\n", s32Ret);
        CIPHER_FREE(pMallocAddr);
        return s32Ret;
    }

    s32Ret = cipher_memcmp_ext(pMallocAddr, pSourceAddr,u32Length);
    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pMallocAddr);
        return s32Ret;
    }

    CIPHER_FREE(pMallocAddr);
    return BSP_OK;
}


BSP_S32 BSP_KDF_001_001()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x10;
    BSP_U32 u32KeyIndex = 0x0;
    BSP_VOID *pDAddr = BSP_NULL;

    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);

    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }
    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen); 
    
    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_002()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x18;
    BSP_U32 u32KeyIndex = 0x0;
    BSP_VOID *pDAddr = BSP_NULL;

    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);

    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }
    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);    
    
    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_003()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x20;
    BSP_U32 u32KeyIndex = 0x0;
    BSP_VOID *pDAddr = BSP_NULL;

    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);

    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);
    
    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_004()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x10;
    BSP_U32 u32KeyIndex = 0x8;
    BSP_VOID *pDAddr = BSP_NULL;

    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);

    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);
    
    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_005()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x18;
    BSP_U32 u32KeyIndex = 0x8;
    BSP_VOID *pDAddr = BSP_NULL;
    
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);
    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);

    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_006()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x20;
    BSP_U32 u32KeyIndex = 0x8;
    BSP_VOID *pDAddr = BSP_NULL;
    
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);

    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);

    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_007()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x10;
    BSP_U32 u32KeyIndex = 0xf;
    BSP_VOID *pDAddr = BSP_NULL;
    
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);
    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);

    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_008()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x18;
    BSP_U32 u32KeyIndex = 0xf;
    BSP_VOID *pDAddr = BSP_NULL;
    
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);
    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);
    
    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_001_009()
{
    BSP_S32 i;
    BSP_U8  tmp;
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x20;
    BSP_U32 u32KeyIndex = 0xf;
    BSP_VOID *pDAddr = BSP_NULL;
    
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                        17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    pDAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + 
             CIPHER_KEYRAM_OFFSET +
             u32KeyIndex * CIPHER_KEY_LEN);
    for(i=0; i<(BSP_S32)keyLen/4; i++)
    {
        tmp        = u8Key[4*i];
        u8Key[4*i]   = u8Key[4*i+3];
        u8Key[4*i+3] = tmp;
        tmp        = u8Key[4*i+1];
        u8Key[4*i+1] = u8Key[4*i+2];
        u8Key[4*i+2] = tmp;
    }

    memcpy(pDAddr, (BSP_VOID *)u8Key, keyLen);

    s32Ret = Kdf_RamRead_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_001()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x10;
    BSP_U32 u32KeyIndex = 0x0;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_002()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x18;
    BSP_U32 u32KeyIndex = 0x0;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_003()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x20;
    BSP_U32 u32KeyIndex = 0x0;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_004()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x10;
    BSP_U32 u32KeyIndex = 0x8;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_005()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x18;
    BSP_U32 u32KeyIndex = 0x8;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_006()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x20;
    BSP_U32 u32KeyIndex = 0x8;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_007()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x10;
    BSP_U32 u32KeyIndex = 0xf;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_008()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x18;
    BSP_U32 u32KeyIndex = 0xf;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

BSP_S32 BSP_KDF_002_009()
{
    BSP_S32 s32Ret;
    BSP_U32 keyLen = 0x20;
    BSP_U32 u32KeyIndex = 0xf;

    s32Ret = Kdf_RamWrite_test(u32KeyIndex, keyLen);  

    return s32Ret;   
}

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

/* key and s all come from KeyRam*/
BSP_S32 BSP_KDF_003_001()
{
    BSP_S32 i;
    BSP_S32 s32Ret = BSP_OK;
    BSP_VOID *pDestAddr;
//    BSP_U32 u32Value;
    BSP_VOID *pTempAddr = BSP_NULL;
    BSP_U32 u32Length = 0;
    BSP_U8 u8Key[32] = {0};
    
    KEY_CONFIG_INFO_S stKeyInfo = {0, 0, 0};
    S_CONFIG_INFO_S   stSInfo   = {0, 0, 0, 0};
    KEY_MAKE_S stKeyMake        = {0, {0, 0, 0}};
    BSP_U32 u32DIndex           = 0x3;

    stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_KEYRAM;
    stKeyInfo.u32ShaKeyIndex = 0;
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
    
    s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyMake ERROR! \r\n");
        CIPHER_FREE(pTempAddr);
        return BSP_ERROR;
    }
#if 0
    for(i = 0; i < KDF_DESTKEY_LENGTH/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pDestAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }

    s32Ret = cipher_memcmp_ext((void *)pTempAddr, \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
#endif

    s32Ret = cipher_memcmp_ext((void *)(&u8Key[16]), \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));

    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    CIPHER_FREE(pTempAddr);
    return s32Ret;   
}

BSP_S32 BSP_KDF_003_002()
{
    BSP_S32 i;
    BSP_S32 s32Ret = BSP_OK;
    BSP_VOID *pDestAddr;
//    BSP_U32 u32Value;
    BSP_VOID *pTempAddr = BSP_NULL;
    BSP_U32 u32Length = 0;
    BSP_U8 u8Key[32] = {0};
    
    KEY_CONFIG_INFO_S stKeyInfo = {0, 0, 0};
    S_CONFIG_INFO_S   stSInfo   = {0, 0, 0, 0};
    KEY_MAKE_S stKeyMake        = {0, {0, 0, 0}};
    BSP_U32 u32DIndex           = 0x3;

    stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_KEYRAM;
    stKeyInfo.u32ShaKeyIndex = 0;
    stKeyInfo.pKeySourceAddr = BSP_NULL;

    stSInfo.enShaSSource = SHA_S_SOURCE_DDR;
    stSInfo.u32ShaSIndex = 0x1;
    stSInfo.u32ShaSLength = 0x7;
    stSInfo.pSAddr       = (BSP_VOID*)u32GoldenS1;

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
/*
        tmp        = u32GoldenKey[4*i];
        u32GoldenKey[4*i]   = u32GoldenKey[4*i+3];
        u32GoldenKey[4*i+3] = tmp;
        tmp        = u32GoldenKey[4*i+1];
       u32GoldenKey[4*i+1] = u32GoldenKey[4*i+2];
        u32GoldenKey[4*i+2] = tmp;
*/      
        *((BSP_U32 *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stKeyInfo.u32ShaKeyIndex) * CIPHER_KEY_LEN + 4*i))  \
        = (((BSP_U32)u32GoldenKey[4*i])<<24) | (((BSP_U32)u32GoldenKey[4*i+1])<<16) \
          | (((BSP_U32)u32GoldenKey[4*i+2])<<8) | ((BSP_U32)u32GoldenKey[4*i+3]);
    }
/*
    memcpy((void *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stKeyInfo.u32ShaKeyIndex) * CIPHER_KEY_LEN), \
           (void *)u32GoldenKey, \
           KDF_SHA_KEY_LENGTH);       
*/
    s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyMake ERROR! \r\n");
        CIPHER_FREE(pTempAddr);
        return BSP_ERROR;
    }
    
#if 0
    for(i = 0; i < KDF_DESTKEY_LENGTH/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pDestAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }

    s32Ret = cipher_memcmp_ext((void *)pTempAddr, \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
#endif

    s32Ret = cipher_memcmp_ext((void *)&u8Key[16], \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
    
    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    CIPHER_FREE(pTempAddr);
    return s32Ret;   
}

BSP_S32 BSP_KDF_003_003()
{
    BSP_S32 i;
    BSP_S32 s32Ret = BSP_OK;
    BSP_VOID *pDestAddr;
//    BSP_U32 u32Value;
    BSP_VOID *pTempAddr = BSP_NULL;
    BSP_U32 u32Length = 0;
    BSP_U8 u8Key[32] = {0};
    
    KEY_CONFIG_INFO_S stKeyInfo = {0, 0, 0};
    S_CONFIG_INFO_S   stSInfo   = {0, 0, 0, 0};
    KEY_MAKE_S stKeyMake        = {0, {0, 0, 0}};
    BSP_U32 u32DIndex           = 0x3;

    stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_DDR;
    stKeyInfo.u32ShaKeyIndex = 0;
    stKeyInfo.pKeySourceAddr = (BSP_VOID *)u32GoldenKey;

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
/*
    memcpy((void *)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + (stSInfo.u32ShaSIndex) * CIPHER_KEY_LEN), \
           (void *)u32GoldenS1, \
           KDF_SHA_S_LENGTH);
*/
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
    
    s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyMake ERROR! \r\n");
        CIPHER_FREE(pTempAddr);
        return BSP_ERROR;
    }

#if 0
    for(i = 0; i < KDF_DESTKEY_LENGTH/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pDestAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }

    s32Ret = cipher_memcmp_ext((void *)pTempAddr, \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
#endif
    s32Ret = cipher_memcmp_ext((void *)(&u8Key[16]), \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));

    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    CIPHER_FREE(pTempAddr);
    return s32Ret;   
}

BSP_S32 BSP_KDF_003_004()
{

//    BSP_S32 i;
//	BSP_U32 u32Value;
    BSP_S32 s32Ret = BSP_OK;
    BSP_VOID *pDestAddr;
    BSP_VOID *pTempAddr = BSP_NULL;
    BSP_U32 u32Length = 0;
    BSP_U8 u8Key[32] = {0};
    
    KEY_CONFIG_INFO_S stKeyInfo = {0, 0, 0};
    S_CONFIG_INFO_S   stSInfo   = {0, 0, 0, 0};
    KEY_MAKE_S stKeyMake        = {0, {0, 0, 0}};
    BSP_U32 u32DIndex           = 0x3;

    stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_DDR;
    stKeyInfo.u32ShaKeyIndex = 0;
    stKeyInfo.pKeySourceAddr = (BSP_VOID *)u32GoldenKey;

    stSInfo.enShaSSource = SHA_S_SOURCE_DDR;
    stSInfo.u32ShaSIndex = 0x1;
    stSInfo.u32ShaSLength = 0x7;
    stSInfo.pSAddr       = (BSP_VOID *)u32GoldenS1;

    stKeyMake.enKeyOutput = CIPHER_KEY_OUTPUT;
    stKeyMake.stKeyGet.enKeyLen = CIPHER_KEY_L256;
    stKeyMake.stKeyGet.penOutKeyLen = &u32Length;
    stKeyMake.stKeyGet.pKeyAddr     = (BSP_VOID *)u8Key;

    pTempAddr = (BSP_VOID *)CIPHER_MALLOC(KDF_DESTKEY_LENGTH);

    pDestAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + \
                 CIPHER_KEYRAM_OFFSET +
                 u32DIndex * CIPHER_KEY_LEN + KDF_DESTKEY_OFF);  

    s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyMake ERROR! \r\n");
        CIPHER_FREE(pTempAddr);
        return BSP_ERROR;
    }
#if 0
    for(i = 0; i < KDF_DESTKEY_LENGTH/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pDestAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }

    s32Ret = cipher_memcmp_ext((void *)pTempAddr, \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
#endif
    
    s32Ret = cipher_memcmp_ext((void *)(&u8Key[16]), \
                                  (void *)u32GoldenResult1,\
                                  (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));

    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    CIPHER_FREE(pTempAddr);
    return s32Ret;   
}


BSP_S32 BSP_KDF_004_001()
{
//    BSP_S32 i;
//	BSP_U32 u32Value;
    BSP_S32 s32Ret = BSP_OK;
    BSP_VOID *pDestAddr;
    BSP_VOID *pTempAddr = BSP_NULL;
    BSP_U32 u32Length = 0;
    BSP_U8 u8Key[32] = {0};
    
    KEY_CONFIG_INFO_S stKeyInfo = {0, 0, 0};
    S_CONFIG_INFO_S   stSInfo   = {0, 0, 0, 0};
    KEY_MAKE_S stKeyMake        = {0, {0, 0, 0}};
    BSP_U32 u32DIndex           = 0x3;

    stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_DDR;
    stKeyInfo.u32ShaKeyIndex = 0;
    stKeyInfo.pKeySourceAddr = (BSP_VOID *)u32GoldenKey;

    stSInfo.enShaSSource = SHA_S_SOURCE_DDR;
    stSInfo.u32ShaSIndex = 0x1;
    stSInfo.u32ShaSLength = 0x7;
    stSInfo.pSAddr       = (BSP_VOID *)u32GoldenS1;

    stKeyMake.enKeyOutput = CIPHER_KEY_OUTPUT;
    stKeyMake.stKeyGet.enKeyLen = CIPHER_KEY_L256;
    stKeyMake.stKeyGet.penOutKeyLen = &u32Length;
    stKeyMake.stKeyGet.pKeyAddr     = (BSP_VOID *)u8Key;

    pTempAddr = (BSP_VOID *)CIPHER_MALLOC(KDF_DESTKEY_LENGTH);

    pDestAddr = (BSP_VOID *)(CIPHER_REGBASE_ADDR + \
                 CIPHER_KEYRAM_OFFSET +
                 u32DIndex * CIPHER_KEY_LEN + KDF_DESTKEY_OFF);  

    s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyMake ERROR! \r\n");
        CIPHER_FREE(pTempAddr);
        return BSP_ERROR;
    }

#if 0
    for(i = 0; i < KDF_DESTKEY_LENGTH/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pDestAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }

    s32Ret = cipher_memcmp_ext((void *)pTempAddr, \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
#endif
    s32Ret = cipher_memcmp_ext((void *)(&u8Key[16]), \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
 
    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

/*    printf("The second KEY");*/
    stKeyInfo.enShaKeySource = SHA_KEY_SOURCE_FOLLOW;
    stKeyInfo.u32ShaKeyIndex = 7;
    stKeyInfo.pKeySourceAddr = (BSP_VOID *)u32GoldenKey;

    stSInfo.enShaSSource = SHA_S_SOURCE_DDR;
    stSInfo.u32ShaSIndex = 0x1;
    stSInfo.u32ShaSLength = 0x7;
    stSInfo.pSAddr       = (BSP_VOID *)u32GoldenS1;

    stKeyMake.enKeyOutput = CIPHER_KEY_OUTPUT;
    stKeyMake.stKeyGet.enKeyLen = CIPHER_KEY_L256;
    stKeyMake.stKeyGet.penOutKeyLen = &u32Length;
    stKeyMake.stKeyGet.pKeyAddr     = (BSP_VOID *)u8Key;
    
    s32Ret = BSP_KDF_KeyMake(&stKeyInfo, &stSInfo, u32DIndex, &stKeyMake);
    if(BSP_OK != s32Ret)
    {
        printf("BSP_KDF_KeyMake ERROR! \r\n");
        CIPHER_FREE(pTempAddr);
        return BSP_ERROR;
    }
#if 0
    for(i = 0; i < KDF_DESTKEY_LENGTH/4; i++)
    {
        u32Value =  (*(BSP_U32 *)((BSP_U32)pDestAddr+4*i));
        u32Value =  CIPHER_BSWAP32(u32Value);
        (*(BSP_U32 *)((BSP_U32)pTempAddr + 4*i)) = u32Value;
    }

    s32Ret = cipher_memcmp_ext((void *)pTempAddr, \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));
#endif
    s32Ret = cipher_memcmp_ext((void *)(&u8Key[16]), \
                               (void *)u32GoldenResult1,\
                               (CIPHER_KEY_LEN - KDF_DESTKEY_OFF));

    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        CIPHER_FREE(pTempAddr);
        return s32Ret;
    }

    CIPHER_FREE(pTempAddr);
    return s32Ret;   
}

BSP_S32 BSP_KDF_005_001()
{
    BSP_S32 i;
    BSP_S32 s32Ret;
    
    for(i=0; ; i++)
    {
        s32Ret = BSP_KDF_003_001();

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_KDF_005_001 fail,s32Ret =:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }
}

BSP_S32 BSP_KDF_006_001()
{
    BSP_S32 s32Ret;
    BSP_U32 u32Kenlength;
    BSP_U32 u32KeyIndex = 0x6;
    CIPHER_KEY_LEN_E keyLen = CIPHER_KEY_L192;
    KEY_GET_S stKey = {0, 0, 0};
    BSP_U8 u8Key[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
                        18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    BSP_U8 u8DestKey[32] = {0};
    
    s32Ret = BSP_CIPHER_SetKey(u8Key, keyLen, u32KeyIndex);
    if(s32Ret != CIPHER_SUCCESS)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey fail,ret:0x%x\n",s32Ret,0,0,0,0);
        return BSP_ERROR;
    }

    stKey.enKeyLen = CIPHER_KEY_L192;
    stKey.penOutKeyLen = &u32Kenlength;
    stKey.pKeyAddr     = (BSP_VOID *)u8DestKey;

    s32Ret = BSP_CIPHER_GetKey(u32KeyIndex, &stKey);

    s32Ret = cipher_memcmp_ext((void *)u8Key, \
                               (void *)u8DestKey,\
                               (0x18));

    if(BSP_OK != s32Ret)
    {
        printf("cipher_memcmp_ext Error\r\n");
        return s32Ret;
    }
    
    return s32Ret;
}

/*
* cipherInBlock:  输入数据长度为1个Byte，golden数据提供的理论值
* cipherTmpBlock: cipher根据cipherInBlock算出来的值
* cipherOutBlock: 存放golden数据提供的mac理论值
*Test1 完整性
*/
#define INPUT_DATA_LEN_TEST_CASE1 1
BSP_S32 chain_protect_check_nas_longmac_validate_golden1(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;
    CIPHER_DATA_BLOCK* blkChain = NULL;
    BSP_U8 u8Key[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;
    BSP_U8 u8Msg[INPUT_DATA_LEN_TEST_CASE1] = {0};
    BSP_U8 u8Mac[MAC_SIZE_LONG]={0x39,0x0a,0x91,0xb7};

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;


    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = 0;

    blkChain = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));

    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = MAC_SIZE_LONG;
    cipherTmpBlock[chnid].pNextBlock = 0;

    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);

    /*输入message*/
    for(i = 0; i < INPUT_DATA_LEN_TEST_CASE1; i++)
    {
      *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr + i)) = u8Msg[i];
    }
    /*MAC赋值*/
    for(i = 0; i < MAC_SIZE_LONG; i++)
    {
      *((BSP_U8*)(cipherOutBlock[chnid].u32DataAddr + i)) = u8Mac[i];
    }

    s32Ret = BSP_CIPHER_SetKey(u8Key, keyLen, u32KeyIndex);
    if(s32Ret != CIPHER_SUCCESS)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey fail,ret:0x%x\n",s32Ret,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }
    
    /*完成上行业务*/
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_PROTECT_LMAC;
    g_cipher_uplink_cfg[chnid].u8BearId = 0x0;
    g_cipher_uplink_cfg[chnid].u8Direction = 0;/*0表示上行*/
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0x0;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
                 &(g_cipher_uplink_cfg[chnid]),enSubmit, u32Private);
    }
    
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

    cipherDebugMsg("Message     Addr =0x%x\n",cipherInBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Cipher MAC  Addr =0x%x\n",cipherTmpBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Golden MAC  Addr =0x%x\n",cipherOutBlock[chnid].u32DataAddr,0,0,0,0);
    
    /* 生成的MAC与逻辑提供的golden数据进行比较*/
    s32Ret = cipherDataCmp(&cipherTmpBlock[chnid],&cipherOutBlock[chnid]);
    if(s32Ret != BSP_OK)
    {
         cipherDebugMsg("data cmp fail.\n",0,0,0,0,0);
         s32TestStatus = BSP_ERROR;
         goto clearup;
    }
    
clearup:
    free(blkChain);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

BSP_S32 BSP_CIPHER_GOLDEN_INT_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {INPUT_DATA_LEN_TEST_CASE1};
   BSP_U32 dataTestLen = INPUT_DATA_LEN_TEST_CASE1;

   return chain_protect_check_nas_longmac_validate_golden1((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128);
}

/*
* cipherInBlock:  输入数据长度为76个Byte，golden数据提供的原文
* cipherTmpBlock: cipher根据cipherInBlock算出来的值
* cipherOutBlock: 存放golden数据提供的密文
* Test2 完整性
*/
#define INPUT_DATA_LEN_TEST_CASE3 76
BSP_S32 chain_protect_check_nas_longmac_validate_golden2(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;
    CIPHER_DATA_BLOCK* blkChain = NULL;
    BSP_U8 u8Key[32] = {0xc9,0xe6,0xce,0xc4,0x60,0x7c,0x72,0xdb,\
                        0x00,0x0a,0xef,0xa8,0x83,0x85,0xab,0x0a};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;
    BSP_U8 u8Msg[INPUT_DATA_LEN_TEST_CASE3] = {\
                        0x98,0x3b,0x41,0xd4,0x7d,0x78,0x0c,0x9e,\
                        0x1a,0xd1,0x1d,0x7e,0xb7,0x03,0x91,0xb1,\
                        0xde,0x0b,0x35,0xda,0x2d,0xc6,0x2f,0x83,\
                        0xe7,0xb7,0x8d,0x63,0x06,0xca,0x0e,0xa0,\
                        0x7e,0x94,0x1b,0x7b,0xe9,0x13,0x48,0xf9,\
                        0xfc,0xb1,0x70,0xe2,0x21,0x7f,0xec,0xd9,\
                        0x7f,0x9f,0x68,0xad,0xb1,0x6e,0x5d,0x7d,\
                        0x21,0xe5,0x69,0xd2,0x80,0xed,0x77,0x5c,\
                        0xeb,0xde,0x3f,0x40,0x93,0xc5,0x38,0x81,\
                        0x00,0x00,0x00,0x00};
    BSP_U32 u32Mac[MAC_SIZE_LONG]={0x99,0xd8,0xa4,0x1f};

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;


    /*配置数据加解密缓冲区*/
    cipherInBlock[chnid].u32DataLen = dataTestLen;
    cipherInBlock[chnid].pNextBlock = 0;

    blkChain = (CIPHER_DATA_BLOCK*)malloc(blk_num*sizeof(CIPHER_DATA_BLOCK));

    s32Ret = build_blk_chain(&cipherInBlock[chnid],blkChain,dataTestLen,blk_num,blk_len);
    if(s32Ret != BSP_OK)
    {
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }    

    cipherTmpBlock[chnid].u32DataLen = MAC_SIZE_LONG;
    cipherTmpBlock[chnid].pNextBlock = 0;

    memset((void*)cipherInBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherTmpBlock[chnid].u32DataAddr,0,dataTestLen + 20);
    memset((void*)cipherOutBlock[chnid].u32DataAddr,0,dataTestLen + 20);

    /*输入message*/
    for(i = 0; i < INPUT_DATA_LEN_TEST_CASE3; i++)
    {
      *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = u8Msg[i];
    }
    /*MAC赋值*/
    for(i = 0; i < MAC_SIZE_LONG; i++)
    {
      *((BSP_U8*)(cipherOutBlock[chnid].u32DataAddr+i)) = u32Mac[i];
    }

    s32Ret = BSP_CIPHER_SetKey(u8Key, keyLen, u32KeyIndex);
    if(s32Ret != CIPHER_SUCCESS)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey fail,ret:0x%x\n",s32Ret,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }
    
    /*完成上行业务*/
    g_cipher_uplink_cfg[chnid].enOpt = CIPHER_OPT_PROTECT_LMAC;
    g_cipher_uplink_cfg[chnid].u8BearId = 0x0a;
    g_cipher_uplink_cfg[chnid].u8Direction = 1;/*0表示上行*/
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0xa94059da;
    g_cipher_uplink_cfg[chnid].bMemBlock = 0;
    g_cipher_uplink_cfg[chnid].u32BlockLen = 0;
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = 0;    
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = u32KeyIndex;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = 0;   
    g_cipher_uplink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = alg;      

    {
        s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
                 &(g_cipher_uplink_cfg[chnid]),enSubmit, u32Private);
    }
    
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

    cipherDebugMsg("Message     Addr =0x%x\n",cipherInBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Cipher MAC  Addr =0x%x\n",cipherTmpBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Golden MAC  Addr =0x%x\n",cipherOutBlock[chnid].u32DataAddr,0,0,0,0);

    /* 生成的MAC与逻辑提供的golden数据进行比较*/
    s32Ret = cipherDataCmp(&cipherTmpBlock[chnid],&cipherOutBlock[chnid]);
    if(s32Ret != BSP_OK)
    {
         cipherDebugMsg("data cmp fail.\n",0,0,0,0,0);
         s32TestStatus = BSP_ERROR;
         goto clearup;
    }
    
clearup:
    free(blkChain);
    if(s32TestStatus == BSP_OK)
    {
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

BSP_S32 BSP_CIPHER_GOLDEN_INT_002()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {INPUT_DATA_LEN_TEST_CASE3};
   BSP_U32 dataTestLen = INPUT_DATA_LEN_TEST_CASE3;

   return chain_protect_check_nas_longmac_validate_golden2((BSP_U8*)__FUNCTION__,NAS_CHN,dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128);
}

/*
 cipherInBlock:  输入数据长度为28个Byte，golden数据提供的理论值
 cipherTmpBlock: cipher根据cipherInBlock算出来的值
 cipherOutBlock: 存放golden数据提供的mac理论值
 Test1 加密
*/
#define INPUT_DATA_LEN_SEC_TESTCASE1 24
BSP_S32 chain_encrypt_decrypt_validate_golden1(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;

    CIPHER_DATA_BLOCK* blkChain  = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;
    BSP_U8 u8Key[32] = {0x17,0x3d,0x14,0xba,0x50,0x03,0x73,0x1d,\
                        0x7a,0x60,0x04,0x94,0x70,0xf0,0x0a,0x29};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;
    BSP_U8 u8PlainText[INPUT_DATA_LEN_SEC_TESTCASE1] = {\
                              0x6c,0xf6,0x53,0x40,0x73,0x55,0x52,0xab,\
                              0x0c,0x97,0x52,0xfa,0x6f,0x90,0x25,0xfe,\
                              0x0b,0xd6,0x75,0xd9,0x00,0x58,0x75,0xb2};
    BSP_U8 u8CipherText[INPUT_DATA_LEN_SEC_TESTCASE1] = {\
                              0xa6,0xc8,0x5f,0xc6,0x6a,0xfb,0x85,0x33,\
                              0xaa,0xfc,0x25,0x18,0xdf,0xe7,0x84,0x94,\
                              0x0e,0xe1,0xe4,0xb0,0x30,0x23,0x8c,0xc8};

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;

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

    for(i = 0; i < INPUT_DATA_LEN_SEC_TESTCASE1; i++)
    {
        *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = u8PlainText[i];
    }
    for(i = 0; i < INPUT_DATA_LEN_SEC_TESTCASE1; i++)
    {
        *((BSP_U8*)(cipherOutBlock[chnid].u32DataAddr+i)) = u8CipherText[i];
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
    g_cipher_uplink_cfg[chnid].u8BearId = 0x0f;
    g_cipher_uplink_cfg[chnid].u8Direction = 0;/*0表示上行*/
    g_cipher_uplink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_uplink_cfg[chnid].u32Count = 0x66035492;
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
    
    cipherDebugMsg("Message Addr =0x%x\n",cipherInBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Cipher  Addr =0x%x\n",cipherTmpBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Golden  Addr =0x%x\n",cipherOutBlock[chnid].u32DataAddr,0,0,0,0);

    /* 比较算法加密后的密文和golden数据提供的密文*/
    s32Ret = cipherDataCmp(&cipherTmpBlock[chnid],&cipherOutBlock[chnid]);
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
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

/*上行数据加密*/
BSP_S32 BSP_CIPHER_GOLDEN_SEC_001()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {INPUT_DATA_LEN_SEC_TESTCASE1};
   BSP_U32 dataTestLen = INPUT_DATA_LEN_SEC_TESTCASE1;
   
   return  chain_encrypt_decrypt_validate_golden1((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128);
}

/*
 cipherInBlock:  输入数据长度为100个Byte，golden数据提供的原文
 cipherTmpBlock: cipher根据cipherInBlock算出来的值
 cipherOutBlock: 存放golden数据提供的密文理论值
 Test2 加密
*/
#define INPUT_DATA_LEN_SEC_TESTCASE2 100
/* 加密操作*/
BSP_S32 chain_encrypt_decrypt_validate_golden2(BSP_U8* caseName,BSP_U32 chnid,
                    BSP_U32 dataTestLen,BSP_U32 blk_num,BSP_U32 blk_len[],
                    CIPHER_ALGORITHM_E alg, CIPHER_KEY_LEN_E keyLen)
{
    BSP_S32 s32TestStatus = BSP_OK;
    BSP_S32 s32Ret = BSP_OK;
    BSP_S32 i = 0;

    CIPHER_DATA_BLOCK* blkChain  = NULL;
    CIPHER_DATA_BLOCK* blkChain2 = NULL;
    BSP_U8 u8Key[32] = {0xe5,0xbd,0x3e,0xa0,0xeb,0x55,0xad,0xe8,\
                        0x66,0xc6,0xac,0x58,0xbd,0x54,0x30,0x2a};
    BSP_U32 u32KeyIndex = 0x1;
    BSP_U32 u32Private = 0x0;
    CIPHER_SUBMIT_TYPE_E enSubmit;
    BSP_U32 u8PlainText[INPUT_DATA_LEN_SEC_TESTCASE2] = {\
                                 0x14,0xa8,0xef,0x69,0x3d,0x67,0x85,0x07,\
                                 0xbb,0xe7,0x27,0x0a,0x7f,0x67,0xff,0x50,\
                                 0x06,0xc3,0x52,0x5b,0x98,0x07,0xe4,0x67,\
                                 0xc4,0xe5,0x60,0x00,0xba,0x33,0x8f,0x5d,\
                                 0x42,0x95,0x59,0x03,0x67,0x51,0x82,0x22,\
                                 0x46,0xc8,0x0d,0x3b,0x38,0xf0,0x7f,0x4b,\
                                 0xe2,0xd8,0xff,0x58,0x05,0xf5,0x13,0x22,\
                                 0x29,0xbd,0xe9,0x3b,0xbb,0xdc,0xaf,0x38,\
                                 0x2b,0xf1,0xee,0x97,0x2f,0xbf,0x99,0x77,\
                                 0xba,0xda,0x89,0x45,0x84,0x7a,0x2a,0x6c,\
                                 0x9a,0xd3,0x4a,0x66,0x75,0x54,0xe0,0x4d,\
                                 0x1f,0x7f,0xa2,0xc3,0x32,0x41,0xbd,0x8f,\
                                 0x01,0xba,0x22,0x0d};  
    BSP_U32 u8CipherText[INPUT_DATA_LEN_SEC_TESTCASE2] = {\
                                 0x13,0x1d,0x43,0xe0,0xde,0xa1,0xbe,0x5c,\
                                 0x5a,0x1b,0xfd,0x97,0x1d,0x85,0x2c,0xbf,\
                                 0x71,0x2d,0x7b,0x4f,0x57,0x96,0x1f,0xea,\
                                 0x32,0x08,0xaf,0xa8,0xbc,0xa4,0x33,0xf4,\
                                 0x56,0xad,0x09,0xc7,0x41,0x7e,0x58,0xbc,\
                                 0x69,0xcf,0x88,0x66,0xd1,0x35,0x3f,0x74,\
                                 0x86,0x5e,0x80,0x78,0x1d,0x20,0x2d,0xfb,\
                                 0x3e,0xcf,0xf7,0xfc,0xbc,0x3b,0x19,0x0f,\
                                 0xe8,0x2a,0x20,0x4e,0xd0,0xe3,0x50,0xfc,\
                                 0x0f,0x6f,0x26,0x13,0xb2,0xf2,0xbc,0xa6,\
                                 0xdf,0x5a,0x47,0x3a,0x57,0xa4,0xa0,0x0d,\
                                 0x98,0x5e,0xba,0xd8,0x80,0xd6,0xf2,0x38,\
                                 0x64,0xa0,0x7b,0x01};

    if(!g_cipher_test_inited)
    {
       cipherTestInit();
    }  

    enSubmit = CIPHER_SUBM_CALLBACK;

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

    for(i = 0; i < INPUT_DATA_LEN_SEC_TESTCASE2; i++)
    {
        *((BSP_U8*)(cipherInBlock[chnid].u32DataAddr+i)) = u8PlainText[i];
    }
    for(i = 0; i < INPUT_DATA_LEN_SEC_TESTCASE2; i++)
    {
        *((BSP_U8*)(cipherOutBlock[chnid].u32DataAddr+i)) = u8CipherText[i];
    }

    s32Ret = BSP_CIPHER_SetKey(u8Key, keyLen, u32KeyIndex);
    if(s32Ret != CIPHER_SUCCESS)
    {
        cipherDebugMsg("BSP_CIPHER_SetKey fail,ret:0x%x\n",s32Ret,0,0,0,0);
        s32TestStatus = BSP_ERROR;
        goto clearup;
    }

    /*完成下行业务*/
    g_cipher_downlink_cfg[chnid].enOpt = CIPHER_OPT_ENCRYPT;
    g_cipher_downlink_cfg[chnid].u8BearId = 0x18;
    g_cipher_downlink_cfg[chnid].u8Direction = 1;/*0表示上行*/
    g_cipher_downlink_cfg[chnid].enAppdHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].enHeaderLen = CIPHER_HDR_BIT_TYPE_0;
    g_cipher_downlink_cfg[chnid].u32Count = 0x56823;
    g_cipher_downlink_cfg[chnid].bMemBlock = 0;
    g_cipher_downlink_cfg[chnid].u32BlockLen = 0;
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexSec = u32KeyIndex;    
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.u32KeyIndexInt = 0;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgSecurity = alg;   
    g_cipher_downlink_cfg[chnid].stAlgKeyInfo.enAlgIntegrity = 0; 
     
    s32Ret = BSP_CIPHER_SingleSubmitTask(chnid,(void*)blkChain,(void*)&cipherTmpBlock[chnid],
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

    cipherDebugMsg("Message Addr =0x%x\n",cipherInBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Cipher  Addr =0x%x\n",cipherTmpBlock[chnid].u32DataAddr,0,0,0,0);
    cipherDebugMsg("Golden  Addr =0x%x\n",cipherOutBlock[chnid].u32DataAddr,0,0,0,0);

    /* 比较算法加密后的密文和golden数据提供的密文*/
    s32Ret = cipherDataCmp(&cipherTmpBlock[chnid],&cipherOutBlock[chnid]);
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
        cipherDebugMsg("test pass.\n",0,0,0,0,0);
    }
    else
    {
        cipherDebugMsg("test fail.\n",0,0,0,0,0); 
    }
    return s32TestStatus;
}

/* 下行数据加密*/
BSP_S32 BSP_CIPHER_GOLDEN_SEC_002()
{
   BSP_U32 blk_num = 1;
   BSP_U32 blk_len[] = {INPUT_DATA_LEN_SEC_TESTCASE2};
   BSP_U32 dataTestLen = INPUT_DATA_LEN_SEC_TESTCASE2;
   
   return  chain_encrypt_decrypt_validate_golden2((BSP_U8*)__FUNCTION__,NAS_CHN, dataTestLen,blk_num,blk_len,
                    CIPHER_ALG_ZUC, CIPHER_KEY_L128);
}

#ifdef CONFIG_CCORE_PM
#include <bsp_dpm.h>
#include <osl_bio.h>
extern s32 bsp_cipher_resume(struct dpm_device *dev);
extern s32 bsp_cipher_suspend(struct dpm_device *dev);
BSP_S32 re_su_test()
{
	BSP_S32 i = 0;
    BSP_S32 s32Ret;
    CIPHER_SUBMIT_TYPE_E submAttr;
    BSP_U32 blk_num = 1;
    BSP_U32 blk_len[] = {200};
    BSP_U32 dataTestLen = 200; 
    BSP_S32 test_times = 0x8;
	struct dpm_device *dev = NULL;
	u32 reg;
	u32 ii = 100000;

    submAttr = CIPHER_SUBM_BLK_HOLD;

    for(i=0; i<test_times; i++)
    {
        s32Ret = chain_encrypt_decrypt_multitype((BSP_U8*)__FUNCTION__,NAS_CHN, 
                dataTestLen,blk_num,blk_len, CIPHER_ALG_AES_128, CIPHER_KEY_L128, submAttr);


		if(i == 4)
		{
			printf("%X\n",readl(0x900ab100));
			printf("%X\n",readl(0x900ab104));
			printf("%X\n",readl(0x900ab108));
			printf("%X\n",readl(0x900ab10c));
			printf("%X\n",readl(0x900ab110));
			printf("%X\n",readl(0x900ab114));
			printf("%X\n",readl(0x900ab118));
			printf("%X\n",readl(0x900ab11c));
			printf("%X\n",readl(0x900ab120));
			printf("%X\n",readl(0x900ab124));
			
			s32Ret = bsp_cipher_suspend(dev);
			if(s32Ret)
				return s32Ret;
			reg = readl(0x900ab000);
			reg |= 0x1;
			writel(reg, 0x900ab000);
			while(ii-- > 0);
			printf("%X\n",readl(0x900ab100));
			printf("%X\n",readl(0x900ab104));
			printf("%X\n",readl(0x900ab108));
			printf("%X\n",readl(0x900ab10c));
			printf("%X\n",readl(0x900ab110));
			printf("%X\n",readl(0x900ab114));
			printf("%X\n",readl(0x900ab118));
			printf("%X\n",readl(0x900ab11c));
			printf("%X\n",readl(0x900ab120));
			printf("%X\n",readl(0x900ab124));
			
			bsp_cipher_resume(dev);
			
			printf("%X\n",readl(0x900ab100));
			printf("%X\n",readl(0x900ab104));
			printf("%X\n",readl(0x900ab108));
			printf("%X\n",readl(0x900ab10c));
			printf("%X\n",readl(0x900ab110));
			printf("%X\n",readl(0x900ab114));
			printf("%X\n",readl(0x900ab118));
			printf("%X\n",readl(0x900ab11c));
			printf("%X\n",readl(0x900ab120));
			printf("%X\n",readl(0x900ab124));
		}
		

        if(BSP_OK != s32Ret)
        {
            cipherDebugMsg("BSP_CIPHER_Purge Test 1 fail,ret:0x%x\n",s32Ret,0,0,0,0);
            return s32Ret;
        }
    }

    cipherDebugMsg("test pass.\n",0,0,0,0,0);
    return BSP_OK;
}
#endif
