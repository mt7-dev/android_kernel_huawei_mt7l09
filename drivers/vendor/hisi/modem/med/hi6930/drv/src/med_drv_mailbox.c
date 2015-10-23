

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_drv_mb_hifi.h"
#include "med_drv_ipc.h"
#include "med_drv_timer_hifi.h"
#include "om_log.h"
#include "ucom_stub.h"
#include "ucom_share.h"
#include "codec_typedefine.h"

#include "om_comm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint +e413*/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_DRV_MAILBOX_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 数据表, 以索引号顺序存放各邮箱头及邮箱的首地址和长度, 格式为:
    邮箱0的邮箱头首地址,邮箱首地址,邮箱长度
    邮箱1的邮箱头首地址,邮箱首地址,邮箱长度
    ...
    邮箱n的邮箱头首地址,邮箱首地址,邮箱长度*/
#if (VOS_CPU_TYPE == VOS_HIFI)

VOS_UINT32 g_uwDrvMailboxAddrTbl[DRV_MAILBOX_ID_BUTT][DRV_MAILBOX_INFO_NO] =
{
    {MAILBOX_HEAD_ADDR(HIFI, CCPU, MSG),
     MAILBOX_QUEUE_ADDR(HIFI, CCPU, MSG),
     MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG)},                    /* HIFI2CCPU */
    {MAILBOX_HEAD_ADDR(CCPU, HIFI, MSG),
     MAILBOX_QUEUE_ADDR(CCPU, HIFI, MSG),
     MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG)},                    /* CCPU2HIFI */
    {MAILBOX_HEAD_ADDR(HIFI, ACPU, MSG),
     MAILBOX_QUEUE_ADDR(HIFI, ACPU, MSG),
     MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG)},                    /* HIFI2ACPU */
    {MAILBOX_HEAD_ADDR(ACPU, HIFI, MSG),
     MAILBOX_QUEUE_ADDR(ACPU, HIFI, MSG),
     MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG)},                    /* ACPU2HIFI */
    {MAILBOX_HEAD_ADDR(HIFI, BBE16, MSG),
     MAILBOX_QUEUE_ADDR(HIFI, BBE16, MSG),
     MAILBOX_QUEUE_SIZE(HIFI, BBE16, MSG)},                   /* HIFI 2 BBE16 */
    {MAILBOX_HEAD_ADDR(BBE16, HIFI, MSG),
     MAILBOX_QUEUE_ADDR(BBE16, HIFI, MSG),
     MAILBOX_QUEUE_SIZE(BBE16, HIFI, MSG)}                    /* BBE16 2 HIFI */
};

#endif

/* 数据表, 存放对HiFi而言各CPU对应的发送、接收邮箱索引号, 格式为:
    HiFi往CPU0的发送邮箱所引号, HiFi往CPU0的接收邮箱所引号
    HiFi往CPU1的发送邮箱所引号, HiFi往CPU1的接收邮箱所引号
    ...
    HiFi往CPUn的发送邮箱所引号, HiFi往CPUn的接收邮箱所引号
    说明:RV_MAILBOX_ID_BUTT表示对应的邮箱不存在, 即还不支持该邮箱；
         HIFI往AARM发送消息暂通过发送至CARM然后经由CARM转发至AARM完成 */

VOS_UINT16 g_uhwDrvMailboxIdTbl[VOS_CPU_ID_BUTT][DRV_MAILBOX_TXRX_BUTT] =
{
    {DRV_MAILBOX_ID_HIFI2CCPU,          DRV_MAILBOX_ID_CCPU2HIFI},
    {DRV_MAILBOX_ID_HIFI2ACPU,          DRV_MAILBOX_ID_ACPU2HIFI},
    {DRV_MAILBOX_ID_BUTT,               DRV_MAILBOX_ID_BUTT},
    {DRV_MAILBOX_ID_BUTT,               DRV_MAILBOX_ID_BUTT},
    {DRV_MAILBOX_ID_BUTT,               DRV_MAILBOX_ID_BUTT},
    {DRV_MAILBOX_ID_HIFI2BBE16,         DRV_MAILBOX_ID_BBE162HIFI}
};

/* 数据表, 存放HiFi发送到各CPU的跨核消息对应的互发中断信息, 格式为:
    CPU0的IPC中断目标CPU ID, HiFi到CPU0的跨核消息通知bit位
    CPU1的IPC中断目标CPU ID, HiFi到CPU1的跨核消息通知bit位
    ...
    CPUn的IPC中断目标CPU ID, HiFi到CPUn的跨核消息通知bit位
    说明: DRV_IPC_MAX_INT_NUM表示对应的跨核消息bit不存在, 即与该CPU不支持跨核消息；
          HIFI往AARM发送消息暂通过发送至CARM然后经由CARM转发至AARM完成 */
VOS_UINT16 g_uhwDrvMailboxIpcTbl[VOS_CPU_ID_BUTT][DRV_MAILBOX_IPC_INFO_NUM] =
{
    {MAILBOX_CPUID_CCPU,               IPC_CCPU_INT_SRC_HIFI_MSG},
    {MAILBOX_CPUID_ACPU,               IPC_ACPU_INT_SRC_HIFI_MSG},
    {MAILBOX_CPUID_BUTT,               IPC_INT_BUTTOM},
    {MAILBOX_CPUID_BUTT,               IPC_INT_BUTTOM},
    {MAILBOX_CPUID_BUTT,               IPC_INT_BUTTOM},
    {MAILBOX_CPUID_BBE16,              IPC_INT_HIFI_DSP_MBX}
};

/* 数据表, 存放HiFi发送到各VOS CPU的跨核消息对应的邮编, 格式为:
    CPU0的通道0邮编，通道1邮编，通道2邮编，通道0邮编，
    CPU1的通道0邮编，通道1邮编，通道2邮编，通道0邮编，
    ...
    CPUn的通道0邮编，通道1邮编，通道2邮编，通道0邮编，
    说明: 邮编为VOS_NULL_DWORD表示对应的通道邮编不存在, 即与该CPU不支持该通道跨核消息； */
VOS_UINT32 g_uwDrvMailCodeTbl[VOS_CPU_ID_BUTT][DRV_MAILBOX_CHANNEL_BUTT] =
{
    {MAILBOX_MAILCODE_HIFI_TO_CCPU_VOS_MSG_NORMAL,
     MAILBOX_MAILCODE_HIFI_TO_CCPU_VOS_MSG_URGENT,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD},
    {MAILBOX_MAILCODE_HIFI_TO_ACPU_VOS_MSG_NORMAL,
     MAILBOX_MAILCODE_HIFI_TO_ACPU_VOS_MSG_URGENT,
     MAILBOX_MAILCODE_HIFI_TO_ACPU_AUDIO,
     MAILBOX_MAILCODE_HIFI_TO_ACPU_MISC,
     MAILBOX_MAILCODE_HIFI_TO_ACPU_CCORE_RESET_ID,
     MAILBOX_MAILCODE_RESERVED(ACPU, HIFI, MSG)},
    {VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD},
    {VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD},
    {VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD},
    {MAILBOX_MAILCODE_HIFI_TO_BBE16_VOS_MSG_NORMAL,
     MAILBOX_MAILCODE_HIFI_TO_BBE16_VOS_MSG_URGENT,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD,
     VOS_NULL_DWORD}
};

/* 各跨核邮箱消息序列号(SN号)记录全局变量，其序列号记录按DRV_MAILBOX_ID_ENUM枚举依次排列 */
VOS_UINT32 g_auwDrvMailboxSeqNum[DRV_MAILBOX_ID_BUTT];

/*  上下电标志，邮箱初始化只在加载时候做，上下电不做初始化 */
VOS_UINT32 g_uwMbInitFlag = 0x5a5a5a5a;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID DRV_MAILBOX_Init( VOS_VOID )
{
    VOS_UINT32                             *puwSeqNum = DRV_MAILBOX_GetMailBoxSeqAddr();
    VOS_CPU_SR                              uwCpuSr;
    DRV_MAILBOX_ID_ENUM_UINT16              enMailboxId;
    VOS_UINT32                              uwCpuId;
    VOS_UINT32                              uwMailboxHeadAddr;                  /* 邮箱头首地址 */
    VOS_UINT32                              uwMailboxAddr;
    VOS_UINT32                              uwMailboxLen;
    MAILBOX_HEAD_STRU                      *pstMailboxHead;
    volatile VOS_UINT32                    *puwMbInitFlag = (VOS_UINT32*)UCOM_GetUncachedAddr((VOS_UINT32)&g_uwMbInitFlag);


    /*初始化各邮箱消息序列号记录*/
    UCOM_MemSet(puwSeqNum, 0, sizeof(VOS_UINT32)*DRV_MAILBOX_ID_BUTT);

    /*注册CCPU消息到达回调函数*/
    DRV_IPC_RegIntSrc(IPC_HIFI_INT_SRC_CCPU_MSG,
                      DRV_MAILBOX_ReceiveMsg,
                      DRV_MAILBOX_GetMailBoxRx(VOS_CPU_ID_CCPU));

    /*注册ACPU消息到达回调函数*/
    DRV_IPC_RegIntSrc(IPC_HIFI_INT_SRC_ACPU_MSG,
                      DRV_MAILBOX_ReceiveMsg,
                      DRV_MAILBOX_GetMailBoxRx(VOS_CPU_ID_ACPU));

    /*注册BBE16消息到达回调函数*/
    DRV_IPC_RegIntSrc(IPC_HIFI_INT_SRC_BBE_MSG,
                      DRV_MAILBOX_ReceiveMsg,
                      DRV_MAILBOX_GetMailBoxRx(VOS_CPU_ID_BBE16));

    if(0x55aa55aa == *puwMbInitFlag)
    {
        return;
    }
    else
    {
        *puwMbInitFlag = 0x55aa55aa;
    }

#if (VOS_CPU_TYPE == VOS_HIFI)
    CARM_HIFI_DYN_ADDR_SHARE_STRU  *pstCarmShare    = UCOM_GetCarmShareAddrStru();
    CARM_HIFI_MAILBOX_STRU         *pstCarmShareMB  = &(pstCarmShare->stCarmHifiMB);
    AARM_HIFI_MAILBOX_STRU         *pstAarmShareMB  = &(pstCarmShare->stAarmHifiMB);

    /* HIFI2ACPU 邮箱地址*/
    DRV_MAILBOX_InitMBHeadAddr(DRV_MAILBOX_ID_HIFI2ACPU,
                pstAarmShareMB->uwHifiAarmHeadAddr);
    DRV_MAILBOX_InitMBAddr(DRV_MAILBOX_ID_HIFI2ACPU,
                pstAarmShareMB->uwHifiAarmBodyAddr);
    DRV_MAILBOX_InitMBLen(DRV_MAILBOX_ID_HIFI2ACPU,
                pstAarmShareMB->uwHifi2AarmMailBoxLen);

    /* ACPU2HIFI 邮箱地址 */
    DRV_MAILBOX_InitMBHeadAddr(DRV_MAILBOX_ID_ACPU2HIFI,
                pstAarmShareMB->uwAarmHifiHeadAddr);
    DRV_MAILBOX_InitMBAddr(DRV_MAILBOX_ID_ACPU2HIFI,
                pstAarmShareMB->uwAarmHifiBodyAddr);
    DRV_MAILBOX_InitMBLen(DRV_MAILBOX_ID_ACPU2HIFI,
                pstAarmShareMB->uwAarm2HifiMailBoxLen);

    /* HIFI2CCPU 邮箱地址*/
    DRV_MAILBOX_InitMBHeadAddr(DRV_MAILBOX_ID_HIFI2CCPU,
                pstCarmShareMB->uwHifiCarmHeadAddr);
    DRV_MAILBOX_InitMBAddr(DRV_MAILBOX_ID_HIFI2CCPU,
                pstCarmShareMB->uwHifiCarmBodyAddr);
    DRV_MAILBOX_InitMBLen(DRV_MAILBOX_ID_HIFI2CCPU,
                pstCarmShareMB->uwHifi2CarmMailBoxLen);

    /* CCPU2HIFI 邮箱地址 */
    DRV_MAILBOX_InitMBHeadAddr(DRV_MAILBOX_ID_CCPU2HIFI,
                pstCarmShareMB->uwCarmHifiHeadAddr);
    DRV_MAILBOX_InitMBAddr(DRV_MAILBOX_ID_CCPU2HIFI,
                pstCarmShareMB->uwCarmHifiBodyAddr);
    DRV_MAILBOX_InitMBLen(DRV_MAILBOX_ID_CCPU2HIFI,
                pstCarmShareMB->uwCarm2HifiMailBoxLen);

    /* HIFI2BBE 邮箱地址, 邮箱长度已初始化 */
    DRV_MAILBOX_InitMBHeadAddr(DRV_MAILBOX_ID_HIFI2BBE16,
                UCOM_GetUncachedAddr(DRV_MAILBOX_GetMailBoxHeadAddr(DRV_MAILBOX_ID_HIFI2BBE16)));
    DRV_MAILBOX_InitMBAddr(DRV_MAILBOX_ID_HIFI2BBE16,
                UCOM_GetUncachedAddr(DRV_MAILBOX_GetMailBoxAddr(DRV_MAILBOX_ID_HIFI2BBE16)));

    /* BBE2HIFI 邮箱地址, 邮箱长度已初始化 */
    DRV_MAILBOX_InitMBHeadAddr(DRV_MAILBOX_ID_BBE162HIFI,
                UCOM_GetUncachedAddr(DRV_MAILBOX_GetMailBoxHeadAddr(DRV_MAILBOX_ID_BBE162HIFI)));
    DRV_MAILBOX_InitMBAddr(DRV_MAILBOX_ID_BBE162HIFI,
                UCOM_GetUncachedAddr(DRV_MAILBOX_GetMailBoxAddr(DRV_MAILBOX_ID_BBE162HIFI)));

#endif

    /*锁中断互斥*/
    uwCpuSr             = VOS_SplIMP();

    /*对各跨核邮箱的读邮箱进行初始化(写邮箱由对方进行初始化)，并赋值分隔字*/
    for (uwCpuId = 0; uwCpuId < VOS_CPU_ID_BUTT; uwCpuId++)
    {
        /*获取该CPU对应的写邮箱编号*/
        enMailboxId = DRV_MAILBOX_GetMailBoxRx(uwCpuId);

        /*若邮箱编号非法(或暂不支持)则不进行初始化*/
        if (DRV_MAILBOX_ID_BUTT != enMailboxId)
        {
            /*获取邮箱头地址*/
            uwMailboxHeadAddr   = DRV_MAILBOX_GetMailBoxHeadAddr(enMailboxId);

            /*获取邮箱地址*/
            uwMailboxAddr       = DRV_MAILBOX_GetMailBoxAddr(enMailboxId);

            /*获取邮箱长度，单位byte*/
            uwMailboxLen        = DRV_MAILBOX_GetMailBoxLen(enMailboxId);

            /*按公共邮箱头结构进行地址内容解析*/
            pstMailboxHead      = (MAILBOX_HEAD_STRU *)uwMailboxHeadAddr;

            /*初始化邮箱头*/
            pstMailboxHead->ulProtectWord1  = DRV_MAILBOX_PROTECT1;
            pstMailboxHead->ulProtectWord2  = DRV_MAILBOX_PROTECT2;
            pstMailboxHead->ulFront         = 0;
            pstMailboxHead->ulFrontslice    = 0;
            pstMailboxHead->ulRear          = pstMailboxHead->ulFront;
            pstMailboxHead->ulRearslice     = 0;
            pstMailboxHead->ulProtectWord3  = DRV_MAILBOX_PROTECT1;
            pstMailboxHead->ulProtectWord4  = DRV_MAILBOX_PROTECT2;

            /*初始化邮箱*/
            UCOM_MemSet((VOS_UCHAR *)uwMailboxAddr, 0, uwMailboxLen);

            /*初始化邮箱头部保护字*/
            UCOM_RegWr(uwMailboxAddr, DRV_MAILBOX_PROTECT1);
            UCOM_RegWr(uwMailboxAddr + DRV_MAILBOX_PROTECT_LEN, DRV_MAILBOX_PROTECT2);

            /*初始化邮箱尾部保护字*/
            UCOM_RegWr((uwMailboxAddr + uwMailboxLen) - (2*DRV_MAILBOX_PROTECT_LEN),
                       DRV_MAILBOX_PROTECT1);
            UCOM_RegWr((uwMailboxAddr + uwMailboxLen) - DRV_MAILBOX_PROTECT_LEN,
                       DRV_MAILBOX_PROTECT2);
        }
    }

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

}


VOS_VOID DRV_MAILBOX_WriteQueue(
                DRV_MAILBOX_QUEUE_STRU *pstQueue,
                VOS_UCHAR              *pucData,
                VOS_UINT32              uwSize)
{
    VOS_UINT32                          uwSizeToBottom;

    /*计算写指针位置距离环形缓存尾部长度*/
    uwSizeToBottom  = (pstQueue->uwBase + pstQueue->uwLength) - pstQueue->uwFront;

    /*若写指针距环形缓存尾部长度大于要写的内容长度，则直接拷贝内容，并更新写指针*/
    if (uwSizeToBottom > uwSize)
    {
        /*写入pucData到写指针处*/
        UCOM_MemCpy((VOS_UCHAR *)pstQueue->uwFront, pucData, uwSize);

        /*更新写指针*/
        pstQueue->uwFront += uwSize;
    }
    else
    {
        /*写入pucData前R长度到写指针处*/
        UCOM_MemCpy((VOS_UCHAR *)(pstQueue->uwFront), pucData, uwSizeToBottom);

        /*写入pucData+R到环形缓存起始处*/
        UCOM_MemCpy((VOS_UCHAR *)(pstQueue->uwBase),
                    pucData + uwSizeToBottom,
                    uwSize - uwSizeToBottom);

        /*更新写指针*/
        pstQueue->uwFront = (pstQueue->uwBase + uwSize) - uwSizeToBottom;
    }
}
VOS_VOID DRV_MAILBOX_ReadQueue(
                DRV_MAILBOX_QUEUE_STRU *pstQueue,
                VOS_UCHAR              *pucData,
                VOS_UINT32              uwSize)
{
    VOS_UINT32                          uwSizeToBottom;

    /*计算读指针位置距离环形缓存尾部长度*/
    uwSizeToBottom  = (pstQueue->uwBase + pstQueue->uwLength) - pstQueue->uwRear;

    /*若读指针距环形缓存尾部长度大于要写的内容长度，则直接拷贝内容，并更新读指针*/
    if (uwSizeToBottom > uwSize)
    {
        /*将读指针处数据拷贝至pucData处*/
        UCOM_MemCpy(pucData, (VOS_UCHAR *)(pstQueue->uwRear), uwSize);

        /*更新读指针*/
        pstQueue->uwRear += uwSize;
    }
    else
    {
        /*将读指针处数据前若干byte拷贝到pucData处*/
        UCOM_MemCpy(pucData, (VOS_UCHAR *)(pstQueue->uwRear), uwSizeToBottom);

        /*从环形缓存起始处拷贝剩余内容到pucData*/
        UCOM_MemCpy(pucData + uwSizeToBottom,
                    (VOS_UCHAR *)(pstQueue->uwBase),
                    uwSize - uwSizeToBottom);

        /*更新读指针*/
        pstQueue->uwRear = (pstQueue->uwBase + uwSize) - uwSizeToBottom;
    }
}
VOS_VOID DRV_MAILBOX_CheckRecMsg(
                VOS_UINT32              uwMailboxId,
                VOS_UINT32              uwTimeStamp,
                MAILBOX_MSG_HEADER    *pstMsgHead)
{
    VOS_UINT32                              uwSeqNum;

    /*读取当前消息序列号(即上一条消息的Sn号)*/
    uwSeqNum        = DRV_MAILBOX_GetMailBoxSeqNum(uwMailboxId);

    /*若SN号不连续(去除两CPU可能出现分别下电的情况)*/
    if ((  0 != uwSeqNum)
        &&(0 != pstMsgHead->ulSeqNum)
        &&((uwSeqNum + 1) != pstMsgHead->ulSeqNum))
    {
        /*上报异常，某邮箱接收的SN号不连续*/
        OM_LogError2(OM_MAILBOX_RecieveMsg_MsgSeqNumErr, uwMailboxId, uwSeqNum);
    }

    /*更新SN号记录*/
    DRV_MAILBOX_SetMailBoxSeqNum(uwMailboxId, pstMsgHead->ulSeqNum);

    if ( pstMsgHead->ulWriteSlice >= uwTimeStamp)
    {
        uwTimeStamp = pstMsgHead->ulWriteSlice - uwTimeStamp;
    }
    else
    {
        uwTimeStamp = uwTimeStamp + (0xffffffff - pstMsgHead->ulWriteSlice);
    }

    if (uwTimeStamp >= DRV_MAILBOX_MSG_DELAY_THD)
    {
        /*上报异常，某邮箱接收的消息滞留时间超过40ms*/
        OM_LogError1(OM_MAILBOX_RecieveMsg_MsgDelayErr, uwMailboxId);
    }

}
VOS_UINT32 DRV_MAILBOX_Write(
                DRV_MAILBOX_ID_ENUM_UINT16  enMailboxId,
                VOS_UINT32                  uwCpuId,
                VOS_UINT32                  uwPriority,
                VOS_UCHAR                  *pucwMsg,
                VOS_UINT32                  uwSize,
                VOS_UINT32                  uwMailCode)
{
    VOS_CPU_SR                              uwCpuSr;
    VOS_UINT32                              uwSeqNum;
    VOS_UINT32                              uwMailboxAddr;                      /* 邮箱可写空间的首地址,与邮箱头首地址无必然关系 */
    VOS_UINT32                              uwMailboxLen;                       /* 邮箱长度，不含邮箱头，不含保护字长度 */
    VOS_UINT32                              uwMailboxHeadAddr;                  /* 邮箱头首地址 */
    VOS_UINT32                              uwSizeLeft;
    VOS_UINT32                              uwMsgHeadLen;
    VOS_UINT32                              uwTimeStamp;
    MAILBOX_HEAD_STRU                      *pstMailboxHead;
    MAILBOX_MSG_HEADER                      stMsgHeader;
    DRV_MAILBOX_QUEUE_STRU                  stQueue;

    /*公共邮箱消息头格式固定为MAILBOX_MSG_HEADER*/
    uwMsgHeadLen        = sizeof(MAILBOX_MSG_HEADER);

    /*获取邮箱长度，去除邮箱头部及尾部的四个保护字(共16byte)*/
    uwMailboxLen        = DRV_MAILBOX_GetMailBoxLen(enMailboxId)
                           - (4*DRV_MAILBOX_PROTECT_LEN);

    /*获取邮箱首地址，去除邮箱头部两个保护字*/
    uwMailboxAddr       = DRV_MAILBOX_GetMailBoxAddr(enMailboxId)
                           + (2*DRV_MAILBOX_PROTECT_LEN);

    /*获取邮箱头首地址*/
    uwMailboxHeadAddr   = DRV_MAILBOX_GetMailBoxHeadAddr(enMailboxId);

    /*锁中断互斥*/
    uwCpuSr             = VOS_SplIMP();

    /*按公共邮箱头结构进行地址内容解析*/
    pstMailboxHead      = (MAILBOX_HEAD_STRU *)uwMailboxHeadAddr;

    /*若出现邮箱头保护字不匹配，说明邮箱头被踩，此状态无法恢复，做复位处理*/
    if ((  DRV_MAILBOX_PROTECT1 != pstMailboxHead->ulProtectWord1)
        ||(DRV_MAILBOX_PROTECT1 != pstMailboxHead->ulProtectWord3)
        ||(DRV_MAILBOX_PROTECT2 != pstMailboxHead->ulProtectWord2)
        ||(DRV_MAILBOX_PROTECT2 != pstMailboxHead->ulProtectWord4))
    {
        VOS_Splx(uwCpuSr);

        /*触发软件中断,标明是写邮箱时邮箱头保护字异常*/
        OM_LogError(OM_MAILBOX_SendMsg_MailboxHeadPartitionErr);
        return VOS_ERR;
    }

    /*记录当前系统时戳,作为邮箱修改及消息头的时戳*/
    uwTimeStamp         = DRV_TIMER_ReadSysTimeStamp();

    /*计算邮箱剩余空间，为了防止队列头尾重叠去除了4byte空间未使用，单位byte*/
    if (pstMailboxHead->ulRear > pstMailboxHead->ulFront)
    {
        uwSizeLeft      = ((pstMailboxHead->ulRear - pstMailboxHead->ulFront) - 1)
                            *sizeof(VOS_INT32);
    }
    else
    {
        uwSizeLeft      = (uwMailboxLen + (pstMailboxHead->ulRear*sizeof(VOS_INT32)))
                           - ((pstMailboxHead->ulFront + 1)*sizeof(VOS_INT32));
    }

    /*若邮箱剩余空间不够*/
    if (uwSizeLeft < (uwSize + uwMsgHeadLen))
    {
        /*释放互斥*/
        VOS_Splx(uwCpuSr);

        /*上报异常，写邮箱空间不足*/
        OM_LogError1(OM_MAILBOX_SendMsg_MailboxFull, enMailboxId);

        /* 复位 */
        OM_LogFatalAndReset(OM_MAILBOX_SendMsg_MailboxFull);

        return VOS_ERR;
    }

    /*获取该消息对应的SN*/
    uwSeqNum = DRV_MAILBOX_GetMailBoxSeqNum(enMailboxId);

    /*填充消息头*/
    stMsgHeader.ulWriteSlice    = uwTimeStamp; /* uwTimeStamp */
    stMsgHeader.ulReadSlice     = 0;           /* 初始化,由读取方赋值 */
    stMsgHeader.ulPartition     = DRV_MAILBOX_MSGHEAD_NUMBER;
    stMsgHeader.ulPriority      = uwPriority;
    stMsgHeader.ulSeqNum        = uwSeqNum;
    stMsgHeader.ulMsgLength     = uwSize;
    stMsgHeader.ulMailCode      = uwMailCode;

    /*更新SN，供下一条消息使用*/
    DRV_MAILBOX_SetMailBoxSeqNum(enMailboxId, uwSeqNum + 1);

    /*初始化队列状态记录临时结构体,写队列仅关心写指针*/
    stQueue.uwBase              = uwMailboxAddr;
    stQueue.uwLength            = uwMailboxLen;
    stQueue.uwFront             = stQueue.uwBase + (pstMailboxHead->ulFront*sizeof(VOS_INT32));

    /*向邮箱队列中拷贝消息头，并更新队列状态记录临时结构体*/
    DRV_MAILBOX_WriteQueue(&stQueue, (VOS_UCHAR*)(&stMsgHeader), uwMsgHeadLen);

    /*向邮箱队列中拷贝消息内容，并更新队列状态记录临时结构体*/
    DRV_MAILBOX_WriteQueue(&stQueue, pucwMsg, uwSize);

    /*更新写指针，并向上取整为4Byte*/
    pstMailboxHead->ulFront     = ((stQueue.uwFront - stQueue.uwBase)
                                    + (sizeof(VOS_INT32) - 1))/sizeof(VOS_INT32);

    /*更新邮箱头写指针(头指针)时间*/
    pstMailboxHead->ulFrontslice = uwTimeStamp;

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

    return VOS_OK;
}


VOS_UINT32 DRV_MAILBOX_Read(
                IPC_MSG_BLK_STRU       *pstMsgBlk,
                DRV_MAILBOX_QUEUE_STRU *pstQueue)
{
    MsgBlock                               *pstSndOsaMsg;
    VOS_UINT32                              uwMsgPriority;
    VOS_UINT32                              uwCpuId;

    /* 根据邮编进行相应处理 */
    switch (pstMsgBlk->stMsgHead.ulMailCode)
    {
        case MAILBOX_MAILCODE_ACPU_TO_HIFI_VOS_MSG_NORMAL :
        case MAILBOX_MAILCODE_CCPU_TO_HIFI_VOS_MSG_NORMAL :
        case MAILBOX_MAILCODE_BBE16_TO_HIFI_VOS_MSG_NORMAL:
        {
            uwMsgPriority = VOS_NORMAL_PRIORITY_MSG;

            /* 读出消息头信息中的VOS头(需要知道PID) */
            DRV_MAILBOX_ReadQueue(pstQueue,
                                  (VOS_UCHAR*)&pstMsgBlk->ulSenderCpuId,
                                  sizeof(VOS_MSG_HEADER_STRU));
            break;
        }

        case MAILBOX_MAILCODE_ACPU_TO_HIFI_VOS_MSG_URGENT :
        case MAILBOX_MAILCODE_CCPU_TO_HIFI_VOS_MSG_URGENT :
        case MAILBOX_MAILCODE_BBE16_TO_HIFI_VOS_MSG_URGENT:
        {
            uwMsgPriority = VOS_EMERGENT_PRIORITY_MSG;

            /* 读出消息头信息中的VOS头(需要知道PID) */
            DRV_MAILBOX_ReadQueue(pstQueue,
                                  (VOS_UCHAR*)&pstMsgBlk->ulSenderCpuId,
                                  sizeof(VOS_MSG_HEADER_STRU));
            break;
        }

        case MAILBOX_MAILCODE_ACPU_TO_HIFI_AUDIO :
        case MAILBOX_MAILCODE_ACPU_TO_HIFI_MISC:
        {
            uwMsgPriority = VOS_NORMAL_PRIORITY_MSG;

            /* 若非VOS消息，则自行填充VOS头 */
            pstMsgBlk->ulSenderCpuId  = (pstMsgBlk->stMsgHead.ulMailCode>>MAILBOX_ID_SRC_CPU_OFFSET)&0xff;
            pstMsgBlk->ulSenderPid    = UEPS_PID_DL;                          /* 此处填写AP侧DRV对应的PID,,待定 */
            pstMsgBlk->ulReceiverCpuId= VOS_CPU_ID_MEDDSP;
            pstMsgBlk->ulReceiverPid  = DSP_PID_AUDIO_RT;                     /* 目前默认为这两个通道的消息均为需实时响应的消息 */
            pstMsgBlk->ulLength       = pstMsgBlk->stMsgHead.ulMsgLength;
            pstMsgBlk->stMsgHead.ulMsgLength += sizeof(VOS_MSG_HEADER_STRU);
            break;
        }
        /* C核复位A核通知hifi C核复位channel */
        case MAILBOX_MAILCODE_ACPU_TO_HIFI_CCORE_RESET_ID:
        {
            uwMsgPriority = VOS_NORMAL_PRIORITY_MSG;

            /* 若非VOS消息，则自行填充VOS头 */
            pstMsgBlk->ulSenderCpuId  = (pstMsgBlk->stMsgHead.ulMailCode>>MAILBOX_ID_SRC_CPU_OFFSET)&0xff;
            pstMsgBlk->ulSenderPid    = UEPS_PID_DL;                            /* 此处填写AP侧DRV对应的PID,,待定 */
            pstMsgBlk->ulReceiverCpuId= VOS_CPU_ID_MEDDSP;
            pstMsgBlk->ulReceiverPid  = DSP_PID_VOICE;                          /* 目前默认为这两个通道的消息均为需实时响应的消息 */
            pstMsgBlk->ulLength       = pstMsgBlk->stMsgHead.ulMsgLength;
            pstMsgBlk->stMsgHead.ulMsgLength += sizeof(VOS_MSG_HEADER_STRU);
            break;
        }
        case MAILBOX_MAILCODE_RESERVED(CCPU, HIFI, MSG) :
        {
            uwCpuId = VOS_CPU_ID_CCPU;

            DRV_MAILBOX_Write(DRV_MAILBOX_GetMailBoxTx(uwCpuId),
                              DRV_MAILBOX_GetTargetCpu(uwCpuId),
                              VOS_NORMAL_PRIORITY_MSG,
                              (VOS_UCHAR *)pstQueue->uwRear,
                              pstMsgBlk->stMsgHead.ulMsgLength,
                              MAILBOX_MAILCODE_RESERVED(HIFI, CCPU, MSG));

            /*向目标CPU触发IPC中断*/
            DRV_IPC_TrigInt(DRV_MAILBOX_GetTargetCpu(uwCpuId),
                            DRV_MAILBOX_GetIntrSrc(uwCpuId));

            return VOS_OK;
        }
        case MAILBOX_MAILCODE_RESERVED(ACPU, HIFI, MSG):
        {
            uwCpuId = VOS_CPU_ID_ACPU;

            DRV_MAILBOX_Write(DRV_MAILBOX_GetMailBoxTx(uwCpuId),
                              DRV_MAILBOX_GetTargetCpu(uwCpuId),
                              VOS_NORMAL_PRIORITY_MSG,
                              (VOS_UCHAR *)pstQueue->uwRear,
                              pstMsgBlk->stMsgHead.ulMsgLength,
                              MAILBOX_MAILCODE_RESERVED(HIFI, ACPU, MSG));

            /*向目标CPU触发IPC中断*/
            DRV_IPC_TrigInt(DRV_MAILBOX_GetTargetCpu(uwCpuId),
                            DRV_MAILBOX_GetIntrSrc(uwCpuId));

            return VOS_OK;
        }
        default:
        {
            /*上报异常，MailCode异常*/
            OM_LogError1(OM_MAILBOX_RecieveMsg_MsgChannelIdErr, pstMsgBlk->stMsgHead.ulMailCode);
            return VOS_ERR;
        }

    }


    /*申请OSA消息缓存pMsg*/
    pstSndOsaMsg    = (MsgBlock*)VOS_AllocMsg(pstMsgBlk->ulSenderPid, pstMsgBlk->ulLength);

    /*若内存不足,释放互斥,直接返回*/
    if (VOS_NULL == pstSndOsaMsg)
    {
        OM_LogError(OM_MAILBOX_SendMsg_AllocFail);

        /* 复位 */
        OM_LogFatalAndReset(OM_MAILBOX_SendMsg_AllocFail);

        return VOS_ERR;
    }

    /* 将VOS_HEADER内容拷贝入消息中 */
    UCOM_MemCpy(pstSndOsaMsg,
                (VOS_VOID *)(&(pstMsgBlk->ulSenderCpuId)),
                sizeof(VOS_MSG_HEADER_STRU));

    /*从邮箱内容中读出消息内容(不含VOS头)，并更新队列状态记录临时结构体*/
    DRV_MAILBOX_ReadQueue(pstQueue,
                          (VOS_UCHAR*)pstSndOsaMsg->aucValue,
                          pstMsgBlk->ulLength);

    /*根据消息优先级发送OSA消息*/
    if (VOS_EMERGENT_PRIORITY_MSG == uwMsgPriority)
    {
        return VOS_SendUrgentMsg(pstMsgBlk->ulSenderPid, pstSndOsaMsg, DRV_MAILBOX_CHANNEL_VOS_CH);
    }
    else
    {
        return VOS_SendMsg(pstMsgBlk->ulSenderPid, pstSndOsaMsg, DRV_MAILBOX_CHANNEL_VOS_CH);
    }

}
VOS_VOID DRV_MAILBOX_ReceiveMsg(VOS_UINT32 uwMailboxId)
{
    VOS_CPU_SR                              uwCpuSr;
    VOS_UINT32                              uwMailboxAddr;                      /* 邮箱可写空间的首地址,与邮箱头首地址无必然关系 */
    VOS_UINT32                              uwMailboxLen;                       /* 邮箱长度，不含邮箱头，不含保护字长度 */
    VOS_UINT32                              uwTimeStamp;
    VOS_INT32                               swContentLen;                       /* 邮箱未接收消息长度 */
    MAILBOX_HEAD_STRU                      *pstMailboxHead;
    IPC_MSG_BLK_STRU                        stMsgBlk;                           /* 临时消息块结构体 */
    DRV_MAILBOX_QUEUE_STRU                  stQueue;                            /* 临时队列信息状态结构体 */


    /*获取邮箱长度，去除邮箱头部及尾部的四个保护字(共16byte)*/
    uwMailboxLen    = DRV_MAILBOX_GetMailBoxLen(uwMailboxId)
                       - (4*DRV_MAILBOX_PROTECT_LEN);

    /*获取邮箱首地址，去除邮箱头部两个保护字*/
    uwMailboxAddr       = DRV_MAILBOX_GetMailBoxAddr(uwMailboxId)
                           + (2*DRV_MAILBOX_PROTECT_LEN);

    /*锁中断互斥*/
    uwCpuSr             = VOS_SplIMP();

    /*按公共邮箱头格式MAILBOX_HEAD_STRU进行邮箱头地址内容解析*/
    pstMailboxHead      = (MAILBOX_HEAD_STRU*)DRV_MAILBOX_GetMailBoxHeadAddr(uwMailboxId);

    /*计算邮箱中待取数据长度，单位4byte*/
    if (pstMailboxHead->ulFront >= pstMailboxHead->ulRear)
    {
        swContentLen    = (VOS_INT32)(pstMailboxHead->ulFront - pstMailboxHead->ulRear);
    }
    else
    {
        swContentLen    = (VOS_INT32)((pstMailboxHead->ulFront + (uwMailboxLen/sizeof(VOS_INT32)))
                            - (pstMailboxHead->ulRear));
    }

    /*初始化队列状态记录临时结构体,读队列仅关心读指针*/
    stQueue.uwBase      = uwMailboxAddr;
    stQueue.uwLength    = uwMailboxLen;

    /*若出现邮箱头保护字不匹配，说明邮箱头被踩，此状态无法恢复，做复位处理*/
    if ((  DRV_MAILBOX_PROTECT1 != pstMailboxHead->ulProtectWord1)
        ||(DRV_MAILBOX_PROTECT1 != pstMailboxHead->ulProtectWord3)
        ||(DRV_MAILBOX_PROTECT2 != pstMailboxHead->ulProtectWord2)
        ||(DRV_MAILBOX_PROTECT2 != pstMailboxHead->ulProtectWord4))
    {
        VOS_Splx(uwCpuSr);

        /*触发软件中断,标明是读邮箱时邮箱头保护字异常*/
        OM_LogError(OM_MAILBOX_RecieveMsg_MailboxHeadPartitionErr);
        return;
    }

    /*若待取数据长度非0，即邮箱内容非空*/
    while (swContentLen > 0)
    {
        /*取最新读指针值*/
        stQueue.uwRear  = (pstMailboxHead->ulRear*sizeof(VOS_INT32)) + stQueue.uwBase;

        /*从邮箱内容队列中读出消息头信息，并更新队列状态记录临时结构体*/
        DRV_MAILBOX_ReadQueue(&stQueue,
                      (VOS_UCHAR*)&stMsgBlk,
                      sizeof(MAILBOX_MSG_HEADER));

        /*获取当前全系统时戳*/
        uwTimeStamp     = DRV_TIMER_ReadSysTimeStamp();

        /*若出现消息分隔字不匹配，说明读取消息出现异常，此状态无法恢复，做复位处理*/
        if (DRV_MAILBOX_MSGHEAD_NUMBER != stMsgBlk.stMsgHead.ulPartition)
        {
            VOS_Splx(uwCpuSr);

            /*触发软件中断,标明是消息间隔字异常*/
            OM_LogFatalAndReset(OM_MAILBOX_RecieveMsg_MsgPartitionErr);
            return;
        }

        /*检查读到的消息是否存在异常，包括SN号是否连续、消息滞留时间是否过长*/
        DRV_MAILBOX_CheckRecMsg(uwMailboxId, uwTimeStamp, &(stMsgBlk.stMsgHead));

        /* 读取邮件内容并转成VOS消息进行处理，过程中会修改stQueue的值 */
        if (VOS_OK != DRV_MAILBOX_Read(&stMsgBlk, &stQueue))
        {
            OM_LogError(OM_MAILBOX_RecieveMsg_MsgReadErr);
        }

        /*更新读指针，并向上取整为4Byte*/
        pstMailboxHead->ulRear      = ((stQueue.uwRear - stQueue.uwBase)
                                        + (sizeof(VOS_INT32) - 1))/sizeof(VOS_INT32);

        /*更新邮箱头读指针(尾指针)时间*/
        pstMailboxHead->ulRearslice = uwTimeStamp;

        /*更新待取数据长度*/
        swContentLen -= (VOS_INT32)(((stMsgBlk.stMsgHead.ulMsgLength + sizeof(MAILBOX_MSG_HEADER))
                            + (sizeof(VOS_INT32) - 1))/sizeof(VOS_INT32));
    }

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

    return;
}

#ifndef VOS_VENUS_TEST_STUB
VOS_UINT32 DRV_MAILBOX_SendMsg(
                VOS_UINT32                  uwCpuId,
                VOS_UINT32                  uwChannel,
                VOS_UINT32                  uwPriority,
                VOS_UCHAR                  *pucMsg,
                VOS_UINT32                  uwMsgSize)
{
    VOS_UINT32                              uwRet;
    DRV_MAILBOX_ID_ENUM_UINT16              enMailboxId;
    VOS_UINT32                              uwMailCode;

    /*获取写入的邮箱编号*/
    enMailboxId = DRV_MAILBOX_GetMailBoxTx(uwCpuId);

    /*若邮箱编号非法(或暂不支持),直接返回错误*/
    if (DRV_MAILBOX_ID_BUTT == enMailboxId)
    {
        return VOS_ERR;
    }

    /* 若非普通VOS通道的消息，去除VOS头后写入邮箱 */
    if (DRV_MAILBOX_CHANNEL_VOS_CH != uwChannel)
    {
        uwMsgSize   = uwMsgSize - sizeof(VOS_MSG_HEADER_STRU);
        pucMsg      = pucMsg + sizeof(VOS_MSG_HEADER_STRU);
    }

    uwMailCode = DRV_MAILBOX_GetMailCode(uwCpuId, uwChannel);

    /* 写入邮箱 */
    uwRet           = DRV_MAILBOX_Write(enMailboxId,
                                        uwCpuId,
                                        uwPriority,
                                        pucMsg,
                                        uwMsgSize,
                                        uwMailCode);
    /*若写入失败,直接返回错误*/
    if (VOS_ERR == uwRet)
    {
        return VOS_ERR;
    }

    /*向目标CPU触发IPC中断*/
    DRV_IPC_TrigInt(DRV_MAILBOX_GetTargetCpu(uwCpuId),
                    DRV_MAILBOX_GetIntrSrc(uwCpuId));

    return VOS_OK;

}
#endif

VOS_VOID DRV_SOCP_Init(VOS_VOID)
{
    VOS_UINT32                          uwStartAddr;
    VOS_UINT32                          uwBufLen         = 0;
    VOS_UINT32                          i;
    VOS_UINT16                          uhwSrcChanId     = DRV_SOCP_SRC_CHAN_ID_HIFI;
    VOS_UINT16                          uhwDestChanId    = DRV_SOCP_DES_CHAN_ID_HIFI;
    VOS_UINT32                          uwResetFlag;

    /* 需确保起始地址及Buff长度是8字节对齐的 */
    uwStartAddr = DRV_SOCP_CHAN_START_ADDR_HIFI;
    uwBufLen    = DRV_SOCP_CHAN_LEN_HIFI;

    /* 复位通道,写比特29(对应通道29)值为1 */
    UCOM_RegBitWr(DRV_SOCP_ENC_SRST_REG, uhwSrcChanId, uhwSrcChanId, 1);

    /* 等待通道自清 */
    for(i=0; i<DRV_SOCP_RESET_TIMECNT; i++)
    {
        uwResetFlag = UCOM_RegBitRd(DRV_SOCP_ENC_SRST_REG, uhwSrcChanId, uhwSrcChanId);
        if(0 == uwResetFlag)
        {
            break;
        }
    }

    /* 写入起始地址到源buffer起始地址寄存器*/
    UCOM_RegWr(DRV_SOCP_GET_ENCSRC_WPTR(uhwSrcChanId), uwStartAddr);
    UCOM_RegWr(DRV_SOCP_GET_ENCSRC_RPTR(uhwSrcChanId), uwStartAddr);
    UCOM_RegWr(DRV_SOCP_GET_ENCSRC_BUFADDR(uhwSrcChanId),uwStartAddr);
    UCOM_RegWr(DRV_SOCP_GET_ENCSRC_BUFCFG0(uhwSrcChanId), uwBufLen);

    /*配置源Buffer为连续存储数据包结构*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId),
                  UCOM_BIT1,
                  UCOM_BIT2,
                  DRV_SOCP_ENCSRC_CHNMODE_CTSPACKET);

    /*配置目的Buffer ID*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId),
                  UCOM_BIT4,
                  UCOM_BIT7,
                  uhwDestChanId);

    /*配置为最低优先级队列*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId),
                  UCOM_BIT8,
                  UCOM_BIT9,
                  DRV_SOCP_CHAN_PRIORITY_0);

    /*配置通道Bypass不使能*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId),
                  UCOM_BIT10,
                  UCOM_BIT10,
                  DRV_SOCP_ENCSRC_BYPASS_DISABLE);

    /*配置该通道为GU可维可测数据类型*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId),
                  UCOM_BIT16,
                  UCOM_BIT23,
                  DRV_SOCP_DATA_TYPE_1);

    /* 使能中断*/
    UCOM_RegBitWr(DRV_SOCP_REG_ENC_RAWINT1, uhwSrcChanId, uhwSrcChanId, 1);
    UCOM_RegBitWr(DRV_SOCP_REG_APP_MASK1, uhwSrcChanId, uhwSrcChanId, 0);

    /*启动通道*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId),
                  UCOM_BIT0,
                  UCOM_BIT0,
                  1);

}
VOS_UINT32 DRV_SOCP_Write(UCOM_DATA_BLK_STRU *pstBlk, VOS_UINT32 uwBlkNum)
{
    VOS_CPU_SR                              uwCpuSr;
    VOS_UINT32                              uwBlkCount;
    VOS_UINT32                              uwTotalSize     = 0;                /* 一个SOCP包中实际数据的总长度，单位byte*/
    VOS_UINT32                              uwLen           = 0;                /* 一个SOCP包中8byte对齐后实际写入缓存的数据总长度，单位byte */
    VOS_UINT32                              uwSizeLeft;                         /* 缓存剩余的空间 */
    VOS_UINT16                              uhwSrcChanId    = DRV_SOCP_SRC_CHAN_ID_HIFI;
    DRV_SOCP_EMPTY_MSG_STRU                 stEmptyMsg;                         /* SOCP消息包临时结构体，不含实际消息内容 */
    DRV_MAILBOX_QUEUE_STRU                  stQueue;                            /* 临时队列状态记录结构体 */

    /*所有BLK作为一个SOCP包发送,计算实际数据总长度*/
    for (uwBlkCount = 0; uwBlkCount < uwBlkNum; uwBlkCount++)
    {
        uwTotalSize += pstBlk[uwBlkCount].uwSize;
    }

    /*计算8bytes对齐总长度,此处使用宏反而不利于理解,因此未作宏定义*/
    uwLen           = (uwTotalSize + 7)&0xfffffff8;

    /*读取SOCP缓存首地址,并存为临时队列状态结构体*/
    /* 当前SOCP缓存处于HIFI子系统的Cache访问地址范围，此处需要制定是Uncache访问,下同 */
    stQueue.uwBase  = UCOM_RegRd(DRV_SOCP_GET_ENCSRC_BUFADDR(uhwSrcChanId));

    /*读取SOCP缓存长度(低25位),单位byte,并存为临时队列状态结构体*/
    stQueue.uwLength= UCOM_RegRd(DRV_SOCP_GET_ENCSRC_BUFCFG0(uhwSrcChanId))&0x1ffffff;

    /*锁中断互斥*/
    uwCpuSr         = VOS_SplIMP();

    /*读取SOCP缓存写(头)指针,并存为临时队列状态结构体*/
    stQueue.uwFront = UCOM_RegRd(DRV_SOCP_GET_ENCSRC_WPTR(uhwSrcChanId));

    /*读取SOCP缓存读(尾)指针,并存为临时队列状态结构体*/
    stQueue.uwRear  = UCOM_RegRd(DRV_SOCP_GET_ENCSRC_RPTR(uhwSrcChanId));

    /*计算可写入缓存长度*/
    if (stQueue.uwRear > stQueue.uwFront)
    {
        uwSizeLeft  = (stQueue.uwRear - stQueue.uwFront) - 1;
    }
    else
    {
        uwSizeLeft  = (stQueue.uwLength + stQueue.uwRear) - (stQueue.uwFront + 1);
    }


    /*若可写入缓存不足，直接返回错误*/
    if (uwSizeLeft < (uwLen + sizeof(DRV_SOCP_EMPTY_MSG_STRU)))
    {
        /*释放互斥*/
        VOS_Splx(uwCpuSr);
        return VOS_ERR;
    }

    /*填充stEmptyMsg值*/
    stEmptyMsg.uwMagicNum               = DRV_SOCP_MAGIC_NUMBER;
    stEmptyMsg.uhwMsgLen                = (VOS_UINT16)uwTotalSize + DRV_SOCP_HEADER_LEN;
    stEmptyMsg.stFrmHead.ucCpuId        = DRV_SOCP_HIFI_SESSION_ID;
    stEmptyMsg.uwUsbHead                = DRV_SOCP_USBHEAD_NUMBER;
    stEmptyMsg.stFrmSegHead.uhwSegLen   = (VOS_UINT16)uwTotalSize
                                           + sizeof(stEmptyMsg.stFrmSegHead);
    stEmptyMsg.stFrmSegHead.ucSegNum    = DRV_SOCP_SEG_NUM_ONE;                 /*不分段，固定填1*/
    stEmptyMsg.stFrmSegHead.ucSegSn     = DRV_SOCP_SEG_SN_ONE;                  /*1表示当前段序号为1*/
    stEmptyMsg.uwUsbTail                = DRV_SOCP_USBTAIL_NUMBER;

    /*写入消息块头部信息*/
    DRV_MAILBOX_WriteQueue(&stQueue,
                           (VOS_UCHAR*)&stEmptyMsg,
                           sizeof(stEmptyMsg) - sizeof(stEmptyMsg.uwUsbTail));

    for (uwBlkCount = 0; uwBlkCount < uwBlkNum; uwBlkCount++)
    {
        /*逐个BLK写入数据内容*/
        DRV_MAILBOX_WriteQueue(&stQueue, pstBlk[uwBlkCount].pucData, pstBlk[uwBlkCount].uwSize);
    }

    /*写入消息块结束符*/
    DRV_MAILBOX_WriteQueue(&stQueue,
                           (VOS_UCHAR*)&stEmptyMsg.uwUsbTail,
                           sizeof(stEmptyMsg.uwUsbTail));

    /*对写指针进行8byte向上对齐*/
    stQueue.uwFront = (stQueue.uwFront+ 7)&0xfffffff8;

    /*若写指针对齐后位于源Buffer尾部，则需转圈为Buffer头部*/
    if (stQueue.uwFront == (stQueue.uwBase + stQueue.uwLength))
    {
        stQueue.uwFront = stQueue.uwBase;
    }

    /*将映射地址转换为总线地址后，更新stQueue写指针到DRV_SOCP_ENCSRC_WPTR_REG*/
    UCOM_RegWr(DRV_SOCP_GET_ENCSRC_WPTR(uhwSrcChanId), stQueue.uwFront);

    /*释放互斥*/
    VOS_Splx(uwCpuSr);

    return VOS_OK;
}


VOS_VOID DRV_SOCP_Stop(VOS_VOID)
{
    VOS_UINT16                          uhwSrcChanId     = DRV_SOCP_SRC_CHAN_ID_HIFI;

    /*停止通道*/
    UCOM_RegBitWr(DRV_SOCP_GET_ENCSRC_BUFCFG1(uhwSrcChanId), UCOM_BIT0, UCOM_BIT0, 0);

}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

