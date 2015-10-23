
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
/*lint -save -e506 */
#include <hi_base.h>
#include <hi_syscrg_interface.h>
#include <hi_syssc_interface.h>
#include <bsp_shared_ddr.h>
#include "nandc_inc.h"
#include "nandc_balong.h"

/*lint -save -e767*/
#define NFCDBGLVL(LVL)      (NANDC_TRACE_NAND|NANDC_TRACE_##LVL)
/*lint -restore*/

struct nandc_host*  nandc_nand_host = NANDC_NULL;

#ifdef __FASTBOOT__
/**********************************************************************************************
 * FUNCTION:
 * nandc_host_get_partition -
 *
 * PARAMETER:
 * @ptable  - [input ]partition table for all nand flash chips(there may be two or more chips).
 * @nr_parts - [output]partition number.
 *
 * DESCRIPTION:
 *
 * CALL FUNC:
 * nandc_nand_init() -
 *
 **********************************************************************************************/
u32 nandc_host_get_partition(struct mtd_partition** ptable, u32* nr_parts)
{
    //TODO : here should read partitition info from bootload in first block of cs0 nand chip

    HI_NOTUSED(ptable);
    HI_NOTUSED(nr_parts);

    return NANDC_ERROR;
}

/********************************************************************
 * FUNCTION:
 * nandc_nand_transfer_oob -  transfer oob to client buffer
 *
 * PARAMETER:
 * @host  -  [input ]nand controller host structure
 * @oobdst-	 [output]oob destination address (client buffer)
 * @oobsrc-  [input ]oob source address      (nand driver buffer)
 * @len   -	 [input ]size of oob to transfer
 *
 * DESCRIPTION:
 * This function transfers oob to client buffer.
 *
 * CALL FUNC:
 * () -
 *******************************************************************/
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_fill_oob - [intern]Transfer client buffer to oob
 *
 * PARAMETER:
 * @host  -  [input ]nand controller host structure
 * @oobdst-	 [output]oob destination address (nand driver buffer)
 * @oobsrc-  [input ]oob source address      (client buffer)
 * @len:	 [input ]size of oob to transfer
 *
 * DESCRIPTION:
 * This function transfers client buffer to oob.
 *
 * CALL FUNC:
 *
 *********************************************************************/
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

/****************************************************************
 * FUNCTION:
 * nandc_nand_set_latch - sets host->latched
 *
 * PARAMETER:
 * @host  - [input]through it get host->latched.
 * @access - [output]NAND_CMD_READSTART : reading operation.
 *                   NAND_CMD_PAGEPROG  : writing operation.
 *
 * DESCRIPTION:
 * This function sets host->latched.
 *
 * CALL FUNC:
 * nandc_nand_mass_read() -
 * nandc_nand_mass_write() -
 *
 *******************************************************************/
__inline void nandc_nand_set_latch(struct nandc_host* host, u32 access)
{   /*lint !e578 */
    if(NAND_CMD_READSTART == access)
    {
        if(nandc_read_latch_done != host->latched)
        {
            host->latched = nandc_read_latch_start;
        }
    }
    else if(NAND_CMD_PAGEPROG == access)
    {
        if(nandc_write_latch_done != host->latched)
        {
            host->latched = nandc_write_latch_start;
        }
    }
}

/****************************************************************
 * FUNCTION:
 * nandc_nand_clear_latch - clear host->latched
 *
 * PARAMETER:
 * @host  - [input]through it to get host->latched.
 *
 * DESCRIPTION:
 * This function clears host->latched.
 *
 * CALL FUNC:
 * nandc_nand_mass_read() -
 * nandc_nand_mass_write() -
 *
 *******************************************************************/
__inline void nandc_nand_clear_latch(struct nandc_host* host)
{
    host->latched = nandc_latch_none;
}

/************************************************************************
 * FUNCTION:
 * nandc_nand_set_address - [intern]set physics address by real address
 *
 * PARAMETER:
 * @host  -  [input ]nand controller host structure
 * @oobdst-	 [output]oob destination address (nand driver buffer)
 *
 * DESCRIPTION:
 * when not called form Linux'MTD, the physics address should not been set yet,
 * but the real address is given ,so here can calculate physics address by real address.
 *
 * CALL FUNC:
 * () -
 *
 ************************************************************************************/
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
        /*may called from native*/
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

        host->addr_physics[0] = column_offset;
        host->addr_physics[0] |= page_offset << NANDC_COLUMN_SHIFT;

        host->addr_physics[1] = page_offset >> NANDC_COLUMN_SHIFT;

    }

    NANDC_DO_ASSERT(((u32)host->addr_real < (u32)spec->chipsize), "virtual address too large!", (u32)host->addr_real);

    return NANDC_OK;
}

/****************************************************************************************************
 * FUNCTION:
 * nandc_nand_access - [intern]from this entry to low layer nand driver: nand controller layer.
 *
 * PARAMETER:
 * @host    -  [input ] nand controller host structure
 * @databuff-  [input/output]  a pointer to ram address that store data witch will read from nand or will
 *                             write to nand.
 *
 * @oobbuff -  [input/output]  a pointer to ram address that store oob witch will read from nand or will
 *                             write to nand.

 * @datalen -  [input ]  indicate the length of @databuff.
 * @ooblen  -  [input ]  indicate the length of @oobbuff.
 * @access  -  [input ]  indicate direction of this operation: read from nand or write to nand.
 *
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
 * DESCRIPTION:
 * This function is the only way from this layer to low nand driver:nandc_ctrl.c
 * here first fill phsics address for hardware register set, then select the rigth ecc function.
 * according to @access select, the oob will be filled before nand operation or transfored after
 * nand operation.
 *
 * CALL FUNCIONS:
 * @nandc_ctrl_entry() : the route to low layer driver.
 *
 *****************************************************************************************************/
__inline u32 nandc_nand_access(struct nandc_host* host, u8* databuff, u8* oobbuff, u32 datalen, u32 ooblen, u32 access)
{/*lint !e578 */
    u32 errorcode = NANDC_ERROR;

    if(NANDC_OK != nandc_nand_set_address(host))
    {
        goto ERRO;
    }

    switch(access)
    {
        case NAND_CMD_PAGEPROG:
        case NAND_CMD_READSTART:
        {
            NANDC_REJECT_NULL((u32)databuff|(u32)oobbuff);

            if(0 == datalen)
            {
            	/*ecc mode,read whole page data and oob data*/
                host->databuf   = databuff;
#ifdef NANDC_USE_V600
				host->options   =   NANDC_OPTION_DMA_ENABLE;
#else
				host->options   =   0 ;
#endif

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
#ifdef NANDC_USE_V600
				host->options   |=  NANDC_OPTION_DMA_ENABLE ; 
#endif
                host->length    =   datalen;
            }

            if((NAND_CMD_PAGEPROG == access)&&(NANDC_NULL != oobbuff))
            {
                nandc_nand_fill_oob(host, host->oobbuf, oobbuff, ooblen);
            }
        }
        break ;
        case NAND_CMD_ERASE2:
        {
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
        //TODO: bbm managment
        goto EXIT;
    }

    if((NAND_CMD_READSTART == access)&&(NANDC_NULL != oobbuff))
    {
        nandc_nand_transfer_oob(host, oobbuff, host->oobbuf, ooblen);
    }

EXIT:
    return errorcode;

ERRO:
    return NANDC_ERROR;
}

/**********************************************************************
 * FUNCTION:
 * nandc_nand_read_raw - read raw data from nand flash chip to ram.
 *
 * PARAMETER:
 * @pagenum  -  [input ]page number in total address space(there may be two or more
 *                      nand flash chips).
 * @offsetinpage -	[output]offset in one page of nand flash chip.
 * @dst_addr  -  [input ]ram address of destination data.
 * @readsize  -  [input ]the size of data.
 *
 * DESCRIPTION:
 * This function reads raw data from nand flash chip to ram.
 *
 * CALL FUNC:
 * nandc_nand_funcs->read_page_raw()-
 *
 *********************************************************************/
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_read_page - read data in one page of nand flash chip.
 *
 * PARAMETER:
 * @address  -  [input]address in total address space(there may be two or more chips).
 * @databuff -	[intput/output]ram address of raw data.
 * @oobbuff  -  [input]ram address of oob data.
 * @oobsize  -  [input]the length of oob data.
 *
 * DESCRIPTION:
 * This function reads data in one page of nand flash chip.
 *
 * CALL FUNC:
 * nandc_nand_funcs->read_page_ecc() -
 *
 *********************************************************************/
static u32 nandc_nand_read_page(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize)
{
    struct nandc_host* host = nandc_nand_host ;

    NANDC_REJECT_NULL(host);

    if(NANDC_OK != nandc_native_location_by_address( host, address))
    {
        return NANDC_E_NOFIND;
    }

    host->addr_real &= host->nandchip->spec.pagealign;

    return nandc_nand_access(host, databuff,  oobbuff, 0, oobsize, NAND_CMD_READSTART);

ERRO:
	return NANDC_E_NULL;
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
    ret = nandc_nand_read_page(flash_addr, (u8*)dst_data,NULL,0);
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_flag_address -
 *
 * PARAMETER:
 * @host  -  [input]the main structure.
 * @blk_id - [output]block id (there may be more than one nand flash chip,
 *                    all nand flash chips' block are arranged)
 * @pageoffset  - [input]page offset in total nand flash chips.
 *
 * DESCRIPTION:
 *
 *
 * CALL FUNC:
 * nandc_nand_quary_flag() -
 * nandc_nand_mark_flag() -
 *
 *********************************************************************/
static __inline u32 nandc_nand_flag_address(struct nandc_host* host, u32 blk_id, u32 pageoffset)
{
    u32 flagoffset;
    if(NANDC_OK != nandc_native_location_by_blocknum( host, blk_id))
    {
        goto ERRO;
    }

    host->addr_real +=  pageoffset << host->nandchip->spec.pageshift;

    if(NANDC_OK != nandc_nand_set_address(host))
    {
        goto ERRO;
    }

    flagoffset = host->badoffset;
    /* coverity[printf_arg_mismatch] */
	NANDC_TRACE(NFCDBGLVL(NORMAL),("host->badoffset=0x%x\n",flagoffset));

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

/*********************************************************************************
 * FUNCTION:
 * nandc_nand_quary_flag -
 *
 * PARAMETER:
 * @blk_id  -  [input]block id (there may be more than one nand flash chip,
 *                    all nand flash chips' block are arranged)
 * @pageoffset - [output]page offset in total nand flash chips(there may be
 *                       more than one nand flash chip,all nand flash chips'
 *                       block are arranged)
 *
 * @databuf  -  [input]flag data which will be read from nand flash chip.
 * @size  -  [input]length ot flag data.
 *
 * DESCRIPTION:
 * This function reads bad block flag and queries a block it is a bad block or not.
 *
 * CALL FUNC:
 *
 *************************************************************************************/
u32 nandc_nand_quary_flag(u32 blk_id, u32 pageoffset, u8* oobbuf ,u32 size)
{
    struct nandc_host* host = nandc_nand_host ;

    NANDC_REJECT_NULL(host);

	struct nandc_spec	*spec 	 = &(host->nandchip->spec);
	u32 sparesize 	= spec->sparesize;
	u32 pagesize  	= spec->pagesize;
    u32 errcode     = NANDC_ERROR;
	u8 *databuf 	= host->buffers->databuf;

    if(NANDC_OK ==nandc_nand_flag_address(host, blk_id, pageoffset))
    {
    	errcode = nandc_nand_access(host, (u8*)databuf, (u8*)(databuf + pagesize), pagesize + sparesize, 0, NAND_CMD_READSTART);
        if(NANDC_OK == errcode)
        {
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_quary_bad - query a block is a good block or a bad block.
 *
 * PARAMETER:
 * @blk_id  - [input ]block id (there may be more than one nand flash chip,
 *                    all nand flash chips' block are arranged)
 * @isbad  -  [input/output]read from first page of one block and returned with
 *                      NANDC_BAD_BLOCK or NANDC_GOOD_BLOCK
 * DESCRIPTION:
 * This function queries a block is a good block or a bad block.
 *
 * CALL FUNC:
 *
 *********************************************************************/
static u32 nandc_nand_quary_bad(u32 blk_id, u32 * isbad )
{
    u8 badflag[NANDC_BADBLOCK_FLAG_SIZE] = {0};
    u32 errcode = NANDC_ERROR, cnt;
    struct nandc_host *host = nandc_nand_host;

    /* 对于SLC Nand，需要查询page0和page1，对于MLC Nand，需要查询last page */
	if (host->bbt_options & NAND_BBT_SCANLASTPAGE)
    {
        /* scan last page */
        errcode = nandc_nand_quary_flag(blk_id, (host->nandchip->spec.blocksize / host->nandchip->spec.pagesize - 1),
            badflag, sizeof(badflag));
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
    else if(host->bbt_options & NAND_BBT_SCAN2NDPAGE)
    {
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_read_inpage - read data from offset to end in one page.
 *
 * PARAMETER:
 * @host  -  [input ]the main structure.
 * @pageoffset - [output]offset in page.
 * @dstaddr  -  [input ]destination address in ram.
 * @pageleft - [output]data lenght from offset to end in page.
 *
 * DESCRIPTION:
 * This function reads data from offset to end in one page.
 * data transfer direction:
 *
 *   nand controller buffer            temporary storage            final destination
 * -------------------------        ------------------------       --------------------
 * |      host->bufbase    |  ----> |host->buffers->databuf| ----> |      dstaddr     |
 * -------------------------        ------------------------       --------------------
 *
 * CALL FUNC:
 *
 *********************************************************************/
static __inline u32 nandc_nand_read_inpage(struct nandc_host* host, u32 pageoffset, u32 dstaddr, u32 pageleft)
{
    u8* pagebuf     =   host->buffers->databuf;
    u32 pagealign   =   host->nandchip->spec.pagealign;

    if (NANDC_OK != nandc_bbm_real_addr((host->addr_logic & pagealign), &host->addr_real))
    {
        goto ERRO;
    }

    if(NANDC_OK !=  nandc_nand_access( host, (u8 *)pagebuf, NANDC_NULL, 0, 0, NAND_CMD_READSTART))
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("error read in page ,chip:%d addrglob:0x%x\n",
        (u32)host->chipselect, (u32)host->addr_real));
		goto ERRO;
    }
    memcpy((void*)dstaddr, pagebuf + pageoffset, pageleft);

    return  NANDC_OK;

ERRO:

    return NANDC_ERROR;

}

/**********************************************************************
 * FUNCTION:
 * nandc_nand_mass_read - mass data reading opearation interface.
 *
 * PARAMETER:
 * @flashaddr  -  [input ]address in nand flash chips(there may be two or more chips).
 * @dstaddr - [output]ram adderss for source data.
 * @readsize  -  [input ]the length of the raw data.
 * @oobsize - [output]the length of the oob data.
 * @skiplength - [output]has little signification.
 *
 *
 * DESCRIPTION:
 * This function reads mass data from nand flash chip to ram buffer.
 *
 * CALL FUNC:
 * nandc_nand_funcs -> read() -
 *
 *********************************************************************/
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

    /*if oobsize is not null, the readsize and flashaddr must be block aligned!(1MB)*/
    if(0 != oobsize)
    {
        errorcode = nandc_native_location_by_address(host, flashaddr);
        if(NANDC_OK == errorcode)
        {
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
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERRO! the read addr(0x%x) beyond the end of pattition(%s):0x%x\n",
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
            addrglob += host->nandchip->spec.blocksize;
            skips++;
            continue;
        }
        else
        {
            /* coverity[printf_arg_mismatch] */            
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("quary block good:0x%x\n", addrglob));
		}

        /*read page left*/
        pageoffset  =   (u32)(addrglob & spec->offinpage);
        pageleft    =   spec->pagesize - pageoffset;
        pageleft    =   ((sizeleft < pageleft)  ? sizeleft : pageleft);

        /* If start address for reading operation is not page aligned we need to
           call "nandc_nand_read_inpage"*/
        if(pageleft < spec->pagesize)
        {
            /* coverity[printf_arg_mismatch] */
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("warning: start page size not aligned :addr_logic:0x%x, \
pageleft:0x%x\n", host->addr_logic, pageleft));

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

            if(0 == sizeleft)
            {
                errorcode = NANDC_OK;
                goto EXIT;
            }
        }

        blockoffset = (u32)(addrglob & spec->offinblock);
        blockleft = spec->blocksize - blockoffset;
        blockleft = ((sizeleft < blockleft) ? sizeleft : blockleft);

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
            if(blockleft < spec->pagesize)
            {
                /* coverity[printf_arg_mismatch] */
                NANDC_TRACE(NFCDBGLVL(WARNING), ("warning: end page size not aligned :addr_logic:0x%x,\
blockleft:0x%x\n",host->addr_logic, blockleft));

                errorcode = nandc_nand_read_inpage(host, 0, dstaddr, blockleft);
                if(NANDC_OK != errorcode)
                {
                    nandc_nand_clear_latch(host);
					errorcode = NANDC_E_READ;
                    goto EXIT;
                }

                addrglob    +=  blockleft;
                sizeleft    -=  blockleft;
                break;
           }

           if(oobsize)
           {
                oobbuff = (u8*)(dstaddr + spec->pagesize);
           }
           else
           {
                oobbuff = 0;
           }
           #ifdef NANDC_USE_V600
            if(( dstaddr&0x3 )&&(oobsize == 0))
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_create_host - initialize global structure nandc_nand_host.
 *
 * PARAMETER:
 * @host  -  [input/output]one the main structure correlative with nand.
 *
 * DESCRIPTION:
 * This function initializes global structure "nandc_nand_host".
 *
 * CALL FUNC:
 * nandc_mtd_host_init() -
 * nandc_nand_init() -
 *
 *********************************************************************/
u32 nandc_nand_create_host(struct nandc_host  **host)
{
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

/**********************************************************************
 * FUNCTION:
 * nandc_add_mtd_partition -
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * add mtd partition for nandc driver by flash partition table
 *
 * CALL FUNC:
 * nand_init() -
 *
 *********************************************************************/
u32 nandc_init_mtd_partition(struct nandc_host* host)
{
    struct mtd_partition* ptable = NANDC_NULL;
    u32 errorcode = NANDC_ERROR;
    u32 nr_parts;

    if(NANDC_OK == ptable_parse_mtd_partitions(&ptable, &nr_parts))
    {
        errorcode = nandc_host_set_define(host, ptable, nr_parts);
    }
    else
    {
        errorcode = nandc_host_set_define(host, NANDC_NULL, 0);
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

    NANDC_TRACE(NFCDBGLVL(ERRO), ("pagesize %d\necctype %d\naddrnum %d\nblocksize 0x%x\nsparesize %d\nchipsize 0x%x\n",
        spec_dst->page_size, spec_dst->ecc_type, spec_dst->addr_num, spec_dst->block_size, spec_dst->spare_size, spec_dst->chip_size));/*lint !e778*/

    spec_dst->flag = NAND_SPEC_ALREADY_SAVED;
	return;
}

void nand_shared_mem_clear(void)
{
    memset((void *)SHM_MEM_NAND_SPEC_ADDR, 0, sizeof(struct nand_spec_shared_mem));
    return;
}

/**********************************************************************
 * FUNCTION:
 * nandc_nand_init - malloc space for the main structure "host" and initialize it.
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * This function  mallocs space for the main structure "host" and initializes it.
 *
 * CALL FUNC:
 * nand_init() -
 *
 *********************************************************************/
u32 nandc_nand_init(void)
{
    struct nandc_host* host = NANDC_NULL;
    u32 errorcode = NANDC_ERROR;

    nand_shared_mem_clear();

    /* set ebi normal mode to avoid fault when sharing ebi with emi, only for nandc v600 */
    hi_syssc_ebi_mode_normal();

    /* nandc clk enable */
    hi_syscrg_nandc_clk_enable();

    /*use "nandc_init_seed" to initialize host*/
    if(NANDC_OK !=  nandc_nand_create_host(&host))
    {
        goto ERRO;
    }

    /*nandc2_ctrl_init ,nandc3_ctrl_init or nandc4_ctrl_init*/
    if(host->ctrlfunc->init_cmd)
    {
        /*modified for lint e522 */
        (void)host->ctrlfunc->init_cmd(&host->bitcmd,  host);
    }

    host->probwidth = nandc_bus_08;
    if(NANDC_OK != nandc_native_nand_prob(host))
    {
        host->probwidth = nandc_bus_16;
        if(NANDC_OK != nandc_native_nand_prob(host))
        {
            errorcode = NANDC_ERROR;
            goto ERRO;
        }
    }

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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_deinit - free space for the main structure "host" and de-initialize it.
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * This function  frees space for the main structure "host" and de-initializes it.
 *
 * CALL FUNC:
 *
 *********************************************************************/
static u32 nandc_nand_deinit(void)
{

     if(nandc_nand_host)
        nandc_native_host_delete(nandc_nand_host);

    return NANDC_OK;
}

/*****************************************************************************************
 * FUNCTION:
 * nandc_nand_write_raw -  write source data (raw data + oob data)with ecc calibration and
 *                         correction is closed.
 *
 * PARAMETER:
 * @pagenum  -  [input]page number in nand flash chips.(there may be more than one chip)
 * @offsetinpage - [output]offset in one page of nand flash chip.
 * @src_addr  -  [input]ram address for raw data + oob data.
 * @writesize - [output]length for source data.
 *
 * DESCRIPTION:
 * This function writes source data(raw data + oob data) with ecc calibration and correction
 * is closed.
 *
 * CALL FUNC:
 * nandc_nand_funcs->write_page_raw()-
 *
 ******************************************************************************************/
static u32 nandc_nand_write_raw(u32 pagenum, u32 offsetinpage, u32 src_addr, u32 writesize)
{
    struct nandc_host* host = nandc_nand_host ;
    struct nandc_spec* spec;
    int ret;

    spec = &host->nandchip->spec ;

    host->addr_real  = (pagenum & spec->pagemask) << spec->pageshift;
    host->chipselect = pagenum >> (spec->chipshift - spec->pageshift);

    if(NANDC_NULL == host->buffers)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("write raw error!! host->buffers is NULL \n"));
        goto ERRO;
    }
    
    memset((void*)(host->buffers->databuf), 0xff, sizeof(host->buffers->databuf)); /*clean the pclint 516*/
    memcpy(host->buffers->databuf + offsetinpage, (const void *)src_addr, writesize);
    
    ret = nandc_nand_access(host, host->buffers->databuf, host->buffers->databuf + spec->pagesize, spec->pagesize + spec->sparesize, 0, NAND_CMD_PAGEPROG);
    if(NANDC_OK != ret)
    {
        return ret;
    }

    return NANDC_OK;
ERRO:
    return NANDC_ERROR;
}

/****************************************************************************************************
 * FUNCTION:
 * nandc_nand_write_page - write raw data and oob data with ecc calibration and correction is open
 *
 * PARAMETER:
 * @address  -  [input]address in nand flash chips(there may be more than one chip).
 * @databuff - [output]ram address for raw data.
 * @oobbuff  -  [input]ram address for oob data.
 * @oobsize - [output]length for oob data.
 *
 * DESCRIPTION:
 * This function write raw data and oob data with ecc calibration and correction is open.
 *
 * CALL FUNC:
 * nandc_nand_funcs->write_page_ecc()-
 *
 ***************************************************************************************************/
static u32 nandc_nand_write_page(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize)
{

    struct nandc_host* host = nandc_nand_host ;

    NANDC_REJECT_NULL(host);

    if(NANDC_OK != nandc_native_location_by_address( host, address))
    {
        return NANDC_E_NOFIND;
    }

    return nandc_nand_access(host, databuff,  oobbuff, 0, oobsize, NAND_CMD_PAGEPROG);

 ERRO:

    return NANDC_E_PARAM;
}

/**********************************************************************
 * FUNCTION:
 * nandc_nand_mark_flag - mark a block as a bad block according to block id
 *                        with flag data.
 *
 * PARAMETER:
 * @blk_id  -  [input]block id (there may be more than one nand flash chip,
 *                    all nand flash chips' block are arranged)
 * @pageoffset - [output]offset from page start address.
 * @databuf  -  [input]flag data which indicates a block is a bad block.
 * @size - [output]size of the flag data.
 *
 * DESCRIPTION:
 * This function marks a block as a bad block according to block id
 * with flag data.
 *
 * CALL FUNC:
 * nandc_nand_mark_bad() -
 * nandc_nand_mark_erased() -
 *
 *********************************************************************/
u32 nandc_nand_mark_flag(u32 blk_id, u32 pageoffset)
{
    struct nandc_host 	*host 	 = nandc_nand_host ;

    NANDC_REJECT_NULL(host);

	struct nandc_spec	*spec 	 = &(host->nandchip->spec);
	u32 sparesize 	= spec->sparesize;
	u32 pagesize  	= spec->pagesize;
	u8 *databuf 	= host->buffers->databuf;

    if(NANDC_OK ==nandc_nand_flag_address(host, blk_id, pageoffset))
    {
		memset((void *)databuf, 0, (pagesize + sparesize));
        return nandc_nand_access(host, (u8*)databuf, (u8*)(databuf + pagesize), pagesize + sparesize, 0, NAND_CMD_PAGEPROG);
    }

 ERRO:
     return NANDC_ERROR;
}

/**********************************************************************
 * FUNCTION:
 * nandc_nand_mark_bad - mark one block as a block according to block id
 *
 * PARAMETER:
 * @blk_id  -  [input]block id (there may be more than one nand flash chip,
 *                    all nand flash chips' block are arranged)
 *
 * DESCRIPTION:
 * This function marks one block as a block according to block id.
 *
 * CALL FUNC:
 * nandc_nand_mass_write() -
 *
 *********************************************************************/
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

    /* 对于SLC Nand，标记page0和page1，对于MLC Nand，标记last page */
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_erase_block - erase one block of nand flash chip.
 *
 * PARAMETER:
 * @blknum  -  [input] the no. of block which will be erased.
 *
 * DESCRIPTION:
 * This function erases one block of nand flash chip according to the no.
 * of block.
 *
 * CALL FUNC:
 *
 *********************************************************************/
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

    errorcode = nandc_native_location_by_blocknum(host, blknum);

    if(NANDC_OK != errorcode)
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("erase blknum:0x%x not find\n", blknum));
        goto ERRO;
    }

    return nandc_nand_access(host, NANDC_NULL, NANDC_NULL, 0, 0, NAND_CMD_ERASE2);

ERRO:
    return NANDC_ERROR;
}

/****************************************************************************************
 * FUNCTION:
 * nandc_nand_mass_write - mass data writing opearation interface.
 *
 * PARAMETER:
 * @flashaddr  -  [input]address in nand flash chips(there may be two or more chips).
 * @srcaddr  -  [input]ram adderss for source data.
 * @writesize  -  [input]the length of the raw data.
 * @oobsize  -  [input]the length of the oob data.
 * @skiplength  -  [input]has little signification.
 *
 * DESCRIPTION:
 * This function writes mass data from ram buffer to nand flash chip.
 *
 * CALL FUNC:
 * nandc_nand_funcs -> write() -
 *
 ****************************************************************************************/
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

    /*if oobsize is not null, the writesize and flashaddr must be block aligned!*/
    if(0 != oobsize)
    {
        errorcode = nandc_native_location_by_address(host, flashaddr);
        if(NANDC_OK == errorcode)
        {
            writesize = writesize - writesize/(spec->pagesize + oobsize) * oobsize;
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

/**********************************************************************
 * FUNCTION:
 * nandc_nand_spec - get nand flash chip's spec
 *
 * PARAMETER:
 * @specs  -  [input/out]the structure which stores the spec data.
 *
 * DESCRIPTION:
 * This function gets nand flash chip's spec data.
 *
 * CALL FUNC:
 * nandc_nand_funcs -> get_spec() -
 *
 *********************************************************************/
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
    nandc_nand_deinit,
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
#else /* __KERNEL__ */
/**********************************************************************
 * FUNCTION:
 * nandc_nand_spec - get nand flash chip's spec
 *
 * PARAMETER:
 * @specs  -  [input/out]the structure which stores the spec data.
 *
 * DESCRIPTION:
 * This function gets nand flash chip's spec data.
 *
 * CALL FUNC:
 * nandc_nand_funcs -> get_spec() -
 *
 *********************************************************************/
u32 nandc_nand_init(void)
{
    return NANDC_OK;
}

struct nand_interface nandc_nand_funcs=
{
    nandc_nand_init,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,
};

#endif
#ifdef __cplusplus
}
#endif
