

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "xa_aac_dec.h"
#include "ucom_mem_dyn.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                      OM_FILE_ID_XA_AAC_DEC_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
XA_COMM_MEMORY_CFG_STRU                   g_stAacDecMemCfg;       /* AAC解码内存配置管理 */
AAC_DEC_CONFIG_PARAMS_STRU                g_stAacDecParamCfg;     /* AAC解码参数配置管理 */
AAC_DEC_PARAMS_STRU                       g_stAacParams;          /* AAC编解码参数 */

/* AAC解码库API指针 */
xa_codec_handle_t                         g_pstAacApiObj;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32  AAC_DECODER_SetParams(AAC_DEC_CONFIG_PARAMS_STRU *pstParamCfg)
{
    VOS_INT32            swErrCode;
    xa_codec_func_t     *pfuncAacProcApi = xa_aac_dec;
    xa_codec_handle_t    pstAacApiObj    = XA_AACDEC_GetApiObjPtr();

    /* 0、入参判断 */
    if (VOS_NULL == pstParamCfg)
    {
        return VOS_ERR;
    }

    /* 1、配置是否输出固定配置为立体声 */
    swErrCode = (*pfuncAacProcApi)(pstAacApiObj,
                                    XA_API_CMD_SET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_TO_STEREO,
                                    (VOS_VOID *)&(pstParamCfg->swForceOutputStereo));
    /* 检查uwErrCode返回值 */
    XA_ERROR_HANDLE(swErrCode);

    /* 2、配置PCM位宽 */
    swErrCode = (*pfuncAacProcApi)(pstAacApiObj,
                                    XA_API_CMD_SET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_PCM_WDSZ,
                                    (VOS_VOID *)&(pstParamCfg->swPcmWidthSize));
    /* 检查uwErrCode返回值 */
    XA_ERROR_HANDLE(swErrCode);

    /* 3、配置最大输出声道数 */
    /*swErrCode = (*pfuncAacProcApi)(pstAacApiObj,
                                    XA_API_CMD_SET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_OUTNCHANS,
                                    (VOS_VOID *)&(pstParamCfg->swMaxOutChanNum));*/
    /* 检查uwErrCode返回值 */
    //XA_ERROR_HANDLE(swErrCode);

    return VOS_OK;
}
VOS_UINT32  AAC_DECODER_GetParams(AAC_DEC_PARAMS_STRU *pstConfig)
{
    VOS_INT32            swErrCode;
    xa_codec_func_t     *pfuncAacProcApi  = xa_aac_dec;
    xa_codec_handle_t    pstAacApiObj     = XA_AACDEC_GetApiObjPtr();

    /* 0、入参判断 */
    if (VOS_NULL == pstConfig)
    {
        return VOS_ERR;
    }

    /* 1、调用XA函数，分别获取:*/
    /* 编码AAC格式 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_AAC_FORMAT,
                                   (void *)&(pstConfig->enAacFormat));

    XA_ERROR_HANDLE(swErrCode);

    /* 声道数 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_NUM_CHANNELS,
                                   (VOS_VOID *)&(pstConfig->swNumChannels));

    XA_ERROR_HANDLE(swErrCode);

    /* 立体声模式 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_ACMOD,
                                   (VOS_VOID *)&(pstConfig->enAcmod));

    XA_ERROR_HANDLE(swErrCode);

    /* 采样率 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_SAMP_FREQ,
                                   (VOS_VOID *)&(pstConfig->swSampFreq));

    XA_ERROR_HANDLE(swErrCode);

    /* SBR类型 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_SBR_TYPE,
                                   (VOS_VOID *)&(pstConfig->swSbrType));

    XA_ERROR_HANDLE(swErrCode);

    /* AAC编码速率 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_AAC_SAMPLERATE,
                                   (VOS_VOID *)&(pstConfig->swAacRate));

    XA_ERROR_HANDLE(swErrCode);

    /* 通道映射表 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_CHANMAP,
                                   (VOS_VOID *)&(pstConfig->swChanMap));

    XA_ERROR_HANDLE(swErrCode);

    /* 解码允许的最大声道数 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_AACDEC_CONFIG_PARAM_OUTNCHANS,
                                   (VOS_VOID *)&(pstConfig->swOutNChans));

    XA_ERROR_HANDLE(swErrCode);

    /* 2、通过SBR信息，获取AAC LIB FORMAT */
    if (XA_AACDEC_SBR_TYPE_2 == pstConfig->swSbrType)
    {
        pstConfig->enAacLibFormat = XA_AACDEC_EBITSTREAM_FORMAT_AAC_PLUS_V2;
    }
    else if (XA_AACDEC_SBR_TYPE_1 == pstConfig->swSbrType)
    {
        pstConfig->enAacLibFormat = XA_AACDEC_EBITSTREAM_FORMAT_AAC_PLUS;
    }
    else
    {
        pstConfig->enAacLibFormat = XA_AACDEC_EBITSTREAM_FORMAT_PLAIN_AAC;
    }

    /* 3、当前输出只支持2声道，需要强行修改CHANNEL MAP的值 */
    //AAC_DECODER_SetOutChanFixedTo2(&pstConfig->swChanMap);

    return VOS_OK;
}



VOS_UINT32  AAC_DECODER_Init(XA_AUDIO_DEC_USER_CFG_STRU *pstUserCfg)
{
    VOS_UINT32                  uwErrCode;
    VOS_INT32                   swErrCode;
    xa_codec_func_t            *pfuncAacProcApi  = xa_aac_dec;                  /* AAC 解码进程管理API */
    AAC_DEC_CONFIG_PARAMS_STRU *pstParamCfg      = XA_AACDEC_GetParaCfgPtr(); /* AAC 解码参数配置管理 */
    xa_codec_handle_t           pstAacApiObj;

    XA_COMM_MEMORY_CFG_STRU    *pstMemCfg        = XA_AACDEC_GetMemCfgPtr();

    /* 读取pstUserCfg的设置: 位宽，文件头指针，文件头长度 */
    VOS_INT32                   swPcmWidthSize;

    /* 1、判断入参的合法性 */
    if (VOS_NULL == pstUserCfg)
    {
        return VOS_ERR;
    }

    /* 读取pstUserCfg的设置: 位宽，文件头指针，文件头长度 */
    swPcmWidthSize              = (VOS_INT32)pstUserCfg->uwPcmWidthSize;
    pstMemCfg->pHeaderBuff      = pstUserCfg->pvHeadBuff;
    pstMemCfg->swHeaderMemSize  = pstUserCfg->swHeaderMemSize;

    /* 2、调用函数XA_COMM_Startup，对API指针指向的API对象进行初始化 */
    swErrCode = XA_COMM_Startup(pfuncAacProcApi, &(XA_AACDEC_GetApiObjPtr()), pstMemCfg);

    /* 3、检查函数XA_COMM_Startup返回值是否异常，调用异常检查函数XA_ERROR_HANDLE进行检查。*/
    XA_ERROR_HANDLE(swErrCode);

    pstAacApiObj     = XA_AACDEC_GetApiObjPtr();

    /*4.1 初始化结构体，注:AAC解码约束如下：
    --如果输入为单声道，将输出固定复制为立体声；
    --输出最大通道数固定为2；
    --PCM宽度固定为16bit；
    --只支持ADTS和ADIF格式的AAC码流*/

    /*配置是否复制单声道为立体声 */
    pstParamCfg->swForceOutputStereo       = XA_AACDEC_SET_MONO2STEREO_ENABLE;
    /* 配置PCM位宽 */
    pstParamCfg->swPcmWidthSize            = swPcmWidthSize;
    /* 配置最大输出通道数 */
    pstParamCfg->swMaxOutChanNum           = XA_AACDEC_SET_MAXOUTCHAN_2;

    /* 4.2 调用函数AAC_DECODER_SetParams(pstParamCfg)，进行参数设置；*/
    uwErrCode = AAC_DECODER_SetParams(pstParamCfg);

    /* 5、检查函数AAC_DECODER_SetParams返回值是否异常 */
    if (VOS_OK != uwErrCode)
    {
        return VOS_ERR;
    }

    /* 6、调用函数XA_COMM_InitMemoryAlloc()，进行指针绑定、内存申请、空间分配和对齐操作；*/
    swErrCode = XA_COMM_MemoryAlloc(pfuncAacProcApi,
                                    pstAacApiObj,
                                    pstMemCfg);

    /* 7、检查函数XA_COMM_InitMemoryAlloc返回值是否异常，调用异常检查函数XA_ERROR_HANDLE进行检查；*/
    XA_ERROR_HANDLE(swErrCode);

    /* 8、tensilica解码库执行初始化命令阶段，读取文件头完成初始化 */
    swErrCode = XA_COMM_Init(pfuncAacProcApi,
                            pstAacApiObj,
                            pstMemCfg->pInputBuff,
                            pstMemCfg->swInputMemSize,
                            pstMemCfg->pHeaderBuff,
                            pstMemCfg->swHeaderMemSize,
                           &pstUserCfg->swConsumed);

    XA_ERROR_HANDLE(swErrCode);

    pstUserCfg->swInSize = pstMemCfg->swInputMemSize;

    return VOS_OK;
}



VOS_UINT32 AAC_DECODER_Decode(XA_AUDIO_DEC_USER_CFG_STRU *pstUserCfg)
{
    VOS_INT32                  swErrCode;
    VOS_INT32                  swDecodeFinish   = CODEC_DECODE_UNFINISH_FLAG;
    xa_codec_func_t           *pfuncAacProcApi  = xa_aac_dec;
    xa_codec_handle_t          pstAacApiObj     = XA_AACDEC_GetApiObjPtr();
    XA_COMM_MEMORY_CFG_STRU   *pstAacMemCfg     = XA_AACDEC_GetMemCfgPtr();

    /* for input */
    VOS_INT32                  swInputSize;
    VOS_UINT32                 uwPcmWidthSize;
    VOS_UINT32                 enIsEndOfStream;

    /* for output */
    VOS_INT32                  swConsumeSize   = 0;
    VOS_UINT32                 uwDecoderState  = CODEC_STATE_INITIALIZED;

    /* 1、入参检查 */
    if(VOS_NULL == pstUserCfg)
    {
        return VOS_ERR;
    }

    swInputSize                 = pstUserCfg->swInSize;
    uwPcmWidthSize              = pstUserCfg->uwPcmWidthSize;
    enIsEndOfStream             = pstUserCfg->enIsEndOfStream;


    if (XA_AACDEC_SET_PCMWIDTH_16 != uwPcmWidthSize)
    {
        return VOS_ERR;
    }

    /* for input */
    UCOM_MemCpy(pstAacMemCfg->pInputBuff, pstUserCfg->pvInBuff, (VOS_UINT32)pstAacMemCfg->swInputMemSize);

    /* 2、调用XA函数，对AAC进行解码 */
    swErrCode = XA_COMM_Decode(pfuncAacProcApi,
                               pstAacApiObj,
                               swInputSize,
                               enIsEndOfStream,
                               &pstAacMemCfg->swOutputMemSize,
                               &swConsumeSize,
                               &swDecodeFinish);

    /* 3、错误检查 */
    XA_ERROR_HANDLE_WITH_STATUS_CODE(swErrCode, &uwDecoderState);

    /* 4、判断是否编码完毕 */
    if(CODEC_DECODE_FINISH_FLAG == swDecodeFinish)
    {
        uwDecoderState = CODEC_STATE_FINISH;
    }

    /* 5、输出BUFF的码流转换 */
    /* 根据
       pstAacDecParams->swChanMap;       通道映射表
       pstAacDecParams->swOutNChans;     解码允许的最大声道数
       的信息，进行逐个声道循环写入数据，不写unused channels
       参照DEMO中的PCM_write函数进行码流转换。只需要支持16bit位宽 */
    if (VOS_OK != AAC_DECODER_PcmDataConvert(pstAacMemCfg->pOutputBuff,
                                           XA_AACDEC_SET_PCMWIDTH_16,
                                            pstAacMemCfg->swOutputMemSize,
                                           (VOS_INT16 *)pstUserCfg->pvOutBuff,
                                           &(pstUserCfg->swOutputSize)))
    {
        return VOS_ERR;
    }

    /* 6、将处理完的数据交给pstUserCfg */
    pstUserCfg->swConsumed     = swConsumeSize;
    pstUserCfg->uwDecoderState = uwDecoderState;

    return VOS_OK;
}
VOS_UINT32  AAC_DECODER_Seek(
                VOS_UINT32                    uwSkipSize,
                CODEC_SEEK_DERECT_ENUM_UINT16 enSeekDirect)
{
    VOS_INT32    swErrCode;
    VOS_UINT32   uwCurrStreamPos;
    VOS_UINT32   uwNewStreamPos;
    xa_codec_func_t *pstAacProcessApi = xa_aac_dec;

    /* 查询当前播放位置 */
    swErrCode = (*pstAacProcessApi)(g_pstAacApiObj,
                          XA_API_CMD_GET_CONFIG_PARAM,
                          XA_CONFIG_PARAM_CUR_INPUT_STREAM_POS,
                          &uwCurrStreamPos);
    XA_ERROR_HANDLE(swErrCode);

    /* 计算要移动到的播放位置 */
    if(CODEC_SEEK_FORWARDS == enSeekDirect)
    {
        /* 检查新位置是否超过了文件大小，需要知道文件大小 */
        uwNewStreamPos = uwCurrStreamPos + uwSkipSize;
    }
    else if (CODEC_SEEK_BACKWARDS == enSeekDirect)
    {
        if(uwSkipSize > uwCurrStreamPos)
        {
            return VOS_ERR;
        }
        uwNewStreamPos = uwCurrStreamPos - uwSkipSize;
    }
    else
    {
        return VOS_ERR;
    }

    /* 设置新的播放位置 */
    swErrCode = (*pstAacProcessApi)(g_pstAacApiObj,
                          XA_API_CMD_SET_CONFIG_PARAM,
                          XA_CONFIG_PARAM_CUR_INPUT_STREAM_POS,
                          &uwNewStreamPos);
    XA_ERROR_HANDLE(swErrCode);

    swErrCode = (*pstAacProcessApi)(g_pstAacApiObj,
                          XA_API_CMD_EXECUTE,
                          XA_CMD_TYPE_DO_RUNTIME_INIT,
                          NULL);
    XA_ERROR_HANDLE(swErrCode);

    return VOS_OK;
}


VOS_VOID AAC_DECODER_Close(VOS_VOID)
{
    XA_COMM_MEMORY_CFG_STRU    *pstMemCfg    = XA_AACDEC_GetMemCfgPtr();

    if (VOS_NULL != pstMemCfg)
    {
        XA_COMM_MemFree(pstMemCfg);
    }

    /* API对象指针的内存已经在上面释放，这里清零API对象指针，防止出现野指针 */
    XA_AACDEC_GetApiObjPtr()                 = VOS_NULL;
}
VOS_INT32 AAC_DECODER_GetCurrDecPos(VOS_UINT32 *puwPosInBytes)
{
    VOS_INT32            swErrCode;
    xa_codec_func_t     *pfuncAacProcApi  = xa_aac_dec;
    xa_codec_handle_t    pstAacApiObj     = XA_AACDEC_GetApiObjPtr();

    /* 1、调用库函数查询当前AAC播放进度 */
    swErrCode = (*pfuncAacProcApi )(pstAacApiObj,
                                    XA_API_CMD_GET_CONFIG_PARAM,
                                    XA_CONFIG_PARAM_CUR_INPUT_STREAM_POS,
                                    puwPosInBytes);

    /* 2、检查函数的返回值是否异常，调用异常检查函数XA_ERROR_HANDLE进行检查；*/
    XA_ERROR_HANDLE(swErrCode);

    return VOS_OK;
}


VOS_UINT32 AAC_DECODER_PcmDataConvert
                (VOS_INT32       *pswTimeCh,
                 VOS_INT32        swPcmSampleSize,
                 VOS_INT32        swOutputSize,       /* AUDIO_DEC_MEMORY_CFG_STRU中的临时输出Buff的长度 */
                 VOS_INT16       *pshwOutBuff,
                 VOS_INT32       *pswTotalOutSize)    /* 最终输入BUFF的长度 */
{
    AAC_DEC_PARAMS_STRU        *pstParaCfg  = XA_AACDEC_GetParamsPtr();
    VOS_INT32                   swI;
    VOS_INT32                   swCh;
    VOS_INT32                   swChmap;
    VOS_INT32                   swOutNChans;
    VOS_INT32                   swOutchmap;
    VOS_INT32                   swFrameSize;
    VOS_INT16                  *pshwTimeChShort;
    VOS_INT16                  *pshwTmpOut  = pshwOutBuff;

    AAC_DECODER_GetParams(pstParaCfg);
    swOutNChans                 = pstParaCfg->swOutNChans;
    swOutchmap                  = pstParaCfg->swChanMap;
    swFrameSize                 = (swOutputSize / 2) / swOutNChans;
    *pswTotalOutSize            = 0;

    if(swPcmSampleSize != XA_AACDEC_SET_PCMWIDTH_16)
    {
        return VOS_ERR;
    }
    else
    {
        pshwTimeChShort = (VOS_INT16 *)pswTimeCh;
        for(swI = 0; swI < swFrameSize; swI++)
        {
            swChmap = swOutchmap;
            for(swCh = 0 ; swCh < XA_AACDEC_MAXCHAN_NUMBER; swCh++)
            {
                /* Check if any channel data is present at this sample offset */
                if ((swChmap & 0x0f) == 0x0f)
                {
                    /* Don't write unused channels. */
                }
                else
                {
                    (*pshwTmpOut) = pshwTimeChShort[(swI * swOutNChans) + swCh];
                    pshwTmpOut++;
                    (*pswTotalOutSize) += (VOS_INT32)sizeof(VOS_INT16);
                }
                swChmap >>= XA_AACDEC_CHANNELMAP_SHIFT_SCALE;
            }
         }
    }

    return VOS_OK;
}

#if 0

VOS_VOID AAC_DECODER_SetOutChanFixedTo2(VOS_INT32 *pswChanMap)
{
    VOS_INT32       swChanMap = *pswChanMap;

    /* 使用如下混音策略 */
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




