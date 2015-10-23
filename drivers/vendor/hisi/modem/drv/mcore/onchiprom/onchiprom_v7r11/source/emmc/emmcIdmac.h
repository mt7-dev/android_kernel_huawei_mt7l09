
#ifndef __EMMC_IDMAC_H__
#define __EMMC_IDMAC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "emmcConfig.h"
#include "config.h"

#ifdef EMMC_IDMAC_SUPPORT
#include "sys.h"

/* 每个描述符对应的最大buf大小 */
#define EMMC_IDMAC_BUF_SIZE (512)

/* 描述符个数 */
#define EMMC_IDMAC_DESC_NUM ((BOOTLOAD_SIZE_MAX + \
                                        EMMC_IDMAC_BUF_SIZE - 1)/EMMC_IDMAC_BUF_SIZE)

/* 描述符个数是否为2的n次幂 */
//#define EMMC_IDMAC_DESC_2_POWER

/* 总线宽度 */
#define SDIO_BUS_WITDTH 4

typedef struct DmaDescStruct
{
    UINT32 status;      /* control and status information of descriptor */
    UINT32 size;        /* buffer sizes                                 */
    UINT32 buf1;        /* physical address of the buffer 1             */
    UINT32 buf2;        /* physical address of the buffer 2             */
}DmaDesc;

/*
 DBADDR  = 0x88 : Descriptor List Base Address Register
 The DBADDR is the pointer to the first Descriptor
 The Descriptor format in Little endian with a 32 bit Data bus is as shown below
           --------------------------------------------------------------------------
     DES0 | OWN (31)| Control and Status                                             |
        --------------------------------------------------------------------------
     DES1 | Reserved |         Buffer 2 Size        |        Buffer 1 Size           |
       --------------------------------------------------------------------------
     DES2 |  Buffer Address Pointer 1                                                |
       --------------------------------------------------------------------------
     DES3 |  Buffer Address Pointer 2 / Next Descriptor Address Pointer              |
       --------------------------------------------------------------------------
*/
#if 0
enum DmaDescriptorDES0    // Control and status word of DMA descriptor DES0
{
     DescOwnByDma          = 0x80000000,   /* (OWN)Descriptor is owned by DMA engine              31   */
     DescCardErrSummary    = 0x40000000,   /* Indicates EBE/RTO/RCRC/SBE/DRTO/DCRC/RE             30   */
     DescEndOfRing         = 0x00000020,   /* A "1" indicates End of Ring for Ring Mode           05   */
     DescSecAddrChained    = 0x00000010,   /* A "1" indicates DES3 contains Next Desc Address     04   */
     DescFirstDesc         = 0x00000008,   /* A "1" indicates this Desc contains first            03
                                              buffer of the data                                       */
     DescLastDesc          = 0x00000004,   /* A "1" indicates buffer pointed to by this this      02
                                              Desc contains last buffer of Data                        */
     DescDisInt            = 0x00000002,   /* A "1" in this field disables the RI/TI of IDSTS     01
                                              for data that ends in the buffer pointed to by
                                              this descriptor                                          */
};
#endif

enum DmaBusModeReg
{                                          /* Bit description                                   Bits    R/W */
    DmaBurstLength256       = 0x00000700,   /* Dma burst length = 256                             10:8    R   */
    DmaBurstLength128       = 0x00000600,   /* Dma burst length = 128                             10:8    R   */
    DmaBurstLength64        = 0x00000500,   /* Dma burst length = 64                              10:8    R   */
    DmaBurstLength32        = 0x00000400,   /* Dma burst length = 32                              10:8    R   */
    DmaBurstLength16        = 0x00000300,   /* Dma burst length = 16                              10:8    R   */
    DmaBurstLength8         = 0x00000200,   /* Dma burst length = 8                               10:8    R   */
    DmaBurstLength4         = 0x00000100,   /* Dma burst length = 4                               10:8    R   */
    DmaBurstLength1         = 0x00000000,   /* Dma burst length = 1                               10:8    R   */

    DmaEnable               = 0x00000080,   /* IDMAC Enable                                       7       RW  */
    DmaDisable              = 0x00000000,   /* IDMAC Disable                                      7       RW  */

    DmaDescriptorSkip16     = 0x00000040,   /* (DSL)Descriptor skip length (no.of dwords)         6:2     RW  */
    DmaDescriptorSkip8      = 0x00000020,   /* between two unchained descriptors                              */
    DmaDescriptorSkip4      = 0x00000010,   /*                                                                */
    DmaDescriptorSkip2      = 0x00000008,   /*                                                                */
    DmaDescriptorSkip1      = 0x00000004,   /*                                                                */
    DmaDescriptorSkip0      = 0x00000000,   /*                                                                */

    DmaFixedBurstEnable     = 0x00000002,   /* (FB)Fixed Burst SINGLE, INCR4, INCR8 or INCR16     1       RW  */
    DmaFixedBurstDisable    = 0x00000000,   /* SINGLE, INCR                                       1       RW  */

    DmaResetOn              = 0x00000001,   /* (SWR)Software Reset DMA engine                     0       RW  */
    DmaResetOff             = 0x00000000   /*                                                     0       RW  */
};

enum DmaIntStatusReg
{
    DmaStatusAbnormal      = 0x00000200,   /* (AIS)Abnormal interrupt summary                  9       RW  */
    DmaStatusNormal        = 0x00000100,   /* (NIS)Normal interrupt summary                    8       RW  */

    DmaStatusCardErrSum    = 0x00000020,   /* Card Error Summary                               5       RW  */
    DmaStatusNoDesc        = 0x00000010,   /* Descriptor Unavailable                           4       RW  */
    DmaStatusBusError      = 0x00000004,   /* Fatal bus error (Abnormal)                       2       RW  */
    DmaStatusRxCompleted   = 0x00000002,   /* Receive completed (Abnormal)                     1       RW  */
    DmaStatusTxCompleted   = 0x00000001    /* Transmit completed (Normal)                      0       RW  */
};

enum DmaIntEnableReg
{
    DmaIntAbnormal          = 0x00000200,   /* (AIS)Abnormal interrupt summary                  9       RW  */
    DmaIntNormal            = 0x00000100,   /* (NIS)Normal interrupt summary                    8       RW  */

    DmaIntCardErrSum        = 0x00000020,   /* Card Error Summary                               5       RW  */
    DmaIntNoDesc            = 0x00000010,   /* Descriptor Unavailable                           4       RW  */
    DmaIntBusError          = 0x00000004,   /* Fatal bus error (Abnormal)                       2       RW  */
    DmaIntRxCompleted       = 0x00000002,   /* Receive completed (Abnormal)                     1       RW  */
    DmaIntTxCompleted       = 0x00000001    /* Transmit completed (Normal)                      0       RW  */
};

/**********************************************************
 * DMA Engine descriptors
 **********************************************************/
enum DmaDescriptorStatus
{
    DescOwnByDma        = 0x80000000,   /* (OWN)Descriptor is owned by DMA engine               31      RW   */
    DescCardErrorSum    = 0x40000000,   /* (CES)status of the transaction to or from the card   30      RW   */

    DescEndOfRing       = 0x00000020,   /* (ER)the first buffer of the data                     5      RW   */
    DescChain           = 0x00000010,   /* (CH)the second address in the descriptor is the
    Next Descriptor address rather than the second buffer address. 4      RW   */

    DescFirst            = 0x00000008,    /* (FS)indicates that this descriptor contains the
    first buffer of the data            3      RW   */
    DescLast           = 0x00000004,    /* (LD)indicates that the buffers pointed to by this descriptor
    are the last buffers of the data    2      RW   */
    DisIntComplete      = 0x00000002,    /* (DIC)prevent the setting of the TI/RI bit of the IDMAC
    Status Register (IDSTS) for the data that ends in the buffer pointed to by this descriptor  1      RW   */

    DescSize2Mask         = 0x3FFE000,   /* (TBS2) Buffer 2 size                                25:13                    */
    DescSize2Shift        = 13,
    DescSize1Mask         = 0x00001FFF,   /* (TBS1) Buffer 1 size                                12:0                     */
    DescSize1Shift        = 0
};

typedef struct tagEMMC_IDMAC_HANDLE_S
{
    UINT32  DescCount;  /* number of descriptors in the descriptor queue/pool */
    UINT32  Next;       /* index of the descriptor next available with driver, given to DMA */
    DmaDesc *NextDesc;  /* Descriptor address corresponding to the index Next */
    DmaDesc Desc[EMMC_IDMAC_DESC_NUM];  /* start address of descriptors ring or chain, this is used by the driver */
} EMMC_IDMAC_HANDLE_S;

/* IDMAC包分段枚举 */
typedef enum tagEMMC_IDMAC_PARTITION_TYPE_E
{
    EMMC_IDMAC_PARTITION_TYPE_HEAD = 0,/* 首段包 */
    EMMC_IDMAC_PARTITION_TYPE_MID,     /* 中段包 */
    EMMC_IDMAC_PARTITION_TYPE_TAIL,    /* 尾段包 */
    EMMC_IDMAC_PARTITION_TYPE_TOTAL,   /* 整段包 */
    EMMC_IDMAC_PARTITION_TYPE_MAX      /* 边界值 */
}EMMC_IDMAC_PARTITION_TYPE_E;

#define EMMC_IDMAC_GET_DESC_COUNT(len) (((len)+(EMMC_IDMAC_BUF_SIZE-1))/EMMC_IDMAC_BUF_SIZE)

#define EMMC_IDMAC_SET_DMAOWN(desc) \
do\
{\
    (desc)->status |= (UINT32)DescOwnByDma;\
}while(0)

#define EMMC_IDMAC_RESUME_DMA() OUTREG32(EMMC_REG_PLDMND, 1)

#define EMMC_DMA_BUS_MODE_INIT(init_value) OUTREG32(EMMC_REG_BMOD, (init_value))

#define EMMC_DMA_INTERRUPT_MASK(init_value) OUTREG32(EMMC_REG_IDINTEN, (init_value))

#define EMMC_DMA_INTERRUPT_CLEAR(init_value) OUTREG32(EMMC_REG_IDSTS, (init_value))

#define EMMC_IDMAC_DESC_INIT_RING(desc, last_ring_desc)\
    do{\
        (desc)->status = (last_ring_desc)? DescEndOfRing : 0;\
        (desc)->length = 0;\
    }while(0)

#define EMMC_IDMAC_INIT_DESC_BASE(idmachandle)	OUTREG32(EMMC_REG_DBADDR, (UINT32)idmachandle->Desc)

EMMC_IDMAC_PARTITION_TYPE_E emmc_idmac_get_node_partition(UINT32 i,UINT32 cnt);
UINT32 emmc_idmac_set_qptr(EMMC_IDMAC_HANDLE_S *imachandle,EMMC_IDMAC_PARTITION_TYPE_E ePartType,UINT32 buf1,UINT32 len1, DmaDesc **desc_over);
VOID emmc_idmac_nocopysend(EMMC_IDMAC_HANDLE_S *pstIdmacHandle,UINT32 ulbuf,UINT32 ulLen,UINT32 ulNodeCnt);
VOID emmc_idmac_nocopyreceive(EMMC_IDMAC_HANDLE_S *pstIdmacHandle,UINT32 ulbuf,UINT32 ulLen,UINT32 ulNodeCnt);
#endif

#ifdef __cplusplus
}
#endif

#endif
