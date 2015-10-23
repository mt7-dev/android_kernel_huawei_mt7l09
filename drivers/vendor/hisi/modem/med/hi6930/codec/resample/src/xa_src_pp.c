

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "xa_src_pp.h"
#include "ucom_mem_dyn.h"
#include "codec_op_lib.h"
#include "om_log.h"
#ifndef _MED_C89_
#include <xtensa/tie/xt_hifi2.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_CODEC_XA_SRC_PP_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* SRC库入口函数指针 */
xa_codec_func_t *                       g_pfuncAudioSrcApi;

/* SRC解码对象 */
xa_codec_handle_t                       g_pastAudioSrcApiObj[XA_SRC_PROC_ID_BUTT + 1];

/* SRC 模块内存配置信息 */
XA_COMM_MEMORY_CFG_STRU                 g_astSrcMemCfg[XA_SRC_PROC_ID_BUTT + 1];

/* SRC模块当前运行状态配置信息 */
XA_SRC_CURRENT_CFG_STRU                 g_astSrcCurrentCfg[XA_SRC_PROC_ID_BUTT + 1];

/* 按照tensilica库要求，配置输入输出buff */
UCOM_ALIGN(8)
XA_SRC_IO_CFG_STRU                      g_astIOBuffCfg[XA_SRC_PROC_ID_BUTT + 1];

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_INT32 XA_SRC_Init( XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                           XA_SRC_USER_CFG_STRU * pstSrcUserCfg)
{
    xa_codec_func_t                    *pfuncSrcApi;
    xa_codec_handle_t                   pstSrcApiObj;
    XA_COMM_MEMORY_CFG_STRU            *pstSrcMemCfg;
    XA_SRC_CURRENT_CFG_STRU            *pstCurrentCfg;
    VOS_INT32                           swErrorCode     = VOS_OK;

    /* 异常入参检查 */
    if (  (VOS_NULL == pstSrcUserCfg)
        ||(pstSrcUserCfg->swChannels > XA_SRC_MAX_CHANLE_NUM)
        ||(pstSrcUserCfg->swInputChunkSize > XA_SRC_MAX_INPUT_CHUNK_LEN)
        ||(enProcId > XA_SRC_PROC_ID_BUTT))
    {
        return VOS_ERR;
    }

    /* 将对应处理ID先清空 */
    XA_SRC_Close(enProcId);

    /* 获取参数 */
    XA_SRC_SetApiPtr(xa_src_pp);
    pfuncSrcApi     = XA_SRC_GetApiPtr();
    pstSrcMemCfg    = XA_SRC_GetMemCfgPtr(enProcId);
    pstCurrentCfg   = XA_SRC_GetCurrentCfgPtr(enProcId);

    /* 调用XA_COMM_Startup，为g_pstAudioSrcApiObj分配内存空间并且设置为缺省值 */
    swErrorCode = XA_COMM_Startup(pfuncSrcApi, &(XA_SRC_GetApiObjPtr(enProcId)), pstSrcMemCfg);
    XA_ERROR_HANDLE(swErrorCode);

    /* 对APIObj临时变量赋值 */
    pstSrcApiObj = XA_SRC_GetApiObjPtr(enProcId);

    /* 调用XA_SRC_SetPara，配置用户信息 */
    swErrorCode = XA_SRC_SetPara(enProcId, pstSrcUserCfg);
    XA_ERROR_HANDLE(swErrorCode);

    /* 调用XA_COMM_MemoryAlloc，设置memory tables */
    swErrorCode = XA_SRC_MemoryAlloc(pfuncSrcApi, pstSrcApiObj, pstSrcMemCfg, pstSrcUserCfg, enProcId);
    XA_ERROR_HANDLE(swErrorCode);

    /* 配置输入输出Buff */
    XA_SRC_SetInputBuff(enProcId, pstSrcMemCfg->pInputBuff);
    XA_SRC_SetOutputBuff(enProcId, pstSrcMemCfg->pOutputBuff);

    /* 初始化SRC模块 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_INIT,
                                 XA_CMD_TYPE_INIT_PROCESS,
                                 NULL);
    XA_ERROR_HANDLE(swErrorCode);

    /* 重置历史缓冲Buff */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_EXECUTE,
                                 XA_CMD_TYPE_DO_RUNTIME_INIT,
                                 NULL);
    XA_ERROR_HANDLE(swErrorCode);

    /* 配置输出buff大小为空 */
    //pstSrcUserCfg->swOutputSize = 0;

    /* 记录本次初始化所配置的信息 */
    pstCurrentCfg->swChannels           = pstSrcUserCfg->swChannels;
    pstCurrentCfg->swInputChunkSize     = pstSrcUserCfg->swInputChunkSize;
    pstCurrentCfg->swInputSmpRat        = pstSrcUserCfg->swInputSmpRat;
    pstCurrentCfg->swOutputSmpRat       = pstSrcUserCfg->swOutputSmpRat;
    pstCurrentCfg->swOutputChunkSize    = 0;
    pstCurrentCfg->enInitFlag           = XA_SRC_INIT_OK;

    return XA_NO_ERROR;

}


#if 1
VOS_VOID XA_SRC_Convert16To24bit(XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                         VOS_VOID *pSrcBuff,
                                         VOS_INT32 *pswDestBuff)
{
    XA_SRC_CURRENT_CFG_STRU            *pstCurrentCfg   = XA_SRC_GetCurrentCfgPtr(enProcId);
    VOS_INT16                          *pshwOriginal    = (VOS_INT16 *)pSrcBuff;
    VOS_INT32                           swTotalNum;
    VOS_INT32                           swCnt;

    /* 获取输入Buff中所有样点个数 */
    swTotalNum = (pstCurrentCfg->swChannels) * (pstCurrentCfg->swInputChunkSize);

    /* 遍历每一个16bit输入语音信号，用32bit表示16bit
     *
     *  |      8bit    | 8bit | 8bit | 8bit |
     *  |sign extension|    占用位   |   0  |
     */
    for (swCnt = 0; swCnt < swTotalNum; swCnt++)
    {
        pswDestBuff[swCnt] = pshwOriginal[swCnt];

        pswDestBuff[swCnt] <<= 8;
    }

}
#else
VOS_VOID XA_SRC_Convert16To24bit(XA_SRC_USER_CFG_STRU * pstSrcUserCfg, VOS_INT32 *pswInputBuff)
{
    VOS_INT16                          *pshwOriginal    = pstSrcUserCfg->pInputBuff;
    VOS_INT32                          *pswInput;
    VOS_INT32                           swTotalNum;
    VOS_INT32                           k;
    ae_p24x2s                           aepX1, aepX2, aepY1, aepY2;


    /* 获取输入Buff中所有样点个数 */
    swTotalNum = ((pstSrcUserCfg->swChannels) * (pstSrcUserCfg->swInputChunkSize))/4;

    /* 遍历每一个16bit输入语音信号，用32bit表示16bit
     *
     *  |      8bit    | 8bit | 8bit | 8bit |
     *  |sign extension|    占用位   |   0  |
     */

    WUR_AE_SAR(8);

    pswInput = pswInputBuff;

    for (k = 0; k < swTotalNum; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwOriginal[4*k]);
        aepX2 = *((ae_p16x2s *)&pshwOriginal[4*k + 2]);

        aepY1 = AE_SRASP24(aepX1);
        aepY2 = AE_SRASP24(aepX2);

        *((ae_p24x2f *) &pswInputBuff[4*k])     = aepY1;
        *((ae_p24x2f *) &pswInputBuff[4*k + 2]) = aepY2;
    }

}
#endif


#ifdef _MED_C89_
VOS_VOID  XA_SRC_ConvertChn24To16bit( VOS_INT16* pshwDst, VOS_INT32 *pswSrc, VOS_UINT32 uwLen)
{
    VOS_UINT32  i;

    /* 处理每一个声道 */
    for(i = 0; i < uwLen; i++)
    {
        /* 算数右移8位 */
        pswSrc[i] = CODEC_OpL_shr(pswSrc[i], 8);

        pshwDst[i] = (VOS_INT16)(pswSrc[i]);
     }
}
#else
VOS_VOID  XA_SRC_ConvertChn24To16bit( VOS_INT16* pshwDst, VOS_INT32 *pswSrc, VOS_UINT32 uwLen)
{
    VOS_INT32                           k;
    ae_p24x2s                           aepX1, aepX2, aepY1, aepY2;
    VOS_UINT32                          uwCnt;

    uwCnt = uwLen / 4;

    WUR_AE_SAR(8);

    for (k = 0; k < uwCnt; k++)
    {
        aepY1 = *((ae_p24x2f *) &pswSrc[4*k]);
        aepY2 = *((ae_p24x2f *) &pswSrc[4*k + 2]);

        aepX1 = AE_SLLSP24(aepY1);
        aepX2 = AE_SLLSP24(aepY2);

        *((ae_p16x2s *)&pshwDst[4*k])     = aepX1;
        *((ae_p16x2s *)&pshwDst[4*k + 2]) = aepX2;
    }

    if(uwLen & 0x2)
    {
        uwCnt = (uwLen & 0xFFFFFFFC);

        aepY1 = *((ae_p24x2f *) &pswSrc[uwCnt]);

        aepX1 = AE_SLLSP24(aepY1);

        *((ae_p16x2s *)&pshwDst[uwCnt])     = aepX1;
    }

    if(uwLen & 0x1)
    {
        aepY1 = *((ae_p24f *) &pswSrc[uwLen - 1]);

        aepX1 = AE_SLLSP24(aepY1);

        *((ae_p16s *)&pshwDst[uwLen - 1])     = aepX1;
    }

}
#endif



VOS_VOID XA_SRC_Convert24To16bit( XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                          VOS_INT32 **ppuwSrcBuff,
                                          VOS_VOID  *pDestBuff)
{
    XA_SRC_CURRENT_CFG_STRU            *pstCurrentCfg           = XA_SRC_GetCurrentCfgPtr(enProcId);
    VOS_UINT32                          uwChannleNum;
    VOS_INT16                          *pshwOut;

    pshwOut = (VOS_INT16*)pDestBuff;

    /* 遍历每一个SRC输出的24bit数据，将其转化为16bit，截取高16bit
     *
     *  | 8bit | 8bit | 8bit | 8bit |
     *  |   0  |    截取位   |   0  |
     */
    for(uwChannleNum = 0; uwChannleNum < (VOS_UINT32)pstCurrentCfg->swChannels; ++uwChannleNum)
    {
        XA_SRC_ConvertChn24To16bit(
                    &pshwOut[uwChannleNum * (VOS_UINT32)pstCurrentCfg->swOutputChunkSize],
                    ppuwSrcBuff[uwChannleNum],
                    (VOS_UINT32)pstCurrentCfg->swOutputChunkSize);
    }

}



VOS_INT32 XA_SRC_SetPara(XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                              XA_SRC_USER_CFG_STRU *pstSrcUserCfg)
{
    xa_codec_func_t *                   pfuncSrcApi     = XA_SRC_GetApiPtr();
    xa_codec_handle_t                   pstSrcApiObj    = XA_SRC_GetApiObjPtr(enProcId);
    VOS_INT32                           swErrorCode     = 0;

    /* 设置输入采样率 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                        XA_API_CMD_SET_CONFIG_PARAM,
                                        XA_SRC_PP_CONFIG_PARAM_INPUT_SAMPLE_RATE,
                                       &(pstSrcUserCfg->swInputSmpRat));
    XA_ERROR_HANDLE(swErrorCode);

    /* 设置输出采样率 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                       XA_API_CMD_SET_CONFIG_PARAM,
                                       XA_SRC_PP_CONFIG_PARAM_OUTPUT_SAMPLE_RATE,
                                       &(pstSrcUserCfg->swOutputSmpRat));
    XA_ERROR_HANDLE(swErrorCode);

    /* 设置输入语音数据声道个数 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                       XA_API_CMD_SET_CONFIG_PARAM,
                                       XA_SRC_PP_CONFIG_PARAM_INPUT_CHANNELS,
                                       &(pstSrcUserCfg->swChannels));
    XA_ERROR_HANDLE(swErrorCode);

    /* 设置输入语音采样点个数(chunk size) */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                       XA_API_CMD_SET_CONFIG_PARAM,
                                       XA_SRC_PP_CONFIG_PARAM_INPUT_CHUNK_SIZE,
                                       &(pstSrcUserCfg->swInputChunkSize));
    XA_ERROR_HANDLE(swErrorCode);

    return VOS_OK;
}


VOS_UINT32 XA_SRC_ExeProcess( XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                                     VOS_VOID   *pInputBuff,
                                     VOS_VOID   *pOutputBuff,
                                     VOS_UINT32 *puwOutputBuffSize)
{
    xa_codec_func_t                    *pfuncSrcApi;
    xa_codec_handle_t                   pstSrcApiObj;
    XA_SRC_CURRENT_CFG_STRU            *pstCurrentCfg;
    VOS_INT32                           swErrorCode;

    /* 异常入参检查 */
    if (  (VOS_NULL == pInputBuff)
        ||(VOS_NULL == pOutputBuff)
        ||(XA_SRC_PROC_ID_BUTT <= enProcId))
    {
        OM_LogError1(XA_SRC_Para_Error, enProcId);

        return VOS_ERR;
    }

    /* 获取参数 */
    pfuncSrcApi     = XA_SRC_GetApiPtr();
    pstCurrentCfg   = XA_SRC_GetCurrentCfgPtr(enProcId);
    pstSrcApiObj    = XA_SRC_GetApiObjPtr(enProcId);

    /* 如果未初始化，则返回err */
    if (XA_SRC_INIT_OK != pstCurrentCfg->enInitFlag)
    {
        OM_LogError1(XA_SRC_Para_Error, pstCurrentCfg->enInitFlag);

        return VOS_ERR;
    }

    XA_SRC_Convert16To24bit(enProcId, pInputBuff, g_astIOBuffCfg[enProcId].paswInputBuff[0]);

    /* 设置输入Buff采样点个数 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_SRC_PP_CONFIG_PARAM_INPUT_CHUNK_SIZE,
                                 &(pstCurrentCfg->swInputChunkSize));
    XA_ERROR_HANDLE(swErrorCode);

    /* 设置输入、输出Buff，要求首地址8字节对齐 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_SRC_PP_CONFIG_PARAM_SET_INPUT_BUF_PTR,
                                 g_astIOBuffCfg[enProcId].paswInputBuff);
    XA_ERROR_HANDLE(swErrorCode);

    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_SRC_PP_CONFIG_PARAM_SET_OUTPUT_BUF_PTR,
                                 g_astIOBuffCfg[enProcId].paswOutputBuff);
    XA_ERROR_HANDLE(swErrorCode);

    /* 执行变采样 */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_EXECUTE,
                                 XA_CMD_TYPE_DO_EXECUTE,
                                 NULL);
    XA_ERROR_HANDLE(swErrorCode);

    /* 获取变采样之后chunk size(采样点个数) */
    swErrorCode = (*pfuncSrcApi)(pstSrcApiObj,
                                 XA_API_CMD_GET_CONFIG_PARAM,
                                 XA_SRC_PP_CONFIG_PARAM_OUTPUT_CHUNK_SIZE,
                                 &(pstCurrentCfg->swOutputChunkSize));
    XA_ERROR_HANDLE(swErrorCode);

    /* 配置模块内部输出 */
    pstCurrentCfg->pswSrcOutput = g_astIOBuffCfg[enProcId].paswOutputBuff[0];

    /* 将SRC模块输出的24bit数据转换为16bit */
    XA_SRC_Convert24To16bit(enProcId, g_astIOBuffCfg[enProcId].paswOutputBuff, pOutputBuff);

    /* 输出采样点个数 */
    *puwOutputBuffSize = (VOS_UINT32)(pstCurrentCfg->swChannels * pstCurrentCfg->swOutputChunkSize) * sizeof(VOS_UINT16);

    return VOS_OK;

}


VOS_UINT32 XA_SRC_Close(XA_SRC_PROC_ID_ENUM_UINT32 enProcId)
{
    xa_codec_handle_t                   pstSrcApiObj    = XA_SRC_GetApiObjPtr(enProcId);
    XA_COMM_MEMORY_CFG_STRU            *pstSrcMemCfg    = XA_SRC_GetMemCfgPtr(enProcId);
    XA_SRC_CURRENT_CFG_STRU            *pstCurrentCfg   = XA_SRC_GetCurrentCfgPtr(enProcId);
    VOS_UINT32                          uwCnt;

    /* 赋空句柄 */
    XA_SRC_SetApiPtr(VOS_NULL);

    /* 释放ApiObj空间 */
    UCOM_MemFree(pstSrcApiObj);
    XA_SRC_SetApiObjPtr(enProcId, VOS_NULL);

    /* 释放所有动态申请空间 */
    XA_COMM_MemFree(pstSrcMemCfg);

    /* 当前SRC运行信息清空 */
    UCOM_MemSet(pstCurrentCfg, 0, sizeof(XA_SRC_CURRENT_CFG_STRU));

    /* 显式标志初始化位 */
    pstCurrentCfg->enInitFlag = XA_SRC_INIT_NO;

    /* 清空内部模块输入输出指针 */
    for (uwCnt = 0; uwCnt < 8; uwCnt++)
    {
        g_astIOBuffCfg[enProcId].paswInputBuff[uwCnt]  = VOS_NULL;
        g_astIOBuffCfg[enProcId].paswOutputBuff[uwCnt] = VOS_NULL;
    }

    return VOS_OK;

}


/*****************************************************************************
函 数 名：XA_COMM_SetBlksMemory
功能描述：Tensilica库设置各个内存块的指针
输    入：xa_codec_func_t           p_xa_process_api        tensilica库入口函数指针
          xa_codec_handle_t         p_process_handle        codec句柄
          CODEC_MEMORY_CFG_STRU*    pstMemoryCfg
输    出：
返 回 值：XA_NO_ERROR 成功  ；否则失败
*****************************************************************************/
VOS_INT32 XA_SRC_SetBlksMemory(xa_codec_func_t        *pfuncProcess,
                                xa_codec_handle_t               pstApiObj,
                                XA_COMM_MEMORY_CFG_STRU        *pstMemoryCfg,
                                XA_SRC_USER_CFG_STRU           *pstSrcUserCfg,
                                XA_SRC_PROC_ID_ENUM_UINT32      enProcId)
{
    VOS_INT32  err_code;
    VOS_INT32  swMemBlockNum;
    VOS_INT32  swSize;
    VOS_INT32  swAlignment;
    VOS_INT32  swType;
    VOS_VOID*  pvMemory;
    VOS_INT32  swMemIndex;
    VOS_INT*   ptemp;
    VOS_INT32  swCnt;
    VOS_INT32  swOutput;

    /* 获取需要的内存块数量 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_GET_N_MEMTABS,
                               0,
                               &swMemBlockNum);
    CODEC_XA_ERRCODE_CHECK(err_code);

    for(swMemIndex=0; swMemIndex<swMemBlockNum; swMemIndex++)
    {
        /* 获取需要内存大小*/
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_GET_MEM_INFO_SIZE,
                                   swMemIndex,
                                   &swSize);

        /* 采用在DDR中动态申请的方式申请内存，内存首地址均为8字节对齐 */
        pvMemory = (VOS_VOID *)UCOM_MemAlloc(swSize);

        if(VOS_NULL == pvMemory)
        {
            return XA_COM_CUSTOM_NULL_POINT_ERROR;
        }

        CODEC_XA_ERRCODE_CHECK(err_code);

        /* 查询所需对齐方式 */
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_GET_MEM_INFO_ALIGNMENT,
                                   swMemIndex,
                                   &swAlignment);
        CODEC_XA_ERRCODE_CHECK(err_code);

        /* 只支持8字节对齐 */
        if (    (1 != swAlignment) &&
                (2 != swAlignment) &&
                (4 != swAlignment) &&
                (8 != swAlignment))
        {
            return VOS_ERR;
        }

        /* 查询所需内存类型 */
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_GET_MEM_INFO_TYPE,
                                   swMemIndex,
                                   &swType);
        CODEC_XA_ERRCODE_CHECK(err_code);

        switch(swType)
        {
        case XA_MEMTYPE_PERSIST:

            /* 记录分配内存信息 */
            pstMemoryCfg->pPersistBuff      = pvMemory;
            pstMemoryCfg->swPersistMemSize  = swSize;
            break;
        case XA_MEMTYPE_SCRATCH:

            /* 记录分配内存信息 */
            pstMemoryCfg->pScratchBuff      = pvMemory;
            pstMemoryCfg->swScratchMemSize  = swSize;
            break;
        case XA_MEMTYPE_INPUT:

            /* 记录分配内存信息 */
            pstMemoryCfg->pInputBuff        = pvMemory;
            pstMemoryCfg->swInputMemSize    = swSize;

            /* 配置ppin_buff */
            ptemp = (VOS_INT32 *)pvMemory;
            for(swCnt= 0; swCnt < pstSrcUserCfg->swChannels; ++swCnt)
            {
                //g_paswInputBuff[swCnt] = (VOS_INT32 *)(ptemp + swCnt);
                g_astIOBuffCfg[enProcId].paswInputBuff[swCnt] = (VOS_INT32 *)(ptemp + swCnt);
            }

            break;
        case XA_MEMTYPE_OUTPUT:

            /* 记录分配内存信息 */
            pstMemoryCfg->pOutputBuff       = pvMemory;
            pstMemoryCfg->swOutputMemSize   = swSize;

            /* 设置输出buff */
            ptemp = (VOS_INT32 *)pvMemory;
            swOutput = pstMemoryCfg->swOutputMemSize/((pstSrcUserCfg->swChannels) * (VOS_INT32)sizeof(VOS_INT32));
            for(swCnt = 0; swCnt < pstSrcUserCfg->swChannels; ++swCnt)
            {
                //g_paswOutputBuff[swCnt] = (VOS_INT32 *)(ptemp + (swCnt * swOutput));
                g_astIOBuffCfg[enProcId].paswOutputBuff[swCnt] = (VOS_INT32 *)(ptemp + (swCnt * swOutput));
            }

            break;
        default:
            return XA_COM_CUSTOM_MEM_BLOCK_SIZE_ERROR;
        }

        /* 设置内存指针 */
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_SET_MEM_PTR,
                                   swMemIndex,
                                   pvMemory);
        CODEC_XA_ERRCODE_CHECK(err_code);
    }

    return err_code;
}


/*****************************************************************************
函 数 名：XA_COMM_MemoryAlloc
功能描述：为tensilica的库函数分配必须的内存空间
输    入：xa_codec_func_t           p_xa_process_api        tensilica库入口函数指针
          xa_codec_handle_t         p_process_handle        API配置信息
          CODEC_MEMORY_CFG_STRU*    pstMemoryCfg            用于记录该模块所动态申请的内存，
                                                            用于退出该模块时释放内存所用，
                                                            调用该函数的模块负责为该结构体分配内存
输    出：
返 回 值：XA_NO_ERROR 成功  ；否则失败
*****************************************************************************/
VOS_INT32  XA_SRC_MemoryAlloc (xa_codec_func_t                  *pfuncProcess,
                                       xa_codec_handle_t                 pstApiObj,
                                       XA_COMM_MEMORY_CFG_STRU          *pstMemoryCfg,
                                       XA_SRC_USER_CFG_STRU             *pstSrcUserCfg,
                                       XA_SRC_PROC_ID_ENUM_UINT32        enProcId)
{
    VOS_INT32   err_code;
    VOS_INT32   swTableMemSize;
    VOS_VOID*   pTableBuff;

    if(   (VOS_NULL == pfuncProcess)
       || (VOS_NULL == pstApiObj)
       || (VOS_NULL == pstMemoryCfg)
       || (VOS_NULL == pstSrcUserCfg))
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }

    /* 获取 memory info tables 大小 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_GET_MEMTABS_SIZE,
                               0,
                               &(swTableMemSize));
    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 采用在DDR中动态申请的方式申请内存，内存首地址均为8字节对齐 */
    pTableBuff = (VOS_VOID *)UCOM_MemAlloc(swTableMemSize);

    /* 记录分配的TableMem */
    pstMemoryCfg->pTableBuff        = pTableBuff;
    pstMemoryCfg->swTableMemSize    = swTableMemSize;

    /* 设置 memory tables 指针 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_SET_MEMTABS_PTR,
                               0,
                               pTableBuff);
    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 设置参数 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_INIT,
                               XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS,
                               NULL);
    CODEC_XA_ERRCODE_CHECK(err_code);

    err_code = XA_SRC_SetBlksMemory(pfuncProcess, pstApiObj, pstMemoryCfg, pstSrcUserCfg, enProcId);
    CODEC_XA_ERRCODE_CHECK(err_code);

    return VOS_OK;
}
VOS_UINT32  XA_SRC_Proc8x(XA_SRC_PROC_ID_ENUM_UINT32   enProcId,
                                VOS_INT16                   *pshwInputBuff,
                                VOS_INT16                   *pshwOutputBuff,
                                VOS_UINT32                  *puwOutPutSize,
                                VOS_UINT32                   uwProcTimes)
{
    VOS_VOID                *pInputBuffTmp;
    VOS_VOID                *pOutputBuffTmp;
    XA_SRC_CURRENT_CFG_STRU *pstCurrentCfg;
    VOS_UINT32               uwOutSize;
    VOS_UINT32               uwCnt;
    VOS_UINT32               uwRet;

    /* 异常入参检查 */
    if (  (XA_SRC_PROC_ID_BUTT <= enProcId)
        ||(VOS_NULL == pshwInputBuff)
        ||(VOS_NULL == pshwOutputBuff))
    {
        OM_LogError(XA_SRC_Para_Error);

        return VOS_ERR;
    }

    /* 获取相应的配置 */
    pstCurrentCfg = XA_SRC_GetCurrentCfgPtr(enProcId);

    /* 设置输出buff */
    pOutputBuffTmp = (VOS_VOID *)pshwOutputBuff;

    /* 设置输出buff大小 */
    *puwOutPutSize = 0;

    /* 采用分段处理，每次处理AUDIO_PLAYER_CHUNK_SIZE个采样点 */
    for (uwCnt = 0; uwCnt < uwProcTimes; uwCnt++)
    {
        /* 设置输入buff */
        pInputBuffTmp = (VOS_VOID *)(pshwInputBuff + \
                                    (uwCnt * (VOS_UINT32)(pstCurrentCfg->swInputChunkSize* pstCurrentCfg->swChannels)));

        /* 执行变采样 */
        uwRet = XA_SRC_ExeProcess(enProcId,
                                  pInputBuffTmp,
                                  pOutputBuffTmp,
                                  &uwOutSize);
        /* 执行异常，返回 */
        if(uwRet != VOS_OK)
        {
            OM_LogError1(XA_SRC_ExeProcess_Fail, uwRet);

            return VOS_ERR;
        }

        /* 更新输出数据指针 */
        pOutputBuffTmp = (VOS_INT16*)pOutputBuffTmp + \
                      (uwOutSize/(VOS_INT32)sizeof(VOS_INT16));

        /* 更新输出buff大小 */
        *puwOutPutSize = *puwOutPutSize + uwOutSize;

    }

    return VOS_OK;

}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

