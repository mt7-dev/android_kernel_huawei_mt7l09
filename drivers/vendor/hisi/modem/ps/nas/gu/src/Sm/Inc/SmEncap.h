/*******************************************************************************
  Copyright   : 2005-2007, Huawei Tech. Co., Ltd.
  File name   : SmEncap.h
  Description : 封装函数
  History     :
      1.  张志勇      2003.12.04   新版作成
      2.  张志勇   2005.01.18   移植修改
*******************************************************************************/

#ifndef _SM_ENCAP_H_
#define _SM_ENCAP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif                                                                          /* __cpluscplus                             */
#endif                                                                          /* __cpluscplus                             */

#define SM_TafFree(p)               PS_SM_MEM_FREE(VOS_MEMPOOL_INDEX_NAS,(p))
#define SM_RabmFree(p)              PS_SM_MEM_FREE(VOS_MEMPOOL_INDEX_NAS,(p))
#define SM_GmmFree(p)               PS_SM_MEM_FREE(VOS_MEMPOOL_INDEX_NAS,(p))
#define SM_Free(p)                  PS_SM_MEM_FREE(VOS_MEMPOOL_INDEX_NAS,(p))
#define SM_Malloc(length)           PS_SM_MEM_ALLOC(VOS_MEMPOOL_INDEX_NAS,(length),WUEPS_MEM_WAIT)
#define SM_MemCpy(p1,p2,length)     PS_NAS_MEM_CPY((p1),(p2),(length))
#define SM_MemSet(p,value,length)   PS_NAS_MEM_SET((p),(value),(length))

#define SM_NULL                0


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif                                                                          /* __cpluscplus                             */

#endif
