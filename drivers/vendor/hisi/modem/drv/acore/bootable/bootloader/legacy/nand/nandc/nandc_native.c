/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
*
* Filename:    nandc_native.c
* Description: nandc控制器的自身的一些相关操作放在此文件中
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
*
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"


/*lint -save -e767*/
#define NFCDBGLVL(LVL)   (NANDC_TRACE_NATIVE|NANDC_TRACE_##LVL)
/*lint -restore*/

#ifdef __FASTBOOT__
#include <hi_nandc.h>

#define le8_to_cpu(x) (x)
#define le16_to_cpu(v) (v)
#define le32_to_cpu(v) (v)

/*****************************************************************************
* 函 数 名  : onfi_crc16
*
* 功能描述  : 对数据做crc校验
*
* 输入参数  : @crc - crc code
*			  @p - 要校验的数据地址
*			  @len - 要校验的数据长度
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  : from Linux kernel 3.10
*
*****************************************************************************/
u16 onfi_crc16(u16 crc, u8 const *p, u32 len)
{
	int i;
	while (len--) {
		crc ^= *p++ << 8;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x8005 : 0);
	}

	return crc;
}

/*****************************************************************************
* 函 数 名  : find_full_id_nand
*
* 功能描述  : 判断是否是full id nand，如果是，则获取full id nand的spec info
*
* 输入参数  : @spec_real - nand spec info
*			  @type - nand flash设备结构体
*			  @id_data - flash id值
* 输出参数  : null
*
* 返 回 值  :NANDC_TRUE - 是；NANDC_FALSE - 不是
*
* 其它说明  :
*
*****************************************************************************/
int find_full_id_nand(struct nandc_spec *spec, const struct nand_flash_dev *type, u8 *id_data)
{
	if (!memcmp(type->id, id_data, type->id_len))
	{
		spec->pagesize = type->pagesize;
		spec->blocksize = type->erasesize;
		spec->sparesize = type->oobsize;
        spec->chipsize = (unsigned long long)type->chipsize << 20;

		return NANDC_TRUE;
	}
	return NANDC_FALSE;
}

/*****************************************************************************
* 函 数 名  : is_full_id_nand
*
* 功能描述  : 获取nand flash的id长度。full nand的id长度为8
*
* 输入参数  : @type - nand flash设备结构体，对于full id nand，存储有id实际长度，其余nand，该字段为0
* 输出参数  : null
*
* 返 回 值  : id长度
*
* 其它说明  : from Linux kernel 3.10
*
*****************************************************************************/
int is_full_id_nand(const struct nand_flash_dev *type)
{
	return type->id_len;
}

/*****************************************************************************
* 函 数 名  : nand_id_has_period
*
* 功能描述  : Check if an ID string has a given wraparound period
*
* 输入参数  : @id_data: the ID string
*			  @arrlen: the length of the @id_data array
* 			  @period: the period of repitition
* 输出参数  : null
*
* 返 回 值  :NANDC_TRUE - 有周期；NANDC_FALSE - 没有周期
*
* 其它说明  : from Linux kernel 3.10
*
*****************************************************************************/
int nand_id_has_period(unsigned char *id_data, int arrlen, int period)
{
	int i, j;

	for (i = 0; i < period; i++)
	{
		for (j = i + period; j < arrlen; j += period)
		{
			if (id_data[i] != id_data[j])
			{
				return NANDC_FALSE;
			}
		}
	}
	return NANDC_TRUE;
}

/*****************************************************************************
* 函 数 名  : nand_id_len
*
* 功能描述  : Get the length of an ID string returned by CMD_READID
*
* 输入参数  : @id_data: the ID string
* 			  @arrlen: the length of the @id_data array
* 输出参数  : null
*
* 返 回 值  : nand id 长度
*
* 其它说明  : from Linux kernel 3.10
*
*****************************************************************************/
int nand_id_len(unsigned char *id_data, int arrlen)
{
	int last_nonzero, period;

	/* Find last non-zero byte */
	for (last_nonzero = arrlen - 1; last_nonzero >= 0; last_nonzero--)
	{
		if (id_data[last_nonzero])
		{
			break;
		}
	}

	/* All zeros */
	if (last_nonzero < 0)
	{
		return 0;
	}

	/* Calculate wraparound period */
	for (period = 1; period < arrlen; period++)
	{
		if (nand_id_has_period(id_data, arrlen, period))
		{
			break;
		}
	}

	/* There's a repeated pattern */
	if (period < arrlen)
	{
		return period;
	}

	/* There are trailing zeros */
	if (last_nonzero < arrlen - 1)
	{
		return last_nonzero + 1;
	}

	/* No pattern detected */
	return arrlen;
}

/*****************************************************************************
* 函 数 名  : nand_wait_result
*
* 功能描述  : 查询op_done中断，查询次数为time_out
*
* 输入参数  : @time_out - 查询次数
* 输出参数  : null
*
* 返 回 值  : OK - op_done中断上报；NAND_WAIT_TIMEOUT - 查询超时
*
* 其它说明  :
*
*****************************************************************************/
s32 nand_wait_result(u32 value, u32 time_out)
{
	u32 count = time_out;
	do{
        if(get_hi_nfc_ints_op_done() == value)
		{
			return NANDC_OK;
		}
		count--;
	}while(count > 0);

	return NANDC_E_TIMEOUT;
}

/*****************************************************************************
* 函 数 名  : nand_readbuffer
*
* 功能描述  : 从nandc buffer起始地址处读取长度为length的数据到dest地址
*
* 输入参数  : @dest - 目的地址
*			  @length - 数据长度
* 输出参数  : null
*
* 返 回 值  : OK - 读取成功； NAND_READSIZE_TOO_LARGE - 长度超过nandc buffer大小
*
* 其它说明  :
*
*****************************************************************************/
s32 nand_readbuffer(u8 *dest, u32 length)
{
	if(length > NANDC_BUFSIZE_BASIC)
    {
		return NANDC_E_PARAM;
    }

	memcpy((void *)dest, (void *)NANDC_BUFFER_BASE_ADDR, (s32)length);
	return NANDC_OK;
}

/*****************************************************************************
* 函 数 名  : nand_readbuffer_byte
*
* 功能描述  : 从nandc buffer的offset偏移位置读取1字节数据
*
* 输入参数  : @offset - nandc buffer偏移
* 输出参数  : null
*
* 返 回 值  : offset偏移处的数据
*
* 其它说明  :
*
*****************************************************************************/
u8 nand_readbuffer_byte(u32 offset)
{
	return *((u8*)(NANDC_BUFFER_BASE_ADDR + offset));
}

/*****************************************************************************
* 函 数 名  : nand_send_cmd_onfi_param
*
* 功能描述  : 向nand flash发读取ONFI param的命令
*
* 输入参数  : null
* 输出参数  : null
*
* 返 回 值  : OK - 成功； else - 失败
*
* 其它说明  :
*
*****************************************************************************/
s32 nand_send_cmd_onfi_param(void)
{
	s32 ret;
	/* set command */
	set_hi_nfc_cmd_cmd1(NAND_CMD_PARAM);

	/* set addr 0, low addr must be 0 as nand flash chip required */
	set_hi_nfc_addrl_addr_l(0);
	set_hi_nfc_addrh_addr_h(0);

	/* set ecc type 0 */
	set_hi_nfc_con_ecc_type(NANDC_ECC_NONE);

    /* set data length */
	set_hi_nfc_data_num_nfc_data_num(sizeof(struct nand_onfi_params));

	/* set op param and addr cycle 1 */
	NANDC_REG_WRITE32(NANDC_REG_BASE_ADDR + HI_NFC_OP_OFFSET, NAND_OP_READ_PARAM | (NANDC_ADDRCYCLE_1 << NANDC_ADDRBIT_OFFSET));

	/* wait result */
	ret = nand_wait_result(NANDC_STATUS_OP_DONE, NANDC_RESULT_TIME_OUT);
	if(ret)
	{
		return ret;
	}

	/* int status clear */
	NANDC_REG_WRITE32(NANDC_REG_BASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INT_CLEAR_ALL);

	return NANDC_OK;
}

/*****************************************************************************
* 函 数 名  : nand_send_cmd_id
*
* 功能描述  : 向nand flash发送读id命令
*
* 输入参数  : @addr - flash 地址
* 输出参数  : null
*
* 返 回 值  : OK - 成功； else - 失败
*
* 其它说明  :
*
*****************************************************************************/
s32 nand_send_cmd_id(u8 addr)
{
	s32 ret;

	/* reset nand flash */
	//nand_reset();

	/* set ecc type 0 */
	set_hi_nfc_con_ecc_type(NANDC_ECC_NONE);

    /* set data num */
	set_hi_nfc_data_num_nfc_data_num(NANDC_READID_SIZE);

	/* set command */
	set_hi_nfc_cmd_cmd1(NAND_CMD_READID);

    /* set addr */
	set_hi_nfc_addrl_addr_l(addr);
	set_hi_nfc_addrh_addr_h(0);

    /* set op read id and addr cycle 1 */
	NANDC_REG_WRITE32(NANDC_REG_BASE_ADDR + HI_NFC_OP_OFFSET, NAND_OP_READ_ID | (NANDC_ADDRCYCLE_1 << NANDC_ADDRBIT_OFFSET));

	/* wait result */
	ret = nand_wait_result(nandc_status_op_done, NANDC_RESULT_TIME_OUT);
	if(ret)
	{
        NANDC_TRACE(NFCDBGLVL(ERRO),("[%s]ERROR: wait result failed, ret = %d %d\n", ret));
		return ret;
	}

	/* int status clear */
	NANDC_REG_WRITE32(NANDC_REG_BASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INT_CLEAR_ALL);

	return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_bbm_real_addr() -
 *
 * PARAMETER:
 * @addr_virtual - [input]
 * @addr_real - [input]
 *
 * DESCRIPTION:
 *
 *
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
u32 nandc_bbm_real_addr(FSZ addr_virtual, FSZ *addr_real)
{
    *addr_real = addr_virtual;

    return NANDC_OK;
}


/**
* 作用:nandc模块提供对外复位nandflash芯片函数接口
*
* 参数:
* @host    		 ---nandc_host表示nandc主机控制器
* @chipselect    ---要复位nandflash芯片
*
*
* 描述:复位nandflash芯片
*
*/
u32 nandc_native_reset_chip(struct nandc_host* host , u32 chipselect)
{
    host->command = NAND_CMD_RESET;
    host->chipselect   = chipselect;

    return nandc_ctrl_entry(host);

}


/*****************************************************************************
* 函 数 名  : nandc_native_detect_onfi
*
* 功能描述  : 检测nand flash是否支持ONFI，如果支持，则读取param页并获取nand spec info
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : OK - 成功； else - 失败
*
* 其它说明  : from Linux kernel 3.10 and modified
*
*****************************************************************************/
#define ONFI_ECC_4BIT    4
#define ONFI_ECC_8BIT    8
s32 nandc_native_detect_onfi(struct nandc_host* host , struct nandc_spec * spec)
{
	struct nand_onfi_params p;
	s32 i, ret;
    u32 ecctype_onfi=0;

    if(!host)
    {
        return NANDC_E_PARAM;
    }

    if(!spec)
    {
        return NANDC_E_PARAM;
    }

	ret = nand_send_cmd_id(NAND_READ_ID_ONFI);
	if(ret)
	{
		return ret;
	}

	if (nand_readbuffer_byte(0) != 'O' || nand_readbuffer_byte(1) != 'N' ||
		nand_readbuffer_byte(2) != 'F' || nand_readbuffer_byte(3) != 'I')
	{
        NANDC_TRACE(NFCDBGLVL(ERRO),("Not ONFI Nand Flash\n"));
		return NANDC_E_READID;
	}

	for (i = 0; i < 3; i++)
	{
        ret = nand_send_cmd_onfi_param();
        if(ret)
        {
            return ret;
        }
		ret = nand_readbuffer((u8 *)(&p), sizeof(p));
		if(ret)
		{
			NANDC_TRACE(NFCDBGLVL(ERRO),("Nand read buffer failed, ret = %d\n", ret));
			return ret;
		}

		if(onfi_crc16(ONFI_CRC_BASE, (u8 *)(&p), 254) == le16_to_cpu(p.crc))
		{
			break;
		}
	}

	if (3 == i)
	{
		NANDC_TRACE(NFCDBGLVL(ERRO),("Nand ONFI CRC error\n"));
		return NANDC_E_CRC;
	}

    ecctype_onfi = le8_to_cpu(p.ecc_bits);
    if( ecctype_onfi == ONFI_ECC_4BIT)
    {
        host->ecctype_onfi = NANDC_ECC_4BIT;
    }
    else if( ecctype_onfi == ONFI_ECC_8BIT)
    {
        host->ecctype_onfi = NANDC_ECC_8BIT;
    }
    else
    {
        NANDC_TRACE(NFCDBGLVL(ERRO),("nandc_native_detect_onfi error %d\n",le8_to_cpu(p.ecc_bits)));
    }

	spec->pagesize= le32_to_cpu(p.byte_per_page);
	spec->blocksize= le32_to_cpu(p.pages_per_block) * le32_to_cpu(p.byte_per_page);
	spec->sparesize= le16_to_cpu(p.spare_bytes_per_page);
	spec->chipsize = le32_to_cpu(p.blocks_per_lun);
	spec->chipsize *= (u64)spec->blocksize * p.lun_count;

	return NANDC_OK;
}

/*******************************************************************************
 * FUNC NAME:
 * nandc_native_get_sparesize() - get information of sparesize
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @writesize - [input]page size of nand.
 * @ecctype - [input] ecc type of nandc chose
 * @oobsize - [output] min oobsize of nand
 * DESCRIPTION:
 * 这个函数可在已知nand flash的pagesize和ecctype的场景下，获取sparesize。仅用于东芝 SLC nand
 *
 * CALL FUNC:
 * nandc_host_set_partition() -
 *
 ********************************************************************************/
u32 nandc_native_get_sparesize(struct nandc_host  *host, u32 writesize, u32 ecctype, u32 *oobsize)
{
    struct nandc_ecc_info* ecc_table = NULL;

    if(!host)
    {
        return NANDC_E_PARAM;
    }

    ecc_table = host->eccinfo;
    while(NULL != ecc_table->layout)
    {
        if((writesize == ecc_table->pagesize ) && (ecctype == ecc_table->ecctype))
        {
            *oobsize = ecc_table->sparesize;
            return NANDC_OK;
        }
        ecc_table++;
    }

    return NANDC_ERROR;
}

/**
* 作用:nandc模块提供对外读Flash函的ID数据的函数接口
*
* 参数:
* @host    		 ---nandc主机控制器的指针
* @idbuf      	 ---存放Flash的ID数据的内存地址
* @length        ---Flash ID的数据的长度
*
*
* 描述:读Flash的ID数据放指定的内存中
*
*/
u32 nandc_native_read_id(struct nandc_host* host , u8* idbuf, u32 length)
{

    if(length > NANDC_READID_SIZE)
    {
        NANDC_TRACE( NFCDBGLVL(ERRO),("read id length error: %d",length ));
        NANDC_DO_ASSERT(0, "", 0 );

        return NANDC_ERROR;
    }

    host->command = NAND_CMD_READID;
    host->databuf = idbuf;

    return nandc_ctrl_entry(host);

}


/**
* 作用:通过得到的NandFlash的ID数据来解析nandflash的规格参数并填充在相关的数据结构中去
*
* 参数:
* @host    		 ---nandc主机控制器的指针
* @id_data       ---存放Flash的ID数据
* @spec          ---把解析的nandflash的规格参数填充在spec参数中去
*
*
* 描述:读Flash的ID数据放指定的内存中
* 注意:id_data的数据格式如下
* ID datas from nand chip.
*  id[0]: manufacture id
*  id[1]: device id.
*  id[3]:the most important byte.bits function as follow:
*  --------------------------------------------------------------
*  | item       | description  |  7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
*  |-------------------------------------------------------------
*  |            |     1KB      |    |   |   |   |   |   | 0 | 0 |
*  | Page Size  |     2KB      |    |   |   |   |   |   | 0 | 1 |
*  |            |     4KB      |    |   |   |   |   |   | 1 | 0 |
*  |            |     8KB      |    |   |   |   |   |   | 1 | 1 |
*  |-------------------------------------------------------------
*  |            |    64KB      |    |   |   |   | 0 | 0 |   |   |
*  | Block Size |   128KB      |    |   |   |   | 0 | 1 |   |   |
*  |            |   256KB      |    |   |   |   | 1 | 0 |   |   |
*  |            |   512KB      |    |   |   |   | 1 | 1 |   |   |
*  |-------------------------------------------------------------
*  |            |     8        |    |   | 0 | 0 |   |   |   |   |
*  | Redundant  |    16        |    |   | 0 | 1 |   |   |   |   |
*  | Area Size  |   Reserved   |    |   | 1 | 0 |   |   |   |   |
*  | (Byte/512) |   Reserved   |    |   | 1 | 1 |   |   |   |   |
*  |-------------------------------------------------------------
*  |Organization|       8      |    | 0 |   |   |   |   |   |   |
*  |            |      16      |    | 1 |   |   |   |   |
|   |
*  |-------------------------------------------------------------
*  | Reserved   |              |0or1|   |   |   |   |   |   |   |
*  |-------------------------------------------------------------
*/
u32 nandc_native_get_spec(struct nandc_host* host, u8* id_data, struct nandc_spec *spec)
{
    struct nand_flash_dev *type = NANDC_NULL;
    u32 dev_id, maf_id, ret = NANDC_ERROR;
    u32 writesize = 0, erasesize = 0, maf_idx, oobsize = 0;
    FSZ chipsize;
    u8  cellinfo = 0, id_len = nand_id_len(id_data, NANDC_READID_SIZE);

    maf_id = id_data[0];
    dev_id = id_data[1];

    /*Copy and modified from linux2.6.35 mtd code
    look up for the achieved ID in flash IDs table.*/
    type = nand_flash_ids;

    /* check if it's full id nand */
	for (; type&&type->name != NULL; type++)
	{
		if (is_full_id_nand(type))
		{
			if (find_full_id_nand(spec, type, id_data))
			{
				goto DONE;
			}
		}
        else if (id_data[1] == type->dev_id)
        {
			break;
		}
	}

    /* detect ONFI */
	if(!nandc_native_detect_onfi(host,spec))
	{
		goto DONE;
	}

    if (!type || !type->name)
    {
        NANDC_TRACE( NFCDBGLVL(ERRO),("find dev by id  error ,maf_idx: 0x%x, dev_id:0x%x\n",maf_id, dev_id));
		return NANDC_E_READID;
    }

    chipsize = (FSZ)type->chipsize << 20;

    /* Newer devices have all the information in additional id bytes */
    if (!type->pagesize)
    {
        int extid;
        /* The 3rd id byte holds MLC / multichip data */
        cellinfo = id_data[2];
        /* The 4th id byte is the important one */
        extid = id_data[3];

        /*
        * Field definitions are in the following datasheets:
        * Old style (4,5 byte ID): Samsung K9GAG08U0M (p.32)
        * New style   (6 byte ID): Samsung K9GAG08U0D (p.40)
        *
        * Check for wraparound + Samsung ID + nonzero 6th byte
        * to decide what to do.
        */
        if (id_data[0] == id_data[6] && id_data[1] == id_data[7] &&
            id_data[0] == NAND_MFR_SAMSUNG &&
            (cellinfo & NAND_CI_CELLTYPE_MSK) &&
            id_data[5] != 0x00)
        {
    		/* Calc pagesize */
    		writesize= NANDC_SIZE_2K << (extid & 0x03);
    		extid >>= 2;

    		/* Calc oobsize */
    		switch (((extid >> 2) & 0x04) | (extid & 0x03)) {
    		case 1:
    			oobsize = NANDC_SPARE_LENGTH_128BYTE;
    			break;
    		case 2:
    			oobsize = NANDC_SPARE_LENGTH_218BYTE;
    			break;
    		case 3:
    			oobsize = NANDC_SPARE_LENGTH_400BYTE;
    			break;
    		case 4:
    			oobsize = NANDC_SPARE_LENGTH_436BYTE;
    			break;
    		case 5:
    			oobsize = NANDC_SPARE_LENGTH_512BYTE;
    			break;
    		case 6:
    		default:
    			oobsize = NANDC_SPARE_LENGTH_640BYTE;
    			break;
    		}
    		extid >>= 2;

    		/* Calc blocksize */
    		erasesize = ((NANDC_BLOCK_128PAGE * NANDC_SIZE_1K) <<
    			(((extid >> 1) & 0x04) | (extid & 0x03)));
	    }
		else if (id_len == 6 && id_data[0] == NAND_MFR_HYNIX &&
			(cellinfo & NAND_CI_CELLTYPE_MSK))
    	{
        	unsigned int tmp;

        	/* Calc pagesize */
        	writesize = NANDC_SIZE_2K << (extid & 0x03);
        	extid >>= 2;

        	/* Calc oobsize */
        	switch (((extid >> 2) & 0x04) | (extid & 0x03)) {
        	case 0:
        		oobsize = NANDC_SPARE_LENGTH_128BYTE;
        		break;
        	case 1:
        		oobsize = NANDC_SPARE_LENGTH_224BYTE;
        		break;
        	case 2:
        		oobsize = NANDC_SPARE_LENGTH_448BYTE;
        		break;
        	case 3:
        		oobsize = NANDC_SPARE_LENGTH_64BYTE;
        		break;
        	case 4:
        		oobsize = NANDC_SPARE_LENGTH_32BYTE;
        		break;
        	case 5:
        		oobsize = NANDC_SPARE_LENGTH_16BYTE;
        		break;
        	default:
        		oobsize = NANDC_SPARE_LENGTH_640BYTE;
        		break;
        	}
            extid >>= 2;

        	/* Calc blocksize */
        	tmp = ((extid >> 1) & 0x04) | (extid & 0x03);
        	if (tmp < 0x03)
        		erasesize = ((NANDC_BLOCK_128PAGE * NANDC_SIZE_1K) << tmp);
        	else if (tmp == 0x03)
        		erasesize = NANDC_BLOCK_768PAGE * NANDC_SIZE_1K;
        	else
        		erasesize = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << tmp);
        }
        else if(id_data[0] == NAND_MFR_SAMSUNG)
    	{
    		/* Calc pagesize */
    		writesize = NANDC_SIZE_1K << (extid & 0x03);
    		extid >>= 2;

    		/* Calc oobsize */
    		oobsize = (8 << (extid & 0x03)) * (writesize >> 9);
    		extid >>= 2;

    		/* Calc blocksize. Blocksize is multiples of 64KiB */
    		erasesize = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << (extid & 0x03));
        }
        /* TOSHIBA (2k, 128Byte, 128KB, 8bits) */
        else if(id_data[0] == NAND_MFR_TOSHIBA)
    	{
    		/* Calc pagesize */
    		writesize = NANDC_SIZE_1K << (extid & 0x03);
    		extid >>= 4;

    		/* Calc blocksize. Blocksize is multiples of 64KiB */
    		erasesize = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << (extid & 0x03));

    		if(id_data[4] & NAND_ONCHIP_ECC) /* Toshiba benand */
    		{
    			/* 据东芝FAE反馈，Toshiba benand只有2k(24nm, 64byte spare size)和4k(24nm,32nm, 都是128 byte spare size)这三种 */
    			if(NANDC_SIZE_2K == writesize)
    			{
    				oobsize = NANDC_SPARE_LENGTH_64BYTE;
    			}
    			else
    			{
    				oobsize = NANDC_SPARE_LENGTH_128BYTE;
    			}
    		}
    		else /* Toshiba SLC nand */
    		{
    			unsigned int ecc_type = 0;

    			/* 根据pagesize和加工工艺确定ecc type */
    			if(NANDC_SIZE_2K == writesize)				/* 2k 的只有24nm和43nm */
    			{
    				if(NAND_TOSHIBA_43NM == (NAND_TOSHIBA_TECH_MASK & id_data[5]))
    				{
    					ecc_type = NANDC_ECC_4BIT;
    				}
    				else /* NAND_TOSHIBA_24NM */
    				{
    					ecc_type = NANDC_ECC_8BIT;
    				}

    			}
    			else if(NANDC_SIZE_4K == writesize)			/* 4k的只有24nm和32nm */
    			{
    				if(NAND_TOSHIBA_24NM == (NAND_TOSHIBA_TECH_MASK & id_data[5]))
    				{
    					ecc_type = NANDC_ECC_8BIT;
    				}
    				else /* NAND_TOSHIBA_32NM、NAND_TOSHIBA_43NM */
    				{
    					ecc_type = NANDC_ECC_4BIT;
    				}
    			}
    			else /* NANDC_SIZE_8K，只有24nm和43nm */
    			{
    				if(NAND_TOSHIBA_24NM == (NAND_TOSHIBA_TECH_MASK & id_data[5]))
    				{
    					ecc_type = NANDC_ECC_24BIT_1K;
    				}
    				else /* NAND_TOSHIBA_43NM */
    				{
    					ecc_type = NANDC_ECC_4BIT;
    				}
    			}

                ret = nandc_native_get_sparesize(host, writesize, ecc_type, &oobsize);
                if(ret)
                {
                    NANDC_TRACE( NFCDBGLVL(ERRO),("get spare size failed, ret = %d\n", ret));
                    return ret;
                }
    		}

        }
        else
        {
            /* Calc pagesize */
            writesize = NANDC_SIZE_1K << (extid & 0x03);
            extid >>= 2;
            /* Calc oobsize */
            oobsize = (8 << (extid & 0x01)) *
            (writesize >> 9);
            extid >>= 2;
            /* Calc blocksize. Blocksize is multiples of 64KiB */
            erasesize = (NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << (extid & 0x03);
            extid >>= 2;
            NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_native_get_spec defualt\n"));
        }
    }

	/*
	 * Check, if buswidth is correct. Hardware drivers should set
	 * chip correct !
	 */

    /*-copy and modified from linux6.2.35 mtd code*/

    spec->blocksize = erasesize;
    spec->pagesize = writesize;
    spec->chipsize = chipsize;
    spec->sparesize = oobsize;

DONE:
    spec->buswidth = 0;

    /* Try to identify manufacturer */
    for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++)
    {
        if (nand_manuf_ids[maf_idx].id == (int)maf_id)
        break;
    }

    /*
	 * Bad block marker is stored in the last page of each block
	 * on Samsung and Hynix MLC devices; stored in first two pages
	 * of each block on Micron devices with 2KiB pages and on
	 * SLC Samsung, Hynix, Toshiba, AMD/Spansion, and Macronix.
	 * All others scan only the first page.
	 */
	if ((cellinfo & NAND_CI_CELLTYPE_MSK) && (maf_id == NAND_MFR_SAMSUNG || maf_id == NAND_MFR_HYNIX))
    {
		host->bbt_options = NAND_BBT_SCANLASTPAGE;
    }
	else
    {
		host->bbt_options = NAND_BBT_SCAN2NDPAGE;
    }

    NANDC_TRACE( NFCDBGLVL(WARNING),("NAND device: Manufacturer ID:"
        " 0x%x, Chip ID: 0x%x (%s 0x%xMB)\n", maf_id, dev_id,
        nand_manuf_ids[maf_idx].name, spec->chipsize >> 20));

    return NANDC_OK;
}

/**
* 作用:得到nandflash的ID数据
*
* 参数:
* @host    		 ---nandc主机控制器的指针
* @idbuf      	 ---存放Flash的ID数据的内存地址
* @length        ---Flash ID的数据的长度
*
* 描述:读Flash的ID数据放指定的内存中，会读两次，比较两次读到的ID数据是否一样的
*
*/
u32 nandc_native_get_id(struct nandc_host* host, u8* id_data, u32 length)
{
    u32 dev_id, maf_id;
	/*读nandflash的ID数据*/
    if(NANDC_OK != nandc_native_read_id( host,  id_data, length))
    {
        NANDC_TRACE( NFCDBGLVL(ERRO),("read id  error1\n"));
        NANDC_DO_ASSERT(0, "", 0 );
        return NANDC_E_READID;
    }

    maf_id = id_data[0];
    dev_id = id_data[1];

    /*再一次读nandflash的ID数据*/
    if(NANDC_OK != nandc_native_read_id( host,  id_data, length))
    {
        NANDC_TRACE( NFCDBGLVL(ERRO),("read id length error2\n"));
        NANDC_DO_ASSERT(0, HICHAR_NULL, 0 );
        return NANDC_E_READID;
    }
	/*比较两次都的数据是否相同*/
    if (id_data[0] != maf_id || id_data[1] != dev_id)
    {
        NANDC_TRACE( NFCDBGLVL(ERRO), ("%s: second ID read did not match \
        %x,%x against %x,%x\n", __func__, maf_id, dev_id, id_data[0], id_data[1]));

		return NANDC_E_READID;
    }

    return NANDC_OK;
}
#ifdef NANDC_USE_ONCHIPROM_PARAM
extern unsigned int is_usb_burn_trapflag(void);
u32 nandc_get_ecctype_from_onchiprom(struct nand_spec_sram *spec_sram)
{
    u32 ecc_type=0;
    switch(spec_sram->ecc_type)
	{
		case NANDC_ECCTYPE_ECC_4BIT_REG:
			ecc_type = NANDC_ECC_4BIT;
			break;
		case NANDC_ECCTYPE_ECC_8BIT_REG:
			ecc_type = NANDC_ECC_8BIT;
			break;
		case NANDC_ECCTYPE_ECC_24BIT_REG:
			ecc_type = NANDC_ECC_24BIT_1K;
			break;
		default:
			/* NOP */
            break;
	}
    return ecc_type;
}
/*****************************************************************************
* 函 数 名  : nandc_get_paramer_from_onchiprom
*
* 功能描述  : 将nand_spec_sram参数转换为nandc_spec
* 格式  : nand_spec_sram
*			--------------------------------------------------------------------------------------
* ofs	:	  31    25    	    21          16        		   12         8    	    4    	   0
* bit	:	 0000   000    	  0000       00000  	  		 0000      0000 	 0000 	    0000
*            |  flag| chipsize | sparesize |  pagenumperblock | addrnum | ecctype | pagesize |
*           --------------------------------------------------------------------------------------
*
*****************************************************************************/

void nandc_get_paramer_from_onchiprom(struct nandc_spec *spec_real, struct nand_spec_sram *spec_sram)
{
	if(!spec_real || !spec_sram)
	{
		NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_get_paramer_from_onchiprom error \n"));
		return;
	}
	/* page size */
	switch(spec_sram->page_size)
	{
		case NANDC_PAGE_SIZE_2K_REG:
			spec_real->pagesize = NANDC_SIZE_2K;
			break;
		case NANDC_PAGE_SIZE_4K_REG:
			spec_real->pagesize = NANDC_SIZE_4K;
			break;
		case NANDC_PAGE_SIZE_8K_REG:
			spec_real->pagesize = NANDC_SIZE_8K;
			break;
		default:
			/* NOP */
            break;
	}

	/* chip size */
	switch(spec_sram->chip_size)
	{
		case NANDC_CHIPSIZE_64M_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_64M;
			break;
		case NANDC_CHIPSIZE_128M_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_128M;
			break;
		case NANDC_CHIPSIZE_256M_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_256M;
			break;
		case NANDC_CHIPSIZE_512M_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_512M ;
			break;
		case NANDC_CHIPSIZE_1G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_1G;
			break;
		case NANDC_CHIPSIZE_2G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_2G;
			break;
		case NANDC_CHIPSIZE_4G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_4G;
			break;
		case NANDC_CHIPSIZE_8G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_8G;
			break;
		case NANDC_CHIPSIZE_16G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_16G;
			break;
		case  NANDC_CHIPSIZE_32G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_32G;
			break;
		case  NANDC_CHIPSIZE_64G_REG:
			spec_real->chipsize = NANDC_CHIPSIZE_64G;
			break;
	}
    spec_real->chipsize = (spec_real->chipsize) << 20;
    spec_real->blocksize = (spec_real->pagesize*64) << (spec_sram->page_num_per_block);

	/* spare size */
	switch(spec_sram->spare_size)
	{
		case NANDC_SPARE_LENGTH_16BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_16BYTE;
			break;
		case NANDC_SPARE_LENGTH_32BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_32BYTE;
			break;
		case NANDC_SPARE_LENGTH_60BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_60BYTE;
			break;
		case NANDC_SPARE_LENGTH_64BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_64BYTE;
			break;
		case NANDC_SPARE_LENGTH_88BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_88BYTE;
			break;
		case NANDC_SPARE_LENGTH_116BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_116BYTE;
			break;
		case NANDC_SPARE_LENGTH_128BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_128BYTE;
			break;
		case NANDC_SPARE_LENGTH_144BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_144BYTE;
			break;
		case NANDC_SPARE_LENGTH_200BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_200BYTE;
			break;
		case NANDC_SPARE_LENGTH_218BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_218BYTE;
			break;
		case NANDC_SPARE_LENGTH_224BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_224BYTE;
			break;
		case NANDC_SPARE_LENGTH_232BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_232BYTE;
			break;
		case NANDC_SPARE_LENGTH_256BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_256BYTE;
			break;
		case NANDC_SPARE_LENGTH_368BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_368BYTE;
			break;
		case NANDC_SPARE_LENGTH_400BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_400BYTE;
			break;
		case  NANDC_SPARE_LENGTH_436BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_436BYTE;
			break;
		case NANDC_SPARE_LENGTH_448BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_448BYTE;
			break;
		case NANDC_SPARE_LENGTH_512BYTE_REG:
			spec_real->sparesize = NANDC_SPARE_LENGTH_512BYTE;
			break;
		default:
			/* NOP */
			break;
	}
    spec_real->name = NULL;
    NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_get_paramer_from_onchiprom \n"));
}

/**
* 作用:nandflash的probe函数功能,查询nandflash是否存在
*
* 参数:
* @host    		 ---nandc主机控制器的指针
*
* 描述:通过读nandflash的ID数据来解析nandflash的规格参数
*
*/
u32 nandc_native_nand_prob(struct nandc_host* host)
{
    struct nandc_spec spec;
    u8 id_data[NANDC_READID_SIZE];
    u32 addnum = 0;
    int ret=0;
    tOcrShareData *pShareData = (tOcrShareData*)M3_SRAM_SHARE_DATA_ADDR;

    memset((void *)&spec, 0, sizeof(struct nandc_spec));
    nandc_native_reset_chip(host, 0);
    /*如果是先贴后烧程序*/
    if(is_usb_burn_trapflag())
    {
        NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_is_usb_burn \n"));
        NANDC_TRACE( NFCDBGLVL(ERRO),("pShareData->ulOcrInitedFlag %x sizeof %d\n",pShareData->ulOcrInitedFlag,sizeof(tOcrShareData)));
        NANDC_TRACE( NFCDBGLVL(ERRO),("pShareData->bsp_nand_get_spec_and_save %x\n",pShareData->bsp_nand_get_spec_and_save));
        NANDC_TRACE( NFCDBGLVL(ERRO),("pShareData %x\n",pShareData));
        NANDC_TRACE( NFCDBGLVL(ERRO),("pShareData->nandspec.flag %x\n",pShareData->nandspec.flag));
        /* set page size default(2k) for read id */
        set_hi_nfc_con_pagesize(NANDC_PAGE_SIZE_2K_REG);
        ret = pShareData->bsp_nand_get_spec_and_save(&pShareData->nandspec);
        NANDC_TRACE( NFCDBGLVL(ERRO),("pShareData->nandspec.flag %x\n",pShareData->nandspec.flag));
        NANDC_TRACE( NFCDBGLVL(ERRO),("ret %x\n",ret));
        if(ret)
        {
            NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_is_usb_burn get id error \n"));
            return NANDC_E_INIT;
        }
    }
    else
    {
        /*如果onchiprom传过来的参数只是部分的话那么就要读ID来解析一下了*/
        if(pShareData->nandspec.flag==NAND_ARGS_PART_AVAIL)
        {
            NANDC_TRACE( NFCDBGLVL(ERRO),("NAND_ARGS_PART_AVAIL \n"));
            /*得到所有的nandflash的ID数据*/
            for(host->chipselect = 0 ; host->chipselect < host->chipmax ; host->chipselect++)
            {
                //nandc_native_reset_chip(host, host->chipselect);
                if(NANDC_OK ==  nandc_native_get_id(host, id_data, sizeof(id_data)))
                {
                    if(NANDC_OK == nandc_native_get_spec(host, id_data, &spec))
                    {
                        addnum++;
                        memcpy(host->flash_id, id_data, NANDC_READID_SIZE); /* 保存flash id */
                    }
                    else
                    {
                        NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_native_get_spec error:chip_select = %d id[0]:0x%x, id[1]:0x%x\n",
                                        	host->chipselect, id_data[0], id_data[1]));
                        return NANDC_E_INIT;
                    }
                }
            }
        }
    }
    if(pShareData->nandspec.flag==NAND_ARGS_FULL_AVAIL)
    {
        /*把nand_spec_sram的参数格式转换为nandc_spec*/
        nandc_get_paramer_from_onchiprom(&spec,&pShareData->nandspec);
        addnum =1;
        NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_is_usb_burn full \n"));

    }
    if(!host->bbt_options)
    {
        host->bbt_options = NAND_BBT_SCAN2NDPAGE;
    }
    /*把onchiprom中的ecc_type保存起来*/
    host->ecctype_onfi = nandc_get_ecctype_from_onchiprom(&pShareData->nandspec);
    spec.buswidth = 0;
    NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_native_get_spec pagesize:0x%x, blocksize:0x%x, chipsize:0x%x, sparesize:0x%x, ecctypes:0x%x\n",
                                        	spec.pagesize, spec.blocksize, spec.chipsize,spec.sparesize,host->ecctype_onfi));

	/*如果检测到有nandflash的话就把nandflash添加到host中去*/
    if(addnum > 0)
    {
        return  nandc_host_add_nand_chip(host, addnum, &spec);
    }
    else
    {
        return NANDC_ERROR;
    }
}
#else

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_nand_prob() - reads ids for all nand flash chips and get flash information
 *                             from flash table according to flash id
 *
 * PARAMETER:
 * @host - [input]the main struction.
 *
 *
 * DESCRIPTION:
 * This function reads ids for all nand flash chips if host->chipmax is not 1.
 * and get flash information from flash table according to flash id.
 *
 *
 * CALL FUNC:
 * nandc_nand_init() -
 *
 ********************************************************************************/
u32 nandc_native_nand_prob(struct nandc_host* host)
{
    struct nandc_spec spec;
    u8 id_data[NANDC_READID_SIZE];
    u32 addnum = 0;

    /*host->chipmax = NANDC_MAX_CHIPS*/
    for(host->chipselect = 0 ; host->chipselect < host->chipmax ; host->chipselect++)
    {
        nandc_native_reset_chip(host, host->chipselect);
        if(NANDC_OK ==  nandc_native_get_id(host, id_data, sizeof(id_data)))
        {
            if(NANDC_OK == nandc_native_get_spec(host, id_data, &spec))
            {
                addnum++;
                memcpy(host->flash_id, id_data, NANDC_READID_SIZE); /* 保存flash id */
            }
            else
            {
                NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_native_get_spec error:chip_select = %d id[0]:0x%x, id[1]:0x%x\n",
                                        host->chipselect, id_data[0], id_data[1]));
				return NANDC_E_INIT;
            }
        }
    }
    if(addnum > 0)
    {
        return  nandc_host_add_nand_chip(host, addnum, &spec);
    }
    else
    {
        return NANDC_ERROR;
    }
}

#endif

/**
* 作用:根据globaddr的Flash地址来确定此地址在nandflash上的哪一个分区上,并设置当前分区为此分区
*
* 参数:
* @host    		 ---nandc主机控制器的指针
* @globaddr      ---flash的地址
*
*
* 描述:通过Flash的地址信息得到此地址是在flash上的哪一个分区上，并设置当前分区为此分区
*
*/
u32 nandc_native_find_partition(struct nandc_host* host, FSZ globaddr)
{
    struct mtd_partition* partition;
    u32 curpart;
    u32 nparts;

    partition = host->usrdefine->partition;
    nparts  = host->usrdefine->nbparts;

    for(curpart = 0 ; curpart < nparts ; curpart++)
    {
    	/*此地址在当前处理的分区上*/
        if((globaddr >= partition->offset)&&(globaddr < partition->offset + partition->size))
        {
            host->curpart = partition;
            break;
        }
        partition++;
    }

    if(curpart == nparts)
    {
        NANDC_TRACE( NFCDBGLVL(ERRO), ("partition not find, address:0x%lx\n",globaddr));
        return NANDC_E_NOFIND;
    }

    return NANDC_OK;

}


/**
* 作用:根据block块号来设置flash的地址
*
* 参数:
* @host    		 ---nandc主机控制器的指针
* @blknum        ---flash的block块地址
*
* 描述:通过Flash的block块地址来设置要处理的flash地址
*
*/

u32 nandc_native_location_by_blocknum(struct nandc_host* host, u32 blknum)
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;

    host->addr_real= (blknum & spec->blockmask) << spec->blockshift;

    host->chipselect = blknum >> (spec->chipshift - spec->blockshift);

    return NANDC_OK;/*nandc_bbm_real_addr(host->addr_logic, &host->addr_real);*/

}

/**
* 作用:根据要操作nandflash的地址来设置nandc_host中的地址信息和当前要处理的分区信息
*
* 参数:
* @host    		 ---nandc主机控制器的指针
* @globaddr      ---要操作的Flash地址
*
* 描述:通过Flash的地址来设置nandc_host中相关的地址信息
*
*/
u32 nandc_native_location_by_address(struct nandc_host* host, FSZ globaddr )
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;

    host->addr_logic = globaddr & (spec->chipsize - 1);

    host->chipselect = globaddr >> spec->chipshift;
	/*设置当前要处理的分区指针*/
    if(NANDC_OK != nandc_native_find_partition( host, globaddr))
    {
        goto ERRO;
    }

    return nandc_bbm_real_addr(host->addr_logic, &host->addr_real);

ERRO:

    return NANDC_E_NOFIND;
}






/**
* 作用:根据nandc_init_info这个参数来创建nandc_host这个数据结构
*
* 参数:
* @init_info    		 ---nandc主机控制器的初始话指针
*
*
* 描述:根据nandc_init_info这个结构体指针来创建nandc_host这个数据结构
*
*/
struct nandc_host* nandc_native_host_create(struct nandc_init_info* init_info)
{
    struct nandc_host *host = NANDC_NULL ;
    struct nandc_ctrl_desc*  ctrlspec = NANDC_NULL;


    host = (struct nandc_host *)himalloc(sizeof(struct nandc_host));
    if(!host)
    {
        hiout(("get memory failed\r\n"));
        goto ERRO;
    }

    NANDC_REJECT_NULL((u32)host & (u32)init_info);
    NANDC_REJECT_NULL((u32)init_info->ctrlspec
                      &(u32)init_info->ctrlfunc
                      &(u32)init_info->eccinfo
                      &(u32)init_info->bitstbl
                      &(u32)init_info->dmaplookup
                     );
    memset((void*)host,0x00,sizeof(struct nandc_host)); /*clean the pclint e516*/

    hiout(("Hisilicon %s initialize...\r\n",(char*)init_info->ctrlspec->ctrlname));

    ctrlspec = init_info->ctrlspec;

    host->bufbase   =   (void __iomem *)ctrlspec->buffbase;
    host->regbase   =   (void __iomem *)ctrlspec->regbase;
    host->chipmax   =   ctrlspec->maxchip;
    host->ctrlfunc  =   init_info->ctrlfunc;
    host->eccinfo   =   init_info->eccinfo;
    host->dmaplookup=   init_info->dmaplookup;
    host->regsize   =   ctrlspec->regsize;
    host->bufsize   =   ctrlspec->buffsize;
    host->latched   =   nandc_latch_none;

    host->buffers = (struct nand_buffers*)himalloc(sizeof(struct nand_buffers));
    NANDC_REJECT_NULL(host->buffers );
    memset((void*)(host->buffers), 0xff, sizeof(struct nand_buffers)); /*clean the pclint e516*/

    if(NANDC_OK != nandc_host_init_cluster(host,  ctrlspec->regdesc, init_info->bitstbl))
    {
        goto ERRO;
    }

    NANDC_TRACE( NFCDBGLVL(NORMAL), ("nandc_native_host_create ok\n"));

    return host;

ERRO:

    /*modified for lint e613 e429 */
    if ((host != NANDC_NULL) && (host->buffers != NANDC_NULL))
    {
        hifree((void*)host->buffers);
    }

    if (host != NANDC_NULL)
    {
        hifree((void*)host);
    }
    /*modified for lint e613 */

    NANDC_TRACE( NFCDBGLVL(ERRO), ("nandc_native_host_create error\n"));/*lint !e778*/

    return NANDC_NULL;

}
#endif

#ifdef __cplusplus
}
#endif
