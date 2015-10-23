/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : xa_dm3.c
  版 本 号   : 初稿
  作    者   : twx144307
  生成日期   : 2012年12月20日
  最近修改   :
  功能描述   : dobly音效
  函数列表   :

  修改历史   :
  1.日    期   : 2012年8月2日
    作    者   : twx144307
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "xa_dm3plus_ak_pp_api.h"
#include "xa_comm.h"
#include "dm3.h"
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
#define THIS_FILE_ID                    OM_FILE_ID_CODEC_DM3_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* mp3解码库handle,在init中初始化赋值 */
xa_codec_handle_t                g_pstDm3ApiObj;

/* DM3+音效库初始化状态 */
VOS_UINT32                       g_uwDm3InitStatus;

/* hifi下电后，由于全局变量在DDR中，数据能被保存 */
XA_AUDIO_EFFECT_USER_CFG_STRU    g_stAudioEffectUserCfg;     /* 音效配置全局变量 */


XA_COMM_MEMORY_CFG_STRU          g_stDm3MemoryCfg;           /* 内存管理 */
XA_COMM_MEMORY_CFG_STRU          g_stDtsMemoryCfg;           /* 内存管理 */

AUDIO_EFFECT_DOBLY_PARAMS_STRU   g_stDm3HeadphoneParams;     /* dolby headphone音效配置全局数据,不能初始化 */
AUDIO_EFFECT_DOBLY_PARAMS_STRU   g_stDm3SpkParams;           /* dolby spk音效配置全局数据,不能初始化 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 AUDIO_EFFECT_Execute(XA_AUDIO_EFFECT_USER_CFG_STRU* pstUserCfg)
{
    if (AUDIO_EFFECT_SWITCH_ON == pstUserCfg->enDm3Enable)
    {
        return(AUDIO_EFFECT_Dm3Execute(pstUserCfg));
    }

    /*if (AUDIO_EFFECT_SWITCH_ON == pstUserCfg->enDtsEnable)
    {...}*/

    if (    (AUDIO_EFFECT_SWITCH_ON != pstUserCfg->enDm3Enable)
         && (AUDIO_EFFECT_SWITCH_ON != pstUserCfg->enDtsEnable))
    {
        /* 透传操作 */
        UCOM_MemCpy(pstUserCfg->pvOutBuff, pstUserCfg->pvInBuff, (VOS_UINT32)pstUserCfg->swInputSize);
    }
    return UCOM_RET_SUCC;
}
VOS_VOID AUDIO_EFFECT_Dm3Close(VOS_VOID)
{
    XA_COMM_MEMORY_CFG_STRU      *pstDm3MemCfg  = AUDIO_EFFECT_GetDm3MemCfgPtr();   /* 内存管理 */

    if(VOS_NULL != pstDm3MemCfg)
    {
        XA_COMM_MemFree(pstDm3MemCfg);
    }

    AUDIO_EFFECT_SetDm3InitStatus(AUDIO_EFFECT_PARA_UNINITIALIZED);
}
VOS_UINT32  AUDIO_EFFECT_Dm3Init(XA_AUDIO_EFFECT_USER_CFG_STRU* pstUserCfg)
{
    VOS_UINT32                      uwBlockBytes;                               /* 需要申请内存块的大小 */
    VOS_INT32                       swErrCode;
    AUDIO_EFFECT_DOBLY_PARAMS_STRU *pstDoblyParams;
    xa_codec_handle_t               pstDm3ApiObj;
    XA_COMM_MEMORY_CFG_STRU        *pstDm3MemCfg         = AUDIO_EFFECT_GetDm3MemCfgPtr();   /* 内存管理 */
    xa_codec_func_t                *pstDm3plusProcessApi = xa_dm3plus_ak_pp;

    /* 这里不使用杜比音效中的变采样功能 */
    VOS_INT32                       swUpsampleRate       = AUDIO_EFFECT_DM3_IO_SAMP_EQU;   /* 输入输出采样率比恒定为1，即输入什么(合法的)采样率，输出什么采样率 */

    /* 1、入参非空由调用者保证 */

    if(  (pstUserCfg->uhwCurDevice  >= AUDIO_EFFECT_DEVICE_BUT)
       ||(pstUserCfg->uwSampleRate  >  AUDIO_EFFECT_MAX_SAMPLE_RATE)
       ||(   (pstUserCfg->uwChannel != AUDIO_EFFECT_DM3_CHANS_ORDER_C)
           &&(pstUserCfg->uwChannel != AUDIO_EFFECT_DM3_CHANS_ORDER_L_R)))
    {
        return UCOM_RET_FAIL;
    }

    if (AUDIO_EFFECT_PARA_UNINITIALIZED == AUDIO_EFFECT_GetDm3InitStatus())
    {
        /* 2、调用函数XA_COMM_Startup，对API指针指向的API对象进行初始化，在初始化的时候，函数XA_COMM_Startup会： */
        /******
        ①获取API对象需要空间的大小，并存到pstMemCfg->swApiObjSize中；
        ②使用UCOM_MemAlloc函数申请一块大小等同于pstMemCfg->swApiObjSize的内存，并确保该内存首地址4字节对齐；
        ③将该API对象初始化；
        ******/
        swErrCode = XA_COMM_Startup(pstDm3plusProcessApi,
                                    &(AUDIO_EFFECT_GetDm3ApiPtr()),
                                    pstDm3MemCfg);
        XA_ERROR_HANDLE(swErrCode);

        pstDm3ApiObj= AUDIO_EFFECT_GetDm3ApiPtr();

        /* 3、调用函数XA_COMM_InitMemoryAlloc()，进行指针绑定、内存分配和对齐操作；*/
        swErrCode = XA_COMM_MemoryAlloc(pstDm3plusProcessApi,
                                        pstDm3ApiObj,
                                        pstDm3MemCfg);
        XA_ERROR_HANDLE(swErrCode);

        /* 4、解码库初始化 */
        swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiObj,
                                     XA_API_CMD_INIT,
                                     XA_CMD_TYPE_INIT_PROCESS,
                                     NULL);
        XA_ERROR_HANDLE(swErrCode);

        /* 5、RUN TIME INIT */
        swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiObj,
                                     XA_API_CMD_EXECUTE,
                                     XA_CMD_TYPE_DO_RUNTIME_INIT,
                                     NULL);
        XA_ERROR_HANDLE(swErrCode);

    }
    else
    {
        pstDm3ApiObj= AUDIO_EFFECT_GetDm3ApiPtr();
    }

    if (VOS_NULL == pstDm3ApiObj)
    {
        return UCOM_RET_FAIL;
    }

    /* 6、根据当前设备类型选择配置项 */
    if(AUDIO_EFFECT_DEVICE_HEADPHONE == pstUserCfg->uhwCurDevice)
    {
        pstDoblyParams = AUDIO_EFFECT_GetDm3HeadphoneCfgPtr();
    }
    else
    {
        pstDoblyParams = AUDIO_EFFECT_GetDm3SpkCfgPtr();
    }


    /* 7、设置输入数据的采样率，并对API设置输入采样率 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_INP_SAMP_FREQ,
                                 (VOS_INT32 *)(&pstUserCfg->uwSampleRate));
    XA_ERROR_HANDLE(swErrCode);

    /* 8、设置输出数据的采样率，输入采样率和输出采样率一致 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_OUT_SAMP_FREQ,
                                 (VOS_INT32 *)(&pstUserCfg->uwSampleRate));
    XA_ERROR_HANDLE(swErrCode);

    /* 9、设置输入声道数 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_INP_NUM_CHANS,
                                 (VOS_INT32*)(&pstUserCfg->uwChannel));
    XA_ERROR_HANDLE(swErrCode);

    /* 10、设置输入输出采样率比值 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiObj,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_RESAMP_UPSAMPLE_RATE,
                                 &swUpsampleRate);
    XA_ERROR_HANDLE(swErrCode);

    /* 11、调用AUDIO_EFFECT_SetParams()对杜比音效参数进行初始化，要求此时AP已经告知杜比音效参数的配置(对全局变量进行了配置) */
    if (UCOM_RET_SUCC != AUDIO_EFFECT_Dm3SetPara(pstDoblyParams))
    {
        return UCOM_RET_FAIL;
    }

    /* 12、初始化inputSize，并根据该大小申请内存 */
    /* 12.1 计算I/O内存大小 */
    uwBlockBytes = (sizeof(VOS_INT16) * pstUserCfg->uwChannel) * DM3_N_SAMPLES;

    /* 12.2 申请内存 */
    /*pvInBuff     = UCOM_MemAlloc(uwBlockBytes);
    pvOutBuff    = UCOM_MemAlloc(uwBlockBytes);*/

    /* 12.3 赋值到内存管理结构体，并记录大小 */
    /*pstDm3MemCfg->pInputBuff      = pvInBuff;
    pstDm3MemCfg->pOutputBuff     = pvOutBuff;*/
    pstDm3MemCfg->swInputMemSize  = (VOS_INT32)uwBlockBytes;
    pstDm3MemCfg->swOutputMemSize = (VOS_INT32)uwBlockBytes;

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_EFFECT_Dm3SetPara(AUDIO_EFFECT_DOBLY_PARAMS_STRU* pstDoblyParams)
{
    VOS_INT32            swErrCode;
    xa_codec_func_t     *pstDm3plusProcessApi = xa_dm3plus_ak_pp;
    xa_codec_handle_t    pstDm3ApiPtr         = AUDIO_EFFECT_GetDm3ApiPtr();


    if(VOS_NULL == pstDoblyParams)
    {
        return UCOM_RET_FAIL;
    }

    /* 1. 设置输入矩阵(input matrix)的值，它的含义如下:
    Input matrix (imtx): Specifies the stereo input matrix config for the
    2-channel input signal. Possible values are
    0: stereo: Left and right input channels are treated normally.
    1: channel one mono: The right channel is replaced by the left input channel
    internally.
    2: channel two mono: The left channel is replaced by the right input channel
    internally.
    3: dual mono: Both channels are summed and replicated to both channels
    internally.
    4: Dolby Pro-Logic II Encoded stereo content: Left and right channel are
    treated as pro-Logic II encoded. This is only applicable for use with mobile
    surround.
    默认值为0 - 立体声    */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_IN_MATRIX,
                                 &pstDoblyParams->swInMatrix);
    XA_ERROR_HANDLE(swErrCode);

    /* 2. 这里可以设置输入信道排列格式，0为interleaved(交叉的)，1为deinterleaved
    (非交叉)格式。
    默认值为interleaved - 交叉。
    【说明】根据当前设计，DM3+输入的一定是interleaved格式 ，因此不做设定 */

    /* 3. 设置Bypass Channels Mode是否激活，当激活的时候，它的输出声道数等于输
    入声道数。默认值为0，即该模式不激活 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_BYPASS,
                                 &pstDoblyParams->swByPass);
    XA_ERROR_HANDLE(swErrCode);

    /* 4. 这里可以配置音频混缩功能(Down Mix)，它的含义如下:
    Downmix  config  (dmxc):  Specifies  the  downmixer  internal  channel
    config.  The valid range is 0 (downmix to shuffled L/R channels (2 channels))
    to 7(downmix to L, C, R, Ls, Rs, Lb, Rb (7 channels)). The default value is
    0 (shuffled L/R); This config parameter  is  provided  only  for  test
    purposes;  it  should  not  be  explicitly  set  by  the application.
    默认值为2
    【说明】音频混缩功能会控制输出声道数，Tensilica文档指出该参数仅用于测试目的，
    因此这里不做设置。*/

    /* 5. 配置低音声道混缩水平(LFE channel Downmix level)
    杜比音效输出有一个超低音声道，称为LFE channel，LFE是Low Frequency Enhancement的
    缩写。它的含义如下:
    LFE channel downmix level (lfel): Specifies the LFE downmix level ranging
    from value 0 (corresponds to +10dB) to value 31 (corresponds to -20 dB) in
    1 dB steps.
    默认值为10 (0 dB).    */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_LFE_MIX_LEVEL,
                                 &(pstDoblyParams->swLfeMixLevel));
    XA_ERROR_HANDLE(swErrCode);

    /* 6. 配置移动环绕声(Mobile Surround)最大声道数，它的含义是:
    Mobile Surround max profile (msmp): Controls the mobile surround profile.
    Four mobile surround profiles are supported:
    0  : stereo
    1  : 4 channel
    2  : 5 channel
    3  : 7 channel
    默认值为2，即五声道 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_MSR_MAX_PROFILE,
                                 &pstDoblyParams->swMsrMaxProfile);
    XA_ERROR_HANDLE(swErrCode);

    /* 7. 配置单声道转立体声功能是否打开，它的含义是:
     Mono-to-Stereo  enable  (tupe):  Enables/disables  the  mono  to  stereo
     converter module. Once enabled, this modules acts in following manner.
     Single channel input:  For single channel input, it produces stereo content
    from the input signal.
     Stereo channel input: If the input signal is two channel, Mono to Stereo
    Creator determines whether the Left and Right channel content is sufficiently
    different to be considered stereo and left untouched, or similar enough to
    require enhancing. The more similar the channel content, the more Mono to
    Stereo Creator enhances the content.
    Mono to  Stereo Creator does not  attempt to synthesize stereo content if the
    input file has Dolby Pro-Logic II Encoded stereo content or if the input file
    has more than two channels.
    默认该功能是不打开的 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_M2S_ENABLE,
                                 &pstDoblyParams->swM2sEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 8. 配置音场扩展功能(Sound  Space  Expander)，音场扩展可以产生一个宽广的
    立体音响空间，改善乐器的分离感，但不会降低混音中心的音质。它的说明如下:
    The  Sound  Space  Expander  widens  the  perceived  stereo field produced
    by a pair of speakers. This works by modeling a phantom Center  channel and
    two phantom side channels from the incoming stereo audio stream.
    The  Sound  Space Expander  and  Mobile Surround features are mutually
    exclusive; if one of the  features is enabled, the other must be disabled. */

    /* 8.1 配置音场扩展是否使能，默认使能关闭 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_ENABLE,
                                 &pstDoblyParams->swSSEEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 8.2 配置音场扩展的宽度:
    Sound  Space  Expander  width  (stww):  Controls  the  width  of  the  Sound
    Space Expander. If set to 0, it sets the expander in half width mode; if set
    to 1, it sets the expander in full width mode.
    默认值为1    */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_WIDTH,
                                 &pstDoblyParams->swSSEWidth);
    XA_ERROR_HANDLE(swErrCode);

    /* 8.3 配置音场扩展扬声器模式
     Sound  Space  Expander  speaker  mode(stwm):  This  parameter  controls  the
    following speaker modes:
    0 = Normal (default)
    1 = Narrow
    2 = Binaural
    Narrow speaker mode is intended for closely spaced speakers, such as those found
    in mobile devices. Binaural speaker mode is intended for processing audio that has
    been specifically pre-encoded for Mobile Surround processing, allowing such audio
    to be rendered via speakers. */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_SPK_MODE,
                                 &pstDoblyParams->swSSESpkMode);
    XA_ERROR_HANDLE(swErrCode);

    /* 9. 配置移动环绕声增混(Mobile Surround Upmixer)功能是否使能，它的含义是:
    The Mobile Surround Upmixer processing block is a highly efficient matrix
    decoder that  expands  a  stereo  signal  (including  one  that  has  been
    encoded with  Dolby Surround or Dolby Pro Logic?II) into 4- channel audio
    (Left, Right, Left Surround, and  Right  Surround).  It  is  optimized  for
    use  with  the  Mobile  Surround  headphone virtualizer  as  well  as  for
    small,  low  power  devices,  and  is  therefore  more computationally
    efficient than a typical matrix decoder application. For multichannel
    input, the mobile surround upmixer is internally disabled.
    - This block should be always enabled when mobile surround feature is enabled.
    By default, the Mobile Surround Upmixer feature is disabled.
    - Please  note  that,  this  config  parameter  is  provided  for  test
    purposes  only;  proper validation should be done before enabling/disabling
    it explicitly.
    默认该功能是关闭的。
    【说明】根据Tensilica的声明，该功能仅用于测试目的，因此这里不打开。 */

    /* 10. 配置移动环绕声( Mobile Surround )功能。它的含义如下:
    The   Mobile   Surround   processing   block   reproduces   surround   sound
    over headphones  by  virtualizing  multiple  channels  (Left,  Right,  Center,
    Left  Surround, Right Surround, Left Back, and Right Back). Mobile Surround
    works with either the Mobile Surround Upmixer or the multi-channel downmixer
    block output.
    Mobile Surround and Sound Space Expander are mutually exclusive;
    if one of the features is enabled, the other must be disabled.
    Mobile Surround also adds two effects.
    - Room Size: Creates the impression of listening to the audio in different
    sized rooms, from small to large.
    - Brightness: Adjusts the timbre of the audio, adding "brightness"(more
    high frequencies) or "warmth"(less high frequencies). */

    /* 10.1 配置移动环绕声功能是否使能，默认为不使能 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_ENABLE,
                                 &pstDoblyParams->swMsrEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 10.2 配置移动环绕声的Room Size:
    Controls the perceived size of a room and the  sense  of  envelopment.
    The  range  for  this  control  is 0 (small  room size)to 2 (large room size)
    默认值为1 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_ROOM,
                                 &pstDoblyParams->swMsrRoot);
    XA_ERROR_HANDLE(swErrCode);

    /* 10.3 配置移动环绕声的Brightness:
    Adjusts the timbre of the audio, adding "brightness"(more high frequencies)
    or "warmth"(less high frequencies).
    默认值为1 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_BRIGHT,
                                 &pstDoblyParams->swMsrBright);
    XA_ERROR_HANDLE(swErrCode);

    /* 11. 配置声音级别控制(SLC)控制功能
    The Sound Level Controller feature is a dynamic range compressor that suppresses
    Loud passages and boosts quiet passages. */

    /* 11.1 配置SLC是否使能
    Sound Level Ctrl enable (late): This configuration activates/deactivates the sound
    level control-processing block.
    默认声音级别控制(SLC)是关闭的 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_ENABLE,
                                 &pstDoblyParams->swSlcEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 11.2 配置SLC输入的级别
    This  parameter  is  set  to  specify  the  average
    input  level  for  the  expected  input  signal.  This  control  has  seven  settings,  ranging
    from 0 (loudest input level) to 6 (softest input level). This parameter, along with the
    sound  level  control  depth  parameter  tunes  the  sound  level  controller  in  following
    ways
    - It decides the input signal level at which the sound level control starts modifying
    the input signal
    - It decides the dynamic range of the sound level controller 's output.
    默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_LEVEL,
                                 &pstDoblyParams->swSlcLevel);
    XA_ERROR_HANDLE(swErrCode);

    /* 11.3 配置SLC的深度
    This parameter controls the amount of the Sound Level Controller effect,
    where the effect is the suppression of loud passages and the boosting  of
    quiet  passages.  This  control  has  three  settings,  ranging  from  0
    (least  effect) to 2 (most effect).
    默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_DEPTH,
                                 &pstDoblyParams->swSlcDepth);
    XA_ERROR_HANDLE(swErrCode);

    /* 12. 这里可以配置音量相关信息，默认不使用DM3+模块调整音量水平，因此这里
    不进行配置。
    【说明】根据设计，不使用DM3+模块调整音量。 */

    /* 13. 配置自然低音技术(Nature Bass Proc.)，让声音透过耳机或喇叭后更向下延伸
    低八度音，创造出层次更加丰富的频响，而不丢失高频分量
    Natural  bass  features  makes  listening  material  sound  more natural by
    enhancing the bass content without losing quality in higher frequencies. It
    also  improves  the  performance  of  headphones  or  speakers  in  the  low
    frequency range  by  enhancing  the  bass  response  in  a  manner  similar
    to  a  subwoofer crossover. */

    /* 13.1 配置Nature Bass使能项，该功能默认关闭 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_ENABLE,
                                 &pstDoblyParams->swNBEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 13.2 配置Nature Bass的角频(Corner Frequency)，它的含义:
    This  parameter  specifies  the  corner  frequencies  for natural bass. The
    corner frequencies can be set from a set of logarithmically spaced 24
    frequencies ranging from 41 Hz to 1000Hz. These corner frequencies are set
    so that  when  adjusted  to  the  roll  off  frequency  (-3 dB point)  of
    the  headphones  or speakers,  the  Natural  Bass  produces  a  flat  overall
    frequency  response  in  this region.
    默认值为0，即 41赫兹 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_CUTOFF,
                                 &pstDoblyParams->swNBCutoff);
    XA_ERROR_HANDLE(swErrCode);

    /* 13.3 配置自然低音增强(natural bass boost)系数:
    The Boost parameter indicates the amount of gain the Natural  Bass  applies
    to  the  low  frequency  enhancement  path  of  the  processing block.  The
    strength  required  depends  on  the  low  frequency  characteristics  of  the
    headphones or speakers, and the end user's listening preferences. There are seven
    boost  levels, ranging from 0 (minimum amount of gain) to 6 (maximum amount  of
    gain).
    默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_BOOST,
                                 &pstDoblyParams->swNBBoost);
    XA_ERROR_HANDLE(swErrCode);

    /* 13.3 配置natural bass level系数，它表示了耳机或扬声器最大输出能力，以避免
    大音量噪声的失真:
    Specifies  the  maximum  output  capability  of  the headphones  or  speakers
    to protect against distortion when generating audio at maximum volume.
    There are seven output values, ranging from 0 (maximum output capability) to
    6 (minimum output capability).
    默认值为0
    */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_LEVEL,
                                 &pstDoblyParams->swNBLevel);
    XA_ERROR_HANDLE(swErrCode);

    /* 14. 配置高音增强功能(High Frequency Enhancement, HFE)，具体:
    High  Frequency  Enhancer  synthesizes  missing  high  frequency components
    from  the  remaining  part  of  the  signal  spectrum,  restoring  crispness,
    edge,  and  clarity  to  the  audio.  This  feature  improves  high  frequency
    response on compressed audio signals and full band, uncompressed signals.
    另外，需要注意下面一点:
    When using High Frequency Enhancer in conjunction with the resampler (upsampler
    and downsampler), do not enable High Frequency Enhancer when the downsampler
    is in operation (where the sample rate multiplier is equal to 1 and the input
    rate is  less than 24 kHz).  */

    /* 14.1 配置高音增强功能是否使能，默认不使能 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_HFE_ENABLE,
                                 &pstDoblyParams->swHfeEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 14.2 配置高音增强功能的深度，具体:
    This config parameter lets the end user set  the  level  of  the  synthesized
    high  frequency  components  added  to  the  original  signal. The range for
    depth parameter is from 0 (lowest level of enhancement) to 6  (highest level
    of enhancement.)
    默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_HFE_DEPTH,
                                 &pstDoblyParams->swHfeDepth);
    XA_ERROR_HANDLE(swErrCode);

    /* 15. 这里可以配置音量衰减(fade)的相关信息，默认不使用DM3+模块的FADE功能，
    因此这里不进行配置。
    【说明】根据设计，不使用DM3+模块的FADE功能。 */

    /* 16. 配置图形均衡(Graphic EQ)功能，图形均衡功能用于加强或者削弱所选频段，
    以纠正或修改该频段的信号频谱，他的定义如下:
     graphic  equalizer-processing block. Both the number of frequency bands and
     the boost/cut level of each band can be adjusted at run time.*/

    /* 16.1 配置图形均衡使能，默认为不使能 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_ENABLE,
                                 &pstDoblyParams->swGeqEnable);
    XA_ERROR_HANDLE(swErrCode);

    /* 16.2 配置GEQ的MAX BOOST:
    Controls  the  maximum  amount  of  boost  in  increments of 0.25 dB,
    ranging from 0 (no boost) to +48 (12 dB, maximum boost).
    默认值为6dB，即24 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_MAXBOOST,
                                 &pstDoblyParams->swGeqMaxBoost);
    XA_ERROR_HANDLE(swErrCode);

    /* 16.3 配置GEQ的前置放大器(PREAMP)
    Controls  the  amount  of  internal  attenuation  that   Graphic  EQ  provides
    internally  across the entire spectrum before performing  level   adjustments.
    This setting ranges from 0 (no attenuation) to -48 (-12 dB, maximum   attenuation),
    in increments of 0.25 dB.
    默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_PREAMP,
                                 &pstDoblyParams->swGeqPreamp);
    XA_ERROR_HANDLE(swErrCode);

    /* 16.4 配置GEQ的频带数，默认值为2
    Controls the No of bands ranging from 2 to max 7.*/
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                XA_API_CMD_SET_CONFIG_PARAM,
                                XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_NBANDS,
                                &pstDoblyParams->swGeqNbands);
    XA_ERROR_HANDLE(swErrCode);

    /* 16.5 配置GEQ对频带1 - 频带7的增益，取值范围为[-48, +48]，分别对应的增益为
    [-12dB, +12dB]，每增加1，增加0.25dB，默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND1,
                                 &(pstDoblyParams->aswGeqBands[0]));
    XA_ERROR_HANDLE(swErrCode);
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND2,
                                 &(pstDoblyParams->aswGeqBands[1]));
    XA_ERROR_HANDLE(swErrCode);
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND3,
                                 &(pstDoblyParams->aswGeqBands[2]));
    XA_ERROR_HANDLE(swErrCode);
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND4,
                                 &(pstDoblyParams->aswGeqBands[3]));
    XA_ERROR_HANDLE(swErrCode);
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND5,
                                 &(pstDoblyParams->aswGeqBands[4]));
    XA_ERROR_HANDLE(swErrCode);
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND6,
                                 &(pstDoblyParams->aswGeqBands[5]));
    XA_ERROR_HANDLE(swErrCode);
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND7,
                                 &(pstDoblyParams->aswGeqBands[6]));
    XA_ERROR_HANDLE(swErrCode);

    /* 17. 配置扬声器均衡
    Speaker  EQ  is  an  equalizer  that  improves  or  equalizes  speaker  and
    headphone  Transfer  functions.  Speaker  EQ  has  a  configurable  numbers
    of  one  to  four    cascaded  infinite  impulse  response  (IIR)  filters
    to  implement  a  custom  frequency  response  by  using  custom  coefficients
    that  are  designed  to  equalize  the  device  headphone  or  speaker  transfer
    function.  Speaker  EQ  also  includes  an  individual   gain  on  each
    channel  to  align  the  output  levels  of  stereo  devices. */

    /* 17.1 配置扬声器均衡是否使能，默认为不使能 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_ENABLE,
                                 &(pstDoblyParams->swSpkEQEnable));
    XA_ERROR_HANDLE(swErrCode);

    /* 17.2 配置左声道额外的增益水平，取值范围为[-24, +24]，分别对应的增益为
    [-6dB, +6dB]，每增加1，增加0.25dB，默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_CHGAIN_DB1,
                                 &(pstDoblyParams->swSpkEQChgainDB1));
    XA_ERROR_HANDLE(swErrCode);

    /* 17.3 配置右声道额外的增益水平，取值范围为[-24, +24]，分别对应的增益为
    [-6dB, +6dB]，每增加1，增加0.25dB，默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_CHGAIN_DB2,
                                 &(pstDoblyParams->swSpkEQChgainDB2));
    XA_ERROR_HANDLE(swErrCode);

    /* 17.4 Tensilica Lib库支持动态地调整SPK EQ的参数，这里不做支持，因此也不进
    行配置。 */

    /* 18 配置PORTABLE MODE，它的含义为:
    Portable mode applies gain or attenuation to the signal after the postgain
    limiter has limited  the  signal.  The  postgain  limiter  is  aware  of  any
    Portable  mode  gain,  and  limits the signal appropriately to ensure that
    clipping artifacts do not occur. The aim  of  Portable  mode  is  to  provide
    a  normalized  dialogue  level  specified  by  the  ORL (output reference level)
    parameter.  */

    /* 18.1 配置PORTABLE MODE的使能项，默认不使能 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE,
                                 &(pstDoblyParams->swPortableMode));
    XA_ERROR_HANDLE(swErrCode);

    /* 18.2 配置PORTABLE MODE的增益，含义为:
    Specifies the desired gain, in dB. This parameter is
    specified in 0.25 dB increments (1 = 0.25 dB, 2 = 0.5 dB, and so on). This value is
    always referenced to a loudness level of -11 dB regardless of the ORL setting.
    默认值为0 */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE_GAIN,
                                 &(pstDoblyParams->swPortableModeGain));
    XA_ERROR_HANDLE(swErrCode);

    /* 18.3 配置PORTABLE MODE的ORL项，含义为:
    Specifies the ORL of the Portable mode. Valid range  is  0  to  2.   Value
    of  0,  1,  2  corresponds  to  ORL  of  -8  dB.    -11  dB,  -14  dB  value
    respectively.
    默认值为1，即-11dB */
    swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                 XA_API_CMD_SET_CONFIG_PARAM,
                                 XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE_ORL,
                                 &(pstDoblyParams->swPortableModeOrl));
     XA_ERROR_HANDLE(swErrCode);

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_EFFECT_Dm3Execute(XA_AUDIO_EFFECT_USER_CFG_STRU* pstUserCfg)
{
    VOS_INT32                       swErrCode;
    xa_codec_func_t                *pstDm3plusProcessApi = xa_dm3plus_ak_pp;
    VOS_INT32                       swSamplesPerChan;

    /* 输入输出采样率之比固定为1 */
    VOS_INT32                       swUpsampleRate       = AUDIO_EFFECT_DM3_IO_SAMP_EQU;

    /* 音效使能配置项 */
    XA_COMM_MEMORY_CFG_STRU        *pstMemCfg            = AUDIO_EFFECT_GetDm3MemCfgPtr();

    xa_codec_handle_t               pstDm3ApiPtr;

    VOS_UINT32                      uwInitResult;
    VOS_CHAR                       *pcUserInBuffShift;
    VOS_CHAR                       *pcUserOutBuffShift;

    VOS_INT32                       swCnt;
    VOS_INT32                       swProcNum;

    VOS_UINT32                      uwApiStatusInvalid   = AUDIO_EFFECT_Dm3ApiValid;

    /* 入参非空检查和入参合法性检查，这里检查pstMemCfg的IO非空，
    是为了防止Exec在初始化前被调用 */

    if(   (  (VOS_NULL                   == pstMemCfg->pApiStructBuff)
           || (VOS_NULL                  == pstMemCfg->pInputBuff)
           || (VOS_NULL                  == pstMemCfg->pOutputBuff))
        &&(AUDIO_EFFECT_PARA_INITIALIZED == AUDIO_EFFECT_GetDm3InitStatus()))
    {
        uwApiStatusInvalid        =  AUDIO_EFFECT_Dm3ApiInvalid;
    }

    if(    (VOS_NULL                   == pstDm3plusProcessApi)
        || (VOS_NULL                   == pstUserCfg->pvInBuff)
        || (VOS_NULL                   == pstUserCfg->pvOutBuff)
        || (AUDIO_EFFECT_Dm3ApiInvalid == uwApiStatusInvalid))
    {
        return UCOM_RET_FAIL;
    }

    /* 如果需要初始化DM3音效库，先进行初始化 */
    if (AUDIO_EFFECT_PARA_INITIALIZED != AUDIO_EFFECT_GetDm3InitStatus())
    {
        if (AUDIO_EFFECT_PARA_UNINITIALIZED == AUDIO_EFFECT_GetDm3InitStatus())
        {
            AUDIO_EFFECT_Dm3Close();
        }

        uwInitResult = AUDIO_EFFECT_Dm3Init(pstUserCfg);

        if ((UCOM_RET_SUCC != uwInitResult) || (VOS_NULL == pstMemCfg->pApiStructBuff))
        {
            return UCOM_RET_FAIL;
        }

        /* 修改标示，表明DM3音效库已经初始化成功 */
        AUDIO_EFFECT_SetDm3InitStatus(AUDIO_EFFECT_PARA_INITIALIZED);
    }

    /* 1. 数据拷贝和转换 */
    /* 杜比音效处理图示

    (1) 第一次处理前SRC给的数据
    ____________________________________________
    |           Input Buff (Persistent)        |  UserCfg.InBuff(不变)
    |__________________________________________|

    (2) 处理中
    ____________________________________________
    |           Input Buff (Persistent)        |  UserCfg.InBuff(不变)
    |__________________________________________|
                                |       |
                                |In Buff|         MemCfg.InBuff(可变)
                                |-------|
                                |In Size|         MemCfg.InSize(不变)
                                |_______|

    (3) 约束: 要求输入点数为[(N)*(声道数)]的倍数

    */

    /* 计算需要内存拷贝多少次 */
    if (0 != (pstUserCfg->swInputSize % pstMemCfg->swInputMemSize))
    {
        swProcNum        = (pstUserCfg->swInputSize / pstMemCfg->swInputMemSize) + 1;
    }
    else
    {
        swProcNum        = pstUserCfg->swInputSize / pstMemCfg->swInputMemSize;
    }

    pcUserInBuffShift    = (VOS_CHAR*)(pstUserCfg->pvInBuff);
    pcUserOutBuffShift   = (VOS_CHAR*)(pstUserCfg->pvOutBuff);
    pstDm3ApiPtr         = AUDIO_EFFECT_GetDm3ApiPtr();

    for(swCnt = 0; swCnt < swProcNum; swCnt++)
    {
        pstMemCfg->pInputBuff  = pcUserInBuffShift;
        pstMemCfg->pOutputBuff = pcUserOutBuffShift;

        /* 计算每个声道采样点个数 */
        swSamplesPerChan       = DM3_N_SAMPLES / swUpsampleRate;

        //设置每个声道中采样点个数，必须为32的整数倍
        swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                     XA_API_CMD_SET_CONFIG_PARAM,
                                     XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SET_NUM_SAMPLES,
                                     &swSamplesPerChan);
        XA_ERROR_HANDLE(swErrCode);

        /* 设置输入BUFFER指针 */
        swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                     XA_API_CMD_SET_CONFIG_PARAM,
                                     XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SET_INPUT_BUF_PTR,
                                     (VOS_VOID*)pstMemCfg->pInputBuff);
        XA_ERROR_HANDLE(swErrCode);

        /* 设置输出BUFFER指针 */
        swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                     XA_API_CMD_SET_CONFIG_PARAM,
                                     XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SET_OUTPUT_BUF_PTR,
                                     (VOS_VOID*)pstMemCfg->pOutputBuff);
        XA_ERROR_HANDLE(swErrCode);

        /* 启动DM3+音效处理 */
        swErrCode = (*pstDm3plusProcessApi)(pstDm3ApiPtr,
                                     XA_API_CMD_EXECUTE,
                                     XA_CMD_TYPE_DO_EXECUTE,
                                     NULL);
        XA_ERROR_HANDLE(swErrCode);

        pcUserInBuffShift  += pstMemCfg->swInputMemSize;
        pcUserOutBuffShift += pstMemCfg->swOutputMemSize;

    }
    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_EFFECT_Dm3ParseParams
 功能描述  : 根据paramid-value键值对解析杜比音效配置
 输入参数  : paramId
             value
             pstDoblyParams
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年1月19日
    作    者   : twx144307
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AUDIO_EFFECT_Dm3ParseParams(VOS_INT32 swParamId, VOS_INT32 swValue, AUDIO_EFFECT_DOBLY_PARAMS_STRU* pstDoblyParams)
{
    /* 入参非空由调用者保证 */

    switch(swParamId)
    {
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_BYPASS:
        {
            if(swValue == 0)
            {
                pstDoblyParams->swByPass = 0;/* bypass off */
            }
            else
            {
                pstDoblyParams->swByPass = 2;/* all feature bypass */
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_M2S_ENABLE:
        {
            //0 enable,1 disable
            if((M2S_N_ENABLES <= swValue) || (0 > swValue))
            {
                pstDoblyParams->swM2sEnable = 0;
            }
            else
            {
                pstDoblyParams->swM2sEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_ENABLE:
        {
            //0 disable,1 enable
            if((swValue >= SSE_N_ENABLES) || (swValue < 0))
            {
                pstDoblyParams->swSSEEnable = 0;
            }
            else
            {
                pstDoblyParams->swSSEEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_WIDTH:
        {
            if((swValue >= SSE_N_WIDTHS) || (swValue < 0))
            {
                pstDoblyParams->swSSEWidth = 0;
            }
            else
            {
                pstDoblyParams->swSSEWidth = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SSE_SPK_MODE:
        {
            if((swValue >= SSE_N_SPK_MODES) || (swValue < 0))
            {
                pstDoblyParams->swSSESpkMode = 1;
            }
            else
            {
                pstDoblyParams->swSSESpkMode = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_ENABLE:
        {
            if((swValue >= MSR_N_ENABLES) || (swValue < 0))
            {
                pstDoblyParams->swMsrEnable = 0;
            }
            else
            {
                pstDoblyParams->swMsrEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_ROOM:
        {
            if((swValue >= MSR_N_ROOMS) || (swValue < 0))
            {
                pstDoblyParams->swMsrRoot = MSR_DEF_ROOM;
            }
            else
            {
                pstDoblyParams->swMsrRoot = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_MSR_BRIGHT:
        {
            if((swValue >= MSR_N_BRIGHTS) || (swValue < 0))
            {
                pstDoblyParams->swMsrBright = MSR_DEF_BRIGHT;
            }
            else
            {
                pstDoblyParams->swMsrBright = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_ENABLE:
        {
            if((swValue >= SLC_N_ENABLES) || (swValue < 0))
            {
                pstDoblyParams->swSlcEnable = 0;
            }
            else
            {
                pstDoblyParams->swSlcEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_LEVEL:
        {
            if( (swValue >= SLC_N_LEVELS) || (swValue < 0))
            {
                pstDoblyParams->swSlcLevel = 4;
            }
            else
            {
                pstDoblyParams->swSlcLevel = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SLC_DEPTH:
        {
            if( (swValue >= SLC_N_DEPTHS) || (swValue < 0) )
            {
                pstDoblyParams->swSlcDepth = 1;
            }
            else
            {
                pstDoblyParams->swSlcDepth = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_ENABLE:
        {
            if( (swValue >= NB_N_ENABLES) || (swValue < 0) )
            {
                pstDoblyParams->swNBEnable = 0;
            }
            else
            {
                pstDoblyParams->swNBEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_CUTOFF:
        {
            if( (swValue >= NB_N_CUTOFFS) || (swValue < 0) )
            {
                pstDoblyParams->swNBCutoff = 12;
            }
            else
            {
                pstDoblyParams->swNBCutoff = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_BOOST:
        {
            if( (swValue >= NB_N_BOOSTS) || (swValue < 0) )
            {
                 pstDoblyParams->swNBBoost = 3;
            }
            else
            {
                 pstDoblyParams->swNBBoost = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_NB_LEVEL:
        {
            if( (swValue >= NB_N_LEVELS) || (swValue < 0) )
            {
                pstDoblyParams->swNBLevel = 3;
            }
            else
            {
                pstDoblyParams->swNBLevel = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_HFE_ENABLE:
        {
            if( (swValue >= HFE_N_ENABLES) || (swValue < 0) )
            {
                pstDoblyParams->swHfeEnable = 0;
            }
            else
            {
                pstDoblyParams->swHfeEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_HFE_DEPTH:
        {
            if( (swValue >= HFE_N_DEPTHS) || (swValue < 0) )
            {
                pstDoblyParams->swHfeDepth = 3;
            }
            else
            {
                pstDoblyParams->swHfeDepth = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_ENABLE:
        {
            if( (swValue >= GEQ_N_ENABLES) || (swValue < 0) )
            {
                pstDoblyParams->swGeqEnable = 0;
            }
            else
            {
                pstDoblyParams->swGeqEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_MAXBOOST:
        {
            if(swValue < GEQ_MIN_MAXBOOST)
            {
                pstDoblyParams->swGeqMaxBoost = GEQ_MIN_MAXBOOST;
            }
            else if(swValue > GEQ_MAX_MAXBOOST)
            {
                pstDoblyParams->swGeqMaxBoost = GEQ_MAX_MAXBOOST;
            }
            else
            {
                pstDoblyParams->swGeqMaxBoost = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_PREAMP:
        {
            if(swValue < GEQ_MIN_PREAMP)
            {
                pstDoblyParams->swGeqPreamp = GEQ_MIN_PREAMP;
            }
            else if(swValue > GEQ_MAX_PREAMP)
            {
                pstDoblyParams->swGeqPreamp = GEQ_MAX_PREAMP;
            }
            else
            {
                pstDoblyParams->swGeqPreamp = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_NBANDS:
        {
            if( (swValue > GEQ_N_BANDS_MAX) || (swValue < GEQ_N_BANDS_MIN) )
            {
                pstDoblyParams->swGeqNbands = GEQ_N_BANDS_MIN;
            }
            else
            {
                pstDoblyParams->swGeqNbands = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND1:
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND2:
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND3:
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND4:
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND5:
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND6:
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND7:
        {
            if(swValue >= AUDIO_EFFECT_GEQ_MAX_GAIN_SCALE)
            {
                pstDoblyParams->aswGeqBands[swParamId - XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND1] = AUDIO_EFFECT_GEQ_MAX_GAIN_SCALE;
            }
            else if(swValue < AUDIO_EFFECT_GEQ_MIN_GAIN_SCALE)
            {
                pstDoblyParams->aswGeqBands[swParamId - XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND1] = AUDIO_EFFECT_GEQ_MIN_GAIN_SCALE;
            }
            else
            {
                pstDoblyParams->aswGeqBands[swParamId - XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_GEQ_BAND1] = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_ENABLE:
        {
            if( (swValue >= SPKEQ_N_ENABLES) || (swValue < 0) )
            {
                pstDoblyParams->swSpkEQEnable = 0;
            }
            else
            {
                pstDoblyParams->swSpkEQEnable = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_CHGAIN_DB1:
        {
            if( (swValue > SPKEQ_MAX_CHGAIN_DB) || (swValue < SPKEQ_MIN_CHGAIN_DB) )
            {
                pstDoblyParams->swSpkEQChgainDB1 = SPKEQ_DEFAULT_CHGAIN_DB;
            }
            else
            {
                pstDoblyParams->swSpkEQChgainDB1 = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_SPKEQ_CHGAIN_DB2:
        {
            if( (swValue > SPKEQ_MAX_CHGAIN_DB) || (swValue < SPKEQ_MIN_CHGAIN_DB) )
            {
                pstDoblyParams->swSpkEQChgainDB2 = SPKEQ_DEFAULT_CHGAIN_DB;
            }
            else
            {
                pstDoblyParams->swSpkEQChgainDB2 = swValue;
            }
            break;
        }

        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE:
        {
            if(TDAS_PORTABLEMODE_ENABLE != swValue)
            {
                pstDoblyParams->swPortableMode = TDAS_PORTABLEMODE_DISABLE;
            }
            else
            {
                pstDoblyParams->swPortableMode = TDAS_PORTABLEMODE_ENABLE;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE_GAIN:
        {
            if( (swValue >= TDAS_PORTABLEMODE_GAIN_MAX) || (swValue < TDAS_PORTABLEMODE_GAIN_MIN) )
            {
                pstDoblyParams->swPortableModeGain = TDAS_PORTABLEMODE_GAIN_DEF;
            }
            else
            {
                pstDoblyParams->swPortableModeGain = swValue;
            }
            break;
        }
        case XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_PORTABLEMODE_ORL:
        {
            if( (swValue > TDAS_PORTABLEMODE_ORL_14DB) || (swValue < TDAS_PORTABLEMODE_ORL_8DB) )
            {
                pstDoblyParams->swPortableModeOrl = TDAS_PORTABLEMODE_ORL_DEF;
            }
            else
            {
                pstDoblyParams->swPortableModeOrl = swValue;
            }
            break;
        }
        default:
        {
            return;
        }
    }

    return;

}

/*****************************************************************************
 函 数 名  : CharArrayToInt16
 功能描述  : 字节转INT16
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : twx144307
    修改内容   : 新生成函数

*****************************************************************************/
VOS_INT32 AUDIO_EFFECT_CharArrayToInt16(VOS_CHAR cLb, VOS_CHAR cHb)
{
    /* 实现说明: cLb为低位char，cHb为高位char，将它们分别放在一个INT16的低8位和高8位 */
    return (VOS_INT32) ((0xff&cHb)<<8) | (0xff&cLb);
}

/*****************************************************************************
 函 数 名  : CharArrayToInt32
 功能描述  : 字节转INT32
 输入参数  : ba 字节数组
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : twx144307
    修改内容   : 新生成函数

*****************************************************************************/
VOS_INT32 AUDIO_EFFECT_CharArrayToInt32(VOS_CHAR* cBa)
{
    /* 实现说明: 将char cBa[0] - cBa[3]，分别放在一个INT32的4个从低到高的位置 */
    return ((cBa[3] & 0xff) << 24) | ((cBa[2] & 0xff) << 16) | ((cBa[1] & 0xff) << 8) | (cBa[0] & 0xff);
}
VOS_UINT32 AUDIO_EFFECT_MsgSetEnableReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_EFFECT_ENABLE_REQ_STRU  *pstMsg   = (AUDIO_EFFECT_ENABLE_REQ_STRU*)pvOsaMsg;
    XA_AUDIO_EFFECT_USER_CFG_STRU *pstCfg   = AUDIO_EFFECT_GetUserCfgPtr();

    if(VOS_NULL == pstMsg)
    {
        return UCOM_RET_FAIL;
    }

    if(   (pstMsg->enEffectType   >= AUDIO_EFFECT_TYPE_BUT)
       || (pstMsg->enEffectEnable >= AUDIO_EFFECT_ENABLE_BUT))
    {
        return UCOM_RET_FAIL;
    }

    if(AUDIO_EFFECT_NONE == pstMsg->enEffectEnable)
    {
        pstCfg->enDm3Enable = AUDIO_EFFECT_SWITCH_OFF;
        pstCfg->enDtsEnable = AUDIO_EFFECT_SWITCH_OFF;

        /* 音效库需要重新初始化 */
        AUDIO_EFFECT_SetDm3InitStatus(AUDIO_EFFECT_PARA_UNINITIALIZED);

        return UCOM_RET_SUCC;
    }

    if(AUDIO_EFFECT_DM3 == pstMsg->enEffectType)
    {
        switch(pstMsg->enEffectEnable)
        {
        case AUDIO_EFFECT_DM3_ENABLE:
            pstCfg->enDm3Enable = AUDIO_EFFECT_SWITCH_ON;
            break;
        case AUDIO_EFFECT_DM3_DISABLE:
            pstCfg->enDm3Enable = AUDIO_EFFECT_SWITCH_OFF;
            break;
        default:
            return UCOM_RET_FAIL;
        }
    }

    if(AUDIO_EFFECT_DTS == pstMsg->enEffectType)
    {
        switch(pstMsg->enEffectEnable)
        {
        case AUDIO_EFFECT_DTS_ENABLE:
            pstCfg->enDtsEnable = AUDIO_EFFECT_SWITCH_ON;
            break;
        case AUDIO_EFFECT_DTS_DISABLE:
            pstCfg->enDtsEnable = AUDIO_EFFECT_SWITCH_OFF;
            break;
        default:
            return UCOM_RET_FAIL;
        }
    }

    /* 音效库需要重新初始化 */
    AUDIO_EFFECT_SetDm3InitStatus(AUDIO_EFFECT_PARA_UNINITIALIZED);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_EFFECT_MsgSetParam
 功能描述  : 配置参数
 输入参数  : VOS_VOID*   消息
 输出参数  : 无
 返 回 值：UCOM_RET_SUCC 成功 ；其他失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年12月25日
    作    者   : twx144307
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_EFFECT_MsgSetParamReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_EFFECT_SET_PARAMS_REQ_STRU *pstMsg;
    VOS_UINT16                        uhwByPass;
    VOS_INT32                         swTmpParamId;
    VOS_INT32                         swTmpValue;
    VOS_UINT32                        uwIndex     = 0;
    VOS_INT32                         swDevice    = AUDIO_EFFECT_DEVICE_HEADPHONE;
    VOS_INT32                         swParamId;
    VOS_INT32                         swValue;

    pstMsg                            = (AUDIO_EFFECT_SET_PARAMS_REQ_STRU*)pvOsaMsg;

    /* 暂时仅支持DM3音效参数配置 */
    if(pstMsg->enEffectType != AUDIO_EFFECT_DM3)
    {
        return UCOM_RET_FAIL;
    }

    switch(pstMsg->enEffectSetParam)
    {
        case AUDIO_EFFECT_DM3_BYPASS:
        {
            uhwByPass = *((VOS_UINT16*)pstMsg->uwBufAddr);
            AUDIO_EFFECT_Dm3ParseParams(XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_BYPASS, uhwByPass, &g_stDm3HeadphoneParams);
            AUDIO_EFFECT_Dm3ParseParams(XA_DM3PLUS_AK_PP_CONFIG_PARAM_TDAS_BUNDLE_BYPASS, uhwByPass, &g_stDm3SpkParams);
            break;
        }
        case AUDIO_EFFECT_DM3_SINGLE_DEVICE_VALUE:
        {
            swDevice = AUDIO_EFFECT_CharArrayToInt32((VOS_CHAR*)pstMsg->uwBufAddr);
            swParamId = AUDIO_EFFECT_CharArrayToInt16( ((VOS_CHAR*)pstMsg->uwBufAddr)[4],
                            ((VOS_CHAR*)pstMsg->uwBufAddr)[5]);
            swValue = AUDIO_EFFECT_CharArrayToInt16( ((VOS_CHAR*)pstMsg->uwBufAddr)[6],
                            ((VOS_CHAR*)pstMsg->uwBufAddr)[7] );

            if( AUDIO_EFFECT_DEVICE_HEADPHONE == swDevice)
            {
                AUDIO_EFFECT_Dm3ParseParams(swParamId, swValue, &g_stDm3HeadphoneParams);
            }
            else if( AUDIO_EFFECT_DEVICE_SPK == swDevice)
            {
                AUDIO_EFFECT_Dm3ParseParams(swParamId, swValue, &g_stDm3SpkParams);
            }
            else
            {
                //不支持的设备
                return UCOM_RET_FAIL;
            }
            break;
        }
        case AUDIO_EFFECT_DM3_ALL_VALUES:
        {
            uwIndex += 2;
            swDevice = AUDIO_EFFECT_CharArrayToInt32( &((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex]);
            uwIndex += 2;
            if( AUDIO_EFFECT_DEVICE_HEADPHONE == swDevice)
            {
                //耳机设备
                while(uwIndex < pstMsg->uwDataSize)
                {
                    swTmpParamId = (VOS_INT16)AUDIO_EFFECT_CharArrayToInt16(((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex],
                                    ((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex+1]);
                    swTmpValue = (VOS_INT16)AUDIO_EFFECT_CharArrayToInt16(((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex+2],
                                    ((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex+3]);
                    uwIndex += 4;
                    AUDIO_EFFECT_Dm3ParseParams(swTmpParamId, swTmpValue, &g_stDm3HeadphoneParams);
                }
            }
            else if( AUDIO_EFFECT_DEVICE_SPK == swDevice)
            {
                //外放设备
                while(uwIndex < pstMsg->uwDataSize)
                {
                    swTmpParamId = (VOS_INT16)AUDIO_EFFECT_CharArrayToInt16(((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex],
                                    ((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex+1]);
                    swTmpValue = (VOS_INT16)AUDIO_EFFECT_CharArrayToInt16(((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex+2],
                                    ((VOS_CHAR*)pstMsg->uwBufAddr)[uwIndex+3]);
                    uwIndex += 4;
                    AUDIO_EFFECT_Dm3ParseParams(swTmpParamId, swTmpValue, &g_stDm3SpkParams);
                }
            }
            else
            {
                //设备不支持
                return UCOM_RET_FAIL;
            }
            break;
        }
        default:
        {
            // 不支持 param id
            return UCOM_RET_FAIL;
        }
    }

    /* 音效库需要重新初始化 */
    if (AUDIO_EFFECT_PARA_UNINITIALIZED != AUDIO_EFFECT_GetDm3InitStatus())
    {
        AUDIO_EFFECT_SetDm3InitStatus(AUDIO_EFFECT_PARA_RESET_ONLY);
    }

    return UCOM_RET_SUCC;

}


VOS_VOID AUDIO_EFFECT_Dm3SetParaAsDefault(AUDIO_EFFECT_DOBLY_PARAMS_STRU *pstPara)
{
    pstPara->swInMatrix         = 0;
    pstPara->swByPass           = 0;
    pstPara->swLfeMixLevel      = 10;
    pstPara->swMsrMaxProfile    = 2;
    pstPara->swM2sEnable        = 0;
    pstPara->swSSEEnable        = 0;
    pstPara->swSSEWidth         = 1;
    pstPara->swSSESpkMode       = 0;
    pstPara->swMsrEnable        = 0;
    pstPara->swMsrRoot          = 1;
    pstPara->swMsrBright        = 1;
    pstPara->swSlcEnable        = 0;
    pstPara->swSlcLevel         = 0;
    pstPara->swSlcDepth         = 0;
    pstPara->swNBEnable         = 0;
    pstPara->swNBCutoff         = 0;
    pstPara->swNBBoost          = 0;
    pstPara->swNBLevel          = 0;
    pstPara->swHfeEnable        = 0;
    pstPara->swHfeDepth         = 0;
    pstPara->swGeqEnable        = 0;
    pstPara->swGeqMaxBoost      = 24;
    pstPara->swGeqPreamp        = 0;
    pstPara->swGeqNbands        = 2;
    pstPara->aswGeqBands[0]     = 0;
    pstPara->aswGeqBands[1]     = 0;
    pstPara->aswGeqBands[2]     = 0;
    pstPara->aswGeqBands[3]     = 0;
    pstPara->aswGeqBands[4]     = 0;
    pstPara->aswGeqBands[5]     = 0;
    pstPara->aswGeqBands[6]     = 0;
    pstPara->swSpkEQEnable      = 0;
    pstPara->swSpkEQChgainDB1   = 0;
    pstPara->swSpkEQChgainDB2   = 0;
    pstPara->swPortableMode     = 0;
    pstPara->swPortableModeGain = 0;
    pstPara->swPortableModeOrl  = 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

