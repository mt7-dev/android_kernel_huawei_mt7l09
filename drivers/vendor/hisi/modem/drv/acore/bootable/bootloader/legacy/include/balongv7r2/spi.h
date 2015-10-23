/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  spi.h
*
*   作    者 :  wuzechun
*
*   描    述 :  spi.h 的头文件
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__

#include <balongv7r2/types.h>
#include <hi_spi.h>
#include <soc_memmap.h>

#define DUMMY_DATA  0x00

#define ECS_SPI0_BASE HI_SPI_MST0_REGBASE_ADDR /*SPI0控制器基址*/
#define ECS_SPI1_BASE HI_SPI_MST1_REGBASE_ADDR /*SPI1控制器基址*/
extern const u32 spiBase[2];
#define SPI_BASE(x)        (spiBase[x])   /* SPI基地址 */

#define SPI_CTRL0(x)       (SPI_BASE(x) + HI_SPI_CTRLR0_OFFSET)
/*配置Ctrl0寄存器时，各成员的偏移*/
#define SPI_CTRL0_DFS_BITPOS        0   /* 数据长度 */
#define SPI_CTRL0_FRF_BITPOS        4   /* 帧类型 */
#define SPI_CTRL0_SCPH_BITPOS       6   /* 时钟相位 */
#define SPI_CTRL0_SCPOL_BITPOS      7   /* 时钟极性 */

#define SPI_CTRL0_TMOD_BITPOS       8   /* 传输模式设置 */
#define SPI_CTRL0_TMOD_BITWIDTH     2
#define SPI_CTRL0_TMOD_BITMASK     (((1<<SPI_CTRL0_TMOD_BITWIDTH)-1)<<SPI_CTRL0_TMOD_BITPOS)
#define SPI_CTRL0_TMOD_SEND_RECV    (0x00<<SPI_CTRL0_TMOD_BITPOS)   /* 收发模式 */
#define SPI_CTRL0_TMOD_SEND         (0x01<<SPI_CTRL0_TMOD_BITPOS)   /* 发送模式 */
#define SPI_CTRL0_TMOD_RECV         (0x02<<SPI_CTRL0_TMOD_BITPOS)   /* 接收模式 */
#define SPI_CTRL0_TMOD_EEPROM_READ  (0x03<<SPI_CTRL0_TMOD_BITPOS)   /* EEPROM读模式 */

#define SPI_CFS_BITPOS              12  /* 控制帧大小 */

#define SPI_CTRL1(x)      		 (SPI_BASE(x) + HI_SPI_CTRLR1_OFFSET)
#define SPI_EN(x)         		 (SPI_BASE(x) + HI_SPI_SSIENR_OFFSET)
#define SPI_MWCTRL(x)     	 (SPI_BASE(x) + HI_SPI_MWCR_OFFSET  )
#define SPI_SLAVE_EN(x)   	 (SPI_BASE(x) + HI_SPI_SER_OFFSET   )
#define SPI_BAUD(x)       		 (SPI_BASE(x) + HI_SPI_BAUDR_OFFSET )
#define SPI_TXFTL(x)      		 (SPI_BASE(x) + HI_SPI_TXFTLR_OFFSET)
#define SPI_RXFTL(x)      		 (SPI_BASE(x) + HI_SPI_RXFTLR_OFFSET)
#define SPI_TXFL(x)       		 (SPI_BASE(x) + HI_SPI_TXFLR_OFFSET )
#define SPI_RXFL(x)       		 (SPI_BASE(x) + HI_SPI_RXFLR_OFFSET )
#define SPI_STATUS(x)     		 (SPI_BASE(x) + HI_SPI_SR_OFFSET    )
#define SPI_IMR(x)        		 (SPI_BASE(x) + HI_SPI_IMR_OFFSET   )
#define SPI_INT_STATUS(x) 		 (SPI_BASE(x) + HI_SPI_ISR_OFFSET   )
#define SPI_RX_SAMPLE_DLY(x)     (SPI_BASE(x) + HI_SPI_RX_SAMPLE_DLY_OFFSET)

#define SPI_STATUS_RXFULL         (1 << 4)
#define SPI_STATUS_RXNOTEMPTY     (1 << 3)
#define SPI_STATUS_TXEMPTY        (1 << 2)
#define SPI_STATUS_TXNOTFULL      (1 << 1)
#define SPI_STATUS_BUSY           (1 << 0)

#define SPI_RAW_INT_STATUS(x)  (SPI_BASE(x) + HI_SPI_RISR_OFFSET            )
#define SPI_TXO_INT_CLEAR(x)   (SPI_BASE(x) + HI_SPI_TXOICR_OFFSET          )
#define SPI_RXO_INT_CLEAR(x)   (SPI_BASE(x) + HI_SPI_RXOICR_OFFSET          )
#define SPI_RXU_INT_CLEAR(x)   (SPI_BASE(x) + HI_SPI_RXUICR_OFFSET          )
#define SPI_MST_INT_CLEAR(x)   (SPI_BASE(x) + HI_SPI_MSTICR_OFFSET          )
#define SPI_INT_CLEAR(x)       (SPI_BASE(x) + HI_SPI_ICR_OFFSET             )
#define SPI_DMAC(x)            (SPI_BASE(x) + HI_SPI_DMACR_OFFSET           )
#define SPI_DMATDL(x)          (SPI_BASE(x) + HI_SPI_DMATDLR_OFFSET         )
#define SPI_DMARDL(x)          (SPI_BASE(x) + HI_SPI_DMARDLR_OFFSET         )
#define SPI_ID(x)              (SPI_BASE(x) + HI_SPI_IDR_OFFSET             )
#define SPI_COMP_VERSION(x)    (SPI_BASE(x) + HI_SPI_SSI_COMP_VERSION_OFFSET)
#define SPI_DR(x)              (SPI_BASE(x) + HI_SPI_DR_OFFSET              )

/* for porting test */
/* Porting E2PROM使用SPI0 CS0 ; SFLASH使用SPI0 CS1*/
#define SPI_NO_DEV          0        /*SPI0*/
#define SPI_CS_EEPROM       (1<<0)   /* EEPROM使用SSI0 CS0 */
#define SPI_CS_SFLASH       (1<<1)   /* SFLASH使用SSI0 CS1 */
#define SPI_CS_DEV          SPI_CS_SFLASH/*硬件实际连接的设备*/
/* EEPROM和串行Flash通用命令 */
#define SPI_DEV_CMD_WRSR	0x1     /*Write Status Register*/
#define SPI_DEV_CMD_WRITE	0x2     /*Write Data to Memory Array*/
#define SPI_DEV_CMD_READ	0x3     /*Read Data from Memory Array*/
#define SPI_DEV_CMD_WRDI	0x4     /*Reset Write Enable Latch*/
#define SPI_DEV_CMD_RDSR	0x5     /*Read Status Register*/
#define SPI_DEV_CMD_WREN	0x6     /*Set Write Enable Latch*/
/* 串行Flash特有命令 */
#define SPI_DEV_CMD_RDID	0x9F    /* Read Identification */
#define SPI_DEV_CMD_PP		0x02    /* 页编程 */
#define SPI_DEV_CMD_SE		0xD8    /* 块擦除 */
#define SPI_DEV_CMD_BE		0xC7    /* Bulk Erase */
/* 状态寄存器状态位 */
#define SPI_DEV_STATUS_WIP  (1<<0)  /* Write In Progress Bit */
#define SPI_DEV_STATUS_WEL  (1<<1)  /* Write Enable Latch Bit */
#define SPI_DEV_STATUS_BP0  (1<<2)  /* Block Protect Bits */
#define SPI_DEV_STATUS_BP1  (1<<3)  /* Block Protect Bits */
#define SPI_DEV_STATUS_BP2  (1<<4)  /* Block Protect Bits */
#define SPI_DEV_STATUS_SRWD  (1<<7)  /* Status Register Write Protect */

/*for ASIC*/
/* PMU使用SPI0 CS0 */
#define SPI_NO_PMU          	0
#define SPI_CS_PMU          	(1<<0)     /* CS0,for hi6451 */

/* LCD使用SPI1 CS0 */
#define SPI_NO_LCD          	1
#define SPI_CS_LCD          	(1<<0)     /* CS0,for LCD */

#define SPI_MAX_DELAY_TIMES 0x10000	/*最大延迟时间*/

/*错误码*/
#define OK        		0
#define ERROR          -1

typedef enum eSpiDevType_t
{
    E_SPI_DEV_TYPE_EEPROM = 0,
    E_SPI_DEV_TYPE_SFLASH
}eSpiDevType;

/*函数声明*/
s32 spi_init (u32 spiNo);
s32 spi_recv (u32 spiNo, u32 cs, u16* prevData, u32 recvSize,u16* psendData,u32 sendSize );
s32 spi_send (u32 spiNo, u32 cs, u16* pData, u32 ulLen);
s32 spi_dev_detect( eSpiDevType *devType );
s32 spi_test(void);
#endif
