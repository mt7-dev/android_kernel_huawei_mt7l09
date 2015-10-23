#ifndef _SDIO_SLAVE_HAL_H
#define _SDIO_SLAVE_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_comm.h"
#include <mach/hardware.h>
#include <asm/cacheflush.h>
#include <asm/io.h>

//#define SLAVE_CFG_SPS_SUPPORT  //是否支持高电压
#define SLAVE_CFG_SHS_SUPPORT    //是否支持高速模式
#define SLAVE_CFG_UHS_SUPPORT

extern BSP_U32 g_u32SlaveBase;

//配置system address寄存器
#define sdio_adma_init_desc_base(pstAdmahandle)\
	BSP_REG_WRITE(g_u32SlaveBase, ADMA_SYS_ADDR, (BSP_U32)pstAdmahandle->DescDma)
//读中断状态
#define SLAVE_IntStatus_Global()\
    BSP_REG(g_u32SlaveBase,GLOBAL_INT_STATUS)

#define SLAVE_IntStatus_Fun0()\
    BSP_REG(g_u32SlaveBase,AHB_FUN0_INT_STATUS)

#define SLAVE_IntStatus_Fun1()\
    BSP_REG(g_u32SlaveBase,AHB_FUN1_INT_STATUS)

#define SLAVE_IntStatus_Mem()\
    BSP_REG(g_u32SlaveBase,AHB_MEM_INT_STATUS)

//清中断状态
#define SLAVE_IntClr_Global(value)\
    BSP_REG_WRITE(g_u32SlaveBase,GLOBAL_INT_STATUS,value)

#define SLAVE_IntClr_Fun0(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_FUN0_INT_STATUS,value)
    
#define SLAVE_IntClr_Fun1(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_FUN1_INT_STATUS,value)
    
#define SLAVE_IntClr_Mem(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_MEM_INT_STATUS,value)

//中断使能 中断屏蔽  
#define SLAVE_IntMask_Global(value) \
    BSP_REG_WRITE(g_u32SlaveBase,GLOBAL_INT_STATUS_ENABLE,(value))

#define SLAVE_IntMask_Mem(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_MEM_INT_ENABLE,value)
    
#define SLAVE_IntMask_Fun0(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_FUN0_INT_ENABLE,value)
    
#define SLAVE_IntMask_Fun1(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_FUN1_INT_ENABLE,value)
 //读blocksize 参数寄存器，并解析参数     
#define SLAVE_GetBlockSize()\
    (BSP_REG(g_u32SlaveBase,BLOCK_SIZE_REG)&0xfff)
    
#define SLAVE_GetArgument()\
    BSP_REG(g_u32SlaveBase,ARGUMENT_REG)

#define SLAVE_GetCount(Argument)\
    ((Argument)&ARGUMENT_BITS_COUNT)
    
#define SLAVE_GetBlkMode(Argument)\
    (ARGUMENT_BITS_BLK_MODE == ((Argument)&ARGUMENT_BITS_BLK_MODE))

#define SLAVE_GetOpMode(Argument)\
    (ARGUMENT_BITS_OP_MODE == ((Argument)&ARGUMENT_BITS_OP_MODE))

#define SLAVE_GetRwDir(Argument)\
    (ARGUMENT_BITS_RW_FLAG == ((Argument)&ARGUMENT_BITS_RW_FLAG))

#define SLAVE_GetFunNo(Argument)\
    (((Argument)&ARGUMENT_BITS_FUN_NO)>>28)

#define SLAVE_GetRegAddr(Argument)\
    (((Argument)&ARGUMENT_BITS_REGOFS)>>9)

#define SLAVE_GetReady()\
    BSP_REG(g_u32SlaveBase ,ESW_CARD_RDY)
         
#define SLAVE_Fun1_Ready()\
    BSP_REG_WRITE(g_u32SlaveBase,ESW_IOR_REG,PROG_FUN1_IOR_RDY)
     
#define SLAVE_Fun_Ready()\
    BSP_REG_WRITE(g_u32SlaveBase,ESW_FUN_RDY,PROG_FUN_RDY_BIT)

#define SLAVE_GetCsaPointer()\
    BSP_REG(g_u32SlaveBase,CSA_POINTER)

//消息寄存器读写
#define SLAVE_MsgWrite(u32Msg)\
    BSP_REG_WRITE(g_u32SlaveBase,ARM_GENERAL_PURPOSE,u32Msg)
#define SLAVE_MsgRead()\
    BSP_REG(g_u32SlaveBase,HOST_GENERAL_PURPOSE)

//写transfer cnt寄存器
#define SLAVE_TranCnt(value)\
    BSP_REG_WRITE(g_u32SlaveBase,AHB_TRANS_CNT,value)
    
/*set FUN1_RD_DATA_RDY bit to 1*/
#define SLAVE_SetDataRdy()\
    BSP_REG_WRITE(g_u32SlaveBase,FUN1_RD_DATA_RDY,1)

//32bit address descriptor Table Struction
typedef struct tagADMA_DESC_S
{
    BSP_U16     Attribute;          //0-5bit arrtib area,6-15bit reserved
    BSP_U16     BufferLen;          //16bit length area
    BSP_U32     NodeAddr;           //64bit address area    
    
}ADMA_DESC_S;

/* ADMA desciptor Struction */
typedef struct tagADMA_HANDLE_S
{
	BSP_U32 DescDma;            /* Dma-able address of descriptor */
    ADMA_DESC_S *Desc;          /* start address of descriptors  */
    BSP_U32 Next;               /* index of the descriptor line next available to ADMA */
    ADMA_DESC_S *NextDesc;      /* Descriptor address corresponding to the index Next */
    BSP_U32 DescCount;          /* number of descriptor lines used */    
	dma_addr_t Desc_Phy;
	BSP_U32 size;
	
    BSP_VOID *assist_buf;       /* 辅助buf，用于字节不对齐时的处理 */ 
    BSP_VOID *assist_buf_notalign;
    dma_addr_t assist_buf_phy;
    dma_addr_t assist_buf_phy_notalign;
	unsigned int assist_buf_len;
    BSP_BOOL bDescSetupFlag;    /* 描述符是否建立完成标识 */    

} ADMA_HANDLE_S;

/* ADMA 描述符类型枚举*/
typedef enum tagADMA_DESC_TYPE_E
{
    ADMA_DESC_TYPE_HEAD = 0,   
    ADMA_DESC_TYPE_MID,        
    ADMA_DESC_TYPE_TAIL,       
    ADMA_DESC_TYPE_TOTAL,      
    ADMA_DESC_TYPE_NULL,      
    ADMA_DESC_TYPE_MAX         
}ADMA_DESC_TYPE_E;

typedef struct tagADMA_CACHE_OP_S
{
    BSP_BOOL bFlushDesc;
    BSP_BOOL bTxBuf;
    BSP_BOOL bRxBuf;
}ADMA_CACHE_OP_S;

/*define attribute area*/
#define ATTRIB_ACT_TRANS    0x0020  //point to next descriptor line
#define ATTRIB_ACT_LINK     0x0030  //point to next descriptor
#define ATTRIB_FLAG_INT     0x0004  //give adma_int
#define ATTRIB_FLAG_END     0x0002  //all data have been transacted
#define ATTRIB_FLAG_NOP     0x0000  //no operation
#define ATTRIB_FLAG_VALID   0x0001  //valid bit

BSP_S32 SLAVE_IOEnumCfg(BSP_VOID);
BSP_S32 SLAVE_IOMEM_EnumCfg(BSP_VOID);
BSP_S32 SLAVE_MEM_EnumCfg(BSP_VOID);
BSP_S32 hal_sdio_S2MRdySend(BSP_U32 Length);
BSP_VOID hal_sdio_S2MMsgSend(BSP_U32 u32Msg);
BSP_VOID SLAVE_UHS_Support(BSP_BOOL bUhs);
BSP_VOID SLAVE_SHS_Support(BSP_BOOL bSpi);
BSP_VOID SLAVE_IO_Mode(BSP_U32 value);
BSP_VOID SLAVE_Version_Set(BSP_BOOL bflag);
BSP_VOID SLAVE_VolSwt();
BSP_VOID SLAVE_Rst();
BSP_VOID SLAVE_Sps_Support(BSP_BOOL bSpi);
BSP_VOID SLAVE_CSA_Support(BSP_BOOL bSup);
BSP_VOID SLAVE_int_init(BSP_VOID);
BSP_VOID SLAVE_fun1_switch(BSP_BOOL bMode);
BSP_VOID SLAVE_RegPrint();
BSP_VOID SLAVE_int_disable();
void slave_reinit();
#ifdef __cplusplus
}
#endif

#endif
