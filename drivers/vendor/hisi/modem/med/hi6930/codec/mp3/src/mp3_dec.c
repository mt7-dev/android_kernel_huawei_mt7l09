/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : mp3_dec.c
  版 本 号   : 初稿
  作    者   : twx144307
  生成日期   : 2012年12月20日
  最近修改   :
  功能描述   : mp3解码模块
  函数列表   :

  修改历史   :
  1.日    期   : 2012年8月2日
    作    者   : twx144307
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "mp3_dec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_CODEC_MP3_DEC_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* mp3解码库handle,在init中初始化赋值 */
xa_codec_handle_t                         g_pstMp3ApiObj;

/* mp3模块所涉及内存信息记录 */
XA_COMM_MEMORY_CFG_STRU                   g_stMp3MemCfg;

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
函 数 名：MP3_DECODER_SetParams
功能描述：设置MP3解码参数
输    入：VOS_UINT32    uwPcmSize           pcm位宽
输    出：无
返 回 值：XA_ERRORCODE                      错误码
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_INT32  MP3_DECODER_SetParams(VOS_INT32 swPcmWidthSize)
{
    VOS_INT32        swErrCode;
    VOS_INT32        swCrcCheckFlg    = MP3_DEC_CRC_CHECK_DISABLE;
    xa_codec_func_t *pstMp3ProcessApi = xa_mp3_dec;

    /* 设置PCM位宽 */
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_SET_CONFIG_PARAM,
                          XA_MP3DEC_CONFIG_PARAM_PCM_WDSZ,
                          &swPcmWidthSize);

    CODEC_XA_ERRCODE_CHECK(swErrCode);

    /* 设置CRC校验不使能 */
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_SET_CONFIG_PARAM,
                          XA_MP3DEC_CONFIG_PARAM_CRC_CHECK,
                          &swCrcCheckFlg);

    CODEC_XA_ERRCODE_CHECK(swErrCode);

    return VOS_OK;
}

/*****************************************************************************
函 数 名：MP3_DECODER_GetParams
功能描述：获取MP3解码参数
输    入：无
输    出：pstConfig       解码后ＰＣＭ的信息结构体
返 回 值：VOS_OK 成功 ；VOS_ERR 失败
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  MP3_DECODER_GetParams(AUDIO_DEC_PARAMS_STRU* pstConfig)
{
    VOS_INT32 swErrCode;
    xa_codec_func_t *pstMp3ProcessApi = xa_mp3_dec;

    if(VOS_NULL == pstConfig)
    {
        return VOS_ERR;
    }

    /* 获取比特率 */
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_GET_CONFIG_PARAM,
                          XA_MP3DEC_CONFIG_PARAM_BITRATE,
                          &(pstConfig->swBitRate));
    XA_ERROR_HANDLE(swErrCode);


    /* 获取采样率 */
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_GET_CONFIG_PARAM,
                          XA_MP3DEC_CONFIG_PARAM_SAMP_FREQ,
                          &pstConfig->swSampleRate);
    XA_ERROR_HANDLE(swErrCode);

    /* 获取声道数 */
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_GET_CONFIG_PARAM,
                          XA_MP3DEC_CONFIG_PARAM_NUM_CHANNELS,
                          &(pstConfig->swChannel));
    XA_ERROR_HANDLE(swErrCode);

    /* 获取PCM位宽 */
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                           XA_API_CMD_GET_CONFIG_PARAM,
                           XA_MP3DEC_CONFIG_PARAM_PCM_WDSZ,
                           &pstConfig->swPcmWidthSize);
    XA_ERROR_HANDLE(swErrCode);

    return VOS_OK;
}

/*****************************************************************************
函 数 名：MP3_DECODER_Init
功能描述：MP3解码库初始化
输    入：VOS_UINT32 uwPcmSize  pcm位宽
输    出：无
返 回 值：0 成功 ；非0 失败
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  MP3_DECODER_Init(XA_AUDIO_DEC_USER_CFG_STRU *pstMp3UserCfg)
{
    VOS_INT32                       swErrCode;
    xa_codec_func_t                *pstMp3ProcessApi;
    XA_COMM_MEMORY_CFG_STRU        *pstMp3MemCfg       = MP3_DEC_GetMemCfgPtr();
    VOS_VOID                       *pApBufAddr;
    VOS_INT32                       swHeaderMemSize;
    VOS_UINT32                      uwPcmWidthSize;

    /* 异常入参检查 */
    if (   (VOS_NULL == pstMp3UserCfg)
        || (VOS_NULL == pstMp3UserCfg->pvHeadBuff))
    {
        return VOS_ERR;
    }
    /* 获取配置信息 */
    pApBufAddr          = pstMp3UserCfg->pvHeadBuff;
    swHeaderMemSize     = pstMp3UserCfg->swHeaderMemSize;
    uwPcmWidthSize      = pstMp3UserCfg->uwPcmWidthSize;

    pstMp3MemCfg->swHeaderMemSize = swHeaderMemSize;
    pstMp3MemCfg->pHeaderBuff     = pApBufAddr;

    pstMp3ProcessApi = xa_mp3_dec;
    /* tensilica 库解码 Startup 阶段*/
    swErrCode = XA_COMM_Startup(pstMp3ProcessApi,
                               &g_pstMp3ApiObj,
                               pstMp3MemCfg);
    XA_ERROR_HANDLE(swErrCode);

    /* 设置MP3解码参数,主要是位宽 */
    swErrCode = MP3_DECODER_SetParams((VOS_INT32)uwPcmWidthSize);
    XA_ERROR_HANDLE(swErrCode);

    /* tensilica库解码MemoryAlloction阶段 */
    swErrCode = XA_COMM_MemoryAlloc(pstMp3ProcessApi,
                                   g_pstMp3ApiObj,
                                   pstMp3MemCfg);
    XA_ERROR_HANDLE(swErrCode);

    /* tensilica解码库执行初始化命令阶段，读取文件头完成初始化 */
    swErrCode = XA_COMM_Init(pstMp3ProcessApi,
                            g_pstMp3ApiObj,
                            pstMp3MemCfg->pInputBuff,
                            pstMp3MemCfg->swInputMemSize,
                            pstMp3MemCfg->pHeaderBuff,
                            pstMp3MemCfg->swHeaderMemSize,
                           &pstMp3UserCfg->swConsumed);
    XA_ERROR_HANDLE(swErrCode);

    pstMp3UserCfg->swInSize = pstMp3MemCfg->swInputMemSize;

    return VOS_OK;
}



/*****************************************************************************
函 数 名：MP3_DECODER_Decode
功能描述：一帧数据解码
输    入：uwInputSize       输入数据大小
输    出：puwOuputSize      输出数据大小
          puwConsumeSize    消耗数据大小
          uwDecoderState    CODEC状态码
返 回 值：VOS_OK 成功; VOS_ERR 失败
调用函数  :
被调函数  :
说    明  : 需要通过USER层从AUDIO PLAYER取的数据:
            stMP3UserCfg.enIsEndOfStream // End of Stream;
            stMP3UserCfg.uwPcmWidthSize  // 位宽
            stMP3UserCfg.pvInBuff        // 输入BUFF的首地址
            stMP3UserCfg.swInBuffSize    // 输入BUFF有效字节数
            stMP3UserCfg.pvOutBuff;      // 输出指针位置(拷贝目标)

            启动解码操作，在解码操作中，应该配置完成下面项:
            stMP3UserCfg.swConsumed;     // 此次解码消耗的BUFF大小
            stMP3UserCfg.swOutputSize;   // 解码后输出数据的总大小
            stMP3UserCfg.uwDecoderState; // 解码状态

 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 MP3_DECODER_Decode(XA_AUDIO_DEC_USER_CFG_STRU *pstUserCfg)
{
    VOS_INT32                   swErrCode;
    VOS_INT32                   swDecodeFinish          = CODEC_DECODE_UNFINISH_FLAG;
    xa_codec_func_t             *pstMp3ProcessApi       = xa_mp3_dec;
    XA_COMM_MEMORY_CFG_STRU     *pstMemCfg              = MP3_DEC_GetMemCfgPtr();

    /* for input */
    VOS_INT32                   swInputSize     = 0;
    VOS_UINT32                  uwPcmWidthSize  = 0;
    VOS_UINT32                  enIsEndOfStream = 0;

    /* for output */
    VOS_INT32                    swOutputSize           = 0;
    VOS_INT32                    swConsumeSize          = 0;
    VOS_UINT32                   uwDecoderState         = CODEC_STATE_INITIALIZED;

    /* 入参检查 */
    if(VOS_NULL == pstUserCfg)
    {
        return VOS_ERR;
    }

    swInputSize     = pstUserCfg->swInSize;
    uwPcmWidthSize  = pstUserCfg->uwPcmWidthSize;
    enIsEndOfStream = pstUserCfg->enIsEndOfStream;

    if (16 != uwPcmWidthSize)
    {
        return VOS_ERR;
    }

    /* for input */
    UCOM_MemCpy(pstMemCfg->pInputBuff, pstUserCfg->pvInBuff, (VOS_UINT32)pstUserCfg->swInSize);


    swErrCode = XA_COMM_Decode(pstMp3ProcessApi,
                              g_pstMp3ApiObj,
                              swInputSize,
                              enIsEndOfStream,
                              &swOutputSize,
                              &swConsumeSize,
                              &swDecodeFinish);
    XA_ERROR_HANDLE_WITH_STATUS_CODE(swErrCode, &uwDecoderState);

    /* 判断是否解码完毕 */
    if(CODEC_DECODE_FINISH_FLAG == swDecodeFinish)
    {
        uwDecoderState = CODEC_STATE_FINISH;
    }

    /* 配置输出buff */
    UCOM_MemCpy(pstUserCfg->pvOutBuff, pstMemCfg->pOutputBuff, (VOS_UINT32)swOutputSize);
    pstUserCfg->swConsumed     = swConsumeSize;
    pstUserCfg->uwDecoderState = uwDecoderState;
    pstUserCfg->swOutputSize   = swOutputSize;

    return VOS_OK;
}

/*****************************************************************************
函 数 名：MP3_DECODER_Seek
功能描述：seek操作
输    入：VOS_UINT32                uwSkipSize                 //跳过的字节数
          CODEC_SEEK_DERECT_ENUM    enSeekDirect               //方向
输    出：
返 回 值：VOS_OK 成功 ；VOS_ERR 失败
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  MP3_DECODER_Seek(VOS_UINT32 uwSkipSize, CODEC_SEEK_DERECT_ENUM_UINT16 enSeekDirect)
{
    VOS_INT32    swErrCode;
    VOS_UINT32   uwCurrStreamPos;
    VOS_UINT32   uwNewStreamPos;
    xa_codec_func_t *pstMp3ProcessApi = xa_mp3_dec;

    //查询当前播放位置
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_GET_CONFIG_PARAM,
                          XA_CONFIG_PARAM_CUR_INPUT_STREAM_POS,
                          &uwCurrStreamPos);
    XA_ERROR_HANDLE(swErrCode);

    //计算要移动到的播放位置
    if(CODEC_SEEK_FORWARDS == enSeekDirect)
    {
        //检查新位置是否超过了文件大小，需要知道文件大小
        uwNewStreamPos = uwCurrStreamPos + uwSkipSize;
    }
    else
    {
        if(uwSkipSize > uwCurrStreamPos)
        {
            return VOS_ERR;
        }
        uwNewStreamPos = uwCurrStreamPos - uwSkipSize;
    }

    //设置新的播放位置
    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_SET_CONFIG_PARAM,
                          XA_CONFIG_PARAM_CUR_INPUT_STREAM_POS,
                          &uwNewStreamPos);
    XA_ERROR_HANDLE(swErrCode);

    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_EXECUTE,
                          XA_CMD_TYPE_DO_RUNTIME_INIT,
                          NULL);
    XA_ERROR_HANDLE(swErrCode);

    return VOS_OK;
}

/*****************************************************************************
函 数 名：MP3_DECODER_Close
功能描述：关闭库
输    入：无
输    出：无
返 回 值：无
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月19日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID MP3_DECODER_Close(VOS_VOID)
{
    XA_COMM_MEMORY_CFG_STRU            *pstMp3MemCfg    = MP3_DEC_GetMemCfgPtr();

    /* 释放所有动态申请空间 */
    XA_COMM_MemFree(pstMp3MemCfg);

    /* 赋空APIObj */
    g_pstMp3ApiObj = VOS_NULL;

    return;
}

/*****************************************************************************
函 数 名：MP3_DECODER_GetCurrDecPos
功能描述：查询当前解码进度，截止上一帧，已经解码的字节数
输    出：VOS_UINT32 *puwPosInBytes     已经解码字节数
返 回 值：VOS_OK 成功:VOS_ERR 失败
调用函数  :
被调函数  :


 修改历史      :
  1.日    期   : 2012年12月31日
    作    者   : twx144307
    修改内容   : 新生成函数
*****************************************************************************/
VOS_INT32 MP3_DECODER_GetCurrDecPos(VOS_UINT32 *puwPosInBytes)
{
    VOS_INT32 swErrCode;
    xa_codec_func_t *pstMp3ProcessApi = xa_mp3_dec;

    swErrCode = (*pstMp3ProcessApi)(g_pstMp3ApiObj,
                          XA_API_CMD_GET_CONFIG_PARAM,
                          XA_CONFIG_PARAM_CUR_INPUT_STREAM_POS,
                          puwPosInBytes);
    return swErrCode;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif





