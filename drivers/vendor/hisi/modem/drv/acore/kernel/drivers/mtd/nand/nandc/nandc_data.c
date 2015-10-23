
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"

 /*******************************************************************************
 * FUNC NAME:
 * nandc_data_transmit_page() -
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @pagemap - [input]data map in one page
 * @times - [input]the times it needs to transfer data between nand controller buffer
 *                 and ram buffer.because Hisi nand controller buffer is 2k~3k large,when
 *                 we do operations to nand flash chip with 4k page,we need 2 times data
 *                 transmission between nand controller buffer and nand flash chip,as the
 *                 same,when we do operation to nand flash chip with 8k page,we need 3 times
 *                 data transmission between nand controller buffer and nand flash chip.
 * @access - [input] NAND_CMD_READSTART : reading operation to nand flash chip.
 *                   NAND_CMD_PAGEPROG  : writing operation to nand flash chip.
 *
 * DESCRIPTION:
 * This functions transfers data between ram and nand controller buffer.
 * reading operation : from nand controller buffer to ram buffer.
 *   nand flash chip           nand controller buffer     ram buffer
 *   ----------------          ----------------         ----------------
 *  |                |  ----> |  host->bufbase | --->  | host->databuf  |
 *   ----------------          ----------------         ----------------
 *
 * writing operation : from ram buffer to nand controllers buffer.
 *   nand flash chip           nand controller buffer     ram buffer
 *   ----------------          ----------------         ----------------
 *  |                |  <---- |  host->bufbase | <---  | host->databuf  |
 *   ----------------          ----------------         ----------------
 * for __KERNEL__ platform :host->databuf == host->bufbase
 * for other platform eg : __FASTBOOT__ ,__VXWORKS__,__RVDS__
 * host->databuf != host->bufbase
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
u32 nandc_data_transmit_page(struct nandc_host *host, struct nandc_pagemap *pagemap, u32 times, u32 access)
{/*lint !e578 */
    u32 bufoffset;
    u32 ramoffset;
    u32 copylen;
    u32 i;

    if(host->databuf == host->bufbase)
    {
        return NANDC_OK;
    }

    /*copy page data*/
    copylen = pagemap[times].data.lens;

    if((0 != copylen) && (NANDC_NULL != host->databuf))
    {
		bufoffset = pagemap[times].data.addr;
        i = 0;
        ramoffset = 0;
        while(i < times)
        {
            ramoffset += pagemap[i].data.lens;
            i++;
        }

        if(NAND_CMD_READSTART == access)
        {
            memcpy((void*)((u32)host->databuf + ramoffset), (const void*)((u32)(host->bufbase) + bufoffset), copylen);
        }
        else /*NAND_CMD_PAGEPROG*/
        {
            memcpy((void*)((u32)(host->bufbase) + bufoffset),(const void*)((u32)host->databuf + ramoffset),  copylen);
        }
    }

    /*copy oob data*/
    copylen     =   pagemap[times].oob.lens;
	bufoffset   =   pagemap[times].oob.addr ;

    if((0 != copylen)&&(NANDC_NULL != host->oobbuf))
    {
        i = 0;
        ramoffset = 0;
        while(i < times)
        {
            ramoffset += pagemap[i].oob.lens;
            i++;
        }

        if(NAND_CMD_READSTART == access)
        {
            memcpy((void*)((u32)host->oobbuf + ramoffset), (const void*)((u32)host->bufbase + bufoffset), copylen);
#ifdef __KERNEL__
			memcpy((void*)((u32)host->databuf + host->nandchip->spec.pagesize), (const void*)((u32)host->bufbase + bufoffset), copylen);
#endif
		}
        else /*NAND_CMD_PAGEPROG*/
        {
#ifdef __KERNEL__
			/* change for 4K Nand */
            memcpy(host->oobbuf + ramoffset, (const void*)((u32)host->databuf + host->nandchip->spec.pagesize), copylen);
#endif
            memcpy((void*)((u32)host->bufbase + bufoffset), (const void*)((u32)host->oobbuf + ramoffset), copylen);
        }
    }
    else if((0 != copylen)&&(NAND_CMD_READSTART != access))
    {
        /*纯数据写(不填充spare区 )模式下确保坏块标志不为0*/
        memset((void*)((u32)host->bufbase + bufoffset), 0xff , copylen);

    }

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_data_transmit_raw() -
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @datasize - [input]the length of raw data and oob data.
 * @bufoffset - [input]offset form host->databuf's base address.
 * @access - [input] NAND_CMD_READSTART : reading operation to nand flash chip.
 *                   NAND_CMD_PAGEPROG  : writing operation to nand flash chip.
 *
 * DESCRIPTION:
 * This functions transfers data between ram and nand controller buffer.
 * reading operation : from nand controller buffer to ram buffer.
 *   nand flash chip           nand controller buffer     ram buffer
 *   ----------------          ----------------         ----------------
 *  |                |  ----> |  host->bufbase | --->  | host->databuf  |
 *   ----------------          ----------------         ----------------
 *
 * writing operation : from ram buffer to nand controllers buffer.
 *   nand flash chip           nand controller buffer     ram buffer
 *   ----------------          ----------------         ----------------
 *  |                |  <---- |  host->bufbase | <---  | host->databuf  |
 *   ----------------          ----------------         ----------------
 * for __KERNEL__ platform :host->databuf == host->bufbase
 * for other platform eg : __FASTBOOT__ ,__VXWORKS__,__RVDS__,__BOOTLOADER__
 * host->databuf != host->bufbase
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
u32 nandc_data_transmit_raw(struct nandc_host *host, u32 datasize, u32 bufoffset, u32 access)
{/*lint !e578 */
    if(host->databuf == host->bufbase)
    {
        return NANDC_OK;
    }

    if(NAND_CMD_READSTART == access)
    {
        memcpy(host->databuf + bufoffset, host->bufbase, datasize);
    }
    else /*NAND_CMD_PAGEPROG*/
    {
        memcpy( host->bufbase, host->databuf + bufoffset, datasize);
    }

    return NANDC_OK;
}

#ifdef __cplusplus
}
#endif


