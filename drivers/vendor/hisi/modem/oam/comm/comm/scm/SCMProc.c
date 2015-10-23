

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "SCMProc.h"
/* log2.0 2014-03-19 Begin:*/
#include "NvIdList.h"
#include "omnvinterface.h"
#include "NVIM_Interface.h"
#include  <diag_buf_ctrl.h>
#include <diag_debug.h>
/* log2.0 2014-03-19 End*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* lint -e767  */
#define    THIS_FILE_ID        PS_FILE_ID_SCM_PROC_C
/* lint +e767  */

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
#if (VOS_OS_VER == VOS_WIN32)       /* PC Stub */
SCM_CODER_SRC_CFG_STRU      g_astSCMCoderSrcCfg[SCM_CODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_CNF1, SOCP_CODER_DST_LOM_CNF,  SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_DATA_TYPE_3, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_IND1, SOCP_CODER_DST_LOM_IND,  SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_DATA_TYPE_2, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GU_OM1,   SOCP_CODER_DST_GU_OM,    SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_LIST, SOCP_DATA_TYPE_1, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GU_OM2,   SOCP_CODER_DST_GU_OM,    SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_LIST, SOCP_DATA_TYPE_1, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE}
};

SCM_CODER_DEST_CFG_STRU     g_astSCMCoderDstCfg[SCM_CODER_DST_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_LOM_CNF, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_CNF_SIZE, SCM_CODER_DST_THRESHOLD_L, VOS_NULL_PTR},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_LOM_IND, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_IND_SIZE, SCM_CODER_DST_THRESHOLD_L, VOS_NULL_PTR},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_GU_OM,   VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_IND_SIZE, SCM_CODER_DST_THRESHOLD_GU, VOS_NULL_PTR}
};

SCM_DECODER_SRC_CFG_STRU    g_astSCMDecoderSrcCfg[SCM_DECODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_DECODER_SRC_LOM, VOS_NULL_PTR, VOS_NULL_PTR, SCM_DECODER_SRC_SIZE}
};

SCM_DECODER_DEST_CFG_STRU   g_astSCMDecoderDstCfg[SCM_DECODER_DST_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_DECODER_DST_LOM, SOCP_DECODER_SRC_LOM, SOCP_DATA_TYPE_0, VOS_NULL_PTR, VOS_NULL_PTR, SCM_DECODER_DST_SIZE, SCM_DECODER_DST_THRESHOLD, VOS_NULL_PTR}
};
#else

#if (OSA_CPU_CCPU == VOS_OSA_CPU)

#if (RAT_MODE != RAT_GU)    /* Mutil Mode */
SCM_CODER_SRC_CFG_STRU      g_astSCMCoderSrcCfg[SCM_CODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_IND2, SOCP_CODER_DST_LOM_IND, SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_3, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_IND3, SOCP_CODER_DST_LOM_IND, SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_0, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_CNF2, SOCP_CODER_DST_LOM_IND, SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_2, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GU_OM2,   SOCP_CODER_DST_GU_OM,   SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_1, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE}
};
#else                           /* GU  Single Mode */
SCM_CODER_SRC_CFG_STRU      g_astSCMCoderSrcCfg[SCM_CODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GU_OM2,   SOCP_CODER_DST_GU_OM,   SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_1, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE}
};
#endif  /* (RAT_MODE != RAT_GU) */

#endif /*(OSA_CPU_CCPU == VOS_OSA_CPU)*/

#if (OSA_CPU_ACPU == VOS_OSA_CPU)

#if (RAT_MODE != RAT_GU)    /* Mutil Mode */
SCM_CODER_SRC_CFG_STRU      g_astSCMCoderSrcCfg[SCM_CODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_CNF1, SOCP_CODER_DST_LOM_CNF,  SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_3, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_LOM_IND1, SOCP_CODER_DST_LOM_IND,  SOCP_DATA_TYPE_0, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_2, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GU_OM1,   SOCP_CODER_DST_GU_OM,    SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_1, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE}
};

SCM_CODER_DEST_CFG_STRU     g_astSCMCoderDstCfg[SCM_CODER_DST_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_LOM_CNF, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_CNF_SIZE, SCM_CODER_DST_THRESHOLD_L,   VOS_NULL_PTR},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_LOM_IND, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_IND_SIZE, SCM_CODER_DST_THRESHOLD_L,   VOS_NULL_PTR},
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_GU_OM,   VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_IND_SIZE, SCM_CODER_DST_THRESHOLD_GU,  VOS_NULL_PTR}
};

SCM_DECODER_SRC_CFG_STRU    g_astSCMDecoderSrcCfg[SCM_DECODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_DECODER_SRC_LOM, VOS_NULL_PTR, VOS_NULL_PTR, SCM_DECODER_SRC_SIZE}
};

SCM_DECODER_DEST_CFG_STRU   g_astSCMDecoderDstCfg[SCM_DECODER_DST_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_DECODER_DST_LOM, SOCP_DECODER_SRC_LOM, SOCP_DATA_TYPE_0, VOS_NULL_PTR, VOS_NULL_PTR, SCM_DECODER_DST_SIZE, SCM_DECODER_DST_THRESHOLD, VOS_NULL_PTR}
};
#else                           /* GU Single Mode */
SCM_CODER_SRC_CFG_STRU      g_astSCMCoderSrcCfg[SCM_CODER_SRC_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_SRC_GU_OM1, SOCP_CODER_DST_GU_OM, SOCP_DATA_TYPE_1, SOCP_ENCSRC_CHNMODE_LIST, SOCP_CHAN_PRIORITY_1, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_BDSIZE, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_SRC_RDSIZE}
};

SCM_CODER_DEST_CFG_STRU     g_astSCMCoderDstCfg[SCM_CODER_DST_NUM]=
{
    {SCM_CHANNEL_UNINIT, SOCP_CODER_DST_GU_OM, VOS_NULL_PTR, VOS_NULL_PTR, SCM_CODER_DST_IND_SIZE, SCM_CODER_DST_THRESHOLD_GU,  VOS_NULL_PTR}
};
#endif /* (RAT_MODE != RAT_GU) */

#endif /* (OSA_CPU_ACPU == VOS_OSA_CPU) */

#endif /* (VOS_OS_VER == VOS_WIN32) */

SCM_INFODATA_STRU           g_stSCMInfoData;    /* 用于保存log信息 */

/* c核单独复位回调函数中需要关闭的编译源通道 */
#if (FEATURE_ON == FEATURE_SOCP_CHANNEL_REDUCE)
SOCP_CODER_SRC_ENUM_U32     g_ulCloseSrcNum[] =
{
    SOCP_CODER_SRC_GU_OM2,          /* GU OM诊断消息 */
    SOCP_CODER_SRC_GUBBP1,          /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUBBP2,          /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUDSP1,          /* GUDSP诊断消息 */
    SOCP_CODER_SRC_GUDSP2,          /* GUDSP诊断消息 */
};

#else
SOCP_CODER_SRC_ENUM_U32     g_ulCloseSrcNum[] =
{
    SOCP_CODER_SRC_GU_OM2,          /* GU OM诊断消息 */
    SOCP_CODER_SRC_RFU,             /* 保留 */
    SOCP_CODER_SRC_HIFI,            /* GU HIFI诊断消息 */
    SOCP_CODER_SRC_MUTIL_MEDIA1,    /* 媒体诊断消息 */
    SOCP_CODER_SRC_MUTIL_MEDIA2,    /* 媒体诊断消息 */
    SOCP_CODER_SRC_MCU1,            /* MCU诊断消息 */
    SOCP_CODER_SRC_MCU2,            /* MCU诊断消息 */
    SOCP_CODER_SRC_LDSP1,           /* LDSP诊断消息 */
    SOCP_CODER_SRC_LDSP2,           /* LDSP诊断消息 */
    SOCP_CODER_SRC_LBBP1,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP2,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP3,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP4,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP5,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP6,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP7,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP8,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_LBBP9,           /* LBBP诊断消息 */
    SOCP_CODER_SRC_GUBBP1,          /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUBBP2,          /* GUBBP诊断消息 */
    SOCP_CODER_SRC_GUDSP1,          /* GUDSP诊断消息 */
    SOCP_CODER_SRC_GUDSP2,          /* GUDSP诊断消息 */
    SOCP_CODER_SRC_TDDSP1,          /* TDDSP诊断消息 */
    SOCP_CODER_SRC_TDBBP1,          /* TDBBP诊断消息 */
    SOCP_CODER_SRC_TDBBP2,          /* TDBBP诊断消息 */
};
#endif

/*****************************************************************************
  3 外部引用声明
*****************************************************************************/
#ifdef SCM_CCORE
	extern VOS_VOID Om_BbpDbgChanInit(VOS_VOID);
#endif

extern VOS_VOID SOCP_Stop1SrcChan(VOS_UINT32 ulSrcChanID);

/*****************************************************************************
  4 函数实现
*****************************************************************************/

/*****************************************************************************
* 函 数 名  : OM_Inner_Log
*
* 功能描述  : 记录OM的内部log数据
*
* 输入参数  :  pucStr       OM的输出字符串信息
               ulP1         保存参数1
               ulP2         保存参数2
               pstLogData   保存数据的Log缓冲区
               ulFileID     保存数据产生的文件
               lLineNo      保存数据产生的行号
*
* 输出参数  : 无
*
* 返 回 值  :  无
*****************************************************************************/
VOS_VOID OM_Inner_Log(VOS_CHAR *pucStr, VOS_UINT32 ulP1, VOS_UINT32 ulP2,
                          INNER_LOG_DATA_STRU *pstLogData ,VOS_UINT32 ulFileID, VOS_INT32 lLineNo)
{
    VOS_UINT32  ulCounter;

#ifdef OM_DEBUG_PRINT
    vos_printf("\r\n T:0x%x ", OM_GetSlice());
    vos_printf("%s ",pucStr);
    vos_printf("F:%d,L:%d,P1:0x%x,P2:0x%x",ulFileID,lLineNo,ulP1,ulP2);
#endif  /* end of OM_DEBUG_PRINT */

    if(pstLogData == VOS_NULL)
    {
        return;
    }

    ulCounter = pstLogData->ulCnt;

    pstLogData->astLogData[ulCounter].ulFileNO  = ulFileID;
    pstLogData->astLogData[ulCounter].lLineNO   = lLineNo;
    pstLogData->astLogData[ulCounter].ulP1      = ulP1;
    pstLogData->astLogData[ulCounter].ulP2      = ulP2;
    pstLogData->astLogData[ulCounter].ulSlice   = OM_GetSlice();

    pstLogData->ulCnt                           = (ulCounter+1)%INNER_LOG_DATA_MAX;

    return;
}
VOS_UINT8* SCM_VirtMemAlloc(VOS_UINT32 ulSize,VOS_UINT32 *pulRealAddr)
{
    VOS_UINT8* ret =NULL;

#if (VOS_VXWORKS == VOS_OS_VER)
    ret = (VOS_UINT8*)cacheDmaMalloc(ulSize);
#elif (VOS_LINUX == VOS_OS_VER)
    VOS_INT32 index =0,i=0;

	if(ulSize > SCM_MALLOC_MAX_SIZE)
    {
        return NULL;
    }
    for(i=0;i<=SCM_MALLOC_MAX_INDEX;i++)
    {
        if(ulSize <= SCM_PAGE_SIZE * (1<<i))
        {
            index = i;
            break;
        }
    }
    ret = (VOS_UINT8*)__get_free_pages(GFP_KERNEL|GFP_DMA,index);
#endif

    if(ret == NULL)
    {
        vos_printf("SCM_VirtMemAlloc: Alloc Mem Error!\n");
        return NULL;
    }
	*pulRealAddr = SCM_MemVirtToPhy(ret,0,0,0);
    return ret;
}


VOS_UINT32 SCM_MemPhyToVirt(VOS_UINT8 *pucCurPhyAddr, VOS_UINT8 *pucPhyStart, VOS_UINT8 *pucVirtStart, VOS_UINT32 ulBufLen)
{
#if (VOS_LINUX == VOS_OS_VER)
    return (VOS_UINT32)phys_to_virt((VOS_UINT32)pucCurPhyAddr);
#else
    return (VOS_UINT32)(pucCurPhyAddr);
#endif
}

/*****************************************************************************
 函 数 名  : SCM_MemVirtToPhy
 功能描述  : 根据输入的虚地址，计算对应的实地址
 输入参数  : pucCurVirtAddr:  当前虚地址
             pucPhyStart: 通道配置内存起始的实地址
             pucVirtStart:通道配置内存起始的虚地址
             ulBufLen:    通道内存空间大小
 输出参数  : 无
 返 回 值  : VOS_NULL: 转换失败/other: 虚地址的值
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_MemVirtToPhy(VOS_UINT8 *pucCurVirtAddr, VOS_UINT8 *pucPhyStart, VOS_UINT8 *pucVirtStart, VOS_UINT32 ulBufLen)
{
#if (VOS_LINUX == VOS_OS_VER)
    return (VOS_UINT32)(virt_to_phys(pucCurVirtAddr));
#else
    return (VOS_UINT32)(pucCurVirtAddr);
#endif
}

/*****************************************************************************
 函 数 名  : SCM_FindChannelCfg
 功能描述  : 根据通道ID查询通道配置表的位置
 输入参数  : ulChannelID: 需要查询的通道ID
             ulChNax:     通道配置表最大值
             pstCfg:      通道配置表
 输出参数  : pulNum:      通道配置表的位置
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32  SCM_FindChannelCfg(VOS_UINT32           ulChannelID,
                                    VOS_UINT32              ulChMax,
                                    SCM_CHANNEL_CFG_HEAD    *pstCfg,
                                    VOS_UINT32               ulStruLen,
                                    VOS_UINT32              *pulNum)
{
    VOS_UINT32                          i;
    SCM_CHANNEL_CFG_HEAD               *pstTmpCfg = pstCfg;

    for(i=0; i<ulChMax; i++)
    {
        if((pstTmpCfg->ulChannelID == ulChannelID)
            &&(SCM_CHANNEL_INIT_SUCC == pstTmpCfg->enInitState))
        {
            *pulNum = i;

            return VOS_OK;/* 返回成功 */
        }

        pstTmpCfg = (SCM_CHANNEL_CFG_HEAD *)((VOS_UINT8 *)pstTmpCfg + ulStruLen);
    }

    return VOS_ERR;/* 返回失败 */
}

/*****************************************************************************
 函 数 名  : SCM_CoderSrcChannelCfg
 功能描述  : 将ACPU/CCPU的编码源通道的配置调用SOCP接口配置到IP
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 SCM_CoderSrcChannelCfg(SCM_CODER_SRC_CFG_STRU *pstCfg)
{
    SOCP_CODER_SRC_CHAN_S               stChannle;      /* 当前通道的属性信息 */

    stChannle.u32DestChanID = (BSP_U32)pstCfg->enDstCHID;   /*  目标通道ID */
    stChannle.eDataType     = pstCfg->enDataType;           /*  数据类型，指明数据封装协议，用于复用多平台 */
    stChannle.eMode         = pstCfg->enCHMode;             /*  通道数据模式 */
    stChannle.ePriority     = pstCfg->enCHLevel;            /*  通道优先级 */
    stChannle.u32BypassEn   = SOCP_HDLC_ENABLE;             /*  通道bypass使能 */
    stChannle.eDataTypeEn   = SOCP_DATA_TYPE_EN;            /*  数据类型使能位 */
    stChannle.eDebugEn      = SOCP_ENC_DEBUG_DIS;           /*  调试位使能 */

    stChannle.sCoderSetSrcBuf.u32InputStart  = (VOS_UINT32)pstCfg->pucSrcPHY;                               /*  输入通道起始地址 */
    stChannle.sCoderSetSrcBuf.u32InputEnd    = (VOS_UINT32)((pstCfg->pucSrcPHY + pstCfg->ulSrcBufLen)-1);   /*  输入通道结束地址 */
    stChannle.sCoderSetSrcBuf.u32RDStart     = (VOS_UINT32)(pstCfg->pucRDPHY);                              /* RD buffer起始地址 */
    stChannle.sCoderSetSrcBuf.u32RDEnd       = (VOS_UINT32)((pstCfg->pucRDPHY + pstCfg->ulRDBufLen)-1);     /*  RD buffer结束地址 */
    stChannle.sCoderSetSrcBuf.u32RDThreshold = SCM_CODER_SRC_RD_THRESHOLD;                                  /* RD buffer数据上报阈值 */

    if (VOS_OK != DRV_SOCP_CORDER_SET_SRC_CHAN(pstCfg->enChannelID, &stChannle))
    {
        SCM_CODER_SRC_ERR("SCM_CoderSrcChannelCfg: Search Channel ID Error", pstCfg->enChannelID, 0);/* 打印失败 */

        return VOS_ERR;/* 返回错误 */
    }

    pstCfg->enInitState = SCM_CHANNEL_INIT_SUCC; /* 记录通道初始化配置错误 */

    return VOS_OK;/* 返回成功 */
}

/* ****************************************************************************
 函 数 名  : SCM_ResetCoderSrcChan
 功能描述  : 将ACPU/CCPU的编码源通道的配置重置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_ResetCoderSrcChan(SOCP_CODER_SRC_ENUM_U32 enChID)
{

    return VOS_OK;
}

/* ****************************************************************************
 函 数 名  : SCM_CoderSrcChannelInit
 功能描述  : 将ACPU/CCPU的编码源通道的配置初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_CoderSrcChannelInit(VOS_VOID)
{
    VOS_UINT32                          i;

    for (i = 0; i < SCM_CODER_SRC_NUM; i++)
    {
        if (VOS_OK != SCM_CoderSrcChannelCfg(&g_astSCMCoderSrcCfg[i]))
        {
            g_astSCMCoderSrcCfg[i].enInitState = SCM_CHANNEL_CFG_FAIL;  /* 记录通道初始化配置错误 */

            return VOS_ERR;/* 返回失败 */
        }

        if(VOS_OK != DRV_SOCP_START(g_astSCMCoderSrcCfg[i].enChannelID))
        {
            g_astSCMCoderSrcCfg[i].enInitState = SCM_CHANNEL_START_FAIL;  /* 记录通道开启配置错误 */

            return VOS_ERR;/* 返回失败 */
        }

        g_astSCMCoderSrcCfg[i].enInitState = SCM_CHANNEL_INIT_SUCC;     /* 记录通道初始化配置错误 */
    }

    return VOS_OK;/* 返回成功 */
}

/* ****************************************************************************
 函 数 名  : SCM_RlsSrcRDAll
 功能描述  : 将编码源通道的所有RD内存进行释放
 输入参数  : enChanlID: 编码源通道ID
 输出参数  : pulDataPhyAddr: 编码源通道待释放数据实地址值
             pulDataLen: 编码源通道待释放数据长度
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_RlsSrcRDAll(SOCP_CODER_SRC_ENUM_U32 enChanlID, VOS_UINT32 *pulDataPhyAddr, VOS_UINT32 *pulDataLen)
{
    SOCP_BUFFER_RW_S                    stSrcChanRD;
    SOCP_RD_DATA_STRU                  *pstRDData;
    VOS_UINT32                          ulRDNum;
    VOS_UINT32                          ulRDTotalNum= 0;
    VOS_UINT32                          ulTotalLen  = 0;
    VOS_UINT32                          ulFirstAddr = 0;
    VOS_UINT32                          ulCfgNum;
    VOS_UINT32                          i;

    VOS_MemSet(&stSrcChanRD, 0, sizeof(stSrcChanRD));

    if ( VOS_OK != SCM_FindChannelCfg(enChanlID,
                                        SCM_CODER_SRC_NUM,
                                        (SCM_CHANNEL_CFG_HEAD *)g_astSCMCoderSrcCfg,
                                        sizeof(SCM_CODER_SRC_CFG_STRU),
                                        &ulCfgNum))/* 判断通道参数 */
    {
        SCM_CODER_SRC_ERR("SCM_RlsSrcRDAll: Find Channel Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    SCM_CODER_SRC_LOG("SCM_RlsSrcRDAll: Release Channel Data", enChanlID, 0);

    if (VOS_OK != DRV_SOCP_GET_RD_BUFFER(enChanlID, &stSrcChanRD))
    {
        SCM_CODER_SRC_ERR("SCM_RlsSrcRDAll: Get RD Info Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    if (VOS_NULL_PTR == stSrcChanRD.pBuffer)        /* 参数错误 */
    {
        SCM_CODER_SRC_ERR("SCM_RlsSrcRDAll: RD Info is Error", enChanlID, (VOS_UINT32)stSrcChanRD.pBuffer);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    if ((0 == stSrcChanRD.u32Size) && (0 == stSrcChanRD.u32RbSize)) /* 无数据需要释放 */
    {
        *pulDataPhyAddr = 0;
        *pulDataLen     = 0;

        return VOS_OK;/* 返回成功 */
    }

    /* RD个数获取非法 */
    if(((stSrcChanRD.u32Size + stSrcChanRD.u32RbSize) / sizeof(SOCP_RD_DATA_STRU)) > SCM_CODE_SRC_RD_NUM)
    {
        SCM_CODER_SRC_ERR("SCM_RlsSrcRDFirst: Get RD Data Error", enChanlID, 0);/* 记录Log */
        SCM_CODER_SRC_ERR("SCM_RlsSrcRDFirst: Get RD PTR Error", stSrcChanRD.u32Size, stSrcChanRD.u32RbSize);/* 记录Log */

        return VOS_ERR;/* 返回失败 */
    }

    /* 计算RD个数 */
    ulRDNum = stSrcChanRD.u32Size / sizeof(SOCP_RD_DATA_STRU);

    if (0 != ulRDNum)
    {
    	stSrcChanRD.pBuffer = (BSP_CHAR*)VOS_UncacheMemPhyToVirt((VOS_UINT8*)stSrcChanRD.pBuffer,
                                    g_astSCMCoderSrcCfg[ulCfgNum].pucRDPHY,
                                    g_astSCMCoderSrcCfg[ulCfgNum].pucRDBuf,
                                    g_astSCMCoderSrcCfg[ulCfgNum].ulRDBufLen);
#ifdef SCM_CCORE
		SCM_INVALID_CACHE(stSrcChanRD.pBuffer, stSrcChanRD.u32Size);
#endif
        ulRDTotalNum = ulRDNum;

        /* 获取RD数据的地址和长度 */
        pstRDData   = (SOCP_RD_DATA_STRU*)stSrcChanRD.pBuffer;

        /* 记录当前第一个释放的数据地址,后面需要返回给上层 */
        ulFirstAddr = (VOS_UINT32)pstRDData->pucData;

        for (i = 0; i < ulRDNum; i++)
        {
            /* 累计RD数据长度 */
            ulTotalLen += pstRDData->usMsgLen;

            //pstRDData->usMsgLen = 0xffff;
            //pstRDData->pucData  = 0x0f;


            pstRDData++;
        }
    }

    /* 计算回卷RD个数 */
    ulRDNum = stSrcChanRD.u32RbSize / sizeof(SOCP_RD_DATA_STRU);

    if (0 != ulRDNum)
    {
    	stSrcChanRD.pRbBuffer = (BSP_CHAR*)VOS_UncacheMemPhyToVirt((VOS_UINT8*)stSrcChanRD.pRbBuffer,
                                    g_astSCMCoderSrcCfg[ulCfgNum].pucRDPHY,
                                    g_astSCMCoderSrcCfg[ulCfgNum].pucRDBuf,
                                    g_astSCMCoderSrcCfg[ulCfgNum].ulRDBufLen);
#ifdef SCM_CCORE
    	SCM_INVALID_CACHE(stSrcChanRD.pRbBuffer, stSrcChanRD.u32RbSize);
#endif
        ulRDTotalNum += ulRDNum;

        /* 获取RD数据回卷的地址和长度 */
        pstRDData   = (SOCP_RD_DATA_STRU*)stSrcChanRD.pRbBuffer;

        if (0 == ulFirstAddr)/* 记录当前第一个释放的数据地址,后面需要返回给上层 */
        {
            ulFirstAddr = (VOS_UINT32)pstRDData->pucData;
        }

        for (i = 0; i < ulRDNum; i++)
        {
            /* 累计RD数据长度 */
            ulTotalLen += pstRDData->usMsgLen;

//            pstRDData->usMsgLen = 0;
//            pstRDData->pucData  = VOS_NULL_PTR;
            pstRDData++;
        }
    }

    if (VOS_OK != DRV_SOCP_READ_RD_DONE(enChanlID, (stSrcChanRD.u32Size+stSrcChanRD.u32RbSize)))
    {
        SCM_CODER_SRC_ERR("SCM_RlsSrcRDAll: Write RD Done is Error", enChanlID, (stSrcChanRD.u32Size+stSrcChanRD.u32RbSize));/* 记录Log */
        return VOS_ERR;/* 返回错误 */
    }

    /* 获取的RD最大值记录到全局变量中 */
    if (ulRDTotalNum > g_stSCMInfoData.aulRDUsedMax[ulCfgNum])
    {
        g_stSCMInfoData.aulRDUsedMax[ulCfgNum] = ulRDTotalNum;
    }

    *pulDataPhyAddr = ulFirstAddr;

    *pulDataLen     = ulTotalLen;

    return VOS_OK;/* 返回成功 */
}


/* ****************************************************************************
 函 数 名  : SCM_GetBDFreeNum
 功能描述  : 查询BD剩余空间
 输入参数  : enChanlID: 通道ID
 输出参数  : pulBDNum:BD剩余个数
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_GetBDFreeNum(SOCP_CODER_SRC_ENUM_U32 enChanlID, VOS_UINT32 *pulBDNum)
{
    SOCP_BUFFER_RW_S                    stRwBuf;
    VOS_UINT32                          ulBDNum;
    VOS_UINT32                          ulBDTotal;
    VOS_UINT32                          ulCfgNum;

    /* 判断指针的正确 */
    if (VOS_NULL_PTR == pulBDNum)
    {
        return VOS_ERR;
    }

    if (VOS_OK != SCM_FindChannelCfg(enChanlID, SCM_CODER_SRC_NUM,
                                (SCM_CHANNEL_CFG_HEAD *)g_astSCMCoderSrcCfg, sizeof(SCM_CODER_SRC_CFG_STRU), &ulCfgNum))/* 判断通道参数 */
    {
        SCM_CODER_SRC_ERR("SCM_GetBDFreeNum: Find Channel Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    SCM_CODER_SRC_LOG("SCM_GetBDFreeNum: Get BD Number", enChanlID, 0);

    if (VOS_OK != DRV_SOCP_GET_WRITE_BUFF(enChanlID, &stRwBuf))
    {
        SCM_CODER_SRC_ERR("SCM_GetBDFreeNum: Get Write Buffer Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    /* 计算BD的值 */
    ulBDNum = (stRwBuf.u32Size + stRwBuf.u32RbSize) / sizeof(SOCP_BD_DATA_STRU);

    /* BD个数获取非法 */
    if(ulBDNum > SCM_CODE_SRC_BD_NUM)
    {
        SCM_CODER_SRC_ERR("SCM_GetBDFreeNum: Get BD Data Error", enChanlID, 0);/* 记录Log */
        SCM_CODER_SRC_ERR("SCM_GetBDFreeNum: Get BD PTR Error", stRwBuf.u32Size, stRwBuf.u32RbSize);/* 记录Log */

        return VOS_ERR;/* 返回失败 */
    }

    /* 至少要保留一个空闲BD，保证通道不会被写满而异常 */
    if (ulBDNum <= 1)
    {
        *pulBDNum = 0;
    }
    else
    {
        *pulBDNum = (ulBDNum - 1);
    }

    /* 计算通道全部BD的个数 */
    ulBDTotal = g_astSCMCoderSrcCfg[ulCfgNum].ulSrcBufLen / sizeof(SOCP_BD_DATA_STRU);

    /* 和全局变量中比较记录最大值 */
    if ((ulBDTotal- ulBDNum) > g_stSCMInfoData.aulBDUsedMax[ulCfgNum])
    {
        g_stSCMInfoData.aulBDUsedMax[ulCfgNum] = (ulBDTotal- ulBDNum);
    }

    return VOS_OK;
}

/* ****************************************************************************
 函 数 名  : SCM_SendCoderSrc
 功能描述  : 通过SOCP的编码源通道发送数据
 输入参数  : enChanlID: 通道ID
             pucSendDataPhy:发送数据实地址
             ulSendLen: 发送数据长度
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_SendCoderSrc(SOCP_CODER_SRC_ENUM_U32 enChanlID, VOS_UINT8 *pucSendDataPhy, VOS_UINT32 ulSendLen)
{
    SOCP_BUFFER_RW_S                    stRwBuf;
    VOS_UINT32                          ulBDNum;
    VOS_UINT32                          ulCfgNum;
    SOCP_BD_DATA_STRU                   stBDData;
	/*VOS_UINT8 *pucSendData	=NULL;*/

    /* 判断数据指针和长度的正确，长度不能大于16K */
    if ((VOS_NULL_PTR == pucSendDataPhy)
        ||(0 == ulSendLen)
        ||(SCM_CODER_SRC_MAX_LEN < ulSendLen))
    {
    	SCM_CODER_SRC_ERR("SCM_SendCoderSrc: invalid param Error", pucSendDataPhy, ulSendLen);/* 记录Log */
        return VOS_ERR;
    }

    if (VOS_OK != SCM_FindChannelCfg(enChanlID, SCM_CODER_SRC_NUM,
                                     (SCM_CHANNEL_CFG_HEAD *)g_astSCMCoderSrcCfg,
                                     sizeof(SCM_CODER_SRC_CFG_STRU), &ulCfgNum))/* 判断通道参数 */
    {
        SCM_CODER_SRC_ERR("SCM_SendCoderSrc: Find Channel Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    SCM_CODER_SRC_LOG("SCM_SendCoderSrc: Get BD Number", enChanlID, 0);

    if (VOS_OK != DRV_SOCP_GET_WRITE_BUFF(g_astSCMCoderSrcCfg[ulCfgNum].enChannelID, &stRwBuf))
    {
        SCM_CODER_SRC_ERR("SCM_SendCoderSrc: Get Write Buffer Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    /* 计算空闲BD的值 */
    ulBDNum = (stRwBuf.u32Size + stRwBuf.u32RbSize) / sizeof(SOCP_BD_DATA_STRU);

    /* 判断是否还有空间 */
    if (1 >= ulBDNum)
    {
        SCM_CODER_SRC_ERR("SCM_SendCoderSrc: Buffer is Full", enChanlID, ulBDNum);/* 记录Log */
        return VOS_ERR;
    }

	stRwBuf.pBuffer = (BSP_CHAR*)VOS_UncacheMemPhyToVirt((VOS_UINT8*)stRwBuf.pBuffer,
                                    g_astSCMCoderSrcCfg[ulCfgNum].pucSrcPHY,
                                    g_astSCMCoderSrcCfg[ulCfgNum].pucSrcBuf,
                                    g_astSCMCoderSrcCfg[ulCfgNum].ulSrcBufLen);

	//pucSendData = (VOS_UINT8*)SCM_MemPhyToVirt(pucSendDataPhy,0,0,ulSendLen);

    stBDData.pucData    = pucSendDataPhy;
    stBDData.usMsgLen   = (VOS_UINT16)ulSendLen;
    stBDData.enDataType = SOCP_BD_DATA;

    VOS_MemCpy(stRwBuf.pBuffer, &stBDData, sizeof(stBDData));    /* 复制数据到指定的地址 */

#ifdef SCM_CCORE
	SCM_FLUSH_CACHE(stRwBuf.pBuffer,sizeof(stBDData));
#endif
	//SCM_FLUSH_CACHE(pucSendData,ulSendLen);

    if (VOS_OK != DRV_SOCP_WRITE_DONE(enChanlID, sizeof(stBDData)))   /* 当前数据写入完毕 */
    {
        SCM_CODER_SRC_ERR("SCM_SendCoderSrc: Write Buffer is Error", enChanlID, 0);/* 记录Log */
        return VOS_ERR;/* 返回失败 */
    }

    return VOS_OK;
}

#ifdef SCM_ACORE
/* log2.0 2014-03-19 Begin:*/
/* ****************************************************************************
 函 数 名  : SCM_ChangeCoderDstChanCfg
 功能描述  : 根据SD卡保存Log的NV配置，更新编码目的通道的配置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年8月20日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_VOID SCM_ChangeCoderDstChanCfg(VOS_VOID)
{
    NV_SOCP_SDLOG_CFG_STRU              stSocpCfg = {0};
    VOS_UINT32                          i;

    if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_ID_SOCP_SDLOG_CFG, &stSocpCfg, sizeof(NV_SOCP_SDLOG_CFG_STRU)))
    {
        vos_printf("\r\n SCM_ChangeCoderDstChanCfg: Read NV Fail");

        return;
    }

    if(VOS_TRUE != stSocpCfg.ulSocpDelayWriteFlg)
    {
        return;
    }

    for(i = 0; i < SCM_CODER_DST_NUM; i++)
    {
        g_astSCMCoderDstCfg[i].ulBufLen     = SCM_CODER_DST_IND_SD_SIZE;
        //g_astSCMCoderDstCfg[i].ulBufLen     = SCM_CODER_DST_IND_SIZE;

        if(SOCP_CODER_DST_GU_OM == g_astSCMCoderDstCfg[i].enChannelID)
        {
            g_astSCMCoderDstCfg[i].ulThreshold  = stSocpCfg.ulGuSocpLevel;
        }
        else
        {
            g_astSCMCoderDstCfg[i].ulThreshold  = stSocpCfg.ulLSocpLevel;
        }
    }

    return;
}
/* log2.0 2014-03-19 End:*/

/* ****************************************************************************
 函 数 名  : SCM_RlsDestBuf
 功能描述  : 处理目的通道的数据释放
 输入参数  : ulChanlID 目的通道ID
             ulReadSize 数据大小
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_RlsDestBuf(VOS_UINT32 ulChanlID, VOS_UINT32 ulReadSize)
{
    VOS_UINT32                          ulDataLen;
    SOCP_BUFFER_RW_S                    stBuffer;

    if(0 == ulReadSize) /*释放通道所有数据*/
    {
        if (VOS_OK != DRV_SOCP_GET_READ_BUFF(ulChanlID, &stBuffer))
        {
            SCM_CODER_DST_ERR("SCM_RlsDestBuf: Get Read Buffer is Error", (VOS_UINT)ulChanlID, (VOS_UINT)0);/* 记录Log */
            return VOS_ERR;
        }

        ulDataLen = stBuffer.u32Size + stBuffer.u32RbSize;

        SCM_CODER_DST_LOG("SCM_RlsDestBuf: Relese All Data", (VOS_UINT)ulChanlID, (VOS_UINT)ulDataLen);
    }
    else
    {
        /* 记录调用时间 */
        SCM_CODER_DST_LOG("SCM_RlsDestBuf: Relese Read Data", (VOS_UINT)ulChanlID, (VOS_UINT)ulReadSize);

        ulDataLen = ulReadSize;
    }

    if (VOS_OK != DRV_SOCP_READ_DATA_DONE(ulChanlID, ulDataLen))
    {
        SCM_CODER_DST_ERR("SCM_RlsDestBuf: Read Data Done is Error", (VOS_UINT)ulChanlID, (VOS_UINT)ulDataLen);/* 记录Log */

        return VOS_ERR;
    }

    return VOS_OK;
}

/* ****************************************************************************
 函 数 名  : SCM_CoderDestReadCB
 功能描述  : 处理编码目的通道的数据
 输入参数  : ulDstChID 目的通道ID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_VOID SCM_CoderDestReadCB(VOS_UINT32 ulDstChID)
{
    VOS_UINT32                          ulChType;
    VOS_UINT32                          ulCfgNum;
    SOCP_BUFFER_RW_S                    stBuffer;
    VOS_UINT32                          ulTimerIn;
    VOS_UINT32                          ulTimerOut;

    ulChType = SOCP_REAL_CHAN_TYPE(ulDstChID);

    if (SOCP_CODER_DEST_CHAN != ulChType)
    {
        SCM_CODER_DST_ERR("SCM_CoderDestReadCB: Channel Type is Error", (VOS_UINT)ulDstChID, (VOS_UINT)ulChType);/* 记录Log */
        return;
    }

    if (VOS_OK != DRV_SOCP_GET_READ_BUFF(ulDstChID, &stBuffer))
    {
        SCM_CODER_DST_ERR("SCM_CoderDestReadCB: Get Read Buffer is Error", (VOS_UINT)ulDstChID, 0);/* 记录Log */
        return;
    }

    if (VOS_OK != SCM_FindChannelCfg(ulDstChID, SCM_CODER_DST_NUM,
                                (SCM_CHANNEL_CFG_HEAD *)g_astSCMCoderDstCfg, sizeof(SCM_CODER_DEST_CFG_STRU), &ulCfgNum))
    {
        DRV_SOCP_READ_DATA_DONE(ulDstChID, stBuffer.u32Size + stBuffer.u32RbSize);  /* 清空数据 */

        SCM_CODER_DST_ERR("SCM_CoderDestReadCB: Find Channel is Error", (VOS_UINT)ulDstChID, 0);/* 记录Log */

        return;
    }

    if ((VOS_NULL_PTR == g_astSCMCoderDstCfg[ulCfgNum].pfunc)
        || (0 == (stBuffer.u32Size + stBuffer.u32RbSize)))
    {
        DRV_SOCP_READ_DATA_DONE(ulDstChID, stBuffer.u32Size + stBuffer.u32RbSize);  /* 清空数据 */

        if(VOS_NULL_PTR == g_astSCMCoderDstCfg[ulCfgNum].pfunc)
        {
            SCM_CODER_DST_ERR("SCM_CoderDestReadCB: Channel Callback Fucn is NULL", (VOS_UINT)ulDstChID,(VOS_UINT)0);/* 记录Log */
        }

        return;
    }

    if (0 != stBuffer.u32Size)   /* 发送数据 */
    {
        ulTimerIn = OM_GetSlice();

		stBuffer.pBuffer = (VOS_CHAR*)SCM_MemPhyToVirt((VOS_UINT8*)stBuffer.pBuffer,
                                    g_astSCMCoderDstCfg[ulCfgNum].pucBufPHY,
                                    g_astSCMCoderDstCfg[ulCfgNum].pucBuf,
                                    g_astSCMCoderDstCfg[ulCfgNum].ulBufLen);
        if(NULL == stBuffer.pBuffer)
        {
            return;
        }
        SCM_INVALID_CACHE(stBuffer.pBuffer, stBuffer.u32Size);

        g_astSCMCoderDstCfg[ulCfgNum].pfunc(ulDstChID, (VOS_UINT8*)stBuffer.pBuffer, 0,(VOS_UINT32)stBuffer.u32Size);

        ulTimerOut = OM_GetSlice();

        /* 记录回调函数的执行时间 */
        SCM_CODER_DST_LOG("SCM_CoderDestReadCB: Call channel Func Proc time", (VOS_UINT)ulDstChID, (VOS_UINT)(ulTimerIn-ulTimerOut));
    }

    return;
}

/*****************************************************************************
 函 数 名  : SCM_CoderDstChannelInit
 功能描述  : 将ACPU的编码目的通道的配置重置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_UINT32 SCM_CoderDstChannelInit(VOS_VOID)
{
    VOS_UINT32                          i;
    SOCP_CODER_DEST_CHAN_S              stChannel;

    stChannel.u32EncDstThrh = SCM_CODER_DST_GTHRESHOLD;

    for (i = 0; i < SCM_CODER_DST_NUM; i++)
    {
        stChannel.sCoderSetDstBuf.u32OutputStart    = (VOS_UINT32)g_astSCMCoderDstCfg[i].pucBufPHY;

        stChannel.sCoderSetDstBuf.u32OutputEnd
            = (VOS_UINT32)((g_astSCMCoderDstCfg[i].pucBufPHY + g_astSCMCoderDstCfg[i].ulBufLen)-1);

        stChannel.sCoderSetDstBuf.u32Threshold      = g_astSCMCoderDstCfg[i].ulThreshold;

        if (VOS_OK != DRV_SOCP_CODER_SET_DEST_CHAN_ATTR(g_astSCMCoderDstCfg[i].enChannelID, &stChannel))
        {
            g_astSCMCoderDstCfg[i].enInitState = SCM_CHANNEL_CFG_FAIL;  /* 记录通道初始化配置错误 */

            return VOS_ERR;/* 返回失败 */
        }

        g_astSCMCoderDstCfg[i].enInitState = SCM_CHANNEL_INIT_SUCC;     /* 记录通道初始化配置错误 */

        DRV_SOCP_REGISTER_READ_CB((BSP_U32)g_astSCMCoderDstCfg[i].enChannelID, (socp_read_cb)SCM_CoderDestReadCB);
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : SCM_RegCoderDestProc
 功能描述  : ACPU的编码目的通道的回调函数注册接口
 输入参数  : enChanlID: 解码目的通道ID
             func: 回调函数指针
 输出参数  :
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/

VOS_UINT32 SCM_RegCoderDestProc(SOCP_CODER_DST_ENUM_U32 enChanlID, SCM_CODERDESTFUCN func)
{
    VOS_UINT32                          ulCgfNum;

    if (VOS_OK != SCM_FindChannelCfg(enChanlID,
                                SCM_CODER_DST_NUM,
                                (SCM_CHANNEL_CFG_HEAD *)g_astSCMCoderDstCfg,
                                sizeof(SCM_CODER_DEST_CFG_STRU),
                                &ulCgfNum))
    {
        SCM_CODER_DST_ERR("SCM_RegCoderDestProc: Find Channeld is Error", (VOS_UINT)enChanlID, (VOS_UINT)func);

        return VOS_ERR;/* 返回失败 */
    }

    g_astSCMCoderDstCfg[ulCgfNum].pfunc = func;

    return VOS_OK;/* 返回成功 */
}

#if (RAT_MODE != RAT_GU)                /* Mutil Mode */
/* ****************************************************************************
 函 数 名  : SCM_DecoderDestReadCB
 功能描述  : 处理解码目的通道的数据
 输入参数  : ulDstChID 目的通道ID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */

VOS_VOID SCM_DecoderDestReadCB(VOS_UINT32 ulDstChID)
{
    VOS_UINT32                          ulChType;
    VOS_UINT32                          ulCfgNum;
    VOS_UINT32                          ulTimerIn;
    VOS_UINT32                          ulTimerOut;
    SOCP_BUFFER_RW_S                    stBuffer;

    ulChType = SOCP_REAL_CHAN_TYPE(ulDstChID);

    if (SOCP_DECODER_DEST_CHAN != ulChType)
    {
        SCM_DECODER_DST_ERR("SCM_DecoderDestReadCB: Channel Type is Error", (VOS_UINT)ulDstChID, (VOS_UINT)ulChType);/* 记录Log */
        return;
    }

    if (VOS_OK != DRV_SOCP_GET_READ_BUFF(ulDstChID, &stBuffer))
    {
        SCM_DECODER_DST_ERR("SCM_DecoderDestReadCB: Get Read Buffer is Error", (VOS_UINT)ulDstChID, 0);/* 记录Log */
        return;
    }

    if (VOS_OK != SCM_FindChannelCfg(ulDstChID, SCM_DECODER_DST_NUM,
                                (SCM_CHANNEL_CFG_HEAD *)g_astSCMDecoderDstCfg, sizeof(SCM_DECODER_DEST_CFG_STRU), &ulCfgNum))
    {
        DRV_SOCP_READ_DATA_DONE(ulDstChID, (stBuffer.u32Size + stBuffer.u32RbSize));  /* 清空数据 */

        SCM_DECODER_DST_ERR("SCM_DecoderDestReadCB: Find Channel is Error", (VOS_UINT)ulDstChID, 0);/* 记录Log */

        return;
    }

    if ((VOS_NULL_PTR == g_astSCMDecoderDstCfg[ulCfgNum].pfunc)
        || (0 == (stBuffer.u32Size + stBuffer.u32RbSize)))
    {
        DRV_SOCP_READ_DATA_DONE(ulDstChID, (stBuffer.u32Size + stBuffer.u32RbSize));  /* 清空数据 */

        SCM_DECODER_DST_ERR("SCM_DecoderDestReadCB: Channel Callback Func or Data is NULL", (VOS_UINT)ulDstChID, 0);/* 记录Log */

        return;
    }

	stBuffer.pBuffer = (BSP_CHAR*)SCM_MemPhyToVirt((VOS_UINT8*)stBuffer.pBuffer,
                            g_astSCMDecoderDstCfg[ulCfgNum].pucBufPHY,
                            g_astSCMDecoderDstCfg[ulCfgNum].pucBuf,
                            g_astSCMDecoderDstCfg[ulCfgNum].ulBufLen);

    stBuffer.pRbBuffer = (BSP_CHAR*)SCM_MemPhyToVirt((VOS_UINT8*)stBuffer.pRbBuffer,
                            g_astSCMDecoderDstCfg[ulCfgNum].pucBufPHY,
                            g_astSCMDecoderDstCfg[ulCfgNum].pucBuf,
                            g_astSCMDecoderDstCfg[ulCfgNum].ulBufLen);


	if( (stBuffer.pBuffer != NULL)	&&	(stBuffer.u32Size > 0) )
	{
		SCM_INVALID_CACHE(stBuffer.pBuffer, stBuffer.u32Size);
	}
	if( (stBuffer.pRbBuffer != NULL)&&(stBuffer.u32RbSize > 0) )
	{
		SCM_INVALID_CACHE(stBuffer.pRbBuffer, stBuffer.u32RbSize);
	}
    ulTimerIn = OM_GetSlice();/* [false alarm]:屏蔽Fortify */

    g_astSCMDecoderDstCfg[ulCfgNum].pfunc(ulDstChID,
                                          (VOS_UINT8*)stBuffer.pBuffer,
                                          stBuffer.u32Size,
                                          (VOS_UINT8*)stBuffer.pRbBuffer,
                                          stBuffer.u32RbSize);

    ulTimerOut = OM_GetSlice();/* [false alarm]:屏蔽Fortify */

    /* 记录回调函数的执行时间 */
    SCM_DECODER_DST_LOG("SCM_DecoderDestReadCB: Call channel Func Proc time", (VOS_UINT)ulDstChID, (ulTimerIn-ulTimerOut));

    if (VOS_OK != DRV_SOCP_READ_DATA_DONE(ulDstChID, (stBuffer.u32Size + stBuffer.u32RbSize)))  /* 清空数据 */
    {
        SCM_DECODER_DST_ERR("SCM_DecoderDestReadCB: Channel Read Done is Error", (VOS_UINT)ulDstChID, 0);/* 记录Log */
    }

    return;
}

/* ****************************************************************************
 函 数 名  : SCM_DecoderDstChannelInit
 功能描述  : 将ACPU的解码目的通道的配置初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_DecoderDstChannelInit(VOS_VOID)
{
    VOS_UINT32                          i;
    SOCP_DECODER_DEST_CHAN_S            stChannel;

    for (i = 0; i < SCM_DECODER_DST_NUM; i++)
    {
        stChannel.eDataType     = g_astSCMDecoderDstCfg[i].enDataType;

        stChannel.u32SrcChanID  = g_astSCMDecoderDstCfg[i].enSrcCHID;

        stChannel.sDecoderDstSetBuf.u32OutputStart  = (VOS_UINT32)g_astSCMDecoderDstCfg[i].pucBufPHY;

        stChannel.sDecoderDstSetBuf.u32OutputEnd
                = (VOS_UINT32)((g_astSCMDecoderDstCfg[i].pucBufPHY + g_astSCMDecoderDstCfg[i].ulBufLen)-1);

        stChannel.sDecoderDstSetBuf.u32Threshold    = g_astSCMDecoderDstCfg[i].ulThreshold;

        if (VOS_OK != DRV_SOCP_DECODER_SET_DEST_CHAN(g_astSCMDecoderDstCfg[i].enChannelID, &stChannel))
        {
            g_astSCMDecoderDstCfg[i].enInitState = SCM_CHANNEL_CFG_FAIL;  /* 记录通道初始化配置错误 */

            return VOS_ERR;/* 返回失败 */
        }

        g_astSCMDecoderDstCfg[i].enInitState = SCM_CHANNEL_INIT_SUCC;     /* 记录通道初始化配置错误 */

        DRV_SOCP_REGISTER_READ_CB((BSP_U32)g_astSCMDecoderDstCfg[i].enChannelID,(socp_read_cb)SCM_DecoderDestReadCB);
    }

    return VOS_OK;

}

/* ****************************************************************************
 函 数 名  : SCM_CoderSrcChannelReset
 功能描述  : 将ACPU/CCPU的编码源通道的配置重置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_DecoderSrcChannelInit(VOS_VOID)
{
    VOS_UINT32                          i;
    SOCP_DECODER_SRC_CHAN_S             stChannel;

    stChannel.eDataTypeEn   = SOCP_DATA_TYPE_EN;
    stChannel.eMode         = SOCP_DECSRC_CHNMODE_BYTES;

    stChannel.sDecoderSetSrcBuf.u32RDStart      = 0;
    stChannel.sDecoderSetSrcBuf.u32RDEnd        = 0;
    stChannel.sDecoderSetSrcBuf.u32RDThreshold  = 0;

    for (i = 0; i < SCM_DECODER_SRC_NUM; i++)
    {
        stChannel.sDecoderSetSrcBuf.u32InputStart = (VOS_UINT32)g_astSCMDecoderSrcCfg[i].pucSrcPHY;

        stChannel.sDecoderSetSrcBuf.u32InputEnd
                = (VOS_UINT32)((g_astSCMDecoderSrcCfg[i].pucSrcPHY + g_astSCMDecoderSrcCfg[i].ulSrcBufLen)-1);

        if (VOS_OK != DRV_SOCP_DECODER_SET_SRC_CHAN_ATTR(g_astSCMDecoderSrcCfg[i].enChannelID, &stChannel))
        {
            g_astSCMDecoderSrcCfg[i].enInitState = SCM_CHANNEL_CFG_FAIL;  /* 记录通道初始化配置错误 */

            return VOS_ERR;/* 返回失败 */
        }

        if(VOS_OK != DRV_SOCP_START(g_astSCMDecoderSrcCfg[i].enChannelID))
        {
            g_astSCMDecoderSrcCfg[i].enInitState = SCM_CHANNEL_START_FAIL;  /* 记录通道开启配置错误 */

            return VOS_ERR;/* 返回失败 */
        }

        g_astSCMDecoderSrcCfg[i].enInitState = SCM_CHANNEL_INIT_SUCC;     /* 记录通道初始化配置错误 */
    }

    return VOS_OK;

}

/* ****************************************************************************
 函 数 名  : SCM_RegDecoderDestProc
 功能描述  : ACPU的解码目的通道的回调函数注册接口
 输入参数  : enChanlID: 解码目的通道ID
             func: 回调函数指针
 输出参数  :
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */

VOS_UINT32 SCM_RegDecoderDestProc(SOCP_DECODER_DST_ENUM_U32 enChanlID, SCM_DECODERDESTFUCN func)
{
    VOS_UINT32                          ulCgfNum;

    if (VOS_OK != SCM_FindChannelCfg(enChanlID,
                                SCM_DECODER_DST_NUM,
                                (SCM_CHANNEL_CFG_HEAD *)g_astSCMDecoderDstCfg,
                                sizeof(SCM_DECODER_DEST_CFG_STRU),
                                &ulCgfNum))
    {
        SCM_DECODER_DST_ERR("SCM_RegDecoderDestProc: Find Channeld is Error", (VOS_UINT)enChanlID, (VOS_UINT)func);

        return VOS_ERR;/* 返回失败 */
    }

    g_astSCMDecoderDstCfg[ulCgfNum].pfunc = func;

    return VOS_OK;/* 返回成功 */
}

/* ****************************************************************************
 函 数 名  : SCM_SendDecoderSrc
 功能描述  : ACPU的解码源通道的数据发送函数
 输入参数  : enChanlID: 解码目的通道ID
             pucSendDataVirt: 发送数据的虚地址
             ulSendLen:       发送数据的长度
 输出参数  :
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */

VOS_UINT32 SCM_SendDecoderSrc(SOCP_DECODER_SRC_ENUM_U32 enChanlID, VOS_UINT8 *pucSendDataVirt, VOS_UINT32 ulSendLen)
{
    SOCP_BUFFER_RW_S                    stRwBuf;
    VOS_UINT32                          ulCfgNum;
    VOS_UINT32                          ulResult;

    /* 判断数据指针和长度的正确 */
    if ((VOS_NULL_PTR == pucSendDataVirt)||(0 == ulSendLen))
    {
        SCM_DECODER_SRC_ERR("SCM_SendDecoderSrc: Input Para is Error", (VOS_UINT)enChanlID, (VOS_UINT)pucSendDataVirt);

        return VOS_ERR;
    }

    if (VOS_OK != SCM_FindChannelCfg(enChanlID, SCM_DECODER_SRC_NUM,
                                (SCM_CHANNEL_CFG_HEAD *)g_astSCMDecoderSrcCfg, sizeof(SCM_DECODER_SRC_CFG_STRU), &ulCfgNum))/* 判断通道参数 */
    {
        SCM_DECODER_SRC_ERR("SCM_SendDecoderSrc: Find Channel is Error", (VOS_UINT)enChanlID, (VOS_UINT)0);/* 记录Log */

        return VOS_ERR;/* 返回失败 */
    }

    SCM_DECODER_SRC_LOG("SCM_SendDecoderSrc: Send Decoder Data", (VOS_UINT)enChanlID, (VOS_UINT)ulSendLen);

    if (VOS_OK != DRV_SOCP_GET_WRITE_BUFF(enChanlID, &stRwBuf))
    {
        SCM_DECODER_SRC_ERR("SCM_SendDecoderSrc: Get Write Buffer is Error", (VOS_UINT)enChanlID, 0);/* 记录Log */

        return VOS_ERR;/* 返回失败 */
    }

    /* 判断是否还有空间 */
    if (ulSendLen > (stRwBuf.u32Size + stRwBuf.u32RbSize))
    {
        SCM_DECODER_SRC_ERR("SCM_SendDecoderSrc: Buffer is Full", (VOS_UINT)enChanlID, (VOS_UINT)ulSendLen);/* 记录Log */

        return VOS_ERR;/* 返回失败 */
    }

	stRwBuf.pBuffer = (BSP_CHAR *)SCM_MemPhyToVirt((VOS_UINT8 *)stRwBuf.pBuffer,
                                    g_astSCMDecoderSrcCfg[ulCfgNum].pucSrcPHY,
                                    g_astSCMDecoderSrcCfg[ulCfgNum].pucSrcBuf,
                                    g_astSCMDecoderSrcCfg[ulCfgNum].ulSrcBufLen);

    if(VOS_NULL_PTR == stRwBuf.pBuffer)
    {

        SCM_DECODER_SRC_ERR("SCM_MemPhyToVirt: Buffer alloc error", (VOS_UINT)enChanlID,(VOS_UINT)(stRwBuf.u32Size));/* 记录Log */
        ulResult = VOS_ERR;/* [false alarm]:屏蔽Fortify */
    }

    stRwBuf.pRbBuffer = (BSP_CHAR *)SCM_MemPhyToVirt((VOS_UINT8 *)stRwBuf.pRbBuffer,
                                    g_astSCMDecoderSrcCfg[ulCfgNum].pucSrcPHY,
                                    g_astSCMDecoderSrcCfg[ulCfgNum].pucSrcBuf,
                                    g_astSCMDecoderSrcCfg[ulCfgNum].ulSrcBufLen);

    if (stRwBuf.u32Size >= ulSendLen)
    {
       VOS_MemCpy(stRwBuf.pBuffer, pucSendDataVirt, ulSendLen);
	   SCM_FLUSH_CACHE(stRwBuf.pBuffer,ulSendLen);

       ulResult = DRV_SOCP_WRITE_DONE(enChanlID, ulSendLen);
    }
    else if ((stRwBuf.pRbBuffer != VOS_NULL_PTR)&& (stRwBuf.u32RbSize != 0))    /* 空间不足，则写入回卷地址 */
    {
        VOS_MemCpy(stRwBuf.pBuffer, pucSendDataVirt, stRwBuf.u32Size);
		SCM_FLUSH_CACHE(stRwBuf.pBuffer,stRwBuf.u32Size);

        VOS_MemCpy(stRwBuf.pRbBuffer, (pucSendDataVirt + stRwBuf.u32Size), (ulSendLen-stRwBuf.u32Size));
		SCM_FLUSH_CACHE(stRwBuf.pRbBuffer,(ulSendLen-stRwBuf.u32Size));

        ulResult = DRV_SOCP_WRITE_DONE(enChanlID, ulSendLen);
    }
    else
    {
        SCM_DECODER_SRC_ERR("SCM_SendDecoderSrc: Buffer Info is Full", (VOS_UINT)enChanlID,(VOS_UINT)(stRwBuf.u32RbSize));/* 记录Log */
        ulResult = VOS_ERR;
    }

    if (VOS_OK != ulResult)
    {
        SCM_DECODER_SRC_ERR("SCM_SendDecoderSrc: Write Done is Error", (VOS_UINT)enChanlID, (VOS_UINT)(stRwBuf.u32Size));/* 记录Log */
        return VOS_ERR;
    }

    return VOS_OK;
}

#endif /* (RAT_MODE != RAT_GU) */

#endif  /* SCM_ACORE */

/* ****************************************************************************
 函 数 名  : SCM_AcpuChannelMemInit
 功能描述  : 将ACPU的编码源、编码目的、解码源、解码目的通道的内存初始化，
             函数失败会复位单板，不需要释放内存
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_ChannelMemInit(VOS_VOID)
{
    VOS_UINT32                           i;
    VOS_UINT32                           ulPHYAddr;

#ifdef SCM_ACORE
    for (i=0; i<SCM_CODER_DST_NUM; i++)
    {
        /* 申请编码目的空间 */
        g_astSCMCoderDstCfg[i].pucBuf = (VOS_UINT8*)SCM_VirtMemAlloc(g_astSCMCoderDstCfg[i].ulBufLen, &ulPHYAddr);

        /* 申请空间错误 */
        if (VOS_NULL_PTR == g_astSCMCoderDstCfg[i].pucBuf)
        {
            /* 记录通道初始化标记为内存申请异常 */
            g_astSCMCoderDstCfg[i].enInitState = SCM_CHANNEL_MEM_FAIL;

            return VOS_ERR;/* 返回错误 */
        }

        g_astSCMCoderDstCfg[i].pucBufPHY = (VOS_UINT8*)ulPHYAddr;
    }

#if (RAT_MODE != RAT_GU)                /* Mutil Mode */
    for (i=0; i<SCM_DECODER_DST_NUM; i++)
    {
        /* 申请解码目的空间 */
        g_astSCMDecoderDstCfg[i].pucBuf = (VOS_UINT8*)SCM_VirtMemAlloc(g_astSCMDecoderDstCfg[i].ulBufLen, &ulPHYAddr);

        /* 申请空间错误 */
        if (VOS_NULL_PTR == g_astSCMDecoderDstCfg[i].pucBuf)
        {
            /* 记录通道初始化标记为内存申请异常 */
            g_astSCMDecoderDstCfg[i].enInitState = SCM_CHANNEL_MEM_FAIL;

            return VOS_ERR;   /* 返回错误 */
        }

        g_astSCMDecoderDstCfg[i].pucBufPHY = (VOS_UINT8*)ulPHYAddr;
    }

    for(i=0; i<SCM_DECODER_SRC_NUM; i++)
    {
        /* 申请解码源空间 */
        g_astSCMDecoderSrcCfg[i].pucSrcBuf = (VOS_UINT8*)SCM_VirtMemAlloc(g_astSCMDecoderSrcCfg[i].ulSrcBufLen, &ulPHYAddr);

        /* 申请空间错误 */
        if(VOS_NULL_PTR == g_astSCMDecoderSrcCfg[i].pucSrcBuf)
        {
            /* 记录通道初始化标记为内存申请异常 */
            g_astSCMDecoderSrcCfg[i].enInitState = SCM_CHANNEL_MEM_FAIL;

            return VOS_ERR; /* 返回错误 */
        }

        g_astSCMDecoderSrcCfg[i].pucSrcPHY = (VOS_UINT8*)ulPHYAddr;
    }
#endif  /* (RAT_MODE != RAT_GU) */

#endif  /* SCM_ACORE */

    for (i=0; i<SCM_CODER_SRC_NUM; i++)
    {
        /* 申请BD空间 */
        g_astSCMCoderSrcCfg[i].pucSrcBuf = (VOS_UINT8*)VOS_UnCacheMemAlloc(g_astSCMCoderSrcCfg[i].ulSrcBufLen, &ulPHYAddr);

        if (VOS_NULL_PTR == g_astSCMCoderSrcCfg[i].pucSrcBuf)/* 申请BD空间错误 */
        {
            /* 记录通道初始化标记为内存申请异常 */
            g_astSCMCoderSrcCfg[i].enInitState = SCM_CHANNEL_MEM_FAIL;

            return VOS_ERR;/* 返回错误 */
        }

        g_astSCMCoderSrcCfg[i].pucSrcPHY = (VOS_UINT8*)ulPHYAddr;

        if (SOCP_ENCSRC_CHNMODE_LIST == g_astSCMCoderSrcCfg[i].enCHMode)
        {
            /* 申请RD空间 */
            g_astSCMCoderSrcCfg[i].pucRDBuf = (VOS_UINT8*)VOS_UnCacheMemAlloc(g_astSCMCoderSrcCfg[i].ulRDBufLen, &ulPHYAddr);

            /* 申请RD空间错误 */
            if(VOS_NULL_PTR == g_astSCMCoderSrcCfg[i].pucRDBuf)
            {
                /* 记录通道初始化标记为内存申请异常 */
                g_astSCMCoderSrcCfg[i].enInitState = SCM_CHANNEL_MEM_FAIL;

                return VOS_ERR; /* 返回错误 */
            }

            g_astSCMCoderSrcCfg[i].pucRDPHY = (VOS_UINT8*)ulPHYAddr;
        }
    }

    return VOS_OK;
}

#if 1   /* 等待底软提供接口 */
/* ****************************************************************************
 函 数 名  : SCM_ErrorChInfoSave
 功能描述  : 将SOCP通道的通道配置数据保存到Exc文件中
 输入参数  : 无
 输出参数  : pstData : 保存数据的信息
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_VOID SCM_ErrorChInfoSave(cb_buf_t *pstData)
{
    VOS_UINT32                          ulDataLen;
    VOS_UINT8                          *pucData;

    /* 计算需要保存的通道配置信息大小 */
#ifdef SCM_CCORE
    /* 需要多申请4个标志位 */
    ulDataLen = sizeof(g_astSCMCoderSrcCfg)+ sizeof(g_stSCMInfoData) + (2*sizeof(VOS_UINT32));
#endif

#ifdef SCM_ACORE

#if (RAT_MODE != RAT_GU)                /* Mutil Mode */

    ulDataLen = sizeof(g_astSCMCoderDstCfg)
                + sizeof(g_astSCMCoderSrcCfg)
                + sizeof(g_astSCMDecoderDstCfg)
                + sizeof(g_astSCMDecoderSrcCfg)
                + sizeof(g_stSCMInfoData)
                + (5*sizeof(VOS_UINT32));        /* 需要多申请5个标志位 */
#else                                       /* GU Single Mode */
    ulDataLen = sizeof(g_astSCMCoderDstCfg)
                + sizeof(g_astSCMCoderSrcCfg)
                + sizeof(g_stSCMInfoData)
                + (3*sizeof(VOS_UINT32));        /* 需要多申请5个标志位 */;
#endif  /* (RAT_MODE != RAT_GU) */

#endif  /* SCM_ACORE */

    /* 填写数据信息 */
    VOS_StrNCpy(pstData->aucName, "SCM CHCfg Info", EXCH_CB_NAME_SIZE);

    /* 申请内存 */
    pucData = (VOS_UINT8 *)VOS_CacheMemAlloc(ulDataLen);

    if (VOS_NULL_PTR == pucData)
    {
        /* 内存申请失败，只保留部分信息 */
        pstData->pucData    = (VOS_UINT8 *)g_astSCMCoderSrcCfg;

        pstData->ulDataLen  = sizeof(g_astSCMCoderSrcCfg);
    }
    else
    {
        pstData->pucData = pucData;

        pstData->ulDataLen = ulDataLen;

        /* 保存通道的LOG信息 */
        VOS_MemSet(pucData, SCM_DATA_SAVE_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, (VOS_UINT8 *)&g_stSCMInfoData, sizeof(g_stSCMInfoData));

        pucData += sizeof(g_stSCMInfoData);

        /* 保存编码源通道的配置信息 */
        VOS_MemSet(pucData, SCM_DATA_SAVE_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, g_astSCMCoderSrcCfg, sizeof(g_astSCMCoderSrcCfg));

        pucData += sizeof(g_astSCMCoderSrcCfg);

#ifdef SCM_ACORE
        /* 保存编码目的通道的配置信息 */
        VOS_MemSet(pucData, SCM_DATA_SAVE_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, g_astSCMCoderDstCfg, sizeof(g_astSCMCoderDstCfg));

        pucData += sizeof(g_astSCMCoderDstCfg);

#if (RAT_MODE != RAT_GU)                /* Mutil Mode */
        /* 保存解码目的通道的配置信息 */
        VOS_MemSet(pucData, SCM_DATA_SAVE_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, g_astSCMDecoderDstCfg, sizeof(g_astSCMDecoderDstCfg));

        pucData += sizeof(g_astSCMDecoderDstCfg);

        /* 保存解码解码源通道的配置信息 */
        VOS_MemSet(pucData, SCM_DATA_SAVE_TAG, sizeof(VOS_UINT32));

        pucData += sizeof(VOS_UINT32);

        VOS_MemCpy(pucData, g_astSCMDecoderSrcCfg, sizeof(g_astSCMDecoderSrcCfg));
#endif  /* (RAT_MODE != RAT_GU) */

#endif  /* SCM_ACORE */
    }

    return;    /* 返回，单板马上重启不需要释放内存 */
}

#endif

/* ****************************************************************************
 函 数 名  : SCM_ChannelInit
 功能描述  : 将SOCP通道的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_ERR/VOS_OK
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
**************************************************************************** */
VOS_UINT32 SCM_ChannelInit(VOS_VOID)
{
    VOS_MemSet(&g_stSCMInfoData, 0, sizeof(g_stSCMInfoData));

/* log2.0 2014-03-19 Begin:*/
#ifdef SCM_ACORE
        SCM_ChangeCoderDstChanCfg();
#endif
/* log2.0 2014-03-19 End:*/

    if (VOS_OK != SCM_ChannelMemInit())/* 通道内存初始化 */
    {
        vos_printf("111 SCM_ChannelMemInit!\n");
        return VOS_ERR;/* 返回错误 */
    }

#ifdef SCM_ACORE
    if (VOS_OK != SCM_CoderDstChannelInit())
    {
        vos_printf("222 SCM_CoderDstChannelInit!\n");
        return VOS_ERR;/* 返回错误 */
    }
#if (RAT_MODE != RAT_GU)                /* Mutil Mode */
    if (VOS_OK != SCM_DecoderDstChannelInit())
    {
        vos_printf("333 SCM_DecoderDstChannelInit!\n");
        return VOS_ERR;/* 返回错误 */
    }

    if (VOS_OK != SCM_DecoderSrcChannelInit())
    {
        vos_printf("444 SCM_DecoderSrcChannelInit!\n");
        return VOS_ERR;/* 返回错误 */
    }
#endif  /* (RAT_MODE != RAT_GU) */
#endif  /* SCM_ACORE */

    if (VOS_OK != SCM_CoderSrcChannelInit()) /* 编码源通道初始化 */
    {
        vos_printf("555 SCM_CoderSrcChannelInit!\n");
        return VOS_ERR;/* 返回错误 */
    }

    /* 注册异常信息保存回调 */
    DRV_EXCH_CUST_FUNC_REG((exchCBReg)SCM_ErrorChInfoSave);

#ifdef SCM_CCORE
    Om_BbpDbgChanInit();
#endif

    /*V7R2 底软SOCP通道使能接口*/
    DRV_SOCP_CHAN_ENABLE();

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : SCM_ChannelInfoShow
 功能描述  : 将SOCP通道的信息打印到串口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID SCM_ChannelInfoShow(VOS_VOID)
{
    VOS_UINT32      i;

    for(i=0; i<SCM_CODER_SRC_NUM; i++)
    {
        vos_printf("\r\n The Channle 0x%x info is :", g_astSCMCoderSrcCfg[i].enChannelID);

        vos_printf("\r\n The Max BD num is %d", g_stSCMInfoData.aulBDUsedMax[i]);

        vos_printf("\r\n The Max rD num is %d \r\n", g_stSCMInfoData.aulRDUsedMax[i]);
    }

    return;
}

/*****************************************************************************
 函 数 名  : SCM_CoderSrcCHShow
 功能描述  : 将SOCP 编码源通道的信息打印到串口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID SCM_CoderSrcCHShow(VOS_UINT32 ulCfgNum)
{
    vos_printf("\r\n CH id         is 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].enChannelID);
    vos_printf("\r\n CH init state is   %d", g_astSCMCoderSrcCfg[ulCfgNum].enInitState);
    vos_printf("\r\n CH type       is   %d", g_astSCMCoderSrcCfg[ulCfgNum].enCHMode);
    vos_printf("\r\n CH Dst        is 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].enDstCHID);
    vos_printf("\r\n CH data type  is   %d", g_astSCMCoderSrcCfg[ulCfgNum].enDataType);
    vos_printf("\r\n CH Level      is   %d", g_astSCMCoderSrcCfg[ulCfgNum].enCHLevel);
    vos_printf("\r\n CH BD VirtAddris 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].pucSrcBuf);
    vos_printf("\r\n CH BD PHYAddr is 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].pucSrcPHY);
    vos_printf("\r\n CH BD BufLen  is 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].ulSrcBufLen);
    vos_printf("\r\n CH RD VirtAddris 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].pucRDBuf);
    vos_printf("\r\n CH RD PHYAddr is 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].pucRDPHY);
    vos_printf("\r\n CH RD BufLen  is 0x%x", g_astSCMCoderSrcCfg[ulCfgNum].ulRDBufLen);

    return;
}

#ifdef SCM_ACORE

/*****************************************************************************
 函 数 名  : SCM_CoderDstCHShow
 功能描述  : 将SOCP 编码源通道的信息打印到串口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2012年8月8日
     作    者  : zhuli
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID SCM_CoderDstCHShow(VOS_UINT32 ulCfgNum)
{
    vos_printf("\r\n CH id         is 0x%x", g_astSCMCoderDstCfg[ulCfgNum].enChannelID);
    vos_printf("\r\n CH init state is   %d", g_astSCMCoderDstCfg[ulCfgNum].enInitState);
    vos_printf("\r\n CH BD VirtAddris 0x%x", g_astSCMCoderDstCfg[ulCfgNum].pucBuf);
    vos_printf("\r\n CH BD PHYAddr is 0x%x", g_astSCMCoderDstCfg[ulCfgNum].pucBufPHY);
    vos_printf("\r\n CH BD BufLen  is 0x%x", g_astSCMCoderDstCfg[ulCfgNum].ulBufLen);
    vos_printf("\r\n CH threshold  is 0x%x", g_astSCMCoderDstCfg[ulCfgNum].ulThreshold);
    vos_printf("\r\n CH CB func    is 0x%x", g_astSCMCoderDstCfg[ulCfgNum].pfunc);

    return;
}
#endif

/*****************************************************************************
 函 数 名  : SCM_StopAllSrcChan
 功能描述  : disable 所有C核，HIFI使用的编码源通道
 输入参数  : ulSrcChanID 通道ID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
 修改历史  :
   1.日    期  : 2013年5月2日
     作    者  : j0174725s
     修改内容  : Creat Function
*****************************************************************************/
VOS_VOID SCM_StopAllSrcChan(VOS_VOID)
{
    /* K3V3上，通道去使能在M3完成 */
#ifndef BSP_CONFIG_HI3630    
    VOS_UINT32                          ulSrcChanID;
    VOS_UINT32                          ulNum;
    VOS_UINT32                          i;

    ulNum = sizeof(g_ulCloseSrcNum)/sizeof(SOCP_CODER_SRC_ENUM_U32);

    for (i = 0; i < ulNum; i++)
    {
        ulSrcChanID = g_ulCloseSrcNum[i];

        DRV_SOCP_STOP(ulSrcChanID);
    }
#endif
    return ;
}


/*适配V9代码可维可测打桩，要保留*/
VOS_VOID SCM_LogToFile(FILE *fp)
{
    return;
}
VOS_VOID SOCP_LogToFile(FILE *fp)
{
    return;
}


VOS_UINT8* SCM_GetDebugLogInfo(VOS_VOID)
{
    return (VOS_UINT8*)&g_stSCMInfoData;
}


VOS_UINT32 SCM_GetDebugLogInfoLen(VOS_VOID)
{
    return (VOS_UINT32)sizeof(g_stSCMInfoData);
}


VOS_UINT8* SOCP_GetDebugLogInfo(VOS_UINT32  ulType)
{
    return VOS_NULL;
}


VOS_UINT32 SOCP_GetDebugLogInfoLen(VOS_UINT32  ulType)
{
    return 0;
}


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




