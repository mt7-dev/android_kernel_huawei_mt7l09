

/******************************************************************************
 PROJECT   :
 SUBSYSTEM :
 MODULE    :
 OWNER     :
******************************************************************************/

#include "msp_errno.h"
#include "osm.h"
#include "gen_msg.h"
#include <ftm.h>
#include "LMspLPhyInterface.h"
#include "ftm_ct.h"
#include "msp_at.h"

#include "TPsTMspInterface.h"
/*lint -e767 原因:Log打印*/
#define    THIS_FILE_ID        MSP_FILE_ID_FTM_CT_CORE_C
/*lint +e767 */

VOS_UINT32 g_ulCrntClientId;  /*lwx26324 add 20110927*/

/* FTM模块超时控制 */
FTM_CTCMD_CTRL_STRU g_ct_cmd_ctrl = {0};

static FTM_MAIN_INFO_STRU g_stFtmMainInfo = {
    NULL, /* SYM注册接口，主要是为了保证架构关系 */
    /* 普通校准默认值及运行时数据保持*/
    {
        {EN_FCHAN_MODE_FDD_LTE,FTM_AT_FCHAN_BAND,{0},0,0,ERR_MSP_SUCCESS},
        EN_FTM_TXON_CLOSE,
        EN_FTM_RXON_CLOSE,
        EN_FWAVE_TYPE_CW,
        0,
        0,
        1200, /* -150dbm */
        0,
        FTM_NONESIG_RTT_CLOSE,
        0xFF,
        0xFF
    },

    /* 快速校准默认值及运行时数据保持*/
    {
        CT_F_STEPS_DONE_NULL,
        EN_FCHAN_MODE_UNKONWN,
        255,
        TXWAVE_TYPE_CW,
        START_TRIGGER_TYPE_TX,
        0,
        20 ,0 ,1,
        0 ,0 ,0 ,0 ,0 ,0 ,0 ,
        {0},{0} ,{0},{0},{0},
        {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
        {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
		{0}, {0}, 0, 0, 0, {0}, 0, 0, 0,
        {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
     },
/*lint -e651*/
/*lint -e156*/

    /* 非信令综测参数保存 */
    {
         0 ,
		{0}, {0}, {0}, {0}, {0},
        {0}, {0}, {0}, {0}, {0},
        {0}, {0}, {0}, {0}, {0},
        {{0},}, {0}, {0}
    },
/*lint -e156*/
/*lint -e651*/
    /* 系统当前TMODE模式 */
    EN_SYM_TMODE_SIGNAL,

    /* 通用AT命令返回内容 */
    {
         ERR_MSP_SUCCESS,
         0,
         { 0 }
    },
};

/*************************************************************
   A核发送的装备消息处理表
   需要RTT确认的有超时处理函数，不需要RTT确认的没有超时函数
**************************************************************/
static FTM_EQUIP_MSG_FUNTBL_STRU g_stFtmEquipMsgTbl[] = {
    {ID_MSG_SYM_SET_TMODE_REQ,          at_ftm_tmode_set,           at_ftm_tmode_timeout        },
    {ID_MSG_SYM_RD_TMODE_REQ,           at_ftm_tmode_read,          NULL                        },
    {ID_MSG_FTM_SET_TSELRF_REQ,         at_ftm_tselrf_set,          at_ftm_tselrf_timeout       },
    {ID_MSG_FTM_SET_FCHAN_REQ,          at_ftm_fchan_set,           at_ftm_fchan_timeout        },
    {ID_MSG_FTM_RD_FCHAN_REQ,           at_ftm_fchan_read,          NULL                        },
    {ID_MSG_FTM_SET_FWAVE_REQ,          at_ftm_fwave_set,           at_ftm_fwave_timeout        },
    {ID_MSG_FTM_RD_FWAVE_REQ,           at_ftm_fwave_read,          NULL                        },
    {ID_MSG_FTM_SET_TXON_REQ,           at_ftm_ftxon_set,           at_ftm_ftxon_timeout        },
    {ID_MSG_FTM_RD_TXON_REQ,            at_ftm_ftxon_read,          NULL                        },
    {ID_MSG_FTM_SET_RXON_REQ,           at_ftm_frxon_set,           at_ftm_frxon_timeout        },
    {ID_MSG_FTM_RD_RXON_REQ,            at_ftm_frxon_read,          NULL                        },
    {ID_MSG_FTM_SET_AAGC_REQ,           at_ftm_flna_set,            at_ftm_flna_timeout         },
    {ID_MSG_FTM_RD_AAGC_REQ,            at_ftm_flna_read,           NULL                        },
    {ID_MSG_FTM_FRSSI_REQ,              at_ftm_frssi_read,          at_ftm_frssi_timeout        },
    {ID_MSG_FTM_SET_BANDSW_REQ,         at_ftm_bandsw_set,          at_ftm_bandsw_timeout       },
    {ID_MSG_FTM_RD_BANDSW_REQ,          at_ftm_bandsw_read,         NULL                        },
    {ID_MSG_FTM_SET_FVCTCXO_REQ,        at_ftm_fvctcxo_set,         at_ftm_fvctcxo_timeout      },
    {ID_MSG_FTM_RD_FVCTCXO_REQ,         at_ftm_fvctcxo_read,        NULL                        },
    {ID_MSG_FTM_F_SET_FBLK_REQ,         at_ftm_fblk_set,            at_ftm_fblk_timeout         },
    {ID_MSG_FTM_F_RD_FBLK_REQ,          at_ftm_fblk_read,           NULL                        },
    {ID_MSG_FTM_F_SET_FCHANS_REQ,       at_ftm_fchans_set,          at_ftm_fchans_timeout       },
    {ID_MSG_FTM_F_RD_FCHANS_REQ,        at_ftm_fchans_read,         NULL                        },
    {ID_MSG_FTM_F_SET_FSEGMENT_REQ,     at_ftm_fsegment_set,        at_ftm_fsegment_timeout     },
    {ID_MSG_FTM_F_RD_FSEGMENT_REQ,      at_ftm_fsegment_read,       NULL                        },
    {ID_MSG_FTM_F_SET_FPOWS_REQ,        at_ftm_fpows_set,           at_ftm_fpows_timeout        },
    {ID_MSG_FTM_F_RD_FPOWS_REQ,         at_ftm_fpows_read,          NULL                        },
    {ID_MSG_FTM_F_SET_FPAS_REQ,         at_ftm_fpas_set,            at_ftm_fpas_timeout         },
    {ID_MSG_FTM_F_RD_FPAS_REQ,          at_ftm_fpas_read,           NULL                        },
    {ID_MSG_FTM_F_SET_FLNAS_REQ,        at_ftm_flnas_set,           at_ftm_flnas_timeout        },
    {ID_MSG_FTM_F_RD_FLNAS_REQ,         at_ftm_flnas_read,          NULL                        },
    {ID_MSG_FTM_SET_FPDMS_REQ,          at_ftm_fpdms_set,           at_ftm_fpdms_timeout        },
    {ID_MSG_FTM_RD_FPDMS_REQ,           at_ftm_fpdms_read,          NULL                        },
    {ID_MSG_FTM_F_SET_FTXWAVE_REQ,      at_ftm_ftxwave_set,         at_ftm_ftxwave_timeout      },
    {ID_MSG_FTM_F_RD_FTXWAVE_REQ,       at_ftm_ftxwave_read,        NULL                        },
    {ID_MSG_FTM_F_SET_FSTART_REQ,       at_ftm_fstart_set,          at_ftm_fstart_timeout       },
    {ID_MSG_FTM_F_SET_FRSSIS_REQ,       at_ftm_frssis_set,          NULL                        },
    {ID_MSG_FTM_F_RD_FSTART_REQ,        at_ftm_fstart_read,         at_ftm_fstart_read_timeout  },
    {ID_MSG_FTM_F_RD_FCMTMS_REQ,        at_ftm_fcmtms_read,         NULL                        },
    {ID_MSG_FTM_F_SET_FMAXPOWER_REQ,    at_ftm_fmaxpower_set,       at_ftm_fmaxpower_timeout    },
    {ID_MSG_FTM_F_RD_FMAXPOWER_REQ,     at_ftm_fmaxpower_read,      NULL                        },
    {ID_MSG_FTM_F_SET_FPAPOWER_REQ,     at_ftm_fpapower_set,        at_ftm_fpapower_timeout     },
    {ID_MSG_FTM_F_RD_FPAPOWER_REQ,      at_ftm_fpapower_read,       NULL                        },
    {ID_MSG_FTM_SET_FCALIIP2S_REQ,      at_ftm_fcaliip2s_set,       at_ftm_fcaliip2s_timeout    },
    {ID_MSG_FTM_RD_FCALIIP2S_REQ,       at_ftm_fcaliip2s_read,      NULL                        },
    {ID_MSG_FTM_SET_FCALIDCOCS_REQ,     at_ftm_fcalidcocs_set,      at_ftm_fcalidcocs_timeout   },
    {ID_MSG_FTM_RD_FCALIDCOCS_REQ,      at_ftm_fcalidcocs_read,     NULL                        },
    {ID_MSG_FTM_SET_FCALIIP2SMRF_REQ,   at_ftm_fcaliip2sMrf_set,    at_ftm_fcaliip2sMrf_timeout },
    {ID_MSG_FTM_RD_FCALIIP2SMRF_REQ,    at_ftm_fcaliip2sMrf_read,   NULL                        },
    {ID_MSG_FTM_SET_FCALIDCOCSMRF_REQ,  at_ftm_fcalidcocsMrf_set,   at_ftm_fcalidcocsMrf_timeout},
    {ID_MSG_FTM_RD_FCALIDCOCSMRF_REQ,   at_ftm_fcalidcocsMrf_read,  NULL                        },
    {ID_MSG_FTM_SET_FGAINSTATES_REQ,    at_ftm_fgainstates_set,     at_ftm_fgainstates_timeout  },
    {ID_MSG_FTM_RD_FGAINSTATES_REQ,     at_ftm_fgainstates_read,    NULL                        },
    {ID_MSG_FTM_SET_FDBBATTS_REQ,       at_ftm_fdbbatts_set,        at_ftm_fdbbatts_timeout     },
    {ID_MSG_FTM_RD_FDBBATTS_REQ,        at_ftm_fdbbatts_read,       NULL                        },
    {ID_MSG_FTM_SET_FBBATTS_REQ,        at_ftm_fbbatts_set,         at_ftm_fbbatts_timeout      },
    {ID_MSG_FTM_RD_FBBATTS_REQ,         at_ftm_fbbatts_read,        NULL                        },
    {ID_MSG_FTM_SET_FCALITXIQS_REQ,     at_ftm_fcalitxiqs_set,      at_ftm_fcalitxiqs_timeout   },
    {ID_MSG_FTM_RD_FCALITXIQS_REQ,      at_ftm_fcalitxiqs_read,     NULL                        },
    {ID_MSG_FTM_RD_TEMPERATURE_REQ,     at_ftm_cmtm_read,           NULL                        },
    {ID_MSG_FTM_SET_SSYNC_REQ,          at_ftm_ssync_set,           at_ftm_ssync_timeout        },
    {ID_MSG_FTM_RD_SSYNC_REQ,           at_ftm_ssync_read,          NULL                        },
    {ID_MSG_FTM_SET_STXBW_REQ,          at_ftm_stxbw_set,           at_ftm_stxbw_timeout        },
    {ID_MSG_FTM_RD_STXBW_REQ,           at_ftm_stxbw_read,          NULL                        },
    {ID_MSG_FTM_SET_STXCHAN_REQ,        at_ftm_stxchan_set,         at_ftm_stxchan_timeout      },
    {ID_MSG_FTM_RD_STXCHAN_REQ,         at_ftm_stxchan_read,        NULL                        },
    {ID_MSG_FTM_SET_SSUBFRAME_REQ,      at_ftm_ssubframe_set,       at_ftm_ssubframe_timeout    },
    {ID_MSG_FTM_RD_SSUBFRAME_REQ,       at_ftm_ssubframe_read,      NULL                        },
    {ID_MSG_FTM_SET_SPARA_REQ,          at_ftm_spara_set,           at_ftm_spara_timeout        },
    {ID_MSG_FTM_RD_SPARA_REQ,           at_ftm_spara_read,          NULL                        },
    {ID_MSG_FTM_SET_SSEGNUM_REQ,        at_ftm_ssegnum_set,         at_ftm_ssegnum_timeout      },
    {ID_MSG_FTM_RD_SSEGNUM_REQ,         at_ftm_ssegnum_read,        NULL                        },
    {ID_MSG_FTM_SET_STXMODUS_REQ,       at_ftm_stxmodus_set,        at_ftm_stxmodus_timeout     },
    {ID_MSG_FTM_RD_STXMODUS_REQ,        at_ftm_stxmodus_set,        NULL                        },
    {ID_MSG_FTM_SET_STXRBNUMS_REQ,      at_ftm_stxrbnums_set,       at_ftm_stxrbnums_timeout    },
    {ID_MSG_FTM_RD_STXRBNUMS_REQ,       at_ftm_stxrbnums_read,      NULL                        },
    {ID_MSG_FTM_SET_STXRBPOSS_REQ,      at_ftm_stxrbposs_set,       at_ftm_stxrbposs_timeout    },
    {ID_MSG_FTM_RD_STXRBPOSS_REQ,       at_ftm_stxrbposs_read,      NULL                        },
    {ID_MSG_FTM_SET_STXPOWS_REQ,        at_ftm_stxpows_set,         at_ftm_stxpows_timeout      },
    {ID_MSG_FTM_RD_STXPOWS_REQ,         at_ftm_stxpows_read,        NULL                        },
    {ID_MSG_FTM_SET_STXCHANTYPES_REQ,   at_ftm_stxchantypes_set,    at_ftm_stxchantypes_timeout },
    {ID_MSG_FTM_RD_STXCHANTYPES_REQ,    at_ftm_stxchantypes_read,   NULL                        },
    {ID_MSG_FTM_SET_SSEGLEN_REQ,        at_ftm_sseglen_set,         at_ftm_sseglen_timeout      },
    {ID_MSG_FTM_RD_SSEGLEN_REQ,         at_ftm_sseglen_read,        NULL                        },
    {ID_MSG_FTM_SET_SRXSET_REQ,         at_ftm_srxset_set,          at_ftm_srxset_timeout       },
    {ID_MSG_FTM_RD_SRXSET_REQ,          at_ftm_srxset_read,         NULL                        },
    {ID_MSG_FTM_SET_SRXSUBFRA_REQ,      at_ftm_srxsubfra_set,       at_ftm_srxsubfra_timeout    },
    {ID_MSG_FTM_RD_SRXSUBFRA_REQ,       at_ftm_srxsubfra_read,      NULL                        },
    {ID_MSG_FTM_RD_SRXBLER_REQ,         at_ftm_srxmeas_read,        NULL                        },
    {ID_MSG_FTM_SET_SSTART_REQ,         at_ftm_sstart_set,          at_ftm_sstart_timeout       },
    {ID_MSG_FTM_RD_SSTART_REQ,          at_ftm_sstart_read,         NULL                        },
    {ID_MSG_FTM_SET_SSTOP_REQ,          at_ftm_sstop_set,           at_ftm_sstop_timeout        },
    {ID_MSG_FTM_F_SET_FCALPDDCS_REQ,    at_ftm_fcalpddcs_set,       at_ftm_fcalpddcs_timeout    },
    {ID_MSG_FTM_F_RD_FCALPDDCS_REQ,     at_ftm_fcalpddcs_read,      NULL                        },
    {ID_MSG_FTM_SET_FPDPOWS_REQ,        at_ftm_fpdpows_set,         at_ftm_fpdpows_timeout      },
    {ID_MSG_FTM_F_SET_FQPDDCRES_REQ,    at_ftm_fqpddcres_set,        NULL                       },

    {ID_MSG_FTM_SET_STCFGDPCH_REQ,      at_ftm_stcfgdpch_set,       NULL},
    {ID_MSG_FTM_FPOW_REQ,               at_ftm_fpow_set,            at_ftm_fpow_timeout         },
    {ID_MSG_FTM_RD_STRXBER_REQ,         at_ftm_strxmeas_read,        NULL                        },
#if(FEATURE_ON == FEATURE_UE_MODE_TDS)
	{ID_MSG_FTM_RD_SCALIB_REQ,          at_ftm_scalibRdFun,         NULL                        },
    {ID_MSG_FTM_SET_SCALIB_REQ,         at_ftm_scalibSetFun,        NULL                        },
    {ID_MSG_FTM_RD_SCELLINFO_REQ,       at_ftm_scellinfoRdFun,      NULL                        },
#endif
    {ID_MSG_FTM_SET_LTCOMMCMD_REQ,      at_ftm_ltcommcmd_set,       at_ftm_ltcommcmd_timeout    },
    {ID_MSG_FTM_RD_LTCOMMCMD_REQ,       at_ftm_ltcommcmd_read,      NULL                        },
    {ID_MSG_FTM_RD_FPLLSTATUS_REQ,      at_ftm_fpllstatus_read,     at_ftm_fpllstatus_timeout   },

};
/*************************************************************
   PS确认原语消息处理函数表，需要超时判断处理
**************************************************************/

static FTM_PS_MSG_FUNTBL_STRU g_stFtmPsCnfMsgTbl[] = {
    {ID_T_PS_MSP_SFN_SYNC_CNF,           at_ftm_ssync_pscnf         },
   // {ID_T_PS_MSP_DPCH_CNF,                  at_ftm_stcfgdpch_pscnf  },

};

/*************************************************************
   DSP确认原语消息处理函数表，需要超时判断处理
   注: PHY_OM_CT_RSSI_IND,PHY_OM_CT_F_MEAS_IP2_IND
       原语由于历史原语虽然写为IND实际使用为CNF
**************************************************************/
static FTM_DSP_MSG_FUNTBL_STRU g_stFtmDspCnfMsgTbl[] = {
    {PHY_OM_CT_TEST_MODE_CNF,           at_ftm_tmode_dspcnf         },
    {PHY_OM_CT_TSELRF_SET_CNF,          at_ftm_tselrf_dspcnf        },
    {PHY_OM_CT_FREQ_SET_CNF,            at_ftm_fchan_dspcnf         },
    {PHY_OM_CT_FWAVE_SET_CNF,           at_ftm_fwave_dspcnf         },
    {PHY_OM_CT_TXON_CNF,                at_ftm_ftxon_dspcnf         },
    {PHY_OM_CT_RXON_CNF,                at_ftm_frxon_dspcnf         },
    {PHY_OM_CT_AAGC_SET_CNF,            at_ftm_flna_dspcnf          },
    {PHY_OM_CT_RSSI_IND,                at_ftm_frssi_dspind         },
    {PHY_OM_CT_F_BANDSW_SET_CNF,        at_ftm_bandsw_dspcnf        },
    {PHY_OM_CT_VCTCXO_SET_CNF,          at_ftm_fvctcxo_dspcnf       },
    {PHY_OM_CT_F_FBLK_SET_CNF,          at_ftm_fblk_dspcnf          },
    {PHY_OM_CT_F_FREQ_SET_CNF,          at_ftm_fchans_dspcnf        },
    {PHY_OM_CT_F_SEGMENT_SET_CNF,       at_ftm_fsegment_dspcnf      },
    {PHY_OM_CT_F_TXPOW_SET_CNF,         at_ftm_fpows_dspcnf         },
    {PHY_OM_CT_F_PA_SET_CNF,            at_ftm_fpas_dspcnf          },
    {PHY_OM_CT_F_AAGC_SET_CNF,          at_ftm_flnas_dspcnf         },
    {PHY_OM_CT_F_FPDMS_CNF,             at_ftm_fpdms_dspcnf         },
    {PHY_OM_CT_F_TXWAVE_SET_CNF,        at_ftm_ftxwave_dspcnf       },
    {PHY_OM_CT_F_TRIGGER_SET_CNF,       at_ftm_fstart_dspcnf        },
    {PHY_OM_CT_F_TRIGGER_RD_CNF,        at_ftm_fstart_read_dspcnf   },
    {PHY_OM_CT_F_MAX_POWER_SET_CNF,     at_ftm_fmaxpower_dspcnf     },
    {PHY_OM_CT_F_PA_POWER_SET_CNF,      at_ftm_fpapower_dspcnf      },
    {PHY_OM_CT_F_CALIIP2_SET_CNF,       at_ftm_fcaliip2s_dspcnf     },
    {PHY_OM_CT_F_CALIDCOCS_SET_CNF,     at_ftm_fcalidcocs_dspcnf    },
    {PHY_OM_CT_F_CALIIP2_SET_MUTIRF_CNF,at_ftm_fcaliip2sMrf_dspcnf   },
    {PHY_OM_CT_F_CALIDCOCS_SET_MUTIRF_CNF,at_ftm_fcalidcocsMrf_dspcnf},
    {PHY_OM_CT_F_GAINSTATE_SET_CNF,     at_ftm_fgainstates_dspcnf   },
    {PHY_OM_CT_F_DBBATT_SET_CNF,        at_ftm_fdbbatts_dspcnf      },
    {PHY_OM_CT_F_BBATT_SET_CNF,         at_ftm_fbbatts_dspcnf       },
    {PHY_OM_CT_F_CALITXIQ_SET_CNF,      at_ftm_fcalitxiqs_dspcnf    },
    {PHY_OM_CT_F_MEAS_IP2_IND,          at_ftm_fipstart_dspind      },
    {PHY_OM_SYN_SSYNC_SET_CNF,          at_ftm_ssync_dspcnf         },
    {PHY_OM_SYN_STXBW_SET_CNF,          at_ftm_stxbw_dspcnf         },
    {PHY_OM_SYN_STXCHAN_SET_CNF,        at_ftm_stxchan_dspcnf       },
    {PHY_OM_SYN_SSUBFRAME_SET_CNF,      at_ftm_ssubframe_dspcnf     },
    {PHY_OM_SYN_SPARA_SET_CNF,          at_ftm_spara_dspcnf         },
    {PHY_OM_SYN_SSEGNUM_SET_CNF,        at_ftm_ssegnum_dspcnf       },
    {PHY_OM_SYN_STXMODUS_SET_CNF,       at_ftm_stxmodus_dspcnf      },
    {PHY_OM_SYN_STXRBNUMS_SET_CNF,      at_ftm_stxrbnums_dspcnf     },
    {PHY_OM_SYN_STXRBPOSS_SET_CNF,      at_ftm_stxrbposs_dspcnf     },
    {PHY_OM_SYN_STXPOWS_SET_CNF,        at_ftm_stxpows_dspcnf       },
    {PHY_OM_SYN_STXCHANTYPES_SET_CNF,   at_ftm_stxchantypes_dspcnf  },
    {PHY_OM_SYN_SSEGLEN_SET_CNF,        at_ftm_sseglen_dspcnf       },
    {PHY_OM_SYN_SRXSET_SET_CNF,         at_ftm_srxset_dspcnf        },
    {PHY_OM_SYN_SRXSUBFRA_SET_CNF,      at_ftm_srxsubfra_dspcnf     },
    {PHY_OM_SYN_SSTART_SET_CNF,         at_ftm_sstart_dspcnf        },
    {PHY_OM_SYN_SSTOP_SET_CNF,          at_ftm_sstop_dspcnf         },
    {PHY_OM_CT_F_PD_AUTO_CAL_CNF,          at_ftm_fcalpddcs_dspcnf         },
    {PHY_OM_CT_F_PD_CAL_CNF,          at_ftm_fpdpows_dspcnf         },
    {PHY_OM_BT_FPOW_SET_CNF,            at_ftm_fpow_dspcnf          },
    {PHY_OM_COMM_CMD_SET_CNF,           at_ftm_ltcommcmd_dspcnf     },
    {PHY_OM_FPLLSTATUS_RD_CNF,          at_ftm_fpllstatus_dspcnf     },
};

/*************************************************************
   DSP执行某些异步操作完成后主动上报原语消息处理函数表，不需要超时判断
   注: PHY_OM_CT_MEAS_RSSI_CNF,PHY_OM_CT_MEAS_RSSI_CNF
       原语由于历史原语虽然写为CNF实际使用为IND
**************************************************************/
static FTM_DSP_MSG_FUNTBL_STRU g_stFtmDspIndMsgTbl[] = {
    {PHY_OM_CT_MEAS_RSSI_CNF,      at_ftm_frssi_dspcnf     },
    {PHY_OM_CT_F_MEAS_CMTMS_IND,   at_ftm_fcmtms_dspind    },
    {PHY_OM_CT_F_MEAS_RSSI_IND,    at_ftm_frssis_dspind    },
    {PHY_OM_CT_F_CALIIP2_IND,      at_ftm_fcaliip2s_dspind },
    {PHY_OM_CT_F_CALIDCOCS_IND,    at_ftm_fcalidcocs_dspind},
    {PHY_OM_CT_F_CALIIP2_MUTIRF_IND,   at_ftm_fcaliip2sMrf_dspind },
    {PHY_OM_CT_F_CALIDCOCS_MUTIRF_IND, at_ftm_fcalidcocsMrf_dspind},
    {PHY_OM_CT_F_CALITXIQ_IND,     at_ftm_fcalitxiqs_dspind},
    {PHY_OM_SYN_SRXBLER_RD_CNF,    at_ftm_srxmeas_dspind   },
    {PHY_OM_SYN_SSYNC_IND,         at_ftm_ssync_dspind     },
    {PHY_OM_CT_F_PD_AUTO_CAL_IND,  at_ftm_fcalpddcs_dspind  },
    {PHY_OM_CT_F_PD_CAL_IND,  at_ftm_fpdpows_dspind  },
    {PHY_OM_SYN_SRXBER_RD_CNF,     at_ftm_strxmeas_dspind   },
    {PHY_OM_COMM_CMD_IND,           at_ftm_ltcommcmd_dspind  },
};

FTM_MAIN_INFO_STRU * ftm_GetMainInfo(VOS_VOID)
{
    return &g_stFtmMainInfo;
}

SYM_TMODE_ENUM* ftm_GetTmodeMainInfo(VOS_VOID)
{
    return &(g_stFtmMainInfo.symMode);
}


FTM_CT_F_MANAGE_INFO_STRU* ftm_GetFastCTMainInfo(VOS_VOID)
{
    return &(g_stFtmMainInfo.stFtmFastCtInfo);
}

/* 非信令综测 */
FTM_SYN_INFO_STRU *ftm_GetSynMainInfo(VOS_VOID)
{
    return &(g_stFtmMainInfo.stFtmSynInfo);
}

VOS_VOID ftm_SetDefault(VOS_VOID)
{
	FTM_SYN_INFO_STRU *stFCtMainInfo = NULL;

	stFCtMainInfo = ftm_GetSynMainInfo();
	stFCtMainInfo->stSstartRdCnf.usType = 0;
	stFCtMainInfo->stSstartRdCnf.usStatus = 2;

	stFCtMainInfo->stSsyncRdCnf.ulStatus = 0;

	stFCtMainInfo->stSrxblerRdCnf.usbler[0] = 0;
	stFCtMainInfo->stSrxblerRdCnf.usSnr[0] = 0;
    stFCtMainInfo->stSrxblerRdCnf.usbler[1] = 0;
	stFCtMainInfo->stSrxblerRdCnf.usSnr[1] = 0;

       stFCtMainInfo->stFtmSparaInfo.usType = 0;
       stFCtMainInfo->stFtmSparaInfo.usValue = 0;
       stFCtMainInfo->stStrxBerRdCnf.ulberErr=0;
       stFCtMainInfo->stStrxBerRdCnf.ulbersum=0;
	stFCtMainInfo->ulCurCmdStepsFlag &=(~CT_S_STEPS_DONE_STRXBER_IND);
	stFCtMainInfo->ulCurCmdStepsFlag &= (~CT_S_STEPS_DONE_SRXBLER_IND);
}

/* 设置非信令开机模式 */
VOS_VOID ftm_SetNoneSig(VOS_UINT8 ucNoneSigNal)
{
    (ftm_GetMainInfo()->stFtmCtInfo).ucNoSigFlg = ucNoneSigNal;
}

/* 获取非信令打开模式 */
VOS_UINT8 ftm_GetNoneSig(VOS_VOID)
{
    return ((ftm_GetMainInfo()->stFtmCtInfo).ucNoSigFlg);
}
VOS_UINT8 ftm_GetNoneSigNV(VOS_VOID)
{
   return ftm_GetNoneSig();
}

VOS_BOOL ftm_CheckCmdSteps(FTM_CT_F_CMD_STEPS_FLAG_ENUM_UINT32 enCmdSteps)
{
    VOS_BOOL bRet = TRUE;

    FTM_CT_F_MANAGE_INFO_STRU *stFCtMainInfo = NULL;
    stFCtMainInfo = ftm_GetFastCTMainInfo();

    if(enCmdSteps == (stFCtMainInfo->ulCurCmdStepsFlag & enCmdSteps))
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }

    return  bRet;
}
/* 检查非信令综测的steps */
VOS_BOOL ftm_CheckSynCmdSteps(VOS_UINT32 enCmdSteps)
{
    VOS_BOOL bRet = TRUE;

    FTM_SYN_INFO_STRU *stSynMainInfo = NULL;
    stSynMainInfo = ftm_GetSynMainInfo();

    if(enCmdSteps == (stSynMainInfo->ulCurCmdStepsFlag & enCmdSteps))
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }

    return  bRet;
}

/* 获取FTM当前模式的同步接口, PS调用 */
VOS_UINT32 FTM_GetMode(OUT FTM_TMODE_ENUM* penFtmMode)
{
   VOS_UINT32 ret = ERR_MSP_UNKNOWN;

   if(penFtmMode == NULL)
   {
       ret = ERR_MSP_INVALID_PARAMETER;
   }
   else
   {
       SYM_TMODE_ENUM symTmode = (SYM_SYSMODE_ENUM)*ftm_GetTmodeMainInfo();

       if(symTmode == EN_SYM_TMODE_SIGNAL)
       {
           *penFtmMode = EN_FTM_TMODE_SIGNAL;
       }
       else if(symTmode == EN_SYM_TMODE_NONESIGNAL||
		     symTmode == EN_SYM_TMODE_F_NONESIGNAL||
		     symTmode == EN_SYM_TMODE_SYN_NONESIGNAL||
		     symTmode == EN_SYM_TMODE_TDS_FAST_CT||
		     symTmode == EN_SYM_TMODE_TDS_BT||
		     symTmode == EN_SYM_TMODE_COMM_CT)
       {
           *penFtmMode = EN_FTM_TMODE_NONESIGNAL;
       }
       else if(symTmode == EN_SYM_TMODE_RESET)
       {
           *penFtmMode = EN_FTM_TMODE_RESET;
       }
       else if(symTmode == EN_SYM_TMODE_SIGNAL_NOCARD)
       {
           *penFtmMode = EN_FTM_TMODE_SIGNAL_NOCARD;
       }
       else if(symTmode == EN_SYM_TMODE_OFFLINE)
       {
           *penFtmMode = EN_FTM_TMODE_OFFLINE;
       }
       else
       {
           *penFtmMode = EN_FTM_TMODE_UNKNOWN;
       }

       if(symTmode != (VOS_UINT32)EN_SYM_TMODE_UNKNOWN)
       {
           ret = ERR_MSP_SUCCESS;
       }
   }

   return ret;
}


VOS_VOID ftm_MainInfoInit(VOS_VOID)
{
     FTM_CT_F_MANAGE_INFO_STRU *stFCtMainInfo = NULL;

	 /* 非信令综测 */
	 FTM_SYN_INFO_STRU *stSynInfo = NULL;

    /* 1、初始化SYM注册接口函数指针 */
    /* ((FTM_MAIN_INFO_STRU *)ftm_GetMainInfo())->pfnResetSymFunc = NULL; */

    /* 2、TODO:初始化普通校准默认值及运行时数据(暂不实现) */

	/* bandsw之后，必须设置非信令关闭，否则不会给DSP下发TMODE命令(参考openChanReq函数) */

	(ftm_GetMainInfo()->stFtmCtInfo).ucNoSigFlg = FTM_NONESIG_RTT_CLOSE;

    /* 3、初始化快速校准默认值及运行时数据 */
    stFCtMainInfo = ftm_GetFastCTMainInfo();
	stSynInfo = ftm_GetSynMainInfo();

    /* 以下所有变量赋值必须与静态全局变量中的完全一致，
       所有的取值取决于DSP提供的初始值 */

    stFCtMainInfo->ulCurCmdStepsFlag   = CT_F_STEPS_DONE_NULL    ; /* 初始状态:当前没有执行任何命令 */
    stFCtMainInfo->enCurMode           = EN_FCHAN_MODE_UNKONWN ;   /* 初始状态为无效模式 */
    stFCtMainInfo->ucCurBand           = 255;                      /* 初始状态为无效频段 */
    stFCtMainInfo->ucCurWaveType       = TXWAVE_TYPE_CW          ; /* 0->CW波型（DSP默认配置为CW波型）*/
    stFCtMainInfo->ucCurTriggerType    = START_TRIGGER_TYPE_TX   ; /* 发送通道校准 */

    stFCtMainInfo->usCurSegmentLen     = 20  ; /* segment长度，单位ms，默认值为20 */
    stFCtMainInfo->usCurMeasOffset     =  0  ; /* 测量起始位置偏移，单位ms，取值范围0~19，无默认值 */
    stFCtMainInfo->usCurMeasLen        =  1  ; /* 测量长度，单位ms，取值范围1~20，无默认值 */

    stFCtMainInfo->usCurUlListNum      = 0  ; /* 快速校准上行信道号数量,默认值为0 */
    stFCtMainInfo->usCurDlListNum      = 0  ; /* 快速校准下行信道号数量,默认值为0 */

    stFCtMainInfo->usCurTxPowerNum     = 0  ; /* 发射机的发射功率的数量，取值范围0～20，默认值为0 */
    stFCtMainInfo->usCurPaLevelNum     = 0  ; /* PA的等级数量，取值范围0～20，长度为一个字节，数值应该与^FPOWS中power_num的值一致，默认值为0。*/
    stFCtMainInfo->usCurAagcNum        = 0  ; /* LNA的等级的数量，取值范围0~20，默认值为0。 */
    stFCtMainInfo->usCmtmsNum          = 0  ;  /* 温度值数量，取值范围为0～16 */
    stFCtMainInfo->usRsv               = 0  ;

    MSP_MEMSET(stFCtMainInfo->ausCurUlFreqInfo  ,0 ,sizeof(stFCtMainInfo->ausCurUlFreqInfo));
    MSP_MEMSET(stFCtMainInfo->ausCurDlFreqInfo  ,0 ,sizeof(stFCtMainInfo->ausCurDlFreqInfo));
    MSP_MEMSET(stFCtMainInfo->asCurTxPowerList  ,0 ,sizeof(stFCtMainInfo->asCurTxPowerList));
    MSP_MEMSET(stFCtMainInfo->aucCurPaLevelList ,0 ,sizeof(stFCtMainInfo->aucCurPaLevelList));
    MSP_MEMSET(stFCtMainInfo->aucCurAagcList    ,0 ,sizeof(stFCtMainInfo->aucCurAagcList));

    MSP_MEMSET(stFCtMainInfo->ausRxANT1RSSI     ,0 ,sizeof(stFCtMainInfo->ausRxANT1RSSI));
    MSP_MEMSET(stFCtMainInfo->ausRxANT2RSSI     ,0 ,sizeof(stFCtMainInfo->ausRxANT2RSSI));
	MSP_MEMSET(stFCtMainInfo->ausCmtms     		,0 ,sizeof(stFCtMainInfo->ausCmtms));

    MSP_MEMSET(&stFCtMainInfo->stFCaliIp2sSetReq ,0 ,sizeof(stFCtMainInfo->stFCaliIp2sSetReq));
    MSP_MEMSET(&stFCtMainInfo->stFCaliIp2sRdCnf  ,0 ,sizeof(stFCtMainInfo->stFCaliIp2sRdCnf));
    stFCtMainInfo->stFCaliIp2sRdCnf.usStatus = FTM_STATUS_EXECUTED;

    MSP_MEMSET(&stFCtMainInfo->stFCaliDcocsSetReq,0 ,sizeof(stFCtMainInfo->stFCaliDcocsSetReq));
    MSP_MEMSET(&stFCtMainInfo->stFCaliDcocsRdCnf ,0 ,sizeof(stFCtMainInfo->stFCaliDcocsRdCnf));
    stFCtMainInfo->stFCaliDcocsRdCnf.usStatus = FTM_STATUS_EXECUTED;

    MSP_MEMSET(&stFCtMainInfo->stFGainStatesSetReq,0 ,sizeof(stFCtMainInfo->stFGainStatesSetReq));

    MSP_MEMSET(&stFCtMainInfo->stFCaliIp2sMrfSetReq ,0 ,sizeof(stFCtMainInfo->stFCaliIp2sMrfSetReq));
    MSP_MEMSET(&stFCtMainInfo->stFCaliIp2sMrfRdCnf  ,0 ,sizeof(stFCtMainInfo->stFCaliIp2sMrfRdCnf));
    stFCtMainInfo->stFCaliIp2sMrfRdCnf.usStatus = FTM_STATUS_EXECUTED;

    MSP_MEMSET(&stFCtMainInfo->stFCaliDcocsMrfSetReq,0 ,sizeof(stFCtMainInfo->stFCaliDcocsMrfSetReq));
    MSP_MEMSET(&stFCtMainInfo->stFCaliDcocsMrfRdCnf ,0 ,sizeof(stFCtMainInfo->stFCaliDcocsMrfRdCnf));
    stFCtMainInfo->stFCaliDcocsMrfRdCnf.usStatus = FTM_STATUS_EXECUTED;

	MSP_MEMSET(&stFCtMainInfo->stFDbbAttSetReq    ,0 ,sizeof(stFCtMainInfo->stFDbbAttSetReq));

	MSP_MEMSET(&stFCtMainInfo->stFBbAttSetReq	  ,0 ,sizeof(stFCtMainInfo->stFBbAttSetReq));

	MSP_MEMSET(&stFCtMainInfo->stFCaliTxiqSetReq  ,0 ,sizeof(stFCtMainInfo->stFCaliTxiqSetReq));
    MSP_MEMSET(&stFCtMainInfo->stFCaliTxiqRdCnf   ,0 ,sizeof(stFCtMainInfo->stFCaliTxiqRdCnf));
	MSP_MEMSET(&stFCtMainInfo->stFCalPDDCSSetReq, 0, sizeof(stFCtMainInfo->stFCalPDDCSSetReq));
    MSP_MEMSET(&stFCtMainInfo->stFCalPDDCSRdCnf, 0, sizeof(stFCtMainInfo->stFCalPDDCSRdCnf));
	MSP_MEMSET(&stFCtMainInfo->stFpdpowsSetReq, 0, sizeof(stFCtMainInfo->stFpdpowsSetReq));
    stFCtMainInfo->stFCaliTxiqRdCnf.usStatus = FTM_STATUS_EXECUTED;
	/* Modified By Yangzhi For V9r1_Hi6361, End! */
    /*****************************************/
    /* 非信令综测初始化 */
    stSynInfo->ulCurCmdStepsFlag = CT_S_STEPS_DONE_NULL;
	stSynInfo->stSsyncRdCnf.ulStatus = 0;
	stSynInfo->stSsyncRdCnf.ulErrCode = 0;

}

/*****************************************************************************
 函 数 名  : ftm_get_equipmsg_handler
 功能描述  : 通过消息ID获取装备消息处理函数指针
 输入参数  : ulMsgId 装备消息ID
 输出参数  : 无
 返 回 值  : 成功返回消息处理函数指针, 否则返回NULL
 *****************************************************************************/
static PFN_AT_FTM_EQUIP_MSG_PROC ftm_get_equipmsg_handler(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTblSize = sizeof(g_stFtmEquipMsgTbl) / sizeof(FTM_EQUIP_MSG_FUNTBL_STRU);

    for(i=0; i<ulTblSize; i++)
    {
        if(ulMsgId == g_stFtmEquipMsgTbl[i].ulMsgId)
        {
            return g_stFtmEquipMsgTbl[i].pfnEquipMsgProc;
        }
    }

    return NULL;
}

/*****************************************************************************
 函 数 名  : ftm_get_timeout_handler
 功能描述  : 通过装备消息ID获取超时处理函数指针
 输入参数  : ulMsgId 装备消息ID
 输出参数  : 无
 返 回 值  : 成功返回消息处理函数指针, 否则返回NULL
 *****************************************************************************/
static PFN_AT_FTM_TIMEOUT_PROC ftm_get_timeout_handler(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTblSize = sizeof(g_stFtmEquipMsgTbl) / sizeof(FTM_EQUIP_MSG_FUNTBL_STRU);

    for(i=0; i<ulTblSize; i++)
    {
        if(ulMsgId == g_stFtmEquipMsgTbl[i].ulMsgId)
        {
            return g_stFtmEquipMsgTbl[i].pfnTimeoutProc;
        }
    }

    return NULL;
}
/*****************************************************************************
 函 数 名  : ftm_get_pscnfmsg_handler
 功能描述  : 通过消息ID获取装备消息处理函数指针
 输入参数  : ulMsgId 装备消息ID
 输出参数  : 无
 返 回 值  : 成功返回消息处理函数指针, 否则返回NULL
 *****************************************************************************/
static PFN_AT_FTM_PS_MSP_PROC ftm_get_pscnfmsg_handler(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTblSize = sizeof(g_stFtmPsCnfMsgTbl) / sizeof(FTM_PS_MSG_FUNTBL_STRU);

    for(i=0; i<ulTblSize; i++)
    {
        if(ulMsgId == g_stFtmPsCnfMsgTbl[i].ulMsgId)
        {
            return g_stFtmPsCnfMsgTbl[i].pfnPsMsgProc;
        }
    }

    return NULL;
}

/*****************************************************************************
 函 数 名  : ftm_get_dspcnfmsg_handler
 功能描述  : 通过DSP确认原语消息ID获取消息处理函数指针
 输入参数  : ulMsgId DSP确认原语消息ID
 输出参数  : 无
 返 回 值  : 成功返回消息处理函数指针, 否则返回NULL
 *****************************************************************************/
static PFN_AT_FTM_DSP_MSP_PROC ftm_get_dspcnfmsg_handler(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTblSzie = sizeof(g_stFtmDspCnfMsgTbl) / sizeof(FTM_DSP_MSG_FUNTBL_STRU);

    for(i=0; i<ulTblSzie; i++)
    {
        if(ulMsgId == g_stFtmDspCnfMsgTbl[i].ulMsgId)
        {
            return g_stFtmDspCnfMsgTbl[i].pfnDspMsgProc;
        }
    }

    return NULL;
}

/*****************************************************************************
 函 数 名  : ftm_get_dspindmsg_handler
 功能描述  : 通过DSP主动上报原语消息ID获取消息处理函数指针
 输入参数  : ulMsgId DSP原语原语消息ID
 输出参数  : 无
 返 回 值  : 成功返回消息处理函数指针, 否则返回NULL
 *****************************************************************************/
static PFN_AT_FTM_DSP_MSP_PROC ftm_get_dspindmsg_handler(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTblSize = sizeof(g_stFtmDspIndMsgTbl) / sizeof(FTM_DSP_MSG_FUNTBL_STRU);

    for(i=0; i<ulTblSize; i++)
    {
        if(ulMsgId == g_stFtmDspIndMsgTbl[i].ulMsgId)
        {
            return g_stFtmDspIndMsgTbl[i].pfnDspMsgProc;
        }
    }

    return NULL;
}

/*****************************************************************************
 函 数 名  : ftm_RttMsgProc
 功能描述  : DSP原语消息处理
             1、确认原语搜索确认原语表获得处理函数并调用之，关闭超时处理定时器
             2、主动上报原语搜索主动上报原语表获得处理函数
 输入参数  : pMsg DSP原语消息
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS 否则返回其他
 *****************************************************************************/
VOS_UINT32 ftm_RttMsgProc(OS_MSG_STRU* pMsg)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    PFN_AT_FTM_DSP_MSP_PROC pfnDspMsgProc = NULL;
    L1A_CT_IND_STRU* pstDspCt             = NULL;

    pstDspCt = (L1A_CT_IND_STRU*)(pMsg->ulParam1);

    if(NULL == pstDspCt)
    {
        HAL_SDMLOG("\n ftm_RttMsgProc: Invalid Msg \n");
        return ERR_MSP_INVALID_PARAMETER;
    }

    HAL_SDMLOG("\n ftm_RttMsgProc: DSP Msg ID: 0x%x \n", pstDspCt->ulDspId);

    /* DSP CNF消息处理过程 */
    pfnDspMsgProc = ftm_get_dspcnfmsg_handler(pstDspCt->ulDspId);
    FTM_MNTN_RTT2FTM_MSG(pstDspCt->ulDspId);
    if(NULL != pfnDspMsgProc)
    {
        if(g_ct_cmd_ctrl.ucSta == FTM_CT_WAIT)
        {
            ftm_comm_stoptimer(&g_ct_cmd_ctrl.ulReqTimer);
            g_ct_cmd_ctrl.ucSta = FTM_CT_NORMAL;
            ulRet = (pfnDspMsgProc)(pstDspCt);
        }
        else
        {
            HAL_SDMLOG("\n Warning: Released DSP Msg 0x%x \n", pMsg->ulMsgId);
        }
        return ulRet;
    }

    /* DSP IND消息处理过程 */
    pfnDspMsgProc = ftm_get_dspindmsg_handler(pstDspCt->ulDspId);
    if(NULL != pfnDspMsgProc)
    {
        ulRet = (pfnDspMsgProc)(pstDspCt);
        return ulRet;
    }

    HAL_SDMLOG("\n ftm_RttMsgProc: Invalid DSP Msg ID: 0x%x \n", pstDspCt->ulDspId);
    return ERR_MSP_NOT_EXIST;
}

/*****************************************************************************
 函 数 名  : ftm_TimeoutMsgProc
 功能描述  : 超时消息处理
             搜所装备消息处理表获得超时处理函数并调用之
 输入参数  : pMsg定时器消息
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他
 *****************************************************************************/
VOS_UINT32 ftm_TimeoutMsgProc(OS_MSG_STRU* pMsg)
{
    VOS_UINT32 ulTimeoutMsgId = 0;
    VOS_UINT32 ulRet          = ERR_MSP_SUCCESS;
    OSM_MSG_TIMER_STRU*     pTimerMsg       = NULL;
    PFN_AT_FTM_TIMEOUT_PROC pfnTimeoutProc  = NULL;

    pTimerMsg = (OSM_MSG_TIMER_STRU *)(pMsg->ulParam1);

    if(NULL == pTimerMsg)
    {
        HAL_SDMLOG("\n ftm_TimeoutMsgProc: Invalid Msg \n");
        return ERR_MSP_INVALID_PARAMETER;
    }

    ulTimeoutMsgId = (VOS_UINT32)(pTimerMsg->ulPara);
    pfnTimeoutProc = ftm_get_timeout_handler(ulTimeoutMsgId);
    /* HAL_SDMLOG("\n ftm_TimeoutMsgProc: Timeout Msg ID 0x%x \n", ulTimeoutMsgId); */
    if(NULL == pfnTimeoutProc)
    {
        HAL_SDMLOG("\n ftm_TimeoutMsgProc: Invalid Timeout Msg ID: 0x%x \n", ulTimeoutMsgId);
        return ERR_MSP_NOT_EXIST;
    }

     /* 超时处理定时器消息 */
    else
    {
        FTM_MNTN_TIMEOUT_MSG(ulTimeoutMsgId);
        if(g_ct_cmd_ctrl.ucSta == FTM_CT_WAIT)
        {
            ftm_comm_stoptimer(&g_ct_cmd_ctrl.ulReqTimer);
            g_ct_cmd_ctrl.ucSta = FTM_CT_NORMAL;
			/*lint -save -e746*/
            ulRet = (pfnTimeoutProc)();
			/*lint -restore*/
        }
        else
        {
            HAL_SDMLOG("\n Waring: Released Timeout Msg 0x%x \n", pMsg->ulMsgId);
        }
        return ulRet;
    }
}

/*****************************************************************************
 函 数 名  : ftm_CmdMsgProc
 功能描述  : A核发送的装备消息处理
             搜所装备消息处理表获得处理函数并处理之，关闭超时处理定时器
 输入参数  : pMsg装备消息
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他
 *****************************************************************************/
VOS_UINT32 ftm_CmdMsgProc(OS_MSG_STRU* pMsg)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    PFN_AT_FTM_EQUIP_MSG_PROC pfnEquipMsgProc = NULL;
    PFN_AT_FTM_PS_MSP_PROC pstPscnfMsgProc = NULL;

    pfnEquipMsgProc = ftm_get_equipmsg_handler(pMsg->ulMsgId);
    FTM_MNTN_ACORE2FTM_MSG(pMsg->ulMsgId);
    HAL_SDMLOG("ftm_CmdMsgProc: pMsg-ID=0x%08x \n",pMsg->ulMsgId);

	#if 0
    if(NULL == pfnEquipMsgProc)
    {
        HAL_SDMLOG("\nftm_CmdMsgProc: Invalid Equip Msg ID: 0x%x\n", pMsg->ulMsgId);
        return ERR_MSP_INVALID_OP;
    }

    HAL_SDMLOG("\n ftm_CmdMsgProc: Equip Msg ID 0x%x \n", pMsg->ulMsgId);
    #endif

    if(NULL != pfnEquipMsgProc)
    {
        if(g_ct_cmd_ctrl.ucSta == FTM_CT_NORMAL)
	    {
	        ulRet = (pfnEquipMsgProc)((VOS_VOID *)pMsg->ulParam1);

            if(ulRet == ERR_MSP_WAIT_ASYNC)
            {
                ftm_comm_starttimer(&g_ct_cmd_ctrl.ulReqTimer, pMsg->ulMsgId);
                g_ct_cmd_ctrl.ucSta = FTM_CT_WAIT;
                return ERR_MSP_SUCCESS;
            }
            else if(ulRet == ERR_MSP_SUCCESS)
            {
                return ERR_MSP_SUCCESS;
            }
            else
            {
                HAL_SDMLOG("\n Warning: Msg 0x%x processed abnormal \n", pMsg->ulMsgId);
                return ulRet;
            }
        }
        else
        {
            HAL_SDMLOG("\n Waring: Discarded Msg 0x%x \n", pMsg->ulMsgId);
        }
    }

	pstPscnfMsgProc = ftm_get_pscnfmsg_handler(pMsg->ulMsgId);

	if(NULL != pstPscnfMsgProc)
	{
        HAL_SDMLOG("pstPscnfMsgProc  IS NOT NULL \n");
	    if(g_ct_cmd_ctrl.ucSta == FTM_CT_WAIT)
        {
            ftm_comm_stoptimer(&g_ct_cmd_ctrl.ulReqTimer);
            g_ct_cmd_ctrl.ucSta = FTM_CT_NORMAL;
            ulRet = (pstPscnfMsgProc)(pMsg);
        }
        else
        {
            HAL_SDMLOG("\n Warning: Released PS Msg 0x%x \n", pMsg->ulMsgId);
        }
        return ulRet;

	}
   return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : ftm_TaskMsg
 功能描述  : FTM任务消息处理函数，对发送到的消息进行分发处理
 输入参数  : pMsg 消息内容
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他
 *****************************************************************************/
VOS_UINT32 ftm_TaskMsg(OS_MSG_STRU * pMsg)
{
    VOS_UINT32 ulRet = ERR_MSP_FAILURE;

    if (NULL == pMsg)
    {
        HAL_SDMLOG("\n ftm_TaskMsg: Invalid Msg \n");
        return ERR_MSP_INVALID_PARAMETER;
    }

    switch(pMsg->ulMsgId)
    {
        case ID_MSG_L1A_CT_IND:
            ulRet = ftm_RttMsgProc(pMsg);
            break;

        case ID_MSG_FTM_TIME_OUT:
            ulRet = ftm_TimeoutMsgProc(pMsg);
            break;

        default :
            ulRet = ftm_CmdMsgProc(pMsg);
    }

    /* PS传上来的ulParam1非动态内存不需要释放，其他需释放 */
    if( ID_T_PS_MSP_SFN_SYNC_CNF != pMsg->ulMsgId)
    {
        if (NULL != pMsg->ulParam1)
        {
        	/*lint -save -e740*/
			VOS_MemFree(MSP_SYS_FTM_PID, pMsg->ulParam1);
			/*lint -restore*/
        }
    }

    return ulRet;
}

/*****************************************************************************
 函 数 名  : FTM_MsgConvert
 功能描述  : 消息格式转换接口(由于AT移到A核，消息不能传输指针，适配原有接口)
 输入参数  : MsgBlock* pMsg
             OS_MSG_STRU* pstFtmMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月28日
    作    者   : 刘海君/wx26324
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 FTM_MsgConvert(MsgBlock* pMsg, OS_MSG_STRU* pstFtmMsg)
{
	/*lint -save -e740*/
	AT_FW_DATA_MSG_STRU* pDataMsg = (AT_FW_DATA_MSG_STRU*)pMsg;
	/*lint -restore*/

    g_ulCrntClientId = pDataMsg->ulClientId;
    pstFtmMsg->ulMsgId = pDataMsg->ulMsgId;

    pstFtmMsg->ulParam1 = VOS_MemAlloc(MSP_SYS_FTM_PID, (DYNAMIC_MEM_PT), pDataMsg->ulLen);
    if(pstFtmMsg->ulParam1 == NULL)
    {
      return ERR_MSP_FAILURE;
    }

    MSP_MEMCPY(pstFtmMsg->ulParam1, pDataMsg->pContext, pDataMsg->ulLen);
    pstFtmMsg->ulParam2 = pDataMsg->ulLen;

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : ftm_SendDataMsg
 功能描述  : FTM给AT发送消息接口(由于AT移到A核，消息不能传输指针，适配原有接口)
 输入参数  : VOS_UINT32 ulSenderId
             VOS_UINT32 ulRecverId
             VOS_UINT32 ulMsgId
             VOS_UINT32 ulParam1
             VOS_UINT32 ulParam2
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月28日
    作    者   : 刘海君/wx26324
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 ftm_SendDataMsg(VOS_UINT32 ulSenderId, VOS_UINT32 ulRecverId, VOS_UINT32 ulMsgId, VOS_UINT32 ulParam1, VOS_UINT32 ulParam2)
{
    VOS_UINT32 ret = ERR_MSP_UNKNOWN;

    AT_FW_DATA_MSG_STRU* pDataMsg = NULL;
	/*lint -save -e740*/
    pDataMsg = (AT_FW_DATA_MSG_STRU*)VOS_AllocMsg(ulSenderId,(sizeof(AT_FW_DATA_MSG_STRU)+ ulParam2 - VOS_MSG_HEAD_LENGTH));
	/*lint -restore*/
    HAL_SDMLOG("\n ftm_SendDataMsg: ulParam1: 0x%x, ulParam2: 0x%x \n", ulParam1, ulParam2);
    if (pDataMsg)
    {
        pDataMsg->ulReceiverPid = ulRecverId;
        pDataMsg->ulSenderPid   = ulSenderId;
        pDataMsg->ulLength      = sizeof(AT_FW_DATA_MSG_STRU)+ ulParam2 - VOS_MSG_HEAD_LENGTH;

        pDataMsg->ulClientId = g_ulCrntClientId;
        pDataMsg->ulMsgId    = ulMsgId;
        pDataMsg->ulLen      = ulParam2;
        MSP_MEMCPY(pDataMsg->pContext,(VOS_UINT8*)ulParam1,ulParam2);

        ret = VOS_SendMsg(ulSenderId, pDataMsg);
        if (ret != VOS_OK)
        {
        }
        else
        {
            ret = ERR_MSP_SUCCESS;
        }

    }
    return ret;
}

/*****************************************************************************
 函 数 名  : atFTMSendVosMsg
 功能描述  : FTM向其他模块发送VOSMSG
 输入参数  : TaskId  接受模块ID
                            pData  消息指针
                            uLen   消息长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_UINT32 atFTMSendVosMsg(VOS_UINT32 TaskId, IN VOS_VOID* pData, VOS_UINT32 uLen)
{
    VOS_UINT32 ulRet = ERR_MSP_UNKNOWN;
    MsgBlock *pMsgBlock = NULL;

    pMsgBlock = VOS_AllocMsg(MSP_SYS_FTM_PID, uLen);
    if(NULL == pMsgBlock)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }

    pMsgBlock->ulReceiverPid = TaskId;
    pMsgBlock->ulSenderPid   = MSP_SYS_FTM_PID;

	/*lint -e419*/
	VOS_MemCpy(pMsgBlock->aucValue, pData, uLen);
	/*lint -e419*/
    ulRet = VOS_SendMsg(MSP_SYS_FTM_PID, pMsgBlock);
    if (VOS_OK != ulRet)
    {
/*        VOS_FreeMsg(sda_GetMspDsMuxPid(), (PVOID_T)pMsgBlock);*/
    }
    else
    {
        ulRet = ERR_MSP_SUCCESS;
    }

    return ulRet;
}

