#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_nandc.h"

#ifndef NANDC_OK
#define NANDC_OK        0
#endif

#ifndef NANDC_ERROR
#define NANDC_ERROR     1
#endif

#ifdef __FASTBOOT__
#include <boot/flash.h>
#else /* __KERNEL__ */
#include <linux/mtd/mtd.h>
#include <linux/err.h>
#endif

/* Interface for other modules */
#ifdef __KERNEL__

/**
 * Name			:	bsp_nand_read
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset	- offset from partition to read from
 * 					@ptr_ram_addr  	- pointer to ram addr to store the data read from flash
 * 					@length        	- number of bytes to read
 * Return		:	0				- success
 *				:   else  			- failure
 * Desciption	:	NULL
 */
int bsp_nand_read(const char *partition_name, loff_t partition_offset, void* ptr_ram_addr, size_t length, u32 *skip_len)
{
	int ret = NANDC_ERROR;
    size_t retlen = 0;
    struct mtd_info *mtd = NULL;

    /* check param */
	if((!partition_name) || (!ptr_ram_addr))
	{
		printk(KERN_ERR"param error.\n");
		goto erro;
	}

	/* get mtd device */
	mtd = get_mtd_device_nm(partition_name);
	if (IS_ERR(mtd))
    {
		printk(KERN_ERR"get_mtd_device_nm error\n");
		ret = PTR_ERR(mtd);
		goto erro;
	}

    if(partition_offset >= mtd->size)
    {
        printk(KERN_ERR"ERROR: invalid partition offset 0x%llx , 0x%llx ,%s!\n", partition_offset,mtd->size,mtd->name);
        goto erro;
    }

	/* read */
    ret = mtd_read(mtd, partition_offset, length, &retlen, (unsigned char*)ptr_ram_addr);
    if(ret)
    {
        printk(KERN_ERR"mtd read error, ret = 0x%x.\n", ret);
        goto erro;
    }

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        put_mtd_device(mtd);
    }

    return 0;

erro:

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        put_mtd_device(mtd);
    }

    return ret;
}

/**
 * Name			:	bsp_nand_erase
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset	- offset from partition to erase
 * Return		:	0 				- success
 *				:   else  			- failure
 * Desciption	:	NULL
 */
int bsp_nand_erase(const char *partition_name, loff_t partition_offset)
{
    /* eMMC doesn't need erasing  */
    return 0;
}

/**
 * Name			:	bsp_nand_write
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset	- offset from partition to write to, must be page align.
 * 					@ptr_ram_addr  	- pointer to ram addr to store the data write to flash
 * 					@length        	- number of bytes to write
 * Return		:	0 				- success
 *				:   else  			- failure
 * Description	: 	write flash
 */
s32 bsp_nand_write(const char *partition_name, loff_t partition_offset, void* ptr_ram_addr, size_t length)
{
	int ret = NANDC_ERROR;
	size_t retlen = 0;
    struct mtd_info *mtd = NULL;

    /* check param */
	if((!partition_name) || (!ptr_ram_addr))
	{
		printk(KERN_ERR"param error.\n");
		goto erro;
	}

	/* get mtd device */
	mtd = get_mtd_device_nm(partition_name);
	if (IS_ERR(mtd))
    {
		printk(KERN_ERR"get_mtd_device_nm error\n");
		ret = PTR_ERR(mtd);
		goto erro;
	}

    if(partition_offset >= mtd->size)
    {
        printk(KERN_ERR"ERROR: invalid partition offset 0x%llx!\n", partition_offset);
        goto erro;
    }

    /* write to flash */
    ret = mtd_write(mtd, partition_offset, length, &retlen, (unsigned char*)ptr_ram_addr);
    if(ret)
    {
        /* write failed */
        printk(KERN_ERR"mtd write failed!\n");
        goto erro;
    }
    mtd_sync(mtd);
    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        put_mtd_device(mtd);
    }

    return 0;

erro:
    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        put_mtd_device(mtd);
    }

	return ret;
}

/**
 * Name			: 	bsp_nand_isbad
 * Arguments	: 	@partition_name  	- partition name
 * 					@partition_offset	- block offset from partition to check
 *
 * Return		:	0 				- good block
 *				:   1  			    - bad block
 *              :   -1              - error
 * Description	: 	check whether a block is bad
 */
int bsp_nand_isbad(const char *partition_name, loff_t partition_offset)
{
    /* eMMC doesn't need check bad */
	return 0;
}

/**
 * Name			:	bsp_nand_blk_write_nv
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset   - offset from partition to read from, must be block aligned.
 * 					@flag        	- flag value. NV_FLAG means write successful last time, else failure
 * Return		:	0 				- success
 *				:   else  			- failure
 * Description	: 	read nv_flag of a block
 */
unsigned int bsp_nand_read_flag_nv(const char *partition_name, loff_t partition_offset, unsigned char *flag)
{
    if (!flag)
    {
        return NANDC_ERROR;
    }
    *flag = NV_WRITE_SUCCESS;

    return NANDC_OK;
}

/**
 * Name			:	bsp_nand_blk_write_nv
 * Arguments	:	@mtd  	        - mtd device
 * 					@to	            - offset from partition to write to, must be page align.
 * 					@len  	        - write data lenth
 * 					@retlen        	- write length actually
 * 					@buf        	- source data address
 * Return		:	0 				- success
 *				:   else  			- failure
 * Description	: 	write flash for nv, as writing data and oob for last page, else only data
 */
int bsp_nand_blk_write_nv(struct mtd_info *mtd, loff_t to, size_t len, const uint8_t *buf)
{
    return bsp_nand_write(mtd->name, to, (void*)buf, len);
}

#else /* __FASTBOOT__ */

/**
 * Name			:	bsp_nand_read
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset	- offset from partition to read from
 * 					@ptr_ram_addr  	- pointer to ram addr to store the data read from flash
 * 					@length        	- number of bytes to read
 * Return		:	0				- success
 *				:   else  			- failure
 * Desciption	:	NULL
 */
int bsp_nand_read(char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length, u32 *skip_len)
{
    u32 flash_addr;
    ptentry *p = NULL;

    p = flash_find_ptn(partition_name);

    if(p == 0)
    {
        cprintf("ERROR: bsp_nand_read can't find partition %s\n", partition_name);
        return NANDC_ERROR;
    }

    return flash_read(p, 0, partition_offset, ptr_ram_addr, length, NULL);

}

/**
 * Name			:	bsp_get_nand_info
 * Arguments	:	@spec  		    - to store flash info
 * Return		:	0 				- success
 *				:   else  			- failure
 * Desciption	:	adapt for nv in k3v3
 */
int bsp_get_nand_info(struct nand_spec *spec)
{
	spec->pagesize 	 		= 512;
	spec->pagenumperblock	= 0;
	spec->addrnum      		= 0;
	spec->ecctype 	 		= 0;
	spec->buswidth   		= 0;
    spec->blocksize         = 0x20000;          /* 128KB */
    spec->sparesize         = 0;
    spec->chipsize          = 0;

	return NANDC_OK;
erro:
	return NANDC_ERROR;
}

#endif /* #ifdef __KERNEL__ */

#ifdef __cplusplus
}
#endif

