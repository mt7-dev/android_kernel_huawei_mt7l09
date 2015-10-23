#ifndef __IMS_PARM_PARM_H__
#define __IMS_PARM_PARM_H__

#ifndef HW_VOLTE
#include "vos.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HW_VOLTE
#ifndef VOS_INT8
#define VOS_INT8       char
#endif

#ifndef VOS_UINT8
#define VOS_UINT8      unsigned char
#endif

#ifndef VOS_INT16
#define VOS_INT16      short
#endif

#ifndef VOS_UINT16
#define VOS_UINT16     unsigned short
#endif

#ifndef VOS_INT32
#define VOS_INT32      long
#endif

#ifndef VOS_UINT32
#define VOS_UINT32     unsigned long
#endif
#endif
/*****************************************************************************
 * 0. Common
*****************************************************************************/
/** IMS String Parameter Size */
#define IMS_PARM_STRING_SIZE             (129)

/** Indicates IMS Parameter type */
enum IMS_PARM_TYPE_ENUM{
    IMS_PARM_TYPE_SIP,
    IMS_PARM_TYPE_VOIP,
    IMS_PARM_TYPE_CODE,
    IMS_PARM_TYPE_SS_CONF,
    IMS_PARM_TYPE_SECURITY
};
typedef VOS_UINT8 IMS_PARM_TYPE_ENUM_UINT8;

/*****************************************************************************
 * 1. SIP
*****************************************************************************/
/** Indicates SIP transport type */
enum IMS_PARM_TRANSPORT_TYPE_ENUM{
    IMS_PARM_TRANSPORT_TYPE_UDP,
    IMS_PARM_TRANSPORT_TYPE_TCP,
    IMS_PARM_TRANSPORT_TYPE_BUTT
};
typedef VOS_UINT8 IMS_PARM_TRANSPORT_TYPE_ENUM_UINT8;

/** SIP parameter definiton */
typedef struct{
    IMS_PARM_TRANSPORT_TYPE_ENUM_UINT8     ucTransportType;
    VOS_UINT8     ucHeaderCompactEnabled;
    VOS_UINT8     ucReserved[2];
    VOS_UINT32    ulServerPort;
    VOS_UINT32    ulTimerSipRegValue;
    VOS_UINT32    ulTimerT1Value;
    VOS_UINT32    ulTimerT2Value;
    VOS_UINT32    ulTimerT4Value;
    VOS_UINT32    ulTimerTFValue;
    VOS_UINT32    ulTimerTJValue;
    VOS_UINT32    ulSipSocketKeepAliveValue;
    VOS_INT8      aucUserAgent[IMS_PARM_STRING_SIZE];
    VOS_INT8      aucReserved[3];
}IMS_PARM_SIP_STRU;

/*****************************************************************************
 * 2. VOIP
*****************************************************************************/
/** Indicates Session Refresh type */
enum    IMS_PARM_SESSION_REFRESH_TYPE_ENUM
{
    IMS_PARM_SESSION_REFRESH_TYPE_UAC      = 0x00,   /* Client Refresh */
    IMS_PARM_SESSION_REFRESH_TYPE_UAS            ,   /* Server Refresh */
    IMS_PARM_SESSION_REFRESH_TYPE_BUTT
};
typedef VOS_UINT8   IMS_PARM_SESSION_REFRESH_TYPE_ENUM_UINT8;

/** Indicates Session Refresh method */
enum    IMS_PARM_SESSION_REFRESH_METHOD_ENUM
{
    IMS_PARM_SESSION_REFRESH_METHOD_UPDATE       = 0x00,   /* Update */
    IMS_PARM_SESSION_REFRESH_METHOD_REINVITE           ,   /* Reinvite */
    IMS_PARM_SESSION_REFRESH_METHOD_BUTT
};
typedef VOS_UINT8   IMS_PARM_SESSION_REFRESH_METHOD_ENUM_UINT8;

/** VOIP parameter definiton */
typedef struct{
    VOS_UINT8     ucSessionTimerEnabled;                                  /* Enabled Session Expire in Invite */
    VOS_UINT8     ucPreConditionEnabled;
    IMS_PARM_SESSION_REFRESH_TYPE_ENUM_UINT8     ucSessionRefreshType;
    IMS_PARM_SESSION_REFRESH_METHOD_ENUM_UINT8   ucSessionRefreshMethod;
    VOS_UINT16    usSessionExpires;
    VOS_UINT8     ucReserved2[2];
}IMS_PARM_VOIP_STRU;

/*****************************************************************************
 * 3. CODE
*****************************************************************************/
/** Indicates Audio Capability */
enum    IMS_PARM_AUDIO_CAP_ENUM
{
    IMS_PARM_AUDIO_CAP_AMR_NB_AND_WB           = 0x00,   /* AMR-NB and AMR-WB (AMR-NB preferred) */
    IMS_PARM_AUDIO_CAP_AMR_WB_AND_NB                 ,   /* AMR-WB and AMR-NB (AMR-WB preferred) */
    IMS_PARM_AUDIO_CAP_AMR_NB                        ,   /* AMR-NB */
    IMS_PARM_AUDIO_CAP_AMR_WB                        ,   /* AMR-WB */
    IMS_PARM_AUDIO_CAP_TYPE_BUTT
};
typedef VOS_UINT8   IMS_PARM_AUDIO_CAP_ENUM_UINT8;

/** Indicates Audio Code Mode */
enum    IMS_PARM_AUDIO_CODE_TYPE_ENUM
{
    IMS_PARM_AUDIO_CODE_MODE_OCTECT_ALIGN      = 0x00,  /* Octet-aligned Mode */
    IMS_PARM_AUDIO_CODE_MODE_BAND_EFFICIENT          ,  /* Bandwidth-Efficient Mode */
    IMS_PARM_AUDIO_CODE_TYPE_BUTT
};
typedef VOS_UINT8 IMS_PARM_AUDIO_CODE_MODE_ENUM_UINT8;

/** Code parameter definiton */
typedef struct{
    IMS_PARM_AUDIO_CAP_ENUM_UINT8       ucAudioCap;           /* Audio Capabilities */
    VOS_UINT8                           ucSetNbAmrModeFlag;   /* Enabled setting Mode set of AMR-NB */
    VOS_UINT8                           ucSetWbAmrModeFlag;   /* Enabled setting Mode set of AMR-WB */
    VOS_UINT8                           ucReserved;
    VOS_UINT32                          ulNbAmrModeSet;       /* Mode set of AMR-NB */
    VOS_UINT32                          ulWbAmrModeSet;       /* Mode set of AMR-WB */
    IMS_PARM_AUDIO_CODE_MODE_ENUM_UINT8 ucNbAmrAudioCodeMode; /* Code Mode of AMR-NB */
    IMS_PARM_AUDIO_CODE_MODE_ENUM_UINT8 ucWbAmrAudioCodeMode; /* Code Mode of AMR-WB */
    VOS_UINT16                          usPtimerValue;        /* PTime */
}IMS_PARM_VOIP_CODE_STRU;

/*****************************************************************************
 * 4. Supplementary Service
*****************************************************************************/
/** Supplementary Service parameter definiton */
typedef struct{
    VOS_INT8      ucConfFactoryURI[IMS_PARM_STRING_SIZE];     /* Conference factory URI */
    VOS_INT8      aucReserved[3];
}IMS_PARM_SS_CONF_STRU;

/*****************************************************************************
 * 5. Security
*****************************************************************************/
enum    IMS_PARM_IPSEC_EALGO_TYPE_ENUM
{
    IMS_PARM_IPSEC_EALGO_TYPE_NULL      = 0x00,       /* NULL */
    IMS_PARM_IPSEC_EALGO_TYPE_AES                  ,  /* AES */
    IMS_PARM_IPSEC_EALGO_TYPE_3DES                   ,  /* 3DES */
    IMS_PARM_IPSEC_EALGO_TYPE_BUTT
};
typedef VOS_UINT16 IMS_PARM_IPSEC_EALGO_TYPE_ENUM_UINT16;
enum    IMS_PARM_IPSEC_IALGO_TYPE_ENUM
{
    IMS_PARM_IPSEC_IALGO_TYPE_MD5      = 0x00,       /* NULL */
    IMS_PARM_IPSEC_IALGO_TYPE_SHA1                  ,  /* AES */
    IMS_PARM_IPSEC_IALGO_TYPE_BUTT
};
typedef VOS_UINT16 IMS_PARM_IPSEC_IALGO_TYPE_ENUM_UINT16;
/** Security parameter definiton */
typedef struct{
    VOS_UINT8     ucSrtpFlag;      /* SRTP enabled */
    VOS_UINT8                            aucReserved[3];
    IMS_PARM_IPSEC_EALGO_TYPE_ENUM_UINT16  usIpsecEAlgo;   /* IPSec encrypt algorithms */
    IMS_PARM_IPSEC_IALGO_TYPE_ENUM_UINT16  usIpsecIAlgo;    /* IPSec integrity algorithms */
}IMS_PARM_SECURITY_STRU;

#ifdef __cplusplus
}
#endif

#endif //__IMS_PARM_PARM_H__
