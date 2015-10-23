/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: ../TARGET/config/v_iddef.inc                                    */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version:  1.0                                                             */
/*                                                                           */
/* Date:                                                                     */
/*                                                                           */
/* Description: create this file                                             */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef _V_IDDEF_INC
#define _V_IDDEF_INC

/* FIDæ≤Ã¨–≈œ¢±Ì */
FID_Static_Table_Type g_aFidStaticTable[VOS_FID_BUTT] =
{
    {  DOPRA_FID_SYS, VOS_NULL_PTR, 0, 0},

    DEFINE_PRODUCT_FID_TABLE
};

/* modify this to config memmory */
/* defination of message buffer */
VOS_MEM_BLOCK_INFO MsgBlockInfo[VOS_MEM_CTRL_BLOCK_NUMBER] =
{
    { 128,     10 },
    { 512,     2  },
};


#endif /* _V_IDDEF_INC */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


