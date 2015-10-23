

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "TafMtaMntn.h"
#include "AtMtaInterface.h"
#include "TafSdcLib.h"
#include "TafMtaTimerMgmt.h"
#include "TafMtaMain.h"
#include "TafMtaCtx.h"
#include "TafMtaComm.h"
#include "DrvInterface.h"
#include "NasOmInterface.h"
#include "NasMtaInterface.h"
#include "TafAgentInterface.h"
#include "DrvInterface.h"

#include "NasErrorLog.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID                 PS_FILE_ID_TAF_MTA_MNTN_C
/*lint +e767 */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID TAF_MTA_RcvQryWrrAutotestReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq       = VOS_NULL_PTR;
    MTA_WRR_AUTOTEST_PARA_STRU         *pstRrcAtQryPara = VOS_NULL_PTR;
    MTA_AT_WRR_AUTOTEST_QRY_CNF_STRU    stQryCnf;
    VOS_UINT32                          ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryReq          = (AT_MTA_MSG_STRU *)pMsg;
    pstRrcAtQryPara    = (MTA_WRR_AUTOTEST_PARA_STRU *)pstQryReq->aucContent;
    ulResult           = VOS_ERR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF))
    {
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                      ID_MTA_AT_WRR_AUTOTEST_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndWrrQryAutotestReqMsg(pstRrcAtQryPara);
    }

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                         ID_MTA_AT_WRR_AUTOTEST_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF,
                        TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvQryWrrCellInfoReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryCellinfoReq = VOS_NULL_PTR;
    MTA_AT_WRR_CELLINFO_QRY_CNF_STRU    stQryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulAtCmdCellInfo;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryCellinfoReq      = (AT_MTA_MSG_STRU *)pMsg;
    ulResult               = VOS_ERR;

    PS_MEM_CPY(&ulAtCmdCellInfo, pstQryCellinfoReq->aucContent, sizeof(VOS_UINT32));

    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    /* 如果当前定时器已启动，则给AT回复查询失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF))
    {
        stQryCnf.ulResult = VOS_ERR;
        TAF_MTA_SndAtMsg(&pstQryCellinfoReq->stAppCtrl,
                        ID_MTA_AT_WRR_CELLINFO_QRY_CNF,
                        sizeof(stQryCnf),
                        (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        ulResult = TAF_MTA_SndWrrQryCellinfoReqMsg(ulAtCmdCellInfo);
    }

    /* 如果当前的Utran模式不是FDD模式,ulResult为VOS_ERR，
       或者给接入层发送消息失败,ulResult也为VOS_ERR，给AT回复查询失败 */
    if (VOS_OK != ulResult)
    {
        /* 消息发送失败，给at回复失败*/
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryCellinfoReq->stAppCtrl,
                         ID_MTA_AT_WRR_CELLINFO_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );

        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF, TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF,
                                     (VOS_UINT8*)&pstQryCellinfoReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvQryWrrMeanrptReq(
    VOS_VOID                           *pMsg
)
{

    AT_MTA_MSG_STRU                    *pstQryReq = VOS_NULL_PTR;
    MTA_AT_WRR_MEANRPT_QRY_CNF_STRU     stQryCnf;
    VOS_UINT32                          ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryReq           = (AT_MTA_MSG_STRU *)pMsg;
    ulResult            = VOS_ERR;
    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    /* 如果当前定时器已启动，则给AT回复查询失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF))
    {
        stQryCnf.ulResult = VOS_ERR;
        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                      ID_MTA_AT_WRR_MEANRPT_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndWrrQryMeanrptReqMsg();
    }

    if (VOS_OK != ulResult)
    {
        /* 消息发送失败，给at回复失败*/
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                         ID_MTA_AT_WRR_MEANRPT_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );

        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF, TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvSetWrrFreqlockReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq       = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_CTRL_STRU      *pstRrcAtSetPara = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_SET_CNF_STRU    stQryCnf;
    VOS_UINT32                          ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryReq          = (AT_MTA_MSG_STRU *)pMsg;
    pstRrcAtSetPara    = (MTA_AT_WRR_FREQLOCK_CTRL_STRU *)pstQryReq->aucContent;
    ulResult           = VOS_ERR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF))
    {
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                      ID_MTA_AT_WRR_FREQLOCK_SET_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndWrrSetFreqlockReqMsg((MTA_WRR_FREQLOCK_CTRL_STRU *)pstRrcAtSetPara);
    }

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                         ID_MTA_AT_WRR_FREQLOCK_SET_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF,
                        TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvSetWrrRrcVersionReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                                        *pstSetReq = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_SET_CNF_STRU                     stSetCnf;
    VOS_UINT32                                              ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstSetReq = (AT_MTA_MSG_STRU *)pMsg;
    ulResult  = VOS_ERR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF))
    {
        PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));
        stSetCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                      ID_MTA_AT_WRR_RRC_VERSION_SET_CNF,
                      sizeof(stSetCnf),
                      (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndWrrSetVersionReqMsg(pstSetReq->aucContent[0]);
    }

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));
        stSetCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                         ID_MTA_AT_WRR_RRC_VERSION_SET_CNF,
                         sizeof(stSetCnf),
                         (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF,
                       TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF,
                                     (VOS_UINT8*)&pstSetReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvSetWrrCellsrhReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                                        *pstSetReq = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_SET_CNF_STRU                         stSetCnf;
    VOS_UINT32                                              ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstSetReq = (AT_MTA_MSG_STRU *)pMsg;
    ulResult  = VOS_ERR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF))
    {
        PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));
        stSetCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                      ID_MTA_AT_WRR_CELLSRH_SET_CNF,
                      sizeof(stSetCnf),
                      (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndWrrSetCellsrhReqMsg(pstSetReq->aucContent[0]);
    }

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));
        stSetCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                         ID_MTA_AT_WRR_CELLSRH_SET_CNF,
                         sizeof(stSetCnf),
                         (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF,
                       TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF,
                                     (VOS_UINT8*)&pstSetReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvQryWrrFreqlockReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstQryReq = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_QRY_CNF_STRU    stQryCnf;
    VOS_UINT32                          ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryReq          = (AT_MTA_MSG_STRU *)pMsg;
    ulResult           = VOS_ERR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF))
    {
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                      ID_MTA_AT_WRR_FREQLOCK_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndWrrQryFreqlockReqMsg();
    }

    if (ulResult != VOS_OK)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                         ID_MTA_AT_WRR_FREQLOCK_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF,
                        TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvQryWrrRrcVersionReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                                        *pstQryVersionReq = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_QRY_CNF_STRU                     stQryCnf;
    VOS_UINT32                                              ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryVersionReq = (AT_MTA_MSG_STRU *)pMsg;
    ulResult         = VOS_ERR;

    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    /* 如果当前定时器已启动，则给AT回复查询失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF))
    {
        stQryCnf.ulResult = VOS_ERR;
        TAF_MTA_SndAtMsg(&pstQryVersionReq->stAppCtrl,
                      ID_MTA_AT_WRR_RRC_VERSION_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        ulResult = TAF_MTA_SndWrrQryRrcVersionReqMsg();
    }

    /* 如果当前的Utran模式不是FDD模式,ulResult为VOS_ERR，
       或者给接入层发送消息失败,ulResult也为VOS_ERR，给AT回复查询失败 */
    if (VOS_OK != ulResult)
    {
        /* 消息发送失败，给at回复失败*/
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryVersionReq->stAppCtrl,
                         ID_MTA_AT_WRR_RRC_VERSION_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF, TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF,
                                     (VOS_UINT8*)&pstQryVersionReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvQryWrrCellsrhReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                                        *pstQryVersionReq = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_QRY_CNF_STRU                         stQryCnf;
    VOS_UINT32                                              ulResult;
    /* 不用UtranMode判断是否给was发送消息，改为平台支持FDD，都给WAS发送消息 */

    pstQryVersionReq = (AT_MTA_MSG_STRU *)pMsg;
    ulResult         = VOS_ERR;

    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    /* 如果当前定时器已启动，则给AT回复查询失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF))
    {
        stQryCnf.ulResult = VOS_ERR;
        TAF_MTA_SndAtMsg(&pstQryVersionReq->stAppCtrl,
                      ID_MTA_AT_WRR_CELLSRH_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持W模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportUtranFdd() )
    {
        ulResult = TAF_MTA_SndWrrQryCellsrhReqMsg();
    }

    /* 如果当前的Utran模式不是FDD模式,ulResult为VOS_ERR，
       或者给接入层发送消息失败,ulResult也为VOS_ERR，给AT回复查询失败 */
    if (VOS_OK != ulResult)
    {
        /* 消息发送失败，给at回复失败*/
        stQryCnf.ulResult = VOS_ERR;

        TAF_MTA_SndAtMsg(&pstQryVersionReq->stAppCtrl,
                         ID_MTA_AT_WRR_CELLSRH_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF, TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF,
                                     (VOS_UINT8*)&pstQryVersionReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}
VOS_VOID TAF_MTA_RcvAtSetNCellMonitorReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                            *pstSetReq = VOS_NULL_PTR;
    AT_MTA_NCELL_MONITOR_SET_REQ_STRU          *pstSetRptReq = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU                      stSetCnf;
    MTA_AT_RESULT_ENUM_UINT32                   enResult;

    pstSetReq    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetRptReq = (AT_MTA_NCELL_MONITOR_SET_REQ_STRU *)pstSetReq->aucContent;
    enResult     = MTA_AT_RESULT_ERROR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF))
    {
        PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));
        stSetCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                      ID_MTA_AT_NCELL_MONITOR_SET_CNF,
                      sizeof(stSetCnf),
                      (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 如果平台支持G模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportGsm() )
    {
        /* 收到at命令请求，发请求消息通知as */
        enResult = TAF_MTA_SndGrrNCellMonitorSetReq(pstSetRptReq->ucSwitch);
    }

    if (MTA_AT_RESULT_NO_ERROR != enResult)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));
        stSetCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                         ID_MTA_AT_NCELL_MONITOR_SET_CNF,
                         sizeof(stSetCnf),
                         (VOS_UINT8*)&stSetCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF,
                       TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF,
                                     (VOS_UINT8*)&pstSetReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}


VOS_VOID TAF_MTA_RcvAtQryNCellMonitorReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                            *pstQryReq = VOS_NULL_PTR;
    MTA_AT_NCELL_MONITOR_QRY_CNF_STRU           stQryCnf;
    MTA_AT_RESULT_ENUM_UINT32                   enResult;

    pstQryReq    = (AT_MTA_MSG_STRU *)pMsg;
    enResult     = MTA_AT_RESULT_ERROR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF))
    {
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                      ID_MTA_AT_NCELL_MONITOR_QRY_CNF,
                      sizeof(stQryCnf),
                      (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 如果平台支持G模，则发送请求给as，否则给at回复失败 */
    if ( VOS_TRUE == TAF_SDC_IsPlatformSupportGsm() )
    {
        /* 收到at命令请求，发请求消息通知as */
        enResult = TAF_MTA_SndGrrNCellMonitorQryReq();
    }

    if (MTA_AT_RESULT_NO_ERROR != enResult)
    {
        /* 消息发送失败，给at回复失败*/
        PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));
        stQryCnf.enResult = MTA_AT_RESULT_ERROR;

        TAF_MTA_SndAtMsg(&pstQryReq->stAppCtrl,
                         ID_MTA_AT_NCELL_MONITOR_QRY_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf );
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF,
                       TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF,
                                     (VOS_UINT8*)&pstQryReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}



VOS_VOID TAF_MTA_RcvAtSetJamDetectReq(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstSetReq       = VOS_NULL_PTR;
    AT_MTA_SET_JAM_DETECT_REQ_STRU     *pstJamDetectReq = VOS_NULL_PTR;
    MTA_AT_SET_JAM_DETECT_CNF_STRU      stQryCnf;
    VOS_UINT32                          ulResult;

    pstSetReq         = (AT_MTA_MSG_STRU *)pMsg;
    pstJamDetectReq   = (AT_MTA_SET_JAM_DETECT_REQ_STRU *)(pstSetReq->aucContent);
    ulResult          = VOS_ERR;
    stQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 如果当前定时器已启动，则返回失败 */
    if (TAF_MTA_TIMER_STATUS_RUNING == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF))
    {
        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                         ID_MTA_AT_SET_JAM_DETECT_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf);
        return;
    }

    /* 如果平台支持G模，则发送请求给as，否则给at回复失败 */
    if (VOS_TRUE == TAF_SDC_IsPlatformSupportGsm())
    {
        /* 收到at命令请求，发请求消息通知as */
        ulResult = TAF_MTA_SndGrrSetJamDetectReq(pstJamDetectReq);
    }

    if (VOS_OK != ulResult)
    {
        /* 消息发送失败，给at回复失败*/
        TAF_MTA_SndAtMsg(&pstSetReq->stAppCtrl,
                         ID_MTA_AT_SET_JAM_DETECT_CNF,
                         sizeof(stQryCnf),
                         (VOS_UINT8*)&stQryCnf);
        return;
    }

    /* 启动保护定时器 */
    TAF_MTA_StartTimer(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF,
                       TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF_TIMER_LEN);

    /* 添加消息进等待队列 */
    TAF_MTA_SaveItemInCmdBufferQueue(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF,
                                    (VOS_UINT8*)&pstSetReq->stAppCtrl,
                                     sizeof(AT_APPCTRL_STRU));

    return;
}


VOS_VOID TAF_MTA_RcvWrrAutotestQryCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf            = VOS_NULL_PTR;
    WRR_MTA_AUTOTEST_QRY_CNF_STRU      *pstRcvWrrAutotestCnf = VOS_NULL_PTR;
    MTA_AT_WRR_AUTOTEST_QRY_CNF_STRU    stSndAtAutotestCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrAutotestQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrAutotestQryCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrAutotestCnf                 = (WRR_MTA_AUTOTEST_QRY_CNF_STRU *)pWrrCnfMsg;
    stSndAtAutotestCnf.ulResult          = pstRcvWrrAutotestCnf->ulResult;
    stSndAtAutotestCnf.stWrrAutoTestRslt.ulRsltNum = pstRcvWrrAutotestCnf->stWrrAutoTestRslt.ulRsltNum;
    PS_MEM_CPY(stSndAtAutotestCnf.stWrrAutoTestRslt.aulRslt,
               pstRcvWrrAutotestCnf->stWrrAutoTestRslt.aulRslt,
               sizeof(pstRcvWrrAutotestCnf->stWrrAutoTestRslt.aulRslt));

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_AUTOTEST_QRY_CNF,
                     sizeof(stSndAtAutotestCnf),
                     (VOS_UINT8*)&stSndAtAutotestCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrCellInfoQryCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf            = VOS_NULL_PTR;
    WRR_MTA_CELLINFO_QRY_CNF_STRU      *pstRcvWrrCellinfoCnf = VOS_NULL_PTR;
    MTA_AT_WRR_CELLINFO_QRY_CNF_STRU    stSndAtCellinfoQryCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrCellInfoQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrCellInfoQryCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrCellinfoCnf           = (WRR_MTA_CELLINFO_QRY_CNF_STRU *)pWrrCnfMsg;
    stSndAtCellinfoQryCnf.ulResult = pstRcvWrrCellinfoCnf->ulResult;
    PS_MEM_CPY(&(stSndAtCellinfoQryCnf.stWrrCellInfo),
                &(pstRcvWrrCellinfoCnf->stWrrCellInfo),
                sizeof(pstRcvWrrCellinfoCnf->stWrrCellInfo));

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_CELLINFO_QRY_CNF,
                     sizeof(stSndAtCellinfoQryCnf),
                     (VOS_UINT8*)&stSndAtCellinfoQryCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrMeanrptQryCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf           = VOS_NULL_PTR;
    WRR_MTA_MEANRPT_QRY_CNF_STRU       *pstRcvWrrMeanrptCnf = VOS_NULL_PTR;
    MTA_AT_WRR_MEANRPT_QRY_CNF_STRU     stSndAtMeanrptQryCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrMeanrptQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrMeanrptQryCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrMeanrptCnf                         = (WRR_MTA_MEANRPT_QRY_CNF_STRU *)pWrrCnfMsg;
    stSndAtMeanrptQryCnf.ulResult               = pstRcvWrrMeanrptCnf->ulResult;
    stSndAtMeanrptQryCnf.stMeanRptRslt.ulRptNum = pstRcvWrrMeanrptCnf->stMeanRptRslt.ulRptNum;
    PS_MEM_CPY((stSndAtMeanrptQryCnf.stMeanRptRslt.astMeanRptInfo), pstRcvWrrMeanrptCnf->stMeanRptRslt.astMeanRptInfo, sizeof(pstRcvWrrMeanrptCnf->stMeanRptRslt.astMeanRptInfo));

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_MEANRPT_QRY_CNF,
                     sizeof(stSndAtMeanrptQryCnf),
                     (VOS_UINT8*)&stSndAtMeanrptQryCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrFreqlockSetCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf            = VOS_NULL_PTR;
    WRR_MTA_FREQLOCK_SET_CNF_STRU      *pstRcvWrrFreqlockCnf = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_SET_CNF_STRU    stSndAtFreqlockCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrFreqlockSetCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrFreqlockSetCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrFreqlockCnf        = (WRR_MTA_FREQLOCK_SET_CNF_STRU *)pWrrCnfMsg;
    stSndAtFreqlockCnf.ulResult = pstRcvWrrFreqlockCnf->ulResult;

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_FREQLOCK_SET_CNF,
                     sizeof(stSndAtFreqlockCnf),
                     (VOS_UINT8*)&stSndAtFreqlockCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrVersionSetCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf              = VOS_NULL_PTR;
    WRR_MTA_RRC_VERSION_SET_CNF_STRU                       *pstRcvWrrVersionSetCnf = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_SET_CNF_STRU                     stSndAtVersionSetCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrVersionSetCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrVersionSetCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrVersionSetCnf        = (WRR_MTA_RRC_VERSION_SET_CNF_STRU *)pWrrCnfMsg;
    stSndAtVersionSetCnf.ulResult = pstRcvWrrVersionSetCnf->ulResult;

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_RRC_VERSION_SET_CNF,
                     sizeof(stSndAtVersionSetCnf),
                     (VOS_UINT8*)&stSndAtVersionSetCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrCellsrhSetCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf              = VOS_NULL_PTR;
    WRR_MTA_CELLSRH_SET_CNF_STRU                           *pstRcvWrrCellsrhSetCnf = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_SET_CNF_STRU                         stSndAtCellsrhSetCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrCellsrhSetCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrCellsrhSetCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrCellsrhSetCnf            = (WRR_MTA_CELLSRH_SET_CNF_STRU *)pWrrCnfMsg;
    stSndAtCellsrhSetCnf.ulResult     = pstRcvWrrCellsrhSetCnf->ulResult;

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_CELLSRH_SET_CNF,
                     sizeof(stSndAtCellsrhSetCnf),
                     (VOS_UINT8*)&stSndAtCellsrhSetCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrFreqlockQryCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf            = VOS_NULL_PTR;
    WRR_MTA_FREQLOCK_QRY_CNF_STRU      *pstRcvWrrFreqlockCnf = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_QRY_CNF_STRU    stSndAtFreqlockCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrFreqlockQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrFreqlockQryCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrFreqlockCnf              = (WRR_MTA_FREQLOCK_QRY_CNF_STRU *)pWrrCnfMsg;
    stSndAtFreqlockCnf.ulResult       = pstRcvWrrFreqlockCnf->ulResult;
    stSndAtFreqlockCnf.stFreqLockInfo.ucFreqLockEnable = pstRcvWrrFreqlockCnf->stFreqLockInfo.ucFreqLockEnable;
    stSndAtFreqlockCnf.stFreqLockInfo.usLockedFreq     = pstRcvWrrFreqlockCnf->stFreqLockInfo.usLockedFreq;
    PS_MEM_SET(stSndAtFreqlockCnf.stFreqLockInfo.aucReserved, 0, sizeof(stSndAtFreqlockCnf.stFreqLockInfo.aucReserved));

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_FREQLOCK_QRY_CNF,
                     sizeof(stSndAtFreqlockCnf),
                     (VOS_UINT8*)&stSndAtFreqlockCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrVersionQryCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf              = VOS_NULL_PTR;
    WRR_MTA_RRC_VERSION_QRY_CNF_STRU   *pstRcvWrrVersionQryCnf = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_QRY_CNF_STRU stSndAtVersionQryCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrVersionQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrVersionQryCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrVersionQryCnf            = (WRR_MTA_RRC_VERSION_QRY_CNF_STRU *)pWrrCnfMsg;
    stSndAtVersionQryCnf.ulResult     = pstRcvWrrVersionQryCnf->ulResult;
    stSndAtVersionQryCnf.ucRrcVersion = pstRcvWrrVersionQryCnf->ucRrcVersion;
    PS_MEM_SET((stSndAtVersionQryCnf.aucReserved), 0, sizeof(stSndAtVersionQryCnf.aucReserved));

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_RRC_VERSION_QRY_CNF,
                     sizeof(stSndAtVersionQryCnf),
                     (VOS_UINT8*)&stSndAtVersionQryCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvWrrCellsrhQryCnf(
    VOS_VOID                           *pWrrCnfMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf              = VOS_NULL_PTR;
    WRR_MTA_CELLSRH_QRY_CNF_STRU       *pstRcvWrrCellsrhQryCnf = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_QRY_CNF_STRU     stSndAtCellsrhQryCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrCellsrhQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvWrrCellsrhQryCnf: get command buffer failed!");
        return;
    }

    pstRcvWrrCellsrhQryCnf                = (WRR_MTA_CELLSRH_QRY_CNF_STRU *)pWrrCnfMsg;
    stSndAtCellsrhQryCnf.ulResult         = pstRcvWrrCellsrhQryCnf->ulResult;
    stSndAtCellsrhQryCnf.ucCellSearchType = pstRcvWrrCellsrhQryCnf->ucCellSearchType;
    PS_MEM_SET((stSndAtCellsrhQryCnf.aucReserve), 0, sizeof(stSndAtCellsrhQryCnf.aucReserve));

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_CELLSRH_QRY_CNF,
                     sizeof(stSndAtCellsrhQryCnf),
                     (VOS_UINT8*)&stSndAtCellsrhQryCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvRrcSetNCellMonitorCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf       = VOS_NULL_PTR;
    GRR_MTA_NCELL_MONITOR_SET_CNF_STRU                     *pstGrrMtaSetCnf = VOS_NULL_PTR;
    MTA_AT_UNSOLICITED_RPT_SET_CNF_STRU                     stMtaAtSetCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvRrcSetNCellMonitorCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvRrcSetNCellMonitorCnf: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stMtaAtSetCnf, 0, sizeof(stMtaAtSetCnf));
    pstGrrMtaSetCnf        = (GRR_MTA_NCELL_MONITOR_SET_CNF_STRU *)pMsg;

    if (VOS_OK == pstGrrMtaSetCnf->ulResult)
    {
        stMtaAtSetCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }
    else
    {
        stMtaAtSetCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_NCELL_MONITOR_SET_CNF,
                     sizeof(stMtaAtSetCnf),
                     (VOS_UINT8*)&stMtaAtSetCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvRrcQryNCellMonitorCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf       = VOS_NULL_PTR;
    GRR_MTA_NCELL_MONITOR_QRY_CNF_STRU                     *pstGrrMtaQryCnf = VOS_NULL_PTR;
    MTA_AT_NCELL_MONITOR_QRY_CNF_STRU                       stMtaAtQryCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvRrcQryNCellMonitorCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvRrcQryNCellMonitorCnf: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stMtaAtQryCnf, 0, sizeof(stMtaAtQryCnf));
    pstGrrMtaQryCnf            = (GRR_MTA_NCELL_MONITOR_QRY_CNF_STRU *)pMsg;
    stMtaAtQryCnf.enResult     = pstGrrMtaQryCnf->ulResult;
    stMtaAtQryCnf.ucSwitch     = pstGrrMtaQryCnf->ucSwitch;
    stMtaAtQryCnf.ucNcellState = pstGrrMtaQryCnf->enNcellState;

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_NCELL_MONITOR_QRY_CNF,
                     sizeof(stMtaAtQryCnf),
                     (VOS_UINT8*)&stMtaAtQryCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF);
    return;
}
VOS_VOID TAF_MTA_RcvRrcNCellMonitorInd(
    VOS_VOID                           *pMsg
)
{
    GRR_MTA_NCELL_MONITOR_IND_STRU                     *pstGrrMtaQryCnf = VOS_NULL_PTR;
    MTA_AT_NCELL_MONITOR_IND_STRU                       stMtaAtInd;
    AT_APPCTRL_STRU                                     stAppCtrl;

    /* 填写主动上报的消息内容 */
    PS_MEM_SET(&stMtaAtInd, 0, sizeof(stMtaAtInd));
    pstGrrMtaQryCnf        = (GRR_MTA_NCELL_MONITOR_IND_STRU *)pMsg;
    stMtaAtInd.ucNcellState = pstGrrMtaQryCnf->enNcellState;

    /* 填写消息头，上报类型为广播事件 */
    PS_MEM_SET(&stAppCtrl, 0, sizeof(stAppCtrl));
    stAppCtrl.usClientId = MTA_CLIENTID_BROADCAST;
    stAppCtrl.ucOpId     = MTA_INVALID_TAB_INDEX;

    /* 给at回消息 */
    TAF_MTA_SndAtMsg(&stAppCtrl,
                     ID_MTA_AT_NCELL_MONITOR_IND,
                     sizeof(stMtaAtInd),
                     (VOS_UINT8*)&stMtaAtInd);

    return;
}
VOS_VOID TAF_MTA_RcvRrcJamDetectCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf       = VOS_NULL_PTR;
    RRC_MTA_JAM_DETECT_CNF_STRU        *pstGrrMtaSetCnf = VOS_NULL_PTR;
    MTA_AT_SET_JAM_DETECT_CNF_STRU      stMtaAtSetCnf;

    /* 判断定时器是否运行 */
    if (TAF_MTA_TIMER_STATUS_STOP == TAF_MTA_GetTimerStatus(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvRrcJamDetectCnf: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvRrcJamDetectCnf: get command buffer failed!");
        return;
    }

    pstGrrMtaSetCnf = (RRC_MTA_JAM_DETECT_CNF_STRU *)pMsg;

    PS_MEM_SET(&stMtaAtSetCnf, 0, sizeof(stMtaAtSetCnf));

    if (MTA_RRC_RESULT_NO_ERROR == pstGrrMtaSetCnf->enResult)
    {
        stMtaAtSetCnf.enResult = MTA_AT_RESULT_NO_ERROR;
    }
    else
    {
        stMtaAtSetCnf.enResult = MTA_AT_RESULT_ERROR;
    }

    /* 给at回消息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_SET_JAM_DETECT_CNF,
                     sizeof(stMtaAtSetCnf),
                     (VOS_UINT8*)&stMtaAtSetCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF);
    return;

}
VOS_VOID TAF_MTA_RcvRrcJamDetectInd(
    VOS_VOID                           *pMsg
)
{
    RRC_MTA_JAM_DETECT_IND_STRU        *pstJamDetectInd = VOS_NULL_PTR;
    MTA_AT_JAM_DETECT_IND_STRU          stMtaAtInd;
    AT_APPCTRL_STRU                     stAppCtrl;
    NV_NAS_JAM_DETECT_CFG_STRU          stNvJamDetect;

    /* 初始化消息变量 */
    PS_MEM_SET(&stNvJamDetect, 0x0, sizeof(stNvJamDetect));

    pstJamDetectInd = (RRC_MTA_JAM_DETECT_IND_STRU *)pMsg;

    /* 填写主动上报的消息内容 */
    PS_MEM_SET(&stMtaAtInd, 0, sizeof(stMtaAtInd));
    stMtaAtInd.enJamResult = (MTA_AT_JAM_RESULT_ENUM_UINT32)pstJamDetectInd->enResult;

    /* 填写消息头，上报类型为广播事件 */
    PS_MEM_SET(&stAppCtrl, 0, sizeof(stAppCtrl));
    stAppCtrl.usClientId = MTA_CLIENTID_BROADCAST;
    stAppCtrl.ucOpId     = MTA_INVALID_TAB_INDEX;

    /* 通过读取NV来获取Jam Detect当前配置值 */
    if (NV_OK != NV_Read(en_NV_Item_JAM_DETECT_CFG,
                         &stNvJamDetect,
                         sizeof(NV_NAS_JAM_DETECT_CFG_STRU)))
    {
        MTA_ERROR_LOG("TAF_MTA_RcvRrcJamDetectInd: NV_Read en_NV_Item_JAM_DETECT_CFG fail!");
        return;
    }

    /* 如果jam detect功能未使能，即使收到GAS消息，也不再上报 */
    if (VOS_FALSE == stNvJamDetect.ucMode)
    {
        MTA_ERROR_LOG("TAF_MTA_RcvRrcJamDetectInd: Jam Detect function is disabled!");
        return ;
    }

    /* 给at回消息 */
    TAF_MTA_SndAtMsg(&stAppCtrl,
                     ID_MTA_AT_JAM_DETECT_IND,
                     sizeof(stMtaAtInd),
                     (VOS_UINT8*)&stMtaAtInd);

    return;
}


VOS_VOID TAF_MTA_RcvTiReqAutotestQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_AUTOTEST_QRY_CNF_STRU    stQryAutotestCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_AUTOTEST_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqAutotestQryExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stQryAutotestCnf, 0x0, sizeof(stQryAutotestCnf));

    stQryAutotestCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_AUTOTEST_QRY_CNF,
                     sizeof(stQryAutotestCnf),
                     (VOS_UINT8*)&stQryAutotestCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}


VOS_VOID TAF_MTA_RcvTiReqCellInfoQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_CELLINFO_QRY_CNF_STRU    stCellinfoQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLINFO_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqCellInfoQryExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stCellinfoQryCnf, 0x0, sizeof(stCellinfoQryCnf));

    stCellinfoQryCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_CELLINFO_QRY_CNF,
                     sizeof(stCellinfoQryCnf),
                     (VOS_UINT8*)&stCellinfoQryCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}




VOS_VOID TAF_MTA_RcvTiReqMeanrptQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_MEANRPT_QRY_CNF_STRU     stMeanrptQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_MEANRPT_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqMeanrptQryExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stMeanrptQryCnf, 0x0, sizeof(stMeanrptQryCnf));

    stMeanrptQryCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_MEANRPT_QRY_CNF,
                     sizeof(stMeanrptQryCnf),
                     (VOS_UINT8*)&stMeanrptQryCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}


VOS_VOID TAF_MTA_RcvTiReqFreqlockSetExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_SET_CNF_STRU    stSetFreqlockCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqFreqlockSetExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stSetFreqlockCnf, 0x0, sizeof(stSetFreqlockCnf));

    stSetFreqlockCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_FREQLOCK_SET_CNF,
                     sizeof(stSetFreqlockCnf),
                     (VOS_UINT8*)&stSetFreqlockCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;
}


VOS_VOID TAF_MTA_RcvTiReqRrcVersionSetExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_SET_CNF_STRU                     stVersionSetCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqRrcVersionSetExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stVersionSetCnf, 0x0, sizeof(stVersionSetCnf));

    stVersionSetCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_RRC_VERSION_SET_CNF,
                     sizeof(stVersionSetCnf),
                     (VOS_UINT8*)&stVersionSetCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}




VOS_VOID TAF_MTA_RcvTiReqCellsrhSetExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_SET_CNF_STRU                         stCellsrhSetCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqCellsrhSetExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stCellsrhSetCnf, 0x0, sizeof(stCellsrhSetCnf));

    stCellsrhSetCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_CELLSRH_SET_CNF,
                     sizeof(stCellsrhSetCnf),
                     (VOS_UINT8*)&stCellsrhSetCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}



VOS_VOID TAF_MTA_RcvTiReqFreqlockQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_QRY_CNF_STRU    stQryFreqlockCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_FREQLOCK_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqFreqlockQryExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stQryFreqlockCnf, 0x0, sizeof(stQryFreqlockCnf));

    stQryFreqlockCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_FREQLOCK_QRY_CNF,
                     sizeof(stQryFreqlockCnf),
                     (VOS_UINT8*)&stQryFreqlockCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;
}


VOS_VOID TAF_MTA_RcvTiReqRrcVersionQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_QRY_CNF_STRU                     stVersionQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_RRC_VERSION_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqRrcVersionQryExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stVersionQryCnf, 0x0, sizeof(stVersionQryCnf));

    stVersionQryCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_RRC_VERSION_QRY_CNF,
                     sizeof(stVersionQryCnf),
                     (VOS_UINT8*)&stVersionQryCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}




VOS_VOID TAF_MTA_RcvTiReqCellsrhQryExpired(
    VOS_VOID                           *pMsg
)
{
    TAF_MTA_CMD_BUFFER_STRU                                *pstCmdBuf = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_QRY_CNF_STRU                         stCellsrhQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_WRR_CELLSRH_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        /*定时器超时log*/
        MTA_ERROR_LOG("TAF_MTA_RcvTiReqCellsrhQryExpired: get command buffer failed!");
        return;
    }

    PS_MEM_SET(&stCellsrhQryCnf, 0x0, sizeof(stCellsrhQryCnf));

    stCellsrhQryCnf.ulResult = VOS_ERR;

    /* 上报给AT模块错误信息 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_WRR_CELLSRH_QRY_CNF,
                     sizeof(stCellsrhQryCnf),
                     (VOS_UINT8*)&stCellsrhQryCnf );


    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;

}


VOS_VOID TAF_MTA_RcvTiWaitGrrSetNCellMonitorExpired(VOS_VOID *pMsg)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU              stSetCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    PS_MEM_SET(&stSetCnf, 0x0, sizeof(stSetCnf));

    stSetCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 上报给AT模块设置错误 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_NCELL_MONITOR_SET_CNF,
                     sizeof(stSetCnf),
                     (VOS_UINT8*)&stSetCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_SET_CNF);

    return;
}
VOS_VOID TAF_MTA_RcvTiWaitGrrQryNCellMonitorExpired(VOS_VOID *pMsg)
{
    TAF_MTA_CMD_BUFFER_STRU                *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_NCELL_MONITOR_QRY_CNF_STRU       stQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    stQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 上报给AT模块查询错误 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_NCELL_MONITOR_QRY_CNF,
                     sizeof(stQryCnf),
                     (VOS_UINT8*)&stQryCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_GRR_NCELLMONITOR_QRY_CNF);

    return;
}
VOS_VOID TAF_MTA_RcvTiWaitGrrSetJamDetectExpired(VOS_VOID *pMsg)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf   = VOS_NULL_PTR;
    MTA_AT_SET_JAM_DETECT_CNF_STRU      stQryCnf;

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF);

    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    PS_MEM_SET(&stQryCnf, 0x0, sizeof(stQryCnf));

    stQryCnf.enResult = MTA_AT_RESULT_ERROR;

    /* 上报给AT模块查询错误 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                     ID_MTA_AT_SET_JAM_DETECT_CNF,
                     sizeof(stQryCnf),
                     (VOS_UINT8*)&stQryCnf );

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(TI_TAF_MTA_WAIT_JAM_DETECT_SET_CNF);

    return;
}
VOS_UINT32 TAF_MTA_SndWrrQryAutotestReqMsg(
    MTA_WRR_AUTOTEST_PARA_STRU         *pAutotestQryPara
)
{
    VOS_UINT32                          ulLength;
    MTA_WRR_AUTOTEST_QRY_REQ_STRU      *pstMtaWrrQryReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength              = sizeof(MTA_WRR_AUTOTEST_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrQryReq       = (MTA_WRR_AUTOTEST_QRY_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaWrrQryReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryAutotestReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrQryReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrQryReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrQryReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrQryReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrQryReq->ulMsgName         = ID_MTA_WRR_AUTOTEST_QRY_REQ;

    /* 传递给消息包内容 */
    PS_MEM_CPY(&(pstMtaWrrQryReq->stWrrAutotestPara),
        pAutotestQryPara,
        sizeof(MTA_WRR_AUTOTEST_PARA_STRU));

    /* 发送消息到AS */
    if ( VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrQryReq) )
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryAutotestReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MTA_SndWrrQryCellinfoReqMsg(
    VOS_UINT32                          ulSetCellInfo
)
{
    VOS_UINT32                          ulLength;
    MTA_WRR_CELLINFO_QRY_REQ_STRU      *pstMtaWrrQryCellinfoReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength                    = sizeof(MTA_WRR_CELLINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrQryCellinfoReq     = (MTA_WRR_CELLINFO_QRY_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);
    if (VOS_NULL_PTR == pstMtaWrrQryCellinfoReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryCellinfoReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrQryCellinfoReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrQryCellinfoReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrQryCellinfoReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrQryCellinfoReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrQryCellinfoReq->ulMsgName         = ID_MTA_WRR_CELLINFO_QRY_REQ;
    pstMtaWrrQryCellinfoReq->ulSetCellInfo     = ulSetCellInfo;

    /* 发送消息到AS */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrQryCellinfoReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryCellinfoReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndWrrQryMeanrptReqMsg( VOS_VOID )
{
    VOS_UINT32                          ulLength;
    MTA_WRR_MEANRPT_QRY_REQ_STRU       *pstMtaWrrQryMeanrptReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength                    = sizeof(MTA_WRR_MEANRPT_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrQryMeanrptReq      = (MTA_WRR_MEANRPT_QRY_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);
    if (VOS_NULL_PTR == pstMtaWrrQryMeanrptReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryMeanrptReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrQryMeanrptReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrQryMeanrptReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrQryMeanrptReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrQryMeanrptReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrQryMeanrptReq->ulMsgName         = ID_MTA_WRR_MEASRPT_QRY_REQ;

    /* 发送消息到AS */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrQryMeanrptReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryMeanrptReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndWrrSetFreqlockReqMsg(
    MTA_WRR_FREQLOCK_CTRL_STRU         *pFreqlockCtrlPara
)
{
    VOS_UINT32                          ulLength;
    MTA_WRR_FREQLOCK_SET_REQ_STRU      *pstMtaWrrSetReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength              = sizeof(MTA_WRR_FREQLOCK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrSetReq       = (MTA_WRR_FREQLOCK_SET_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaWrrSetReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrSetFreqlockReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrSetReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrSetReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrSetReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrSetReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrSetReq->ulMsgName         = ID_MTA_WRR_FREQLOCK_SET_REQ;

    /* 传递给消息包内容 */
    PS_MEM_CPY(&(pstMtaWrrSetReq->stFrelock), pFreqlockCtrlPara, sizeof(MTA_WRR_FREQLOCK_CTRL_STRU));

    /* 发送消息到AS */
    if ( VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrSetReq) )
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrSetFreqlockReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MTA_SndWrrSetVersionReqMsg(
    VOS_UINT8                           ucRrcVersion
)
{
    VOS_UINT32                          ulLength;
    MTA_WRR_RRC_VERSION_SET_REQ_STRU   *pstMtaWrrSetReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength              = sizeof(MTA_WRR_RRC_VERSION_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrSetReq       = (MTA_WRR_RRC_VERSION_SET_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaWrrSetReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrSetVersionReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrSetReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrSetReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrSetReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrSetReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrSetReq->ulMsgName         = ID_MTA_WRR_RRC_VERSION_SET_REQ;
    pstMtaWrrSetReq->ucRRCVersion      = ucRrcVersion;

    /* 传递给消息包内容 */
    PS_MEM_SET((pstMtaWrrSetReq->aucReserv), 0, sizeof(pstMtaWrrSetReq->aucReserv));

    /* 发送消息到AS */
    if ( VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrSetReq) )
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrSetVersionReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MTA_SndWrrSetCellsrhReqMsg(
    VOS_UINT8                           ucCellSrh
)
{
    VOS_UINT32                          ulLength;
    MTA_WRR_CELLSRH_SET_REQ_STRU       *pstMtaWrrSetReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength              = sizeof(MTA_WRR_CELLSRH_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrSetReq       = (MTA_WRR_CELLSRH_SET_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaWrrSetReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrSetCellsrhReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrSetReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrSetReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrSetReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrSetReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrSetReq->ulMsgName         = ID_MTA_WRR_CELLSRH_SET_REQ;
    pstMtaWrrSetReq->ucCellSrh         = ucCellSrh;

    /* 传递给消息包内容 */
    PS_MEM_SET((pstMtaWrrSetReq->aucReserve), 0, sizeof(pstMtaWrrSetReq->aucReserve));

    /* 发送消息到AS */
    if ( VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrSetReq) )
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrSetCellsrhReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 TAF_MTA_SndWrrQryFreqlockReqMsg(VOS_VOID)
{

    VOS_UINT32                          ulLength;
    MTA_WRR_FREQLOCK_QRY_REQ_STRU      *pstMtaWrrQryReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength              = sizeof(MTA_WRR_FREQLOCK_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrQryReq       = (MTA_WRR_FREQLOCK_QRY_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMtaWrrQryReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryFreqlockReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrQryReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrQryReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrQryReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrQryReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrQryReq->ulMsgName         = ID_MTA_WRR_FREQLOCK_QRY_REQ;

    /* 发送消息到AS */
    if ( VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrQryReq) )
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryFreqlockReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndWrrQryRrcVersionReqMsg( VOS_VOID )
{
    VOS_UINT32                          ulLength;
    MTA_WRR_RRC_VERSION_QRY_REQ_STRU   *pstMtaWrrQryVersionReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength                    = sizeof(MTA_WRR_RRC_VERSION_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrQryVersionReq      = (MTA_WRR_RRC_VERSION_QRY_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);
    if (VOS_NULL_PTR == pstMtaWrrQryVersionReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryRrcVersionReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrQryVersionReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrQryVersionReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrQryVersionReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrQryVersionReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrQryVersionReq->ulMsgName         = ID_MTA_WRR_RRC_VERSION_QRY_REQ;

    /* 发送消息到AS */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrQryVersionReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryRrcVersionReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndWrrQryCellsrhReqMsg( VOS_VOID )
{
    VOS_UINT32                          ulLength;
    MTA_WRR_CELLSRH_QRY_REQ_STRU       *pstMtaWrrQryCellsrhReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength                    = sizeof(MTA_WRR_CELLSRH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaWrrQryCellsrhReq      = (MTA_WRR_CELLSRH_QRY_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);
    if (VOS_NULL_PTR == pstMtaWrrQryCellsrhReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryCellsrhReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaWrrQryCellsrhReq->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaWrrQryCellsrhReq->ulSenderPid       = UEPS_PID_MTA;
    pstMtaWrrQryCellsrhReq->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaWrrQryCellsrhReq->ulReceiverPid     = WUEPS_PID_WRR;
    pstMtaWrrQryCellsrhReq->ulMsgName         = ID_MTA_WRR_CELLSRH_QRY_REQ;

    /* 发送消息到AS */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaWrrQryCellsrhReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndWrrQryCellsrhReqMsg: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndGrrNCellMonitorSetReq(
    VOS_UINT8                         ucRptCmdStatus
)
{
    MTA_GRR_NCELL_MONITOR_SET_REQ_STRU          *pstNCellMonitorSetReq  = VOS_NULL_PTR;

    /* 分配内存 */
    pstNCellMonitorSetReq = (MTA_GRR_NCELL_MONITOR_SET_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA,
                                                              sizeof(MTA_GRR_NCELL_MONITOR_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 内存分配异常处理 */
    if (VOS_NULL_PTR == pstNCellMonitorSetReq)
    {
        MTA_WARNING_LOG("TAF_MTA_SndGrrNCellMonitorSetReq:alloc msg failed.");
        return VOS_ERR;
    }

    /* 填写新消息内容 */
    PS_MEM_SET(pstNCellMonitorSetReq, 0, sizeof(MTA_GRR_NCELL_MONITOR_SET_REQ_STRU));
    pstNCellMonitorSetReq->ulSenderPid      = UEPS_PID_MTA;
    pstNCellMonitorSetReq->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstNCellMonitorSetReq->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstNCellMonitorSetReq->ulReceiverPid    = UEPS_PID_GAS;
    pstNCellMonitorSetReq->ulMsgName        = ID_MTA_GRR_NCELL_MONITOR_SET_REQ;
    pstNCellMonitorSetReq->ulLength         = sizeof(MTA_GRR_NCELL_MONITOR_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstNCellMonitorSetReq->ucSwitch         = ucRptCmdStatus;

    /* 发送消息异常处理 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstNCellMonitorSetReq))
    {
        MTA_WARNING_LOG("TAF_MTA_SndGrrNCellMonitorSetReq: PS_SEND_MSG ERROR");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 TAF_MTA_SndGrrNCellMonitorQryReq(VOS_VOID)
{
    MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU          *pstNCellMonitorQryReq  = VOS_NULL_PTR;

    /* 分配内存 */
    pstNCellMonitorQryReq = (MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA,
                                                              sizeof(MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 内存分配异常处理 */
    if (VOS_NULL_PTR == pstNCellMonitorQryReq)
    {
        MTA_WARNING_LOG("TAF_MTA_SndGrrNCellMonitorQryReq:alloc msg failed.");
        return VOS_ERR;
    }

    /* 填写新消息内容 */
    PS_MEM_SET(pstNCellMonitorQryReq, 0, sizeof(MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU));
    pstNCellMonitorQryReq->ulSenderPid      = UEPS_PID_MTA;
    pstNCellMonitorQryReq->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstNCellMonitorQryReq->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstNCellMonitorQryReq->ulReceiverPid    = UEPS_PID_GAS;
    pstNCellMonitorQryReq->ulMsgName        = ID_MTA_GRR_NCELL_MONITOR_QRY_REQ;
    pstNCellMonitorQryReq->ulLength         = sizeof(MTA_GRR_NCELL_MONITOR_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 发送消息异常处理 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstNCellMonitorQryReq))
    {
        MTA_WARNING_LOG("TAF_MTA_SndGrrNCellMonitorQryReq: PS_SEND_MSG ERROR");
        return VOS_ERR;
    }

    return VOS_OK;
}



VOS_UINT32 TAF_MTA_SndGrrSetJamDetectReq(
    AT_MTA_SET_JAM_DETECT_REQ_STRU     *pstSetJdrReq
)
{
    VOS_UINT32                          ulLength;
    MTA_RRC_JAM_DETECT_REQ_STRU        *pstMtaGrrJamDetectReq = VOS_NULL_PTR;

    /* 申请消息结构内存 */
    ulLength              = sizeof(MTA_RRC_JAM_DETECT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMtaGrrJamDetectReq = (MTA_RRC_JAM_DETECT_REQ_STRU*)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);
    if (VOS_NULL_PTR == pstMtaGrrJamDetectReq)
    {
        MTA_ERROR_LOG("TAF_MTA_SndGrrSetJamDetectReq: Alloc msg fail!");
        return VOS_ERR;
    }

    /* 构造消息结构体 */
    pstMtaGrrJamDetectReq->stMsgHeader.ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstMtaGrrJamDetectReq->stMsgHeader.ulSenderPid       = UEPS_PID_MTA;
    pstMtaGrrJamDetectReq->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMtaGrrJamDetectReq->stMsgHeader.ulReceiverPid     = UEPS_PID_GAS;
    pstMtaGrrJamDetectReq->stMsgHeader.ulLength          = ulLength;
    pstMtaGrrJamDetectReq->stMsgHeader.ulMsgName         = ID_MTA_RRC_JAM_DETECT_REQ;

    pstMtaGrrJamDetectReq->enMode      = (MTA_RRC_JAM_MODE_ENUM_UINT8)pstSetJdrReq->ucFlag;
    pstMtaGrrJamDetectReq->ucMethod    = pstSetJdrReq->ucMethod;
    pstMtaGrrJamDetectReq->ucThreshold = pstSetJdrReq->ucThreshold;
    pstMtaGrrJamDetectReq->ucFreqNum   = pstSetJdrReq->ucFreqNum;

    /* 发送消息到AS */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMtaGrrJamDetectReq))
    {
        MTA_ERROR_LOG("TAF_MTA_SndGrrSetJamDetectReq: Send msg fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID TAF_MTA_RcvNasAreaLostInd(
    VOS_VOID                           *pMsg
)
{
    VOS_INT32                           lRslt;

    /* 调用底软接口，如果返回0，表示PA正常，不做任何操作。返回非0表示PA异常，则进行钩包 */
#if defined(INSTANCE_1)
    lRslt = DRV_PASTAR_EXC_CHECK((PWC_COMM_MODEM_E)MODEM_ID_1);
#else
    lRslt = DRV_PASTAR_EXC_CHECK((PWC_COMM_MODEM_E)MODEM_ID_0);
#endif

    if (VOS_OK == lRslt)
    {
        return;
    }

    NAS_EventReport(UEPS_PID_MTA,
                    NAS_OM_EVENT_PA_STAR_ABNORMAL,
                    VOS_NULL_PTR,
                    NAS_OM_EVENT_NO_PARA);

    return;
}
VOS_VOID TAF_MTA_RcvTafAgentGetAntStateReq(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT                                ulAntState;
    MODEM_ID_ENUM_UINT16                    enModemId;
    TAFAGENT_MTA_GET_ANT_STATE_CNF_STRU    *pstMsg= VOS_NULL_PTR;

    enModemId   = VOS_GetModemIDFromPid(UEPS_PID_MTA);
    ulAntState  = 0;

    pstMsg = (TAFAGENT_MTA_GET_ANT_STATE_CNF_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA,
                                                                 sizeof(TAFAGENT_MTA_GET_ANT_STATE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_WARNING_LOG("TAF_MTA_RcvTafAgentGetAntStateReq:alloc msg failed.");

        return;
    }

    PS_MEM_SET(((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH), 0, (sizeof(TAFAGENT_MTA_GET_ANT_STATE_CNF_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 消息头 */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = ACPU_PID_TAFAGENT;
    pstMsg->enMsgId         = ID_TAFAGENT_MTA_GET_ANT_STATE_CNF;
    pstMsg->ulRslt          = VOS_ERR;
    pstMsg->usAntState      = 0;

    /* 调用底软接口获取天线状态 */
    if (VOS_OK == DRV_GET_ANTEN_LOCKSTATE((PWC_COMM_MODEM_E)enModemId, &ulAntState))
    {
        pstMsg->ulRslt      = VOS_OK;
        pstMsg->usAntState  = (VOS_UINT16)ulAntState;
    }

    /* 给TAFAGENT发送查询回复 */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvTafAgentGetAntStateReq:send msg failed.");

        return;
    }

    return;
}

#if (FEATURE_ON == FEATURE_PTM)
VOS_VOID TAF_MTA_SndAcpuOmErrLogRptCnf(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulBufUseLen
)
{
    OM_ERR_LOG_REPORT_CNF_STRU         *pstQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;

    /* 上报的消息总长度 */
    ulMsgLen  = sizeof(OM_ERR_LOG_REPORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH - 4 + ulBufUseLen;

    /* 消息空间申请 */
    pstQryCnf = (OM_ERR_LOG_REPORT_CNF_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulMsgLen);
    if (VOS_NULL_PTR == pstQryCnf)
    {
        MTA_ERROR_LOG("TAF_MTA_SndAcpuOmErrLogRptCnf: alloc msg fail!");
        return;
    }

    pstQryCnf->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstQryCnf->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstQryCnf->ulReceiverPid    = ACPU_PID_OM;
    pstQryCnf->ulSenderPid      = UEPS_PID_MTA;
    pstQryCnf->ulMsgName        = ID_OM_ERR_LOG_REPORT_CNF;
    pstQryCnf->ulMsgType        = OM_ERR_LOG_MSG_ERR_REPORT;
    pstQryCnf->ulMsgSN          = 0;
    pstQryCnf->ulRptlen         = ulBufUseLen;

    /* buffer不为空时，len也是不会为空的 */
    if (VOS_NULL_PTR != pbuffer)
    {
        PS_MEM_CPY(pstQryCnf->aucContent, pbuffer, ulBufUseLen);
    }

    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstQryCnf))
    {
        MTA_ERROR_LOG("TAF_MTA_SndAcpuOmErrLogRptCnf: SEND MSG FAIL");
    }

    return;

}
VOS_VOID TAF_MTA_RcvAcpuOmErrLogRptReq(
    VOS_VOID                           *pMsg
)
{
    VOS_CHAR                           *pbuffer   = VOS_NULL_PTR;
    VOS_UINT32                          ulBufUseLen;
    VOS_UINT32                          ulRealLen;

    /* 查询一下RING BUFFER中有多少数据，以便分配内存 */
    ulBufUseLen = TAF_SDC_GetErrLogRingBufferUseBytes();

    /* 如果RING BUFFER中没有数据，也需要给OM发送消息 */
    if (0 == ulBufUseLen)
    {
        /* 发送ID_OM_ERR_LOG_REPORT_CNF内容为空的消息给OM */
        TAF_MTA_SndAcpuOmErrLogRptCnf(VOS_NULL_PTR, 0);
        return;
    }

    pbuffer = (VOS_CHAR *)PS_MEM_ALLOC(UEPS_PID_MTA, ulBufUseLen);
    if (VOS_NULL_PTR == pbuffer)
    {
        /* 发送ID_OM_ERR_LOG_REPORT_CNF内容为空的消息给OM */
        TAF_MTA_SndAcpuOmErrLogRptCnf(VOS_NULL_PTR, 0);
        return;
    }

    PS_MEM_SET(pbuffer, 0, ulBufUseLen);

    /* 获取RING BUFFER的内容 */
    ulRealLen = TAF_SDC_GetErrLogRingBufContent(pbuffer, ulBufUseLen);
    if (ulRealLen != ulBufUseLen)
    {
        /* 发送ID_OM_ERR_LOG_REPORT_CNF内容为空的消息给OM */
        TAF_MTA_SndAcpuOmErrLogRptCnf(VOS_NULL_PTR, 0);
        PS_MEM_FREE(UEPS_PID_MTA, pbuffer);
        return;
    }

    /* 获取完了后需要将RINGBUFFER清空 */
    TAF_SDC_CleanErrLogRingBuf();

    /* 发送ID_OM_ERR_LOG_REPORT_CNF消息给ACPU OM */
    TAF_MTA_SndAcpuOmErrLogRptCnf(pbuffer, ulBufUseLen);

    PS_MEM_FREE(UEPS_PID_MTA, pbuffer);

    return;
}


VOS_VOID TAF_MTA_RcvAcpuOmErrLogCtrlInd(
    VOS_VOID                           *pMsg
)
{
    OM_ERROR_LOG_CTRL_IND_STRU         *pstRcvMsg = VOS_NULL_PTR;

    pstRcvMsg = (OM_ERROR_LOG_CTRL_IND_STRU*)pMsg;

    /* 更新ERRLOG控制标识 */
    if ((VOS_FALSE == pstRcvMsg->ucAlmStatus)
     || (VOS_TRUE  == pstRcvMsg->ucAlmStatus))
    {
        TAF_SDC_SetErrlogCtrlFlag(pstRcvMsg->ucAlmStatus);
    }

    if ((pstRcvMsg->ucAlmLevel >= NAS_ERR_LOG_CTRL_LEVEL_CRITICAL)
     && (pstRcvMsg->ucAlmLevel <= NAS_ERR_LOG_CTRL_LEVEL_WARNING))
    {
        TAF_SDC_SetErrlogAlmLevel(pstRcvMsg->ucAlmLevel);
    }

    return;
}

#endif

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_MTA_SndLrrcSetDpdtTestFlagNtfMsg(
    AT_MTA_SET_DPDTTEST_FLAG_REQ_STRU  *pstSetDpdtFlagReq
)
{
    MTA_LRRC_SET_DPDTTEST_FLAG_NTF_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                              ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_LRRC_SET_DPDTTEST_FLAG_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_LRRC_SET_DPDTTEST_FLAG_NTF_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndLrrcSetDpdtTestFlagNtfMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid           = PS_PID_ERRC;
    pstMsg->ulLength                = ulLength;
    pstMsg->ulMsgName               = ID_MTA_LRRC_SET_DPDTTEST_FLAG_NTF;
    pstMsg->ucFlag                  = pstSetDpdtFlagReq->ucFlag;

    /* 发送消息到LRRC */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_ERROR_LOG("TAF_MTA_SndLrrcSetDpdtTestFlagNtfMsg: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndLrrcSetDpdtValueNtfMsg(
    AT_MTA_SET_DPDT_VALUE_REQ_STRU  *pstSetDpdtReq
)
{
    MTA_LRRC_SET_DPDT_VALUE_NTF_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_LRRC_SET_DPDT_VALUE_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_LRRC_SET_DPDT_VALUE_NTF_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndLrrcSetDpdtValueNtfMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid           = PS_PID_ERRC;
    pstMsg->ulLength                = ulLength;
    pstMsg->ulMsgName               = ID_MTA_LRRC_SET_DPDT_VALUE_NTF;
    pstMsg->ulDpdtValue             = pstSetDpdtReq->ulDpdtValue;

    /* 发送消息到LRRC */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_ERROR_LOG("TAF_MTA_SndLrrcSetDpdtValueNtfMsg: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 TAF_MTA_SndLrrcQryDpdtValueReqMsg(
    AT_MTA_QRY_DPDT_VALUE_REQ_STRU     *pstSetDpdtReq
)
{
    MTA_LRRC_QRY_DPDT_VALUE_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* 申请消息结构内存 */
    ulLength    = sizeof(MTA_LRRC_QRY_DPDT_VALUE_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (MTA_LRRC_QRY_DPDT_VALUE_REQ_STRU *)PS_ALLOC_MSG(UEPS_PID_MTA, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        MTA_ERROR_LOG("TAF_MTA_SndLrrcQryDpdtValueReqMsg: Alloc msg fail!");
        return VOS_ERR;
    }

    PS_MEM_SET((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH, 0, ulLength);

    /* 填写新消息内容 */
    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_MTA;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid           = PS_PID_ERRC;
    pstMsg->ulLength                = ulLength;
    pstMsg->ulMsgName               = ID_MTA_LRRC_QRY_DPDT_VALUE_REQ;

    /* 发送消息到LRRC */
    if (VOS_OK != PS_SEND_MSG(UEPS_PID_MTA, pstMsg))
    {
        MTA_ERROR_LOG("TAF_MTA_SndLrrcQryDpdtValueReqMsg: PS_SEND_MSG fail.");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID TAF_MTA_RcvLrrcDpdtValueQryCnf(VOS_VOID *pMsg)
{
    TAF_MTA_CMD_BUFFER_STRU            *pstCmdBuf        = VOS_NULL_PTR;
    LRRC_MTA_QRY_DPDT_VALUE_CNF_STRU   *pstLrrcMtaQryCnf = VOS_NULL_PTR;
    MTA_AT_QRY_DPDT_VALUE_CNF_STRU      stMtaAtQryCnf;
    TAF_MTA_TIMER_ID_ENUM_UINT32        enTimerId;

    /* 局部变量初始化 */
    pstLrrcMtaQryCnf = (LRRC_MTA_QRY_DPDT_VALUE_CNF_STRU *)pMsg;
    enTimerId = TI_TAF_MTA_WAIT_DPDT_VALUE_QRY_CNF;

    if (TAF_MTA_TIMER_STATUS_RUNING != TAF_MTA_GetTimerStatus(enTimerId))
    {
        MTA_WARNING_LOG("TAF_MTA_RcvLrrcDpdtValueQryCnf: WARNING: Timer was already stop!");
        return;
    }

    /* 停止保护定时器 */
    TAF_MTA_StopTimer(enTimerId);

    /* 获取当前定时器对应的消息队列 */
    pstCmdBuf = TAF_MTA_GetItemFromCmdBufferQueue(enTimerId);
    if (VOS_NULL_PTR == pstCmdBuf)
    {
        return;
    }

    /* 构造消息结构体 */
    if (MTA_RRC_RESULT_NO_ERROR == pstLrrcMtaQryCnf->enResult)
    {
        stMtaAtQryCnf.enResult = MTA_AT_RESULT_NO_ERROR;
        stMtaAtQryCnf.ulDpdtValue = pstLrrcMtaQryCnf->ulDpdtValue;
    }
    else
    {
        stMtaAtQryCnf.enResult = MTA_AT_RESULT_ERROR;
        stMtaAtQryCnf.ulDpdtValue = 0;
    }

    /* 发送消息给AT模块 */
    TAF_MTA_SndAtMsg( (AT_APPCTRL_STRU *)pstCmdBuf->pucMsgInfo,
                      ID_MTA_AT_QRY_DPDT_VALUE_CNF,
                      sizeof(MTA_AT_QRY_DPDT_VALUE_CNF_STRU),
                      (VOS_UINT8 *)&stMtaAtQryCnf);

    /* 从等待队列中删除消息 */
    TAF_MTA_DelItemInCmdBufferQueue(pstCmdBuf->enTimerId);

    return;
}
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

