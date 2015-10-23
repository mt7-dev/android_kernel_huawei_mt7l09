
#ifdef __cplusplus
extern "C"
{
#endif
/*lint --e{537}*/
#include <hi_syscrg_interface.h>
#include "boot/boot.h"
#include "bsp_nandc.h"
#include <bsp_efuse.h>
#include <balongv7r2/efuse_nandc.h>


extern int bsp_get_nand_info(struct nand_spec *spec);

/******************************************************************************
* 函 数 名: do_burn
* 功能描述: 将flash spec信息烧写到efuse
* 输入参数: @spec:存放flash info
* 输出参数: 无
* 返 回 值: 0 - 成功； 非0 - 失败
* 函数说明:
*******************************************************************************/
static s32 do_burn(struct nand_spec *spec)
{
	s32 ret = ERROR;
	u32 efuse_nand_info_addr = EFUSE_NANDC_GROUP_OFFSET;
    u32 read_value;
    u32 value = 0;

	/* param error */
	if(!spec)
	{
		PRINT_MSG("Param failed\n");
		goto erro;
	}

	/* make value */
    switch(spec->pagenumperblock)       /* add page num per block */
    {
        case NANDC_BLOCK_64PAGE:
            value |= EFUSE_NANDC_BLOCKSIZE_64PAGE;
            break;
        case NANDC_BLOCK_128PAGE:
            value |= EFUSE_NANDC_BLOCKSIZE_128PAGE;
            break;
        default:
			break;
    }

	PRINT_MSG("Add block size: 0x%x\n", value);

    /*lint -save -e30, -e142*/
	switch(spec->ecctype)         		/* add ecc type */
	{
	    case NANDC_ECC_NONE:
            value |= EFUSE_NANDC_ECCTYPE_NONE;
			break;
		case NANDC_ECC_1BIT:
            value |= EFUSE_NANDC_ECCTYPE_1BIT;
			break;
		case NANDC_ECC_4BIT:
			value |= EFUSE_NANDC_ECCTYPE_4BIT;
			break;
		case NANDC_ECC_8BIT:
			value |= EFUSE_NANDC_ECCTYPE_8BIT;
			break;
		default:
			break;
	}
    /*lint -restore*/

	PRINT_MSG("Add ecc type: 0x%x\n", value);

	switch(spec->pagesize)				/* add page size */
    {
        case NANDC_SIZE_HK:
			value |= EFUSE_NANDC_PAGESIZE_hK;
			break;
		case NANDC_SIZE_2K:
			value |= EFUSE_NANDC_PAGESIZE_2K;
			break;
		case NANDC_SIZE_4K:
			value |= EFUSE_NANDC_PAGESIZE_4K;
			break;
		case NANDC_SIZE_8K:
			value |= EFUSE_NANDC_PAGESIZE_8K;
			break;
		default:
			break;
	}

	PRINT_MSG("Add page size: 0x%x\n", value);

    switch(spec->addrnum)               /* add addr num */
    {
        case NANDC_ADDRCYCLE_4:
            value |= EFUSE_NANDC_ADDRNUM_4CYCLE;
            break;
        case NANDC_ADDRCYCLE_5:
            value |= EFUSE_NANDC_ADDRNUM_5CYCLE;
            break;
        default:
			break;
    }

	PRINT_MSG("Add addr num: 0x%x\n", value);

	/* set burn flag */
	value |= EFUSE_NANDC_BURNFLAG_HADBURN;

	PRINT_MSG("Add burn flag: 0x%x\n", value);

    /* set configure nandc from efuse */
    value |= EFUSE_CONF_FROM_EFUSE;

	PRINT_MSG("Add conf flag: 0x%x\n", value);

	/* write info */
	ret = bsp_efuse_write(&value, efuse_nand_info_addr, EFUSE_NANDC_GROUP_LENGTH);
	if(ret)
	{
		PRINT_MSG("EFuse write failed\n");
		goto erro;
	}

    /* read and compare */
    ret = bsp_efuse_read(&read_value, efuse_nand_info_addr, EFUSE_NANDC_GROUP_LENGTH);
    if(ret)
	{
		PRINT_MSG("EFuse read failed\n");
		goto erro;
	}

    if(read_value != value)
    {
        PRINT_MSG("EFuse value cmp error, write value: 0x%x, read value 0x%x\n", value, read_value);
        ret = ERROR;
		goto erro;
    }

    PRINT_MSG("SUCCESS: write nandc info successful\n");

	return OK;
erro:
	return ret;
}


/******************************************************************************
* 函 数 名: check_flag
* 功能描述: 检查buffer是否已经烧写了flash info
* 输入参数: @buffer: efuse中nandc info所在group的值
* 输出参数: 无
* 返 回 值: 1 - 已烧写；0 - 未烧写
* 函数说明:
*******************************************************************************/
static s32 check_flag(u32 buffer)
{
	if(buffer & EFUSE_NANDC_BURNFLAG_HADBURN)
	{
		return EFUSE_NANDC_HAD_BURN;
	}
	else
	{
		return EFUSE_NANDC_NOT_BURN;
	}
}

/******************************************************************************
* 函 数 名: is_burn
* 功能描述: 检查efuse是否已经烧写了flash info
* 输入参数: @burn_flag: 烧写标志。1 - 已烧写；0 - 未烧写
*			@buffer: efuse中nandc info所在group的值
* 输出参数: 无
* 返 回 值: 0 - 成功； 非0 - 失败
* 函数说明: 检查
*******************************************************************************/
static s32 is_burn(u32 *burn_flag)
{
	s32 ret;
	u32 efuse_nand_info_addr = EFUSE_NANDC_GROUP_OFFSET;
    u32 buffer = 0;

	ret = bsp_efuse_read(&buffer, efuse_nand_info_addr, EFUSE_NANDC_GROUP_LENGTH);
	if(ret)
	{
		PRINT_MSG("Efuse read info error\n");
		goto erro;
	}

	PRINT_MSG("After read: 0x%x\n", buffer);

	ret = check_flag(buffer);
	if(EFUSE_NANDC_NOT_BURN == ret)
	{
		*burn_flag = EFUSE_NANDC_NOT_BURN;
	}
	else
	{
		*burn_flag = EFUSE_NANDC_HAD_BURN;
	}

	return OK;
erro:
	return ret;
}

/******************************************************************************
* 函 数 名: burn_efuse_nandc
* 功能描述: 将flash spec info烧写进efuse
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0 - 成功； 非0 - 失败
* 函数说明:
*******************************************************************************/
s32 burn_efuse_nandc(void)
{
	s32 ret = ERROR;
	u32 burn_flag = 0;  /* 1 - already burn; 0 - not burn */
	struct nand_spec *spec;

	/* check whether nand info already burn */
    /* coverity[no_effect_test] */
	ret = is_burn(&burn_flag);
	if(ret)
	{
		PRINT_MSG("Check efuse info error\n");
		goto erro;
	}
	if(burn_flag)
	{
		PRINT_MSG("Efuse nandc info already burn\n");
		return OK;
	}

	/* get flash info  */
	spec = (struct nand_spec *)alloc(sizeof(struct nand_spec));
	if(!spec)
	{
		PRINT_MSG("Malloc memory failed\n");
		goto erro;
	}

	ret = bsp_get_nand_info(spec);
	if(OK != ret)
	{
		PRINT_MSG("Get flash info failed\n");
		goto erro;
	}

	PRINT_MSG("After get spec\n");
	PRINT_MSG("spec->pagesize			: 0x%x\n", spec->pagesize);
	PRINT_MSG("spec->pagenumperblock	: 0x%x\n", spec->pagenumperblock);
	PRINT_MSG("spec->addrnum			: 0x%x\n", spec->addrnum);
	PRINT_MSG("spec->ecctype			: 0x%x\n", spec->ecctype);


	/* burn flash info into efuse */
	ret = do_burn(spec);
	if(OK != ret)
	{
		PRINT_MSG("Burn flash info failed\n");
		goto erro;
	}

	return OK;

erro:
	return ret;
}

#ifdef __cplusplus
}
#endif


