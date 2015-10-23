

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef _VOS_PID_DEF_H
#define _VOS_PID_DEF_H

#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 支持互相发送消息的多核的最大数目 */
#define VOS_SUPPORT_CPU_NUM_MAX     (5)

/* CPU0 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_0_DOPRAEND   (20)

/* macro defined for PID management */
#define BEGIN_CPU_ID_0_PID_DEFINITION() \
    enum VOS_CPU_ID_0_Process_ID \
    { \
        VOS_CPU_ID_0_PID_RESERVED = VOS_PID_CPU_ID_0_DOPRAEND - 1,

#define END_CPU_ID_0_PID_DEFINITION() \
        VOS_CPU_ID_0_PID_BUTT \
    };

/* CPU1 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_1_DOPRAEND   (256)

/* macro defined for PID management */
#define BEGIN_CPU_ID_1_PID_DEFINITION() \
    enum VOS_CPU_ID_1_Process_ID \
    { \
        VOS_CPU_ID_1_PID_RESERVED = VOS_PID_CPU_ID_1_DOPRAEND - 1,

#define END_CPU_ID_1_PID_DEFINITION() \
        VOS_CPU_ID_1_PID_BUTT \
    };

/* CPU2 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_2_DOPRAEND   (512)


/* macro defined for PID management */
#define BEGIN_CPU_ID_2_PID_DEFINITION() \
    enum VOS_CPU_ID_2_Process_ID \
    { \
        VOS_CPU_ID_2_PID_RESERVED = VOS_PID_CPU_ID_2_DOPRAEND - 1,

#define END_CPU_ID_2_PID_DEFINITION() \
        VOS_CPU_ID_2_PID_BUTT \
    };

/* CPU3 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_3_DOPRAEND   (768)


/* macro defined for PID management */
#define BEGIN_CPU_ID_3_PID_DEFINITION() \
    enum VOS_CPU_ID_3_Process_ID \
    { \
        VOS_CPU_ID_3_PID_RESERVED = VOS_PID_CPU_ID_3_DOPRAEND - 1,

#define END_CPU_ID_3_PID_DEFINITION() \
        VOS_CPU_ID_3_PID_BUTT \
    };

/* CPU4 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_4_DOPRAEND   (1024)


/* macro defined for PID management */
#define BEGIN_CPU_ID_4_PID_DEFINITION() \
    enum VOS_CPU_ID_4_Process_ID \
    { \
        VOS_CPU_ID_4_PID_RESERVED = VOS_PID_CPU_ID_4_DOPRAEND - 1,

#define END_CPU_ID_4_PID_DEFINITION() \
        VOS_CPU_ID_4_PID_BUTT \
    };

/* CPU5 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_5_DOPRAEND   (1280)

/* macro defined for PID management */
#define BEGIN_CPU_ID_5_PID_DEFINITION() \
    enum VOS_CPU_ID_5_Process_ID \
    { \
        VOS_CPU_ID_5_PID_RESERVED = VOS_PID_CPU_ID_5_DOPRAEND - 1,

#define END_CPU_ID_5_PID_DEFINITION() \
        VOS_CPU_ID_5_PID_BUTT \
    };

/* 注意: 从0x8000开始的PID号已经归外部模块使用 */

/* 具体申明一个PID */
#define DEFINE_PID(pid) pid,

/* 根据PID 得到 CPU ID */
#define VOS_GetCpuId(pid)      (((pid)>>0x08)&0x0F)

/* macro defined for PID management */
#define END_DECLARE_PID_BELONG_TO_FID() \
    }

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
enum MODEM_ID_ENUM
{
    MODEM_ID_0 = 0,
    MODEM_ID_1,
    MODEM_ID_BUTT
};
typedef VOS_UINT16 MODEM_ID_ENUM_UINT16;
#else
enum MODEM_ID_ENUM
{
    MODEM_ID_0 = 0,
    MODEM_ID_BUTT,
    MODEM_ID_1
};
typedef VOS_UINT16 MODEM_ID_ENUM_UINT16;
#endif

/* CPU ID 的定义 0 是CCPU 1 是ACPU 2 是ZSP500,GU DSP 3 是Hifi,Med */
enum VOS_CPU_ID_ENUM
{
    VOS_CPU_ID_CCPU = 0,
    VOS_CPU_ID_ACPU,
    VOS_CPU_ID_GUDSP,
    VOS_CPU_ID_MEDDSP,
    VOS_CPU_ID_MCU,
    VOS_CPU_ID_BBE16,
    VOS_CPU_ID_BUTT
};

BEGIN_CPU_ID_0_PID_DEFINITION()

/*卡1的PID定义*/
    DEFINE_PID(I1_WUEPS_PID_USIM)       /*20*/
    DEFINE_PID(I1_MAPS_STK_PID)
    DEFINE_PID(I1_MAPS_PIH_PID)
    DEFINE_PID(I1_MAPS_PB_PID)
    DEFINE_PID(I1_UEPS_PID_GRM)
    DEFINE_PID(I1_UEPS_PID_DL)
    DEFINE_PID(I1_UEPS_PID_LL)
    DEFINE_PID(I1_UEPS_PID_SN)
    DEFINE_PID(I1_UEPS_PID_GAS)
    DEFINE_PID(I1_WUEPS_PID_MM)
    DEFINE_PID(I1_WUEPS_PID_MMC)
    DEFINE_PID(I1_WUEPS_PID_GMM)
    DEFINE_PID(I1_WUEPS_PID_MMA)
    DEFINE_PID(I1_WUEPS_PID_CC)
    DEFINE_PID(I1_WUEPS_PID_SS)
    DEFINE_PID(I1_WUEPS_PID_TC)
    DEFINE_PID(I1_WUEPS_PID_SMS)
    DEFINE_PID(I1_WUEPS_PID_RABM)
    DEFINE_PID(I1_WUEPS_PID_SM)
    DEFINE_PID(I1_WUEPS_PID_ADMIN)
    DEFINE_PID(I1_WUEPS_PID_TAF)
    DEFINE_PID(I1_WUEPS_PID_VC)
    DEFINE_PID(I1_WUEPS_PID_DRV_AGENT)
    DEFINE_PID(I1_UEPS_PID_MTA)
    DEFINE_PID(I1_DSP_PID_GPHY)
    DEFINE_PID(I1_DSP_PID_DRX)
    DEFINE_PID(I1_DSP_PID_IDLE)
    DEFINE_PID(I1_DSP_PID_APM )
    DEFINE_PID(I1_WUEPS_PID_SLEEP)

/* 卡2的PID定义不能超过98 */
    DEFINE_PID(PC_PID_TOOLRFU = 99)               /* = 99  工具预留 */
    DEFINE_PID(PC_PID_TOOL = 100)                 /* = 100 工具预留 */

/*卡0的PID定义*/
    DEFINE_PID(I0_WUEPS_PID_USIM)
    DEFINE_PID(I0_MAPS_STK_PID)
    DEFINE_PID(I0_MAPS_PIH_PID)
    DEFINE_PID(I0_MAPS_PB_PID)
    DEFINE_PID(WUEPS_PID_OM)
    DEFINE_PID(NVIM_WRITE_DATA_PID)
    DEFINE_PID(WUEPS_PID_AGING)
    DEFINE_PID(WUEPS_PID_DIAG)
    DEFINE_PID(WUEPS_PID_RFA)
    DEFINE_PID(WUEPS_PID_REG)
    DEFINE_PID(MSP_SYS_SYM_PID)
    DEFINE_PID(MSP_DS_MUX_PID)
    DEFINE_PID(MSP_PID_DIAG_AGENT)
    DEFINE_PID(MSP_PID_HAL_DRA)
    DEFINE_PID(MSP_L4_L4A_PID)
    DEFINE_PID(MSP_L4_AT_FW_PID)
    DEFINE_PID(MSP_SYS_FTM_PID)
    DEFINE_PID(MSP_PID_NVIM_MAIN)
    DEFINE_PID(MSP_L4_RAI_PID)
    DEFINE_PID(MSP_L4_AT_LTE_PID)
    DEFINE_PID(MSP_L4_AT_GU_PID)
    DEFINE_PID(UEPS_PID_GHPA)
    DEFINE_PID(WUEPS_PID_HPA)
    DEFINE_PID(UEPS_PID_SHPA)
    DEFINE_PID(UEPS_PID_GHPA_NPP)
    DEFINE_PID(UEPS_PID_APM)
    DEFINE_PID(WUEPS_PID_OM_CALLBACK)
    DEFINE_PID(I0_UEPS_PID_GAS)
    DEFINE_PID(I0_UEPS_PID_DL)
    DEFINE_PID(I0_UEPS_PID_GRM)
    DEFINE_PID(I0_UEPS_PID_LL)
    DEFINE_PID(I0_UEPS_PID_SN)
    DEFINE_PID(WUEPS_PID_WRR)               /* =107, 6B */
    DEFINE_PID(WUEPS_PID_WCOM)              /* =108, 6C */
    DEFINE_PID(UEPS_PID_CST)                /* =109, 6D */
    DEFINE_PID(WUEPS_PID_BMC)               /* =110, 6E */
    DEFINE_PID(WUEPS_PID_PDCP)
    DEFINE_PID(WUEPS_PID_RLC)
    DEFINE_PID(WUEPS_PID_MAC)
    DEFINE_PID(I0_WUEPS_PID_RABM)
    DEFINE_PID(I0_WUEPS_PID_MMC)
    DEFINE_PID(I0_WUEPS_PID_MM )
    DEFINE_PID(I0_WUEPS_PID_GMM)
    DEFINE_PID(I0_WUEPS_PID_CC)
    DEFINE_PID(I0_WUEPS_PID_SM)
    DEFINE_PID(I0_WUEPS_PID_SMS)
    DEFINE_PID(I0_WUEPS_PID_SS)
    DEFINE_PID(I0_WUEPS_PID_TC)
    DEFINE_PID(WUEPS_PID_SMT)
    DEFINE_PID(I0_WUEPS_PID_ADMIN)
    DEFINE_PID(WUEPS_PID_MUX)
    DEFINE_PID(I0_WUEPS_PID_TAF)
    DEFINE_PID(WUEPS_PID_PPP)       /* unused after Porting */
    DEFINE_PID(WUEAPP_PID_MUX)
    DEFINE_PID(CCPU_PID_AT)
    DEFINE_PID(WUEPS_PID_ETHERSTUB) /* unused after Porting */
    DEFINE_PID(I0_WUEPS_PID_MMA)               /* =138, 8A */
    DEFINE_PID(I0_WUEPS_PID_SLEEP)     /* unused after Porting */
    DEFINE_PID(WUEPS_PID_CBS)      /* unused after Porting */         /* =142, 8E */
    DEFINE_PID(WUEPS_PID_MONITOR)
    DEFINE_PID(WUEPS_PID_NDIS)      /* unused after Porting */           /* =148 */
    DEFINE_PID(WUEPS_PID_NDIS_DL)   /* unused after Porting */          /* =149 */
    DEFINE_PID(WUEPS_PID_APCID1)    /* unused after Porting */             /* =150 */
    DEFINE_PID(WUEPS_PID_SPY)
    DEFINE_PID(I0_WUEPS_PID_VC)
    DEFINE_PID(WUEPS_PID_DFS)
    DEFINE_PID(WUEPS_PID_APCID2)    /* unused after Porting */
    DEFINE_PID(WUEPS_PID_APCID3)    /* unused after Porting */
    DEFINE_PID(WUEPS_PID_GNA)       /* unused after Porting */
    DEFINE_PID(WUEPS_PID_GNA_UL)    /* unused after Porting */
    DEFINE_PID(PS_PID_ERRC)
    DEFINE_PID(PS_PID_ERMM)
    DEFINE_PID(PS_PID_MM)
    DEFINE_PID(PS_PID_ESM)
    DEFINE_PID(PS_PID_RABM)
    DEFINE_PID(PS_PID_TC)
    DEFINE_PID(PS_PID_IP)
    DEFINE_PID(PS_PID_OM)
    DEFINE_PID(PS_PID_MAC_UL)
    DEFINE_PID(PS_PID_RLC_UL)
    DEFINE_PID(PS_PID_MAC_DL)
    DEFINE_PID(PS_PID_RLC_DL)
    DEFINE_PID(PS_PID_PDCP_UL)
    DEFINE_PID(PS_PID_PDCP_DL)
    DEFINE_PID(PS_PID_HPA)
    DEFINE_PID(PS_PID_MUX)
    DEFINE_PID(PS_PID_MUXOM)
    DEFINE_PID(CCPU_PID_OMAGENT)
    DEFINE_PID(UEPS_PID_FLOWCTRL_C)
    DEFINE_PID(WUEPS_PID_MACDL)
    DEFINE_PID(WUEPS_PID_RLCDL)
    DEFINE_PID(WUEPS_PID_NOSIG)
    DEFINE_PID(UEPS_PID_DICC_C)
    DEFINE_PID(I0_WUEPS_PID_DRV_AGENT)
    /* added by hujianbo for PTT 20111201 begin*/
    DEFINE_PID(PS_PID_PTT)
    /* added by hujianbo for PTT 20111201 end*/

#if (VOS_WIN32 == VOS_OS_VER)
    DEFINE_PID(WUEPS_PID_ITTSTUB1)
    DEFINE_PID(WUEPS_PID_ITTSTUB2)
    DEFINE_PID(WUEPS_PID_WTTFTEST)
    DEFINE_PID(WUEPS_PID_OAMTEST)
    DEFINE_PID(WUEPS_PID_XMLDECODE)
#endif
#ifdef HPA_ITT
    DEFINE_PID(MAPS_STUB_RCVMSG_PID)
    DEFINE_PID(MAPS_STUB_SENDMSG_PID)
    DEFINE_PID(MAPS_STUB_GTRMSG_PROC_PID)
#endif
    DEFINE_PID(UEPS_PID_TTF_MEM_RB_FREE)
    DEFINE_PID(UEPS_PID_CDS)
    DEFINE_PID(UEPS_PID_NDCLIENT)
    DEFINE_PID(WUEPS_PID_SAR)


    /* DSP 上移 */
    DEFINE_PID(DSP_PID_GU_RESERVED)
    DEFINE_PID(I0_DSP_PID_APM )
    DEFINE_PID(DSP_PID_UPA)
    DEFINE_PID(DSP_PID_WPHY)
    DEFINE_PID(DSP_PID_WMEAS)
    DEFINE_PID(DSP_PID_WBUTT = DSP_PID_WMEAS)
    DEFINE_PID(I0_DSP_PID_GPHY)
    DEFINE_PID(DSP_PID_GTR)
    DEFINE_PID(I0_DSP_PID_DRX)
    DEFINE_PID(I0_DSP_PID_IDLE)
    /* DSP 上移 end */

    DEFINE_PID(I0_UEPS_PID_MTA)

    DEFINE_PID(WUEPS_PID_NVIM_FLUSH)

    /* TDS begin */
    DEFINE_PID(HL1_PID_SNDCMD)
    DEFINE_PID(HL1_PID_RCVRSPPWR)
    DEFINE_PID(HL1_PID_RCVRSPUNPWR)
    DEFINE_PID(TPS_PID_RLC)
    DEFINE_PID(TPS_PID_MAC)
    DEFINE_PID(TPS_PID_PDC)
    /* TDS end */
    DEFINE_PID(TPS_PID_RRC)
    DEFINE_PID(MSP_PID_BBP_AGENT)
    DEFINE_PID(UEPS_PID_MTC)


    DEFINE_PID(WUEPS_PID_CBPCA)
    DEFINE_PID(WUEPS_PID_CSIMA)

    /*added by sunbing for VoLTE 20130715 begin*/
    DEFINE_PID(PS_PID_IMSA)
    DEFINE_PID(PS_PID_IMSVA)
    /*added by sunbing for VoLTE 20130715 end*/
    /* add for VoLTE start */
    DEFINE_PID(UEPS_PID_IPS_CCORE)
    /* add for VoLTE end */
    DEFINE_PID(MSP_PID_DRX)

    DEFINE_PID(UEPS_PID_RRM)
    /*add by fuxin00221597 for volte*/
    DEFINE_PID(VOLTE_PID_DIAG)

    DEFINE_PID(WUEPS_PID_CMMCA)

END_CPU_ID_0_PID_DEFINITION()

/* ACPU PID start */
BEGIN_CPU_ID_1_PID_DEFINITION()
    DEFINE_PID(MSP_APP_DS_MUX_PID)
    DEFINE_PID(PS_PID_APP_NDIS)
    DEFINE_PID(PS_PID_APP_PPP)
    DEFINE_PID(ACPU_PID_OM)
    DEFINE_PID(MSP_APP_DS_DIAG_PID)
    DEFINE_PID(ACPU_PID_PCSC)
    DEFINE_PID(ACPU_PID_OM_CALLBACK)
    DEFINE_PID(ACPU_PID_PCVOICE)
    DEFINE_PID(ACPU_PID_DIAG)
    DEFINE_PID(ACPU_PID_PB)
    DEFINE_PID(ACPU_PID_OMAGENT)
    DEFINE_PID(UEPS_PID_DICC_A)
    DEFINE_PID(ACPU_PID_RNIC)
    DEFINE_PID(WUEPS_PID_AT)
    DEFINE_PID(ACPU_PID_NFEXT)
    DEFINE_PID(ACPU_PID_ADS_UL)
    DEFINE_PID(ACPU_PID_ADS_DL)
    DEFINE_PID(ACPU_PID_CSD)
    DEFINE_PID(ACPU_PID_TAFAGENT)
    DEFINE_PID(UEPS_PID_FLOWCTRL_A)
    DEFINE_PID(ACPU_PID_CPULOAD)
    DEFINE_PID(UEPS_PID_IMM_RB_FREE)
    DEFINE_PID(UEPS_PID_NDSERVER)

    DEFINE_PID(PS_PID_APP_DIPC)
    DEFINE_PID(MSP_PID_DIAG_FW)
    DEFINE_PID(MSP_PID_DIAG_APP_AGENT)


    DEFINE_PID(PS_PID_APP_MUX)

    /* Voice in ACPU start */
    DEFINE_PID(DSP_PID_VOICE_RT_ACPU)
    DEFINE_PID(DSP_PID_VOICE_ACPU)
    /* Voice in ACPU end */

END_CPU_ID_1_PID_DEFINITION()
/* ACPU PID end */

/* GU DSP 的PID 定义 */
BEGIN_CPU_ID_2_PID_DEFINITION()

END_CPU_ID_2_PID_DEFINITION()



BEGIN_CPU_ID_3_PID_DEFINITION()
    DEFINE_PID(DSP_PID_VOICE_RESERVED)
    DEFINE_PID(DSP_PID_VOICE_RT_HIFI)
    DEFINE_PID(DSP_PID_VOICE_HIFI)
    DEFINE_PID(DSP_PID_AUDIO_RT)
    DEFINE_PID(DSP_PID_AUDIO)
    DEFINE_PID(DSP_PID_HIFI_OM)
END_CPU_ID_3_PID_DEFINITION()

#if (FEATURE_VOICE_UP == FEATURE_ON)
/* 语音软件上移时, 使用ACPU的PID */
#define DSP_PID_VOICE_RT    DSP_PID_VOICE_RT_ACPU
#define DSP_PID_VOICE       DSP_PID_VOICE_ACPU
#else
/* 语音软件不上移时, 使用HIFI的PID */
#define DSP_PID_VOICE_RT    DSP_PID_VOICE_RT_HIFI
#define DSP_PID_VOICE       DSP_PID_VOICE_HIFI
#endif

/* MED HiFi的PID 定义，为保证其它组件编译通过，暂时保留MED_RT等PID */
#define DSP_PID_MED_RT  DSP_PID_VOICE_RT
#define DSP_PID_MED     DSP_PID_VOICE
#define DSP_PID_MED_OM  DSP_PID_HIFI_OM


/* MCU的PID 定义 */
BEGIN_CPU_ID_4_PID_DEFINITION()
    DEFINE_PID(ACPU_PID_MCU_OM)
END_CPU_ID_4_PID_DEFINITION()

/* BBE16的PID */
BEGIN_CPU_ID_5_PID_DEFINITION()
    DEFINE_PID(DSP_PID_TDPHY_RESERVED)
    DEFINE_PID(DSP_PID_TDPHY)
END_CPU_ID_5_PID_DEFINITION()


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
#if defined (INSTANCE_1)
#define WUEPS_PID_USIM  I1_WUEPS_PID_USIM
#define MAPS_STK_PID    I1_MAPS_STK_PID
#define MAPS_PIH_PID    I1_MAPS_PIH_PID
#define MAPS_PB_PID     I1_MAPS_PB_PID
#define WUEPS_PID_SLEEP I1_WUEPS_PID_SLEEP

#define UEPS_PID_DL     I1_UEPS_PID_DL
#define UEPS_PID_LL     I1_UEPS_PID_LL
#define UEPS_PID_SN     I1_UEPS_PID_SN
#define UEPS_PID_GRM    I1_UEPS_PID_GRM

#define DSP_PID_GPHY    I1_DSP_PID_GPHY
#define DSP_PID_DRX     I1_DSP_PID_DRX
#define DSP_PID_IDLE    I1_DSP_PID_IDLE
#define DSP_PID_APM     I1_DSP_PID_APM

#define UEPS_PID_GAS    I1_UEPS_PID_GAS

#define WUEPS_PID_MM    I1_WUEPS_PID_MM
#define WUEPS_PID_MMC   I1_WUEPS_PID_MMC
#define WUEPS_PID_GMM   I1_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I1_WUEPS_PID_MMA
#define WUEPS_PID_CC    I1_WUEPS_PID_CC
#define WUEPS_PID_SS    I1_WUEPS_PID_SS
#define WUEPS_PID_TC    I1_WUEPS_PID_TC
#define WUEPS_PID_SMS   I1_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I1_WUEPS_PID_RABM
#define WUEPS_PID_SM    I1_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I1_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I1_WUEPS_PID_TAF
#define WUEPS_PID_VC    I1_WUEPS_PID_VC
#define UEPS_PID_MTA    I1_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I1_WUEPS_PID_DRV_AGENT
#else
#define WUEPS_PID_USIM  I0_WUEPS_PID_USIM
#define MAPS_STK_PID    I0_MAPS_STK_PID
#define MAPS_PIH_PID    I0_MAPS_PIH_PID
#define MAPS_PB_PID     I0_MAPS_PB_PID
#define WUEPS_PID_SLEEP I0_WUEPS_PID_SLEEP

#define UEPS_PID_GAS    I0_UEPS_PID_GAS

#define UEPS_PID_DL     I0_UEPS_PID_DL
#define UEPS_PID_LL     I0_UEPS_PID_LL
#define UEPS_PID_SN     I0_UEPS_PID_SN
#define UEPS_PID_GRM    I0_UEPS_PID_GRM

#define DSP_PID_GPHY    I0_DSP_PID_GPHY
#define DSP_PID_DRX     I0_DSP_PID_DRX
#define DSP_PID_IDLE    I0_DSP_PID_IDLE
#define DSP_PID_APM     I0_DSP_PID_APM

#define WUEPS_PID_MM    I0_WUEPS_PID_MM
#define WUEPS_PID_MMC   I0_WUEPS_PID_MMC
#define WUEPS_PID_GMM   I0_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I0_WUEPS_PID_MMA
#define WUEPS_PID_CC    I0_WUEPS_PID_CC
#define WUEPS_PID_SS    I0_WUEPS_PID_SS
#define WUEPS_PID_TC    I0_WUEPS_PID_TC
#define WUEPS_PID_SMS   I0_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I0_WUEPS_PID_RABM
#define WUEPS_PID_SM    I0_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I0_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I0_WUEPS_PID_TAF
#define WUEPS_PID_VC    I0_WUEPS_PID_VC
#define UEPS_PID_MTA    I0_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I0_WUEPS_PID_DRV_AGENT
#endif


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_PID_DEF_H */
