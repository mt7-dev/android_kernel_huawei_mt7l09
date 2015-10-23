/*
 *  drivers/mtd/nandids.c
 *
 *  Copyright (C) 2002 Thomas Gleixner (tglx@linutronix.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
 #ifdef __KERNEL__
#include <linux/module.h>
#include <linux/mtd/nand.h>
#else
#include "nandc_inc.h"
#endif
#include <bsp_nandc.h>

/*
*	Chip ID list
*
*	Name. ID code, pagesize, chipsize in MegaByte, eraseblock size,
*	options
*
*	Pagesize; 0, 256, 512
*	0	get this information from the extended chip ID
+	256	256 Byte page size
*	512	512 Byte page size
*/
struct nand_flash_dev nand_flash_ids[] = {
    /*
	 * Some incompatible NAND chips share device ID's and so must be
	 * listed by full ID. We list them first so that we can easily identify
	 * the most specific match.
	 */
	{"TC58NVG2S0F 4G 3.3V 8-bit",
		{ .id = {0x98, 0xdc, 0x90, 0x26, 0x76, 0x15, 0x01, 0x08} },
		  NANDC_SIZE_4K, SZ_512, SZ_256K, 0, 8, 224},
	{"TC58NVG3S0F 8G 3.3V 8-bit",
		{ .id = {0x98, 0xd3, 0x90, 0x26, 0x76, 0x15, 0x02, 0x08} },
		  NANDC_SIZE_4K, SZ_1K, SZ_256K, 0, 8, 232},
	{"TC58NVG5D2 32G 3.3V 8-bit",
		{ .id = {0x98, 0xd7, 0x94, 0x32, 0x76, 0x56, 0x09, 0x00} },
		  NANDC_SIZE_8K, SZ_4K, SZ_1M, 0, 8, 640},
	{"TC58NVG6D2 64G 3.3V 8-bit",
		{ .id = {0x98, 0xde, 0x94, 0x82, 0x76, 0x56, 0x04, 0x20} },
		  NANDC_SIZE_8K, SZ_8K, SZ_2M, 0, 8, 640},
		  
    /*name,  id,  chipsize, erasesize, options  */
	LEGACY_ID_NAND("NAND 4MiB 5V 8-bit",   0x6B, 4, NANDC_SIZE_8K, 0),
	LEGACY_ID_NAND("NAND 4MiB 3,3V 8-bit", 0xE3, 4, NANDC_SIZE_8K, 0),
	LEGACY_ID_NAND("NAND 4MiB 3,3V 8-bit", 0xE5, 4, NANDC_SIZE_8K, 0),
	LEGACY_ID_NAND("NAND 8MiB 3,3V 8-bit", 0xD6, 8, NANDC_SIZE_8K, 0),
	LEGACY_ID_NAND("NAND 8MiB 3,3V 8-bit", 0xE6, 8, NANDC_SIZE_8K, 0),

    LEGACY_ID_NAND("NAND 16MiB 1,8V 8-bit",	    0x33, 16, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 16MiB 3,3V 8-bit",	    0x73, 16, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 16MiB 1,8V 16-bit",	0x43, 16, NANDC_SIZE_16K, NAND_BUSWIDTH_16),
	LEGACY_ID_NAND("NAND 16MiB 3,3V 16-bit",	0x53, 16, NANDC_SIZE_16K, NAND_BUSWIDTH_16),

	LEGACY_ID_NAND("NAND 32MiB 1,8V 8-bit",	    0x35, 32, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 32MiB 3,3V 8-bit",	    0x75, 32, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 32MiB 1,8V 16-bit",	0x45, 32, NANDC_SIZE_16K, NAND_BUSWIDTH_16),
	LEGACY_ID_NAND("NAND 32MiB 3,3V 16-bit",	0x55, 32, NANDC_SIZE_16K, NAND_BUSWIDTH_16),

	LEGACY_ID_NAND("NAND 64MiB 1,8V 8-bit",	    0x36, 64, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 64MiB 3,3V 8-bit",	    0x76, 64, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 64MiB 1,8V 16-bit",	0x46, 64, NANDC_SIZE_16K, NAND_BUSWIDTH_16),
	LEGACY_ID_NAND("NAND 64MiB 3,3V 16-bit",	0x56, 64, NANDC_SIZE_16K, NAND_BUSWIDTH_16),

	LEGACY_ID_NAND("NAND 128MiB 1,8V 8-bit",	0x78, 128, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 128MiB 1,8V 8-bit",	0x39, 128, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 128MiB 3,3V 8-bit",	0x79, 128, NANDC_SIZE_16K, 0),
	LEGACY_ID_NAND("NAND 128MiB 1,8V 16-bit",	0x72, 128, NANDC_SIZE_16K, NAND_BUSWIDTH_16),
	LEGACY_ID_NAND("NAND 128MiB 1,8V 16-bit",	0x49, 128, NANDC_SIZE_16K, NAND_BUSWIDTH_16),
	LEGACY_ID_NAND("NAND 128MiB 3,3V 16-bit",	0x74, 128, NANDC_SIZE_16K, NAND_BUSWIDTH_16),
	LEGACY_ID_NAND("NAND 128MiB 3,3V 16-bit",	0x59, 128, NANDC_SIZE_16K, NAND_BUSWIDTH_16),

	LEGACY_ID_NAND("NAND 256MiB 3,3V 8-bit",	0x71, 256, NANDC_SIZE_16K, 0),

	/*
	 * These are the new chips with large page size. The pagesize and the
	 * erasesize is determined from the extended id bytes
	 */
#define LP_OPTIONS (NAND_SAMSUNG_LP_OPTIONS | NAND_NO_READRDY | NAND_NO_AUTOINCR)
#define LP_OPTIONS16 (LP_OPTIONS | NAND_BUSWIDTH_16)

	/*512 Megabit */
	EXTENDED_ID_NAND("NAND 64MiB 1,8V 8-bit",   0xA2, 64, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64MiB 1,8V 8-bit",   0xA0, 64, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 8-bit",   0xF2, 64, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 8-bit",   0xD0, 64, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 8-bit",   0xF0, 64, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64MiB 1,8V 16-bit",  0xB2, 64, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 64MiB 1,8V 16-bit",  0xB0, 64, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 16-bit",  0xC2, 64, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 16-bit",  0xC0, 64, LP_OPTIONS16),

	/* 1 Gigabit */
	EXTENDED_ID_NAND("NAND 128MiB 1,8V 8-bit",	0xA1, 128, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 128MiB 3,3V 8-bit",	0xF1, 128, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 128MiB 3,3V 8-bit",	0xD1, 128, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 128MiB 1,8V 16-bit",	0xB1, 128, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 128MiB 3,3V 16-bit",	0xC1, 128, LP_OPTIONS16),
    EXTENDED_ID_NAND("NAND 128MiB 1,8V 16-bit", 0xAD, 128, LP_OPTIONS16),
    
	/* 2 Gigabit */
	EXTENDED_ID_NAND("NAND 256MiB 1,8V 8-bit",	0xAA, 256, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 256MiB 3,3V 8-bit",	0xDA, 256, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 256MiB 1,8V 16-bit",	0xBA, 256, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 256MiB 3,3V 16-bit",	0xCA, 256, LP_OPTIONS16),

	/* 4 Gigabit */
	EXTENDED_ID_NAND("NAND 512MiB 1,8V 8-bit",	0xAC, 512, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 512MiB 3,3V 8-bit",	0xDC, 512, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 512MiB 3,3V 8-bit",	0xD7, 512, LP_OPTIONS), /*v3r2 add 8k*/
	EXTENDED_ID_NAND("NAND 512MiB 1,8V 16-bit",	0xBC, 512, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 512MiB 3,3V 16-bit",	0xCC, 512, LP_OPTIONS16),

	/* 8 Gigabit */
	EXTENDED_ID_NAND("NAND 1GiB 1,8V 8-bit",	0xA3, 1024, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 1GiB 3,3V 8-bit",	0xD3, 1024, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 1GiB 1,8V 16-bit",	0xB3, 1024, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 1GiB 3,3V 16-bit",	0xC3, 1024, LP_OPTIONS16),

	/* 16 Gigabit */
	EXTENDED_ID_NAND("NAND 2GiB 1,8V 8-bit",	0xA5, 2048, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 2GiB 3,3V 8-bit",	0xD5, 2048, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 2GiB 1,8V 16-bit",	0xB5, 2048, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 2GiB 3,3V 16-bit",	0xC5, 2048, LP_OPTIONS16),

    /* 32 Gigabit */
	EXTENDED_ID_NAND("NAND 4GiB 1,8V 8-bit",  0xA7, 4096, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 4GiB 3,3V 8-bit",  0xD7, 4096, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 4GiB 1,8V 16-bit", 0xB7, 4096, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 4GiB 3,3V 16-bit", 0xC7, 4096, LP_OPTIONS16),

	/* 64 Gigabit */
	EXTENDED_ID_NAND("NAND 8GiB 1,8V 8-bit",  0xAE, 8192, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 8GiB 3,3V 8-bit",  0xDE, 8192, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 8GiB 1,8V 16-bit", 0xBE, 8192, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 8GiB 3,3V 16-bit", 0xCE, 8192, LP_OPTIONS16),

	/* 128 Gigabit */
	EXTENDED_ID_NAND("NAND 16GiB 1,8V 8-bit",  0x1A, 16384, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 16GiB 3,3V 8-bit",  0x3A, 16384, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 16GiB 1,8V 16-bit", 0x2A, 16384, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 16GiB 3,3V 16-bit", 0x4A, 16384, LP_OPTIONS16),

	/* 256 Gigabit */
	EXTENDED_ID_NAND("NAND 32GiB 1,8V 8-bit",  0x1C, 32768, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 32GiB 3,3V 8-bit",  0x3C, 32768, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 32GiB 1,8V 16-bit", 0x2C, 32768, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 32GiB 3,3V 16-bit", 0x4C, 32768, LP_OPTIONS16),

	/* 512 Gigabit */
	EXTENDED_ID_NAND("NAND 64GiB 1,8V 8-bit",  0x1E, 65536, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64GiB 3,3V 8-bit",  0x3E, 65536, LP_OPTIONS),
	EXTENDED_ID_NAND("NAND 64GiB 1,8V 16-bit", 0x2E, 65536, LP_OPTIONS16),
	EXTENDED_ID_NAND("NAND 64GiB 3,3V 16-bit", 0x4E, 65536, LP_OPTIONS16),

	{NULL}
};

/*
*	Manufacturer ID list
*/
struct nand_manufacturers nand_manuf_ids[] = {
	{NAND_MFR_TOSHIBA, "Toshiba"},
	{NAND_MFR_SAMSUNG, "Samsung"},
	{NAND_MFR_FUJITSU, "Fujitsu"},
	{NAND_MFR_NATIONAL, "National"},
	{NAND_MFR_RENESAS, "Renesas"},
	{NAND_MFR_STMICRO, "ST Micro"},
	{NAND_MFR_HYNIX, "Hynix"},
	{NAND_MFR_MICRON, "Micron"},
	{NAND_MFR_AMD, "AMD/Spansion"},
	{NAND_MFR_MACRONIX, "Macronix"},
	{NAND_MFR_EON, "Eon"},
	{0x0, "Unknown"}
};

#ifdef __KERNEL__
EXPORT_SYMBOL(nand_manuf_ids);  /*lint !e19 */
EXPORT_SYMBOL(nand_flash_ids);  /*lint !e19 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Gleixner <tglx@linutronix.de>");
MODULE_DESCRIPTION("Nand device & manufacturer IDs");
#endif
