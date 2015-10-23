
/* Description: nand controller operations in dependence on  hardware,
*              this source file is only for some platforms but not for Linux
*              platform.
*
*                   |                           Layer: platform(Vxworks/ADS/bootloater/fastboot)
*              nand_balong.c
*                   |
*                   |                           Layer: porting
*              nandc_nand.c
*                   |
*                   |
*        ------------------------
*          |                  |
*          |(init)            |(operation)
*       nandc_host.c       nandc_ctrl.c               Layer: controller
*          |           -----------------------
*      nandc_native.c         |
*                      nandc_vxxx.c(nandc_v400.c)       Layer: hardware
******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*
*    All rights reserved.
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __KERNEL__
#include <linux/mtd/mtd.h>
#include <linux/err.h>
#include <linux/vmalloc.h>
#endif

#include <ptable_inc.h>
#include "nandc_balong.h"

struct nand_bus  nand_handle = {0, 0, 0, 0, 0};
 /*******************************************************************************
 * FUNC NAME:
 * nand_init() - nand driver plantform layer initialization
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * This function initializes nand platform layer ram structure and then call low
 * layer nand initialization route
 *
 * CALL FUNC:
 *
 ********************************************************************************/
u32 nand_init_platform(u32 is_from_mtd)
{
    struct nand_bus* handle = NULL;
    u32 result = NAND_ERROR;

    handle = &nand_handle;

    if(NAND_INIT_MAGIC == handle->init_flag)
    {
        result = NAND_OK;
        NAND_TRACE(("!!nand module already  inited!!\n"));
        goto EXIT;
    }

    memset((void*)handle, 0x00, sizeof(nand_handle));

    /*set structure of nand opreation function pointer */
    handle->funcs  =  &nandc_nand_funcs;

    /*call lower nand initialization route*/

    if(handle->funcs->init)
    {
        if(NANDC_FALSE == is_from_mtd) /*lint !e737*/
        {
            result = handle->funcs->init();
        }
        else
        {
            result = NAND_OK;
        }
    }
    else
    {
        NAND_TRACE(("ERRO!!nand module init is NULL\n"));
        result = NAND_ERROR;
    }



    if(NAND_OK == result)
    {
        handle->init_flag = NAND_INIT_MAGIC;
    }
    else
    {
        NAND_TRACE(("ERRO!!nand module init failed\n"));
    }

EXIT:
    return result;

}

/**
 * Name			:	find_partition_by_name
 * Arguments	:	@partition_name - partition name
 * Return		:	not NULL		- success
 *				:   NULL  			- failure
 * Desciption	:	NULL
 */
struct ST_PART_TBL * find_partition_by_name(const char *partition_name)
{
    struct ST_PART_TBL * ptable = NULL;

    ptable_ensure();

    ptable = ptable_get_ram_data();

    /* find the partition */
    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(0 == strcmp(partition_name, ptable->name))
        {
            break;
        }
        ptable++;
    }

    if(0 == strcmp(PTABLE_END_STR, ptable->name))
    {
    	NAND_TRACE(("ERROR: can't find partition %s, function %s\n", partition_name, __FUNCTION__));
        return NULL;
    }

    return ptable;

}

#ifdef __FASTBOOT__

u32 nand_init(void)
{
    return nand_init_platform(NANDC_FALSE);
}
 /**
 * FUNC NAME:
 * nand_deinit() - nand driver plantform layer de-initialization
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * This function deinitialize nand platform layer ram structure and then call low
 * layer nand de-initialization route
 *
 * CALL FUNC:
 *
 ************************************************************************************/
u32 nand_deinit(void)
{
    struct nand_bus* handle = NULL;
    u32 result = NAND_ERROR;

	handle = &nand_handle;

    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        result = NAND_OK;
        NAND_TRACE(("!!nand module already  inited!!\n"));
        goto EXIT;
    }


    if(handle->funcs->deinit)
    {
        result = handle->funcs->deinit();
    }
    else
    {
        NAND_TRACE(("ERRO!!nand module deinit is NULL\n"));
        result = NAND_ERROR;
    }



    if(NAND_OK == result)
    {
        handle->init_flag = 0;
    }

EXIT:
    return result;

}

/*******************************************************************************
 * FUNC NAME:
 * nand_run() - unibus for all nand flash operations from external calling
 *
 * PARAMETER:
 * @param:	    [input]sturcture which contains nand operation parameters.
 * @func_type:	[input]specify nand operation type
 *
 * DESCRIPTION:
 * This is the route to low layer nand driver: nandc_nand.c,
 * this function must run after nand_init() has been called.
 *
 * CALL FUNC:
 *
 *********************************************************************************/
u32 nand_run(struct nand_param* param, u32 func_type)
{
    struct nand_bus* handle;
    struct nand_interface * nandfuncs;
    u32 result;

    handle = &nand_handle;

    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        NAND_TRACE(("error!! balong nand not inited\n"));
        return NAND_ERROR;
    }

    nandfuncs = handle->funcs;

    switch (func_type)
    {
        case READ_NAND:
        {
            if(nandfuncs->read)
            {
                result = nandfuncs->read(param->addr_flash,
                                         param->addr_data,
                                         param->size_data,
                                         param->size_oob,
                                         param->skip_length);
            }
            else
            {
                NAND_TRACE(("error!! func read_random is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case READ_RAW:
        {
            if(nandfuncs->read_page_raw)
            {
                result = nandfuncs->read_page_raw(param->number,
                                                    (u32)param->addr_flash,
                                                    (u32)param->addr_data,
                                                    param->size_data);
            }
            else
            {
                NAND_TRACE(("error!! func read_page_yaffs is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case QUARY_BAD:
        {
            if(nandfuncs->quary_bad)
            {
                result = nandfuncs->quary_bad(param->number, (u32*)param->addr_data);
            }
            else
            {
                NAND_TRACE(("error!! func quary_bad is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case ERASE_BLOCK:
        {
            if(nandfuncs->erase_by_id)
            {
                result = nandfuncs->erase_by_id(param->number);
            }
            else
            {
                NAND_TRACE(("error!! func erase_by_id is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case WRITE_NAND:
        {
            if(nandfuncs->write)
            {
                result = nandfuncs->write(param->addr_flash,
                                          param->addr_data,
                                          param->size_data,
                                          param->size_oob,
                                          param->skip_length);
            }
            else
            {
                NAND_TRACE(("error!! func write flash is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case WRITE_RAW:
        {
            if(nandfuncs->write_page_raw)
            {
                result = nandfuncs->write_page_raw(param->number,
                                                    (u32)param->addr_flash,
                                                    (u32)param->addr_data,
                                                    param->size_data);
            }
            else
            {
                NAND_TRACE(("error!! func write_page_raw is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case MARK_BAD:
        {
            if(nandfuncs->mark_bad)
            {
                result = nandfuncs->mark_bad(param->number );
            }
            else
            {
                NAND_TRACE(("error!! func mark_bad is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        default:
            NAND_TRACE(("ERRO!! nand function type:%d not defined\n",func_type));
            result = NAND_ERROR;
        break;
    }


    return result;
}


/*******************************************************************************************
 * FUNC NAME:
 * nand_read() - external API for nand data read operation
 *
 * PARAMETER:
 * @flash_addr - [input]read start address from nand.
 * @dst_data   - [input]ram address where data will be read to.
 * @read_size  - [input]size of data to be read form flash.
 * @skip_length- [output]a ram address that store bad block skip length during read operation,
 *               this parameter could be NULL if you don't care about skip lengt.
 *
 * DESCRIPTION:
 * This function read flash data like memcpy(), it means you can read flash data form any
 * address and read any size.
 *
 * this route don't read oob data from nand spare area.
 *
 * CALL FUNC:
 * @nand_run() -
 *
 *******************************************************************************************/
u32 nand_read(FSZ flash_addr, u32 dst_data, u32 read_size, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.addr_flash    =   flash_addr;
    param.addr_data     =   dst_data;
    param.size_data     =   read_size;
    param.skip_length   =   skip_length;

    return nand_run(&param, READ_NAND);

}

/**********************************************************************************
 * FUNC NAME:
 * nand_get_spec() - external API for get nand chip specification parmeter
 *
 * PARAMETER:
 * @spec - [output]pointer of structure contains nand chip specification parmeter
 *
 * DESCRIPTION:
 * Tish function get flash total size , page size ,spare size ,block size...etc
 *
 * CALL FUNC:
 *
 *********************************************************************************/
u32 nand_get_spec(struct nand_spec *spec)
{
    struct nand_bus* handle;
    struct nand_interface * nandfuncs;

    handle = &nand_handle;
    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        NAND_TRACE(("error!! balong nand not inited\n"));
        return NAND_ERROR;
    }

    nandfuncs = handle->funcs;

    if(nandfuncs->get_spec)
    {
        return  nandfuncs->get_spec(spec);
    }

    NAND_TRACE(("error!! get_spec not inited\n"));

    return NAND_ERROR;
}

/***********************************************************************************************
 * FUNC NAME:
 * nand_read_oob() - external API for nand data read with oob
 *
 * PARAMETER:
 * @flash_addr - [input]read start address from nand.
 * @dst_data   - [output]ram address where data will be read to.
 * @data_size  - [input]size of data to be read form flash, this is the sum of all raw data and
                 oob data, it means read_size shouled be multiple of (page size +oob_per_page)
 * @oob_per_page - [input]oob data size per page.
 *
 * DESCRIPTION:
 * This function read raw data and spare data page by page, at the same time, the raw data and
 * oob data will be write to ram buffer one by one
 *
 * CALL FUNC:
 * @nand_run() -
 *
 **********************************************************************************************/
u32 nand_read_oob(FSZ flash_addr, u32 dst_data, u32 data_size, u32 oob_per_page, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.addr_flash    =   flash_addr;
    param.addr_data     =   dst_data;
    param.size_data     =   data_size;
    param.size_oob      =   oob_per_page;
    param.skip_length   =   skip_length;

    return nand_run(&param, READ_NAND);

}

/******************************************************************************
 * FUNC NAME:
 * nand_read_raw() - external API for nand raw data read
 *
 * PARAMETER:
 * @src_page    - [input]the page number from which data will be read from flash.
 * @offset_data - [input]offset for read operation in this page
 * @dst_data    - [output]the ram address where the raw data will be read to.
 * @len_data    - [input]data length for read operation in this page
 *
 * DESCRIPTION:
 * This function read raw data without ecc correction in one page, the input parameter
 * (offset_data + len_data) should not exceed one page size.
 *
 * CALL FUNC:
 * @nand_run() -
 *
 ******************************************************************************/
u32 nand_read_raw(u32 src_page, u32 offset_data, u32 dst_data, u32 len_data, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.number        = src_page;
    param.addr_flash    = offset_data;
    param.addr_data     = dst_data;
    param.size_data     = len_data;
    param.skip_length   = skip_length;

    return nand_run(&param, READ_RAW);

}


/**********************************************************************************
 * FUNC NAME:
 * nand_isbad() - external API :check if nand block is bad
 *
 * PARAMETER:
 * @blockID - [input] the block number to be check.
 * @flag    - [output]ram address contain a flag which indicate whether a block is bad block.
 *
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() -
 *
 **********************************************************************************/
u32 nand_isbad(u32 blockID, u32 *flag)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.number    =   blockID;
    param.addr_data =   (u32)flag;

    return nand_run(&param, QUARY_BAD);

}

/*********************************************************
 * FUNC NAME:
 * nand_bad() - external API :mark a block as a bad block.
 *
 * PARAMETER:
 * @blockID - [input] the block number to be marked.
 *
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() -
 *
 ***********************************************************/
u32 nand_bad(u32 blockID)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
    u32 ret;

    param.number    =   blockID;
    ret = nand_run(&param, MARK_BAD);

    return ret;

}
/********************************************************************
 * FUNC NAME:
 * nand_erase() - external API :erase a block by block number.
 *
 * PARAMETER:
 * @blockID - [input] the block number to be erased.
 *
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() -
 *
 ********************************************************************/
u32 nand_erase(u32 blockID)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.number    =   blockID;

    return nand_run(&param, ERASE_BLOCK);

}

/**********************************************************************************************
 * FUNC NAME:
 * nand_write() - external API for nand data write
 *
 * PARAMETER:
 * @flash_addr - [input]write start address to nand.
 * @src_data   - [input]ram address where stored the source data.
 * @write_size - [input]size of data that will be writen to from ram to flash
 * @skip_length- [output]a ram address that store bad block skip length during write operation,
 *               this parameter could be NULL if you don't care about skip length;
 *
 * DESCRIPTION:
 * This function write flash data like memcpy(), it means you can write ram data to any flash address and
 * write any size.
 *
 * this route don't write oob date to nand spare area.
 *
 * CALL FUNC:
 * @nand_run() -
 *
 *********************************************************************************************/
u32 nand_write(FSZ flash_addr, u32 src_data, u32 write_size, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.addr_flash    =   flash_addr;
    param.addr_data     =   src_data;
    param.size_data     =   write_size;
    param.skip_length   =   skip_length;

    return nand_run(&param, WRITE_NAND);

}

/**********************************************************************************************
 * FUNC NAME:
 * nand_write_oob() - external API for nand data write with oob
 *
 * PARAMETER:
 * @flash_addr - [input]write start address to nand.
 * @src_data   - [input]ram address where stored the source data.
 * @data_size  - [input]size of data that will be write to flash, this is the sum of all raw data and
                 oob data, it means data_size shouled be multiple of (page size +oob_per_page)
 * @oob_per_page-[input]oob data size per page.
 *
 * DESCRIPTION:
 * This function write raw data and spare data from ram to nand page by page.

 * CALL FUNC:
 * @nand_run() -
 *
 *********************************************************************************************/
 u32 nand_write_oob(FSZ flash_addr, u32 src_data, u32 data_size, u32 oob_per_page, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.addr_flash    =   flash_addr;
    param.addr_data     =   src_data;
    param.size_data     =   data_size;
    param.size_oob      =   oob_per_page;
    param.skip_length   =   skip_length;

    return nand_run(&param, WRITE_NAND);

}

/*******************************************************************************************
 * FUNC NAME:
 * nand_write_raw() - external API for nand raw data write
 *
 * PARAMETER:
 * @dst_page    - [input]the page number of flash for write operation.
 * @offset_data - [input]offset for write operation in this page
 * @src_data    - [output]the ram buffer address where stored source data
 * @len_data    - [input]data length to be writen to this page
 *
 * DESCRIPTION:
 * write raw data without ecc correction to one page, the input parameter
 * (offset_data + len_data) should not exceed one page size.
 *
 * CALL FUNC:
 * @nand_run() -
 *
 ******************************************************************************************/
u32 nand_write_raw(u32 dst_page, u32 offset_data, u32 src_data, u32 len_data ,u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};

    param.number        = dst_page;
    param.addr_flash    = offset_data;
    param.addr_data     = src_data;
    param.size_data     = len_data;
    param.skip_length   =   skip_length;

    return  nand_run(&param, WRITE_RAW);
}

/**
 * FUNC NAME:
 * nand_scan_bad() - external API for yaffs's nand block scan and mark bad
 *
 * PARAMETER:
 * @start  - [input] the start address of nand flash to scan.
 * @length - [input] the length of nand flash to scan.
 * @if_erase - [input] do erase if block is not bad block
 *
 * DESCRIPTION:
 * this function read data block by block, if failure occured ,it mark current block as bad.
 * and then go on ,till the total length has been read.
 *
 * CALL FUNC:
 * @nand_read()       -
 * @nand_get_spec()   -
 *
 */
u32 nand_scan_bad(FSZ start, FSZ length, u32 if_erase)
{
    u32 addr_block_align;
    u32 length_block_align;
    u32 blk_id, is_bad, times, ret;
    struct nand_spec spec;
    static u32 block_buf = 0 ;

    memset(&spec, 0, sizeof(struct nand_spec));
    if(NAND_OK != nand_get_spec(&spec))
    {
        NAND_TRACE(("nand_scan_bad: nand_get_spec failed\n"));
        return NAND_ERROR;
    }

    /* coverity[uninit_use] */
    addr_block_align = start - start%(spec.blocksize);

    length_block_align = nandc_alige_size(start + length, spec.blocksize) - addr_block_align;
	if(!block_buf)
	{
		block_buf = (u32)himalloc(spec.blocksize);
		if(NULL == (void*)block_buf)//pclint 58
		{
			NAND_TRACE(("nand_scan_bad: himalloc failed\n"));
			goto ERRO;
		}	
	}
    while(length_block_align > 0)
    {
        blk_id = addr_block_align/spec.blocksize;
        if(NAND_OK != nand_isbad(blk_id, &is_bad))
        {
            NAND_TRACE(("nand_scan_bad: nand_isbad failed\n"));
            goto ERRO;
        }
        if(NANDC_GOOD_BLOCK == is_bad)
        {
            times = 1;
            while((NANDC_E_READ == nand_read((FSZ)addr_block_align, block_buf, spec.blocksize, 0)) && (times))
            {
                times--;
            };
            if(0 == times)
            {
                NAND_TRACE(("nand_scan_bad: find and read error, address:0x%x\n",addr_block_align));

                ret = nand_bad(blk_id);
                if(ret)
                {
                    NAND_TRACE(("nand_scan_bad:nand check bad failed, ret = 0x%x\n",ret));
                }
            }
            else if(NANDC_TRUE == if_erase)
            {
                ret = nand_erase(blk_id);
                if(ret)
                {
                    NAND_TRACE(("nand_scan_bad:nand erase failed, ret = 0x%x\n",ret));
                }
            }
            else
            {
            }
        }
        else
        {
            NAND_TRACE(("nand_scan_bad:find bad block: 0x%x\n",addr_block_align));
        }
        length_block_align -= spec.blocksize;
        addr_block_align   += spec.blocksize;
    }

    return NAND_OK;
ERRO:
    
    return NAND_ERROR;
}

/*****************************************************************************
 * FUNC NAME:
 * and_isinit() - external API: quary if nand module has been initialized
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 *
 * CALL FUNC: none
 *******************************************************************************/
u32 nand_isinit(void)
{
    return (nand_handle.init_flag == NAND_INIT_MAGIC);
}

/**************************************************************************************************
 * FUNC NAME:
 * nand_init_clear() - external API: this function clear .bss section of nandc module
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * This function is for some plantform, the .bss section is not cleared when system power(eg. trace .AXF),so I clear .bss
 * manually here to avoid critical error.
 *
 * CALL FUNC:
 * @nandc_minit()  -
 *
 *****************************************************************************************************/
void nand_init_clear(void)
{
    nandc_nand_host = NULL;

    memset((void*)&nand_handle, 0x00 ,sizeof(nand_handle));
}

/**
 * 作用:打印nandflash相关规格参数的函数接口
 *
 * 输入参数:无
 *
 *
 * 描述:打印nand->spec 参数值
 */
void bsp_show_nand_info(void)
{
    struct nand_spec spec;
    if(NAND_ERROR == bsp_get_nand_info(&spec))
    {
        NAND_TRACE(("ERROR: nand get spec error.\n"));
        return;
    }
    NAND_TRACE((" ------begin show nand info----------\n"));
    NAND_TRACE((" chipsize=%dMB,blocksize=%dKB,pagesize=%dKB,sparesize=%d\n",
    	                   (spec.chipsize/1024/1024),spec.blocksize/1024,spec.pagesize/1024,spec.sparesize));
    NAND_TRACE((" pagenumperblock=%d,ecctype=%d,buswidth=%d,addrnum=%d\n",
    	                   spec.pagenumperblock,spec.ecctype,(0==spec.buswidth?8:16),spec.addrnum));
    NAND_TRACE((" ------end  show nand info----------\n"));
}

#else /* __KERNEL__ */
u32 nand_balong_port_mtd(void)
{
    return nand_init_platform(NANDC_TRUE);
}

#endif

/* Interface for other modules */
#ifdef __KERNEL__
#if 0
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
{/*lint !e958*/
    size_t first_size, write_size, retlen;
    int ret = NANDC_ERROR, malloc_flag = 0;
    struct mtd_oob_ops ops; 
    unsigned char oob[YAFFS_BYTES_PER_SPARE ] = {NV_WRITE_SUCCESS};
    unsigned char *last_data_addr = NULL;

    printk(KERN_DEBUG"[%s] partition offset 0x%x, length 0x%x\n", __FUNCTION__, (unsigned int)to, len);

    /* arguments check */
	if (IS_ERR(mtd))
    {
		printk(KERN_ERR"mtd_device is null\n");
		ret = (int)PTR_ERR(mtd);
		goto erro;
	}

    if((to & (mtd->erasesize - 1)) || (to < 0))
    {
		printk(KERN_ERR"error flash address 0x%x\n", (unsigned int)to);
		goto erro;
    }

    if(len > mtd->erasesize)
    {
		printk(KERN_ERR"error data length 0x%x\n", len);
		goto erro;
    }

    first_size = mtd->erasesize - mtd->writesize;
    if(len == mtd->erasesize) /* just a block sizes */
    {
        /* write block to flash except last page */
        write_size = first_size;
        last_data_addr = (unsigned char *)(buf + first_size);
    }
    else if(len <= first_size)  /* not include last page */
    {
        write_size = len;
        last_data_addr = (unsigned char *)kmalloc(mtd->writesize, GFP_KERNEL);
    	if(!last_data_addr)
    	{
    		printk(KERN_ERR"get ram buffer failed!\n");
    		goto erro;
    	}
        malloc_flag = 1;
        memset(last_data_addr, 0xFF, mtd->writesize);
    }
    else /* len < mtd->erasesize && len > firstsize, including part of last page */
    {
        write_size = first_size;
        last_data_addr = (unsigned char *)kmalloc(mtd->writesize, GFP_KERNEL);
    	if(!last_data_addr)
    	{
    		printk(KERN_ERR"get ram buffer failed!\n");
    		goto erro;
    	}
        malloc_flag = 1;
        memset(last_data_addr, 0xFF, mtd->writesize);
        memcpy(last_data_addr, (buf + first_size), len - first_size);
    }

    ret = mtd_write(mtd, to, write_size, &retlen, buf);
    if(ret || (write_size != retlen))
    {
		printk(KERN_ERR"mtd write failed, ret = %d, len = %d, retlen = %d!\n", ret, write_size, retlen);
		goto erro;
    }

    /* write last page, data and oob */
    ops.mode = MTD_OPS_AUTO_OOB;
	ops.len = mtd->writesize;
	ops.ooblen = YAFFS_BYTES_PER_SPARE;
    ops.ooboffs = 0;
	ops.datbuf = (unsigned char *)last_data_addr;
	ops.oobbuf = (unsigned char *)oob;

    ret = mtd->_write_oob(mtd, (to + first_size), &ops);
	if(ret)
    {
        printk(KERN_ERR"mtd write oob error, ret = %d.\n", ret);
        goto erro;
    }

    if(1 == malloc_flag)
    {
        kfree(last_data_addr);
    }

    return NANDC_OK;
erro:
    if((1 == malloc_flag) && (last_data_addr))
    {
        kfree(last_data_addr);
    }

    return ret;
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
unsigned int bsp_nand_read_flag_nv(const char *partition_name, unsigned int partition_offset, unsigned char *flag)
{
    unsigned int ret = NANDC_ERROR;
    struct mtd_oob_ops ops;
    unsigned char *page_buffer = NULL;
    struct mtd_info *mtd = get_mtd_device_nm(partition_name);
	if (IS_ERR(mtd))
    {
		printk(KERN_ERR"get_mtd_device_nm error\n");
		ret = (unsigned int)PTR_ERR(mtd);
		goto erro;
	}

    /* get buffer */
	page_buffer = (unsigned char *)kmalloc((mtd->writesize + YAFFS_BYTES_PER_SPARE), GFP_KERNEL);
    if(!page_buffer)
    {
        printk("ERROR: memory alloc failed.\n");
        goto erro;
    }

    /* read */
    ops.mode = MTD_OPS_AUTO_OOB;
	ops.len = mtd->writesize;
	ops.ooblen = YAFFS_BYTES_PER_SPARE;
    ops.ooboffs = 0;
	ops.datbuf = (unsigned char *)page_buffer;
	ops.oobbuf = (unsigned char *)(page_buffer + mtd->writesize);
    printk(KERN_DEBUG"[%s] flash read addr 0x%x\n", __FUNCTION__, partition_offset + mtd->erasesize - mtd->writesize);
    ret = (unsigned int)mtd->_read_oob(mtd, (long long)(partition_offset + mtd->erasesize - mtd->writesize), &ops);
	if(ret)
    {
        printk(KERN_ERR"mtd read oob error, ret = %d.\n", ret);
        goto erro;
    }
    printk(KERN_DEBUG"[%s] 0x%x, 0x%x\n", __FUNCTION__, (unsigned int)page_buffer , (unsigned int)(page_buffer + mtd->writesize));
    *flag = (*(unsigned char *)(page_buffer + mtd->writesize) == NV_WRITE_SUCCESS) ? NV_WRITE_SUCCESS : (~NV_WRITE_SUCCESS);
    printk(KERN_DEBUG"[%s] nv flag 0x%x\n", __FUNCTION__, *flag);
	kfree(page_buffer);

    return NANDC_OK;

erro:
    if(page_buffer)
    {
        kfree(page_buffer);
    }
    return ret;
}
#endif
/**
 * Name			:	bsp_nand_read
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset	- offset from partition to read from
 * 					@ptr_ram_addr  	- pointer to ram addr to store the data read from flash
 * 					@length        	- number of bytes to read
 *                  @skip_len       - bad block length skipped(Byte)
 * Return		:	0				- success
 *				:   else  			- failure
 * Desciption	:	NULL
 */
int bsp_nand_read(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length, u32 *skip_len)
{
	int ret = 0;
	u32 retlen = 0;
	u32 offsetinblock;
	u32 readsize;
	char * dst_addr = ptr_ram_addr;

    /* coverity[assign_zero] */
    struct mtd_info *mtd = NULL;

    if(skip_len)
    {
        *skip_len = 0;
    }

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

	while(length > 0)
	{
	    if(partition_offset >= mtd->size)
        {
            printk(KERN_ERR"ERROR: invalid partition offset 0x%x!\n", partition_offset);
            return EINVAL;
        }

        /* bad block management */
		ret = mtd_block_isbad(mtd, (long long)(partition_offset - partition_offset % mtd->erasesize));
		if (ret)
		{
			printk(KERN_ERR"bad block detected, skip. partition offset is 0x%x\n", partition_offset);
			goto skip;
		}

		/* read in block */
		offsetinblock = partition_offset % mtd->erasesize;
        readsize = (offsetinblock + length < mtd->erasesize) ? length : (mtd->erasesize - offsetinblock);
        ret = mtd_read(mtd, (long long)partition_offset, readsize, &retlen, (unsigned char*)dst_addr);
        if(ret)
        {
            printk(KERN_ERR"mtd read error, ret = 0x%x.\n", ret);
            goto erro;
        }
        partition_offset += readsize;
        length         -= readsize;
        dst_addr       += readsize;
        continue;
skip:
        if(skip_len)
        {
            *skip_len    += mtd->erasesize;
        }
        partition_offset += mtd->erasesize;
	} /* while(length > 0) */

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
        /* coverity[var_deref_model] */
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
int bsp_nand_erase(const char *partition_name, u32 partition_offset)
{
	struct erase_info instr; 
	int ret = 0;
    
    /* coverity[assign_zero] */
    struct mtd_info *mtd = NULL;

	if(!partition_name)
	{
		printk(KERN_ERR"param error.\n");
		goto out;
	}

	mtd = get_mtd_device_nm(partition_name);
	if (IS_ERR(mtd))
    {
		printk(KERN_ERR"get_mtd_device_nm error\n");
		ret = PTR_ERR(mtd);
		goto out;
	}

	instr.mtd = mtd;
	instr.addr = partition_offset - partition_offset % mtd->erasesize;
	instr.len = mtd->erasesize;
	instr.time = 1000;
	instr.retries = 2;
	instr.callback = NULL;
	instr.priv = 0;

	ret = mtd_erase(mtd, &instr);
	if (0 != ret)
	{
		printk(KERN_ERR"mtd nand_erase error\n");
		goto out;
	}

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        put_mtd_device(mtd);
    }

    return ret;
out:

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        /* coverity[var_deref_model] */
        put_mtd_device(mtd);
    }

    return ret;
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
s32 bsp_nand_write(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length)
{
    /* coverity[assign_zero] */
	long long addrblockalign;
	struct erase_info instr; 
    struct mtd_info *mtd = NULL;
	int ret = 0;
	u32 retlen = 0;
	u32 offsetinblock;
	u32 writesize;
    
	unsigned char *ram_addr = ptr_ram_addr;
	unsigned char *buffer = NULL;
    unsigned char *tmp_buf = NULL;
    
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

	while(length > 0)
	{
	    if(partition_offset >= mtd->size)
        {
            printk(KERN_ERR"ERROR: invalid partition offset 0x%x!\n", partition_offset);
            return EINVAL;
        }

		/* quary whether bad block */
		ret = mtd_block_isbad(mtd, (long long)(partition_offset - partition_offset % mtd->erasesize));
		if (ret)
		{
		    /* bad block, skip */
			printk(KERN_ERR"bad block detected, skip. partition offset is 0x%x\n", partition_offset);
            goto skip;
		}

		addrblockalign = (long long)(partition_offset / mtd->erasesize * mtd->erasesize); /*lint !e647*/

        /* not a whole block, need flash read and ddr copy */
        if((addrblockalign != partition_offset) || (length < mtd->erasesize))
        {
            /* get ram buffer */
            /* coverity[alloc_fn] */
        	/*在A核的后面没有连续的128K的内存来分配,因此用vmalloc来分配内存*/
        	tmp_buf = (unsigned char *)vmalloc(mtd->erasesize);
        	if(!tmp_buf)
        	{
        		printk(KERN_ERR"get ram buffer failed!\n");
        		goto erro;
        	}

    		/* read block into DDR */
    		ret = mtd_read(mtd, addrblockalign, mtd->erasesize, &retlen, (unsigned char*)tmp_buf);
    		if(ret)
            {
                ret = mtd_block_markbad(mtd, addrblockalign);
                if(ret)
            	{
            		printk(KERN_ERR"mtd mark block bad failed as reading!\n");
            		goto erro;
            	}
                /* bad block, skip */
                goto skip;
            }

            /* copy data to buffer */
    		offsetinblock = partition_offset % mtd->erasesize;
            writesize     = (offsetinblock + length < mtd->erasesize) ? length : (mtd->erasesize - offsetinblock);
            memcpy(tmp_buf + offsetinblock, ram_addr, writesize);
            buffer = tmp_buf;
        }
        else
        {
            writesize = mtd->erasesize;
            buffer = ram_addr;
        }

		/* erase a block */
		instr.mtd      = mtd;
		instr.addr     = (unsigned long long)addrblockalign;
		instr.len      = mtd->erasesize;
		instr.time     = 1000;
		instr.retries  = 2;
		instr.callback = NULL;
		instr.priv     = 0;

		ret = mtd_erase(mtd, &instr);
		if(ret)
        {
            /* erase fail, mark bad */
            ret = mtd_block_markbad(mtd, (long long)instr.addr);
            if(ret)
        	{
        		printk(KERN_ERR"mtd mark block bad failed as erasing!\n");
        		goto erro;
        	}
            /* bad block, skip */
            goto skip;
        }

        /* write data to flash */
        /* coverity[noescape] */
        ret = mtd_write(mtd, addrblockalign, mtd->erasesize, &retlen, (unsigned char*)buffer);
        if(ret)
        {
            /* write failed, mark bad */
            ret = mtd_block_markbad(mtd, addrblockalign);
            if(ret)
        	{
        		printk(KERN_ERR"mtd mark block bad failed as writing!\n");
        		goto erro;
        	}
            goto skip;
        }

        partition_offset += writesize;
        length         -= writesize;
        ram_addr       += writesize;
        continue;
skip:
        partition_offset += mtd->erasesize;
    } /* while(length > 0) */

    if(tmp_buf)
    {
        vfree(tmp_buf);
    }

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        /* coverity[var_deref_model] */
        put_mtd_device(mtd);
    }

    return 0;

erro:
    if(tmp_buf)
    {
        kfree(tmp_buf);
    }

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        /* coverity[var_deref_model] */
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
int bsp_nand_isbad(const char *partition_name, u32 partition_offset)
{
	int ret = -1;
	struct mtd_info *mtd = get_mtd_device_nm(partition_name);
	if (IS_ERR(mtd))
    {
		printk(KERN_ERR"get_mtd_device_nm error\n");
		ret = PTR_ERR(mtd);
		goto erro;
	}

	ret = mtd_block_isbad(mtd, (long long)partition_offset);

    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        put_mtd_device(mtd);
    }

    return ret;

erro:

    /*lint -save -e438 */
    /* release mtd device */
    if (!IS_ERR(mtd))
    {
        /* coverity[var_deref_model] */
        put_mtd_device(mtd);
    }

    return ret;
    /*lint -restore*/
}

#else /* __FASTBOOT__ */
#if 0
/**
 * Name			:	bsp_nand_blk_write_nv
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset   - offset from partition to read from, must be block aligned.
 * 					@flag        	- flag value, 1 or 0
 * Return		:	0 				- success
 *				:   else  			- failure
 * Description	: 	read nv_flag of a block
 */
u32 bsp_nand_read_flag_nv(const char *partition_name, u32 partition_offset, unsigned char *flag)
{
    u32 flash_addr;
    u32 ret = NANDC_ERROR;
    static unsigned char *buffer = NULL;
    struct nand_spec spec;
    struct ST_PART_TBL * ptable = find_partition_by_name((char *)partition_name);

    if(!ptable)
    {
        goto ERRO;
    }
    if(!flag)
    {
        cprintf("argu error.\n");
        goto ERRO;
    }

    ret = bsp_get_nand_info(&spec);
    if(ret)
    {
        goto ERRO;
    }

    if(!buffer)
    {
        buffer = (unsigned char *)himalloc(spec.pagesize + YAFFS_BYTES_PER_SPARE);
        if(!buffer)
        {
    		cprintf("get ram buffer failed!\n");
    		goto ERRO;
        }
    }

    memset(buffer, 0xFF, spec.pagesize + YAFFS_BYTES_PER_SPARE);

    flash_addr = ptable->offset + partition_offset;
    ret = nand_read_oob((flash_addr + spec.blocksize - spec.pagesize),
        (unsigned int)buffer, (spec.pagesize + YAFFS_BYTES_PER_SPARE),YAFFS_BYTES_PER_SPARE ,NULL);
    if(ret)
    {
		cprintf("nand read oob failed!\n");
		goto ERRO;
    }
    *flag = (*(buffer + spec.pagesize) == NV_WRITE_SUCCESS) ? NV_WRITE_SUCCESS : (~NV_WRITE_SUCCESS);

    return NANDC_OK;
ERRO:
    return ret;
}
#endif

/**
 * Name			:	bsp_nand_read
 * Arguments	:	@partition_name  	- partition name
 * 					@partition_offset	- offset from partition to read from
 * 					@ptr_ram_addr  	- pointer to ram addr to store the data read from flash
 * 					@length        	- number of bytes to read
 *                  @skip_len       - bad block length skipped(Byte)
 * Return		:	0				- success
 *				:   else  			- failure
 * Desciption	:	NULL
 */
int bsp_nand_read(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length, u32 *skip_len)
{
    u32 flash_addr;
    u32 ret = NANDC_ERROR;
    struct ST_PART_TBL * ptable = find_partition_by_name(partition_name);

    if(!ptable)
    {
        goto ERRO;
    }

    flash_addr = ptable->offset + partition_offset;
	return nand_read(flash_addr, (u32)ptr_ram_addr, length, skip_len);
ERRO:
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
int bsp_nand_erase(const char *partition_name, u32 partition_offset)
{
    u32 flash_addr;
    u32 block_id, bad_flag;
    u32 ret = NANDC_ERROR;
	struct nandc_host  * host   = NULL;
    struct ST_PART_TBL * ptable = find_partition_by_name(partition_name);

    if(!ptable)
    {
        goto ERRO;
    }

    /* get the flash address */
    flash_addr = ptable->offset + partition_offset;

    host = nandc_nand_host;
    if(!host)
    {
        cprintf("ERROR: function %s, line %d\n", __FUNCTION__, __LINE__);
        goto ERRO;
    }

    /* get the block id and check bad */
    block_id = flash_addr / host->nandchip->spec.blocksize;
    ret = nand_isbad(block_id, &bad_flag);
    if(ret)
    {
        cprintf("ERROR: nand quary bad failed, function %s, line %d\n", __FUNCTION__, __LINE__);
        goto ERRO;
    }

    if(NANDC_BAD_BLOCK == bad_flag)
    {
        cprintf("ERROR: try to erase a bad block, function %s, line %d\n", __FUNCTION__, __LINE__);
        goto ERRO;
    }

	return nand_erase(block_id);

ERRO:
    return ret;
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
s32 bsp_nand_write(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length)
{
    u32 flash_addr, ret = NANDC_ERROR;
    struct ST_PART_TBL * ptable = find_partition_by_name(partition_name);

    if(!ptable)
    {
        goto ERRO;
    }

    /* get the flash address */
    flash_addr = ptable->offset + partition_offset;
	return nand_write(flash_addr, (u32)ptr_ram_addr, length, NULL);

 ERRO:
    return ret;
}

/**
 * Name			: 	bsp_nand_isbad
 * Arguments	: 	@partition_name  	- partition name
 * 					@partition_offset	- block offset from partition to check
 *
 * Return		:	0 				- good block
 *				:   1  			- bad block
 *              :   else            - error
 * Description	: 	check whether a block is bad
 */
int bsp_nand_isbad(const char *partition_name, u32 partition_offset)
{
    u32 flash_addr;
    u32 bad_flag;
    u32 ret = NANDC_ERROR;
	struct nandc_host  * host   = NULL;
    struct ST_PART_TBL * ptable = find_partition_by_name(partition_name);

    if(!ptable)
    {
        goto ERRO;
    }

    /* get the flash address */
    flash_addr = ptable->offset + partition_offset;

    host = nandc_nand_host;
    if(!host)
    {
        cprintf("ERROR: function %s, line %d\n", __FUNCTION__, __LINE__);
        goto ERRO;
    }

	ret = nand_isbad(flash_addr / host->nandchip->spec.blocksize, &bad_flag);
    if(ret == 1)
    {
        return -6;
    }
    else if(ret > 1)
    {
        return ret;
    }
    else
    {
        /* nop */
    }

    if(bad_flag == 1)  /* bad block */
    {
        return 1;
    }
    else               /* good block */
    {
        return 0;
    }

ERRO:
    return ret;
}

/**
 * Name			: 	bsp_update_ptable_to_nandc
 * Arguments	: 	null
 *
 * Return		:	null
 * Description	: 	update ptable to nandc
 */
void bsp_update_ptable_to_nandc(void)
{
    nandc_init_mtd_partition(nandc_nand_host);
}


/**
 * Name			:	bsp_get_nand_info
 * Arguments	:	@spec  		    - to store flash info
 * Return		:	0 				- success
 *				:   else  			- failure
 * Desciption	:	the ecctype info is the type used by nandc, not nand flash itself recommend.
 */
int bsp_get_nand_info(struct nand_spec *spec)
{
	struct nandc_host 	*host 		= nandc_nand_host;
    if(!host)
    {
        cprintf("ERROR: get nand info error.\n");
        goto erro;
    }

    if(!nand_isinit())
    {
        cprintf("ERROR: nandc driver not init yet!\n");
        goto erro;
    }

	spec->pagesize 	 		= host->nandchip->spec.pagesize;
	spec->pagenumperblock	= (host->nandchip->spec.blocksize / spec->pagesize);
	spec->addrnum      		= host->nandchip->spec.addrcycle;
	spec->ecctype 	 		= host->nandchip->ecctype;
	spec->buswidth   		= host->nandchip->spec.buswidth;
    spec->blocksize         = host->nandchip->spec.blocksize;
    spec->sparesize         = host->nandchip->spec.sparesize;
    spec->chipsize          = host->nandchip->spec.chipsize;

	return NANDC_OK;
erro:
	return NANDC_ERROR;
}

/******************************************************************************************
 * FUNC NAME:
 * @bsp_update_size_of_lastpart() - external API:
 *
 * PARAMETER:
 * @new_ptable - addr of new ptable
 *
 * RETURN:
 *    null
 *
 * DESCRIPTION:
 *    update the size of last partition to shared memory
 *    分区表最后一个分区将flash最后一块空间完全占有,从而导致静态编译的ptable.bin与软件实际使用的ptable不一样，
 *    升级分区表的时候就认为两个分区不一样，从而导致对yaffs分区的擦除。此处用实际使用的分区表最后一个分区大小
 *    覆盖新ptable最后分区大小
 *
 * CALL FUNC:
 *
 *****************************************************************************************/
void bsp_update_size_of_lastpart(struct ST_PART_TBL *new_ptable)
{
    struct ST_PART_TBL * old_ptable = NULL;    
    if(!new_ptable)
    {
        cprintf("[%s]argu error\n", __FUNCTION__);
        return;
    }

    /* find the partition */
    while(0 != strcmp(PTABLE_END_STR, new_ptable->name))
    {
        new_ptable++;
    }

    new_ptable--;
   
    old_ptable = find_partition_by_name((const char*)new_ptable->name);
    if(old_ptable)
    {
        new_ptable->capacity = old_ptable->capacity;
    }
    
}

/******************************************************************************************
 * FUNC NAME:
 * @bsp_erase_yaffs_partitons() - external API:
 *
 * PARAMETER:
 * @none
 *
 * RETURN:
 *    null
 *
 * DESCRIPTION:
 *    erase all yaffs partitions
 *
 * CALL FUNC:
 *
 *****************************************************************************************/
void bsp_erase_yaffs_partitons(void)
{
    struct ST_PART_TBL * ptable = NULL;
    struct ptentry ptn;
    int ret = NANDC_ERROR;

    ptable_ensure();

    ptable = ptable_get_ram_data();

    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(ptable->property & DATA_YAFFS)
        {
            /* coverity[buffer_size_warning] */
            strncpy(ptn.name, ptable->name, 16);
            ptn.start = ptable->offset;
            ptn.length = ptable->capacity;
            cprintf("earsing %s, start 0x%x, length 0x%x\n", ptn.name, ptn.start, ptn.length);
            ret = flash_erase(&ptn);
            if(ret)
            {
                cprintf("[%s] ERROR: erase %s failed, ret = %d\n", __FUNCTION__, ptn.name, ret);
                return;
            }
        }
        ptable++;
    }
}

/**
 * Name			: 	str2ul
 * Arguments	: 	@str - source string
 *              :   @digit - to store dest digit
 *
 * Return		:	0 - success; 1 - failed
 * Description	: 	mark a block bad
 */
unsigned int str2ul(char *str, unsigned int *digit)
{
    int ret = NANDC_ERROR;
    unsigned int tmp_digit = 0;
    char *temp = str;

    /* reject null */
    if(!str || !digit)
    {
        cprintf("[%s]ERROR: input is NULL.\n", __FUNCTION__);
        goto ERRO;
    }

    /* string to digit */
    while(*temp != 0)
    {
        if(*temp < '0' || *temp > '9')
        {
            cprintf("[%s]ERROR: string input is illegal.\n", __FUNCTION__);
            goto ERRO;
        }
        tmp_digit = tmp_digit * 10 + (*temp - '0');
        temp++;
    }

    *digit = tmp_digit;

    /* return success */
    return NANDC_OK;

ERRO:
    return ret;
}

/**
 * Name			: 	bsp_nand_argument_check
 * Arguments	: 	@argu: string input to check
 *
 * Return		:	null
 * Description	: 	check whether partition name is legal.
 */
char *bsp_nand_argument_check(char *argu)
{
    char *temp;

    /* ignore while space at the start position*/
    while(*argu == ' ')
    {
        argu++;
    }

    /* if partition name is NULL, error. ps: 0x0D is '\n' */
    if(*argu == 0x0D)
    {
        return NULL;
    }

    /* must end with 0 */
    for(temp = argu; (*temp != ' ') && (*temp != 0x0D); temp++)
    {
        /* NOP */
    }
    *temp = 0;
    return argu;

}

/**
 * Name			: 	bsp_nand_markbad
 * Arguments	: 	@blockid - block id
 *
 * Return		:	null
 * Description	: 	mark a block bad
 */
void bsp_nand_markbad(char *blockid)
{
    int ret = NANDC_ERROR;
    unsigned int id, bad_flag;
    char *blk_id = bsp_nand_argument_check(blockid);

    /* string to int  */
    ret= str2ul(blk_id, &id);
    if(ret)
    {
        cprintf("ERROR: string to integer failed, ret = 0x%x.\n", ret);
        goto EXIT;
    }

    /* check block bad. if good, mark bad; else return */
    ret = nand_isbad(id, &bad_flag);
    if(ret)
    {
        cprintf("[%s]ERROR: nand check bad failed, ret = 0x%x.\n", __FUNCTION__, ret);
        goto EXIT;
    }

    if(NANDC_GOOD_BLOCK == bad_flag)
    {
        ret = nand_bad(id);
        if(ret)
        {
            cprintf("[%s]ERROR: nand mark bad failed, ret = 0x%x.\n", __FUNCTION__, ret);
            goto EXIT;
        }
    }
    else
    {
        cprintf("[%s]WARNING: block 0x%x is already bad.\n", __FUNCTION__);
        goto EXIT;
    }

    /* exit */
    return;

EXIT:
    return;
}

/**
 * Name			: 	bsp_nand_erase_force
 * Arguments	: 	@blockid - block id
 *
 * Return		:	null
 * Description	: 	erase a block force whether it is bad
 */
void bsp_nand_erase_force(char *blockid)
{
    int ret = NANDC_ERROR;
    unsigned int id;

    char *blk_id = bsp_nand_argument_check(blockid);

    /* string to int  */
    ret= str2ul(blk_id, &id);
    if(ret)
    {
        cprintf("[%s]ERROR: string to integer failed, ret = 0x%x.\n", __FUNCTION__, ret);
        goto ERRO;
    }

    /* erase whether it is bad or good */
    ret = nand_erase(id);
    if(ret)
    {
        cprintf("[%s]ERROR: nand check bad failed, ret = 0x%x.\n", __FUNCTION__, ret);
        goto ERRO;
    }
    else
    {
        cprintf("SUCCESS: erase block %x forcely.\n", id);
    }

    /* exit */
    return;

ERRO:
    return;
}

#endif /* #ifdef __KERNEL__ */

#ifdef __cplusplus
}
#endif

