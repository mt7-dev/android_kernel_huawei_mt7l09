

#ifndef __SOCPINTERFACE_H__
#define __SOCPINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "DrvInterface.h"


#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
#if 0
static VOS_INT32 BSP_SOCP_EncDstBufFlush(VOS_VOID)
{
	return DRV_SOCP_ENC_DST_BUF_FLUSH();
}

static VOS_UINT32 BSP_SOCP_Start(VOS_UINT32 u32SrcChanID)
{
	return DRV_SOCP_START(u32SrcChanID);
}
static VOS_UINT32 BSP_SOCP_ReadRDDone(VOS_UINT32 u32SrcChanID, VOS_UINT32 u32RDSize)
{
	return DRV_SOCP_READ_RD_DONE(u32SrcChanID,u32RDSize);
}

static VOS_UINT32 BSP_SOCP_WriteDone(VOS_UINT32 u32SrcChanID, VOS_UINT32 u32WrtSize)
{
	return DRV_SOCP_WRITE_DONE(u32SrcChanID,u32WrtSize);
}

static VOS_UINT32 BSP_SOCP_GetRDBuffer( VOS_UINT32 u32SrcChanID,SOCP_BUFFER_RW_S *pBuff)
{
	return DRV_SOCP_GET_RD_BUFFER(u32SrcChanID,pBuff);
}

static VOS_UINT32 BSP_SOCP_GetWriteBuff( VOS_UINT32 u32SrcChanID, SOCP_BUFFER_RW_S *pBuff)
{
	return DRV_SOCP_GET_WRITE_BUFF(u32SrcChanID,pBuff);
}

static VOS_UINT32 BSP_SOCP_CoderSetSrcChan(SOCP_CODER_SRC_ENUM_U32 enSrcChanID,SOCP_CODER_SRC_CHAN_S *pSrcAttr)
{
	return DRV_SOCP_CORDER_SET_SRC_CHAN(enSrcChanID,pSrcAttr);
}

#endif

#define BSP_SOCP_EncDstBufFlush() DRV_SOCP_ENC_DST_BUF_FLUSH()

#define BSP_SOCP_Start(u32SrcChanID) DRV_SOCP_START(u32SrcChanID)

#define BSP_SOCP_ReadRDDone(u32SrcChanID,u32RDSize) DRV_SOCP_READ_RD_DONE(u32SrcChanID,u32RDSize)

#define BSP_SOCP_WriteDone(u32SrcChanID,u32WrtSize) DRV_SOCP_WRITE_DONE(u32SrcChanID,u32WrtSize)

#define BSP_SOCP_GetRDBuffer(u32SrcChanID,pBuff) DRV_SOCP_GET_RD_BUFFER(u32SrcChanID,pBuff)

#define BSP_SOCP_GetWriteBuff(u32SrcChanID, pBuff) DRV_SOCP_GET_WRITE_BUFF(u32SrcChanID,pBuff)

#define BSP_SOCP_CoderSetSrcChan(enSrcChanID, pSrcAttr) DRV_SOCP_CORDER_SET_SRC_CHAN(enSrcChanID,pSrcAttr)




/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of SOCPInterface.h */
