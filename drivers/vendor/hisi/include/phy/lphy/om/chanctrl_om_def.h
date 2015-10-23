

/************************************************************
                     包含其它模块的头文件
************************************************************/

/************************************************************
                               宏定义
************************************************************/

/************************************************************
                             数据结构定义
************************************************************/

/************************************************************
                             接口函数声明
 ************************************************************/

#ifndef __CHANCTRL_OM_DEF_H__
#define __CHANCTRL_OM_DEF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



/************************************************************
                    1. REQ命令和对应的数据结构定义
************************************************************/
typedef enum
{
    LPHY_REQ_CHANCTRL_TM_REQUEST           = OM_CMD_ID(LPHY_CHANCTRL_MID, OM_TYPE_REQ, 0x1),
    LPHY_REQ_CHANCTRL_CP_REQUEST,
    /* BEGIN: Added by yushujing, 2013/8/28   PN:chan_om*/
    LPHY_REQ_CHANCTRL_CELL_STAT_REQUEST,
    /* END:   Added by yushujing, 2013/8/28 */
}LPHY_CHANCTRL_REQ_ENUM;

typedef UINT16 LPHY_CHANCTRL_REQ_ENUM_UINT16;


typedef struct
{
    OM_REQ_ENABLE_ENUM enOmTmSwitch;
    OM_REQ_ENABLE_ENUM enOmCpSwitch;
}LPHY_OM_REQ_CHANCTRL_STRU;
/* BEGIN: Added by yushujing, 2013/8/28   PN:chan_om*/
typedef struct
{
    OM_REQ_ENABLE_ENUM enRptChanEna;
    UINT32 ulRptChanPeriod;
}LPHY_OM_REQ_CHANCTRL_STAT_ENA_STRU;
typedef struct
{
    LPHY_OM_REQ_CHANCTRL_STAT_ENA_STRU stRptChanEna;
    UINT32 ulRptPeriodCnt;
}LPHY_OM_REQ_CHANCTRL_STAT_STRU;
/* END:   Added by yushujing, 2013/8/28 */
/************************************************************
                    2. CNF数据结构定义
************************************************************/


/************************************************************
                    3. Ind数据结构定义
************************************************************/
typedef enum
{
    LPHY_IND_CHANCTRL_TM_REPORT           = OM_CMD_ID(LPHY_CHANCTRL_MID, OM_TYPE_IND, 0x1),
    LPHY_IND_CHANCTRL_CP_REPORT,
    /* BEGIN: Added by yushujing, 2013/8/28   PN:chan_om*/
    LPHY_IND_CHANCTRL_CELL_STAT,
    /* END:   Added by yushujing, 2013/8/28   PN:chan_om*/
}LPHY_CHANCTRL_IND_ENUM;

typedef UINT16 LPHY_CHANCTRL_IND_ENUM_UINT16;

typedef struct
{
    UINT32      ulData1;
    UINT32      ulData2;
    UINT32      ulData3;
    UINT32      ulData4;
}LPHY_OM_IND_CHANCTRL_STRU;
/* BEGIN: Added by yushujing, 2013/8/28   PN:chan_om*/
enum LPHY_CHANCTRL_BAND_WIDTH_ENUM
{
    LPHY_BAND_WIDTH_6RB              = 0,                /* 带宽为6RB */
    LPHY_BAND_WIDTH_15RB,                                /* 带宽为15RB */
    LPHY_BAND_WIDTH_25RB,                                /* 带宽为25RB */
    LPHY_BAND_WIDTH_50RB,                                /* 带宽为50RB */
    LPHY_BAND_WIDTH_75RB,                                /* 带宽为75RB */
    LPHY_BAND_WIDTH_100RB,                               /* 带宽为100RB */
    LPHY_BAND_WIDTH_BUTT
};
typedef UINT8 LPHY_CHANCTRL_BAND_WIDTH_ENUM_UINT8;

enum LPHY_CHANCTRL_LTE_MODE_TYPE_ENUM
{
    LPHY_LTE_MODE_TYPE_FDD                = 0,                             /* LTE FDD 制式 */
    LPHY_LTE_MODE_TYPE_TDD                = 1,                             /* LTE TDD 制式 */
    LPHY_LTE_MODE_TYPE_BUTT
};
typedef UINT8 LPHY_CHANCTRL_LTE_MODE_TYPE_ENUM_UINT8;

enum LPHY_CHANCTRL_CONTINUOUS_CA_ENUM
{
    LPHY_NON_CONTINUOUS_CA                = 0,                             /* LTE FDD 制式 */
    LPHY_CONTINUOUS_CA                    = 1,                             /* LTE TDD 制式 */
    LPHY_CA_TYPE_BUTT
};
typedef UINT8 LPHY_CHANCTRL_CONTINUOUS_CA_ENUM_UINT8;

enum LPHY_CHANCTRL_CROSS_CA_ENUM
{
    LPHY_NON_CROSS_CA                = 0,                             /* LTE FDD 制式 */
    LPHY_CROSS_CA                    = 1,                             /* LTE TDD 制式 */
    LPHY_CROSS_CA_TYPE_BUTT
};
typedef UINT8 LPHY_CHANCTRL_CROSS_CA_ENUM_UINT8;

typedef struct
{
    UINT16 ucCellId;
    UINT16 usFreqInfo;

    UINT8 ucBandInd;
    LPHY_CHANCTRL_BAND_WIDTH_ENUM_UINT8 ucBandWidth;
    LPHY_CHANCTRL_LTE_MODE_TYPE_ENUM_UINT8 ucLteModeType;
    UINT8 ucTmMode;

    UINT8 ucCellIndex;
    UINT8 ucRsv;
    UINT16 usRsv;
}LPHY_OM_CHANCTRL_CELL_STAT_STRU;

typedef struct
{
    LPHY_CHANCTRL_CROSS_CA_ENUM_UINT8 ucCrossCa;
    UINT8 ucScellCifPresent;
    UINT8 ucPdschStart;
    LPHY_CHANCTRL_CONTINUOUS_CA_ENUM_UINT8 ucContiCa;
}LPHY_OM_CHANCTRL_CELL_CA_STAT_STRU;

typedef struct
{
    LPHY_OM_CHANCTRL_CELL_STAT_STRU astCellStat[2];
    LPHY_OM_CHANCTRL_CELL_CA_STAT_STRU stCaStat;
}LPHY_OM_IND_CHANCTRL_STAT_STRU;
/* END:   Added by yushujing, 2013/8/28   PN:chan_om*/
/************************************************************
                    4. TRACE数据结构定义
************************************************************/
typedef enum
{
    LPHY_TRACE_CHACNTRL_PBCH_SETUP           = OM_CMD_ID(LPHY_CHANCTRL_MID, OM_TYPE_TRACE, 0x1),
    LPHY_TRACE_CHACNTRL_PDSCH_BCCH_SETUP,
    LPHY_TRACE_CHACNTRL_PDSCH_PCH_SETUP,
    LPHY_TRACE_CHANCTRL_PDSCH_RACH_SETUP,
    LPHY_TRACE_CHANCTRL_PDSCH_DSCH_SETUP,
    LPHY_TRACE_CHANCTRL_SCELL_PDSCH_DSCH_SETUP,
    LPHY_TRACE_CHANCTRL_PBCH_RELEASE,
    LPHY_TRACE_CHANCTRL_PDSCH_BCCH_RELEASE,
    LPHY_TRACE_CHANCTRL_PDSCH_PCH_RELEASE,
    LPHY_TRACE_CHANCTRL_PDSCH_RACH_RELEASE,
    LPHY_TRACE_CHANCTRL_PDSCH_DSCH_RELEASE,
    LPHY_TRACE_CHANCTRL_SCELL_PDSCH_DSCH_RELEASE,
    LPHY_TRACE_CHACNTRL_CAMPED_ON_MAIN_CELL,
    LPHY_TRACE_CHANCTRL_HANDOVER,
    LPHY_TRACE_CHANCTRL_SI_CONFIG,
    LPHY_TRACE_CHANCTRL_C_RNTI_CONFIG,
    LPHY_TRACE_CHANCTRL_TC_RNTI_CONFIG,
    LPHY_TRACE_CHANCTRL_TC_RNTI_TO_C_RNTI,
    LPHY_TRACE_CHANCTRL_PCH_STOP_BY_NC_BCH,
    /* BEGIN: Added by yushujing, 2012/1/5   PN:ANR_MOD*/
    LPHY_TRACE_CHANCTRL_ANR_READY,
    LPHY_TRACE_CHANCTRL_ANR_START,
    LPHY_TRACE_CHANCTRL_ANR_HOLD,
    LPHY_TRACE_CHANCTRL_ANR_TIME_OK,
    /* END:   Added by yushujing, 2012/1/5 */
    LPHY_TRACE_CHANCTRL_BLIND_HANDOVER,
    LPHY_TRACE_CHANCTRL_RELEASE_ALL,
    LPHY_TRACE_CHANCTRL_CHANNEL_RECONFIG,
    LPHY_TRACE_CHANCTRL_INITIAL_CELL_PBCH_SETUP,
    LPHY_TRACE_CHANCTRL_NEIGHBOUR_CELL_PBCH_SETUP,
    LPHY_TRACE_CHANCTRL_MAINCELL_PBCH_SETUP,
    LPHY_TRACE_CHANCTRL_INITIAL_CELL_BCCH_SETUP,
    LPHY_TRACE_CHANCTRL_NEIGHBOUR_CELL_BCCH_SETUP,
    LPHY_TRACE_CHANCTRL_MAINCELL_BCCH_SETUP,
    LPHY_TRACE_CHANCTRL_TRANSMISSION_MODE_UPDATA,
    LPHY_TRACE_CHANCTRL_CALLING_RFE_CHANGE,
    /* END:   Added by sunyanjie, 2012/3/20 */
    /* BEGIN: Added by yushujing, 2013/8/29   PN:chan_om*/
    LPHY_TRACE_CHANCTRL_SCELL_ACT,
    LPHY_TRACE_CHANCTRL_SCELL_INACT,
    /* END:   Added by yushujing, 2013/8/29   PN:chan_om*/

}LPHY_CHANCTRL_TRACE_ENUM;

typedef UINT16 LPHY_CHANCTRL_TRACE_ENUM_UINT16;

typedef struct
{
    UINT16      usSysFrmNum;    /*系统帧号*/
    UINT16      usSubFrmNum;    /*子帧号*/
    UINT16      usSymbNum;      /*当前符号数*/
    UINT16      usReserved;
    UINT32      ulData1;
    UINT32      ulData2;
    UINT32      ulData3;
    UINT32      ulData4;
}LPHY_OM_TRACE_CHANCTRL_STRU;

/************************************************************
                    5. ERROR数据结构定义
************************************************************/
typedef enum
{
    LPHY_ERROR_CHACNTRL_UL_MAILBOX_FEEDBACK_FAIL         = OM_CMD_ID(LPHY_CHANCTRL_MID, OM_TYPE_ERROR, 0x1),
    LPHY_ERROR_CHACNTRL_SEARCH_CELL_FROM_CSLIST_FAIL,
    LPHY_ERROR_CHACNTRL_SERVE_CELL_STATE_ERROR,
    LPHY_ERROR_CHANCTRL_SETUP_TOO_MANY_CHANNELS,
    LPHY_ERROR_CHANCTRL_CANNOT_FIND_THE_SPECIAL_CHANNEL,
    LPHY_ERROR_CHANCTRL_TPC_RNTI_TYPE_ERROR,
    LPHY_ERROR_CHANCTRL_PRIMITIVE_PARA_ERROR,
    LPHY_ERROR_CHANCTRL_TIMING_START_ERROR,
    LPHY_ERROR_CHANCTRL_TIMING_STOP_ERROR,
    LPHY_ERROR_CHANCTRL_GET_TIMING_CELL_ERROR,
    LPHY_ERROR_CHANCTRL_GET_TIMING_NOTREADY_ERROR,
    LPHY_ERROR_CHANCTRL_BGS_PDS_BCH_CHANNEL_SETUP,
    LPHY_ERROR_CHANCTRL_BGS_PBCH_CHANNEL_SETUP,
    LPHY_ERROR_CHANCTRL_BGS_PCH_CHANNEL_SETUP


}LPHY_CHANCTRL_ERROR_ENUM;

typedef UINT16 LPHY_CHANCTRL_ERROR_ENUM_UINT16;

typedef struct
{
    UINT16      usSysFrmNum;    /*系统帧号*/
    UINT16      usSubFrmNum;    /*子帧号*/
    UINT16      usSymbNum;      /*当前符号数*/
    UINT16      usReserved;
    UINT32      ulData1;
    UINT32      ulData2;
    UINT32      ulData3;
    UINT32      ulData4;
}LPHY_OM_ERROR_CHANCTRL_STRU;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CHANCTRL_OM_DEF_H__ */

