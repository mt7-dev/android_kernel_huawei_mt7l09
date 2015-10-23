/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

******************************************************************************
 文 件 名   : rfd_om_def.h
 版 本 号   : 初稿
 作    者   : wangxu
 生成日期   : 2011年4月25日
 最近修改   :
 功能描述   : RFD OM 的头文件
 函数列表   :
 修改历史   :
 1.日    期   : 2012年1月9日
   作    者   : shijiahong
   修改内容   : 创建文件
 --------------------------------------------------------------------------------
 ******************************************************************************/

#ifndef __RFD_OM_DEF_H__
#define __RFD_OM_DEF_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


typedef enum
{
    LPHY_RFD_FREQ_SWITCH_OM_REQ  = OM_CMD_ID(LPHY_RFD_MID, OM_TYPE_REQ, 0x0),/*切频上报请求*/
    LPHY_RFD_AGC_DC_OM_REQ                                                     /*AGC/DC配置上报请求*/
}LPHY_RFD_OM_REQ_ENUM;

typedef enum
{
    RFD_FREQ_SWITCH_OM_OFF = 0x0,
    RFD_FREQ_SWITCH_OM_ON,
    RFD_FREQ_SWITCH_OM_RD
}RFD_FREQ_SWITCH_OM_SWITCH_ENUM;

typedef enum
{
    RFD_AGC_DC_CFG_OM_OFF = 0x0,
    RFD_AGC_DC_CFG_OM_ON
}RFD_AGC_DC_CFG_OM_SWITCH_ENUM;


typedef struct
{
    UINT16 usBand;
    UINT16 usBw;
    UINT32 ulFreq;
}RFD_BAND_INFO_STRU;

typedef struct
{
    RFD_FREQ_SWITCH_OM_SWITCH_ENUM  enRptSwitch;
    UINT32                          ulRfSsiCmd[10];  /*切频完成后产生的ssi指令*/
    RFD_BAND_INFO_STRU              stDestBandInfo;  /*目标band信息*/
    RFD_BAND_INFO_STRU              stCurrBandInfo;  /*当前band信息*/
}RFD_FREQ_SWITCH_OM_STRU;

typedef struct
{
    RFD_AGC_DC_CFG_OM_SWITCH_ENUM   enAgcDcCfgRpt;
    UINT16                          usAnt0RfGainCode; /*天线0的aagc code*/
    UINT16                          usAnt0RfDCCode;   /*天线0的dc code*/
    UINT16                          usAnt1RfGainCode; /*天线1的aagc code*/
    UINT16                          usAnt1RfDCCode;   /*天线1的dc code*/
}RFD_AGC_DC_CFG_OM_STRU;



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __RFD_OM_DEF_H__ */

