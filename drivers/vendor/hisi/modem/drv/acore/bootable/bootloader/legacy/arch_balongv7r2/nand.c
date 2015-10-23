/*************************************************************************
* Copyright(C) 2013 Hisilicon Tech.Co.,Ltd.
*         !!! Notice: This code is not open source.
*             It is confidential
**************************************************************************
* filename: nand.c
* History:
*
*************************************************************************/
#include <boot/flash.h>
#include <boot/boot.h>
#include "drv_comm.h"
#include <bsp_nandc.h>
#include <bsp_shared_ddr.h>
#include "ptable_com.h"
#include <balongv7r2/serial.h>
#include <nandc_balong.h>
#include <nandc_def.h>

int flash_init(void)
{
    int ret = ERROR;
    nand_init_clear();

    ret = nand_init();
    if(ret)
    {
        cprintf("ERROR: nand init failed, ret = 0x%x\n", ret);
        goto ERRO;
    }

    return ret;

ERRO:
    return ret;
}

void* nand_block_buf = NULL;
unsigned nand_block_buf_size = 0;

void* nand_get_block_buf(unsigned size)
{
    if(!nand_block_buf || nand_block_buf_size != size)
    {
        nand_block_buf = alloc(size);
        nand_block_buf_size = size;
    }
    return nand_block_buf;
}

int nandf_read(void* u32DescBuffAddr, unsigned int u32SrcFlashAddr, unsigned int u32Length, unsigned int *skip_len)
{
    int ret = 0;

    ret = nand_read(u32SrcFlashAddr, (u32)u32DescBuffAddr, u32Length, skip_len);

    return ret;
}
/*****************************************************************************
* name		: flash_read_ext
*
* description	: Read data from nand
*
* input		: ptentry *ptn: flash partition to read
*			: unsigned extra_per_page:
*			: unsigned offset: offset from partition start address
*			: void *data: buffer to save data
*             		: unsigned bytes: number of bytes to read
*
* return		: NANDF_ERROR_INIT: Init error
*			  NANDF_ERROR_ARGS: Input error
*			  NANDF_ERROR_READ: Read error
*			  NANDF_OK: Read successful
*
* other		: No
*****************************************************************************/
int flash_read_ext(ptentry *ptn, unsigned extra_per_page, unsigned offset, void *data, unsigned bytes, unsigned int *skip_len)
{
    int ret = 0;
    u64 src_addr = 0;

    src_addr = ptn->start + offset;

    ret = nand_read_oob((u64)src_addr, (u32)data, bytes, extra_per_page, skip_len);

    return ret;
}

/*****************************************************************************
* name		: flash_erase
*
* description	: Erase nand block
*
* input		: ptentry *ptn: partition to be erase
*
* return		: NANDF_OK: erase OK
*			  NANDF_ERROR_INIT: Init failed
*			  NANDF_ERROR_ARGS: Input error
*			  NANDF_BAD_BLOCK: the block to be erase is a bad block
*			  NANDF_ERROR_ERASE: erase failed, and mark the block as a bad block
*			  NANDF_ERROR_ERASE_MARKBAD: erase failed, and mark bad block failed
*			  NANDF_ERROR_SEMTAKE: apply semphore failed
*			  NANDF_ERROR_SEMGIVE: release semphore failed
*
* other		: No
*****************************************************************************/
int flash_erase(ptentry *ptn)
{
	int ret = ERROR;
	unsigned i = 0;
	unsigned block_start = 0;
	unsigned block_end= 0;
    unsigned bad_flag;
    struct nand_spec spec;

    ret = nand_get_spec(&spec);
    if(ret)
    {
        cprintf("ERROR: nand get spec failed!\n");
        return ERROR;
    }

    /* coverity[uninit_use] */
	block_start = (ptn->start + spec.blocksize- 1) / spec.blocksize;
	block_end = (ptn->start + ptn->length + spec.blocksize - 1) / spec.blocksize;

	for (i = block_start; i < block_end; i++)
	{
	    ret = (s32)nand_isbad(i, &bad_flag);
        if(NANDC_OK != ret || NANDC_BAD_BLOCK == bad_flag)
        {
            cprintf("block %d is bad, not erase, ret=%x\n", i, ret);
            continue;
        }

        ret = (s32)nand_erase(i);
        if(ret)
        {
            cprintf("erase block %d failed, ret=%x\n", i, ret);
        }
	}

    return OK;
}

/*****************************************************************************
* name		: flash_erase_all
*
* description	: Erase all good nand block
*
* input		: null
*
* return	: null
* other		: No
*****************************************************************************/
void flash_erase_all(void)
{
    struct ST_PART_TBL *part = ptable_get_ram_data();
    ptentry tn;
    int ret = OK;

    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        /* coverity[buffer_size_warning] */
        strncpy(tn.name, part->name, 16);
        tn.start = part->offset;
        tn.length = part->capacity;
        ret = flash_erase(&tn);
        if(ret)
        {
            cprintf("ERROR: flash erase failed, ret = 0x%x\n", ret);
            return;
        }
        else
        {
            cprintf("SUCCESS: erase partition %s OK\n", tn.name);
        }
        part++;
    }
    return;
}

/*****************************************************************************
* name		: partition_name_check
*
* description	: check whether partition name is legal.
*
* input		: char *partition_name: partition to check
*
* other		: No
*****************************************************************************/
char *partition_name_check(char *partition_name)
{
    char *temp_name;

    /* ignore while space at the start position*/
    while(*partition_name == ' ')
    {
        partition_name++;
    }

    /* if partition name is NULL, error. ps: 0x0D is '\n' */
    if(*partition_name == 0x0D)
    {
        return NULL;
    }

    /* partition name must end with 0 */
    for(temp_name = partition_name; (*temp_name != ' ') && (*temp_name != 0x0D); temp_name++)
    {
        /* NOP */
    }
    *temp_name = 0;
    return partition_name;

}


void get_partition_badblock(struct ptentry *ptn, struct nand_spec *spec)
{
	unsigned i = 0;
    unsigned block_start = 0;
	unsigned block_end= 0;
    unsigned bad_flag;
    unsigned bad_block_num =0;
    unsigned ret;

    block_start = (ptn->start + spec->blocksize- 1) / spec->blocksize;
	block_end = (ptn->start + ptn->length + spec->blocksize - 1) / spec->blocksize;

	for (i = block_start; i < block_end; i++)
	{
	    ret = nand_isbad(i, &bad_flag);
        if(ret)
        {
            cprintf("ERROR: check flash bad failed\n");
            return;
        }

	    if(NANDC_BAD_BLOCK == bad_flag)
        {
            cprintf("\nBad block found, partition: %s, address: 0x%x", ptn->name, i * spec->blocksize);
            bad_block_num++;
        }
	}

    if(0 == bad_block_num)
    {
        cprintf("\nNo bad block found, partition: %s", ptn->name);
    }

    return;
}

/*****************************************************************************
* name		: flash_scan_bad
*
* description	: scan bad block, if partition_name is NULL, scan all partitions
*
* input		: char *partition_name: partition to scan
*
* other		: No
*****************************************************************************/
void flash_scan_bad(char *partition_name)
{
    struct ptentry *ptn = NULL;
    struct ST_PART_TBL *part = (struct ST_PART_TBL *)SHM_MEM_PTABLE_ADDR;
    struct nand_spec spec;
    struct ptentry tn;

    if(nand_get_spec(&spec))
    {
        cprintf("ERROR: nand get spec failed!\n");
        return;
    }

    partition_name = bsp_nand_argument_check(partition_name);

    /* scan all partitions */
    if(!partition_name)
    {
        part++;
        while(0 != strcmp(PTABLE_END_STR, part->name))
        {
            if(DATA_VALID == ptable_get_validity(part))
            {
                /* coverity[buffer_size_warning] */
                strncpy(tn.name, part->name, 16);
                tn.start = part->offset;
                tn.length = part->capacity;
                /* coverity[uninit_use_in_call] */
                get_partition_badblock(&tn, &spec);
            }


            part++;
        }
    }
    else /* scan one partition */
    {
        ptn = flash_find_ptn(partition_name);
        if(!ptn)
        {
            cprintf("ERROR: wrong partition name: %s\n", partition_name);
            return;
        }
        /* coverity[uninit_use_in_call] */
        get_partition_badblock(ptn, &spec);
    }

    cprintf("\n\n");
    return;
}

/*****************************************************************************
* name		: flash_erase_force
*
* description	: Erase nand block force, even if it's bad.
*
* input		: char *partition_name: partition to be erase
*
* other		: No
*****************************************************************************/
void flash_erase_force(char *partition_name)
{
	unsigned i = 0;
    unsigned ret = ERROR;
	unsigned block_start = 0;
	unsigned block_end= 0;

    struct nand_spec spec;
    struct ptentry *ptn = NULL;

    ret = nand_get_spec(&spec);
    if(ret)
    {
        cprintf("ERROR: nand get spec failed!\n");
        goto EXIT;
    }

    partition_name = bsp_nand_argument_check(partition_name);
    if(NULL == partition_name)
    {
        cprintf("ERROR: no partition name!\n");
        goto EXIT;
    }

    ptn = flash_find_ptn(partition_name);
    if(!ptn)
    {
        cprintf("ERROR: wrong partition name: %s\n", partition_name);
        goto EXIT;
    }

    /* coverity[uninit_use] */
	block_start = (ptn->start + spec.blocksize- 1) / spec.blocksize;
	block_end = (ptn->start + ptn->length + spec.blocksize - 1) / spec.blocksize;

	for (i = block_start; i < block_end; i++)
	{
	    ret = nand_erase(i);
        if(ret)
        {
            cprintf("ERROR: nand erase error, block id = %d, ret = %d.\n", i, ret);
        }
	}
    cprintf("Erase %s force finished!\n", partition_name);

    return;
EXIT:
    cprintf("ret = 0x%x!\n", ret);
    return;
}

/*****************************************************************************
* name		: flash_write
*
* description	: write data to nand
*
* input		: ptentry *ptn: flash partition to write
*			: unsigned extra_per_page:
*			: void *data: buffer to save data
*             		: unsigned bytes: number of bytes to write
*
* return		: NANDF_ERROR_INIT: Init failed
*			  NANDF_ERROR_ARGS: Input error
*			  NANDF_BAD_BLOCK: bad block
*			  NANDF_ERROR_READ: read error when check bad block
*			  NANDF_ERROR_WRITE: write error and mark bad block error
*			  NANDF_ERROR_WRITE_MARKBAD: write error and mark bad block successful
*			  NANDF_ERROR_SEMTAKE: apply semphore failed
*			  NANDF_ERROR_SEMGIVE: release semphore failed
*			  NANDF_OK: write successful
*
* other		: No
*****************************************************************************/
int flash_write(ptentry *ptn, unsigned extra_per_page, const void *data, unsigned bytes)
{
    int ret = 0;
    u64 startAddr = 0;

    startAddr = ptn->start;

    ret = nand_write_oob((u64)startAddr, (u32)data, bytes, extra_per_page,NULL);

    return ret;
}


