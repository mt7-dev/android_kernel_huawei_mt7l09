
#ifdef __cplusplus
    extern "C" {
#endif

#include "sys.h"
#include "emmcConfig.h"

#ifdef EMMC_IDMAC_SUPPORT
#include "string.h"
#include "config.h"
#include "OcrShare.h"
#include "OnChipRom.h"
#include "emmcIp.h"
#include "emmcErrcode.h"
#include "emmcIdmac.h"
#include "emmcNormal.h"
#include "emmcMain.h"

/* 获取数据分段类型 */
EMMC_IDMAC_PARTITION_TYPE_E emmc_idmac_get_node_partition(UINT32 i,UINT32 cnt)
{
    if(1 == cnt)    return EMMC_IDMAC_PARTITION_TYPE_TOTAL;
    else if(i == 0) return EMMC_IDMAC_PARTITION_TYPE_HEAD;
    else if(i == cnt-1) return EMMC_IDMAC_PARTITION_TYPE_TAIL;
    else    return EMMC_IDMAC_PARTITION_TYPE_MID;
}

UINT32 emmc_idmac_set_qptr(EMMC_IDMAC_HANDLE_S *imachandle,EMMC_IDMAC_PARTITION_TYPE_E ePartType,UINT32 buf1,UINT32 len1, DmaDesc **desc_over)
{
    UINT32  next = imachandle->Next;
    DmaDesc *desc = imachandle->Desc + next;
    UINT32 arrIdmacQptrFlag[EMMC_IDMAC_PARTITION_TYPE_MAX]=
    {
        (DisIntComplete|DescFirst),
        (DisIntComplete|DescOwnByDma),
        (DisIntComplete|DescLast|DescOwnByDma),
        (DisIntComplete|DescFirst|DescLast|DescOwnByDma)
    };

    desc->size = (len1<<DescSize1Shift) & DescSize1Mask;   
    desc->status = (next + 1 != imachandle->DescCount)?
        arrIdmacQptrFlag[ePartType]:(arrIdmacQptrFlag[ePartType]|DescEndOfRing);
 	desc->buf1 = buf1;

#ifdef EMMC_IDMAC_DESC_2_POWER
    imachandle->Next = (next + 1)&(imachandle->DescCount - 1);
#else
    imachandle->Next = ((next + 1) != imachandle->DescCount)?(next + 1):0;
#endif

    *desc_over = desc;

    return next;
}

UINT32 emmc_reset_idmac(VOID)
{
    TIMER0_VAR_DECLAR;

    /*
        software reset idmac
    */
    SETREG32(EMMC_REG_BMOD,BMOD_SWR);
    TIMER0_INIT_TIMEOUT(EMMC_RESET_IDMAC_TIMEOUT);
    for(;;)
    {
        if(0 == (INREG32(EMMC_REG_BMOD) & BMOD_SWR))
        {
            break;
        }
        
        if(TIMER0_IS_TIMEOUT())
        {
            return ERRRETRYTIMEOUT; 
        }
    }

    return ERRNOERROR;
}

void emmc_init_ring_desc(DmaDesc *desc, BOOL last_ring_desc)
{
	desc->status    = last_ring_desc ? DescEndOfRing : 0;
	desc->size      = 0;
	desc->buf1      = 0;
	desc->buf2      = 0;
	return;
}

VOID emmc_setup_desc_list(DmaDesc *desc, UINT32 no_of_desc)
{
    UINT32 i;
    
    for(i = 0; i < no_of_desc; i++)
    {
        emmc_init_ring_desc(desc + i, i == (no_of_desc - 1));
    }
}

UINT32 emmc_idmac_init(EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal;
    EMMC_IDMAC_HANDLE_S *pstIdmacHandle = &pstGlbVar->stIdmacHandle;

    /* reset idmac */
    ulRetVal = emmc_reset_idmac();
    if(ulRetVal)
    {
        return ulRetVal;
    }

    /* 初始化内置DMA句柄 */
    memset((VOID *)pstIdmacHandle,0x0,sizeof(EMMC_IDMAC_HANDLE_S));
    pstIdmacHandle->DescCount = EMMC_IDMAC_DESC_NUM;
    pstIdmacHandle->Next = 0;
    pstIdmacHandle->NextDesc = pstIdmacHandle->Desc;

    /* 设置总线模式 */
    EMMC_DMA_BUS_MODE_INIT(DmaEnable | DmaFixedBurstEnable | DmaDescriptorSkip0);

    /* 清除所有的中断状态 */
    EMMC_DMA_INTERRUPT_CLEAR(DmaStatusAbnormal|DmaStatusNormal|DmaStatusCardErrSum|DmaStatusNoDesc|DmaStatusBusError|DmaStatusRxCompleted|DmaStatusTxCompleted);

    /* 设置中断使能位 */
    EMMC_DMA_INTERRUPT_MASK(0);

    /* 创建描述符链 */
    emmc_setup_desc_list(pstIdmacHandle->Desc,pstIdmacHandle->DescCount);    

    /* 将描述符链表首地址通知DMA引擎 */
    EMMC_IDMAC_INIT_DESC_BASE(pstIdmacHandle);    

    SETREG32(EMMC_REG_CTRL, EMMC_CTRL_BIT_IDMAC);

    return ERRNOERROR;
}

#ifdef EMMC_DEBUG
VOID emmc_handle_dump(EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    print_info_with_u32("\r\nDesc     :0x",(UINT32)pstGlbVar->stIdmacHandle.Desc);
    print_info_with_u32("\r\nDescCount:0x",pstGlbVar->stIdmacHandle.DescCount);
    print_info_with_u32("\r\nNext     :0x",pstGlbVar->stIdmacHandle.Next);
    print_info_with_u32("\r\nNextDesc :0x",(UINT32)pstGlbVar->stIdmacHandle.NextDesc);    
}

VOID emmc_desc_dump(DmaDesc *desc)
{
    print_info_with_u32("\r\nstatus:0x",desc->status);
    print_info_with_u32(" size  :0x",desc->size);
    print_info_with_u32(" buf1  :0x",desc->buf1);
    print_info_with_u32(" buf2  :0x" ,desc->buf2);
}
#endif

VOID emmc_idmac_nocopysend
(
    EMMC_IDMAC_HANDLE_S *pstIdmacHandle,
    UINT32 ulbuf,
    UINT32 ulLen,
    UINT32 ulNodeCnt
)
{
    UINT32 i = 0;
    UINT32 buf = ulbuf;
    DmaDesc *desc = NULL;
    DmaDesc *descTag = NULL;
    UINT32 len = 0;
    UINT32 len_remain = ulLen;
    EMMC_IDMAC_PARTITION_TYPE_E ePartitionType = EMMC_IDMAC_PARTITION_TYPE_TOTAL;

    for(i=0; i<ulNodeCnt; i++)
    {
        /* 检查数据分段类型 */
        ePartitionType = emmc_idmac_get_node_partition(i,ulNodeCnt);
        switch(ePartitionType)
        {
            case EMMC_IDMAC_PARTITION_TYPE_TOTAL:
                buf = ulbuf;
                len = ulLen;
                pstIdmacHandle->Next  =  0;
                EMMC_IDMAC_INIT_DESC_BASE(pstIdmacHandle);
                break;

            case EMMC_IDMAC_PARTITION_TYPE_HEAD:
                buf = ulbuf;
                len = EMMC_IDMAC_BUF_SIZE;
                pstIdmacHandle->Next  =  0;
                EMMC_IDMAC_INIT_DESC_BASE(pstIdmacHandle);
                break;

            case EMMC_IDMAC_PARTITION_TYPE_MID:
                buf = ulbuf+i*EMMC_IDMAC_BUF_SIZE;
                len = EMMC_IDMAC_BUF_SIZE;
                break;

            case EMMC_IDMAC_PARTITION_TYPE_TAIL:
                buf = ulbuf+i*EMMC_IDMAC_BUF_SIZE;
                len = len_remain;
                break;
            default:
                break;
        }

        /* 填充描述符 */
        (VOID)emmc_idmac_set_qptr(pstIdmacHandle,ePartitionType,(UINT32)buf,(UINT32)len,&desc);

        if((EMMC_IDMAC_PARTITION_TYPE_HEAD == ePartitionType)
            ||(EMMC_IDMAC_PARTITION_TYPE_TOTAL == ePartitionType))
        {
            descTag = desc;
        }

        len_remain -= len;
    }

    /* 设置第一个描述符OWN */
    if(descTag)
    {
        EMMC_IDMAC_SET_DMAOWN(descTag);
    }
    
#ifdef EMMC_DEBUG
    for(i=0;i<ulNodeCnt;i++)
    {
        if(descTag)
        {
            emmc_desc_dump(descTag+i);
        }        
    }
#endif

    EMMC_IDMAC_RESUME_DMA();
}

VOID emmc_idmac_nocopyreceive(EMMC_IDMAC_HANDLE_S *pstIdmacHandle,UINT32 ulbuf,UINT32 ulLen,UINT32 ulNodeCnt)
{
    emmc_idmac_nocopysend(pstIdmacHandle,ulbuf,ulLen,ulNodeCnt);
}

#endif
#ifdef __cplusplus
}
#endif
