
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*
*    All rights reserved.
*
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <ptable_com.h>
#include "nandc_inc.h"

/*lint -save -e767*/
#define NFCDBGLVL(LVL)      (NANDC_TRACE_HOST|NANDC_TRACE_##LVL)
/*lint -restore*/

struct nandc_bit_cluster  nandc_bit_cluster_normal_template =
{
    STU_SET(.cmd)           		{nandc_bits_cmd,            			NANDC_NULL,    NANDC_NULL},
/*    
    STU_SET(.cmd2)          		{nandc_bits_cmd2,           			NANDC_NULL,    NANDC_NULL},
    STU_SET(.cmd3)          		{nandc_bits_cmd3,           			NANDC_NULL,    NANDC_NULL}, 
*/
    STU_SET(.bus_width)     		{nandc_bits_bus_width,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.addr_high)     		{nandc_bits_addr_high,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.addr_low)      		{nandc_bits_addr_low,       			NANDC_NULL,    NANDC_NULL},

    STU_SET(.addr_cycle)    		{nandc_bits_addr_cycle,     			NANDC_NULL,    NANDC_NULL},
    STU_SET(.chip_select)   		{nandc_bits_chip_select,    			NANDC_NULL,    NANDC_NULL},
    STU_SET(.operate)       		{nandc_bits_operation,      			NANDC_NULL,    NANDC_NULL},

    STU_SET(.op_stauts)     		{nandc_bits_op_status,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_stauts)    		{nandc_bits_int_status,     			NANDC_NULL,    NANDC_NULL},
    STU_SET(.data_num)      		{nandc_bits_data_num,       			NANDC_NULL,    NANDC_NULL},

    STU_SET(.ecc_type)      		{nandc_bits_ecc_type,       			NANDC_NULL,    NANDC_NULL},
    STU_SET(.ecc_select)    		{nandc_bits_ecc_select,     			NANDC_NULL,    NANDC_NULL},
#ifdef NANDC_USE_V400
    STU_SET(.int_enable)    		{nandc_bits_int_enable,     			NANDC_NULL,    NANDC_NULL},
#endif
    STU_SET(.op_mode)       		{nandc_bits_op_mode,        			NANDC_NULL,    NANDC_NULL},
    STU_SET(.page_size)     		{nandc_bits_page_size,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_clr)       		{nandc_bits_int_clear,      			NANDC_NULL,    NANDC_NULL},

    STU_SET(.segment)       		{nandc_bits_segment,        			NANDC_NULL,    NANDC_NULL},
    STU_SET(.ecc_result)    		{nandc_bits_ecc_result,     			NANDC_NULL,    NANDC_NULL},
    STU_SET(.nf_status)     		{nandc_bits_nf_status,      			NANDC_NULL,    NANDC_NULL},

#ifdef NANDC_USE_V600
	STU_SET(.async_interface_type)  {nandc_bits_async_interface_type,      	NANDC_NULL,    NANDC_NULL},
    STU_SET(.randomizer)   			{nandc_bits_randomizer,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.read_id_en) 			{nandc_bits_read_id_en,      			NANDC_NULL,    NANDC_NULL},

	STU_SET(.rw_reg_en)     		{nandc_bits_rw_reg_en,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.ecc_class)     		{nandc_bits_ecc_class,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.randomizer_en)     	{nandc_bits_randomizer_en,      		NANDC_NULL,    NANDC_NULL},

	STU_SET(.cs0_done)     			{nandc_bits_cs0_done,      				NANDC_NULL,    NANDC_NULL},
    STU_SET(.ahb_op_err)     		{nandc_bits_ahb_op_err,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.wr_lock_err)     		{nandc_bits_wr_lock_err,      			NANDC_NULL,    NANDC_NULL},

	STU_SET(.dma_done)     			{nandc_bits_dma_done,      				NANDC_NULL,    NANDC_NULL},
    STU_SET(.dma_err)     			{nandc_bits_dma_err,      				NANDC_NULL,    NANDC_NULL},
    STU_SET(.dma_start)     		{nandc_bits_dma_start,      			NANDC_NULL,    NANDC_NULL},
/*
	STU_SET(.dma_rw_enable)     	{nandc_bits_dma_rw_enable,      		NANDC_NULL,    NANDC_NULL},
    STU_SET(.burst_enable)     		{nandc_bits_burst_enable,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.dma_addr_num)     		{nandc_bits_dma_addr_num,      			NANDC_NULL,    NANDC_NULL},

	STU_SET(.dma_cs)     			{nandc_bits_dma_cs,      				NANDC_NULL,    NANDC_NULL},
    STU_SET(.dma_wr)     			{nandc_bits_dma_wr,      				NANDC_NULL,    NANDC_NULL},
    STU_SET(.dma_rd_oob)     		{nandc_bits_dma_rd_oob,      			NANDC_NULL,    NANDC_NULL},
*/
	STU_SET(.base_addr_data)     	{nandc_bits_base_addr_data,      		NANDC_NULL,    NANDC_NULL},
    STU_SET(.base_addr_oob)     	{nandc_bits_base_addr_oob,      		NANDC_NULL,    NANDC_NULL},
    STU_SET(.dma_len)     			{nandc_bits_dma_len,      				NANDC_NULL,    NANDC_NULL},

	STU_SET(.errnum0buf0)     		{nandc_bits_errnum0buf0,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.rb_status)     		{nandc_bits_rb_status,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.base_addr_d1)     		{nandc_bits_base_addr_d1,      			NANDC_NULL,    NANDC_NULL},

	STU_SET(.base_addr_d2)     		{nandc_bits_base_addr_d2,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.base_addr_d3)     		{nandc_bits_base_addr_d3,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.oob_len_sel)     		{nandc_bits_oob_len_sel,      			NANDC_NULL,    NANDC_NULL},

	STU_SET(.dma_ctrl)     			{nandc_bits_dma_ctrl,      				NANDC_NULL,    NANDC_NULL},

    STU_SET(.int_op_done)     		{NANDC_BITS_OP_DONE_EN,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_cs_done)     		{NANDC_BITS_CS_DONE_EN,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_err_invalid)    	{NANDC_BITS_ERR_INVALID_EN,      		NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_err_valid)     	{NANDC_BITS_ERR_VALID_EN,      		    NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_ahb_op)     		{NANDC_BITS_AHD_OP_EN,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_wr_lock)     		{NANDC_BITS_WR_LOCK_EN,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_dma_done)     		{NANDC_BITS_DMA_DONE_EN,      			NANDC_NULL,    NANDC_NULL},
    STU_SET(.int_dma_err)     		{NANDC_BITS_DMA_ERR_EN,      			NANDC_NULL,    NANDC_NULL},
#endif

    STU_SET(.endtable)      		{nandc_bits_enum_end,       			NANDC_NULL,    NANDC_NULL},
};


/**********************************************************************************************
 * FUNCTION:
 * is_nand_id_4bitecc 
 *
 * PARAMETER:
 * @host  - [input ]the main structure
 *
 * DESCRIPTION:
 *
 * RETRUN:
 *  0 - no; else - yes
 * CALL FUNC:
 * nandc_nand_ecctpye_check() 
 *
 **********************************************************************************************/
int is_nand_id_4bitecc(struct nandc_host* host)
{
    int ret = NANDC_FALSE;
    unsigned char *table = host->nand_4bitecc_table;

    /* ¶ÔÓÚnandc v400£¬host->nand_4bitecc_tableÎª¿Õ */
    if(!table || !(*table))
        return ret;

    while(*table != NANDC_NULL)
    {
        if(!memcmp(host->flash_id, table, NANDC_READID_SIZE))
        {
            ret = NANDC_TRUE;
            goto EXIT;
        }
        table += NANDC_READID_SIZE;
    }
    return ret;
EXIT:
    return ret;
}

/**********************************************************************************************
 * FUNCTION:
 * set_phyaddr_low_8bit 
 *
 * PARAMETER:
 * @host  - [input ]the main structure
 *
 * DESCRIPTION:
 *  set the 
 *
 * RETRUN:
 *  null
 * CALL FUNC:
 * nandc_host_add_nand_chip() 
 *
 **********************************************************************************************/
void set_phyaddr_low_8bit(struct nandc_host* host)
{
    struct ST_PART_TBL* ptable = NULL;
    u32 flash_addr;

    /* 
     * Èç¹ûÊÇm3boot·ÖÇø£¬ÉèÖÃoob_len_sel = 0; else£¬oob_len_sel = 1
     */
    ptable = find_partition_by_name(PTABLE_M3BOOT_NM);
    if(!ptable)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("find m3boot partition failed\n"));
        return;
    }

    flash_addr = ptable->offset + ptable->capacity;
    host->addr_to_sel_oob_len = (flash_addr >> host->nandchip->spec.pageshift) << NANDC_COLUMN_SHIFT;
        
    return;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_get_ecc_ability() - get information of ecc
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @chip - [input]the main structure.
 * @maxeccinfo - [input/output]the data structure to store the ecc information.
 *
 * DESCRIPTION:
 * This function gets  ecc information.
 *
 * CALL FUNC:
 * nandc_host_set_partition() -
 *
 ********************************************************************************/
u32 nandc_host_get_ecc_ability(struct nandc_host  *host,  struct nandc_nand  *chip,struct nandc_ecc_info** maxeccinfo)
{
    struct nandc_ecc_info* ecc_table =  host->eccinfo;
    u32 oobsize = chip->spec.sparesize ;
    u32 pagesize = chip->spec.pagesize;

    while(NULL != ecc_table->layout  )
    {
        if((pagesize == ecc_table->pagesize ) && (oobsize >= ecc_table->sparesize))
        {
            *maxeccinfo = ecc_table;
            return NANDC_OK;
        }
        ecc_table++;
    }
    NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_host_get_ecc_ability error oobsize:%d, pagesize:0x%x \n",
                                        oobsize, pagesize));
    return NANDC_ERROR;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_init_regset() - initlize the registers of the nand flash chip.
 *
 * PARAMETER:
 * @reg_tbl - [input]the pointer to register descriptor list.
 * @nandc_bit_set - [input]the number of "nandc_bit_cluster_normal_template"
 *
 * DESCRIPTION:
 * This function initlizes the registers of the nand flash chip.
 * and initializes members of "nandc_bit_cluster_normal_template".
 *
 * CALL FUNC:
 * nandc_host_init_cluster() -
 *
 ********************************************************************************/
u32 nandc_host_init_regset(struct nandc_reg_desc* reg_tbl, struct nandc_bit_reg* nandc_bit_set )
{
    struct nandc_reg_desc* table = reg_tbl;
    struct nandc_reg_cont* reg_cont = NANDC_NULL;
    u32 target = nandc_bit_set->bits_func;


    while(NANDC_NULL !=  table->content)
    {
        reg_cont = table->content;

        while(nandc_bits_enum_end !=  reg_cont->func  )
        {
            if(target == (u32)reg_cont->func )
            {
                /* set nand controller register's offset */
                nandc_bit_set->reg_offset = table->addr_offset;
                nandc_bit_set->bitset = &reg_cont->set;
                return NANDC_OK;
            }
            reg_cont++;
        }
        table++;
    }
    NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_host_init_regset return error: target = %d\n",target));
	return NANDC_ERROR;
}

 /*******************************************************************************
 * FUNC NAME:
 * find_in_table() - find special function number in function number table.
 *
 * PARAMETER:
 * @bitstbls - [input]function number table: nandc2_bitcmd,nandc3_bitcmd,nandc4_bitcmd
 * @value - [input]the expected function number.
 *
 * DESCRIPTION:
 * This function find special function number in function number table.nandc2_bitcmd for
 * V200 nand controller,nandc3_bitcmd for V320 nand controller,nandc4_bitcmd for V400 nand
 * controller,
 *
 * CALL FUNC:
 * nandc_host_init_cluster() -
 *
 ********************************************************************************/
static inline u32 find_in_table(u8* bitstbl, u32 value)
{
    while((u8)nandc_bits_enum_end != *bitstbl)
    {
        if(value == (u32)*bitstbl)
        {
            return NANDC_TRUE;
        }

        bitstbl++;
    }

    return NANDC_FALSE;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_init_cluster() - initialize registers.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @reg_tbl - [input] &nandc2_reg_desc_table[0] for V200 nand controller.
 *                    &nandc3_reg_desc_table[0] for V320 nand controller.
 *                    &nandc4_reg_desc_table[0] for V400 nand controller.
 * @bitstbl - [input] &nandc2_bitcmd[0] for V200 nand controller.
 *                    &nandc3_bitcmd[0] for V320 nand controller.
 *                    &nandc4_bitcmd[0] for V400 nand controller.
 *
 * DESCRIPTION:
 * This function initlizes the registers of the nand flash chip.
 * and initializes members of "nandc_bit_cluster_normal_template".
 *
 * CALL FUNC:
 * nandc_native_host_create() -
 *
 ********************************************************************************/
u32 nandc_host_init_cluster(struct nandc_host *host, struct nandc_reg_desc* reg_tbl, u8 *bitstbl)
{
    struct nandc_bit_reg* bitreg = NANDC_NULL;

    bitreg = (struct nandc_bit_reg*)&nandc_bit_cluster_normal_template;

    while((u32)nandc_bits_enum_end !=  bitreg->bits_func)
    {
        if(find_in_table(bitstbl,bitreg->bits_func))
        {
            if(NANDC_OK != nandc_host_init_regset(reg_tbl, bitreg))
            {
                goto ERRO;
            }
        }
        bitreg++;
    }

    host->normal = &nandc_bit_cluster_normal_template;

    return  NANDC_OK;

ERRO:

    return NANDC_ERROR;

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_get_nand_priv() - initialize nand_priv with host->nandchip.
 *
 * PARAMETER:
 * @chipselect - [input]returned with nand_chip->chipnum.
 * @nand_chip - [input]host->nandchip.
 * @nand_priv - [input]returned with host->nandchip.
 *
 * DESCRIPTION:
 * This function initializes nand_priv with host->nandchip.
 *
 * CALL FUNC:
 * nandc_host_set_partition() -
 *
 ********************************************************************************/
static inline u32 nandc_host_get_nand_priv(u32 *chipselect,  struct nandc_nand* nand_chip, struct nandc_nand **nand_priv)
{
    u32 i = nand_chip->chipnum;
    while(i > 0)
    {
        *nand_priv = (struct nandc_nand *)nand_chip;
        nand_priv++;
		i--;
    }

    *chipselect = nand_chip->chipnum;

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_set_partition() - initializes host->usrdefine
 *
 * PARAMETER:
 * @host - [input]through it get host->usrdefine.
 * @ptable - [input]partition table.
 * @nr_parts - [input]partition number.
 *
 * DESCRIPTION:
 * This function initializes host->usrdefine.
 *
 * CALL FUNC:
 * nandc_host_set_define() -
 *
 ********************************************************************************/
u32  nandc_host_set_partition(struct nandc_host * host, struct mtd_partition* ptable, u32 nr_parts)
{
    FSZ sizesum, sizemax;
    struct mtd_partition*  patbale = NANDC_NULL;
    struct nandc_nand*   p_nand[NANDC_MAX_CHIPS] = {0};
    struct nandc_ecc_info* maxecc = NANDC_NULL;
    struct nandc_define *ctrl_define = host->usrdefine;
    u32 totalchip ;
    u32 curchip;
    int i;

    nandc_host_get_nand_priv(&totalchip, host->nandchip, p_nand);

    NANDC_REJECT_NULL(ctrl_define);

    if(NANDC_NULL == ptable)
    {
        /*set default partition patbale*/
        patbale = (struct mtd_partition *)himalloc(totalchip * sizeof(struct mtd_partition));
        NANDC_REJECT_NULL(patbale);
        memset((void*)patbale, 0x00 , (totalchip *sizeof(struct mtd_partition)));

        ctrl_define->partition = patbale;
        ctrl_define->nbparts = totalchip;
        /*if no partition table give, one chip defult to has one partition*/
        NANDC_DO_ASSERT( totalchip <= NANDC_MAX_CHIPS, "too many chips", totalchip);

       for (curchip = 0; curchip < totalchip; curchip++)
       {
            if(NANDC_OK == nandc_host_get_ecc_ability(host, p_nand[curchip],  &maxecc))/*[false alarm]:ÆÁ±Îfortify´íÎó */
            {

               if(host->ecctype_onfi)
                {
                    p_nand[curchip]->ecctype = host->ecctype_onfi;

                }
                else
                {
                    p_nand[curchip]->ecctype = maxecc->ecctype;

                    /* check whether 4 bit ecc */
                    if(is_nand_id_4bitecc(host))
                    {
                        p_nand[curchip]->ecctype = NANDC_ECC_4BIT;
                    }
                }

                if(0 == curchip)
                {
                    patbale->size   =   p_nand[curchip]->spec.chipsize;
                    patbale->name   =   "partition0";
                    patbale->ecclayout = maxecc->layout;

                }
                else
                {

                    patbale->offset =  (patbale - 1)->offset + (patbale - 1)->size;
                    patbale->size =    p_nand[curchip]->spec.chipsize;
                    patbale->name = NANDC_NULL;
                    patbale->ecclayout = maxecc->layout;
                }
                patbale++;
            }
            else
            {
                NANDC_DO_ASSERT(0 ,"best ecc not find", curchip) ;
            }
       }

    }
    else
    {
       sizemax = 0;
       for (curchip = 0; curchip < totalchip; curchip++)
       {
            sizemax += p_nand[curchip]->spec.chipsize;/*[false alarm]:ÆÁ±Îfortify´íÎó */
            if(NANDC_OK == nandc_host_get_ecc_ability(host, p_nand[curchip],  &maxecc))/*[false alarm]:ÆÁ±Îfortify´íÎó */
            {
                if(host->ecctype_onfi)
                {
                    p_nand[curchip]->ecctype = host->ecctype_onfi;

                }
                else
                {
                    p_nand[curchip]->ecctype = maxecc->ecctype;

                    /* check whether 4 bit ecc */
                    if(is_nand_id_4bitecc(host))
                    {
                        p_nand[curchip]->ecctype = NANDC_ECC_4BIT;
                    }
                }
            }
            else
            {
                NANDC_DO_ASSERT(0 ,"best ecc not find", curchip) ;
            }
       }
       patbale = ptable;

        sizesum = 0;
        i = nr_parts;   /*lint !e713 */
        while(i > 0)
        {
              sizesum += patbale[i - 1].size;/*lint !e712*/

            /*modified for lint e794 */
            if (maxecc != NANDC_NULL)
            {
                patbale[i - 1].ecclayout  = maxecc->layout;
            }
            /*modified for lint e794 */

            i--;
        }

        NANDC_DO_ASSERT(((u32)sizesum <= (u32)sizemax), "partition total size too large!", (u32)sizesum);

        ctrl_define->partition = ptable;
        ctrl_define->nbparts = nr_parts;
    }

    host->curpart = ctrl_define->partition;
    return NANDC_OK;

ERRO:
    /* coverity[dead_error_condition] */
    if(NANDC_NULL != patbale)/*lint !e774 !e944 */
    {
        /* coverity[dead_error_line] */
        if(NANDC_NULL != patbale->name)
        {
            hifree(patbale->name);
        }
        hifree(patbale);
    }

    return NANDC_ERROR;
}/*lint !e550 */

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_get_pagemap() - return nandc_pagemap according ecctype and pagesize
 *
 * PARAMETER:
 * @lookuptbl - [input]    struct nandc_dmap   *datamap ;
 *                              u32    bad2k;                      / bad block flag location for 2k page/
 *                              struct nandc_pagemap pmap2k[1];    / data map for 2k page /
 *                                     struct nandc_distrib  data; / raw data location/
 *                                              u32 addr;          / raw data start address in one page(page size + oobsize) /
 *                                              u32 lens;          / length from offset /
 *                                     struct nandc_distrib  oob;  / oob data location/
 *
 *                              u32    bad4k;
 *                              struct nandc_pagemap pmap4k[2];
 *                              u32    bad8k;
 *                              struct nandc_pagemap pmap8k[4];
 *                         u32 ecctype;
 *
 * @ecctype - [input]get from nandc2_eccinfo,nandc3_eccinfo,nandc4_eccinfo
 * @pagesize - [input]get from id[] data which is read from nand flash chip.
 * @badoff - [input/output]bad block flag location.
 *
 * DESCRIPTION:
 * This function returns nandc_pagemap type according ecctype and pagesize.
 *
 * CALL FUNC:
 * nandc_host_set_chipparam() -
 *
 ********************************************************************************/
struct nandc_pagemap * nandc_host_get_pagemap(struct nandc_dmap_lookup  * lookuptbl,  u32 ecctype, u32 pagesize, u32* badoff)
{
    struct nandc_pagemap * findmap = NANDC_NULL;

    while(NANDC_NULL != lookuptbl->datamap)
    {
        if( ecctype ==  lookuptbl->ecctype)
        {
            switch(pagesize)
            {
                case NANDC_SIZE_2K:
                        findmap = lookuptbl->datamap->pmap2k;
                        *badoff = lookuptbl->datamap->bad2k;
                goto EXIT;

                case NANDC_SIZE_4K:
                        findmap = lookuptbl->datamap->pmap4k;
                        *badoff = lookuptbl->datamap->bad4k;
                goto EXIT;

                case NANDC_SIZE_8K:
                        findmap = lookuptbl->datamap->pmap8k;
                        *badoff = lookuptbl->datamap->bad8k;
                goto EXIT;

                default:

                 goto ERRO;
            }
        }
        lookuptbl++ ;
    }

    if(NANDC_NULL == lookuptbl->datamap)
    {
        goto ERRO;
    }

 EXIT:

    return findmap;

ERRO:
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_host_get_pagemap error,ecc: 0x%x, size:0x%x \n",ecctype, pagesize));
        NANDC_DO_ASSERT(0, HICHAR_NULL, 0);
       return NANDC_NULL;

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_get_pageset() - return pageset according to pagesize
 *
 * PARAMETER:
 * @pagesize - [input]get for id[] data which is read from nand flash chip.
 *
 * DESCRIPTION:
 * This function returns pageset according to pagesize.
 *
 * CALL FUNC:
 * nandc_host_set_chipparam() -
 *
 ********************************************************************************/
u32 nandc_host_get_pageset(u32 pagesize)
{
    u32 pageset ;
    switch(pagesize)
    {
        case NANDC_SIZE_HK:
            pageset = nandc_page_hk;
        break;
        case NANDC_SIZE_2K:
            pageset = nandc_page_2k;
        break;
        case NANDC_SIZE_4K:
            pageset = nandc_page_4k;
        break;
        case NANDC_SIZE_8K:
            pageset = nandc_page_8k;
        break;
        default:
            pageset = nandc_page_2k;
            NANDC_DO_ASSERT(0, HICHAR_NULL, 0);
            break;
      }

    return pageset;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_set_chipparam() - initialize host->nandchip->datamap and host->nandchip->pageset.
 *
 * PARAMETER:
 * @host - [input]through it to get host->dmaplookup and host->nandchip
 *                host->dmaplookup : nandc2_dmap_lookup
 *                                   nandc3_dmap_lookup
 *                                   nandc4_dmap_lookup
 *
 * DESCRIPTION:
 * This function initializes host->nandchip->datamap and host->nandchip->pageset.
 *
 * CALL FUNC:
 * nandc_host_init_mtd() -
 *
 ********************************************************************************/
u32 nandc_host_set_chipparam(struct nandc_host * host)
{
    /* host->dmaplookup is initialized by "nandc_native_host_create"*/
    struct nandc_dmap_lookup  *lookup   = host->dmaplookup;
    struct nandc_nand    *nand_chip     = host->nandchip;
    u32 ecctype;
    u32 pagesize;

    ecctype   =  nand_chip->ecctype;
    pagesize  =  nand_chip->spec.pagesize;

    nand_chip->datamap = nandc_host_get_pagemap(lookup, ecctype, pagesize, &host->badoffset);
    nand_chip->pageset = nandc_host_get_pageset(nand_chip->spec.pagesize);

    nand_chip->busset   =   (NAND_BUSWIDTH_16 == nand_chip->spec.buswidth )?nandc_bus_16:nandc_bus_08;

    NANDC_TRACE(NFCDBGLVL(NORMAL), ("chip(%d) ,ecc:0x%x, size:0x%x \n",nand_chip->chipnum, ecctype, pagesize));

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_set_define() - malloc space for host->usrdefine and initialize it
 *
 * PARAMETER:
 * @host - [input]throuth it get host->usrdefine.
 * @ptable - [input]partition table.
 * @nr_parts - [input]partition number.
 *
 *
 * DESCRIPTION:
 * This function mallocs space for host->usrdefine and initializes it by calling
 * "nandc_host_set_partition"
 *
 * CALL FUNC:
 * nandc_host_init_mtd() -
 *
 ********************************************************************************/
u32  nandc_host_set_define(struct nandc_host * host, struct mtd_partition* ptable, u32 nr_parts)
{
    struct nandc_define * ldefine = NANDC_NULL ;

    if(!host->usrdefine)
    {
        ldefine = (struct nandc_define*)himalloc(sizeof(struct nandc_define));

        NANDC_REJECT_NULL(ldefine);

        /*modified for lint e516 */
        memset((void*)ldefine, 0x00, (int)sizeof(struct nandc_define));

        ldefine->feature = NANDC_NULL;

        host->usrdefine =  ldefine;
    }


    return nandc_host_set_partition( host,  ptable,  nr_parts);

ERRO:
    return NANDC_E_NOMEM;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_add_nand_chip() - initialize host->nandchip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @addnum - [input]number of nand flash chips.
 * @spec - [input]information about nand flash chip which is get from id[] data.
 *
 *
 * DESCRIPTION:
 * This function initializes host->nandchip.
 *
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
u32 nandc_host_add_nand_chip(struct nandc_host * host,  u32 addnum, struct nandc_spec *spec)
{
    struct nandc_nand  *nand = NANDC_NULL;
    u32 errocode = NANDC_ERROR;
    u32 pagesize, erasesize, buswidth, sparesize;
	FSZ chipsize;

    pagesize  = spec->pagesize;
    erasesize = spec->blocksize;
    buswidth  = spec->buswidth;
    chipsize  = spec->chipsize;
    sparesize = spec->sparesize;

    nand = (struct nandc_nand*)himalloc(sizeof(struct nandc_nand));

    NANDC_REJECT_NULL(nand);
    memset(nand, 0, sizeof(struct nandc_nand));

    nand->spec.name         =   spec->name;
    nand->spec.pagesize     =   NANDC_CHECK_PAGESIZE(pagesize);
    nand->spec.blocksize    =   NANDC_CHECK_ERASESIZE(erasesize);
    nand->spec.buswidth     =   NANDC_CHECK_BUSWIDE(buswidth);
    nand->spec.chipsize     =   NANDC_CHECK_CHIPSIZE(chipsize);
    nand->spec.sparesize    =   NANDC_CHECK_SPARESIZE(sparesize);

    if(0 == nand->spec.pagesize 
        || 0 == nand->spec.sparesize 
        || 0 == nand->spec.blocksize 
        || 0 == nand->spec.chipsize )
    {
        /* coverity[printf_arg_mismatch] */
        NANDC_TRACE(NFCDBGLVL(ERRO), ("create nand error param: pagesize:0x%x, sparesize:0x%x\n erasesize:0x%x, buswidth:0x%x, chipsize:0x%x\n",
                                                         pagesize, sparesize, erasesize, buswidth, (unsigned int)chipsize));
		errocode = NANDC_E_PARAM;
        goto ERRO;
    }

    nand->spec.offinpage    =   pagesize - 1;
    nand->spec.pagealign    =   ~(nand->spec.offinpage);
    nand->spec.offinblock   =   erasesize - 1;
    nand->spec.blockalign   =   ~(nand->spec.offinblock);
	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
	nand->spec.pageshift    =   (u32)(ffs(nand->spec.pagesize) - 1);/*lint !e713 !e732*/
	nand->spec.blockshift   =   (u32)(ffs(nand->spec.blocksize) - 1);/*lint !e713 !e732*/
	/*end*/
    nand->spec.blockpages   =   erasesize >> nand->spec.pageshift; /* page number in one block*/
    nand->spec.blockmask    =   (u32)(chipsize >> nand->spec.blockshift) - 1;
    nand->spec.pagesum      =   nand->spec.blockmask * nand->spec.blockpages;
	nand->spec.pagemask     =   (u32)(chipsize >> nand->spec.pageshift) - 1;
	if (chipsize & 0xffffffff)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
		nand->spec.chipshift = (u32)(ffs((unsigned)chipsize) - 1);/*lint !e713 !e732*/
		/*end*/
    }
    else
    {
#ifdef NANDC_NAND_SIZE_WIDTH64
        nand->spec.chipshift = ffs((unsigned)(chipsize >> 32)) + 32 - 1;
#endif

    }
    if(chipsize > nandc_size_1Gbit)
    {
        nand->spec.addrcycle = NANDC_ADDRCYCLE_5;
    }
    else
    {
        nand->spec.addrcycle = NANDC_ADDRCYCLE_4;
    }

    nand->spec.options      =   NANDC_NULL;


    nand->chipnum  = addnum;
    host->nandchip = nand;

    /* just for 8bit ecc */
    set_phyaddr_low_8bit(host);

    return NANDC_OK;

ERRO:

    if(NANDC_NULL != nand)
    {
        hifree(nand);
    }

    NANDC_DO_ASSERT(0, "nandc create nand error!",errocode);
    return errocode;

}

#ifdef __KERNEL__
 /*******************************************************************************
 * FUNC NAME:
 * nandc_host_init_mtd() - initialize the data structure of MTD layer.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @pagesize - [input]page size of nand flash chip.(2048,4096,8192 etc)
 * @sparesize - [input]spare area size of nand flash chip.(64,128,258 etc)
 * @chipsize - [input]total size of nand flash chip.(128M,258M etc)
 * @erasesize - [input]erase size of nand flash chip,the same as block size.
 * @buswidth - [input]busy width (8 or 16).
 * @numchips - [input]chip acount.
 * @ptable - [input] partition table.
 * @nr_parts - [input]partition acount.(partitions in all nand flash chips is arranged in turn)
 *
 * DESCRIPTION:
 * This function initializes the data structure of MTD layer.
 *
 * CALL FUNC:
 * nandc_mtd_nand_init() -
 *
 ********************************************************************************/
u32  nandc_host_init_mtd(struct nandc_host * host,
                                    u32 pagesize,
                                    u32 sparesize,
                                    u32 chipsize,
                                    u32 erasesize,
                                    u32 buswidth,
                                    u32 numchips,
                                    struct mtd_partition  * ptable,
                                    u32 nr_parts
                                    )
{
    struct nandc_spec spec;
    u32 errocode = NANDC_ERROR;

    memset(&spec, 0, sizeof(struct nandc_spec));

    spec.pagesize  = pagesize;
    spec.sparesize = sparesize;
    spec.blocksize = erasesize;
    spec.buswidth  = buswidth;
    spec.chipsize  = chipsize;

    errocode = nandc_host_add_nand_chip( host,  numchips, &spec);
    if(NANDC_OK != errocode)
    {
        return errocode;
    }

    if(NANDC_OK == nandc_host_set_define(host, ptable, nr_parts))
    {
        return nandc_host_set_chipparam(host);
    }

    return NANDC_ERROR;

}
#endif

#ifdef __cplusplus
}
#endif
