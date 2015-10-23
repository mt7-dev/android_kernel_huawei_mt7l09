#ifndef __EMMC_NORMAL_H__
#define __EMMC_NORMAL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "sys.h"
#include "emmcConfig.h"

#pragma pack(4)

/*****************************************************************************
  宏定义
*****************************************************************************/

#if PLATFORM==PLATFORM_PORTING
#define CIU_CLK 24000   /* In KHz. 24 MHz for Porting */
#else
#define CIU_CLK 15000   /* In KHz. 15 MHz */
#endif

#define SD_MAX_OPFREQUENCY  24000
#define MMC_MAX_OPFREQUENCY 20000

#define  CARD_OCR_RANGE  0x00300000	/* 3.2-3.4V bits 20,21 set. */
#define  SD_VOLT_RANGE   0x80ff8000	/* 2.7-3.6V bits 20,21 set. */
#define  MMC_33_34V      0x80ff8000	/* 2.7-3.6V bits 20,21 set. */

#define  SD_MMC_IRQ      5	/* IRQ number for core. */

#define MMC_FREQDIVIDER    (CIU_CLK/(MMC_MAX_OPFREQUENCY))
#define SD_FREQDIVIDER     (CIU_CLK/(SD_MAX_OPFREQUENCY*2))

#define DIVIDER_FOR_FREQ(x)    ((CIU_CLK)/((x)*2))

#define MAX_THRESHOLD  0x0fff
#define MIN_THRESHOLD  100

#define FOD_VALUE           400	/* 400 KHz */
#define SD_FOD_VALUE        200	/* 200 KHz */
#define FOD_VALUE_KHZ       125 /* 300 KHz */
#define MMC_FOD_VALUE       200	/* 200 KHz */
#define DEFAULT_FOD_VALUE   400 /* 200 KHz */

#define MMC_FOD_DIVIDER_VALUE       ((CIU_CLK/(MMC_FOD_VALUE*2))+1)
#define SD_FOD_DIVIDER_VALUE	    ((CIU_CLK/(SD_FOD_VALUE*2))+1)
#define DEFAULT_FOD_DIVIDER_VALUE   ((CIU_CLK/(DEFAULT_FOD_VALUE*2))+1)
#define ONE_BIT_BUS_FREQ	        ((CIU_CLK/(12500*2))+1)
#define SD_ONE_BIT_BUS_FREQ         ONE_BIT_BUS_FREQ
#define HIGH_SPEED_FREQ	            DIVIDER_FOR_FREQ((CIU_CLK/2))
#define SD_HIGH_SPEED_FREQ	        DIVIDER_FOR_FREQ((CIU_CLK/2))
#define FOD_DIVIDER_VALUE	        ((CIU_CLK/(FOD_VALUE*2))+1)


/* Operation Conditions Register (OCR) Register Definition */
#define OCR_POWER_UP_STATUS	           0x80000000
#define OCR_ACCESSMODE_SECTOR          0x40000000	/* This is to indicate the secor addressing for MMC4.2 High capacity MMC cards */
#define OCR_RESERVED_1	               0x7f000000
#define OCR_27TO36	                   0x00ff8000
#define OCR_20TO26	                   0x00007f00
#define OCR_165TO195	               0x00000010
#define OCR_RESERVED_2	               0x0000007f
#define MMC_MOBILE_VOLTAGE	           OCR_165TO195
#define OCR_CCS	                       0x40000000	/* This is sent by card to indicate it is high capcity SD card*/
#define OCR_HCS	                       OCR_CCS	    /* This is sent by host querying whether card is high capacity?*/
#define OCR_FB                         0x20000000   /* Fast Boot bit reserved for eSD */
#define OCR_XPC                        0x10000000   /* OCR_XPC used to check on SDXC Power Control. If 0 => Power */

/* CSD field definitions */
#define CSD_STRUCT_VER_1_0              0       /* Valid for system specification 1.0 - 1.2 */
#define CSD_STRUCT_VER_1_1              1       /* Valid for system specification 1.4 - 2.2 */
#define CSD_STRUCT_VER_1_2              2       /* Valid for system specification 3.1       */

#define CSD_SPEC_VER_0                  0           /* Implements system specification 1.0 - 1.2 */
#define CSD_SPEC_VER_1                  1           /* Implements system specification 1.4 */
#define CSD_SPEC_VER_2                  2           /* Implements system specification 2.0 - 2.2 */
#define CSD_SPEC_VER_3                  3           /* Implements system specification 3.1 - 3.3 - 3.3.1 */
#define CSD_SPEC_VER_4                  4           /* Implements system specification 4.1 - 4.2 - 4.3 */

/* Retry counts */
#define CMD0_RETRY_COUNT                50          /* changed from 50 Just to save time--wzs */
#define CMD1_RETRY_COUNT                50          /* changed from 50 Just to save time--wzs */
#define ACMD41_RETRY_COUNT              50          /* changed from 50 Just to save time--wzs */
#define CMD2_RETRY_COUNT                50          /* changed from 50 Just to save time--wzs */
#define CMD3_RETRY_COUNT                50          /* changed from 50 Just to save time--wzs */
#define CMD5_RETRY_COUNT                50          /* changed from 50 Just to save time--wzs */

/* Standard MMC commands (3.1)                  type    argument    response */
/* class 1 */
#define	CMD0        0   /* MMC_GO_IDLE_STATE        bc                      */
#define CMD1        1   /* MMC_SEND_OP_COND         bcr     [31:0]  OCR R3  */
#define CMD2        2   /* MMC_ALL_SEND_CID         bcr                 R2  */
#define CMD3        3   /* MMC_SET_RELATIVE_ADDR    ac      [31:16] RCA R1  */
#define CMD4        4   /* MMC_SET_DSR              bc      [31:16] RCA     */
#define CMD5        5   /* SDIO_SEND_OCR            ??   ??                 */
#define CMD6        6   /* HSMMC_SWITCH             ac                  R1  */
                    /* For ACMD6:SET_BUS_WIDTH  ??   ??                 */
#define CMD7        7   /* MMC_SELECT_CARD          ac   [31:16] RCA    R1  */
#define CMD8        8   /* HSMMC_SEND_EXT_CSD       adtc [31:16] RCA    R1  */
#define EMMC_CMD8   8   /* HSMMC_SEND_EXT_CSD       adtc [31:16] RCA    R1  */
#define CMD9        9   /* MMC_SEND_CSD             ac   [31:16] RCA    R2  */
#define CMD10       10  /* MMC_SEND_CID             ac   [31:16] RCA    R2  */
#define CMD11       11  /* MMC_READ_DAT_UNTIL_STOP  adtc [31:0]  dadr   R1  */
#define CMD12       12  /* MMC_STOP_TRANSMISSION    ac                  R1b */
#define CMD13         13  /* MMC_SEND_STATUS          ac   [31:16] RCA    R1  */
#define ACMD13         13  /* SD_STATUS                ac   [31:2] Stuff,
                                                     [1:0]Buswidth  R1  */
#define CMD14       14  /* HSMMC_BUS_TESTING        adtc [31:16] stuff  R1  */
#define CMD15       15  /* MMC_GO_INACTIVE_STATE    ac   [31:16] RCA        */
#define CMD19       19  /* HSMMC_BUS_TESTING        adtc [31:16] stuff  R1  */

/* class 2 */
#define CMD16       16  /* MMC_SET_BLOCKLEN         ac   [31:0] blkln   R1  */
#define CMD17       17  /* MMC_READ_SINGLE_BLOCK    adtc [31:0] dtadd   R1  */
#define CMD18       18  /* MMC_READ_MULTIPLE_BLOCK  adtc [31:0] dtadd   R1  */

/* class 3 */
#define CMD20       20  /* MMC_WRITE_DAT_UNTIL_STOP adtc [31:0] dtadd   R1  */

/* class 4 */
#define CMD23   23  /* MMC_SET_BLOCK_COUNT      adtc [31:0] dtadd   R1  */
#define CMD24   24  /* MMC_WRITE_BLOCK          adtc [31:0] dtadd   R1  */
#define CMD25   25  /* MMC_WRITE_MULTIPLE_BLOCK adtc                R1  */
#define CMD26   26  /* MMC_PROGRAM_CID          adtc                R1  */
#define CMD27   27  /* MMC_PROGRAM_CSD          adtc                R1  */

/* class 6 */
#define CMD28   28  /* MMC_SET_WRITE_PROT       ac   [31:0] dtadd   R1b */
#define CMD29   29  /* _CLR_WRITE_PROT          ac   [31:0] dtadd   R1b */
#define CMD30   30  /* MMC_SEND_WRITE_PROT      adtc [31:0] wpdtaddrR1  */

/* class 5 */
#define CMD32   32  /* SD_ERASE_GROUP_START    ac   [31:0] dtadd  R1  */
#define CMD33   33  /* SD_ERASE_GROUP_END      ac   [31:0] dtaddr R1  */
#define CMD35   35  /* MMC_ERASE_GROUP_START    ac   [31:0] dtadd  R1  */
#define CMD36   36  /* MMC_ERASE_GROUP_END      ac   [31:0] dtaddr R1  */
#define CMD38   38  /* MMC_ERASE                ac                 R1b */

/* class 9 */
#define CMD39   39  /* MMC_FAST_IO              ac   <Complex>     R4  */
#define CMD40   40  /* MMC_GO_IRQ_STATE         bcr                R5  */
#define ACMD41  41  /* SD_SEND_OP_COND          ??                 R1  */

/* class 7 */
#define CMD42   42  /* MMC_LOCK_UNLOCK          adtc               R1b */
#define ACMD51  51  /* SEND_SCR                 adtc               R1  */
#define CMD52   52  /* SDIO_RW_DIRECT           ??                 R5  */
#define CMD53   53  /* SDIO_RW_EXTENDED         ??                 R5  */

/* class 8 */
#define CMD55   55  /* MMC_APP_CMD              ac   [31:16] RCA   R1  */
#define CMD56   56  /* MMC_GEN_CMD              adtc [0] RD/WR     R1b */
// For CE-ATA Drive
#define CMD60	60
#define CMD61	61

#define SDIO_RESET  100  //To differentiate CMD52 for IORESET and other rd/wrs.
#define SDIO_ABORT  101  //To differentiate CMD52 for IO ABORT and other rd/wrs.

#define UNADD_OFFSET	200
#define UNADD_CMD7	    207
#define	WCMC52	        252
#define WCMD53	        253
#define	WCMD60	        260
#define WCMD61	        261
#define ACMD6	        206
#define SD_CMD8	        208  /*This is added to support SD 2.0 (SDHC) cards*/
#define SD_CMD11     	211  /*This is added to support SDXC Voltage Switching*/

/*
    Error bits in response R1,which are set during execution
    of data command. Bits 19,20,21,22,23,24,26,27,29,30,31.
*/
#define R1_EXECUTION_ERR_BITS 0xEDF80000

/*
    MMC status in R1
    Type
        e : error bit
        s : status bit
        r : detected and set for the actual command response
        x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
    Clear condition
        a : according to the card state
        b : always related to the previous command. Reception of
        a valid command will clear it (with a delay of one command)
        c : clear by read
 */
#define R1_OUT_OF_RANGE             ((UINT32)1 << 31)       /* er, c */
#define R1_ADDRESS_ERROR            (1 << 30)               /* erx, c */
#define R1_BLOCK_LEN_ERROR          (1 << 29)               /* er, c */
#define R1_ERASE_SEQ_ERROR          (1 << 28)               /* er, c */
#define R1_ERASE_PARAM              (1 << 27)               /* ex, c */
#define R1_WP_VIOLATION             (1 << 26)               /* erx, c */
#define R1_CARD_IS_LOCKED           (1 << 25)               /* sx, a */
#define R1_LOCK_UNLOCK_FAILED       (1 << 24)               /* erx, c */
#define R1_COM_CRC_ERROR            (1 << 23)               /* er, b */
#define R1_ILLEGAL_COMMAND          (1 << 22)               /* er, b */
#define R1_CARD_ECC_FAILED          (1 << 21)               /* ex, c */
#define R1_CC_ERROR                 (1 << 20)               /* erx, c */
#define R1_ERROR                    (1 << 19)               /* erx, c */
#define R1_UNDERRUN                 (1 << 18)               /* ex, c */
#define R1_OVERRUN                  (1 << 17)               /* ex, c */
#define R1_CID_CSD_OVERWRITE        (1 << 16)               /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP            (1 << 15)               /* sx, c */
#define R1_CARD_ECC_DISABLED        (1 << 14)               /* sx, a */
#define R1_ERASE_RESET              (1 << 13)               /* sr, c */
#define R1_STATUS(x)                (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)         ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA           (1 << 8)                /* sx, a */
#define R1_SWITCH_ERROR             (1 << 7)                /* sr, c */
#define R1_APP_CMD                  (1 << 5)                /* sr, c */

/*
   Get state from card status
*/
#define EMMC_R1_CURRENT_STATE( _status )        (((_status) >>9) & 0xF)
#define EMMC_DONOT_COMPARE_STATE                0xFF
#define EMMC_CARD_STATE_IDENT                   2       /* The card is in iden state */
#define EMMC_CARD_STATE_TRAN                    4       /* The card is in tran state */

/*
    Bits 19,20,21,22,23,24,26,27,29,30,31.
*/
#define EMMC_STATUS_BIT_R1_EXECUTION_ERR_BITS   0xEDF80000

#define EMMC_EXT_CSD_BUS_WIDTH              183
#define EMMC_EXT_CSD_BOOTCONFIG             179
#define EMMC_EXT_CSD_BOOT_BUS_WIDTH         177

/*
 * for boot mode
 */
#define EXTCSD_ACCESS_CMDSET    0
#define EXTCSD_ACCESS_SETBITS   1
#define EXTCSD_ACCESS_CLRBITS   2
#define EXTCSD_ACCESS_WRBYTE    3

#define ECSD_BOOT_CONFIG(x)                 (x)[179]
#define EXTCSD_BOOT_ACK                     0x00000040   //If 1 Boot ack will be sent while booting, else boot ack will not be sent
#define EXTCSD_BOOT_PARTITION_ENABLE_MSK       0x00000038   // Mask for BOOT_PARTION_ENABLE in BOOT_CONFIG
#define EXTCSD_BOOT_PARTITION_1                0x00000008   //Boot Partition 1 Selected/Enabled
#define EXTCSD_BOOT_PARTITION_2                0x00000010   //Boot Partition 2 Selected/Enabled
#define EXTCSD_BOOT_PARTITION_USER             0x00000038   //User area selected for booting
#define EXTCSD_BOOT_PARTITION_ACCESS_MAK       0x00000007   //Read Write access mask
#define EXTCSD_BOOT_PARTITION_ACCESS_1_RW      0x00000001   //read write access to boot partition #1
#define EXTCSD_BOOT_PARTITION_ACCESS_2_RW      0x00000002   //read write access to boot partition #2

#define ECSD_BOOT_BUS_WIDTH(x)          (x)[177]
#define EXTCSD_BOOT_BUS_WIDTH_1		0x00000000
#define EXTCSD_BOOT_BUS_WIDTH_4		0x00000001
#define EXTCSD_BOOT_BUS_WIDTH_8		0x00000002
#define EXTCSD_BOOT_BUS_WIDTH_MASK  0x00000003
#define EXTCSD_BOOT_BUS_OTHER_MASK  0x000000fc
#define RESET_BOOT_BUS_WIDTH_1
#define ECSD_BOOT_SIZE_MULTI(x)         (x)[226]

#define ECSD_BOOT_INFO(x)               (x)[228]
#define EXTCSD_ALT_BOOT_MODE                   0x00000001 //If 1 Device/card supports alternate boot mode. If 0, doesnot supports alternate boot mode

#define EXTCSD_ARG_BOOTCONFIG_INDEX            0x00B30000  //179 is coded in 23:16 of CMD6 argument
#define EXTCSD_ARG_BOOTCONFIG_ACCESS_WRITE     0x03000000  //Write is coded in 25:24 of CMD6 argumnet


/*****************************************************************************
  UNION定义
*****************************************************************************/
/* 0x000 */
typedef union tagCONTROL_REG_U
{
    UINT32 reg;
    struct CtrlbitsTag
    {
        UINT32 controller_reset       :  1;/*lint !e46*/
        UINT32 fifo_reset             :  1;/*lint !e46*/
        UINT32 dma_reset              :  1;/*lint !e46*/
        UINT32 auto_clear_int         :  1;/*lint !e46*/
        UINT32 int_enable             :  1;/*lint !e46*/
        UINT32 dma_enable             :  1;/*lint !e46*/
        UINT32 read_wait              :  1;/*lint !e46*/
        UINT32 send_irq_response      :  1;/*lint !e46*/
        UINT32 abort_read_data        :  1;/*lint !e46*/
        UINT32 Reserved1              :  7;/*lint !e46*/
        UINT32 Card_voltage_a         :  4;/*lint !e46*/
        UINT32 Card_voltage_b         :  4;/*lint !e46*/
        UINT32 enable_OD_pullup       :  1;/*lint !e46*/
        UINT32 use_internal_dmac      :  1;/*lint !e46*/
        UINT32 Reserved2              :  6;/*lint !e46*/
    } Bits;   /* struct */
} CONTROL_REG_U;

/* 0x02C */
typedef union tagCOMMAND_REG_U
{
    UINT32 reg;
    struct CmdbitsTag
    {
        UINT32 cmd_index                :6;/*lint !e46*/
        UINT32 response_expect          :1;/*lint !e46*/
        UINT32 response_length          :1;/*lint !e46*/
        UINT32 check_response_crc       :1;/*lint !e46*/
        UINT32 data_expected            :1;/*lint !e46*/
        UINT32 read_write               :1;/*lint !e46*/
        UINT32 transfer_mode            :1;/*lint !e46*/
        UINT32 auto_stop                :1;/*lint !e46*/
        UINT32 wait_prvdata_complete    :1;/*lint !e46*/
        UINT32 stop_abort_cmd           :1;/*lint !e46*/
        UINT32 send_initialization      :1;/*lint !e46*/
        UINT32 card_number              :5;/*lint !e46*/
        UINT32 Update_clk_regs_only     :1;/*lint !e46*/
        UINT32 read_ceata_device        :1;/*lint !e46*/
        UINT32 ccs_expected             :1;/*lint !e46*/
        UINT32 enable_boot              :1;/*lint !e46*/
        UINT32 expect_boot_ack          :1;/*lint !e46*/
        UINT32 disable_boot             :1;/*lint !e46*/
        UINT32 boot_mode                :1;/*lint !e46*/
        UINT32 volt_switch              :1;/*lint !e46*/
        UINT32 use_hold_reg             :1;/*lint !e46*/
        UINT32 Reserved                 :1;/*lint !e46*/
        UINT32 start_cmd                :1;/*lint !e46*/
    } Bits;
} COMMAND_REG_U;

typedef union tagCSD_REG_U
{
    UINT8 cArr[512];
    struct
    {
        UINT8 csd_structure;
        UINT8 spec_vers;
        UINT8 taac;
        UINT8 nsac;
        UINT8 tran_speed;
        UINT16 ccc;
        UINT8 read_bl_len;
        UINT8 read_bl_partial;
        UINT8 write_blk_misalign;
        UINT8 read_blk_misalign;
        UINT8 dsr_imp;
        UINT32 c_size;
        UINT8 vdd_r_curr_min;
        UINT8 vdd_r_curr_max;
        UINT8 vdd_w_curr_min;
        UINT8 vdd_w_curr_max;
        UINT8 c_size_mult;
        union
        {
            struct
            {
                /* MMC system specification version 3.1 */
                UINT8 erase_grp_size;
                UINT8 erase_grp_mult;
            } v31;
            struct
            {
                /* MMC system specification version 2.2 */
                UINT8 sector_size; //sector_size
                UINT8 erase_grp_size; //erase_blk_en
            } v22;
        } erase;
        UINT8 wp_grp_size;
        UINT8 wp_grp_enable;
        UINT8 default_ecc;
        UINT8 r2w_factor;
        UINT8 write_bl_len;
        UINT8 write_bl_partial;
        UINT8 file_format_grp;
        UINT8 copy;
        UINT8 perm_write_protect;
        UINT8 tmp_write_protect;
        UINT8 file_format;
        UINT8 ecc;
    } Fields;
}CSD_REG_U;

typedef union tagCID_REG_U
{
    UINT8 cCid[16];
    struct
    {
        UINT8 mid;
        UINT8 cbx;
        UINT8 oid;
        UINT8 pnm[6];
        UINT8 prv;
        UINT32 psn;
        UINT8 mdt;
        UINT8 crc;
    }mmc;
    struct
    {
        UINT8 mid;
        UINT16 oid;
        UINT8 pnm[5];
        UINT8 prv;
        UINT32 psn;
        UINT16 mdt;
        UINT8 crc;
    }sd;
}CID_REG_U;

typedef struct tagCCCR_REG_S
{
    UINT8  CCCR_Revision;
    UINT8  Specs_Revision;
    UINT8  IO_Enable;
    UINT8  Int_Enable;
    UINT8  Bus_Interface;
    UINT8  Card_Capability;
    UINT32 Common_CIS_ptr;
    UINT8  Power_Control;
} CCCR_REG_S;

typedef struct tagID_REG_INFO_S
{
    UINT32 u32UserID;
    UINT32 u32VerID;
} ID_REG_INFO_S;

typedef struct
{
    UINT32 ulR1Resp;
}EMMC_R1_RESPONSE_STRU;

typedef struct
{
    UINT32 ulResp[ 128 / 32 ]; /* Total 128 Bits */
}EMMC_R2_RESPONSE_STRU;

typedef union tagR1_RESPONSE_U
{
    UINT32 ulStatusReg;
    struct
    {
        UINT32 Reserved1           :  2;/*lint !e46*/
        UINT32 Reserved2           :  1;/*lint !e46*/
        UINT32 AKE_SEQ_ERR         :  1;/*lint !e46*/
        UINT32 Reserved3           :  1;/*lint !e46*/
        UINT32 APP_CMD             :  1;/*lint !e46*/
        UINT32 Unused              :  2;/*lint !e46*/
        UINT32 READY_FOR_DATA      :  1;/*lint !e46*/
        UINT32 CURRENT_SATTE       :  4;/*lint !e46*/
        UINT32 ERASE_RESET         :  1;/*lint !e46*/
        UINT32 CARD_ECC_DISABLED   :  1;/*lint !e46*/
        UINT32 WP_ERASE_SKIP       :  1;/*lint !e46*/
        UINT32 CID_CSD_OVERWRITE   :  1;/*lint !e46*/
        UINT32 OVERRUN             :  1;/*lint !e46*/
        UINT32 UNDERRUN            :  1;/*lint !e46*/
        UINT32 TODOERROR           :  1;/*lint !e46*/

        //        UINT32 ERROR               :  1; //bit19
        UINT32 CC_ERROR            :  1;/*lint !e46*/
        UINT32 CARD_ECC_FAILED     :  1;/*lint !e46*/
        UINT32 ILLEGAL_COMMAND     :  1;/*lint !e46*/
        UINT32 COM_CRC_ERROR       :  1;/*lint !e46*/
        UINT32 LOCK_UNLOCK_FAILED  :  1;/*lint !e46*/
        UINT32 CARD_IS_LOCKED      :  1;/*lint !e46*/
        UINT32 WP_VIOLATION        :  1;/*lint !e46*/
        UINT32 ERASE_PARAM         :  1;/*lint !e46*/
        UINT32 ERASE_SEQ_ERROR     :  1;/*lint !e46*/
        UINT32 BLOCK_LEN_ERROR     :  1;/*lint !e46*/
        UINT32 ADDRESS_ERROR       :  1;/*lint !e46*/
        UINT32 OUT_OF_RANGE        :  1;/*lint !e46*/
    } Bits;
} R1_RESPONSE_U;

typedef struct tagR2_RESPONSE_S
{
    UINT8 cResp[128 / 8];
} R2_RESPONSE_S;

typedef union tagR6_RESPONSE_U
{
    UINT32 reg;
    struct
    {
        UINT32 card_status  : 16;/*lint !e46*/
        UINT32 card_rca     : 16;/*lint !e46*/
    } Bits;
} R6_RESPONSE_U;

/*****************************************************************************
  ENUM定义
*****************************************************************************/
typedef enum tagCARD_TYPE_E
{
    CARD_TYPE_INVALID = 0,
    CARD_TYPE_MMC,
    CARD_TYPE_MMC_42,
    CARD_TYPE_SD,
    CARD_TYPE_SD2,
    CARD_TYPE_SD2_1,
    CARD_TYPE_MAX
} CARD_TYPE_E;

/* card state */
enum card_state
{
    CARD_STATE_EMPTY = -1,
    CARD_STATE_IDLE  = 0,
    CARD_STATE_READY = 1,
    CARD_STATE_IDENT = 2,
    CARD_STATE_STBY = 3,
    CARD_STATE_TRAN = 4,
    CARD_STATE_DATA = 5,
    CARD_STATE_RCV = 6,
    CARD_STATE_PRG = 7,
    CARD_STATE_DIS = 8,
    CARD_STATE_BOTTOM = 0xff
};

/* error codes */
enum mmc_result_t
{
    MMC_NO_RESPONSE = -1,
    MMC_NO_ERROR = 0,
    MMC_ERROR_OUT_OF_RANGE = 200,
    MMC_ERROR_ADDRESS,
    MMC_ERROR_BLOCK_LEN,
    MMC_ERROR_ERASE_SEQ,
    MMC_ERROR_ERASE_PARAM,
    MMC_ERROR_WP_VIOLATION,

    //MMC_ERROR_CARD_IS_LOCKED, /* Status bit.No need to check. */
    MMC_ERROR_LOCK_UNLOCK_FAILED,

    //MMC_ERROR_COM_CRC,        /* If required,user will read this by CMD13. */
    //MMC_ERROR_ILLEGAL_COMMAND,/* If required,user will read this by CMD13. */
    MMC_ERROR_CARD_ECC_FAILED,
    MMC_ERROR_CC,
    MMC_ERROR_GENERAL,
    MMC_ERROR_UNDERRUN,
    MMC_ERROR_OVERRUN,
    MMC_ERROR_CID_CSD_OVERWRITE,

    //MMC_ERROR_WP_ERASE_SKIP, //Status bit.Check during erase comamnd only.
    //MMC_ERROR_CARD_ECC_DISABLED,
    //MMC_ERROR_ERASE_RESET,   //Status bit.Check during erase comamnd only.
    MMC_ERROR_STATE_MISMATCH,
    MMC_ERROR_HEADER_MISMATCH,
    MMC_ERROR_TIMEOUT,
    MMC_ERROR_CRC,
    MMC_ERROR_DRIVER_FAILURE
};

/*****************************************************************************
  STRUCT定义
*****************************************************************************/
typedef struct tagCOMMAND_INFO_S
{
    COMMAND_REG_U   CmdReg;
    UINT32          CmdArg;
    UINT32          ByteCnt;
    UINT32          BlkSize;
} COMMAND_INFO_S;

typedef struct
{
    UINT32          ulResp0;
    UINT32          ulResp1;
    UINT32          ulResp2;
    UINT32          ulResp3;
}EMMC_RESPONSE_INFO_S;

typedef struct tagEMMC_CURRENT_CMD_INFO_S
{
    UINT8    *pucCurrDataBuff;
    UINT8    *pucCurrCmdRespBuff;
    UINT8    *pucCurrErrRespBuff;
    UINT32   ulCurrCmdOptions;
    UINT32   ulCurrErrorSts;
    UINT32   ulCurrCmdInProcess; /* 1=in process, 0=not in process or over. */
    UINT32   ulCurrErrorSet;
    UINT32   ulResponsetype;     /* !0 =long response(128bit) 0=short response(32bit). */
}EMMC_CURRENT_CMD_INFO_S;

typedef struct INPROCESS_DATA_CMD_Tag
{
    UINT8    *pucCurrDataBuff;
    UINT32   ulWrite;
    UINT32   ulByteCount;
    UINT32   ulRemainedBytes;
    UINT32   ulCurrCmdOptions;
    UINT32   ulCmdInProcess;     /* 1=> data command is in process.  */
    UINT32   ulDataErrStatus;      /* stores data errors.              */
}EMMC_INPROCESS_DATA_CMD_S;

typedef struct tagEMMC_IP_STATUS_INFO_S
{
    BOOL bUseHold;
    UINT32 max_tacc_value;
    UINT32 max_msac_value;
} EMMC_IP_STATUS_INFO_S;

typedef struct tagEMMC_CARD_INFO_S
{
    CARD_TYPE_E card_type;
    CSD_REG_U CSD;
    CID_REG_U CID;
    CCCR_REG_S CCCR;
    UINT64 card_size;
    UINT32 card_rca;
    UINT32 card_ocr;
    UINT32 card_read_blksize;
    UINT32 card_write_blksize;
    BOOL extcsd_bootop;
    UINT32 extcsd_seccount;
    UINT8 extcsd_bootinfo;
    UINT8 extcsd_bootsizemult;
    UINT8 extcsd_rev;
    UINT8 extcsd_partitioncfg;
    UINT8 extcsd_bootbuswidth;
} EMMC_CARD_INFO_S;

typedef struct
{
    UINT32 slot;
    EMMC_IP_STATUS_INFO_S       stIPStatusInfo;
    EMMC_CARD_INFO_S            stCardInfo[EMMC_CARD_NUM];
    EMMC_CURRENT_CMD_INFO_S     stCurrCmdInfo;
    EMMC_INPROCESS_DATA_CMD_S   stDatCmdInfo;
#ifdef EMMC_IDMAC_SUPPORT
    EMMC_IDMAC_HANDLE_S         stIdmacHandle;
#endif
}EMMC_GLOBAL_VAR_S;

extern VOID emmc_config_od(BOOL bEnable);
extern UINT32 emmc_reset_ip(VOID);
extern VOID emmc_global_variable_init(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, BOOL bUseHold);
extern UINT32 emmc_read_bootloader_normal(UINT32 slot, UINT32 dst, BOOL bUseHold);
extern UINT32 emmc_block_read(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulStartBlk, UINT8 *pucBuf, UINT32 ulLen);
#ifdef EMMC_DEBUG
extern UINT32 emmc_block_write(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulStartBlk, UINT8 *pucBuf, UINT32 ulLen);
#endif
extern UINT32 emmc_read_extcsd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT8 *pcExtCSD);
#pragma pack(0)

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of emmcNormal.h */
