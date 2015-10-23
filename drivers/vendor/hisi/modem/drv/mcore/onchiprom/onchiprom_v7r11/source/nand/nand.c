/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  nand.c
*
*   作    者 :  lusuo
*
*   描    述 :  nand 自适应主要流程
*
*   修改记录 :  2013年12月10日  v1.00  lusuo  创建
*
*************************************************************************/

#include "hi_efuse.h"
#include "hi_nandc.h"
#include "ioinit.h"
#include "nand.h"
#include "OcrShare.h"
#include "OnChipRom.h"
#include "string.h"
#include "sys.h"


/*
 * The chip ID list:
 *    name, device ID, page size, chip size in MiB, eraseblock size, options
 *
 * If page size and eraseblock size are 0, the sizes are taken from the
 * extended chip ID.
 */
const struct nand_flash_dev nand_flash_ids[] = {
	/*
	 * Some incompatible NAND chips share device ID's and so must be
	 * listed by full ID. We list them first so that we can easily identify
	 * the most specific match.
	 */
	{"TC58NVG2S0F 4G 3.3V 8-bit",
		{ .id = {0x98, 0xdc, 0x90, 0x26, 0x76, 0x15, 0x01, 0x08} },
		  NANDC_SIZE_4K, 512, NANDC_SIZE_256K, NAND_ID_SIZE, NANDC_SPARE_LENGTH_224BYTE},
	{"TC58NVG3S0F 8G 3.3V 8-bit",
		{ .id = {0x98, 0xd3, 0x90, 0x26, 0x76, 0x15, 0x02, 0x08} },
		  NANDC_SIZE_4K, 1024, NANDC_SIZE_256K, NAND_ID_SIZE, NANDC_SPARE_LENGTH_232BYTE},
	{"TC58NVG5D2 32G 3.3V 8-bit",
		{ .id = {0x98, 0xd7, 0x94, 0x32, 0x76, 0x56, 0x09, 0x00} },
		  NANDC_SIZE_8K, 4096, NANDC_SIZE_1M, NAND_ID_SIZE, NANDC_SPARE_LENGTH_640BYTE},
	{"TC58NVG6D2 64G 3.3V 8-bit",
		{ .id = {0x98, 0xde, 0x94, 0x82, 0x76, 0x56, 0x04, 0x20} },
		  NANDC_SIZE_8K, 8192, NANDC_SIZE_2M, NAND_ID_SIZE, NANDC_SPARE_LENGTH_640BYTE},

	/*
	 * These are the new chips with large page size. Their page size and
	 * eraseblock size are determined from the extended ID bytes.
	 */

	/* 512 Megabit */
	EXTENDED_ID_NAND("NAND 64MiB 1,8V 8-bit",  0xA2,  64),
	EXTENDED_ID_NAND("NAND 64MiB 1,8V 8-bit",  0xA0,  64),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 8-bit",  0xF2,  64),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 8-bit",  0xD0,  64),
	EXTENDED_ID_NAND("NAND 64MiB 3,3V 8-bit",  0xF0,  64),

	/* 1 Gigabit */
	EXTENDED_ID_NAND("NAND 128MiB 1,8V 8-bit",  0xA1, 128),
	EXTENDED_ID_NAND("NAND 128MiB 3,3V 8-bit",  0xF1, 128),
	EXTENDED_ID_NAND("NAND 128MiB 3,3V 8-bit",  0xD1, 128),

	/* 2 Gigabit */
	EXTENDED_ID_NAND("NAND 256MiB 1,8V 8-bit",  0xAA, 256),
	EXTENDED_ID_NAND("NAND 256MiB 3,3V 8-bit",  0xDA, 256),

	/* 4 Gigabit */
	EXTENDED_ID_NAND("NAND 512MiB 1,8V 8-bit",  0xAC, 512),
	EXTENDED_ID_NAND("NAND 512MiB 3,3V 8-bit",  0xDC, 512),

	/* 8 Gigabit */
	EXTENDED_ID_NAND("NAND 1GiB 1,8V 8-bit",  0xA3, 1024),
	EXTENDED_ID_NAND("NAND 1GiB 3,3V 8-bit",  0xD3, 1024),

	/* 16 Gigabit */
	EXTENDED_ID_NAND("NAND 2GiB 1,8V 8-bit",  0xA5, 2048),
	EXTENDED_ID_NAND("NAND 2GiB 3,3V 8-bit",  0xD5, 2048),

	/* 32 Gigabit */
	EXTENDED_ID_NAND("NAND 4GiB 1,8V 8-bit",  0xA7, 4096),
	EXTENDED_ID_NAND("NAND 4GiB 3,3V 8-bit",  0xD7, 4096),

	/* 64 Gigabit */
	EXTENDED_ID_NAND("NAND 8GiB 1,8V 8-bit",  0xAE, 8192),
	EXTENDED_ID_NAND("NAND 8GiB 3,3V 8-bit",  0xDE, 8192),

	/* 128 Gigabit */
	EXTENDED_ID_NAND("NAND 16GiB 1,8V 8-bit",  0x1A, 16384),
	EXTENDED_ID_NAND("NAND 16GiB 3,3V 8-bit",  0x3A, 16384),

	/* 256 Gigabit */
	EXTENDED_ID_NAND("NAND 32GiB 1,8V 8-bit",  0x1C, 32768),
	EXTENDED_ID_NAND("NAND 32GiB 3,3V 8-bit",  0x3C, 32768),

	/* 512 Gigabit */
	EXTENDED_ID_NAND("NAND 64GiB 1,8V 8-bit",  0x1E, 65536),
	EXTENDED_ID_NAND("NAND 64GiB 3,3V 8-bit",  0x3E, 65536),

	{NULL}
};

/* relationship of pagesize, oobsize and ecctype */
const struct nandc_ecc_info nandc6_eccinfo[] =
{
    {NANDC_SIZE_4K,     NANDC_SPARE_LENGTH_144BYTE,    NANDC_ECCTYPE_ECC_8BIT},
    {NANDC_SIZE_4K,     NANDC_SPARE_LENGTH_88BYTE,     NANDC_ECCTYPE_ECC_4BIT},

    {NANDC_SIZE_2K,     NANDC_SPARE_LENGTH_88BYTE,     NANDC_ECCTYPE_ECC_8BIT},
    {NANDC_SIZE_2K,     NANDC_SPARE_LENGTH_60BYTE,     NANDC_ECCTYPE_ECC_4BIT},

    {0,0,0},
};

#define le16_to_cpu(v) (v)
#define le32_to_cpu(v) (v)

/*****************************************************************************
* 函 数 名  : ffs
*
* 功能描述  : find first bit in word.
*
* 输入参数  : @word: The word to search
* 输出参数  : null
*
* 返 回 值  : offset of first bit
*
* 其它说明  : 移植自Linux kernel 3.10
*
*****************************************************************************/
INT32 ffs(UINT32 word)
{
	int num = 0;

	if(0 == word)
		return 0;

	if ((word & 0xffff) == 0)
	{
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0)
	{
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0)
	{
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0)
	{
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
	{
		num += 1;
	}

	return num;
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
INT32 nand_wait_result(UINT32 value, UINT32 time_out)
{
	UINT32 count = time_out;
	do{
        if(get_hi_nfc_ints_op_done() == value)
		{
			return OK;
		}
		count--;
	}while(count > 0);

	return NAND_WAIT_TIMEOUT;
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
UINT8 nand_readbuffer_byte(UINT32 offset)
{
	return *((UINT8*)(NANDC_BUFFER_BASE_ADDR + offset));
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
INT32 nand_readbuffer(UINT8 *dest, UINT32 length)
{
	if(length > NANDC_BUFFER_SIZE)
    {
		return NAND_READSIZE_TOO_LARGE;
    }

	memcpy((void *)dest, (void *)NANDC_BUFFER_BASE_ADDR, (INT32)length);
	return OK;
}

/*****************************************************************************
* 函 数 名  : nand_set_spec_for_boot
*
* 功能描述  : 为nandc从normal模式切换到boot模式做准备工作，主要是将normal模式下配置过的寄存器回复默认值，
*			  将nand spec info配置到nandc相关寄存器
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
*****************************************************************************/
void nand_set_spec_for_boot(struct nand_spec_real *spec_real)
{
	/* 根据芯片建议设置，使normal模式下配置过的寄存器清除。从实际测试来看，这四行代码可加可不加 */
	set_hi_nfc_cmd_cmd1(0);
	set_hi_nfc_cmd_cmd2(NAND_CMD_READSTART);
	set_hi_nfc_cmd_read_status_cmd(NAND_CMD_STATUS);
	set_hi_nfc_addrl_addr_l(0);

	/* ecc type */
	switch(spec_real->ecc_type)
	{
		case NANDC_ECCTYPE_ECC_4BIT:
			set_hi_nfc_con_ecc_type(NANDC_ECCTYPE_ECC_4BIT_REG);
			break;
		case NANDC_ECCTYPE_ECC_8BIT:
			set_hi_nfc_con_ecc_type(NANDC_ECCTYPE_ECC_8BIT_REG);
            set_hi_nfc_oob_sel_oob_len_sel(NANDC_OOB_LEN_32BYTE_REG);  /* oob len select */
			break;
		case NANDC_ECCTYPE_ECC_24BIT:
			set_hi_nfc_con_ecc_type(NANDC_ECCTYPE_ECC_24BIT_REG);
			break;
		default:
			print_info_with_u32("\r\nerr sp ecc: ", spec_real->ecc_type);
            break;
	};

	/* page size */
	switch(spec_real->page_size)
	{
		case NANDC_SIZE_2K:
			set_hi_nfc_con_pagesize(NANDC_PAGE_SIZE_2K_REG);
			break;
		case NANDC_SIZE_4K:
			set_hi_nfc_con_pagesize(NANDC_PAGE_SIZE_4K_REG);
			break;
		case NANDC_SIZE_8K:
			set_hi_nfc_con_pagesize(NANDC_PAGE_SIZE_8K_REG);
			break;
		default:
			print_info_with_u32("\r\nerr sp pagesize: ", spec_real->page_size);
            break;
	};

    /* page num per block */
	switch(spec_real->page_num_per_block)
	{
		case NANDC_BLOCK_64PAGE:
			set_hi_nfc_boot_set_block_size(NANDC_PAGENUM_PER_BLOCK_64_REG);
			break;
		case NANDC_BLOCK_128PAGE:
			set_hi_nfc_boot_set_block_size(NANDC_PAGENUM_PER_BLOCK_128_REG);
			break;
		default:
			print_info_with_u32("\r\nerr sp blksize: ", spec_real->page_num_per_block);
            break;
	};

	/* set bus width */
	set_hi_nfc_con_bus_width(NANDC_BUSWIDTH_8BIT_REG);

	/* addr cycle */
	/*
	 * 配置op寄存器会触发nandc对nand的操作，这就必须做到1.op寄存器一次性配置ok；2.NFC_CMD寄存器的值必须是正确的值。这样需要配置的寄存器比较多，
	 * 从实际测试来看，op_address_cycles不配置不会影响正常流程，并且，当配置成boot模式后，访问0xa0000000时，nandc会自动配置NFC_OP和NFC_CMD寄存器
	 * 并进行读操作
	 *
	 */
	switch(spec_real->addr_cycle)
	{
		case NANDC_ADDRCYCLE_4:
			set_hi_nfc_boot_set_addr_num(NANDC_ADDRCYCLE_4_BOOT_REG);
			break;
		case NANDC_ADDRCYCLE_5:
			set_hi_nfc_boot_set_addr_num(NANDC_ADDRCYCLE_5_BOOT_REG);
			break;
		default:
			print_info_with_u32("\r\nerr sp addrnum: ", spec_real->addr_cycle);
            break;
	};

	/* ints clear */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INTS_CLEAR_ALL_REG);

	return;
}

/*****************************************************************************
* 函 数 名  : do_get_spec_from_efuse_group
*
* 功能描述  : 从efuse 获取nandc spec info
*
* 输入参数  : @spec_real - nand spec info
*			  @group_id - nand spec info 在efuse中的group id
* 输出参数  : null
*
* 返 回 值  : OK - 成功； NAND_EFUSE_ID_ERR - group_id 不合法(目前只支持0/1两个值)
*
* 其它说明  :
*
*****************************************************************************/
INT32 do_get_spec_from_efuse_group(struct nand_spec_real *spec_real, UINT32 group_id )
{
	UINT32 page_size;
	UINT32 ecc_type;
	UINT32 page_num_per_block;
	UINT32 addr_num;

	if(EFUSE_NANDINFO_GROUP_1 == group_id)
	{
		page_size = get_hi_hw_cfg_nf_page_size1();
		ecc_type  = get_hi_hw_cfg_nf_ecc_type1();
		page_num_per_block = get_hi_hw_cfg_nf_block_size1();
		addr_num = get_hi_hw_cfg_nf_addr_num1();
	}
	else if(EFUSE_NANDINFO_GROUP_0 == group_id)
	{
		page_size = get_hi_hw_cfg_nf_page_size();
		ecc_type  = get_hi_hw_cfg_nf_ecc_type();
		page_num_per_block = get_hi_hw_cfg_nf_block_size();
		addr_num = get_hi_hw_cfg_nf_addr_num();
	}
	else
	{
		print_info_with_u32("\r\nefuse grup err ", group_id);
		return NAND_EFUSE_ID_ERR;
	}

	/* page size */
	switch(page_size)
	{
		case NANDC_PAGE_SIZE_2K_EFUSE:
			spec_real->page_size = NANDC_SIZE_2K;
			break;
		case NANDC_PAGE_SIZE_4K_EFUSE:
			spec_real->page_size = NANDC_SIZE_4K;
			break;
		case NANDC_PAGE_SIZE_8K_EFUSE:
			spec_real->page_size = NANDC_SIZE_8K;
			break;
		default:
			print_info_with_u32("\r\nerr efu ps:", page_size);
			break;
	};

	/* ecc type */
	switch(ecc_type)
	{
		case NANDC_ECCTYPE_ECC_4BIT_EFUSE:
			spec_real->ecc_type = NANDC_ECCTYPE_ECC_4BIT;
			break;
		case NANDC_ECCTYPE_ECC_8BIT_EFUSE:
			spec_real->ecc_type = NANDC_ECCTYPE_ECC_8BIT;
			break;
		case NANDC_ECCTYPE_ECC_24BIT_EFUSE:
			spec_real->ecc_type = NANDC_ECCTYPE_ECC_24BIT;
			break;
		default:
			print_info_with_u32("\r\nerr efu ecctype:", ecc_type);
            break;
	};

	/* page num per block */
	switch(page_num_per_block)
	{
		case NANDC_PAGENUM_PER_BLOCK_64_EFUSE:
			spec_real->page_num_per_block= NANDC_BLOCK_64PAGE;
			break;
		case NANDC_PAGENUM_PER_BLOCK_128_EFUSE:
			spec_real->page_num_per_block = NANDC_BLOCK_128PAGE;
			break;
		default:
			print_info_with_u32("\r\nerr efu blocksize:", page_num_per_block);
			break;
	};

	/* addr cycle */
	switch(addr_num)
	{
		case NANDC_ADDRCYCLE_4_EFUSE:
			spec_real->addr_cycle = NANDC_ADDRCYCLE_4;
			break;
		case NANDC_ADDRCYCLE_5_EFUSE:
			spec_real->addr_cycle = NANDC_ADDRCYCLE_5;
			break;
		default:
			print_info_with_u32("\r\nerr efu addrnum:", addr_num);
			break;
	};

	/* done */
	return OK;
}

/*****************************************************************************
* 函 数 名  : get_spec_from_efuse
*
* 功能描述  : 从efuse 获取nandc spec info
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : OK - 成功； else - 失败
*
* 其它说明  :
*
*****************************************************************************/
INT32 get_spec_from_efuse(struct nand_spec_real *spec_real)
{
	/*
	 * 解析efuse中的nandc spec info
	 * nandc spec info共两组, 烧写的时候从第0组开始烧写，解析的时候相反，从第1组开始解析
	 */

	if(get_hi_hw_cfg_nf_ctrl_ena1())
	{
		return do_get_spec_from_efuse_group(spec_real, EFUSE_NANDINFO_GROUP_1);
	}
	else if(get_hi_hw_cfg_nf_ctrl_ena0())
	{
		return do_get_spec_from_efuse_group(spec_real, EFUSE_NANDINFO_GROUP_0);
	}
	else
	{
		return NAND_EFUSE_FAIL;
	}
}

/*****************************************************************************
* 函 数 名  : nand_reset
*
* 功能描述  : 复位nand flash
*
* 输入参数  : null
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
*****************************************************************************/
void nand_reset(void)
{
    /* set reset cmd */
	set_hi_nfc_cmd_cmd1(NAND_CMD_RESET);

	/* set reset op and addr cycle 0*/
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_OP_OFFSET, NAND_OP_RESET | (NANDC_ADDRCYCLE_0 << NANDC_ADDRBIT_OFFSET));

	/* check result */
	if(nand_wait_result(NANDC_STATUS_OP_DONE, NANDC_RESET_TIME_OUT))
	{
		return;
	}

	/* ints clear */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INTS_CLEAR_ALL_REG);

    return;
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
INT32 nand_send_cmd_onfi_param(void)
{
	INT32 ret;
	/* set command */
	set_hi_nfc_cmd_cmd1(NAND_CMD_PARAM);

	/* set addr 0, low addr must be 0 as nand flash chip required */
	set_hi_nfc_addrl_addr_l(0);
	set_hi_nfc_addrh_addr_h(0);

	/* set ecc type 0 */
	set_hi_nfc_con_ecc_type(NANDC_ECCTYPE_ECC_NONE_REG);

    /* set data length */
	set_hi_nfc_data_num_nfc_data_num(sizeof(struct nand_onfi_params));

	/* set op param and addr cycle 1 */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_OP_OFFSET, NAND_OP_READ_PARAM | (NANDC_ADDRCYCLE_1 << NANDC_ADDRBIT_OFFSET));

	/* wait result */
	ret = nand_wait_result(NANDC_STATUS_OP_DONE, NANDC_RESULT_TIME_OUT);
	if(ret)
	{
		return ret;
	}

	/* int status clear */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INTS_CLEAR_ALL_REG);

	return OK;
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
INT32 nand_send_cmd_id(UINT8 addr)
{
	INT32 ret;

	/* reset nand flash */
	nand_reset();

	/* set ecc type 0 */
	set_hi_nfc_con_ecc_type(NANDC_ECCTYPE_ECC_NONE_REG);

    /* set data num */
	set_hi_nfc_data_num_nfc_data_num(NAND_ID_SIZE);

	/* set command */
	set_hi_nfc_cmd_cmd1(NAND_CMD_READID);

    /* set addr */
	set_hi_nfc_addrl_addr_l(addr);
	set_hi_nfc_addrh_addr_h(0);

    /* set op read id and addr cycle 1 */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_OP_OFFSET, NAND_OP_READ_ID | (NANDC_ADDRCYCLE_1 << NANDC_ADDRBIT_OFFSET));

	/* wait result */
	ret = nand_wait_result(NANDC_STATUS_OP_DONE, NANDC_RESULT_TIME_OUT);
	if(ret)
	{
		return ret;
	}

	/* int status clear */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INTS_CLEAR_ALL_REG);

	return OK;
}

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
UINT16 onfi_crc16(UINT16 crc, UINT8 const *p, UINT32 len)
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
* 函 数 名  : nand_detect_onfi
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
INT32 nand_detect_onfi(struct nand_spec_real *spec_real)
{
	INT32 i, ret;
	struct nand_onfi_params p;

	ret = nand_send_cmd_id(NAND_READ_ID_ONFI);
	if(ret)
	{
		return ret;
	}

	if (nand_readbuffer_byte(0) != 'O' || nand_readbuffer_byte(1) != 'N' ||
		nand_readbuffer_byte(2) != 'F' || nand_readbuffer_byte(3) != 'I')
	{
		return NAND_ONFI_ERR;
	}

	ret = nand_send_cmd_onfi_param();
	if(ret)
	{
        print_info_with_u32("\r\nONFI para err ", ret);
		return ret;
	}

	for (i = 0; i < 3; i++)
	{
		ret = nand_readbuffer((UINT8 *)(&p), sizeof(p));
		if(ret)
		{
			print_info_with_u32("\r\nrd buf err ", ret);
			return NAND_RDBUF_FAIL;
		}

		if(onfi_crc16(ONFI_CRC_BASE, (UINT8 *)(&p), 254) == le16_to_cpu(p.crc))
		{
			break;
		}
	}

	if (3 == i)
	{
		print_info("\r\ncrc err");
		return NAND_CRC_ERR;
	}

	spec_real->page_size = le32_to_cpu(p.byte_per_page);
	spec_real->page_num_per_block = le32_to_cpu(p.pages_per_block);
	spec_real->oob_size = le16_to_cpu(p.spare_bytes_per_page);
	spec_real->chip_size = (le32_to_cpu(p.blocks_per_lun) * (UINT64)spec_real->page_size * spec_real->page_num_per_block * p.lun_count) >> 20;

	return OK;
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
INT32 is_full_id_nand(const struct nand_flash_dev *type)
{
	return type->id_len;
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
* 返 回 值  : TRUE - 是；FALSE - 不是
*
* 其它说明  :
*
*****************************************************************************/
INT32 find_full_id_nand(struct nand_spec_real *spec_real, const struct nand_flash_dev *type, UINT8 *id_data)
{
    UINT32 page_size_ffb =0;
	if (!memcmp(type->id, id_data, type->id_len))
	{
		spec_real->page_size = type->pagesize;
        page_size_ffb = ffs (spec_real->page_size);
		spec_real->page_num_per_block = (type->erasesize >>page_size_ffb);
		spec_real->oob_size = type->oobsize;
        spec_real->chip_size = (UINT64)type->chipsize;

		return TRUE;
	}
	return FALSE;
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
* 返 回 值  : TRUE - 有周期； FALSE - 没有周期
*
* 其它说明  : from Linux kernel 3.10
*
*****************************************************************************/
INT32 nand_id_has_period(UINT8 *id_data, INT32 arrlen, INT32 period)
{
	INT32 i, j;

	for (i = 0; i < period; i++)
	{
		for (j = i + period; j < arrlen; j += period)
		{
			if (id_data[i] != id_data[j])
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*
 * nand_id_len - Get the length of an ID string returned by CMD_READID
 * @id_data: the ID string
 * @arrlen: the length of the @id_data array

 * Returns the length of the ID string, according to known wraparound/trailing
 * zero patterns. If no pattern exists, returns the length of the array.
 */
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
INT32 nand_id_len(UINT8 *id_data, int arrlen)
{
	INT32 last_nonzero, period;

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
* 函 数 名  : nand_get_sparesize
*
* 功能描述  : 获取flash的sparesize
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
*****************************************************************************/
void nand_get_sparesize(struct nand_spec_real *spec_real)
{
    const struct nandc_ecc_info* ecc_table = nandc6_eccinfo;
    UINT32 ecctype = spec_real->ecc_type;
    UINT32 pagesize = spec_real->page_size;

    while(0 != ecc_table->pagesize)
    {
        if((pagesize == ecc_table->pagesize ) && (ecctype == ecc_table->ecctype))
        {
            spec_real->oob_size = ecc_table->sparesize;
            return;
        }
        ecc_table++;
    }
    return;
}

/*****************************************************************************
* 函 数 名  : nand_decode_ext_id
*
* 功能描述  : 解析nand id
*
* 输入参数  : @spec_real - nand spec info
*			  @id_data - nand flash id
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  : Many new NAND share similar device ID codes, which represent the size of the
* 			  chip. The rest of the parameters must be decoded according to generic or
* 			  manufacturer-specific "extended ID" decoding patterns.
*
*****************************************************************************/
void nand_decode_ext_id(struct nand_spec_real *spec_real, UINT8 id_data[NAND_ID_SIZE])
{
	UINT8 cellinfo = id_data[2];				/* The 3rd id byte holds MLC / multichip data */
	UINT8 extid = id_data[3]; 					/* The 4th id byte is the important one */
	UINT8 id_len = nand_id_len(id_data, NAND_ID_SIZE);

	UINT32 page_size = 0, page_size_ffb = 0, oob_size = 0, page_num_per_block = 0, byte_num_per_block = 0;

    /*
	 * Field definitions are in the following datasheets:
	 * Old style (4,5 byte ID): Samsung K9GAG08U0M (p.32)
	 * New Samsung (6 byte ID): Samsung K9GAG08U0F (p.44)
	 * Hynix MLC   (6 byte ID): Hynix H27UBG8T2B (p.22)
	 *
	 * Check for ID length, non-zero 6th byte, cell type, and Hynix/Samsung
	 * ID to decide what to do.
	 */
	if (id_len == 6 && id_data[0] == NAND_MFR_SAMSUNG &&
			(cellinfo & NAND_CI_CELLTYPE_MSK) && id_data[5] != 0x00)
	{
		/* Calc pagesize */
		page_size= NANDC_SIZE_2K << (extid & 0x03);
		extid >>= 2;

		/* Calc oobsize */
		switch (((extid >> 2) & 0x04) | (extid & 0x03)) {
		case 1:
			oob_size = NANDC_SPARE_LENGTH_128BYTE;
			break;
		case 2:
			oob_size = NANDC_SPARE_LENGTH_218BYTE;
			break;
		case 3:
			oob_size = NANDC_SPARE_LENGTH_400BYTE;
			break;
		case 4:
			oob_size = NANDC_SPARE_LENGTH_436BYTE;
			break;
		case 5:
			oob_size = NANDC_SPARE_LENGTH_512BYTE;
			break;
		case 6:
		default:
			oob_size = NANDC_SPARE_LENGTH_640BYTE; /* Other cases are "reserved" (unknown) */
			break;
		}
		extid >>= 2;

		/* Calc blocksize */
		byte_num_per_block = ((NANDC_BLOCK_128PAGE * NANDC_SIZE_1K) <<
			(((extid >> 1) & 0x04) | (extid & 0x03)));
	} else if (id_len == 6 && id_data[0] == NAND_MFR_HYNIX &&
			(cellinfo & NAND_CI_CELLTYPE_MSK)) {
		unsigned int tmp;

		/* Calc pagesize */
		page_size = NANDC_SIZE_2K << (extid & 0x03);
		extid >>= 2;

		/* Calc oobsize */
		switch (((extid >> 2) & 0x04) | (extid & 0x03)) {
		case 0:
			oob_size = NANDC_SPARE_LENGTH_128BYTE;
			break;
		case 1:
			oob_size = NANDC_SPARE_LENGTH_224BYTE;
			break;
		case 2:
			oob_size = NANDC_SPARE_LENGTH_448BYTE;
			break;
		case 3:
			oob_size = NANDC_SPARE_LENGTH_64BYTE;
			break;
		case 4:
			oob_size = NANDC_SPARE_LENGTH_32BYTE;
			break;
		case 5:
			oob_size = NANDC_SPARE_LENGTH_16BYTE;
			break;
		default:
			oob_size = NANDC_SPARE_LENGTH_640BYTE;
			break;
		}
		extid >>= 2;

		/* Calc blocksize */
		tmp = ((extid >> 1) & 0x04) | (extid & 0x03);
		if (tmp < 0x03)
			byte_num_per_block = ((NANDC_BLOCK_128PAGE * NANDC_SIZE_1K) << tmp) ;
		else if (tmp == 0x03)
			byte_num_per_block = NANDC_BLOCK_768PAGE * NANDC_SIZE_1K ;
		else
			byte_num_per_block = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << tmp);
	}
	/* SANSUNG SLC nand */
	else if (id_data[0] == NAND_MFR_SAMSUNG)
	{
		/* Calc pagesize */
		page_size = NANDC_SIZE_1K << (extid & 0x03);
		extid >>= 2;

		/* Calc oobsize */
		oob_size = (8 << (extid & 0x03)) * (page_size >> 9);
		extid >>= 2;

		/* Calc blocksize. Blocksize is multiples of 64KiB */
		byte_num_per_block = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << (extid & 0x03)) ;
	}
	/* Toshiba nand */
	else if (id_data[0] == NAND_MFR_TOSHIBA)
	{
		/* Calc pagesize */
		page_size = NANDC_SIZE_1K << (extid & 0x03);
		extid >>= 4;

		/* Calc blocksize. Blocksize is multiples of 64KiB */
		byte_num_per_block = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << (extid & 0x03)) ;

		if(id_data[4] & NAND_ONCHIP_ECC) /* Toshiba benand */
		{
			/* 据东芝FAE反馈，Toshiba benand只有2k(24nm, 64byte spare size)和4k(24nm,32nm, 都是128 byte spare size)这三种 */
			if(NANDC_SIZE_2K == page_size)
			{
				oob_size = NANDC_SPARE_LENGTH_64BYTE;
			}
			else
			{
				oob_size = NANDC_SPARE_LENGTH_128BYTE;
			}
		}
		else /* Toshiba SLC nand */
		{
			UINT32 ecc_type = 0;

			/* 根据pagesize和加工工艺确定ecc type */
			if(NANDC_SIZE_2K == page_size)				/* 2k 的只有24nm和43nm */
			{
				if(NAND_TOSHIBA_43NM == (NAND_TOSHIBA_TECH_MASK & id_data[5]))
				{
					ecc_type = NANDC_ECCTYPE_ECC_4BIT;
				}
				else /* NAND_TOSHIBA_24NM */
				{
					ecc_type = NANDC_ECCTYPE_ECC_8BIT;
				}

			}
			else if(NANDC_SIZE_4K == page_size)			/* 4k的只有24nm和32nm */
			{
				if(NAND_TOSHIBA_24NM == (NAND_TOSHIBA_TECH_MASK & id_data[5]))
				{
					ecc_type = NANDC_ECCTYPE_ECC_8BIT;
				}
				else /* NAND_TOSHIBA_32NM、NAND_TOSHIBA_43NM */
				{
					ecc_type = NANDC_ECCTYPE_ECC_4BIT;
				}
			}
			else /* NANDC_SIZE_8K，只有24nm和43nm */
			{
				if(NAND_TOSHIBA_24NM == (NAND_TOSHIBA_TECH_MASK & id_data[5]))
				{
					ecc_type = NANDC_ECCTYPE_ECC_24BIT;
				}
				else /* NAND_TOSHIBA_43NM */
				{
					ecc_type = NANDC_ECCTYPE_ECC_4BIT;
				}
			}
			spec_real->ecc_type = ecc_type;
			nand_get_sparesize(spec_real);
		}

	}
	/* default branch */
	else
	{
		/* Calc pagesize */
		page_size = NANDC_SIZE_1K << (extid & 0x03);
		extid >>= 2;

		/* Calc oobsize */
		oob_size = (8 << (extid & 0x01)) * (page_size >> 9);
		extid >>= 2;

		/* Calc blocksize. Blocksize is multiples of 64KiB */
		byte_num_per_block = ((NANDC_BLOCK_64PAGE * NANDC_SIZE_1K) << (extid & 0x03));
	}
    page_size_ffb = ffs (page_size);
    page_num_per_block = (byte_num_per_block >>page_size_ffb);
	spec_real->oob_size = oob_size;
	spec_real->page_size = page_size;
	spec_real->page_num_per_block = page_num_per_block;

	return;
}

/*****************************************************************************
* 函 数 名  : nand_get_ecctype
*
* 功能描述  : 获取flash可以使用的最大ecc type
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
*****************************************************************************/
void nand_get_ecctype(struct nand_spec_real *spec_real)
{
    const struct nandc_ecc_info* ecc_table = nandc6_eccinfo;
    UINT32 oobsize = spec_real->oob_size ;
    UINT32 pagesize = spec_real->page_size;

	/* 东芝 SLC nand已经给spec_real->ecc_type赋值，不需要再查询ecc */
	if(spec_real->ecc_type != 0)
	{
		return;
	}

    while(0 != ecc_table->pagesize)
    {
        if((pagesize == ecc_table->pagesize ) && (oobsize >= ecc_table->sparesize))
        {
            spec_real->ecc_type = ecc_table->ecctype;
            return;
        }
        ecc_table++;
    }
    return;
}

/*****************************************************************************
* 函 数 名  : print_nand_spec
*
* 功能描述  : 打印nand flash spec info，用于debug
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  : null
*
*****************************************************************************/
void print_nand_spec(struct nand_spec_real *spec_real)
{
	print_info_with_u32("\r\nNF pagesz 0x", spec_real->page_size);
	print_info_with_u32("B,pagenm 0x", spec_real->page_num_per_block);
	print_info_with_u32(",oobsz 0x", spec_real->oob_size);
	print_info_with_u32("B,ecc 0x", spec_real->ecc_type);
	print_info_with_u32(",addrnum 0x", spec_real->addr_cycle);
	print_info_with_u32(",chipsz 0x", spec_real->chip_size);
	print_info("MB");
	return;
}


/*****************************************************************************
* 函 数 名  : get_spec_from_id
*
* 功能描述  : 从nand flash id获取nand spec info
*
* 输入参数  : @spec_real - nand spec info
* 输出参数  : null
*
* 返 回 值  : OK - 成功；else - 失败
*
* 其它说明  :
*
*****************************************************************************/
long get_spec_from_id(struct nand_spec_real *spec_real)
{
	UINT8 id1[NAND_ID_SIZE], id2[NAND_ID_SIZE];
	UINT32 i, ret, temp;
	const struct nand_flash_dev *type = nand_flash_ids;

	/* read id two times*/
	ret = nand_send_cmd_id(NAND_READ_ID_EXT);
	if(ret)
	{
        print_info_with_u32("\r\nrd id1 err ", ret);
		return NAND_READID_FAIL;
	}

	for(i = 0; i < NAND_ID_SIZE; i++)
	{
		id1[i] = nand_readbuffer_byte(i);
	}

	ret = nand_send_cmd_id(NAND_READ_ID_EXT);
	if(ret)
	{
        print_info_with_u32("\r\nrd id2 err ", ret);
		return NAND_READID_FAIL;
	}

	for(i = 0; i < NAND_ID_SIZE; i++)
	{
		id2[i] = nand_readbuffer_byte(i);
	}

	/* compare two id, if different, return error */
	if(id1[0] != id2[0] || id1[1] != id2[1])
	{
        print_info("\r\nNF id err ");
		print_info_with_u32(" ", id1[0]);
		print_info_with_u32(" ", id1[1]);
		print_info_with_u32(" ", id2[0]);
		print_info_with_u32(" ", id2[1]);

        return NAND_ID_NOT_MATCH;
	}

	/* 打印8byte的nand flash id，方便debug */
	temp = (id1[0] << 24) | (id1[1] << 16 ) | (id1[2] << 8 ) | (id1[3] << 0 );
	print_info_with_u32("\r\nNF ID 0x", temp);

	temp = (id1[4] << 24) | (id1[5] << 16 ) | (id1[6] << 8 ) | (id1[7] << 0 );
	print_info_with_u32(" 0x", temp);

	/* check if it's full id nand */
	for (; type->name != NULL; type++)
	{
		if (is_full_id_nand(type))
		{
			if (find_full_id_nand(spec_real, type, id1))
			{
				goto DONE;
			}
		}
        else if (id1[1] == type->dev_id)
        {
			break;
		}
	}

	/* decode id */
	ret = nand_detect_onfi(spec_real);
	if(!ret)
	{
		goto DONE;
	}

   	nand_decode_ext_id(spec_real, id1);
	spec_real->chip_size = (UINT64)type->chipsize;	/* chip size以MByte为单位，此处可以直接赋值 */

DONE:

	nand_get_ecctype(spec_real);

    /* get addr cycle */
    spec_real->addr_cycle = (spec_real->chip_size > NANDC_CHIPSIZE_128M) ? NANDC_ADDRCYCLE_5 : NANDC_ADDRCYCLE_4;

	/* 打印nand spec信息，方便debug */
	print_nand_spec(spec_real);

	return OK;
}

/*****************************************************************************
* 函 数 名  : nand_set_default_for_read_id
*
* 功能描述  : 读nand flash id前的准备工作
*
* 输入参数  : null
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
*****************************************************************************/
void nand_set_default_config(void)
{
	/* set bus width 8bit */
	set_hi_nfc_con_bus_width(NANDC_BUSWIDTH_8BIT_REG);

    /* disable interupt */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_INTEN_OFFSET, NANDC_INT_DISABLE_ALL_REG);

	/* ints clear */
	OUTREG32(HI_NANDC_REGBASE_ADDR + HI_NFC_INTCLR_OFFSET, NANDC_INTS_CLEAR_ALL_REG);

	/* chip select */
	set_hi_nfc_op_nf_cs(NANDC_CHIP_SELECT_0_REG);

    /* set page size default(2k) for read id */
    set_hi_nfc_con_pagesize(NANDC_PAGE_SIZE_2K_REG);

    return;
}

/*****************************************************************************
* 函 数 名  : bl_len_check
*
* 功能描述  : 检查bootloader长度是否合法
*
* 输入参数  : @blLen - bootloader length
* 输出参数  : null
*
* 返 回 值  : OK - 合法；SEC_IMAGE_LEN_ERROR - 不合法
*
* 其它说明  :
*
*****************************************************************************/
INT32 bl_len_check(UINT32 bootloader_len)
{
	/* 判断长度是否合法:不为零/字对齐/不翻转/不过大 */
    /* 0x1234ABCD - read retry failed */
    /* 0xABCD1234 - all block(0 to 7) is bad */
    if((0 == bootloader_len) ||
       (bootloader_len % 4)  ||
       (bootloader_len + IDIO_LEN + OEM_CA_LEN + IDIO_LEN < bootloader_len) ||
       (bootloader_len + IDIO_LEN + OEM_CA_LEN + IDIO_LEN > BOOTLOAD_SIZE_MAX))
    {
        return SEC_IMAGE_LEN_ERROR;
    }else{
		return OK;
	}
}

/*****************************************************************************
* 函 数 名  : nand_spec_info_save
*
* 功能描述  : 将nand spec info保存到nand_spec_buf
*
* 输入参数  : @spec_real - nand spec info
*			  @spec_sram - buffer to save nand spec info
* 输出参数  : null
*
* 返 回 值  : null
*
* 其它说明  :
*
* 格式  :
*			--------------------------------------------------------------------------------------
* ofs	:	   28    25    	    21          16        		   12         8    	    4    	   0
* bit	:	 0000   000    	  0000       00000  	  		 0000      0000 	 0000 	    0000
*           |  reserved	| chipsize | sparesize |  pagenumperblock | addrnum | ecctype | pagesize |
*           --------------------------------------------------------------------------------------
*
*****************************************************************************/
void nand_spec_info_save(struct nand_spec_real *spec_real, volatile struct nand_spec_sram *spec_sram)
{
	if(!spec_real || !spec_sram)
	{
		print_info_with_u32("\r\nerr arg:", (UINT32)spec_sram);
		return;
	}

	memset((void *)spec_sram, 0, sizeof(struct nand_spec_sram));

	/* ecc type */
	switch(spec_real->ecc_type)
	{
		case NANDC_ECCTYPE_ECC_4BIT:
			spec_sram->ecc_type = NANDC_ECCTYPE_ECC_4BIT_REG;
			break;
		case NANDC_ECCTYPE_ECC_8BIT:
			spec_sram->ecc_type = NANDC_ECCTYPE_ECC_8BIT_REG;
			break;
		case NANDC_ECCTYPE_ECC_24BIT:
			spec_sram->ecc_type = NANDC_ECCTYPE_ECC_24BIT_REG;
			break;
		default:
			/* NOP */
            break;
	};

	/* page size */
	switch(spec_real->page_size)
	{
		case NANDC_SIZE_2K:
			spec_sram->page_size = NANDC_PAGE_SIZE_2K_REG;
			break;
		case NANDC_SIZE_4K:
			spec_sram->page_size = NANDC_PAGE_SIZE_4K_REG;
			break;
		case NANDC_SIZE_8K:
			spec_sram->page_size = NANDC_PAGE_SIZE_8K_REG;
			break;
		default:
			/* NOP */
            break;
	};

    /* page num per block */
	switch(spec_real->page_num_per_block)
	{
		case NANDC_BLOCK_64PAGE:
			spec_sram->page_num_per_block = NANDC_PAGENUM_PER_BLOCK_64_REG;
			break;
		case NANDC_BLOCK_128PAGE:
			spec_sram->page_num_per_block = NANDC_PAGENUM_PER_BLOCK_128_REG;
			break;
		case NANDC_BLOCK_256PAGE:
			spec_sram->page_num_per_block = NANDC_PAGENUM_PER_BLOCK_256_REG;
			break;
		case NANDC_BLOCK_512PAGE:
			spec_sram->page_num_per_block = NANDC_PAGENUM_PER_BLOCK_512_REG;
			break;
		default:
			/* NOP */
            break;
	};

	/* addr cycle */
	switch(spec_real->addr_cycle)
	{
		case NANDC_ADDRCYCLE_4:
			spec_sram->addr_num = NANDC_ADDRCYCLE_4_BOOT_REG;
			break;
		case NANDC_ADDRCYCLE_5:
			spec_sram->addr_num = NANDC_ADDRCYCLE_5_BOOT_REG;
			break;
		default:
			/* NOP */
            break;
	};

	/* chip size */
	switch(spec_real->chip_size)
	{
		case NANDC_CHIPSIZE_64M:
			spec_sram->chip_size = NANDC_CHIPSIZE_64M_REG;
			break;
		case NANDC_CHIPSIZE_128M:
			spec_sram->chip_size = NANDC_CHIPSIZE_128M_REG;
			break;
		case NANDC_CHIPSIZE_256M:
			spec_sram->chip_size = NANDC_CHIPSIZE_256M_REG;
			break;
		case NANDC_CHIPSIZE_512M:
			spec_sram->chip_size = NANDC_CHIPSIZE_512M_REG;
			break;
		case NANDC_CHIPSIZE_1G:
			spec_sram->chip_size = NANDC_CHIPSIZE_1G_REG;
			break;
		case NANDC_CHIPSIZE_2G:
			spec_sram->chip_size = NANDC_CHIPSIZE_2G_REG;
			break;
		case NANDC_CHIPSIZE_4G:
			spec_sram->chip_size = NANDC_CHIPSIZE_4G_REG;
			break;
		case NANDC_CHIPSIZE_8G:
			spec_sram->chip_size = NANDC_CHIPSIZE_8G_REG;
			break;
		case NANDC_CHIPSIZE_16G:
			spec_sram->chip_size = NANDC_CHIPSIZE_16G_REG;
			break;
		case NANDC_CHIPSIZE_32G:
			spec_sram->chip_size = NANDC_CHIPSIZE_32G_REG;
			break;
		case NANDC_CHIPSIZE_64G:
			spec_sram->chip_size = NANDC_CHIPSIZE_64G_REG;
			break;
	}

	/* spare size */
	switch(spec_real->oob_size)
	{
		case NANDC_SPARE_LENGTH_16BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_16BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_32BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_32BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_60BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_60BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_64BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_64BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_88BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_88BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_116BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_116BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_128BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_128BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_144BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_144BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_200BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_200BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_218BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_218BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_224BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_224BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_232BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_232BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_256BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_256BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_368BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_368BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_400BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_400BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_436BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_436BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_448BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_448BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_512BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_512BYTE_REG;
			break;
		case NANDC_SPARE_LENGTH_640BYTE:
			spec_sram->spare_size = NANDC_SPARE_LENGTH_640BYTE_REG;
			break;
		default:
			/* NOP */
			break;
	}

	if((0 == spec_sram->spare_size) || (0 == spec_sram->chip_size))
	{
		spec_sram->flag = NAND_ARGS_PART_AVAIL;
	}
	else
	{
		spec_sram->flag = NAND_ARGS_FULL_AVAIL;
	}

	return;
}

/*****************************************************************************
* 函 数 名  : nand_get_spec_info
*
* 功能描述  : 获取nand spec info
*
* 输入参数  : @spec - nand spec info
* 输出参数  : null
*
* 返 回 值  : OK - 成功； else - 失败
*
* 其它说明  :
*
*****************************************************************************/
INT32 nand_get_spec_info(struct nand_spec_real *spec_real)
{
	/* try to get nand spec info from efuse or nand flash id */
	if(!get_spec_from_efuse(spec_real))
	{
        return OK;
	}
	else
	{
		return get_spec_from_id(spec_real);
	}
}

/*****************************************************************************
* 函 数 名  : nand_boot_config_save
*
* 功能描述  : 沿用v7r2的方式启动(nand管脚或efuse保存nand spec info)时，把nand
*			  spec info保存到SRAM中
*
* 输入参数  : @nand_spec_buf - SRAM中的buffer
* 输出参数  :
*
* 返 回 值  : null
*
* 其它说明  : 由于pagesize/addrnum/ecctype/pagenumperblock这几个参数，
*			  保存到SRAM中的值与寄存器中的值一致，所以这里可以直接赋值
*
*****************************************************************************/
void nand_boot_config_save(volatile struct nand_spec_sram *spec_sram)
{
	if(!spec_sram)
	{
		print_info("\r\nerr arg1");
		return;
	}

	spec_sram->page_size = get_hi_nfc_boot_cfg_page_size_pad();
	spec_sram->addr_num = get_hi_nfc_boot_cfg_addr_num_pad();
	spec_sram->ecc_type = get_hi_nfc_boot_cfg_ecc_type_pad();
	spec_sram->page_num_per_block = get_hi_nfc_boot_cfg_block_size_pad();
	spec_sram->flag = NAND_ARGS_PART_AVAIL;
	return;
}

/*****************************************************************************
* 函 数 名  : nandReadBl
*
* 功能描述  : 根据实际长度读取BootLoader
*
* 输入参数  : @dest - BootLoader读取的目的地
*			  @boot_mode - 启动模式
*				ONLY_NAND_BOOT_MODE: 从nand启动
*				EFUSE_BOOT_MODE: 从efuse获取nand spec info
*				NAND_PARSE_ID_MODE: 从nand id获取nand spec info
* 输出参数  :
*
* 返 回 值  : OK - 读取成功;
*             NAND_ECC_ERR - ECC出现不可纠正的错误
*             SEC_IMAGE_LEN_ERROR - 长度错误
*
* 其它说明  :
*
*****************************************************************************/
int nandReadBl(UINT32 dest , enum en_boot_mode nand_boot_mode, volatile struct nand_spec_sram *spec_sram)
{
    UINT32 blLen, ulEccType;
	INT32 ret;
	struct nand_spec_real spec_real;

	/* init */
	memset((void *)(&spec_real), 0, sizeof(struct nand_spec_real));

	if(spec_sram != NULL)
	{
		memset((void *)spec_sram, 0, sizeof(struct nand_spec_sram));
	}

	/* 配置IO复用,NAND取默认配置。*/
    NF_IOS_SYS_CONFIG();

    /* 配置脉宽为0x555 */
    OUTREG32(NANDC_PWIDTH, NANDC_PWIDTH_555_REG);

	/* delay */
    delay(10);

	if(ONLY_NAND_BOOT_MODE == nand_boot_mode)
	{
		/* save spec */
		nand_boot_config_save(spec_sram);
	}
	else
	{
		/* set normal mode */
		set_hi_nfc_con_op_mode(NANDC_NORMAL_MODE_REG);

	    nand_set_default_config();

		if(EFUSE_BOOT_MODE == nand_boot_mode)
		{
			ret = get_spec_from_efuse(&spec_real);
			if(ret)
			{
				print_info_with_u32("\r\n efu spec err ", ret);
				return ret;
			}
		}
		else	/* NAND_PARSE_ID_MODE */
		{
			/* 获取nandc spec info */
			ret = nand_get_spec_info(&spec_real);
			if(ret)
			{
		        print_info_with_u32("\r\nNF spec err: ", ret);
		        return ret;
			}
		}

		/* save nand spec info to SRAM */
	    nand_spec_info_save(&spec_real, spec_sram);

	    /* set nand spec info to reg */
	    nand_set_spec_for_boot(&spec_real);

	    /* set boot mode */
		set_hi_nfc_con_op_mode(NANDC_BOOT_MODE_REG);

	}

	/* 打印传递给fastboot的nand spec值，方便debug */
    print_info_with_u32("\r\nNand save 0x", *((UINT32 *)spec_sram));

    /* 检查是否为Boot模式,如果不是,则直接重启,再次尝试读取 */
    if(NANDC_OPMODE_BOOT != (INREG32(NANDC_CON) & NANDC_OPMODE_MASK))
    {
        print_info("\r\nnot in boot mode,reboot to try...");
        setErrno(NAND_NO_IN_BOOTMODE);

        wdtRebootDelayMs(TIME_DELAY_MS_6000_FOR_NF_OPBOOT);
    }

    /* 获取BootLoader长度 */
    blLen = *(volatile UINT32 *)(NANDC_BUFFER_BASE_ADDR + BL_LEN_INDEX);

    /* 获取ECC Type */
    ulEccType = get_hi_nfc_con_ecc_type();

    /* boot模式下，不能使用ecc0 mode */
	if(NANDC_ECCTYPE_ECC_NONE_REG == ulEccType)
	{
        print_info("\r\necc0 mode err!");
        return NAND_ECC_MODE_ERR;
	}

    /* 使能ECC情况下,产生ECC不可纠正的错误 */
    if(get_hi_nfc_ints_err_invalid())
    {
        print_info("\r\necc err!");
        return NAND_ECC_ERR;
    }

    /* 判断长度是否合法:不为零/字对齐/不翻转/不过大 */
    /* 0x1234ABCD - read retry failed */
    /* 0xABCD1234 - all block(0 to 7) is bad */
    if((0 == blLen)
        || (blLen % 4)
        || (blLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN < blLen)
        || (blLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN > BOOTLOAD_SIZE_MAX))
    {
        print_info_with_u32("\r\nBL len err:", blLen);

        return SEC_IMAGE_LEN_ERROR;
    }

    /* 加上镜像签名、OEM CA和OEM CA签名的长度 (安全校验时才添加)*/
    blLen += IDIO_LEN + OEM_CA_LEN + IDIO_LEN;

    delay(10);

    /* Boot模式下直接读取整个BootLoader */
    memcpy((void*)dest, (void*)NANDC_BUFFER_BASE_ADDR, blLen);

    return OK;
}

/*****************************************************************************
* 函 数 名  : bsp_nand_get_spec_and_save
*
* 功能描述  : 获取nand spec信息并保存，给先贴后烧用
*
* 输入参数  : @spec_sram - 用以保存nand spec info
* 输出参数  : null
*
* 返 回 值  : 0 - 成功;
*             else - 失败
*
* 其它说明  :
*
*****************************************************************************/
int bsp_nand_get_spec_and_save(volatile struct nand_spec_sram *spec_sram)
{
	struct nand_spec_real spec_real;
	long ret = 0;

	/* argu check */
	if(NULL == spec_sram)
	{
		return -1;
	}

	/* init */
	memset((void *)(&spec_real), 0, sizeof(struct nand_spec_real));
	memset((void *)spec_sram, 0, sizeof(struct nand_spec_sram));

	/* get nand spec */
	ret = get_spec_from_id(&spec_real);
	if(ret)
	{
		return ret;
	}

	/* save nand spec info to SRAM */
    nand_spec_info_save(&spec_real, spec_sram);

	/* done */
	return 0;
}


