/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : CodecNvId.h
  版 本 号   :
  作    者   : 祝锂
  生成日期   : 2009年5月14日
  最近修改   :
  功能描述   : 定义了Codec NV项ID枚举定义
  函数列表   :
  修改历史   :
  1.日    期   : 2010年4月15日
    作    者   : 祝锂 59254
    修改内容   : 创建文件

******************************************************************************/

#ifndef __CODECNVID_H__
#define __CODECNVID_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "NvIddef.h"

/*typedef VOS_UINT16  SYS_NV_ID_ENUM_U16;
 */

enum CODEC_NV_ID_ENUM
{
      en_NV_Item_MaxVolLevel = 0x7530,                                          /* 30000 */
      en_NV_HeadSetPara_Audio,                                                  /* 30001 */
      en_NV_HandSetPara_Audio,                                                  /* 30002 */
      en_NV_HandFreePara_Audio,                                                 /* 30003 */
      en_NV_PCVoice_Audio,                                                      /* 30004 */

      en_NV_Item_ErrLog_Enable,                                                 /* 30005 */
      en_NV_Item_ErrLog_DataStru,                                               /* 30006 */
      en_NV_Item_Hook_Enable_Target,                                            /* 30007 */

      en_NV_WB_MaxVolLevel,                                                     /* 30008 */

      en_NV_CLVL_VOLUM,                                                         /* 30009 */
      en_NV_VoiceTraceCfg,                                                      /* 30010 */
      en_NV_HifiTraceCfg,                                                       /* 30011 */

      en_NV_Item_SIO_Voice_Master,                                              /* 30012 */
      en_NV_AudioTraceCfg,                                                      /* 30013 */
      en_NV_Item_SIO_Audio_Master,                                              /* 30014 */

      en_NV_HifiPowerOffCfg,                                                    /* 30015 */
      en_NV_HifiWatchDogCfg,                                                    /* 30016 */

      en_NV_NB_HandSet1,                                                        /* 30017 */
      en_NV_NB_HandSet2,                                                        /* 30018 */
      en_NV_NB_HandSet3,                                                        /* 30019 */
      en_NV_NB_HandFree1,                                                       /* 30020 */
      en_NV_NB_HandFree2,                                                       /* 30021 */
      en_NV_NB_HandFree3,                                                       /* 30022 */
      en_NV_NB_CarFree1,                                                        /* 30023 */
      en_NV_NB_CarFree2,                                                        /* 30024 */
      en_NV_NB_CarFree3,                                                        /* 30025 */
      en_NV_NB_HeadSet1,                                                        /* 30026 */
      en_NV_NB_HeadSet2,                                                        /* 30027 */
      en_NV_NB_HeadSet3,                                                        /* 30028 */
      en_NV_NB_BlueTooth1,                                                      /* 30029 */
      en_NV_NB_BlueTooth2,                                                      /* 30030 */
      en_NV_NB_BlueTooth3,                                                      /* 30031 */
      en_NV_NB_PCVoice1,                                                        /* 30032 */
      en_NV_NB_PCVoice2,                                                        /* 30033 */
      en_NV_NB_PCVoice3,                                                        /* 30034 */
      en_NV_NB_HeadPhone1,                                                      /* 30035 */
      en_NV_NB_HeadPhone2,                                                      /* 30036 */
      en_NV_NB_HeadPhone3,                                                      /* 30037 */
      en_NV_WB_HandSet1,                                                        /* 30038 */                                                       /* 12335 */
      en_NV_WB_HandSet2,                                                        /* 30039 */
      en_NV_WB_HandSet3,                                                        /* 30040 */
      en_NV_WB_HandFree1,                                                       /* 30041 */
      en_NV_WB_HandFree2,                                                       /* 30042 */
      en_NV_WB_HandFree3,                                                       /* 30043 */
      en_NV_WB_CarFree1,                                                        /* 30044 */
      en_NV_WB_CarFree2,                                                        /* 30045 */
      en_NV_WB_CarFree3,                                                        /* 30046 */
      en_NV_WB_HeadSet1,                                                        /* 30047 */
      en_NV_WB_HeadSet2,                                                        /* 30048 */
      en_NV_WB_HeadSet3,                                                        /* 30049 */
      en_NV_WB_BlueTooth1,                                                      /* 30050 */
      en_NV_WB_BlueTooth2,                                                      /* 30051 */
      en_NV_WB_BlueTooth3,                                                      /* 30052 */
      en_NV_WB_PCVoice1,                                                        /* 30053 */
      en_NV_WB_PCVoice2,                                                        /* 30054 */
      en_NV_WB_PCVoice3,                                                        /* 30055 */
      en_NV_WB_HeadPhone1,                                                      /* 30056 */
      en_NV_WB_HeadPhone2,                                                      /* 30057 */
      en_NV_WB_HeadPhone3,                                                      /* 30058 */
      en_NV_PHY_MIC_NUM,                                                        /* 30059 */

      en_NV_SmartPACfg,                                                         /* 30060 */
      en_NV_AudioTxCompCfg,                                                     /* 30061 */
      en_NV_AudioRxCompCfg,                                                     /* 30062 */

      en_NV_Audio_HandFree1,                                                    /* 30063 */
      en_NV_Audio_HandFree2,                                                    /* 30064 */
      en_NV_Audio_HandFree3,                                                    /* 30065 */
      en_NV_Audio_HeadPhone1,                                                   /* 30066 */
      en_NV_Audio_HeadPhone2,                                                   /* 30067 */
      en_NV_Audio_HeadPhone3,                                                   /* 30068 */
                                                                                /* 30069 - 30140 终端公司算法使用 */
      en_NV_OpenDSP_Config  = 30141,
      en_NV_Audio_Play_MBDRC_Config_Part1,                                      /* 30142 */
      en_NV_Audio_Play_MBDRC_Config_Part2,                                      /* 30143 */

      en_NV_Audio_HandSet1,                                                     /* 30144 */
      en_NV_Audio_HandSet2,                                                     /* 30145 */
      en_NV_Audio_HandSet3,                                                     /* 30146 */

      en_NV_VoiceDiagnoseCfg,                                                   /* 30147 */

      en_NV_VoiceJbCfg,                                                         /* 30148 */

      en_NV_Item_Codec_BUTT
};


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __CODECNVID_H__ */


