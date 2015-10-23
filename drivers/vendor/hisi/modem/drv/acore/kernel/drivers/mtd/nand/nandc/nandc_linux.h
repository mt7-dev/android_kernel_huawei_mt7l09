
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_LINUX_H_
#define _NANDC_LINUX_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define MTD_MAX_OOBFREE_ENTRIES	        8
/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		            0
#define NAND_CMD_READ1		            1
#define NAND_CMD_RNDOUT		            5
#define NAND_CMD_PAGEPROG               0x10
#define NAND_CMD_READOOB                0x50
#define NAND_CMD_ERASE1		            0x60
#define NAND_CMD_STATUS		            0x70
#define NAND_CMD_STATUS_MULTI	        0x71
#define NAND_CMD_SEQIN		            0x80
#define NAND_CMD_RNDIN		            0x85
#define NAND_CMD_READID		            0x90
#define NAND_CMD_ERASE2		            0xd0
#define NAND_CMD_PARAM		            0xec
#define NAND_CMD_RESET		            0xff

#define NAND_CMD_LOCK		            0x2a
#define NAND_CMD_UNLOCK1	            0x23
#define NAND_CMD_UNLOCK2	            0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	            0x30
#define NAND_CMD_RNDOUTSTART	        0xE0
#define NAND_CMD_CACHEDPROG	            0x15

/* Extended commands for AG-AND device */
/*
 * Note: the command for NAND_CMD_DEPLETE1 is really 0x00 but
 *       there is no way to distinguish that from NAND_CMD_READ0
 *       until the remaining sequence of commands has been completed
 *       so add a high order bit and mask it off in the command.
 */
#define NAND_CMD_DEPLETE1	            0x100
#define NAND_CMD_DEPLETE2	            0x38
#define NAND_CMD_STATUS_MULTI	        0x71
#define NAND_CMD_STATUS_ERROR	        0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	        0x73
#define NAND_CMD_STATUS_ERROR1	        0x74
#define NAND_CMD_STATUS_ERROR2	        0x75
#define NAND_CMD_STATUS_ERROR3	        0x76
#define NAND_CMD_STATUS_RESET	        0x7f
#define NAND_CMD_STATUS_CLEAR	        0xff

#define NAND_CMD_NONE		            -1

/* Status bits */
#define NAND_STATUS_FAIL	            0x01
#define NAND_STATUS_FAIL_N1	            0x02
#define NAND_STATUS_TRUE_READY	        0x20
#define NAND_STATUS_READY	            0x40
#define NAND_STATUS_WP		            0x80


/* Option constants for bizarre disfunctionality and real
*  features
*/
/* Chip can not auto increment pages */
#define NAND_NO_AUTOINCR	            0x00000001

/* Buswitdh is 16 bit */
#define NAND_BUSWIDTH_16	            0x00000002

/* Device supports partial programming without padding */
#define NAND_NO_PADDING		            0x00000004

/* Chip has cache program function */
#define NAND_CACHEPRG		            0x00000008

/* Chip has copy back function */
#define NAND_COPYBACK		            0x00000010

/* AND Chip which has 4 banks and a confusing page / block
 * assignment. See Renesas datasheet for further information */
#define NAND_IS_AND		                0x00000020

/* Chip has a array of 4 pages which can be read without
 * additional ready /busy waits */
#define NAND_4PAGE_ARRAY	            0x00000040

/* Chip requires that BBT is periodically rewritten to prevent
 * bits from adjacent blocks from 'leaking' in altering data.
 * This happens with the Renesas AG-AND chips, possibly others.  */
#define BBT_AUTO_REFRESH	            0x00000080

/* Chip does not require ready check on read. True
 * for all large page devices, as they do not support
 * autoincrement.*/
#define NAND_NO_READRDY		            0x00000100

/* Chip does not allow subpage writes */
#define NAND_NO_SUBPAGE_WRITE	        0x00000200

/* Device is one of 'new' xD cards that expose fake nand command set */
#define NAND_BROKEN_XD		            0x00000400

/* Device behaves just like nand, but is readonly */
#define NAND_ROM		                0x00000800

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS \
	(NAND_NO_PADDING | NAND_CACHEPRG | NAND_COPYBACK)

/* Macros to identify the above */
#define NAND_CANAUTOINCR(chip)          (!(chip->options & NAND_NO_AUTOINCR))
#define NAND_MUST_PAD(chip)             (!(chip->options & NAND_NO_PADDING))
#define NAND_HAS_CACHEPROG(chip)        ((chip->options & NAND_CACHEPRG))
#define NAND_HAS_COPYBACK(chip)         ((chip->options & NAND_COPYBACK))
/* Large page NAND with SOFT_ECC should support subpage reads */
#define NAND_SUBPAGE_READ(chip)         ((chip->ecc.mode == NAND_ECC_SOFT) \
					                        && (chip->page_shift > 9))

/* Mask to zero out the chip options, which come from the id table */
#define NAND_CHIPOPTIONS_MSK	        (0x0000ffff & ~NAND_NO_AUTOINCR)

/* Non chip related options */
/* Use a flash based bad block table. This option is passed to the
 * default bad block table function. */
#define NAND_USE_FLASH_BBT	            0x00010000
/* This option skips the bbt scan during initialization. */
#define NAND_SKIP_BBTSCAN	            0x00020000
/* This option is defined if the board driver allocates its own buffers
   (e.g. because it needs them DMA-coherent */
#define NAND_OWN_BUFFERS	            0x00040000
/* Chip may not exist, so silence any errors in scan */
#define NAND_SCAN_SILENT_NODEV	        0x00080000

/* Options set by nand scan */
/* Nand scan has allocated controller struct */
#define NAND_CONTROLLER_ALLOC	        0x80000000

/* Cell info constants */
#define NAND_CI_CHIPNR_MSK	            0x03
#define NAND_CI_CELLTYPE_MSK	        0x0C

/* following two macros are used for bad block managemet, stored in host->bbt_options */
/* Search good / bad pattern on the first and the second page */
#define NAND_BBT_SCAN2NDPAGE	        0x00008000

/* Search good / bad pattern on the last page of the eraseblock */
#define NAND_BBT_SCANLASTPAGE	        0x00010000

struct nand_oobfree
{
    u32 offset;
    u32 length;
};

struct mtd_partition {
	char *name;			/* identifier string */
    u32 reserved;		/* reserved for 64bit aligne */
	FSZ size;			/* partition size */
	FSZ offset;			/* offset within the master MTD space */
	u32 mask_flags;		/* master MTD flags to mask out for this partition */
	struct nand_ecclayout *ecclayout;	/* out of band layout for this partition (NAND only)*/
};

struct nand_ecclayout
{
    u32 eccbytes;
    u32 eccpos[64];
    u32 oobavail;
    struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES];
};

#pragma pack(1)
struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	u8 sig[4];
	u16 revision;
	u16 features;
	u16 opt_cmd;
	u8 reserved[22];

	/* manufacturer information block */
	s8 manufacturer[12];
	s8 model[20];
	u8 jedec_id;	  		
	u16 date_code;
	u8 reserved2[13];    

	/* memory organization block */
	u32 byte_per_page;
	u16 spare_bytes_per_page;     
	u32 data_bytes_per_ppage;
	u16 spare_bytes_per_ppage;
	u32 pages_per_block;
	u32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	u16 bb_per_lun;
	u16 block_endurance;
	u8 guaranteed_good_blocks;
	u16 guaranteed_block_endurance;
	u8 programs_per_page;
	u8 ppage_attr;
	u8 ecc_bits;
	u8 interleaved_bits;
	u8 interleaved_ops;
	u8 reserved3[13];

	/* electrical parameter block */
	u8 io_pin_capacitance_max;
	u16 async_timing_mode;
	u16 program_cache_timing_mode;
	u16 t_prog;
	u16 t_bers;
	u16 t_r;
	u16 t_ccs;
	u16 src_sync_timing_mode;
	u16 src_ssync_features;
	u16 clk_pin_capacitance_typ;
	u16 io_pin_capacitance_typ;
	u16 input_pin_capacitance_typ;
	u8 input_pin_capacitance_max;
	u8 driver_strenght_support;
	u16 t_int_r;
	u16 t_ald;
	u8 reserved4[7];

	/* vendor */
	u8 reserved5[90];

	u16 crc;
};
#pragma pack()


#define NAND_MAX_OOBSIZE	436
#define NAND_MAX_PAGESIZE	8192

struct nand_buffers {
	u8 ecccalc[NAND_MAX_OOBSIZE];
	u8 ecccode[NAND_MAX_OOBSIZE];
	u8 databuf[NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE];
};

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

/* addr for ONFI */
#define NAND_READ_ID_ONFI	    0x20
#define NAND_OP_READ_ID         0x66
#define NAND_OP_READ_PARAM      0x66
#define ONFI_CRC_BASE           0x4F4E
#define NANDC_ADDRBIT_OFFSET    9           /* addr cycle offset in op reg */
#define NANDC_STATUS_OP_DONE    1

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
 */
struct nand_flash_dev {
	char *name;
	union {
		struct {
			unsigned char mfr_id;
			unsigned char dev_id;
		};
		unsigned char id[NANDC_READID_SIZE];
	};
	unsigned int pagesize;
	unsigned int chipsize;
	unsigned int erasesize;
	unsigned int options;
	unsigned short id_len;
	unsigned short oobsize;
};

/**
 * struct nand_manufacturers - NAND Flash Manufacturer ID Structure
 * @name:	Manufacturer name
 * @id:		manufacturer ID code of device.
*/
struct nand_manufacturers {
	int id;
	char * name;
};

extern struct nand_flash_dev nand_flash_ids[];
extern struct nand_manufacturers nand_manuf_ids[];

/*
 * helper functions
 */
/*#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))*/

#define min_t(type, x, y)  (((type)(x) < (type)(y)) ? (x) : (y))

#define max_t(type, x, y)  (((type)(x) > (type)(y)) ? (x) : (y))

static __inline int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

/*
 * A helper for defining older NAND chips where the second ID byte fully
 * defined the chip, including the geometry (chip size, eraseblock size, page
 * size). All these chips have 512 bytes NAND page size.
 */
#define LEGACY_ID_NAND(nm, devid, chipsz, erasesz, opts)          \
	{ .name = (nm), {{ .dev_id = (devid) }}, .pagesize = 512, \
	  .chipsize = (chipsz), .erasesize = (erasesz), .options = (opts) }

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
#define EXTENDED_ID_NAND(nm, devid, chipsz, opts)                      \
	{ .name = (nm), {{ .dev_id = (devid) }}, .chipsize = (chipsz), \
	  .options = (opts) }

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_LINUX_H_*/

