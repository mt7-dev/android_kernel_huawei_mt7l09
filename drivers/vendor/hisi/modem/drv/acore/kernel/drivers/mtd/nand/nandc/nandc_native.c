
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

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_reset_chip() - reset nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @chipselect - [input]according it to reset which flash chip if there are two or more chips.
 *
 * DESCRIPTION:
 * This function resets the nand flash chip.when we read id from nand flash chip. we
 * should reset nand flash chip first.
 *
 *
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
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

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_read_id() - reads nand flash chip ID data.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @idbuf - [input/output]ram buffer to restore  the  nand flash chip ID data.
 * @length - [input]the length of the  nand flash chip ID data.
 *
 *
 * DESCRIPTION:
 * This function reads nand flash chip ID data through low layer interface
 * nandc_ctrl_read_id.
 *
 * CALL FUNC:
 * nandc_native_get_id() -
 *
 ********************************************************************************/
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

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_get_spec() - gets nand chip's spec (page size,oob size,erase size etc.)
 *
 * PARAMETER:
 * @id_data - [input/output]the original id data read from nand flash chip.
 * @spec - [input/output]record nand chip's spec(page size,oob size,erase size etc.)
 *                       which is calculated from original id data.
 *
 * DESCRIPTION:
 * This function gets nand flash chip's spec (page size,oob size,erase size etc.)by reading
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
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
u32 nandc_native_get_spec(struct nandc_host* host, u8* id_data, struct nandc_spec *spec)
{
    struct nand_flash_dev *type = NANDC_NULL;
    u32 dev_id, maf_id, ret = NANDC_ERROR;
    u32 writesize, erasesize = 0, maf_idx, oobsize = 0;
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
            NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_native_get_spec error\n"));
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
	else if (!(cellinfo & NAND_CI_CELLTYPE_MSK) &&
				(maf_id == NAND_MFR_SAMSUNG || maf_id == NAND_MFR_HYNIX || maf_id == NAND_MFR_TOSHIBA ||
				 maf_id == NAND_MFR_AMD     || maf_id == NAND_MFR_MACRONIX || maf_id == NAND_MFR_MICRON))
    {
		host->bbt_options = NAND_BBT_SCAN2NDPAGE;
    }
    else
    {
        NANDC_TRACE( NFCDBGLVL(WARNING),("Unknown nand flash tpye, maf_id 0x%x, cellinfo 0x%x\n",
            maf_id, cellinfo));
        return NANDC_ERROR;
    }

    NANDC_TRACE( NFCDBGLVL(WARNING),("NAND device: Manufacturer ID:"
        " 0x%x, Chip ID: 0x%x (%s 0x%xMB)\n", maf_id, dev_id,
        nand_manuf_ids[maf_idx].name, spec->chipsize >> 20));

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_get_id() -  get id for the nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @id_data - [input]where to store the id data.
 * @length - [input]the length of the id data.
 *
 *
 * DESCRIPTION:
 * This function gets id data.
 *
 *
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
u32 nandc_native_get_id(struct nandc_host* host, u8* id_data, u32 length)
{
    u32 dev_id, maf_id;

    if(NANDC_OK != nandc_native_read_id( host,  id_data, length))
    {
        NANDC_TRACE( NFCDBGLVL(ERRO),("read id  error1\n"));
        NANDC_DO_ASSERT(0, "", 0 );
        return NANDC_E_READID;
    }

    maf_id = id_data[0];
    dev_id = id_data[1];

    /*read ID two times to insure the result for two times is consistent*/
    if(NANDC_OK != nandc_native_read_id( host,  id_data, length))
    {
        NANDC_TRACE(NFCDBGLVL(ERRO),("read id length error2\n"));
        NANDC_DO_ASSERT(0, HICHAR_NULL, 0 );
        return NANDC_E_READID;
    }

    if (id_data[0] != maf_id || id_data[1] != dev_id)
    {
        NANDC_TRACE( NFCDBGLVL(ERRO), ("%s: second ID read did not match \
%x,%x against %x,%x\n", __func__, maf_id, dev_id, id_data[0], id_data[1]));

		return NANDC_E_READID;
    }

    return NANDC_OK;
}

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

    memset(&spec, 0, sizeof(struct nandc_spec));

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

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_find_partition() - find partition according to address in total address space.
 *
 * PARAMETER:
 * @host - [input]the main struction.
 * @globaddr - [input]address in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 *
 * DESCRIPTION:
 * This function finds partition according to address in total address space.
 *
 *
 * CALL FUNC:
 * nandc_native_location_by_pagenum() -
 * nandc_native_find_partition() -
 *
 ********************************************************************************/
u32 nandc_native_find_partition(struct nandc_host* host, FSZ globaddr)
{
    struct mtd_partition* partition;
    u32 curpart;
    u32 nparts;

    partition = host->usrdefine->partition;
    nparts  = host->usrdefine->nbparts;

    for(curpart = 0 ; curpart < nparts ; curpart++)
    {
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

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_location_by_blocknum() - fix on opeation location by block number.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @blknum - [input]block number in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 * DESCRIPTION:
 * This function fixs on opeation location by block number(there may be more than one nand
 * flash chip).
 *
 * CALL FUNC:
 * nandc_nand_flag_address() -
 * nandc_nand_erase_block() - erase function for erasing operation.
 *
 ********************************************************************************/
u32 nandc_native_location_by_blocknum(struct nandc_host* host, u32 blknum)
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;

    host->addr_real= (blknum & spec->blockmask) << spec->blockshift;

    host->chipselect = blknum >> (spec->chipshift - spec->blockshift);

    return NANDC_OK;/*nandc_bbm_real_addr(host->addr_logic, &host->addr_real);*/

}
 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_location_by_address() - fix on opeation location by adderss.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @globaddr - [input]address in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 *
 * DESCRIPTION:
 * This function  fixs on opeation location by adderss
 *
 * CALL FUNC:
 * nandc_nand_mass_read() - read function for mass data reading operation.
 * nandc_nand_mass_write() - write function for mass data writing operation.
 *
 ********************************************************************************/
u32 nandc_native_location_by_address(struct nandc_host* host, FSZ globaddr )
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;

    host->addr_logic = globaddr & (spec->chipsize - 1);

    host->chipselect = globaddr >> spec->chipshift;

    if(NANDC_OK != nandc_native_find_partition( host, globaddr))
    {
        goto ERRO;
    }

    return nandc_bbm_real_addr(host->addr_logic, &host->addr_real);

ERRO:

    return NANDC_E_NOFIND;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_host_delete() -
 *
 * PARAMETER:
 * @host - [input]
 *
 *
 * DESCRIPTION:
 *
 *
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
void nandc_native_host_delete(struct nandc_host* host)
{

#ifdef WIN32
    if(host->debuger)
        hiwin_regdebug_delete(host->debuger);
#endif

    HI_NOTUSED(host);

}

#endif

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_host_create() - initialize the main struct nandc_host.
 *
 * PARAMETER:
 * @init_info - [input]configuration for corresponding nand flash controller.
 *              v200 : nandc2_init_info
 *              v320 : nandc3_init_info
 *              v400 : nandc4_init_info
 *				v600 : nandc6_init_info
 *
 * DESCRIPTION:
 * This function mallocs space for the main struct nandc_host and initializes it.
 *
 * CALL FUNC:
 * nandc_nand_create_host() -
 *
 ********************************************************************************/
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


#ifdef __cplusplus
}
#endif
