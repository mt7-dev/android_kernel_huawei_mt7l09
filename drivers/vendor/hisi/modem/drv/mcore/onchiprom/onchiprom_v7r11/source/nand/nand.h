#ifndef __NAND_H__
#define __NAND_H__

#include "types.h"
#include "OcrShare.h"

/* other */
#define NAND_ID_SIZE		8
#define NANDC_COLUMN_SHIFT	16


/**
 * struct nand_flash_dev - NAND Flash Device ID Structure
 * @name: a human-readable name of the NAND chip
 * @dev_id: the device ID (the second byte of the full chip ID array)
 * @mfr_id: manufecturer ID part of the full chip ID array (refers the same
 *          memory address as @id[0])
 * @dev_id: device ID part of the full chip ID array (refers the same memory
 *          address as @id[1])
 * @id: full device ID array
 * @pagesize: size of the NAND page in bytes; if 0, then the real page size (as
 *            well as the eraseblock size) is determined from the extended NAND
 *            chip ID array)
 * @chipsize: total chip size in MiB
 * @erasesize: eraseblock size in bytes (determined from the extended ID if 0)
 * @options: stores various chip bit options
 * @id_len: The valid length of the @id.
 * @oobsize: OOB size
 *
 * ps: this structure is from Linux
 */
struct nand_flash_dev {
	INT8 *name;
	union {
		struct {
			UINT8 mfr_id;
			UINT8 dev_id;
		};
		UINT8 id[NAND_ID_SIZE];
	};
	UINT32 pagesize;
	UINT32 chipsize;
	UINT32 erasesize;
	UINT16 id_len;
	UINT16 oobsize;
};

/*
* 说明：这个是控制器和控制对象的关联表项定义，填充这个表以后，初始化程序会根据这表来搜索当前
*       控制器和控制对象所需要的控制器buffer数据分布参数。
*/
struct nandc_ecc_info
{
    UINT32 pagesize;                                   /*nand器件page大小*/
    UINT32 sparesize;                                  /*nand器件的spare区大小*/
    UINT32 ecctype;                                    /*上面两项参数对应的本控制器最佳ecc寄存器配置*/
};

#pragma pack(1)
struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	UINT8 sig[4];
	UINT16 revision;
	UINT16 features;
	UINT16 opt_cmd;
	UINT8 reserved[22];

	/* manufacturer information block */
	INT8 manufacturer[12];
	INT8 model[20];
	UINT8 jedec_id;	  		
	UINT16 date_code;
	UINT8 reserved2[13];    

	/* memory organization block */
	UINT32 byte_per_page;
	UINT16 spare_bytes_per_page;     
	UINT32 data_bytes_per_ppage;
	UINT16 spare_bytes_per_ppage;
	UINT32 pages_per_block;
	UINT32 blocks_per_lun;
	UINT8 lun_count;
	UINT8 addr_cycles;
	UINT8 bits_per_cell;
	UINT16 bb_per_lun;
	UINT16 block_endurance;
	UINT8 guaranteed_good_blocks;
	UINT16 guaranteed_block_endurance;
	UINT8 programs_per_page;
	UINT8 ppage_attr;
	UINT8 ecc_bits;
	UINT8 interleaved_bits;
	UINT8 interleaved_ops;
	UINT8 reserved3[13];

	/* electrical parameter block */
	UINT8 io_pin_capacitance_max;
	UINT16 async_timing_mode;
	UINT16 program_cache_timing_mode;
	UINT16 t_prog;
	UINT16 t_bers;
	UINT16 t_r;
	UINT16 t_ccs;
	UINT16 src_sync_timing_mode;
	UINT16 src_ssync_features;
	UINT16 clk_pin_capacitance_typ;
	UINT16 io_pin_capacitance_typ;
	UINT16 input_pin_capacitance_typ;
	UINT8 input_pin_capacitance_max;
	UINT8 driver_strenght_support;
	UINT16 t_int_r;
	UINT16 t_ald;
	UINT8 reserved4[7];

	/* vendor */
	UINT8 reserved5[90];

	UINT16 crc;
};
#pragma pack()

enum en_boot_mode
{
	ONLY_NAND_BOOT_MODE = 0,
	EFUSE_BOOT_MODE,
	NAND_PARSE_ID_MODE	
};

/*
 * A helper for defining newer chips which report their page size and
 * eraseblock size via the extended ID bytes.
 *
 * The real difference between LEGACY_ID_NAND and EXTENDED_ID_NAND is that with
 * EXTENDED_ID_NAND, manufacturers overloaded the same device ID so that the
 * device ID now only represented a particular total chip size (and voltage,
 * buswidth), and the page size, eraseblock size, and OOB size could vary while
 * using the same device ID.
 */
#define EXTENDED_ID_NAND(nm, devid, chipsz)      \
    { .name = (nm), {{ .dev_id = (devid) }}, .chipsize = (chipsz)   }

/* nandc相关的宏 */
#define NANDC_RESET_TIME_OUT		10000
#define NANDC_RD_RESULT_TIME_OUT 	100000
#define NANDC_RESULT_TIME_OUT		10000000	/* 十进制 */
#define NANDC_STATUS_OP_DONE        1
#define NANDC_ADDRBIT_OFFSET        9           /* addr cycle offset in op reg */
#define M3BOOT_POSITION_FLASH		0

#define NAND_ECC_DISABLE		0	
#define NAND_ECC_ENABLE			1

#define EFUSE_NANDINFO_GROUP_1		1
#define EFUSE_NANDINFO_GROUP_0		0

/* macro for nand spec */
#define NANDC_ADDRCYCLE_0			0
#define NANDC_ADDRCYCLE_1			1
#define NANDC_ADDRCYCLE_4			4
#define NANDC_ADDRCYCLE_5			5

#define NANDC_SIZE_HK			512
#define NANDC_SIZE_1K			1024
#define NANDC_SIZE_2K			2048
#define NANDC_SIZE_4K			4096
#define NANDC_SIZE_8K			8192
#define NANDC_SIZE_16K			(16*1024)
#define NANDC_SIZE_256K			(256*1024)
#define NANDC_SIZE_1M			(1024*1024)
#define NANDC_SIZE_2M			(2*1024*1024)

#define NANDC_CHIPSIZE_64M      64			/* chip size 以MByte为单位*/
#define NANDC_CHIPSIZE_128M     128
#define NANDC_CHIPSIZE_256M    256
#define NANDC_CHIPSIZE_512M    512
#define NANDC_CHIPSIZE_1G		1024
#define NANDC_CHIPSIZE_2G      	(2*1024)
#define NANDC_CHIPSIZE_4G      	(4*1024)
#define NANDC_CHIPSIZE_8G      	(8*1024)
#define NANDC_CHIPSIZE_16G      (16*1024)
#define NANDC_CHIPSIZE_32G      (32*1024)
#define NANDC_CHIPSIZE_64G      (64*1024)

#define NANDC_ECCTYPE_ECC_4BIT		4
#define NANDC_ECCTYPE_ECC_8BIT		8
#define NANDC_ECCTYPE_ECC_24BIT		24

#define NANDC_BLOCK_64PAGE  		64
#define NANDC_BLOCK_128PAGE 		128
#define NANDC_BLOCK_256PAGE  		256
#define NANDC_BLOCK_512PAGE 		512
#define NANDC_BLOCK_768PAGE         768

#define NANDC_SPARE_LENGTH_16BYTE	16
#define NANDC_SPARE_LENGTH_32BYTE	32
#define NANDC_SPARE_LENGTH_60BYTE   60
#define NANDC_SPARE_LENGTH_64BYTE	64
#define NANDC_SPARE_LENGTH_88BYTE	88
#define NANDC_SPARE_LENGTH_116BYTE  116
#define NANDC_SPARE_LENGTH_128BYTE	128
#define NANDC_SPARE_LENGTH_144BYTE	144
#define NANDC_SPARE_LENGTH_200BYTE	200
#define NANDC_SPARE_LENGTH_218BYTE	218
#define NANDC_SPARE_LENGTH_224BYTE	224
#define NANDC_SPARE_LENGTH_232BYTE  232
#define NANDC_SPARE_LENGTH_256BYTE	256
#define NANDC_SPARE_LENGTH_368BYTE	368
#define NANDC_SPARE_LENGTH_400BYTE	400
#define NANDC_SPARE_LENGTH_436BYTE	436
#define NANDC_SPARE_LENGTH_448BYTE	448
#define NANDC_SPARE_LENGTH_512BYTE	512
#define NANDC_SPARE_LENGTH_640BYTE	640

/* macro for nand reg bit value */
#define NANDC_PAGE_SIZE_2K_REG			1
#define NANDC_PAGE_SIZE_4K_REG			2
#define NANDC_PAGE_SIZE_8K_REG			3

#define NANDC_ECCTYPE_ECC_NONE_REG		0
#define NANDC_ECCTYPE_ECC_4BIT_REG		2
#define NANDC_ECCTYPE_ECC_8BIT_REG		3
#define NANDC_ECCTYPE_ECC_24BIT_REG		4

#define NANDC_ADDRCYCLE_4_BOOT_REG		0
#define NANDC_ADDRCYCLE_5_BOOT_REG		1

#define NANDC_ADDRCYCLE_4_OP_REG		4
#define NANDC_ADDRCYCLE_5_OP_REG		5

#define NANDC_PAGENUM_PER_BLOCK_64_REG  0
#define NANDC_PAGENUM_PER_BLOCK_128_REG 1
#define NANDC_PAGENUM_PER_BLOCK_256_REG 2
#define NANDC_PAGENUM_PER_BLOCK_512_REG 3

#define NANDC_INTS_CLEAR_ALL_REG        0x7FF
#define NANDC_INT_ENABLE_ALL_REG        0x7FF
#define NANDC_INT_DISABLE_ALL_REG       0

#define NANDC_ECC_SELECT_DISABLE_REG    0
#define NANDC_ECC_SELECT_ENABLE_REG     1

#define NANDC_BUSWIDTH_8BIT_REG         0
#define NANDC_CHIP_SELECT_0_REG         0
#define NANDC_OOB_LEN_32BYTE_REG        1
#define NANDC_PWIDTH_555_REG            0x555

#define NANDC_BOOT_MODE_REG             0
#define NANDC_NORMAL_MODE_REG           1

/* 寄存器不需要配置spare size和chip size，这里的宏用来保存specinfo到SRAM buffer，为了保持与其它参数名称上的一致，宏也以_REG结束
 * 由于这两个参数不一定每次都能解析出，所以默认值0不能作为有效值
 */
#define NANDC_SPARE_LENGTH_16BYTE_REG	 1	
#define NANDC_SPARE_LENGTH_32BYTE_REG	 2  
#define NANDC_SPARE_LENGTH_60BYTE_REG	 3	 
#define NANDC_SPARE_LENGTH_64BYTE_REG	 4	
#define NANDC_SPARE_LENGTH_88BYTE_REG	 5 
#define NANDC_SPARE_LENGTH_116BYTE_REG	 6	
#define NANDC_SPARE_LENGTH_128BYTE_REG	 7	
#define NANDC_SPARE_LENGTH_144BYTE_REG	 8	
#define NANDC_SPARE_LENGTH_200BYTE_REG	 9	
#define NANDC_SPARE_LENGTH_218BYTE_REG	 10
#define NANDC_SPARE_LENGTH_224BYTE_REG	 11	
#define NANDC_SPARE_LENGTH_232BYTE_REG	 12
#define NANDC_SPARE_LENGTH_256BYTE_REG	 13	
#define NANDC_SPARE_LENGTH_368BYTE_REG	 14	
#define NANDC_SPARE_LENGTH_400BYTE_REG	 15	
#define NANDC_SPARE_LENGTH_436BYTE_REG	 16	
#define NANDC_SPARE_LENGTH_448BYTE_REG	 17	
#define NANDC_SPARE_LENGTH_512BYTE_REG	 18	
#define NANDC_SPARE_LENGTH_640BYTE_REG	 19	

#define NANDC_CHIPSIZE_64M_REG			1	
#define NANDC_CHIPSIZE_128M_REG			2
#define NANDC_CHIPSIZE_256M_REG			3
#define NANDC_CHIPSIZE_512M_REG			4
#define NANDC_CHIPSIZE_1G_REG			5
#define NANDC_CHIPSIZE_2G_REG			6
#define NANDC_CHIPSIZE_4G_REG			7
#define NANDC_CHIPSIZE_8G_REG			8
#define NANDC_CHIPSIZE_16G_REG			9
#define NANDC_CHIPSIZE_32G_REG			10
#define NANDC_CHIPSIZE_64G_REG			11

/* 东芝nand flash加工工艺，用来确定SLC Nand的ecc type和Benand的spare size */
#define NAND_TOSHIBA_43NM				4
#define NAND_TOSHIBA_32NM				5
#define NAND_TOSHIBA_24NM				6
#define NAND_TOSHIBA_TECH_MASK			7

/* macro for efuse reg bit value for nandc */
#define NANDC_PAGE_SIZE_2K_EFUSE			1
#define NANDC_PAGE_SIZE_4K_EFUSE			3
#define NANDC_PAGE_SIZE_8K_EFUSE			2

#define NANDC_ECCTYPE_ECC_4BIT_EFUSE		1
#define NANDC_ECCTYPE_ECC_8BIT_EFUSE		3
#define NANDC_ECCTYPE_ECC_24BIT_EFUSE		2	

#define NANDC_ADDRCYCLE_4_EFUSE				0
#define NANDC_ADDRCYCLE_5_EFUSE				1

#define NANDC_PAGENUM_PER_BLOCK_64_EFUSE  	0
#define NANDC_PAGENUM_PER_BLOCK_128_EFUSE 	1

/* nand command */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_GET_FEATURES	0xee
#define NAND_CMD_SET_FEATURES	0xef
#define NAND_CMD_RESET		0xff

#define NAND_READ_ID_ONFI	0x20
#define NAND_READ_ID_EXT	0x00
#define NAND_CMD_READSTART	0x30

/* 以上命令是用来配置cmd的，下面这个命令式用来配置op的 */
#define NAND_OP_RESET       0x44
#define NAND_OP_READ_ID     0x62
#define NAND_OP_READ_PARAM  0x66

/* nandc buffer and reg */
#define NANDC_BUFFER_BASE_ADDR 		0xA0000000
#define NANDC_BUFFER_SIZE			(8192 + 1024)

/* Cell info constants */
#define NAND_CI_CELLTYPE_MSK	0x0C

/* ONFI */
#define ONFI_CRC_BASE	0x4F4E

/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec
#define NAND_MFR_FUJITSU	0x04
#define NAND_MFR_NATIONAL	0x8f
#define NAND_MFR_RENESAS	0x07
#define NAND_MFR_STMICRO	0x20
#define NAND_MFR_HYNIX		0xad
#define NAND_MFR_MICRON		0x2c
#define NAND_MFR_AMD		0x01
#define NAND_MFR_MACRONIX	0xc2
#define NAND_MFR_EON		0x92

/* Chip has cache program function */
#define NAND_CACHEPRG		0x00000008

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS NAND_CACHEPRG

/* the follow is nand ids, which is also from Linux */
#define LP_OPTIONS 				NAND_SAMSUNG_LP_OPTIONS
#define NAND_ONCHIP_ECC			0x80

int nandReadBl(UINT32 dest , enum en_boot_mode nand_boot_mode, volatile struct nand_spec_sram *spec_sram);
#endif
