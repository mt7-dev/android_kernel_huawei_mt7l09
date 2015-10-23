
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_CTRL_H_
#define _NANDC_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <osl_sem.h>
#include "nandc_cfg.h"

struct nandc_bit_reg
{
    u32 bits_func ;
    u32 reg_offset ;  /* offset relative to nand controller registers' base address.*/
    struct nandc_bitset*  bitset;
};


/*
*  这里定义控制器所有的寄存器操作命令合集，这些命令在主干流程里面提取并执行，
* 合集中的每个单位就是一个寄存器的bit位功能描述，其中的具体内容在驱动初始化之前是不空白的，
* 在初始化程序中会有一系列查表分析动作，通过控制器和控制对象传入的属性，对里面的必要项目
* 进行填充，完成这一结构数据的差异化配置。
*
*/
struct nandc_bit_cluster
{
    struct nandc_bit_reg cmd;
    struct nandc_bit_reg cmd2;
    struct nandc_bit_reg cmd3;              /* read status command */

    struct nandc_bit_reg bus_width;
    struct nandc_bit_reg addr_high;
    struct nandc_bit_reg addr_low;

    struct nandc_bit_reg addr_cycle;
    struct nandc_bit_reg chip_select;
    struct nandc_bit_reg operate;           /*nandc3_op_read_id*/

    struct nandc_bit_reg op_stauts;         /*nandc_status_op_done*/
    struct nandc_bit_reg int_stauts;        /*nandc_status_int_done*/
    struct nandc_bit_reg data_num;          /*data write to one page ,size in byte*/

    struct nandc_bit_reg ecc_type;          /*nandc3_ecc_none, nandc3_ecc_4smb*/
    struct nandc_bit_reg ecc_select;        /*nandc4_ecc_oob , nandc4_ecc_all*/
#ifdef NANDC_USE_V400
    struct nandc_bit_reg int_enable;
#endif
    struct nandc_bit_reg op_mode;           /*nandc_op_normal or nandc_op_boot*/
    struct nandc_bit_reg page_size;
    struct nandc_bit_reg int_clr;

    struct nandc_bit_reg segment;           /*nandc4_segment_first for nandc4_segment_last*/
    struct nandc_bit_reg ecc_result;
    struct nandc_bit_reg nf_status;         /*nand chip 8 bit status register*/

	struct nandc_bit_reg async_interface_type;
	struct nandc_bit_reg randomizer;
	struct nandc_bit_reg read_id_en;

	struct nandc_bit_reg rw_reg_en;
	struct nandc_bit_reg ecc_class;
	struct nandc_bit_reg randomizer_en;

	struct nandc_bit_reg cs0_done;
	struct nandc_bit_reg ahb_op_err;
	struct nandc_bit_reg wr_lock_err;

	struct nandc_bit_reg dma_done;
	struct nandc_bit_reg dma_err;
	struct nandc_bit_reg dma_start;

	struct nandc_bit_reg dma_rw_enable;
	struct nandc_bit_reg burst_enable;
	struct nandc_bit_reg dma_addr_num;

	struct nandc_bit_reg dma_cs;
	struct nandc_bit_reg dma_wr;
	struct nandc_bit_reg dma_rd_oob;

	struct nandc_bit_reg base_addr_data;
	struct nandc_bit_reg base_addr_oob;
	struct nandc_bit_reg dma_len;

	struct nandc_bit_reg errnum0buf0;
	struct nandc_bit_reg rb_status;

	struct nandc_bit_reg base_addr_d1;
	struct nandc_bit_reg base_addr_d2;
	struct nandc_bit_reg base_addr_d3;
	struct nandc_bit_reg oob_len_sel;

	struct nandc_bit_reg int_op_done;
	struct nandc_bit_reg int_cs_done;
	struct nandc_bit_reg int_err_invalid;
	struct nandc_bit_reg int_err_valid;
	struct nandc_bit_reg int_ahb_op;
	struct nandc_bit_reg int_wr_lock;
	struct nandc_bit_reg int_dma_done;
	struct nandc_bit_reg int_dma_err;

	struct nandc_bit_reg dma_ctrl;

    struct nandc_bit_reg endtable;          /*don't add after this*/

};

struct nandc_spec
{
    char *name;          /* get from nand_flash_ids[] through nandc_native_get_spec*/
    u32 pagesize;        /* get from id[] data through nandc_native_get_spec*/
    u32 pagesum;         /* page total number of the nand flash chip.*/
    u32 offinpage;       /* pagesize - 1 */
    u32 pagealign;       /* ~(pagesize - 1) */
    u32 pageshift;       /* pagesize >> pageshift = 1*/
    u32 pagemask;        /* address & pagemask = page_no.*/
    u32 sparesize;       /* get from id[] data through nandc_native_get_spec*/
    u32 blocksize;       /* get from id[] data through nandc_native_get_spec*/
    u32 blockpages;      /* page number in one block*/
    u32 blockmask;       /* address & blockmask = block_no.*/
    u32 offinblock;      /* blocksize - 1 */
    u32 blockalign;      /* ~(blocksize - 1) */
    u32 blockshift;      /* blocksize >> blockshift = 1*/
    FSZ chipsize;        /* get from nand_flash_ids[] through nandc_native_get_spec*/
    u32 chipshift;       /* chipsize >> chipshift = 1*/
    u32 buswidth;        /* get from id[] data through nandc_native_get_spec*/
    u32 addrcycle;
    u32 options;
};


struct nandc_define
{
    struct mtd_partition*  partition;
    struct usr_feature*    feature;
    u32 nbparts;
};

struct nandc_nand
{
    struct nandc_spec    spec;
    struct nandc_pagemap *datamap;
    u32 ecctype;            /*not used yet, ecc defined in*/
    u32 pageset;
    u32 busset;
    u32 chipnum;
};

struct nandc_distrib
{
    u32 addr;
    u32 lens;
};

struct nandc_pagemap    /*data map in one page*/
{
    struct nandc_distrib  data;
    struct nandc_distrib  oob;
};

struct nandc_dmap    /*data map in one page*/
{
    u32    bad2k;     /* bad block flag location for 2k page*/
    struct nandc_pagemap pmap2k[1];
    u32    bad4k;     /* bad block flag location for 4k page*/
    struct nandc_pagemap pmap4k[2];
    u32    bad8k;     /* bad block flag location for 8k page*/
    struct nandc_pagemap pmap8k[4];
};


/*
*说明：此机构体描述控制器的一项私有化特性：不同控制器不同ecc方式的buffer数据排列都有不同
*      ，同时也体现了两者之间的关联性.
*/
struct nandc_dmap_lookup
{
    struct nandc_dmap   *datamap ;      /*指向下面ecctype对应的buffer数据分配搜索列表*/
    u32 ecctype;                        /*上面搜索列表里面对应的ecctype*/
};

struct nandc_bit_cmd
{
    u8  op_read_id;
    u8  op_read_start;
    u8  op_read_continue;
    u8  op_read_end;

    u8  op_write_start;
    u8  op_write_continue;
    u8  op_write_end;
    u8  op_erase;

    u8  op_segment_start;
    u8  op_segment_end;
    u8  op_reset;
    u8  ecc_err_none;

    u8  ecc_err_valid;
    u8  ecc_err_invalid;
    u16 intmask_all;

    u16 intdis_all;
    u16 inten_all;
    u16 intclr_all;

    u16 ecc_oob;
    u16 ecc_all;

	u16 dma_read;
	u16 dma_read_oob_only;
	u16 dma_write;

    u16 ecc_none;
    u16 reserved1;
};

struct nandc_reg_list
{
    u32 address;
    u32 value;
    struct nandc_reg_list *p_next;
};

struct nandc_config_snapshot
{
    struct nandc_reg_list* listreg;
    u32 totalsize;
    u32 onesize;
    u32 eccselect;
};

struct nandc_snapshot
{
    struct nandc_config_snapshot  *config_ecc;
    struct nandc_config_snapshot  *config_raw;
    struct nandc_config_snapshot  *config_oob;
};

/*
* 说明: 驱动模块的主控实体描述符，包含所有当前控制对象信息和为此控制对象所定义的控制器信息。
*
*/
struct nandc_host
{
    struct nandc_nand               *nandchip;              /* pointer to current select nandc_nand */
    struct nandc_define             *usrdefine;             /* 用户对象定义，分区表等*/
    struct mtd_partition            *curpart;               /* 当前操作指向分区表*/
    struct nandc_bit_cluster        *normal;                /* pointer to nandc_bit_cluster */
    struct nand_buffers             *buffers;               /* 驱动临时buffer大小，用于page read*/
    struct nandc_ctrl_func          *ctrlfunc;              /* 控制器私有初始化流程指针*/
    struct nandc_ecc_info           *eccinfo;               /* 关联配置搜索表*/
    struct nandc_dmap_lookup        *dmaplookup;            /* the data map(page + oob)of the controller buffer, nandc_dmap*/
    struct nandc_bit_cmd            bitcmd;                 /* 控制器寄存器位命令配置表*/
    struct nandc_snapshot           read_snapshot;          /* 读系列操作寄存器快照*/
    struct nandc_snapshot           write_snapshot;         /* 写系列操作寄存器快照*/
    u32 badoffset;

    u32 command;
    u32 options;        /* 决定访问nand使用DMA/normal，ecc0/非ecc0 */
	u32 bbt_options;    /* 决定坏块管理使用page0和page1，还是last page */
    u32 latched;
    
    u32 addr_cycle;
    u32 addr_physics[2];
    FSZ addr_real;
    FSZ addr_logic;
    u32 length;                         /*for no ecc mode*/
    u32 chipselect;
    u32 chipmax;
    u32 probwidth;
    
    u32 addr_to_sel_oob_len;            /* 8bit ecc 模式下，oob length需要设置为32Byte，
                                            nand地址低于此值时，oob_len_sel需要设置成0；其余情况，需要设置成1 */
    u8 flash_id[NANDC_READID_SIZE];	
	u8* nand_4bitecc_table;
    
    u8* databuf;                  /*if use DMA tramsmit ,this buffer must be coherent*/
    u8* oobbuf;
    void __iomem * regbase;       /* */
    void __iomem * bufbase;       /* */
    u32 regsize;
    u32 bufsize;
    /*在使能ECC的模式下时，如果发现了有ECC的不可纠错时也要把数据读出来的标志*/
    u32 force_readdata;
    /*如果此Flash支持ONFI的功能，那么在ONFI检测的过程中把ECCTYPE保存便于后面恢复*/
    u32 ecctype_onfi;

#ifdef WIN32
    struct reg_debuger *debuger;
#endif

#ifdef __KERNEL__
    struct nand_chip * chip;
    struct mtd_info *   mtd;
    struct clk  *clk; 
    
#ifdef CONFIG_PM  
    u32 *sleep_buffer;  
#endif  

#ifdef FEATURE_NANDC_DMA_USE_INTERRUPT  
    osl_sem_id sem;
	u32 irq_id;
#endif  

    u32 column;
    u32 offset;
    dma_addr_t  dma_addr;
    dma_addr_t  dma_oob_addr;
    /*if nand page size large than NANDC_BUFSIZE_BASIC ,use temp buffer here*/
    u32 IsUseBuffer;
#endif
};

struct nandc_clist
{
    struct nandc_clist  *p_next;
    struct nandc_nand   *chip;
};


struct nandc_bitset
{
    u32 offset;       /* offset in one nand controller register.*/
    u32 length;       /* bit acount from offset.*/
};

 struct nandc_reg_cont
{
    u32 func;                        /* nandc_bits_func */
    struct nandc_bitset   set;       /* nandc_bit_set */
    u16 desc[nandc_reg_desc_max];
};

/*
* 说明：此结构体完整的描述了一个32位寄存器相关定义，包括寄存器偏移，寄存器默认值，
*        按位偏移和位长度分区的功能信息，及每个功能区的可填入信息。
*/
struct nandc_reg_desc
{
    u32 addr_offset;         /* offset relative to nand controller register's base address.*/
    u32 defaultval;
    struct nandc_reg_cont*  content;
};


/*
* 说明：这个对控制器规格参数的描述，包括寄存器功能，控制器名，内部buffer信息，最大支持芯片数等。
*/
struct nandc_ctrl_desc
{
    struct nandc_reg_desc* regdesc;     /* register description */
    char* ctrlname;                     /* NANDC2_NAME,NANDC3_NAME,NANDC4_NAME */
    u32 regbase;                        /* nand controller register base address.*/
    u32 regsize;                        /* nand controller registers' size in bytes(acount * sizeof(int)).*/
    u32 buffbase;                       /* nand controller buffer's base address */
    u32 buffsize;                       /* nand controller buffer's size in bytes.*/
    u32 maxchip;                        /* acount of nand flash chip.*/
};


/*
* 说明：这个是对控制器私有化初始化流程的配置描述，控制器私有的初始化流程（如GPIO解锁，时序配置等放在这里）
*/
struct nandc_ctrl_func
{
    u32 (*init_cmd)(struct nandc_bit_cmd  *  ctrlcmd,  struct nandc_host *host);
};


/*
* 说明：这个结构定义是对一个控制器的整体描述，一个结构数据定义描述了一款控制器的所有私有化差异
*/
struct nandc_init_info        /* nandc2_init_info, nandc3_init_info, nandc4_init_info, nandc6_init_info*/
{
    struct nandc_ctrl_desc      *ctrlspec;          /*控制器私有配置描述表*/
    struct nandc_ctrl_func      *ctrlfunc;          /*控制器私有功能函数表*/
    struct nandc_ecc_info       *eccinfo;           /*控制器对象关联配置表*/
    u8                          *bitstbl;           /*控制器流程命令配置表*/
    struct nandc_dmap_lookup    *dmaplookup;        /*控制器私有内部buffer分布表*/
    struct usr_feature          *usrdefine;         /*扩展定义接口*/
};

struct data_swap
{
    u32 srcoffset;
    u32 dstoffset;
    u32 length;
};

struct usr_feature
{
    struct data_swap exchange[NANDC_MAX_PAGE_SWAP];
    void (*cb) (struct nandc_host * host,  struct usr_feature *feature);
};

/*
* 说明：这个是控制器和控制对象的关联表项定义，填充这个表以后，初始化程序会根据这表来搜索当前
*       控制器和控制对象所需要的控制器buffer数据分布参数。
*/
struct nandc_ecc_info
{
    u32 pagesize;                                   /*nand器件page大小*/
    u32 sparesize;                                  /*nand器件的spare区大小*/
    u32 ecctype;                                    /*上面两项参数对应的本控制器最佳ecc寄存器配置*/
    struct nand_ecclayout *layout;                  /*上面三项参数对应的本控制器buffer数据分布配置*/
};

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_CTRL_H_*/
