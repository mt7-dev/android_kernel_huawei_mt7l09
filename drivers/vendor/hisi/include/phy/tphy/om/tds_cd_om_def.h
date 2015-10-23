
#ifndef __TDS_CD_OM_DEF_H__
#define __TDS_CD_OM_DEF_H__

#ifdef __cplusplus
#if __cplusplus
        extern "C"{
#endif
#endif /* __cplusplus */
#include "balong_lte_dsp.h"


/************************************************************
                           1. REQ命令和对应的数据结构定义
************************************************************/
//#define TPHY_REQ_CD_OM_REP_SWITCH    OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x0)
typedef enum
{
    TPHY_REQ_CD_OM_REP_DPCH             = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x0),
    TPHY_REQ_CD_OM_REP_DEC              = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x1),
    TPHY_REQ_CD_OM_REP_PCCPCH           = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x2),
    TPHY_REQ_CD_OM_REP_FPACH            = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x3),
    TPHY_REQ_CD_OM_REP_SCCPCH           = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x4),
    TPHY_REQ_CD_OM_REP_DSCH             = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_REQ, 0x5),

}TPHY_CD_OM_REQ_ENUM;


/************************************************************
                             枚举定义
************************************************************/

typedef enum TDS_CD_OM_RPT_DSCH_IND
{
    TPHY_IND_CD_SOFTBIT_INPUT_DSCH      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x00),
    TPHY_IND_CD_HARQ_PARA_INPUT_DSCH    = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x01),
    TPHY_IND_CD_TURBO_PARA_INPUT_DSCH   = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x02),
    TPHY_IND_CD_TURBO_BIT_OUTPUT_DSCH   = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x03),
    TPHY_IND_CD_TURBO_CRC_DSCH          = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x04),
    TPHY_IND_CD_PARA_DSCH               = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x05),

}TDS_CD_OM_RPT_DSCH_IND_ENUM;

typedef enum TDS_CD_OM_RPT_DPCH_IND
{
    TPHY_IND_CD_SOFTBIT_INPUT_DPCH      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x10),
    TPHY_IND_CD_HARQ_DATA_INPUT_DPCH    = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x11),
    TPHY_IND_CD_HARQ_PARA_INPUT_DPCH    = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x12),
    TPHY_IND_CD_HARQ_DATA_OUTPUT_DPCH   = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x13),
    TPHY_IND_CD_TFCI_DATA_DPCH          = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x14),
    TPHY_IND_CD_TFCI_DPCH               = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x15),
    TPHY_IND_CD_SB_FLAG_DPCH            = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x16),
    TPHY_IND_CD_CDTABLE                 = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x17),

}TDS_CD_OM_RPT_DPCH_IND_ENUM;

typedef enum TDS_CD_OM_RPT_DEC_IND
{
    TPHY_IND_CD_VIT_PARA_INPUT      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x20),
    TPHY_IND_CD_VIT_DATA_INPUT      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x21),
    TPHY_IND_CD_VIT_DATA_OUTPUT     = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x22),
    TPHY_IND_CD_VIT_CRC             = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x23),

    TPHY_IND_CD_TURBO_PARA_INPUT    = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x24),
    TPHY_IND_CD_TURBO_DATA_INPUT    = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x25),
    TPHY_IND_CD_TURBO_DATA_OUTPUT   = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x26),
    TPHY_IND_CD_TURBO_CRC           = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x27),
}TDS_CD_OM_RPT_DEC_IND_ENUM;

typedef enum TDS_CD_OM_RPT_PCCPCH_IND
{
    TPHY_IND_CD_SOFTBIT_INPUT_PCCPCH      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x30),

}TDS_CD_OM_RPT_PCCPCH_IND_ENUM;

typedef enum TDS_CD_OM_RPT_FPACH_IND
{
    TPHY_IND_CD_SOFTBIT_INPUT_FPACH      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x40),

}TDS_CD_OM_RPT_FPACH_IND_ENUM;

typedef enum TDS_CD_OM_RPT_SCCPCH_IND
{
    TPHY_IND_CD_SOFTBIT_INPUT_SCCPCH      = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x50),

}TDS_CD_OM_RPT_SCCPCH_IND_ENUM;

typedef enum TDS_CD_OM_RPT_BLER_IND
{
    TPHY_IND_CD_DPA_BLER                  = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x90),
    TPHY_IND_VOICE_TX                     = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x91),
    TPHY_IND_VOICE_RX                     = OM_TCMD_ID(TPHY_CD_MID, OM_TYPE_IND, 0x92),
}TDS_CD_OM_RPT_BLER_IND_ENUM;

/************************************************************
                             结构体
************************************************************/

typedef struct TDS_CD_OM_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitInRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitBbpRptEnb;
}TDS_CD_OM_REQ_CTRL_STRU;

typedef struct TDS_CD_OM_BLER_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdDschBlerRptEnb;
    //TDS_SWITCH_FALG_ENUM_UINT16  usCdHarqParaInputDschRptEnb;
    //TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboParaInputDschRptEnb;
    //TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboBitOutputDschRptEnb;
    //TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboCrcDschRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usTpyVoiceRptEnb;
}TDS_CD_OM_BLER_CTRL_STRU;


typedef struct TDS_CD_OM_DSCH_IND_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitInputDschRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdHarqParaInputDschRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboParaInputDschRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboBitOutputDschRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboCrcDschRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdDschParaRptEnb;
}TDS_CD_OM_DSCH_IND_CTRL_STRU;

typedef struct TDS_CD_OM_DPCH_IND_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitInputDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdHarqDataInputDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdHarqParaInputDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdHarqDataOutputDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTfciDataDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTfciDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSbFlagDpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdDpchCdTableRptEnb;

}TDS_CD_OM_DPCH_IND_CTRL_STRU;

typedef struct TDS_CD_OM_DEC_IND_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdVitParaInputRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdVitDataInputRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdVitDataOutputRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdVitCrcRptEnb;

    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboParaInputRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboDataInputRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboDataOutputRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usCdTurboCrcRptEnb;
}TDS_CD_OM_DEC_IND_CTRL_STRU;

typedef struct TDS_CD_OM_PCCPCH_IND_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitInputPccpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usreserved;
}TDS_CD_OM_PCCPCH_IND_CTRL_STRU;

typedef struct TDS_CD_OM_FPACH_IND_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitInputFpachRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usreserved;
}TDS_CD_OM_FPACH_IND_CTRL_STRU;

typedef struct TDS_CD_OM_SCCPCH_IND_RPT_CTRL
{
    TDS_SWITCH_FALG_ENUM_UINT16  usCdSoftBitInputSccpchRptEnb;
    TDS_SWITCH_FALG_ENUM_UINT16  usreserved;
}TDS_CD_OM_SCCPCH_IND_CTRL_STRU;


typedef struct
{
    INT8  acCdSoftBit[16000];
} TPHY_CD_OM_SOFTBIT_STRU;

typedef struct
{
    INT8  cCdSoftBit;
} TPHY_CD_OM_ONE_SOFTBIT_STRU;

typedef struct
{
    UINT32  alCdWords[5000];
} TPHY_CD_OM_WORD_STRU;

typedef struct
{
    UINT32  alCdWord;
} TPHY_CD_OM_ONE_WORD_STRU;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

/******************************** end *************************************/

