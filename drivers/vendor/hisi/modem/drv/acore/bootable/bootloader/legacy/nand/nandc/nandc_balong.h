/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: nandc模块提供对外面的接口
*
* Filename:    nandc_balong.h
* Description: nandc模块提供外面的接口函数
*
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
*
*
******************************************************************************/
#ifndef _NANDC_BALONG_H_
#define _NANDC_BALONG_H_

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __FASTBOOT__
#include "nandc_inc.h"
#include <bsp_nandc.h>
/*enum of nand operation type */
enum NAND_FUNC
{
    READ_NAND,              /*使能ECC功能的读多页数据*/
    READ_YAFFS,
    READ_PAGE,				/*使能ECC功能的读一页数据*/
    READ_RAW,               /*不使能ECC功能的裸读Flash数据功能*/
    WRITE_NAND,             /*使能ECC功能的写多页数据*/
    WRITE_YAFFS,
    WRITE_PAGE, 			/*使能ECC功能的写一页数据*/
    WRITE_RAW,              /*不使能ECC功能的裸写数据*/
    ERASE_BLOCK,            /*擦除一块flash*/
    QUARY_BAD,              /*查询此块block是否是坏块*/
    MARK_BAD                /*标坏块功能*/
};

/*操作nandc模块的参数数据结构*/
struct nand_param
{
    u32 number;             /*indicate a number ,may be a flash block number or a flash page number*/
    FSZ addr_flash;         /*flash address to be read from or be writen to */
    u32 addr_data;          /*the ram address contain data that should be writen to flash, or the ram address
                              should store data read from flash */
    u32 addr_oob;           /*the ram address contain oob that should be writen to flash, or the ram address
                              should store oob read from flash */
    u32 size_data;          /*the size of data to be read or writen*/
    u32 size_oob;           /*the size of oob to be read or writen*/
    u32 *skip_length;       /*flash skip length during nand operation*/
};

/*nandc模块操作数据结构,这样设计的作用是对外提供统一的接口,具体的实现通过funcs中的函数指针来实现，当具体的funcs中实现
发生变化时能做到对外提供的接口不变*/
struct nand_bus
{
    struct nand_interface * funcs;          /*函数指针的数据结构*/
    u32 init_flag;                          /*模块初始化标志,主要是为了在测试程序中查看是否已经初始化完成*/
};
/*nand操作的具体实现,用函数指针的好处就是当具体操作实现发生了变化时，可以做到对外提供的接口函数不发生变化*/
struct nand_interface
{
    u32 (*init)(void);
    u32 (*read)(FSZ flashaddr, u32 dstaddr, u32 readsize, u32 oobsize, u32 *skipblks);
    u32 (*get_spec)(struct nand_spec *specs);
	/*此函数是一页page的读写功能*/
    u32 (*read_page_ecc)(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize);
	/*裸读数据,长度不能超过pagesize+sparesize*/
    u32 (*read_page_raw)(u32 pagenum, u32 offset, u32 dst_addr, u32 readsize);

    u32 (*quary_bad)(u32 blkid, u32 * isbad );

    u32 (*write)(FSZ flashaddr, u32 srcaddr, u32 writesize, u32 oobsize, u32 *skiplength);

    u32 (*write_page_ecc)(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize);
    u32 (*write_page_raw)(u32 pagenum, u32 offset, u32 src_addr, u32 writesize);

    u32 (*mark_bad)(u32 blk_id);
    u32 (*erase_by_id)(u32 blk_id);
};


#define NAND_INIT_MAGIC 0x19840815
#define NAND_OK             0
#define NAND_ERROR        1


#if (NANDC_COMPILE_FLAG & NANDC_DEBUG)
#define NAND_TRACE(p)  hiout(p)
#else
#define NAND_TRACE(p)
#endif

#define NAND_IPC_OVERTIME_MS    0xFFFFFFFF


extern struct nandc_host*  nandc_nand_host;

 /**
 * 作用:nandc模块的初始化，包含硬件寄存器的初始化和相关数据结构的初始化，提供在fastboot阶段初始化nandc控制器
 *
 * 参数: none
 *
 * 描述:提供对外初始化控制器的接口
 *
 */
u32 nand_init(void);


 /**
 * 作用:nandc模块提供对外读Flash数据的函数接口
 *
 * 参数:
 * @flash_addr    ---要读的flash地址
 * @dst_data      ---数据读到内存的地址
 * @read_size     ---要读数据的大小
 * @skip_length   ---过滤的长度,当在读的过程中遇到坏块时跳过要读的块，此结构记录跳过的长度
 *
 *
 * 描述:读Flash的数据,读的数据长度可以超过了一页，此接口只读Flash中的数据，没有读OOB中的数据
 *
 */
u32 nand_read(FSZ flash_addr, u32 dst_data, u32 read_size, u32* skip_length);


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
u32 nand_read_oob(FSZ flash_addr, u32 dst_data, u32 data_size, u32 oob_per_page, u32* skip_length);


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
u32 nand_read_raw(u32 src_page, u32 offset_data, u32 dst_data, u32 len_data, u32* skip_length);


/**
 * 作用:nandc模块提供对外擦除Flash块的函数接口
 *
 * 参数:
 * @blockID       ---要擦除的块号
 *
 *
 * 描述:把要擦除的Flash地址转换为块号就行
 */
u32 nand_erase(u32 blockID);

/**
 * 作用:nandc模块提供对外判断此block是否是坏块函数接口
 *
 * 参数:
 * @blockID       ---要检查的块号
 * @flag          ---把检查的标志值存放在此变量中去,0为好坏，1为坏块
 *
 * 描述:检查此block是否是坏块，并把标记值存放在flag标志中
 */
u32 nand_isbad(u32 blockID, u32 *flag);


/**
 * 作用:nandc模块提供对外标记坏块函数接口
 *
 * 参数:
 * @blockID       ---要标记坏块的块号
 *
 * 描述:标记此block是坏块，也就是把此block的第一页的OOB中的前两个字节清零
 */
u32 nand_bad(u32 blockID);


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
u32 nand_write(FSZ flash_addr, u32 src_data, u32 write_size, u32* skip_length);


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
u32 nand_write_oob(FSZ flash_addr, u32 src_data, u32 data_size, u32 oob_per_page, u32* skip_length);


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
u32 nand_write_raw(u32 dst_page, u32 offset_data, u32 src_data, u32 len_data, u32* skip_length);


/**
 * 作用:nandc模块提供对外得到nandflash相关规格参数的函数接口
 *
 * 参数:
 * @spec      ---把nandflash中的相关参数填写在此指针变量中去
 *
 *
 * 描述:得到nandflash的规格参数
 */
u32 nand_get_spec(struct nand_spec *spec);

/**
 * 作用:打印nandflash相关规格参数的函数接口
 *
 * 输入参数:无
 *
 *
 * 描述:打印nand->spec 参数值
 */
void bsp_show_nand_info(void);
/**
 * 作用:nandc模块提供对外判断nand控制器模块是否初始化的功能
 *
 * 参数:
 * 无
 *
 *
 * 描述:如果nandc模块已经初始化则返回1，没有初始化则返回0
 */
u32 nand_isinit(void);



/**
 * 作用:nandc模块提供对外清除nandc控制器数据结构的函数
 *
 * 参数:
 * 无
 *
 *
 * 描述:清除nandc控制器数据结构的内存
 */
void nand_init_clear(void);

/**
 * 作用:nandc模块提供对外扫描坏块的函数接口
 *
 * 参数:
 * @start         ---flash开始的地址
 * @length        ---扫描的长度
 * @if_erase      ---如果检测是好块根据此参数来决定是否擦除此块
 * 描述:扫描flash是否是坏块，根据参数来决定是否擦除
 */
u32 nand_scan_bad(FSZ start, FSZ length, u32 if_erase);

#endif
#ifdef __cplusplus
}
#endif

#endif



