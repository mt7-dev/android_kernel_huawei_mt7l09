/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Filename:    nand_balong.c
* Description: nandc模块在fastboot阶段提供对外的接口函数,其他模块要调用的接口函数都在此此文件中实现
*			   此文件的作用就是提供对外模块的API接口函数,但与文件中具体的函数实现又是调用nandc_nand.c
*			   文件中的具体函数
*******************************************************************************/
/* Description:此文件在nandc模块中与其他文件的关系图如下所示
*
*
*              nand_balong.c               nandc模块提供对外的接口函数都在此文件中实现,在此文件中调用下面的函数指针
*                   |
*                   |
*              nandc_nand.c                nandc模块中具体操作nand的操作，具体操作函数的实现，给上面的函数指针赋值
*                   |
*                   |
*        ------------------------
*          |                  |
*          |(init)            |(operation)
*       nandc_host.c       nandc_ctrl.c                Layer: 控制器操作层
*          |           -----------------------
*      nandc_native.c         |
*                      nandc_vxxx.c(nandc_600.c)       Layer: 具体的硬件操作
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __FASTBOOT__


#include <ptable_inc.h>
#include "nandc_balong.h"
extern char* strncpy(char *dest, const char *src, unsigned int count);
/*nandc模块的全局变量,包含了操作nandflash的具体函数*/
struct nand_bus  nand_handle = {0, 0};

/**
* 作用:nandc模块提供对外初始化nandc控制器的函数接口
*
* 参数:
* 无
* 描述:初始化nandc控制器的数据结构并调用底层的nandc硬件控制器的初始化函数
*
*/
u32 nand_init(void)
{
	struct nand_bus* handle = NULL;
	u32 result = NAND_ERROR;

	handle = &nand_handle;
	/*如果已经初始化完成了就直接退出来*/
	if(NAND_INIT_MAGIC == handle->init_flag)
	{
		result = NAND_OK;
		NAND_TRACE(("!!nand module already  inited!!\n"));
		goto EXIT;
	}

	memset((void*)handle, 0x00, sizeof(nand_handle));

	/*指向具体的操作nandflash的操作函数,当下面操作具体的nandflash的函数发生了变化时,可以保证对上面提供函数接口不变*/
	handle->funcs  =  &nandc_nand_funcs;

	/*nandflash硬件控制器的具体初始化函数*/
	if(handle->funcs->init)
	{
		result = handle->funcs->init();
	}
	else
	{
		NAND_TRACE(("ERRO!!nand module init is NULL\n"));
		result = NAND_ERROR;
	}
	/*初始化完成后设置标志位*/
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
* 作用:nandc模块提供对外通过分区名来查找分区表的函数接口
*
* 参数:
* @partition_name     ---分区表的名
* 描述:通过分区名返回分区表信息,查找成功返回分区表指针，查找不成功返回NULL
*
*/
struct ST_PART_TBL * find_partition_by_name(const char *partition_name)
{
	struct ST_PART_TBL * ptable = NULL;

	ptable_ensure();

	ptable = ptable_get_ram_data();

	/*遍历所有的分区表信息*/
	while(0 != strcmp(PTABLE_END_STR, ptable->name))
	{
		/*查找到就退出*/
	    if(0 == strcmp(partition_name, ptable->name))
	    {
	        break;
	    }
	    ptable++;
	}
	/*如果不为空就表示查找得所要的分区表信息*/
	if(0 == strcmp(PTABLE_END_STR, ptable->name))
	{
		NAND_TRACE(("ERROR: can't find partition %s, function %s\n", partition_name, __FUNCTION__));
	    return NULL;
	}

	return ptable;

}

/**
* 作用:nandc模块所有函数功能通用实现过程
*
* 参数:
* @param          ---包含了要操作nandflash的具体参数信息
* @func_type      ---函数的类型,根据此值知道要实现什么功能(读，写)
*
*
* 描述:所有的函数功能都是通过这个函数来处理，通过func_type来确定是要实现什么功能，直接调用具体的函数指针来
* 执行具体的操作,当需要增加新的函数功能时，就在此通过处理流程中增加相关的流程就行,方便后面功能的扩展
*
*/
u32 nand_run(struct nand_param* param, u32 func_type)
{
	struct nand_bus* handle;
	struct nand_interface * nandfuncs;
	u32 result = NAND_OK;

	handle = &nand_handle;
	/*判断是否初始化成功*/
	if(NAND_INIT_MAGIC != handle->init_flag)
	{
		NAND_TRACE(("error!! balong nand not inited\n"));
		return NAND_ERROR;
	}
	/*得到具体的操作的函数指针*/
	nandfuncs = handle->funcs;

	switch (func_type)
	{
		case READ_NAND:
		{
			/*使能ECC功能的读Flash操作*/
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
			/*不使能ECC功能的裸读Flash操作*/
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
			/*查询坏块的功能函数*/
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
			/*擦除数据块的功能函数*/
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
			/*使能ECC功能的写Flash操作,注意write中会自已先擦除Flash再来写,所有用户就不用在写之前擦除了*/
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
			/*不使能ECC功能的裸写Flash操作*/
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
			/*标坏块的功能*/
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
	/*返回操作的结果,0表示操作成功，1表示操作失败*/
	return result;
}


/**
* 作用:nandc模块使能ECC功能的读数据操作,注意此函数的读操作不能读OOB数据
*
* 参数:
* @flash_addr          ---要读的Flash地址
* @dst_data      	   ---要读到的内存地址
* @read_size      	   ---要读到长度
* @skip_length         ---过滤的长度,当在读的过程中遇到坏块时跳过要读的块，此结构记录跳过的长度
*
*
* 描述:实现的功能是使能ECC的读数据操作,注意不能读到OOB的数据，flash_addr,read_size这些参数都没有限制
* 首先构造nand_param这个变量，把要传入的参数赋值,再通过nand_run通用处理函数来实现相关功能
*/
u32 nand_read(FSZ flash_addr, u32 dst_data, u32 read_size, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*把要传入的参数赋值*/
    param.addr_flash    =   flash_addr;
    param.addr_data     =   dst_data;
    param.size_data     =   read_size;
    param.skip_length   =   skip_length;
	/*param.size_oob为0，因此是只读数据不读OOB*/
	/*nandflash使能ECC功能的读数据操作*/
    return nand_run(&param, READ_NAND);

}

/**
* 作用:nandc模块提供对外得到nandflash相关规格参数的函数接口
*
* 参数:
* @spec      ---把nandflash中的相关参数填写在此指针变量中去
*
* 描述:得到nandflash的规格参数
*/
u32 nand_get_spec(struct nand_spec *spec)
{
    struct nand_bus* handle;
    struct nand_interface * nandfuncs;

    handle = &nand_handle;
	/*如果没有初始化就返回错误*/
    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        NAND_TRACE(("error!! balong nand not inited\n"));
        return NAND_ERROR;
    }

    nandfuncs = handle->funcs;
	/*得到flash的规格参数*/
    if(nandfuncs->get_spec)
    {
        return  nandfuncs->get_spec(spec);
    }
	/*走到这里就说明有错误了走错误流程了*/
    NAND_TRACE(("error!! get_spec not inited\n"));
    return NAND_ERROR;
}

/**
* 作用:nandc模块提供对外读OOB数据和Flash数据的函数接口
*
* 参数:
* @flash_addr    ---要读的flash地址
* @dst_data      ---数据读到内存的地址,在这个地址中存放读的的Flash数据和OOB数据
* @data_size     ---要读数据的大小,这个数据长度表示了要读的Flash数据和OOB数据之和
* @oob_per_page  ---每一页数据要读的OOB数据的大小
* @skip_length   ---过滤的长度,当在读的过程中遇到坏块时跳过要读的块，此结构记录跳过的长度
*
*
* 描述:读Flash和OOB的数据存放到dst_data的内存中去，在内存中数据存放的格式是:pagesize(flashdata)+oob_per_page(oobdata)+pagesize(flashdata)+oob_per_page(oobdata)
* 使用注意:当oob_per_page不为0时，flash_addr要页地址对齐，且一般的使用时data_size是(pagesize+oob_per_page)的整数倍
*/
u32 nand_read_oob(FSZ flash_addr, u32 dst_data, u32 data_size, u32 oob_per_page, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*把要传入的参数赋值*/
    param.addr_flash    =   flash_addr;
    param.addr_data     =   dst_data;
    param.size_data     =   data_size;
	/*要读的OOB大小不为0表示是使能ECC功能的带OOB数据的读操作*/
    param.size_oob      =   oob_per_page;
    param.skip_length   =   skip_length;
	/*nandflash使能ECC功能的读数据操作*/
    return nand_run(&param, READ_NAND);

}


/**
* 作用:nandc模块提供对外裸读Flash数据的函数接口,在读的过程中不使能ECC
*
* 参数:
* @src_page      ---要读的flash页地址
* @offset_data   ---要读的flash页内偏移地址
* @dst_data      ---数据读到内存的地址,在这个地址中存放的数据格式是Flash颗粒的数据格式
* @len_data      ---要读数据的大小
* @skip_length   ---过滤的长度,当在读的过程中遇到坏块时跳过要读的块，此结构记录跳过的长度
*
*
* 描述:此功能要标坏块时有用和在当中ECC error时，可以用此功能把flash中的数据读出来查找ecc error的原因,
*一般的使用情况是offset_data为0，len_data为pagesize+sparesize的大小，注意offset_data+len_data不能超过pagesize+sparesize的大小
*/
/*注意读的数据不能超过一页的大小*/
u32 nand_read_raw(u32 src_page, u32 offset_data, u32 dst_data, u32 len_data, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*注意src_page表示的是flash的页地址*/
    param.number        = src_page;
    param.addr_flash    = offset_data;
    param.addr_data     = dst_data;
    param.size_data     = len_data;
    param.skip_length   = skip_length;
	/*nandflash不使能ECC功能的裸读数据操作*/
    return nand_run(&param, READ_RAW);

}


/**
* 作用:nandc模块提供对外判断此block是否是坏块函数接口
*
* 参数:
* @blockID       ---要检查的块号
* @flag          ---把检查的标志值存放在此变量中去,0为好坏，1为坏块
*
* 描述:检查此block是否是坏块，并把标记值存放在flag标志中
*/
u32 nand_isbad(u32 blockID, u32 *flag)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*设置查询的块号*/
    param.number    =   blockID;
	/*把标志位的地址传入*/
    param.addr_data =   (u32)flag;

    return nand_run(&param, QUARY_BAD);

}

/**
* 作用:nandc模块提供对外标记坏块函数接口
*
* 参数:
* @blockID       ---要标记坏块的块号
*
* 描述:标记此block是坏块，也就是把此block的第一页的OOB中的前两个字节清零
*/
u32 nand_bad(u32 blockID)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
    u32 ret;
	/*设置要标的坏块号*/
    param.number    =   blockID;
    ret = nand_run(&param, MARK_BAD);

    return ret;

}

/**
* 作用:nandc模块提供对外擦除Flash块函数接口
*
* 参数:
* @blockID       ---要擦除的Flash块的块号
*
* 描述:擦除Flash的数据块
*/

u32 nand_erase(u32 blockID)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*设置要擦除的块号*/
    param.number    =   blockID;

    return nand_run(&param, ERASE_BLOCK);

}

/**
 * 作用:nandc模块提供对使能ECC功能的写Flash数据函数接口
 *
 * 参数:
 * @flash_addr    ---要写的flash地址
 * @src_data      ---要写的数据在内存中的地址,
 * @write_size    ---要写的数据的大小
 * @skip_length   ---过滤的长度,当在写的过程中遇到坏块时跳过此块，此结构记录跳过的长度
 *
 *
 * 描述:此写接口在里面实现了Flash的擦除,因此在写的过程中不需要用户保证在写前面保证Flash是否已经擦除了,写的flash_ddr和write_size参数
   没有限制,注意此功能没有写OOB的功能
 */
u32 nand_write(FSZ flash_addr, u32 src_data, u32 write_size, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*传入参数*/
    param.addr_flash    =   flash_addr;
    param.addr_data     =   src_data;
    param.size_data     =   write_size;
    param.skip_length   =   skip_length;
	/*param.size_oob为0表示是不带OOB的写操作*/
    return nand_run(&param, WRITE_NAND);

}

/**
 * 作用:nandc模块提供对使能ECC功能的写Flash数据和OOB数据的函数接口
 *
 * 参数:
 * @flash_addr    ---要写的flash地址
 * @src_data      ---要写的数据在内存中的地址,
 * @data_size     ---要写的数据的大小,此大小包含了要写的数据和OOB之和
 * @oob_per_page  ---写每一页数据要写的OOB的长度
 * @skip_length   ---过滤的长度,当在写的过程中遇到坏块时跳过此块，此结构记录跳过的长度
 *
 *
 * 描述:此写接口在里面实现了Flash的擦除,因此在写的过程中不需要用户保证在写前面保证Flash是否已经擦除了,写的flash_ddr和write_size参数
   有限制,data_size是(pagesize+sparesize)的整数倍,注意此功能与上面的函数的区别是带OOB的写操作
   src_data中的数据格式是pagesize(flashdata)+sparesize(OOBdata)
 */
 u32 nand_write_oob(FSZ flash_addr, u32 src_data, u32 data_size, u32 oob_per_page, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*传入参数*/
    param.addr_flash    =   flash_addr;
    param.addr_data     =   src_data;
    param.size_data     =   data_size;
	/*param.size_oob不为0表示是带OOB的写操作*/
    param.size_oob      =   oob_per_page;
    param.skip_length   =   skip_length;

    return nand_run(&param, WRITE_NAND);

}

/**
 * 作用:nandc模块提供对外裸写Flash数据的函数接口,在写的过程中不使能ECC
 *
 * 参数:
 * @dst_page      ---要写的flash页地址
 * @offset_data   ---要写的flash页内偏移地址
 * @src_data      ---要写到Flash的数据的地址
 * @len_data      ---要写数据的大小
 * @skip_length   ---过滤的长度,当在读的过程中遇到坏块时跳过要读的块，此结构记录跳过的长度
 *
 *
 * 描述:此功能把数据裸写到flash中去,flash颗粒中的数据格式就是要写的数据格式
 *一般的使用情况是offset_data为0，len_data为pagesize+sparesize的大小，注意offset_data+len_data不能超过pagesize+sparesize的大小
 */
u32 nand_write_raw(u32 dst_page, u32 offset_data, u32 src_data, u32 len_data ,u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0};
	/*传入参数*/
    param.number        = dst_page;
    param.addr_flash    = offset_data;
    param.addr_data     = src_data;
    param.size_data     = len_data;
    param.skip_length   =   skip_length;
	/*不使用ECC功能的写操作*/
    return  nand_run(&param, WRITE_RAW);
}

/**
 * 作用:nandc模块提供对外扫描坏块的函数接口
 *
 * 参数:
 * @start         ---flash开始的地址
 * @length        ---扫描的长度
 * @if_erase      ---如果检测是好块根据此参数来决定是否擦除此块
 * 描述:扫描flash是否是坏块，根据参数来决定是否擦除
 */
u32 nand_scan_bad(FSZ start, FSZ length, u32 if_erase)
{
	u32 addr_block_align;
	u32 length_block_align;
	u32 blk_id, is_bad, times, ret;
	struct nand_spec spec;
	/*因为是在fastboot中实现，内存的释放是没有实现的，使用静态变量表示一次分配以后都不分配了*/
	static u32 block_buf = 0 ;

	memset(&spec, 0, sizeof(struct nand_spec));
	/*通过此函数得到nandflash的规格参数,知道blocksize的大小*/
	if(NAND_OK != nand_get_spec(&spec))
	{
		NAND_TRACE(("nand_scan_bad: nand_get_spec failed\n"));
		return NAND_ERROR;
	}

	/*扫描块时要块对齐*/
	addr_block_align = start - start%(spec.blocksize);
	/*得到要扫描的块长度*/
	length_block_align = nandc_alige_size(start + length, spec.blocksize) - addr_block_align;
	/*如果没有分配内存就分配内存*/
	if(!block_buf)
	{
		block_buf = (u32)himalloc(spec.blocksize);
		if(NULL == (void*)block_buf)//pclint 58
		{
			NAND_TRACE(("nand_scan_bad: himalloc failed\n"));
			goto ERRO;
		}
	}
	/*还有块长度需要扫描*/
	while(length_block_align > 0)
	{
		/*得到块号地址*/
		blk_id = addr_block_align/spec.blocksize;
		if(NAND_OK != nand_isbad(blk_id, &is_bad))
		{
			NAND_TRACE(("nand_scan_bad: nand_isbad failed\n"));
			goto ERRO;
		}
		if(NANDC_GOOD_BLOCK == is_bad)
		{
			times = 1;
			/*读整块数据如果没有出错的话表示是好坏*/
			while((NANDC_E_READ == nand_read((FSZ)addr_block_align, block_buf, spec.blocksize, 0)) && (times))
			{
			    times--;
			}
			if(0 == times)
			{
			    NAND_TRACE(("nand_scan_bad: find and read error, address:0x%x\n",addr_block_align));

			    ret = nand_bad(blk_id);
			    if(ret)
			    {
			        NAND_TRACE(("nand_scan_bad:nand check bad failed, ret = 0x%x\n",ret));
			    }
			}
			/*是好块如果要擦除的话就擦除*/
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


/**
* 作用:nandc模块提供对外判断nand控制器模块是否初始化的功能
*
* 参数:
* 无
*
*
* 描述:如果nandc模块已经初始化则返回1，没有初始化则返回0
*/
u32 nand_isinit(void)
{
    return (nand_handle.init_flag == NAND_INIT_MAGIC);
}

/**
* 作用:nandc模块提供对外清除nandc控制器数据结构的函数
*
* 参数:
* 无
*
*
* 描述:清除nandc控制器数据结构的内存
*/
void nand_init_clear(void)
{
    nandc_nand_host = NULL;
	/*数据结构清零*/
    memset((void*)&nand_handle, 0x00 ,sizeof(nand_handle));
}


/*下面的函数主要是为其他的模块提供的接口*/


/**
* 作用:读取分区相对偏移地址的标志值，主要是为NV模块用，查询此分区的偏移地址的标志位的值
*
* 参数:
* @partition_name         	---分区名
* @partition_offset         ---分区的相对偏移
* @flag                     ---把检测的分区的标志值存放在此flag中
* 描述:NV模块把一个分区的一个block的最后一页的OOB中存放特定标记值，读此标记值存放在flag中
*/
u32 bsp_nand_read_flag_nv(const char *partition_name, u32 partition_offset, unsigned char *flag)
{
    u32 flash_addr;
    u32 ret = NANDC_ERROR;
    static unsigned char *buffer = NULL;
    struct nand_spec spec;
    struct ST_PART_TBL * ptable = find_partition_by_name((char *)partition_name);
	/*参数不对*/
    if(!ptable)
    {
        goto ERRO;
    }
    if(!flag)
    {
        NAND_TRACE(("argu error.\n"));
        goto ERRO;
    }
	/*得到此分区的nandflash的规格参数*/
    ret = bsp_get_nand_info(&spec);
    if(ret)
    {
        goto ERRO;
    }
	/*没有内存时分配内存*/
    if(!buffer)
    {
        buffer = (unsigned char *)himalloc(spec.pagesize + YAFFS_BYTES_PER_SPARE);
        if(!buffer)
        {
    		NAND_TRACE(("get ram buffer failed!\n"));
    		goto ERRO;
        }
    }

    memset(buffer, 0xFF, spec.pagesize + YAFFS_BYTES_PER_SPARE);

    flash_addr = ptable->offset + partition_offset;
	/*使能ECC功能的带OOB读数据功能*/
    ret = nand_read_oob((flash_addr + spec.blocksize - spec.pagesize),
          (unsigned int)buffer, (spec.pagesize + YAFFS_BYTES_PER_SPARE),YAFFS_BYTES_PER_SPARE ,NULL);
    if(ret)
    {
		NAND_TRACE(("nand read oob failed!\n"));
		goto ERRO;
    }
    *flag = (*(buffer + spec.pagesize) == NV_WRITE_SUCCESS) ? NV_WRITE_SUCCESS : (~NV_WRITE_SUCCESS);

    return NANDC_OK;
ERRO:
    return ret;
}

/****************************************************************
函数功能: 在fastboot阶段获取分区大小
输入参数: partition_name-分区名称
输出参数: none
返回参数: 分区size
注意事项: 在fastboot阶段调用
****************************************************************/
unsigned int bsp_get_part_cap( const char *partition_name )
{
    unsigned int ret_size = 0;
    struct ST_PART_TBL * ptable = ( void* )( 0 );

    if ( ( void* )( 0 ) == partition_name )
    {
        cprintf( "fastboot: nv dload partition_name is NULL!\n" );
        goto ERRO;
    }

    ptable = find_partition_by_name(partition_name);
    if(!ptable)
    {
        cprintf( "fastboot: nv dload get ptable fail!\n" );
        goto ERRO;
    }

    ret_size = ptable->capacity;
    cprintf( "fastboot: nv dload cap is 0x%x.\n", ret_size );

ERRO:
    return ret_size;
}

/**
* 作用:nandc模块按分区名的使能ECC功能的读数据操作,注意此函数的读操作不能读OOB数据
*
* 参数:
* @partition_name          		---要读的分区名
* @partition_offset      	    ---要读分区的相对偏移地址
* @ptr_ram_addr      	        ---读到内存的地址
* @length      	        		---读的数据长度
* @skip_len                     ---过滤的长度,当在读的过程中遇到坏块时跳过要读的块，此结构记录跳过的长度
*
*
* 描述:根据分区名和分区的偏移地址来确定Flash的地址,把读到的数据存放在ptr_ram_addr中去
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
	/*通过分区名来转到Flash的地址*/
    flash_addr = ptable->offset + partition_offset;
	/*要Flash的数据到内存中*/
	return nand_read(flash_addr, (u32)ptr_ram_addr, length, skip_len);
ERRO:
    return ret;
}

/**
* 作用:nandc模块按分区名和分区偏移地址来擦除flash操作
*
* 参数:
* @partition_name          		---要擦除的分区名
* @partition_offset      	    ---要擦除的相对偏移地址
*
*
* 描述:根据分区名和分区的偏移地址来确定Flash的地址,再来擦除一个block,注意是擦除一个block数据块
*/
int bsp_nand_erase(const char *partition_name, u32 partition_offset)
{
    u32 flash_addr;
    u32 block_id, bad_flag;
    u32 ret = NANDC_ERROR;
	struct nandc_host  * host   = NULL;
    struct ST_PART_TBL * ptable = find_partition_by_name(partition_name);
	/*参数不正确*/
    if(!ptable)
    {
        goto ERRO;
    }

    /*得到flash的地址信息*/
    flash_addr = ptable->offset + partition_offset;

    host = nandc_nand_host;
    if(!host)
    {
        NAND_TRACE(("ERROR: function %s, line %d\n", __FUNCTION__, __LINE__));
        goto ERRO;
    }

    /*得到块号并判断是否是坏块*/
    block_id = flash_addr / host->nandchip->spec.blocksize;
    ret = nand_isbad(block_id, &bad_flag);
    if(ret)
    {
        NAND_TRACE(("ERROR: nand quary bad failed, function %s, line %d\n", __FUNCTION__, __LINE__));
        goto ERRO;
    }

    if(NANDC_BAD_BLOCK == bad_flag)
    {
        NAND_TRACE(("ERROR: try to erase a bad block, function %s, line %d\n", __FUNCTION__, __LINE__));
        goto ERRO;
    }
	/*擦除flash操作*/
	return nand_erase(block_id);

ERRO:
    return ret;
}


/**
* 作用:nandc模块按分区名和分区相对偏移来使能ECC功能的写数据操作,注意此函数的写操作带OOB数据且在写的过程中会擦除nandflash
*
* 参数:
* @partition_name          		---要写数据的分区名
* @partition_offset      	    ---要写数据的分区相对偏移地址
* @ptr_ram_addr          		---要写数据的地址
* @length      	    			---要写数据的长度
*
* 描述:根据分区名和分区的偏移地址来确定Flash的地址,再来写Flash操作
*/
s32 bsp_nand_write(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length)
{
    u32 flash_addr, ret = NANDC_ERROR;
    struct ST_PART_TBL * ptable = find_partition_by_name(partition_name);

    if(!ptable)
    {
        goto ERRO;
    }

    /*得到要写Flash的地址*/
    flash_addr = ptable->offset + partition_offset;
	/*在写的过程中会擦除nandflash,因此不需要调用者再来擦除一次*/
	return nand_write(flash_addr, (u32)ptr_ram_addr, length, NULL);

 ERRO:
    return ret;
}


/**
* 作用:nandc模块按分区名和分区相对偏移来确定此块是不是坏块
*
* 参数:
* @partition_name          		---查询数据块的分区名
* @partition_offset      	    ---查询数据块的分区相对偏移地址
*
* 描述:根据分区名和分区的偏移地址来确定Flash的地址,再来判断此block是不是坏块
返回值:
* 0表示此块是好块
* 1表示是坏块
* 其他的表示有错误
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

    /*得到Flash地址*/
    flash_addr = ptable->offset + partition_offset;

    host = nandc_nand_host;
    if(!host)
    {
        NAND_TRACE(("ERROR: function %s, line %d\n", __FUNCTION__, __LINE__));
        goto ERRO;
    }
	/*判断此block是否是坏块*/
	ret = nand_isbad(flash_addr / host->nandchip->spec.blocksize, &bad_flag);
    if(ret == 1)
    {
        return -6;
    }
    else if(ret > 1)
    {
        return ret;
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
* 作用:把内存中的分区表信息更新到nandc_nand_host中的数据结构中去
*
* 参数:
* 无
*
* 描述:把内存中的分区表信息更新到nandc_nand_host的数据结构中去，在fastboot中升级使用
*/
void bsp_update_ptable_to_nandc(void)
{
    nandc_init_mtd_partition(nandc_nand_host);
}


/**
 * 作用:得到nandflash相关规格参数的函数接口
 *
 * 参数:
 * @spec      ---把nandflash中的相关规格参数填写在此指针变量中去
 *
 *
 * 描述:得到nandflash的规格参数
 */
int bsp_get_nand_info(struct nand_spec *spec)
{
	struct nandc_host 	*host 		= nandc_nand_host;
	/*检查参数是否正确*/
	if(!host)
	{
	    NAND_TRACE(("ERROR: get nand info error.\n"));
	    goto erro;
	}

	if(!nand_isinit())
	{
	    NAND_TRACE(("ERROR: nandc driver not init yet!\n"));
	    goto erro;
	}
	/*复制nandflash的相关规格参数*/
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
        NAND_TRACE(("[%s]argu error\n", __FUNCTION__));
        return;
    }

    /*根据分区名得到分区数据结构*/
    while(0 != strcmp(PTABLE_END_STR, new_ptable->name))
    {
        new_ptable++;
    }

    new_ptable--;

    old_ptable = find_partition_by_name(new_ptable->name);
    if(old_ptable)
    {
        new_ptable->capacity = old_ptable->capacity;
    }

}

/**
 * 作用:擦除所有的yaffs文件系统分区
 *
 * 参数:
 * 无
 *
 *
 * 描述:擦除所有的yaffs文件系统分区,在升级的过程中使用,擦除所有的yaffs文件系统的分区
 */
void bsp_erase_yaffs_partitons(void)
{
    struct ST_PART_TBL * ptable = NULL;
    struct ptentry ptn;
    int ret = NANDC_ERROR;

    ptable_ensure();

    ptable = ptable_get_ram_data();
	/*遍历所有的分区信息*/
    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
    	/*查找YAFFS文件系统的分区*/
        if(ptable->property & DATA_YAFFS)
        {
            /* coverity[buffer_size_warning] */
            strncpy(ptn.name, ptable->name, 16);
            ptn.start = ptable->offset;
            ptn.length = ptable->capacity;
            NAND_TRACE(("earsing %s, start 0x%x, length 0x%x\n", ptn.name, ptn.start, ptn.length));
            ret = flash_erase(&ptn);
            if(ret)
            {
                NAND_TRACE(("[%s] ERROR: erase %s failed, ret = %d\n", __FUNCTION__, ptn.name, ret));
                return;
            }
        }
        ptable++;
    }
}


/**
 * 作用:把字符串转换为数字
 *
 * 参数:
 * @str          ---要转换的字符串
 * @digit        ---得到的数字值
 *
 * 描述:把字符串转换为数字
 */
unsigned int str2ul(char *str, unsigned int *digit)
{
    int ret = NANDC_ERROR;
    unsigned int tmp_digit = 0;
    char *temp = str;

    /*参数不正确*/
    if(!str || !digit)
    {
        NAND_TRACE(("[%s]ERROR: input is NULL.\n", __FUNCTION__));
        goto ERRO;
    }

    /*把string转换为数字*/
    while(*temp != 0)
    {
        if(*temp < '0' || *temp > '9')
        {
            NAND_TRACE(("[%s]ERROR: string input is illegal.\n", __FUNCTION__));
            goto ERRO;
        }
        tmp_digit = tmp_digit * 10 + (*temp - '0');
        temp++;
    }

    *digit = tmp_digit;

    return NANDC_OK;

ERRO:
    return ret;
}

/**
 * 作用:参数检查
 *
 * 参数:
 * @argu          ---字符串参数
 *
 *
 * 描述:检查参数
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
 * 作用:对nand标坏块
 *
 * 参数:
 * @blockid          ---block块的字符串
 *
 *
 * 描述:nand标坏块
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
        NAND_TRACE(("ERROR: string to integer failed, ret = 0x%x.\n", ret));
        goto EXIT;
    }

    /* check block bad. if good, mark bad; else return */
    ret = nand_isbad(id, &bad_flag);
    if(ret)
    {
        NAND_TRACE(("[%s]ERROR: nand check bad failed, ret = 0x%x.\n", __FUNCTION__, ret));
        goto EXIT;
    }

    if(NANDC_GOOD_BLOCK == bad_flag)
    {
        ret = nand_bad(id);
        if(ret)
        {
            NAND_TRACE(("[%s]ERROR: nand mark bad failed, ret = 0x%x.\n", __FUNCTION__, ret));
            goto EXIT;
        }
    }
    else
    {
        NAND_TRACE(("[%s]WARNING: block 0x%x is already bad.\n", __FUNCTION__));
        goto EXIT;
    }

    return;

EXIT:
    return;
}

/**
 * 作用:对nand标强制擦除
 *
 * 参数:
 * @blockid          ---block块的字符串
 *
 *
 * 描述:nand强制擦除功能,当如果有软件坏块时也可以用此函数来强制擦除
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
        NAND_TRACE(("[%s]ERROR: string to integer failed, ret = 0x%x.\n", __FUNCTION__, ret));
        goto ERRO;
    }

    /*直接擦除*/
    ret = nand_erase(id);
    if(ret)
    {
        NAND_TRACE(("[%s]ERROR: nand check bad failed, ret = 0x%x.\n", __FUNCTION__, ret));
        goto ERRO;
    }
    else
    {
        NAND_TRACE(("SUCCESS: erase block %x forcely.\n", id));
    }

    return;

ERRO:
    return;
}

#endif

#ifdef __cplusplus
}
#endif

