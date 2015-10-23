#ifndef __EMMC_IP_H__
#define __EMMC_IP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#include "emmcConfig.h"

#pragma pack(4)

/*****************************************************************************
  ºê¶¨Òå
*****************************************************************************/

/* SD HOST0 for WIFI */
#define SD_HOST0_REGBASE_ADR                 0x900AD000
#define SD_HOST0_REG_SIZE                     (4*0x400)

/* SD HOST1 for MMC/SD/eMMC */
#define SD_HOST1_REGBASE_ADR            0x900AE000
#define SD_HOST1_REG_SIZE                (4*0x400)

/* SD HOST2 for WIFI2 */
#define SD_HOST2_REGBASE_ADR            0x900AF000
#define SD_HOST2_REG_SIZE               (4*0x400)

/*SD/MMC*/
#define SD_DEV_REGBASE_ADR              0x900B0000
#define SD_DEV_REG_SIZE                 (4*0x400)

#if PLATFORM==PLATFORM_PORTING
#define EMMC_REG_BASE                       (SD_HOST1_REGBASE_ADR)  // eMMC
#else
#define EMMC_REG_BASE                       (SD_HOST0_REGBASE_ADR)  //eMMC/MMC/SD
#endif

#define EMMC_REG_CTRL                       (EMMC_REG_BASE + 0x00)
#define EMMC_REG_PWREN                      (EMMC_REG_BASE + 0x04)
#define EMMC_REG_CLKDIV                     (EMMC_REG_BASE + 0x08)
#define EMMC_REG_CLKSRC                     (EMMC_REG_BASE + 0x0C)
#define EMMC_REG_CLKENA                     (EMMC_REG_BASE + 0x10)
#define EMMC_REG_TMOUT                      (EMMC_REG_BASE + 0x14)
#define EMMC_REG_CTYPE                      (EMMC_REG_BASE + 0x18)
#define EMMC_REG_BLKSIZ                     (EMMC_REG_BASE + 0x1C)
#define EMMC_REG_BYTCNT                     (EMMC_REG_BASE + 0x20)
#define EMMC_REG_INTMASK                    (EMMC_REG_BASE + 0x24)
#define EMMC_REG_CMDARG                     (EMMC_REG_BASE + 0x28)
#define EMMC_REG_CMD                        (EMMC_REG_BASE + 0x2C)
#define EMMC_REG_RESP0                      (EMMC_REG_BASE + 0x30)
#define EMMC_REG_RESP1                      (EMMC_REG_BASE + 0x34)
#define EMMC_REG_RESP2                      (EMMC_REG_BASE + 0x38)
#define EMMC_REG_RESP3                      (EMMC_REG_BASE + 0x3C)
#define EMMC_REG_MINTSTS                    (EMMC_REG_BASE + 0x40)
#define EMMC_REG_RINTSTS                    (EMMC_REG_BASE + 0x44)
#define EMMC_REG_STATUS                     (EMMC_REG_BASE + 0x48)
#define EMMC_REG_FIFOTH                     (EMMC_REG_BASE + 0x4C)
#define EMMC_REG_CDETECT                    (EMMC_REG_BASE + 0x50)
#define EMMC_REG_WRIPRT                     (EMMC_REG_BASE + 0x54)
#define EMMC_REG_GPIO                       (EMMC_REG_BASE + 0x58)
#define EMMC_REG_TCBCNT                     (EMMC_REG_BASE + 0x5C)
#define EMMC_REG_TBBCNT                     (EMMC_REG_BASE + 0x60)
#define EMMC_REG_DEBNCE                     (EMMC_REG_BASE + 0x64)
#define EMMC_REG_USRID                      (EMMC_REG_BASE + 0x68)
#define EMMC_REG_VERID                      (EMMC_REG_BASE + 0x6C)
#define EMMC_REG_HCON                       (EMMC_REG_BASE + 0x70)
#define EMMC_REG_BMOD                       (EMMC_REG_BASE + 0x80)
#define EMMC_REG_PLDMND                     (EMMC_REG_BASE + 0x84)
#define EMMC_REG_DBADDR                     (EMMC_REG_BASE + 0x88)
#define EMMC_REG_IDSTS                      (EMMC_REG_BASE + 0x8C)
#define EMMC_REG_IDINTEN                    (EMMC_REG_BASE + 0x90)
#define EMMC_REG_DSCADDR                    (EMMC_REG_BASE + 0x94)
#define EMMC_REG_BUFADDR                    (EMMC_REG_BASE + 0x98)
#define EMMC_REG_FIFO                       (EMMC_REG_BASE + 0x200)

#define DEFAULT_DEBNCE_VAL                  (0xFFFFF)
#define MAX_DIVIDER_VALUE	                (0xff)

/* Bit 15,11,10,9,7 */
#define EMMC_INT_BIT_DATA_OR_FIFO_ERROR     (0x8E80)

/*
    Get the fifo count
*/
#define EMMC_FIFO_DEPTH_64                  64                     /* Totol size is 64 * 32 bit */
#define EMMC_FIFO_DEPTH_128                 128                    /* Totol size is 128 * 32 bit */
#define EMMC_FIFO_DEPTH_DEFAULT             EMMC_FIFO_DEPTH_64
#define EMMC_FIFO_WIDTH                     32
#define EMMC_GET_FIFO_COUNT( _status )      (((_status) >> 17) & 0x1FFF)
#define EMMC_GET_TX_WMARK(_fifoth_)         ((_fifoth_)&0xfff)
#define EMMC_GET_RX_WMARK(_fifoth_)         (((_fifoth_)&0xfff0000)>>16)

/*
    The bit define of command register
*/
#define EMMC_CMD_BIT_START                  (1UL << 31)
#define EMMC_CMD_BIT_BOOT_MODE              (1UL << 27)
#define EMMC_CMD_BIT_DISABLE_BOOT           (1UL << 26 )
#define EMMC_CMD_BIT_EXPECT_BOOT_ACK        (1UL << 25 )
#define EMMC_CMD_BIT_ENABLE_BOOT            (1UL << 24)
#define EMMC_CMD_BIT_CCS_EXPECTED           (1UL << 23)
#define EMMC_CMD_BIT_READ_CEATA_DEVICE      (1UL << 22)
#define EMMC_CMD_BIT_UPDATE_CLK_ONLY        (1UL << 21)
#define EMMC_CMD_BIT_SEND_INIT              (1UL << 15)
#define EMMC_CMD_BIT_STOP_ABORT             (1UL << 14)
#define EMMC_CMD_BIT_WAIT_PRV_COMPLETE      (1UL << 13)
#define EMMC_CMD_BIT_SEND_AUTO_STOP         (1UL << 12)
#define EMMC_CMD_BIT_TRANSFER_MODE          (1UL << 11)
#define EMMC_CMD_BIT_WRITE                  (1UL << 10)
#define EMMC_CMD_BIT_DATA_EXPECTED          (1UL << 9)
#define EMMC_CMD_BIT_CHECK_RESP_CRC         (1UL << 8)
#define EMMC_CMD_BIT_RESP_LENGTH            (1UL << 7)
#define EMMC_CMD_BIT_RESP_EXPECTED          (1UL << 6)

#define CMD_DONE_BIT	                    0x80000000

/*
    The bit define of ctrl register
*/
#define EMMC_CTRL_BIT_IDMAC                 (1UL << 25)
#define EMMC_CTRL_BIT_OD                    (1UL << 24)
#define EMMC_CTRL_BIT_DMAENABLE             (1UL << 5)
#define EMMC_CTRL_BIT_INTENABLE             (1UL << 4)
#define EMMC_CTRL_BIT_DMA_RESET             (1UL << 2)
#define EMMC_CTRL_BIT_FIFO_RESET            (1UL << 1)
#define EMMC_CTRL_BIT_CTRL_RESET            (1UL << 0)

/*
    The bit define of int status
*/
#define EMMC_INT_BIT_EBE                    (1UL << 15)
#define EMMC_INT_BIT_ACD                    (1UL << 14)
#define EMMC_INT_BIT_SBE                    (1UL << 13)
#define EMMC_INT_BIT_HLE                    (1UL << 12)
#define EMMC_INT_BIT_FRUN                   (1UL << 11)
#define EMMC_INT_BIT_HTO                    (1UL << 10)
#define EMMC_INT_BIT_DRTO                   (1UL << 9)
#define EMMC_INT_BIT_BDS                    (1UL << 9)
#define EMMC_INT_BIT_RTO                    (1UL << 8)
#define EMMC_INT_BIT_BAR                    (1UL << 8)
#define EMMC_INT_BIT_DCRC                   (1UL << 7)
#define EMMC_INT_BIT_RCRC                   (1UL << 6)
#define EMMC_INT_BIT_RXDR                   (1UL << 5)
#define EMMC_INT_BIT_TXDR                   (1UL << 4)
#define EMMC_INT_BIT_DTO                    (1UL << 3)
#define EMMC_INT_BIT_CD                     (1UL << 2)
#define EMMC_INT_BIT_RE                     (1UL << 1)
#define EMMC_INT_BIT_CDETECT                (1UL << 0)

/*
    Bus Mode Register Bit Definitions
*/
#define BMOD_SWR                             0x00000001	// Software Reset: Auto cleared after one clock cycle                                0
#define BMOD_FB                             0x00000002	// Fixed Burst Length: when set SINGLE/INCR/INCR4/INCR8/INCR16 used at the start     1
#define BMOD_DE                             0x00000080	// Idmac Enable: When set IDMAC is enabled                                           7
#define	BMOD_DSL_MSK	                    0x0000007C	// Descriptor Skip length: In Number of Words                                    6:2
#define	BMOD_DSL_Shift	                    2	        // Descriptor Skip length Shift value
#define	BMOD_DSL_ZERO                       0x00000000	// No Gap between Descriptors
#define	BMOD_DSL_TWO                        0x00000008	// 2 Words Gap between Descriptors
#define BMOD_PBL	                        0x00000400	// MSIZE in FIFOTH Register
/* Internal DMAC Status Register IDSTS Bit Definitions
 * Internal DMAC Interrupt Enable Register Bit Definitions */
#define EMMC_IDMAC_AI                       0x00000200   // Abnormal Interrupt Summary Enable/Status                                       9
#define EMMC_IDMAC_NI                           0x00000100   // Normal Interrupt Summary Enable/Status                                         8
#define EMMC_IDMAC_CES	                    0x00000020   // Card Error Summary Interrupt Enable/ status                                     5
#define EMMC_IDMAC_DU	                    0x00000010   // Descriptor Unavailabe Interrupt Enable /Status                                  4
#define EMMC_IDMAC_FBE	                    0x00000004   // Fata Bus Error Enable/ Status                                                   2
#define EMMC_IDMAC_RI	                    0x00000002   // Rx Interrupt Enable/ Status                                                     1
#define EMMC_IDMAC_TI	                    0x00000001   // Tx Interrupt Enable/ Status                                                     0
#define EMMC_IDMAC_EN_INT_ALL               0x00000337   // Enables all interrupts
#define EMMC_IDMAC_HOST_ABORT_TX            0x00000400   // Host Abort received during Transmission                                     12:10
#define EMMC_IDMAC_HOST_ABORT_RX            0x00000800   // Host Abort received during Reception                                        12:10

/* IDMAC FSM States */
#define EMMC_IDMAC_DMA_IDLE                 0x00000000   // DMA is in IDLE state
#define EMMC_IDMAC_DMA_SUSPEND              0x00002000   // DMA is in SUSPEND state
#define EMMC_IDMAC_DESC_RD                  0x00004000   // DMA is in DESC READ or FETCH State
#define EMMC_IDMAC_DESC_CHK                 0x00006000   // DMA is checking the Descriptor for Correctness
#define EMMC_IDMAC_DMA_RD_REQ_WAIT          0x00008000   // DMA is in this state till dma_req is asserted (Read operation)
#define EMMC_IDMAC_DMA_WR_REQ_WAIT          0x0000A000   // DMA is in this state till dma_req is asserted (Write operation)
#define EMMC_IDMAC_DMA_RD                   0x0000C000   // DMA is in Read mode
#define EMMC_IDMAC_DMA_WR                   0x0000E000   // DMA is in Write mode
#define EMMC_IDMAC_DESC_CLOSE               0x00010000   // DMA is closing the Descriptor

#define FIFOTH_MSIZE_1	                    0x00000000   // Multiple Trans. Size is 1
#define FIFOTH_MSIZE_4	                    0x10000000   // Multiple Trans. Size is 4
#define FIFOTH_MSIZE_8                       0x20000000   // Multiple Trans. Size is 8
#define FIFOTH_MSIZE_16	                    0x30000000   // Multiple Trans. Size is 16
#define FIFOTH_MSIZE_32	                    0x40000000   // Multiple Trans. Size is 32
#define FIFOTH_MSIZE_64	                    0x50000000   // Multiple Trans. Size is 64
#define FIFOTH_MSIZE_128	                0x60000000   // Multiple Trans. Size is 128
#define FIFOTH_MSIZE_256	                0x70000000   // Multiple Trans. Size is 256

#pragma pack(0)

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of emmcIp.h */
