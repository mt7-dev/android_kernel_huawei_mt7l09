/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : RFA.h
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2008年2月2日
  最近修改   :
  功能描述   : RFA.C 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2008年2月2日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/

#ifndef __RFA_H__
#define __RFA_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "NVIM_Interface.h"

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 产品定义的最多支持的BAND数目*/
#define RFA_UE_SUPPORT_MAX_BAND_NUM         32

#define RFA_CPHY_SUPPORT_BANDS              0x405BF

#define ID_MMA_RFA_START_MODE_REQ           0x0005
#define ID_MMA_RFA_STOP_MODE_REQ            0x0006

/*APT表NV项初始化值*/
#define APT_NV_DATA_INIT                    0

/*一张APT表对应的NV项最大有16组数*/
#define APT_NV_DATA_MAX_NUM                 16

/*一张APT表对应的NV项最小组数*/
#define APT_NV_DATA_MIN_NUM                 0

/*每张APT表有128个元素*/
#define RFA_APT_ITEM_SIZE                   128

/*单个增益需要APT表数*/
#define RFA_APT_GAIN_TABLE_NUM              4

/*高增益APT表的基址*/
#define RFA_APT_Table_HIGH_GAIN_Base_P0     700

/*中增益APT表的基址*/
#define RFA_APT_Table_MID_GAIN_Base_P0      700

/*低增益APT表的基址*/
#define RFA_APT_Table_LOW_GAIN_Base_P0      700

/*每张CQI表的长度*/
#define RFA_CQI_TABLE_LENGTH                2160

/*CQI表的总长度*/
#define RFA_CQI_TOTAL_LENGTH                (13*1024)

/*APT表的总长度*/
#define RFA_APT_TOTAL_LENGTH                (33*1024)

/*ET表的总长度,暂时定义，后续调试再确定*/
#define RFA_ET_TOTAL_LENGTH                 (11*1024)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum APT_GAIN_MODE_ENUM
{
    APT_GAIN_MODE_HIGH = 0,
    APT_GAIN_MODE_MIDDLE,
    APT_GAIN_MODE_LOW,
    APT_GAIN_MODE_BUTT
};
typedef VOS_UINT32 APT_GAIN_MODE_ENUM_U32;

/*各个增益间同一NV项的APT表相对偏移*/
#define RFA_APT_GAIN_OFFSET                 (RFA_APT_ITEM_SIZE * APT_GAIN_MODE_BUTT)

/*每个 Band的APT表总长度*/
#define RFA_APT_BAND_SIZE                   (RFA_APT_GAIN_OFFSET * RFA_APT_GAIN_TABLE_NUM)

enum RFA_EEIC_MODE_ENUM
{
    RFA_EEIC_MODE_DISABLE = 0,
    RFA_EEIC_MODE_ENABLE,
    RFA_EEIC_MODE_BUTT
};
typedef VOS_UINT16 RFA_EEIC_MODE_ENUM_U16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
extern VOS_UINT32 g_ulUeWcdmaBands;

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32    ulCmdType;        /* 消息类型*/
}MMA_RFA_MODE_MSG;


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_UINT32                  ulCount;                             /*Data Number*/
    VOS_INT16                   asNvItem[APT_NV_DATA_MAX_NUM][2];    /*16 sets of Data*/
}RFA_APT_NV_VBIAS_STRU;

typedef struct
{
    VOS_UINT32                  ulCount;                              /*Data Number*/
    VOS_INT16                   asNvPower[APT_NV_DATA_MAX_NUM];       /*Power*/
    VOS_UINT16                  ausNvVolt[APT_NV_DATA_MAX_NUM+1];     /*Vbias, +1为了解决临界情况,对临界做保护*/
    VOS_INT16                   asNvCompHC[APT_NV_DATA_MAX_NUM+1];    /*High Channel Compensation*/
    VOS_INT16                   asNvCompMC[APT_NV_DATA_MAX_NUM+1];    /*Middle Channel Compensation*/
    VOS_INT16                   asNvCompLC[APT_NV_DATA_MAX_NUM+1];    /*Low Channel Compensation*/

}RFA_APT_NV_DATA_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_UINT32 RFA_MsgProc(MsgBlock *pMsg);
extern VOS_UINT32 WuepsRfaPidInit ( enum VOS_INIT_PHASE_DEFINE ip );

extern VOS_UINT32 RFA_GetFixMemInfo(VOS_UINT32 ulMemType, VOS_UINT32 ulMemSize, VOS_UINT32 *pulMemAddr);

VOS_UINT32 RFA_CQITableProc(VOS_VOID);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of RFA.h */
