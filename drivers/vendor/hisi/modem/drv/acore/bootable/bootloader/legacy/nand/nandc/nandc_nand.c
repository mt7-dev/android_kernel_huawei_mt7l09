/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
*
* Filename:    nandc_nand.c
* Description: nandc模块的实现读、写数据、擦除功能、标坏块都在此文件中实现
*
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*
*    All rights reserved.
*
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __FASTBOOT__

/*lint -save -e506 */
#include "nandc_inc.h"
#include "nandc_balong.h"
#include "hi_base.h"
#include "hi_syscrg_interface.h"
#include "hi_syssc_interface.h"
#include <bsp_shared_ddr.h>
#include <bsp_nandc.h>
extern u32 ptable_parse_mtd_partitions(struct mtd_partition** mtd_parts, u32 *nr_parts);
extern u32  nandc_host_set_partition(struct nandc_host * host, struct mtd_partition* ptable, u32 nr_parts);
/*lint -save -e767*/
#define NFCDBGLVL(LVL)      (NANDC_TRACE_NAND|NANDC_TRACE_##LVL)
/*lint -restore*/
/*全局指针变量*/
struct nandc_host*  nandc_nand_host = NANDC_NULL;



/**
* 作用:把硬件缓冲区中的OOB数据复制到用户内存中去
*
* 参数:
* @host                  ---nandc_host的结构体指针
* @oobdst                ---用户存放OOB数据的内存地址
* @oobsrc                ---硬件缓冲区中的OOB数据的地址
* @len                   ---复制的长度
* 描述:因为在硬件中的OOB的数据格式和用户需要的OOB数据格式不一样,因此需要一个格式转换的复制操作
*/
__inline u32 nandc_nand_transfer_oob(struct nandc_host* host, u8 *oobdst,  u8 *oobsrc, u32 len)
{
    struct nand_oobfree *free  ;    /*lint !e578 */
    u32 boffs = 0;
    u32 bytes = 0;

    free =  host->curpart->ecclayout->oobfree;

    for(; free->length && len; free++, len -= bytes)
    {
        bytes = min_t(u32, len, free->length);
        boffs = free->offset;

        memcpy((void*)oobdst, (const void*)(oobsrc + boffs), bytes);
        oobdst += bytes;
    }

    return NANDC_OK;
}


/**
* 作用:用户写OOB的数据到nandc控制器的buf中去
*
* 参数:
* @host                  ---nandc_host的结构体指针
* @oobdst                ---硬件缓冲区中的OOB数据的地址
* @oobsrc                ---用户存放OOB数据的内存地址
* @len                   ---复制的长度
* 描述:因为在硬件中的OOB的数据格式和用户需要的OOB数据格式不一样,因此需要一个格式转换的复制操作
*/
__inline u32 nandc_nand_fill_oob(struct nandc_host* host, u8 *oobdst,  u8 *oobsrc, u32 len)
{
    struct nand_oobfree *free  ;    /*lint !e578 */
    u32 boffs = 0;
    u32 bytes = 0;

    free =  host->curpart->ecclayout->oobfree;

    for(; free->length && len; free++, len -= bytes)
    {
        bytes = min_t(u32, len, free->length);
        boffs = free->offset;

        memcpy((void *)((u32)oobdst + boffs), (const void*)oobsrc , bytes);
        oobsrc += bytes;
    }

    return NANDC_OK;
}

/**
* 作用:nandc模块设置处理的过程
*
* 参数:
* @host                  ---nandc_host的结构体指针
* @access                ---读写的标志值
* 描述:设置nandc模块的处理过程
*/
__inline void nandc_nand_set_latch(struct nandc_host* host, u32 access)
{
	/*lint !e578 */
	/*读操作*/
	if(NAND_CMD_READSTART == access)
	{
		if(nandc_read_latch_done != host->latched)
		{
			host->latched = nandc_read_latch_start;
		}
	}
	/*写操作*/
	else if(NAND_CMD_PAGEPROG == access)
	{
		if(nandc_write_latch_done != host->latched)
		{
			host->latched = nandc_write_latch_start;
		}
	}
}

/**
* 作用:nandc模块清除处理的过程
*
* 参数:
* @host                  ---nandc_host的结构体指针
* 描述:清除nandc模块的处理过程
*/
__inline void nandc_nand_clear_latch(struct nandc_host* host)
{
    host->latched = nandc_latch_none;
}

/**
* 作用:nandc模块Flash读写地址转换的过程
*
* 参数:
* @host                  ---nandc_host的结构体指针
* 描述:nandc模块的读写Flash地址的转换函数
*/
__inline u32 nandc_nand_set_address(struct nandc_host *host)
{
    struct nandc_spec* spec = &host->nandchip->spec;
    u32 page_offset, column_offset;
	u32 column_addr;

    if(NANDC_ADDR_INVALID == host->addr_real)
    {
		host->addr_real = 0;
    }
    else
    {
        /*把地址转换为页地址和页内地址*/
        column_addr= (u32)host->addr_real & (spec->offinpage);

        if(NAND_BUSWIDTH_16 == spec->buswidth)
        {
           column_offset = column_addr >> 1;
        }
        else
        {
           column_offset = column_addr;
        }

        page_offset = (u32)(host->addr_real >> spec->pageshift);
		/*转换为要写入到寄存器的地址值*/
        host->addr_physics[0] = column_offset;
        host->addr_physics[0] |= page_offset << NANDC_COLUMN_SHIFT;

        host->addr_physics[1] = page_offset >> NANDC_COLUMN_SHIFT;

    }

    NANDC_DO_ASSERT(((u32)host->addr_real < (u32)spec->chipsize), "virtual address too large!", (u32)host->addr_real);

    return NANDC_OK;
}


/**
* 作用:nandc模块Flash读、写、擦除的寄存器实现过程
*
* 参数:
* @host                  ---nandc_host的结构体指针
* @databuff              ---数据地址
* @oobbuff               ---OOB地址
* @datalen               ---数据长度
* @ooblen                ---OOB的长度
* @access                ---读、写、擦除命令
* 描述:nandc模块的读写Flash地址的转换函数
* 注意:
* < ECC mode select by parameter combination: >
*-------------------------------------------------------------------------------------------------
*  | @databuff    |  @oobbuff  |  @datalen | @ooblen  |   ECC mode
*  ------------------------------------------------------------------------------------------------
*  |    YES       |    ANY     |  YES      |  ANY     |   no ECC random
*  ------------------------------------------------------------------------------------------------
*  |    YES       |    NULL    |  NULL     |  ANY     |   data only with ECC
*  ------------------------------------------------------------------------------------------------
*  |    YES       |    YES     |  NULL     |  YES     |   data and spare with ECC
*  ------------------------------------------------------------------------------------------------
*  |    NULL      |    YES     |  NULL     |  YES     |   spare only with ECC
*  ------------------------------------------------------------------------------------------------
*  |  O T H E R S                                     |   INVALID
*  ------------------------------------------------------------------------------------------------
*/
__inline u32 nandc_nand_access(struct nandc_host* host, u8* databuff, u8* oobbuff, u32 datalen, u32 ooblen, u32 access)
{
	/*lint !e578 */
    u32 errorcode = NANDC_ERROR;
	/*设置要转换的地址信息*/
    if(NANDC_OK != nandc_nand_set_address(host))
    {
        goto ERRO;
    }
	/*读写擦除操作的通用处理流程*/
    switch(access)
    {
        case NAND_CMD_PAGEPROG:
        case NAND_CMD_READSTART:
        {
            NANDC_REJECT_NULL((u32)databuff|(u32)oobbuff);
			/*这里是使能ECC功能的读写数据操作*/
            if(0 == datalen)
            {
            	/*ecc mode,read whole page data and oob data*/
                host->databuf   = databuff;
#ifdef NANDC_USE_DMA_ENABLE
				host->options   =   NANDC_OPTION_DMA_ENABLE;
#else
				host->options   =   0 ;
#endif
				/*不直接把host>oobbuf设置为oobbuf的原因是因为数据的排列是不一样的*/
				NANDC_REJECT_NULL(host->buffers);
				host->oobbuf    = host->buffers->ecccode;

                if(host->addr_physics[0] & 0xFFFF)
                {
                    errorcode = NANDC_ERROR;
                    NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_access: invalide ecc access address:(%d) \n", host->addr_physics[0]));
                    goto ERRO;
                }
            }
            else
            {   /*no ecc mode, raw data in one page*/
                host->databuf   =   databuff;
				host->oobbuf	=	oobbuff;

                host->options   =   NANDC_OPTION_ECC_FORBID;             /*use default ecc type */
#ifdef NANDC_USE_DMA_ENABLE
				host->options   |=  NANDC_OPTION_DMA_ENABLE ;
#endif
                host->length    =   datalen;
            }
			/*如果是写操作且有要写的OOB数据那么就写OOB数据*/
            if((NAND_CMD_PAGEPROG == access)&&(NANDC_NULL != oobbuff))
            {
                nandc_nand_fill_oob(host, host->oobbuf, oobbuff, ooblen);
            }
        }
        break ;
        case NAND_CMD_ERASE2:
        {
			/*要擦除的页地址*/
            host->addr_physics[0]   = (host->addr_physics[0] >> NANDC_COLUMN_SHIFT) | (host->addr_physics[1] << NANDC_COLUMN_SHIFT);
        }
        break;
        default:
			errorcode = NANDC_ERROR;
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_access: invalide access type:(%d) \n", access));
            goto EXIT;
    }

    host->command = access;
    errorcode = nandc_ctrl_entry(host);
    if(NANDC_OK !=  errorcode)
    {
        goto EXIT;
    }
	/*如果是读操作且要读OOB的数据那么就把OOB的数据读出来*/
    if((NAND_CMD_READSTART == access)&&(NANDC_NULL != oobbuff))
    {
        nandc_nand_transfer_oob(host, oobbuff, host->oobbuf, ooblen);
    }

EXIT:
    return errorcode;

ERRO:
    return NANDC_ERROR;
}


/**
* 作用:nandc模块不使能ECC功能的裸读操作
*
* 参数:
* @pagenum                ---Flash页号
* @offsetinpage           ---Flash的页内偏移地址
* @dst_addr               ---存放数据的目的地址
* @readsize               ---读的数据长度
* 描述:nandc模块不使能ECC的裸读操作
*/
static u32 nandc_nand_read_raw(u32 pagenum, u32 offsetinpage, u32 dst_addr, u32 readsize)
{
    struct nandc_host* host = nandc_nand_host ;
    struct nandc_spec* spec;
    int ret = NANDC_ERROR;

    spec = &host->nandchip->spec ;
	/*要读的Flash地址转换*/
    host->addr_real= (pagenum & spec->pagemask) << spec->pageshift;

    host->chipselect = pagenum >> (spec->chipshift - spec->pageshift);
	/*把整页的pagedata+sparesize数据一起读到此buffer中来*/
    if(NANDC_NULL == host->buffers)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("read raw error!! host->buffers is NULL \n"));
		goto ERRO;
    }
	if((offsetinpage + readsize) > (spec->pagesize + spec->sparesize))
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("read raw error!! offsetinpage 0x%x,readsize 0x%x \n",offsetinpage,readsize));
		goto ERRO;
    }
	/*不使能ECC功能的裸读数据操作,注意第4个参数不为0为pagesize+sparesize*/
    ret = nandc_nand_access(host, host->buffers->databuf, host->buffers->databuf + spec->pagesize, spec->pagesize + spec->sparesize, 0, NAND_CMD_READSTART);
    if(NANDC_OK == ret)
    {
    	/*再把读出来的数据复制到用户内存中去*/
        memcpy((void*)dst_addr, host->buffers->databuf + offsetinpage,  readsize);
    }
    else
    {
         goto ERRO;
    }
    return NANDC_OK;
ERRO:
    return ret;
}



/**
* 作用:nandc模块不使能ECC功能的读整个页面的数据,主要是提供给usbload.c来使用
*
* 参数:
* @flash_addr             ---Flash地址
* @dst_addr               ---存放数据的目的地址
* @readsize               ---读的数据长度
* 描述:主要是给在pc机上通过fastboot.exe来保存一整页的裸数据
*/
static u32 nandc_read_raw_wholepage(FSZ flash_addr, u32 dst_data, u32 read_size)
{
    struct nandc_host* host = nandc_nand_host ;
    struct nandc_spec* spec;
    int ret = NANDC_ERROR;
    u32 pageoffset=0,pagenum=0;
    u32 raw_whole_pagesize=0;

    spec = &host->nandchip->spec ;
    pageoffset  =   (u32)(flash_addr & spec->offinpage);
    pagenum     =   flash_addr >> spec->pageshift;
    raw_whole_pagesize = spec->pagesize + spec->sparesize;
	if(( pageoffset != 0 ) ||(read_size != raw_whole_pagesize))
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("read raw error!! flash_addr 0x%x,read_size 0x%x \n",flash_addr,read_size));
		goto ERRO;
    }
    return nandc_nand_read_raw(pagenum,0,dst_data,read_size);
ERRO:
    return ret;
}



/**
* 作用:nandc模块使能ECC功能的读整页的数据
*
* 参数:
* @address                ---Flash地址
* @databuff           	  ---读数据存放的内存地址
* @oobbuff                ---OOB数据存放的内存地址
* @oobsize                ---要读的OOB数据长度
* 描述:nandc模块使能ECC的读整页数据，通过OOBBUFF来决定是否要读OOB数据
*/
static u32 nandc_nand_read_page(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize)
{
    struct nandc_host* host = nandc_nand_host ;

    NANDC_REJECT_NULL(host);
	/*设置地址信息*/
    if(NANDC_OK != nandc_native_location_by_address( host, address))
    {
        return NANDC_E_NOFIND;
    }

    host->addr_real &= host->nandchip->spec.pagealign;
	/*使能ECC功能的读一页数据,注意第4个参数是为0*/
    return nandc_nand_access(host, databuff,  oobbuff, 0, oobsize, NAND_CMD_READSTART);

ERRO:
	return NANDC_E_NULL;
}

/**
* 作用:nandc模块使能ECC功能如果有ecc error时也会把整个页面的数据读出来,主要是提供给usbload.c来使用
*
* 参数:
* @flash_addr             ---Flash地址
* @dst_addr               ---存放数据的目的地址
* @readsize               ---读的数据长度
* 描述:主要是给在pc机上通过fastboot.exe来保存一整页的数据
*/
static u32 nandc_read_force_wholepage(FSZ flash_addr, u32 dst_data, u32 read_size)
{
    struct nandc_host* host = nandc_nand_host ;
    struct nandc_spec* spec;
    u32 ret = NANDC_ERROR;
    u32 pageoffset=0;
    u32 whole_pagesize=0;

    spec = &host->nandchip->spec ;
    pageoffset  =   (u32)(flash_addr & spec->offinpage);
    whole_pagesize = spec->pagesize;
    if((pageoffset != 0 ) ||(read_size != whole_pagesize))
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("read raw error!! flash_addr 0x%x,read_size 0x%x \n",flash_addr,read_size));
        goto EXIT;
    }
    host->force_readdata =1;
    ret = nandc_nand_read_page(flash_addr,(u8*)dst_data,NULL,0);
    host->force_readdata =0;
EXIT:
    return ret;
}


/**
* 作用:nandc模块提供给usbloader.c的接口，当有ecc error时可以通过fastboot.exe在pc机上dump flash的数据
*
* 参数:
* @flash_addr             ---Flash地址
* @dst_addr               ---存放数据的目的地址
* @readsize               ---读的数据长度
* @ecc_enable             ---ecc使能
* 描述:主要是给在pc机上通过fastboot.exe来保存一整页的数据
*/
u32 nand_read_wholepage_usbloader(FSZ flash_addr, u32 dst_data, u32 read_size,unsigned int ecc_enable)
{
    if(ecc_enable==1)
    {
        return nandc_read_force_wholepage(flash_addr,dst_data,read_size);
    }
    else if(ecc_enable==0)
    {
        return nandc_read_raw_wholepage(flash_addr,dst_data,read_size);
    }
    /* coverity[printf_arg_mismatch] */
    NANDC_TRACE(NFCDBGLVL(ERRO), ("nand_read_wholepage_usbloaderw error!! ecc_enable 0x%x \n",ecc_enable));
    return NANDC_ERROR;
}

/**
* 作用:nandc模块通过块号和页地址来设置要读写Flash的地址信息
*
* 参数:
* @host                   ---nandc_host的结构体指针
* @blk_id           	  ---要读的Flash的块号
* @pageoffset             ---要读此blk_id的块内页面地址
*/
static __inline u32 nandc_nand_flag_address(struct nandc_host* host, u32 blk_id, u32 pageoffset)
{
    u32 flagoffset;
	/*设置块地址*/
    if(NANDC_OK != nandc_native_location_by_blocknum( host, blk_id))
    {
        goto ERRO;
    }
	/*设置块内页面地址*/
    host->addr_real +=  pageoffset << host->nandchip->spec.pageshift;

    if(NANDC_OK != nandc_nand_set_address(host))
    {
        goto ERRO;
    }
	/*spare区的坏块管理的偏移地址*/
    flagoffset = host->badoffset;
    /* coverity[printf_arg_mismatch] */
	NANDC_TRACE(NFCDBGLVL(NORMAL),("blk_id:0x%x,pageoffset:0x%x\n",blk_id,pageoffset));

    if(NAND_BUSWIDTH_16 == host->nandchip->spec.buswidth)
    {
        host->addr_physics[0] += flagoffset>>1;
    }
    else
    {
        host->addr_physics[0] += flagoffset;
    }
    host->addr_real = NANDC_ADDR_INVALID;
    host->addr_physics[0] &= 0xFFFF0000;

    return NANDC_OK;

ERRO:
    return NANDC_ERROR;
}


/**
* 作用:nandc模块通过块号和块内的页偏移地址来读OOB数据
*
* 参数:
* @blk_id                   ---Flash块号
* @pageoffset           	---块内的页偏移地址
* @oobbuf             		---OOB存放的内存地址
* @size             		---要读的OOB的大小
*/
u32 nandc_nand_quary_flag(u32 blk_id, u32 pageoffset, u8* oobbuf ,u32 size)
{
    struct nandc_host* host = nandc_nand_host ;

    NANDC_REJECT_NULL(host);

	struct nandc_spec	*spec 	 = &(host->nandchip->spec);
	u32 sparesize 	= spec->sparesize;
	u32 pagesize  	= spec->pagesize;
    u32 errcode     = NANDC_ERROR;
	u8 *databuf 	= host->buffers->databuf;
	/*设置地址*/
    if(NANDC_OK ==nandc_nand_flag_address(host, blk_id, pageoffset))
    {
    	/*不使能ECC功能的读Flash中的数据*/
    	errcode = nandc_nand_access(host, (u8*)databuf, (u8*)(databuf + pagesize), pagesize + sparesize, 0, NAND_CMD_READSTART);
        if(NANDC_OK == errcode)
        {
        	/*把OOB的数据复制出来*/
            memcpy((void*)oobbuf, (const void*)(databuf + pagesize), size);
            return errcode;
        }
        else
        {
            goto ERRO;
        }
    }

 ERRO:
     return NANDC_ERROR;
}


/**
* 作用:nandc模块通过块号来查询此块是否是坏块
*
* 参数:
* @blk_id                   ---Flash块号
* @isbad           	        ---存放坏块标志的地址
*/
static u32 nandc_nand_quary_bad(u32 blk_id, u32 * isbad )
{
    u8 badflag[NANDC_BADBLOCK_FLAG_SIZE] = {0};
    u32 errcode = NANDC_ERROR, cnt;
    struct nandc_host *host = nandc_nand_host;

    /*根据标志位来查询坏块标志位*/
	if (host->bbt_options & NAND_BBT_SCANLASTPAGE)
    {
        /*把最后一页的坏块标记读出来*/
        errcode = nandc_nand_quary_flag(blk_id, (host->nandchip->spec.blocksize / host->nandchip->spec.pagesize - 1),
            badflag, sizeof(badflag));
        if(NANDC_OK !=  errcode)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("quary last page failed, ret = 0x%x\n", errcode));
            goto ERRO;
        }
		/*不为全FF的话就是坏块*/
        if((0xFF != badflag[0]))
        {
            *isbad = NANDC_BAD_BLOCK;
        }
        else
        {
            *isbad = NANDC_GOOD_BLOCK;
        }

    }
	/*查询第一页和第二页是否有坏块标记*/
    else if(host->bbt_options & NAND_BBT_SCAN2NDPAGE)
    {
    	/*查询第一页和第二页的坏块标记*/
        errcode = nandc_nand_quary_flag(blk_id, NANDC_BAD_FLAG_FIRSTPAGE_OFFSET,  badflag, sizeof(badflag));
        if(NANDC_OK !=  errcode)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("quary page0 failed, ret = 0x%x\n", errcode));
            goto ERRO;
        }
        if((0xFF != badflag[0]))        /* bad block, return directly */
        {
            *isbad = NANDC_BAD_BLOCK;
            return errcode;
        }

        errcode = nandc_nand_quary_flag(blk_id, NANDC_BAD_FLAG_SECONDPAGE_OFFSET,  badflag, sizeof(badflag));
        if(NANDC_OK !=  errcode)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("quary last page failed, ret = 0x%x\n", errcode));
            goto ERRO;
        }
        if((0xFF != badflag[0]))
        {
            *isbad = NANDC_BAD_BLOCK;
        }
        else
        {
            *isbad = NANDC_GOOD_BLOCK;
        }
    }
    else
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_quary_bad: unknown bad block offset, host->bbt_options = 0x%x\n",
            host->bbt_options));
        goto ERRO;
    }

    return errcode;

ERRO:
	for(cnt=0;cnt<NANDC_BADBLOCK_FLAG_SIZE;cnt++)
	{
		NANDC_TRACE(NFCDBGLVL(NORMAL),("badflag[%d]=0x%x\n",cnt,badflag[cnt]));
	}
    return errcode;
}

/**
* 作用:nandc模块使能ECC功能的读部分页内数据
*
* 参数:
* @host                 ---nandc_host的结构体指针
* @pageoffset           ---要读的Flash的页内偏移地址
* @dstaddr              ---存放读的数据的内存地址
* @pageleft             ---要读数据长度
*/
static __inline u32 nandc_nand_read_inpage(struct nandc_host* host, u32 pageoffset, u32 dstaddr, u32 pageleft)
{
    u8* pagebuf     =   host->buffers->databuf;
    u32 pagealign   =   host->nandchip->spec.pagealign;
	/*在调用此函数前已经把要读的Flash地址设置了*/
    if (NANDC_OK != nandc_bbm_real_addr((host->addr_logic & pagealign), &host->addr_real))
    {
        goto ERRO;
    }
	/*使能ECC功能的读整页的数据*/
    if(NANDC_OK !=  nandc_nand_access( host, (u8 *)pagebuf, NANDC_NULL, 0, 0, NAND_CMD_READSTART))
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("error read in page ,chip:%d addrglob:0x%x\n",
        (u32)host->chipselect, (u32)host->addr_real));
		goto ERRO;
    }
	/*复制部分页内数据到目的内存*/
    memcpy((void*)dstaddr, pagebuf + pageoffset, pageleft);

    return  NANDC_OK;

ERRO:

    return NANDC_ERROR;

}


/**
* 作用:nandc模块使能ECC功能的读多页数据
*
* 参数:
* @flashaddr                 ---要读的Flash地址
* @dstaddr           		 ---存放要读的数据的内存地址
* @readsize                  ---读数据的长度，可以超过一个page的长度
* @oobsize             		 ---读一页数据要读的OOB长度
* @skiplength                ---过滤掉的数据长度
*/
static u32 nandc_nand_mass_read(FSZ flashaddr, u32 dstaddr, u32 readsize, u32 oobsize, u32 *skiplength)
{
    struct mtd_partition    *thispart   = NANDC_NULL;
    struct nandc_host       *host       = nandc_nand_host ;
    struct nandc_spec       *spec       = NANDC_NULL;
    FSZ addrglob;
    u32 sizeleft , blockleft,blockoffset, pageleft, pageoffset, badflag , skips = 0;
    u8* oobbuff;
    u32 errorcode = NANDC_ERROR;

    NANDC_REJECT_NULL(host);
    spec = &host->nandchip->spec;

    /*如果OOBsize不为0就要求flashaddr是页对齐的*/
    if(0 != oobsize)
    {
        errorcode = nandc_native_location_by_address(host, flashaddr);
        if(NANDC_OK == errorcode)
        {
        	/*readsize一定是要pagesize+oobsize的整数倍*/
           if(0 != (readsize % (spec->pagesize + oobsize)))
            {
                NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the writesize(0x%x) is not page aligned!\n",
                                              readsize));
                errorcode = NANDC_E_PARAM;
				goto EXIT;
            }
            readsize = readsize - readsize/(spec->pagesize + oobsize) * oobsize;
            if(0 != (readsize & spec->offinpage))
            {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the writesize(0x%x) is not page aligned!\n",
                                              readsize));
                errorcode = NANDC_E_PARAM;
				goto EXIT;
            }
        }
        else
        {
            goto EXIT;
        }
    }

    /* coverity[printf_arg_mismatch] */
    NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_nand_mass_read flash(0x%x), dst(0x%x), size(0x%x),oobsize(0x%x), skiplength(0x%x) \n",
                                      flashaddr, dstaddr, readsize, oobsize, (u32)skiplength));

    sizeleft = readsize;
    addrglob = flashaddr;
	/*还有要读的数据*/
    while(0 < sizeleft )
    {
    	/*数据读的长度不可以超过一个分区的长度*/
        errorcode = nandc_native_location_by_address( host, addrglob);
        if(NANDC_OK != errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("read addrglob:0x%x not find\n", addrglob));
			goto EXIT;
        }

        if(NANDC_NULL == thispart)
        {
            thispart = host->curpart;
        }
        else if(thispart != host->curpart)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the read addr(0x%x) beyond the end of pattition(%s):0x%x\n",
                                      addrglob, thispart->name, (FSZ)thispart->size));
			errorcode = NANDC_E_ADDR_OVERSTEP;
            goto EXIT;
        }

        /*查询是否是坏块*/
        errorcode = nandc_nand_quary_bad((u32)(addrglob>>spec->blockshift), &badflag);
        if(NANDC_OK != errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("quary block failed:0x%x\n", addrglob));

			goto EXIT;
        }
		/*如果是坏块就会跳过此块*/
        if(NANDC_BAD_BLOCK == badflag)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: bad block detected :addrglob:0x%x\n",
                                                                addrglob));
            addrglob += host->nandchip->spec.blocksize;
            skips++;
            continue;
        }
        else
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("quary block good:0x%x\n", addrglob));
		}

        /*计算要读的数据长度*/
        pageoffset  =   (u32)(addrglob & spec->offinpage);
        pageleft    =   spec->pagesize - pageoffset;
        pageleft    =   ((sizeleft < pageleft)  ? sizeleft : pageleft);

        /*如果是读部分页的话*/
        if(pageleft < spec->pagesize)
		{
			/* coverity[printf_arg_mismatch] */
			NANDC_TRACE(NFCDBGLVL(NORMAL), ("warning: start page size not aligned :addr_logic:0x%x, \
			pageleft:0x%x\n", host->addr_logic, pageleft));
			/*读部分的page数据*/
			errorcode = nandc_nand_read_inpage(host, pageoffset,dstaddr, pageleft);
			if(NANDC_OK != errorcode)
			{
				errorcode = NANDC_E_READ;
				goto EXIT;
			}

			dstaddr    +=  pageleft;
			addrglob   +=  pageleft;
			sizeleft   -=  pageleft;
			host->addr_logic += pageleft;
			/*数据读完了就退出*/
			if(0 == sizeleft)
			{
				errorcode = NANDC_OK;
				goto EXIT;
			}
		}
		/*再来查询此块是否有要读的数据*/
        blockoffset = (u32)(addrglob & spec->offinblock);
        blockleft = spec->blocksize - blockoffset;
        blockleft = ((sizeleft < blockleft) ? sizeleft : blockleft);
		/*地址转换*/
        errorcode = nandc_bbm_real_addr(host->addr_logic , &host->addr_real);
        if (NANDC_OK != errorcode)
        {
            goto EXIT;
        }
		/*设置为读数据的阶段*/
        nandc_nand_set_latch(host, NAND_CMD_READSTART);
		/*还有block的数据要读取*/
        while(0 != blockleft)
		{
			/*读此block的部分页的话*/
			if(blockleft < spec->pagesize)
			{
				/* coverity[printf_arg_mismatch] */
				NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: end page size not aligned :addr_logic:0x%x,\
				blockleft:0x%x\n",host->addr_logic, blockleft));
				/*读部分页的话*/
				errorcode = nandc_nand_read_inpage(host, 0, dstaddr, blockleft);
				if(NANDC_OK != errorcode)
				{
					nandc_nand_clear_latch(host);
					errorcode = NANDC_E_READ;
					goto EXIT;
				}
				/*此block的数据读完了就退出来*/
				addrglob    +=  blockleft;
				sizeleft    -=  blockleft;
				break;
			}
			/*如果有OOB数据的*/
			if(oobsize)
			{
				oobbuff = (u8*)(dstaddr + spec->pagesize);
			}
			else
			{
				oobbuff = 0;
			}
            /*如果是以DMA的方式下面时要求4字节对齐*/
            #ifdef NANDC_USE_DMA_ENABLE
            if(( dstaddr&0x3 )&&( oobsize == 0 ))
            {
                NANDC_TRACE(NFCDBGLVL(ERRO), ("use dma but dstaddr is not align 4 dstaddr:0x%x\n",dstaddr));
                errorcode = nandc_nand_read_inpage(host, 0,dstaddr, spec->pagesize);
                if(NANDC_OK != errorcode)
                {
                    errorcode = NANDC_E_READ;
                    goto EXIT;
                }
            }
            else
            #endif
            {
                /*使能ECC功能的读一页数据根据参数选择的读OOB数据*/
                errorcode = nandc_nand_access( host, (u8 *)dstaddr, oobbuff, 0, oobsize, NAND_CMD_READSTART);
                if(NANDC_OK != errorcode)
                {
                    /* coverity[printf_arg_mismatch] */
                    NANDC_TRACE(NFCDBGLVL(ERRO), ("error read one page,chip:%d addrglob:0x%x\n", host->chipselect,host->addr_real));
                    nandc_nand_clear_latch(host);
                    errorcode = NANDC_E_READ;
                    goto EXIT;
                }
            }
			/*再次来判断是否有OOB数据要读*/
			if(0 == oobsize)
			{
				dstaddr += spec->pagesize;
			}
			else
			{
				dstaddr += spec->pagesize + oobsize;
			}

			addrglob    +=  spec->pagesize;
			sizeleft    -=  spec->pagesize;
			blockleft   -=  spec->pagesize;
			host->addr_logic    +=   spec->pagesize;
			host->addr_real     +=   spec->pagesize;
		}
		/*清除读写的过程*/
        nandc_nand_clear_latch(host);
    }

EXIT:
    if(NANDC_NULL != skiplength)
    {
        *skiplength =  skips << spec->blockshift;
        if(*skiplength)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mass_read bad blk skip length:0x%X\n", *skiplength));
		}
    }

    return errorcode;

ERRO:
    return NANDC_ERROR;
}

/**
* 作用:nandc模块使能ECC功能的读一个block的数据
*
* 参数:
* @flashaddr                 ---要读的Flash地址
* @dstaddr           		 ---存放要读的数据的内存地址
* @oobsize             		 ---读一页数据要读的OOB长度
*/
static u32 nandc_nand_read_block(FSZ flashaddr, u32 dstaddr, u32 oobsize)
{
    struct mtd_partition    *thispart  = NANDC_NULL;
    struct nandc_host       *host       = nandc_nand_host;
    struct nandc_spec       *spec       = NANDC_NULL;
    FSZ addrglob;
    u32 sizeleft, blockleft, badflag;
    u8* oobbuff;
    u32 errorcode = NANDC_ERROR;

    NANDC_REJECT_NULL(host);
    spec = &host->nandchip->spec;

    /* coverity[printf_arg_mismatch] */
    NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_nand_read_block flash(0x%x), dst(0x%x) \n",
                                      flashaddr, dstaddr));

    sizeleft = spec->blocksize;
    addrglob = flashaddr & spec->blockalign;
    while(0 < sizeleft )
    {
        errorcode = nandc_native_location_by_address( host, addrglob);
        if(NANDC_OK != errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("read addrglob:0x%x not find\n", addrglob));
            goto EXIT;
        }

        if(NANDC_NULL == thispart)
        {
            thispart = host->curpart;
        }
        else if(thispart != host->curpart)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the read addr(0x%x) beyond the end of partition(%s):0x%x\n",
                                      addrglob, thispart->name, (FSZ)thispart->size));
            errorcode = NANDC_E_ADDR_OVERSTEP;
            goto EXIT;
        }

        /* query bad block flag*/
        errorcode = nandc_nand_quary_bad((u32)(addrglob>>spec->blockshift), &badflag);
        if(NANDC_OK != errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("quary block failed:0x%x\n", addrglob));
            goto EXIT;
        }

        if(NANDC_BAD_BLOCK == badflag)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: bad block detected :addrglob:0x%x\n",
                                                                addrglob));
            goto ERRO;
        }
        else
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("quary block good:0x%x\n", addrglob));
        }

        blockleft = sizeleft;
        errorcode = nandc_bbm_real_addr(host->addr_logic , &host->addr_real);
        if (NANDC_OK != errorcode)
        {
            goto EXIT;
        }

        nandc_nand_set_latch(host, NAND_CMD_READSTART);
        while(0 != blockleft)
        {
            /* read data page by page in one block,when in the end the data may not
               enough for one page.we need to call "nandc_nand_read_inpage" */
           oobbuff = (u8 *)(dstaddr + spec->pagesize);

           errorcode = nandc_nand_access( host, (u8 *)dstaddr, oobbuff, 0, oobsize, NAND_CMD_READSTART);
           if(NANDC_OK != errorcode)
           {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(ERRO), ("error read one page,chip:%d addrglob:0x%x\n", host->chipselect,host->addr_real));
                nandc_nand_clear_latch(host);
				errorcode = NANDC_E_READ;
                goto EXIT;
           }

            dstaddr += (oobsize)?(spec->pagesize+oobsize):spec->pagesize;

            addrglob    +=  spec->pagesize;
            sizeleft    -=  spec->pagesize;
            blockleft   -=  spec->pagesize;
            host->addr_logic    +=   spec->pagesize;
            host->addr_real     +=   spec->pagesize;
        }
        nandc_nand_clear_latch(host);
    }

EXIT:
    return errorcode;

ERRO:
    return NANDC_ERROR;
}


/**
* 作用:nandc模块分配nandc_host的数据结构
*
* 参数:
* @host                      ---存放nandc_host的指针的指针
*/
u32 nandc_nand_create_host(struct nandc_host  **host)
{
	/*根据nandc_init_seed参数来创建nandc_host数据结构*/
    nandc_nand_host = nandc_native_host_create(nandc_init_seed);

    if(NANDC_NULL == nandc_nand_host)
    {
        *host = NANDC_NULL;
        return NANDC_ERROR;
    }
    else
    {
        *host = nandc_nand_host;
        return NANDC_OK;
    }
}

/**
* 作用:nandc模块初始化分区表信息
*
* 参数:
* @host                      ---存放nandc_host的指针的指针
*/
u32 nandc_init_mtd_partition(struct nandc_host* host)
{
    struct mtd_partition* ptable = NANDC_NULL;
    u32 errorcode = NANDC_ERROR;
    u32 nr_parts;
	/*解析分区表信息*/
    if(NANDC_OK == ptable_parse_mtd_partitions(&ptable, &nr_parts))
    {
        errorcode = nandc_host_set_partition(host, ptable, nr_parts);
    }
    else
    {
        errorcode = nandc_host_set_partition(host, NANDC_NULL, 0);
    }

    return errorcode;
}

/*****************************************************************************
* 函 数 名  : nand_spec_info_save
*
* 功能描述  : 将nand spec info保存到shared memory
*
* 输入参数  : @spec_real - nand spec info
*			  @spec_shmem - buffer to save nand spec info
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
*
*****************************************************************************/
void nand_spec_info_save(struct nandc_host *host, struct nand_spec_shared_mem *spec_dst)
{
    struct nandc_spec *spec_src = NULL;

	if(!host || !host->nandchip || !spec_dst)
	{
	    NANDC_TRACE(NFCDBGLVL(ERRO), ("[%s]ERRIR:argu null\n", __FUNCTION__));
		return;
	}

    spec_src = &host->nandchip->spec;

    spec_dst->page_size = spec_src->pagesize;
    spec_dst->ecc_type = host->nandchip->ecctype;
    spec_dst->addr_num = spec_src->addrcycle;
    spec_dst->block_size = spec_src->blocksize;
    spec_dst->spare_size = spec_src->sparesize;
    spec_dst->chip_size = spec_src->chipsize;

    NANDC_TRACE(NFCDBGLVL(ERRO), ("pagesize %d  ecctype %d  addrnum %d  blocksize 0x%x  sparesize %d chipsize 0x%x\n",
        spec_dst->page_size, spec_dst->ecc_type, spec_dst->addr_num, spec_dst->block_size, spec_dst->spare_size, spec_dst->chip_size));/*lint !e778*/

    spec_dst->flag = NAND_SPEC_ALREADY_SAVED;
	return;
}

void nand_shared_mem_clear(void)
{
    memset((void *)SHM_MEM_NAND_SPEC_ADDR, 0, sizeof(struct nand_spec_shared_mem));
    return;
}

/**
* 作用:nandc模块初始化函数
*
* 参数:
* @无
* 描述:nandc模块的初始化函数，硬件寄存器的初始化、nandc_host数据结构初始化、分区表的初始化函数
*/
u32 nandc_nand_init(void)
{
    struct nandc_host* host = NANDC_NULL;
    u32 errorcode = NANDC_ERROR;
    nand_shared_mem_clear();
    /* set ebi normal mode to avoid fault when sharing ebi with emi, only for nandc v600 */
    hi_syssc_ebi_mode_normal();

    /*nandc模块的时钟使能*/
    hi_syscrg_nandc_clk_enable();

    /*创建nandc_host数据结构*/
    if(NANDC_OK !=  nandc_nand_create_host(&host))
    {
        goto ERRO;
    }

    /*初始化命令字*/
    if(host->ctrlfunc->init_cmd)
    {
        /*modified for lint e522 */
        (void)host->ctrlfunc->init_cmd(&host->bitcmd,  host);
    }
	/*探测nandflash*/
    /*只支持8bit的nandflash*/
    host->probwidth = nandc_bus_08;
    if(NANDC_OK != nandc_native_nand_prob(host))
    {
        goto ERRO;
    }
	/*创建nandflash的分区表信息*/
    if(NANDC_OK == nandc_init_mtd_partition(host))
    {
        if(NANDC_OK != nandc_host_set_chipparam(host))
        {
            errorcode = NANDC_ERROR;
            goto ERRO;
        }
    }

    /* save nand spec info to shared memory */
    nand_spec_info_save(host, (struct nand_spec_shared_mem *)SHM_MEM_NAND_SPEC_ADDR);

    return NANDC_OK;
ERRO:

    return errorcode;
}



/**
* 作用:nandc模块裸写操作
*
* 参数:
* @pagenum                      ---要读写的页号
* @offsetinpage                 ---页面的偏移地址
* @src_addr                     ---源地址
* @writesize                    ---要写的数据长度
* 描述:nandc模块的裸写操作
*/
static u32 nandc_nand_write_raw(u32 pagenum, u32 offsetinpage, u32 src_addr, u32 writesize)
{
    struct nandc_host* host = nandc_nand_host ;
    struct nandc_spec* spec;
    int ret;

    spec = &host->nandchip->spec ;
	/*设置Flash的地址信息*/
    host->addr_real  = (pagenum & spec->pagemask) << spec->pageshift;
    host->chipselect = pagenum >> (spec->chipshift - spec->pageshift);

    if(NANDC_NULL == host->buffers)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("write raw error!! host->buffers is NULL \n"));
        goto ERRO;
    }
    /*填充要写的数据*/
    memset((void*)(host->buffers->databuf), 0xff, sizeof(host->buffers->databuf)); /*clean the pclint 516*/
    memcpy(host->buffers->databuf + offsetinpage, (const void *)src_addr, writesize);
    /*写整个Flash的数据长度*/
    ret = nandc_nand_access(host, host->buffers->databuf, host->buffers->databuf + spec->pagesize, spec->pagesize + spec->sparesize, 0, NAND_CMD_PAGEPROG);
    if(NANDC_OK != ret)
    {
        return ret;
    }

    return NANDC_OK;
ERRO:
    return NANDC_ERROR;
}


/**
* 作用:nandc模块使能ECC功能的写一页数据
*
* 参数:
* @address                      ---要读写的Flash地址
* @databuff                     ---要写的数据在内存中的地址
* @oobbuff                      ---要写的OOB数据在内存中的地址
* @oobsize                      ---一页数据要写的OOB长度

* 描述:nandc模块的使能ECC功能的写数据功能,可以在写数据的过程中写入OOB信息
*/
static u32 nandc_nand_write_page(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize)
{

    struct nandc_host* host = nandc_nand_host ;

    NANDC_REJECT_NULL(host);
	/*地址转换*/
    if(NANDC_OK != nandc_native_location_by_address( host, address))
    {
        return NANDC_E_NOFIND;
    }
	/*使能ECC功能的写一页数据，注意第4个参数为0*/
    return nandc_nand_access(host, databuff,  oobbuff, 0, oobsize, NAND_CMD_PAGEPROG);

 ERRO:

    return NANDC_E_PARAM;
}

/**
* 作用:nandc模块置block的标记值
*
* 参数:
* @blk_id                      ---block块号
* @pageoffset                  ---此block块内的页地址

* 描述:nandc模块标记block块号的标志值
*/
u32 nandc_nand_mark_flag(u32 blk_id, u32 pageoffset)
{
    struct nandc_host 	*host 	 = nandc_nand_host ;

    NANDC_REJECT_NULL(host);

	struct nandc_spec	*spec 	 = &(host->nandchip->spec);
	u32 sparesize 	= spec->sparesize;
	u32 pagesize  	= spec->pagesize;
	u8 *databuf 	= host->buffers->databuf;
	/*设置要写的Flash页地址*/
    if(NANDC_OK ==nandc_nand_flag_address(host, blk_id, pageoffset))
    {
		memset((void *)databuf, 0, (pagesize + sparesize));
		/*裸写数据操作*/
        return nandc_nand_access(host, (u8*)databuf, (u8*)(databuf + pagesize), pagesize + sparesize, 0, NAND_CMD_PAGEPROG);
    }

 ERRO:
     return NANDC_ERROR;
}


/**
* 作用:nandc模块标记坏块的功能
*
* 参数:
* @blk_id                      ---block块号

* 描述:标记nandflash的坏块功能
*/
u32 nandc_nand_mark_bad(u32 blk_id)
{
    u32 errcode = NANDC_ERROR;
    struct nandc_host *host = nandc_nand_host;

    if(0 == blk_id)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mark_bad: try to mark block 0, ignore\n"));
		return NANDC_OK;
	}

    /* coverity[printf_arg_mismatch] */
    NANDC_TRACE(NFCDBGLVL(WARNING), ("nandc_nand_mark_bad: 0x%x.\n",blk_id));

    /*标记此block的最后一页*/
	if (host->bbt_options & NAND_BBT_SCANLASTPAGE)
    {
        errcode = nandc_nand_mark_flag(blk_id, (host->nandchip->spec.blocksize / host->nandchip->spec.pagesize - 1) );
        if(NANDC_OK != errcode)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mark_bad: mark last page failed\n"));
            goto ERRO;
        }
    }
    else if(host->bbt_options & NAND_BBT_SCAN2NDPAGE)
    {
        errcode = nandc_nand_mark_flag(blk_id, NANDC_BAD_FLAG_FIRSTPAGE_OFFSET );
        if(NANDC_OK != errcode)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mark_bad:  mark first page failed\n"));
            goto ERRO;
        }
        errcode = nandc_nand_mark_flag(blk_id, NANDC_BAD_FLAG_SECONDPAGE_OFFSET);
        if(NANDC_OK != errcode)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mark_bad:  mark second page failed\n"));
            goto ERRO;
        }
    }
    else
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mark_bad: unknown bad block offset, host->bbt_options = 0x%x\n",
            host->bbt_options));
        goto ERRO;
    }

    return NANDC_OK;
ERRO:
    NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mark_bad: mark block 0x%x error, ret = 0x%x\n", blk_id, errcode));
    return errcode;
}

/**
* 作用:nandc模块擦除块的功能
*
* 参数:
* @blk_id                      ---block块号

* 描述:擦除nandflash的数据块
*/
static u32 nandc_nand_erase_block(u32 blknum)
{
    struct nandc_host* host = nandc_nand_host;
    u32 errorcode = NANDC_OK;

    NANDC_REJECT_NULL(host);

    if(0 == blknum)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(WARNING), ("nandc_nand_erase_block: erase blknum 0.\n"));
    }
	/*把块地址转换*/
    errorcode = nandc_native_location_by_blocknum(host, blknum);

    if(NANDC_OK != errorcode)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("erase blknum:0x%x not find\n", blknum));
        goto ERRO;
    }
	/*执行擦除块的功能*/
    return nandc_nand_access(host, NANDC_NULL, NANDC_NULL, 0, 0, NAND_CMD_ERASE2);

ERRO:
    return NANDC_ERROR;
}

/**
* 作用:nandc模块使能ECC功能的写多页数据,在写的过程中会擦除Flash，调用者可以不用擦除nandflash
*
* 参数:
* @flashaddr                 ---要写的Flash地址
* @srcaddr           		 ---存放要写的数据的内存地址
* @writesize                 ---要写数据的长度，可以超过一个page的长度，当要写OOB数据时,writesize包含了要写的数据长度和OOB长度
* @oobsize             		 ---写一页数据要写的OOB长度
* @skiplength                ---过滤掉的数据长度
*/
static u32 nandc_nand_mass_write(FSZ flashaddr, u32 srcaddr, u32 writesize, u32 oobsize, u32 *skiplength)
{
    struct mtd_partition    *thispart   =   NANDC_NULL;
    struct nandc_spec       *spec       =   NANDC_NULL;
    struct nandc_host       *host       =   nandc_nand_host;
    FSZ addr_glob;
    u32 sizeleft, sizeinblock, offsetinblock, badflag , blockleft ,skips = 0;
    u8* oobbuff;
    static u8* blockbuf    =   NANDC_NULL;
    static u8* rdblockbuf  =   NANDC_NULL;
    u32 errorcode   =   NANDC_OK;
    u32 ramaddr     =   srcaddr;
    u32 tempaddr, tempbuf;
    u32 rdbackaddr  =   NANDC_NULL;
    u32 rdbacklen   =   0;
    u32 buflen    = 0;
    u32 rdbuflen    = 0;
    u32 nextsrcaddr    = 0;

    NANDC_REJECT_NULL(host);
    spec = &host->nandchip->spec;

    /*如果oobsize不为0那么数据长度和flash地址有要求*/
    if(0 != oobsize)
    {
        errorcode = nandc_native_location_by_address(host, flashaddr);
        if(NANDC_OK == errorcode)
        {
            writesize = writesize - writesize/(spec->pagesize + oobsize) * oobsize;
			/*要写的数据大小是不是整数倍的pagesize+oobsize*/
            if((0 != (writesize & spec->offinpage)))
            {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the writesize(0x%x) and flashaddr(0x%x) is not block aligned!\n",
                                              writesize, flashaddr));
                errorcode = NANDC_E_PARAM;
				goto EXIT;
            }
        }
        else
        {
            goto EXIT;
        }
    }

    /* coverity[printf_arg_mismatch] */
    NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_nand_mass_write flash(0x%x), src(0x%x), size(0x%x),oobsize(0x%x), skiplength(0x%x) \n",
                                      flashaddr, srcaddr, writesize, oobsize, (u32)skiplength));

    sizeleft = writesize;
    addr_glob = flashaddr;
	/*分配内存空间,空间的大小为blocksize+oobsize*页数目*/
    buflen = spec->blocksize/spec->pagesize*(spec->pagesize + YAFFS_BYTES_PER_SPARE);  /* YAFFS_BYTES_PER_SPARE" is the size of oob data length that used by yaffs */
    if(!blockbuf)
    {
        blockbuf = (u8 *)himalloc(buflen);
    }
    NANDC_REJECT_NULL(blockbuf);

    rdbuflen = buflen;
    if(!rdblockbuf)
    {
        rdblockbuf = (u8 *)himalloc(rdbuflen);
    }
    NANDC_REJECT_NULL(rdblockbuf);
	/*有数据要写*/
    while(0 < sizeleft)
    {
        errorcode = nandc_native_location_by_address(host, addr_glob);
        if(NANDC_OK != errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("read addr_glob:0x%x not find\n", addr_glob));
			goto EXIT;
        }

        if(NANDC_NULL == thispart)
        {
            thispart = host->curpart;
        }
        else if(thispart != host->curpart)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the write addr(0x%x) beyond the end of pattition(%s)\n",
                                      addr_glob, thispart->name));

            errorcode = NANDC_E_ADDR_OVERSTEP;
            goto EXIT;
        }
		/*查询是否是坏块*/
        errorcode = nandc_nand_quary_bad((u32)(addr_glob>>spec->blockshift), &badflag);
        if(NANDC_OK != errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("quary block failed:0x%x\n", addr_glob));
			goto EXIT;
        }

        if(NANDC_BAD_BLOCK == badflag)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: bad block at:(0x%x) detected ,skip one block.\n", addr_glob));

            addr_glob += host->nandchip->spec.blocksize;
            skips++;
            goto LOOP_AGAIN;
        }
        else
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("quary block good:0x%x\n", addr_glob));
        }

        offsetinblock = (u32)addr_glob & spec->offinblock;
        sizeinblock = spec->blocksize - offsetinblock;
        sizeinblock = (sizeleft < sizeinblock) ? sizeleft : sizeinblock;

        errorcode = nandc_bbm_real_addr(host->addr_logic , &host->addr_real);
        if (NANDC_OK != errorcode)
        {
            goto EXIT;
        }

        tempaddr = addr_glob;
        tempbuf  = ramaddr;
        if((sizeinblock != spec->blocksize))
        {
            errorcode = nandc_nand_read_block(addr_glob & spec->blockalign, (u32)blockbuf, oobsize);
            if(NANDC_OK != errorcode)
            {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: read error, mark as bad block:0x%x,offset:0x%x,size:0x%x,blocksize:0x%x,line %d\n",
                                                    addr_glob, offsetinblock, sizeinblock, spec->blocksize, __LINE__));

                (void)nandc_nand_mark_bad((u32)(addr_glob>>spec->blockshift));
                addr_glob += spec->blocksize;
                skips++;
                goto LOOP_AGAIN;
            }

            /* always erase */
            ramaddr   =   (u32)blockbuf;
            addr_glob &=  spec->blockalign;
            errorcode =  nandc_nand_erase_block((u32)(addr_glob>>spec->blockshift));
            blockleft = spec->blocksize;
            if(0 != oobsize)
            {
                memcpy((void*)((u32)blockbuf + (offsetinblock/spec->pagesize)*(spec->pagesize+oobsize)), (const void*)srcaddr, (sizeinblock/spec->pagesize)*(spec->pagesize+oobsize));
            }
            else
            {
                memcpy((void*)((u32)blockbuf + offsetinblock), (const void*)srcaddr, sizeinblock);
            }
        }
        else
        {
            errorcode = nandc_nand_read_block(addr_glob & spec->blockalign, (u32)blockbuf, oobsize);
            if(NANDC_OK != errorcode)
            {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: read error, mark as bad block:0x%x,offset:0x%x,size:0x%x,blocksize:0x%x line:%d\n",
                                                    addr_glob, offsetinblock, sizeinblock, spec->blocksize, __LINE__));
				/*如果读出现了错误就要标坏块*/
                (void)nandc_nand_mark_bad((u32)(addr_glob>>spec->blockshift));
                addr_glob += spec->blocksize;
                skips++;
                goto LOOP_AGAIN;
            }

            ramaddr = srcaddr;
            errorcode =  nandc_nand_erase_block((u32)(addr_glob>>spec->blockshift));
            blockleft =  (sizeleft < spec->blocksize) ? sizeleft : spec->blocksize;
        }
        /*确定下一个要复制的数据地址*/
        if(oobsize)
        {
            nextsrcaddr=srcaddr+(sizeinblock/spec->pagesize)*(spec->pagesize+oobsize);
        }
        else
        {
            nextsrcaddr=srcaddr+sizeinblock;
        }
        if(NANDC_OK !=  errorcode)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: erase block at:(0x%x) return error, skip one block.\n", addr_glob));
            (void)nandc_nand_mark_bad((u32)(addr_glob>>spec->blockshift));
            addr_glob = tempaddr + spec->blocksize;
            skips++;
            goto LOOP_AGAIN;
        }

        nandc_nand_set_latch(host, NAND_CMD_PAGEPROG);
        rdbackaddr  = ramaddr;
        rdbacklen   = (oobsize)?(blockleft/spec->pagesize*(spec->pagesize + oobsize)):blockleft;
        while(0 != blockleft)
        {
            if(oobsize)
            {
                oobbuff = (u8*)(ramaddr + spec->pagesize);
            }
            else
            {
                oobbuff = 0;
            }

            errorcode = nandc_nand_access(host, (u8 *)ramaddr, oobbuff, 0, oobsize, NAND_CMD_PAGEPROG);
            if(NANDC_OK != errorcode)
            {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(ERRO), ("error in write ,chip:%d addr_real:0x%x\n", host->chipselect,host->addr_real));
				(void)nandc_nand_mark_bad((u32)(addr_glob>>spec->blockshift));

                skips++;
                addr_glob = tempaddr + spec->blocksize;
                ramaddr   = tempbuf;
                goto LOOP_AGAIN;
            }

            if(0 == oobsize)
            {
                 ramaddr += spec->pagesize;
            }
            else
            {
                 ramaddr += spec->pagesize + oobsize;
            }

            blockleft       -=  spec->pagesize;
            host->addr_real +=  spec->pagesize;
        }

        errorcode = nandc_nand_read_block(addr_glob , (u32)rdblockbuf, oobsize);
        if((NANDC_OK != errorcode)
			|| memcmp((const void *)rdbackaddr, (const void *)rdblockbuf, rdbacklen))
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: read back error, mark as bad block:0x%x,offset:0x%x,size:0x%x,blocksize:0x%x\n",
                                                                                        addr_glob, offsetinblock, sizeinblock, spec->blocksize));

            (void)nandc_nand_mark_bad((u32)(addr_glob>>spec->blockshift));

            skips++;
            addr_glob = tempaddr + spec->blocksize;
            ramaddr   = tempbuf;
            goto LOOP_AGAIN;
        }

        sizeleft    -=  sizeinblock;
        addr_glob   =  tempaddr + sizeinblock;

        srcaddr = nextsrcaddr;

LOOP_AGAIN:
        nandc_nand_clear_latch(host);
    }

    if(NANDC_NULL != skiplength)
    {
        *skiplength =  skips << spec->blockshift;
        if(*skiplength)
        {
           /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_nand_mass_write bad blk skip length:0x%X\n", *skiplength));

		}
    }


EXIT:
    if(blockbuf)
        hifree((void*)blockbuf);
    if(rdblockbuf)
        hifree((void*)rdblockbuf);

    return errorcode;

ERRO:
    if(blockbuf)
        hifree((void*)blockbuf);
    if(rdblockbuf)
        hifree((void*)rdblockbuf);

    return NANDC_ERROR;
}

/**
* 作用:得到nandflash的规格参数
*
* 参数:
* @specs                      ---把nandflash的规格参数填充在此数据结构中
* 描述:得到nandflash的规格参数
*/
static u32 nandc_nand_spec(struct nand_spec *specs)
{
    struct nandc_host* host = nandc_nand_host ;
    struct nandc_spec*  nand_spec;

    NANDC_REJECT_NULL(host);

    nand_spec = &host->nandchip->spec;

    specs->blocksize =   nand_spec->blocksize;
    specs->pagesize  =   nand_spec->pagesize;
    specs->chipsize  =   nand_spec->chipsize;
    specs->sparesize  =   nand_spec->sparesize;

    return NANDC_OK;

ERRO:
    return NANDC_ERROR;

}

struct nand_interface nandc_nand_funcs=
{
    nandc_nand_init,
    nandc_nand_mass_read,
    nandc_nand_spec,
    nandc_nand_read_page,
    nandc_nand_read_raw,
    nandc_nand_quary_bad,
    nandc_nand_mass_write,

    nandc_nand_write_page,
    nandc_nand_write_raw,
    nandc_nand_mark_bad,
    nandc_nand_erase_block

/*lint -restore */
};

#endif
#ifdef __cplusplus
}
#endif
