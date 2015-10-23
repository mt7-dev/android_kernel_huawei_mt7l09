
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


/**
* 作用:此函数主要是在不使能DMA功能时且在使能ECC功能时读整个页面时调用,因为可能出现nandc的硬件缓冲器的空间小于
*	   Flash页面的大小时就要多次传送
*
* 参数: 
* @host                ---nandc_host的结构体指针
* @pagemap             ---一页的flash数据的格式
* @times               ---表示现在是传送的第几次
* @access              ---表示是读操作还是写操作
*/
u32 nandc_data_transmit_page(struct nandc_host *host, struct nandc_pagemap *pagemap, u32 times, u32 access)
{
	/*lint !e578 */
	u32 bufoffset;
	u32 ramoffset;
	u32 copylen;
	u32 i;
	/*如果与硬件缓冲区的地址相同就不用复制数据了*/
	if(host->databuf == host->bufbase)
	{
		return NANDC_OK;
	}

	/*复制数据区*/
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
			/*如果是读操作那么就把硬件缓冲区中的数据复制到databuf中去*/
		    memcpy((void*)((u32)host->databuf + ramoffset), (const void*)((u32)(host->bufbase) + bufoffset), copylen);
		}
		else 
		{
			/*如果是写操作那么把要写的数据复制到硬件缓冲区中*/
		    memcpy((void*)((u32)(host->bufbase) + bufoffset),(const void*)((u32)host->databuf + ramoffset),  copylen);
		}
	}

	/*复制OOB区的数据*/
	copylen     =   pagemap[times].oob.lens;
	/*OOB区数据开始的地方*/
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
			/*复制OOB的数据到用户内存中*/
		    memcpy((void*)((u32)host->oobbuf + ramoffset), (const void*)((u32)host->bufbase + bufoffset), copylen);
		}
		else /*NAND_CMD_PAGEPROG*/
		{
			/*把OOB的数据写硬件缓冲区中去*/
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


/**
* 作用:此函数主要是在不使能DMA功能时且在不使能ECC功能时裸读数据时调用,因为可能出现nandc的硬件缓冲器的空间小于
*	   Flash页面的大小时就要多次传送
*
* 参数: 
* @host                    ---nandc_host的结构体指针
* @datasize                ---要传送的数据的长度
* @bufoffset               ---bufoffset表示用户数据中的偏移
* @access                  ---表示是读操作还是写操作
*/
u32 nandc_data_transmit_raw(struct nandc_host *host, u32 datasize, u32 bufoffset, u32 access)
{
	/*lint !e578 */
	if(host->databuf == host->bufbase)
	{
		return NANDC_OK;
	}
	/*读nand的操作*/
	if(NAND_CMD_READSTART == access)
	{
		memcpy(host->databuf + bufoffset, host->bufbase, datasize);
	}
	else /*写nand的操作*/
	{
		memcpy( host->bufbase, host->databuf + bufoffset, datasize);
	}

	return NANDC_OK;
}

#ifdef __cplusplus
}
#endif


