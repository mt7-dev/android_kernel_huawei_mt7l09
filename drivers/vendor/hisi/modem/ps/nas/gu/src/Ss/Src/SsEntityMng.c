/*******************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : SsEntityMng.c
  Description  : SS实体管理
  Function List:
      1.  Ss_EntityBuildTi      选择TI
      2.  Ss_EntityBuildCr      选择CR
      3.  Ss_EntityCreat        创建实体
      4.  Ss_EntityClear        释放实体
      5.  Ss_EntitySelectTi     根据CR和TAF ID选择其对应的TI
      6.  Ss_AribMalloc         为生成ARIB结构提供内存空间
      7.  Ss_AribFree           释放掉生成ARIB结构过程中申请的内存
  History:
      1.  张志勇 2004.03.08   新版作成
      2.  丁  庆 2006.11.07   问题单A32D07063
*******************************************************************************/

#include "SsInclude.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_SS_ENTITYMNG_C


/***********************************************************************
*  MODULE   : Ss_EntityBuildTi
*  FUNCTION : 为MO过程中的SS实体选择一个新的TI
*  INPUT    : VOS_UINT8             ucCr        消息中的CR
*             VOS_UINT8             ucTafId     消息中的TAF ID
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_UINT8             ucTi    所选择的TI
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
************************************************************************/

VOS_UINT8 Ss_EntityBuildTi(
                            VOS_UINT8 ucCr
                       )
{
    VOS_UINT8   i;

    for( i = 8 ; i < SS_MAX_ENTITY_NUM ; i++ )
    {                                                                           /* 循环考察所有TI                           */
        if(( ucCr == g_SsEntity[i].ucCr )
            && ( SS_S_NULL != g_SsEntity[i].ucState ))
        {                                                                       /* 判断指定TI是否已存在                     */
            return SS_NONE_TI;                                                  /* 返回没找到TI                             */
        }
    }

    for( i = 8 ; i < SS_MAX_ENTITY_NUM ; i++ )
    {                                                                           /* 循环考察所有TI                           */
        if( SS_S_NULL == g_SsEntity[i].ucState )
        {                                                                       /* 判断是否存在空闲的TI                     */
            return i;                                                           /* 返回此TI                                 */
        }
    }
    return SS_NONE_TI;                                                          /* 返回没找到TI                             */
}

/***********************************************************************
*  MODULE   : Ss_EntityBuildCr
*  FUNCTION : 为MT过程中的SS实体选择一个新的CR
*  INPUT    : VOS_UINT8     ucTi        指定实体的TI
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_UINT8     ucCr        所选择的CR
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
************************************************************************/

VOS_UINT8 Ss_EntityBuildCr( VOS_UINT8 ucTi )
{
    if( SS_S_NULL == g_SsEntity[ucTi].ucState )
    {                                                                           /* 判断指定TI是否已存在                     */
        return ((VOS_UINT8)( ucTi + 128 ));                                         /* 返回没找到TI                             */
    }
    return SS_NONE_CR;                                                          /* 返回没找到CR                             */
}

/***********************************************************************
*  MODULE   : Ss_EntityCreat
*  FUNCTION : 创建实体
*  INPUT    : VOS_UINT8     ucTi        指定实体的TI
*             VOS_UINT8     ucCr        指定实体对应的CR
*             VOS_UINT8     ucTafId     指定实体对应的TAF ID
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_VOID
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
************************************************************************/

VOS_VOID Ss_EntityCreat(
                    VOS_UINT8 ucTi,
                    VOS_UINT8 ucCr
                    )
{
    SS_NORM_LOG( "\n Create SS entity()\r");
    g_SsEntity[ucTi].ucCr = ucCr;                                               /* 设置CR                                   */
}

/***********************************************************************
*  MODULE   : Ss_EntityClear
*  FUNCTION : 释放实体
*  INPUT    : VOS_UINT8     ucTi        指定实体的TI
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_VOID
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
*     2.  张志勇 04-03-23  增加停止T551的处理
************************************************************************/


VOS_VOID Ss_EntityClear(
                    VOS_UINT8 ucTi
                    )
{
    g_SsEntity[ucTi].ucState = SS_S_NULL;                                       /* 设置实体状态                             */
    SS_NORM_LOG( "\n SS state = SS_S_NULL() \r" );
    SS_NORM_LOG( "\n Destroy SS entity()\r" );
    g_SsEntity[ucTi].ucCr = SS_NONE_CR;                                         /* 清除CR                                   */
}


/***********************************************************************
*  MODULE   : Ss_EntitySelectTi
*  FUNCTION : 根据CR和TAF ID选择其对应的TI
*  INPUT    : VOS_UINT8     ucCr        指定实体对应的CR
*             VOS_UINT8     ucTafId     指定实体对应的TAF ID
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_UINT8     ucTi;
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
*     2.
************************************************************************/

VOS_UINT8 Ss_EntitySelectTi(
                            VOS_UINT8 ucCr
                        )
{
    VOS_UINT8   i;                                                                  /* 用于循环                                 */
    VOS_UINT8   ucTi;                                                               /* 定义用于返回的TI                         */

    ucTi = SS_NONE_TI;                                                          /* 初始化TI                                 */

    if( ucCr <= SS_MO_CR_NUM )
    {                                                                           /* MO过程                                   */
        for( i = 8 ; i < SS_MAX_ENTITY_NUM ; i++ )
        {                                                                       /* 遍历MO过程的所有SS实体                   */
            if( ucCr == g_SsEntity[i].ucCr )
            {                                                                   /* 找到了指定的CR和TAF ID                   */
                if( SS_S_CONNECTED == g_SsEntity[i].ucState )
                {                                                               /* 对应的SS实体的状态为SS_S_CONNECTED       */
                    ucTi = i;                                                   /* 返回该TI                                 */
                }
                else
                {                                                               /* 实体状态不对                             */
                    SS_WARN_LOG("\n [Error]SsEntity State Error! \r" );
                }
                break;                                                          /* 跳出循环                                 */
            }
        }
    }
    else
    {                                                                           /* MT过程                                   */
        for( i = 0 ; i < SS_MT_ENTITY_NUM ; i++ )
        {                                                                       /* 遍历MT过程的所有SS实体                   */
            if( ucCr == g_SsEntity[i].ucCr )
            {                                                                   /* 找到了指定的CR                           */
                if( SS_S_CONNECTED == g_SsEntity[i].ucState )
                {                                                               /* 对应的SS实体的状态为SS_S_CONNECTED       */
                        ucTi = i;                                               /* 返回该TI                                 */
                    }
                else
                {                                                               /* 实体状态不对                             */
                    SS_WARN_LOG("\n [Error]SsEntity State Error! \r" );
                }
                break;                                                          /* 跳出循环                                 */
            }
        }
    }
    return ucTi;                                                                /* 返回找到的TI                             */
}

VOS_UINT8 Ss_GetSsTi(VOS_UINT8  ucCr)
{
    VOS_UINT8  i;
    VOS_UINT8  ucTi = SS_NONE_TI;

    /* 网络发起USSD业务要求用户确认，用户不确认至超时处理时Cr范围是8-14,转换成128-134 */
    if ((ucCr > (TAF_MIDDLE_SSA_TI))
     && (ucCr <= TAF_MAX_SSA_TI))
    {
        ucCr += 120;
    }

    for( i = 0 ; i < SS_MAX_ENTITY_NUM ; i++ )
    {
        if( ucCr == g_SsEntity[i].ucCr )
        {
            ucTi = i;
            break;
        }
    }

    return ucTi;
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

