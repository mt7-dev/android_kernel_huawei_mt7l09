

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "xa_comm.h"
#include "ucom_mem_dyn.h"
#include "xa_mp3_dec_api.h"
#include "audio_player.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_CODEC_XA_COMM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
函 数 名：XA_COMM_Startup
功能描述：为API结构体分配内存空间，初始化API结构体，初始化默认设置
输    入：pfProcess         -- tensilica库入口函数指针
          pApiObj           -- codec句柄
          pstMemCfg         -- 调用此函数的模块自身维护
输    出：
返 回 值：XA_NO_ERROR 成功；非XA_NO_ERROR失败
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_INT32 XA_COMM_Startup(xa_codec_func_t*  pfuncProcess,
                          xa_codec_handle_t*       ppstApiObj,
                          XA_COMM_MEMORY_CFG_STRU* pstMemCfg)
{
    VOS_INT32  err_code = XA_NO_ERROR;
    VOS_INT32  swApiObjSize;

    /* 异常入参检查 */
    if(   (VOS_NULL == pfuncProcess)
       || (VOS_NULL == ppstApiObj)
       || (VOS_NULL == pstMemCfg) )
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }



   /* 获取API结构大小 */
    err_code = (*pfuncProcess)(NULL,
                               XA_API_CMD_GET_API_SIZE,
                               0,
                               (VOS_VOID *)(&swApiObjSize));

    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 采用在DDR中动态申请的方式申请内存，内存首地址均为8字节对齐 */
    (*ppstApiObj) = (xa_codec_handle_t)UCOM_MemAlloc(swApiObjSize);

    if(VOS_NULL == (*ppstApiObj))
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }

    /* 记录分配的APIMem */
    pstMemCfg->pApiStructBuff= (*ppstApiObj);
    pstMemCfg->swApiObjSize  = swApiObjSize;

    /* 清空API */
    UCOM_MemSet(pstMemCfg->pApiStructBuff, 0, (VOS_UINT32)pstMemCfg->swApiObjSize);

    /* 初始化默认设置 */
    err_code = (*pfuncProcess)((*ppstApiObj),
                               XA_API_CMD_INIT,
                               XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS,
                               NULL);

    CODEC_XA_ERRCODE_CHECK(err_code);

    return err_code;
}

/*****************************************************************************
函 数 名：XA_COMM_SetBlksMemory
功能描述：Tensilica库设置各个内存块的指针
输    入：xa_codec_func_t           p_xa_process_api        tensilica库入口函数指针
          xa_codec_handle_t         p_process_handle		codec句柄
          CODEC_MEMORY_CFG_STRU*    pstMemoryCfg
输    出：
返 回 值：XA_NO_ERROR 成功  ；否则失败
*****************************************************************************/
VOS_INT32 XA_COMM_SetBlksMemory(xa_codec_func_t*           pfuncProcess,
                                xa_codec_handle_t          pstApiObj,
                                XA_COMM_MEMORY_CFG_STRU* pstMemoryCfg)
{
    VOS_INT32  err_code;
    VOS_INT32  swMemBlockNum;
    VOS_INT32  swSize;
    VOS_INT32  swAlignment;
    VOS_INT32  swType;
    VOS_VOID*  pvMemory;
    VOS_INT32  swMemIndex;

    if(   (VOS_NULL == pfuncProcess)
       || (VOS_NULL == pstApiObj)
       || (VOS_NULL == pstMemoryCfg) )
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }

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

        /* 只支持1/2/4/8字节对齐 */
        if (   (1 != swAlignment)
            && (2 != swAlignment)
            && (4 != swAlignment)
            && (8 != swAlignment))
        {
            return XA_COM_CUSTOM_NULL_POINT_ERROR;
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
            break;
        case XA_MEMTYPE_OUTPUT:

            /* 记录分配内存信息 */
            pstMemoryCfg->pOutputBuff       = pvMemory;
            pstMemoryCfg->swOutputMemSize   = swSize;
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
VOS_INT32  XA_COMM_MemoryAlloc (xa_codec_func_t*           pfuncProcess,
                                          xa_codec_handle_t           pstApiObj,
                                          XA_COMM_MEMORY_CFG_STRU*  pstMemoryCfg)
{
    VOS_INT32   err_code;
    VOS_INT32   swTableMemSize;
    VOS_VOID*   pTableBuff;

    if(   (VOS_NULL == pfuncProcess)
       || (VOS_NULL == pstApiObj) )
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
    pstMemoryCfg->pTableBuff     = pTableBuff;
    pstMemoryCfg->swTableMemSize = swTableMemSize;

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

    err_code = XA_COMM_SetBlksMemory(pfuncProcess, pstApiObj, pstMemoryCfg);
    CODEC_XA_ERRCODE_CHECK(err_code);

    return VOS_OK;
}

/*****************************************************************************
函 数 名：XA_COMM_Init
功能描述：执行初始化命令
输    入：pfuncProcess      tensilica库入口函数指针
          pstApiObj         codec句柄
          pvInputBuff
          uwInputBuffSize   buff的长度
          pfuncReadFilecb   读取数据的接口函数
输    出：
返 回 值：XA_NO_ERROR 成功 ；否则失败
描    述:
*****************************************************************************/
VOS_INT32 XA_COMM_Init(xa_codec_func_t*   pfuncProcess,
                       xa_codec_handle_t  pstApiObj,
                       VOS_VOID*          pvInputBuff,
                       VOS_INT32          swInputSize,
                       VOS_VOID*          pvHeaderBuff,
                       VOS_INT32          swHeaderSize,
                       VOS_INT32         *pswLastConsumed)
{
    VOS_INT32  err_code;
    VOS_INT32  swInitDone       = 0;
    VOS_INT32  swBuffSize       = 0;            /* 缓冲区实际数据量 */
    VOS_INT32  swConsumeSize    = swInputSize;  /* 每次解码后input使用的数据 */
    VOS_INT32  swLoopCnt;

    //入参数检查
    if(   (VOS_NULL == pfuncProcess)
       || (VOS_NULL == pstApiObj)
       || (VOS_NULL == pvInputBuff)
       || (VOS_NULL == pvHeaderBuff) )
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }

    swBuffSize              = swInputSize;
    *pswLastConsumed        = 0;
    swLoopCnt               = XA_COMM_MAX_INIT_CNT;

    //循环直到初始化完成或者失败
    do{
        /* 循环次数保护 */
        swLoopCnt--;
        if(swLoopCnt <= 0)
        {
            return (VOS_INT32)XA_FATAL_ERROR;
        }

        /* 移动inputbuff */
        XA_COMM_ShiftBuff((VOS_CHAR*)pvInputBuff, swInputSize, swConsumeSize);

        /* 从头文件中读取数据到inputbuffer*/
        if(swHeaderSize >= swConsumeSize)
        {
            UCOM_MemCpy(&(((VOS_CHAR *)pvInputBuff)[swInputSize - swConsumeSize]), pvHeaderBuff, (VOS_UINT32)swConsumeSize);
            swHeaderSize -= swConsumeSize;
        }
        else
        /* 若头文件已经完全读取完毕则发送INPUT_OVER命令，表示读取的数据结束 */
        {
            /*发送命令表示数据准备好了*/
            err_code = (*pfuncProcess)(pstApiObj,
                                       XA_API_CMD_INPUT_OVER,
                                       0,
                                       NULL);
            CODEC_XA_ERRCODE_CHECK(err_code);
        }

        /*设置要处理的数据长度*/
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_SET_INPUT_BYTES,
                                   0,
                                   &swBuffSize);
        CODEC_XA_ERRCODE_CHECK(err_code);

        /*发送初始化命令*/
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_INIT,
                                   XA_CMD_TYPE_INIT_PROCESS,
                                   NULL);
        CODEC_XA_ERRCODE_CHECK(err_code);

        /*查询初始化是否结束*/
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_INIT,
                                   XA_CMD_TYPE_INIT_DONE_QUERY,
                                   &swInitDone);
        CODEC_XA_ERRCODE_CHECK(err_code);

        /*查询消耗掉多少输入*/
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_GET_CURIDX_INPUT_BUF,
                                   0,
                                   &swConsumeSize);
        CODEC_XA_ERRCODE_CHECK(err_code);

        /* 更新头信息指针 */
        pvHeaderBuff        = (VOS_VOID *)((VOS_CHAR *)pvHeaderBuff + swConsumeSize);
        (*pswLastConsumed) += swConsumeSize;
    }while(!swInitDone);

    return err_code;
}

/*****************************************************************************
函 数 名：XA_COMM_Decode
功能描述：解码一帧数据
输    入：pfuncProcess      tensilica库入口函数指针
          pstApiObj         codec句柄
          pswInputSize      输入数据大小
输    出：puwOutputSize     输出数据大小
          pswConsumeSize    消耗数据大小
          puwExecuteDone    是否到达文件尾
返 回 值：XA_NO_ERROR 成功; 否则失败
描    述:
*****************************************************************************/
VOS_INT32 XA_COMM_Decode(xa_codec_func_t*  pfuncProcess,
                         xa_codec_handle_t pstApiObj,
                         VOS_INT32         swInputSize,
                         VOS_UINT32        enIsEndOfStream,
                         VOS_INT32*        pswOutputSize,
                         VOS_INT32*        pswConsumeSize,
                         VOS_INT32*        pswDecodeDone)
{
    VOS_INT32 err_code;

    //入参检查
    if(   (VOS_NULL == pfuncProcess)
       || (VOS_NULL == pstApiObj)
       || (VOS_NULL == pswOutputSize)
       || (VOS_NULL == pswConsumeSize)
       || (VOS_NULL == pswDecodeDone) )
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }

    if (AUDIO_PLAYER_POS_END_OF_STEAM == enIsEndOfStream)
    {
        /* 发送命令表示数据准备完成*/
        err_code = (*pfuncProcess)(pstApiObj,
                                   XA_API_CMD_INPUT_OVER,
                                   0,
                                   NULL);
        CODEC_XA_ERRCODE_CHECK(err_code);
    }

    /* 设置待解码数据大小*/
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_SET_INPUT_BYTES,
                               0,
                               &swInputSize);
    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 解码 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_EXECUTE,
                               XA_CMD_TYPE_DO_EXECUTE,
                               NULL);
    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 查询解码是否完成 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_EXECUTE,
                               XA_CMD_TYPE_DONE_QUERY,
                               pswDecodeDone);
    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 查询解码后output长度*/
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_GET_OUTPUT_BYTES,
                               0,
                               pswOutputSize);
    CODEC_XA_ERRCODE_CHECK(err_code);

    /* 查询input使用数据长度 */
    err_code = (*pfuncProcess)(pstApiObj,
                               XA_API_CMD_GET_CURIDX_INPUT_BUF,
                               0,
                               pswConsumeSize);
    CODEC_XA_ERRCODE_CHECK(err_code);

    return err_code;
}

/*****************************************************************************
函 数 名：ShiftBuff
功能描述: 清除INPUTBUFF已消耗数据，剩余数据移动到buff首地址
输    入：pchBuff           buff地址
          uwBuffSize        buffer大小
          uwConsumeBytes    已消耗数据大小
输    出：无
返 回 值：无
描    述:
*****************************************************************************/

VOS_VOID XA_COMM_ShiftBuff(VOS_CHAR* pchBuff, VOS_INT32 swBuffSize, VOS_INT32 swConsumeBytes)
{
    //移动剩余数据到buff头
    VOS_INT32 swRemainBytes;
    VOS_INT32 swIndex;

    if( (VOS_NULL == pchBuff) || (swBuffSize<0) || (swConsumeBytes<0) || (swBuffSize < swConsumeBytes))
    {
        return;
    }

    swRemainBytes = swBuffSize - swConsumeBytes;

    /* 剩余数据移动到buff首地址 */
    for (swIndex=0; swIndex<swRemainBytes; swIndex++)
    {
        pchBuff[swIndex] = pchBuff[swIndex + swConsumeBytes];
    }

    /* 清除已消耗数据 */
    UCOM_MemSet(&pchBuff[swRemainBytes], 0, (VOS_UINT32)swConsumeBytes);
}


VOS_INT32 XA_COMM_MemFree(XA_COMM_MEMORY_CFG_STRU* pstMemoryCfg)
{
    /* 异常入参检查 */
    if (VOS_NULL == pstMemoryCfg)
    {
        return XA_COM_CUSTOM_NULL_POINT_ERROR;
    }

    /********************** 依次释放所分配的内存 **********************/

    /* API memory释放 */
    if (VOS_NULL != pstMemoryCfg->pApiStructBuff)
    {
        UCOM_MemFree(pstMemoryCfg->pApiStructBuff);
    }

    /* table memory释放 */
    if (VOS_NULL != pstMemoryCfg->pTableBuff)
    {
        UCOM_MemFree(pstMemoryCfg->pTableBuff);
    }

    /* Persist memory释放 */
    if (VOS_NULL != pstMemoryCfg->pPersistBuff)
    {
        UCOM_MemFree(pstMemoryCfg->pPersistBuff);
    }

    /* Scratch memory释放 */
    if (VOS_NULL != pstMemoryCfg->pScratchBuff)
    {
        UCOM_MemFree(pstMemoryCfg->pScratchBuff);
    }

    /* Input memory释放 */
    if (VOS_NULL != pstMemoryCfg->pInputBuff)
    {
        UCOM_MemFree(pstMemoryCfg->pInputBuff);
    }

    /* Output memory释放 */
    if (VOS_NULL != pstMemoryCfg->pOutputBuff)
    {
        UCOM_MemFree(pstMemoryCfg->pOutputBuff);
    }

    /* 赋空所有内存指针、内存大小 */
    UCOM_MemSet(pstMemoryCfg, 0, sizeof(XA_COMM_MEMORY_CFG_STRU));

    return VOS_OK;

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



