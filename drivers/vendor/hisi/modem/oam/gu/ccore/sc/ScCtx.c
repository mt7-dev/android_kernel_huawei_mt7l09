

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ScCtx.h"
#include "ScApSec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID PS_FILE_ID_SC_CONTEXT_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


#if (VOS_WIN32 == VOS_OS_VER)
static SC_CONTEXT_STRU                  g_stScCtx = {
    /* SC_COMM_GLOBAL_VAR */
    {
        {
            ".\\CKFile.bin",
            ".\\DKFile.bin",
            ".\\AKFile.bin",
            ".\\PIFile.bin",
            ".\\ImeiFile_I0.bin",
            ".\\ImeiFile_I1.bin"
        },
        {
            ".\\CKSign.hash",
            ".\\DKSign.hash",
            ".\\AKSign.hash",
            ".\\PISign.hash",
            ".\\ImeiFile_I0.hash",
            ".\\ImeiFile_I1.hash"
        }
    },
    /* SC_FAC_GLOBAL_VAR */
    {
        SC_AUTH_STATUS_UNDO,
        "",
        VOS_FALSE,
        ""
    },
    /* SC_APSEC_GLOBAL_VAR */
    {
        0,
        {0, 0},
        0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        SC_SECURITY_FILE_STATE_IDEL,
        {
            {0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},
            {0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR}
        },
        ".\\SecureDataA.bin",
        ".\\SecureDataC.bin"
    }
};
#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
static SC_CONTEXT_STRU                  g_stScCtx = {
    /* SC_COMM_GLOBAL_VAR */
    {
        {
            "/mnvm2:0/SC/Pers/CKFile.bin",
            "/mnvm2:0/SC/Pers/DKFile.bin",
            "/mnvm2:0/SC/Pers/AKFile.bin",
            "/mnvm2:0/SC/Pers/PIFile.bin",
            "/mnvm2:0/SC/Pers/ImeiFile_I0.bin",
            "/mnvm2:0/SC/Pers/ImeiFile_I1.bin"
        },
        {
            "/mnvm2:0/SC/Pers/CKSign.hash",
            "/mnvm2:0/SC/Pers/DKSign.hash",
            "/mnvm2:0/SC/Pers/AKSign.hash",
            "/mnvm2:0/SC/Pers/PISign.hash",
            "/mnvm2:0/SC/Pers/ImeiFile_I0.hash",
            "/mnvm2:0/SC/Pers/ImeiFile_I1.hash"
        }
    },
    /* SC_FAC_GLOBAL_VAR */
    {
        SC_AUTH_STATUS_UNDO,
        "",
        VOS_FALSE,
        ""
    },
    /* SC_APSEC_GLOBAL_VAR */
    {
        0,
        {0, 0},
        0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        SC_SECURITY_FILE_STATE_IDEL,
        {
            {0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},
            {0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR}
        },
        "/mnvm2:0/SC/Apsec/SecureDataA.bin",
        "/mnvm2:0/SC/Apsec/SecureDataC.bin"
    }
};
#else
static SC_CONTEXT_STRU                  g_stScCtx = {
    /* SC_COMM_GLOBAL_VAR */
    {
        {
            "/yaffs0/SC/Pers/CKFile.bin",
            "/yaffs0/SC/Pers/DKFile.bin",
            "/yaffs0/SC/Pers/AKFile.bin",
            "/yaffs0/SC/Pers/PIFile.bin",
            "/yaffs0/SC/Pers/ImeiFile_I0.bin",
            "/yaffs0/SC/Pers/ImeiFile_I1.bin"
        },
        {
            "/yaffs0/SC/Pers/CKSign.hash",
            "/yaffs0/SC/Pers/DKSign.hash",
            "/yaffs0/SC/Pers/AKSign.hash",
            "/yaffs0/SC/Pers/PISign.hash",
            "/yaffs0/SC/Pers/ImeiFile_I0.hash",
            "/yaffs0/SC/Pers/ImeiFile_I1.hash"
        }
    },
    /* SC_FAC_GLOBAL_VAR */
    {
        SC_AUTH_STATUS_UNDO,
        "",
        VOS_FALSE,
        ""
    },
    /* SC_APSEC_GLOBAL_VAR */
    {
        0,
        {0, 0},
        0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        SC_SECURITY_FILE_STATE_IDEL,
        {
            {0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},
            {0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR},{0,VOS_NULL_PTR}
        },
        "/yaffs0/SC/Apsec/SecureDataA.bin",
        "/yaffs0/SC/Apsec/SecureDataC.bin"
    }
};
#endif
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/


SC_CONTEXT_STRU* SC_CTX_GetScCtxAddr(VOS_VOID)
{
    return &(g_stScCtx);
}


SC_COMM_GLOBAL_STRU*  SC_CTX_GetCommGlobalVarAddr( VOS_VOID )
{
    return &g_stScCtx.stScCommGlobalVar;
}


SC_FAC_GLOBAL_STRU*  SC_CTX_GetFacGlobalVarAddr( VOS_VOID )
{
    return &g_stScCtx.stScFacGlobalVar;
}


VOS_VOID  SC_CTX_InitFacGlobalVar( VOS_VOID )
{
    g_stScCtx.stScFacGlobalVar.enScFacAuthStatus = SC_AUTH_STATUS_UNDO;

    VOS_MemSet(g_stScCtx.stScFacGlobalVar.aucScFacRandData, 0, SC_RAND_DATA_LEN);

    return;
}


SC_AUTH_STATUS_ENUM_UINT8  SC_CTX_GetFacAuthStatus( VOS_VOID )
{
    return g_stScCtx.stScFacGlobalVar.enScFacAuthStatus;
}


VOS_VOID  SC_CTX_SetFacAuthStatus( SC_AUTH_STATUS_ENUM_UINT8 enScAuthStatus )
{
    g_stScCtx.stScFacGlobalVar.enScFacAuthStatus = enScAuthStatus;

    return;
}


VOS_UINT8  SC_CTX_GetIdentifyStartFlg( VOS_VOID )
{
    return g_stScCtx.stScFacGlobalVar.ucIdentifyStartFlg;
}


VOS_VOID  SC_CTX_SetIdentifyStartFlg( VOS_UINT8 ucIdentifyStartFlg )
{
    g_stScCtx.stScFacGlobalVar.ucIdentifyStartFlg   = ucIdentifyStartFlg;

    return;
}


VOS_UINT8*  SC_CTX_GetFacRandDataAddr( VOS_VOID )
{
    return g_stScCtx.stScFacGlobalVar.aucScFacRandData;
}


VOS_UINT32*  SC_CTX_GetApSecInitStateAddr(VOS_VOID)
{
    return &g_stScCtx.stScApSecGlobalVar.ulInitState;
}


SC_APSEC_ITEM_STRU*  SC_CTX_GetApSecItemBaseAddr(VOS_VOID)
{
    return g_stScCtx.stScApSecGlobalVar.astApSecItem;
}


VOS_UINT8 SC_CTX_GetSecATkeyFlg(VOS_VOID)
{
    return g_stScCtx.stScApSecGlobalVar.ucSecATkeyFlg;
}


VOS_VOID SC_CTX_SetSecATkeyFlg(VOS_UINT8 ucSecATkeyFlg)
{
    g_stScCtx.stScApSecGlobalVar.ucSecATkeyFlg  = ucSecATkeyFlg;
    return;
}


VOS_UINT8* SC_CTX_GetSecATkeyAddr(VOS_VOID)
{
    return g_stScCtx.stScApSecGlobalVar.aucSecATkey;
}


VOS_UINT8 SC_CTX_GetApSecSeqID(VOS_VOID)
{
    return g_stScCtx.stScApSecGlobalVar.ucSeqID;
}


VOS_VOID SC_CTX_SetApSecSeqID(VOS_UINT8 ucSeqID)
{
    g_stScCtx.stScApSecGlobalVar.ucSeqID  = ucSeqID;
    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

