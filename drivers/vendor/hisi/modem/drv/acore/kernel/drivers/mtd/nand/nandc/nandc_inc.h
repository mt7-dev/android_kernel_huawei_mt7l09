
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_INC_H_
#define _NANDC_INC_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "nandc_port.h" /* 该文件包含了product_config.h */

#include "nandc_mco.h"
#include "nandc_def.h"
#include "nandc_ctrl.h"

#ifdef __KERNEL__
#include <linux/mtd/mtd.h>
#else
#include "nandc_linux.h"
#endif

extern u32 nandc_trace_mask;

u32     nandc_data_transmit_raw(struct nandc_host *host, u32 datasize, u32 bufoffset, u32 access);
u32     nandc_data_transmit_page(struct nandc_host *host, struct nandc_pagemap *pagemap, u32 times, u32 access);
u32     nandc_native_nand_prob(struct nandc_host* host);
void    nandc_native_host_delete(struct nandc_host* host);
u32     nandc_native_location_by_pagenum(struct nandc_host* host, u32 pagenum);
u32     nandc_native_location_by_address(struct nandc_host* host, FSZ address);
u32     nandc_native_location_by_blocknum(struct nandc_host* host, u32 blknum);
u32     nandc_ctrl_entry(struct nandc_host *host);
u32     nandc_io_bitwidth_set(u32 bitwidth);
u32     nandc_nand_set_address(struct nandc_host *host);
u32     nandc_bbm_real_addr(FSZ addr_virtual, FSZ *addr_real);
u32     nandc_host_set_define(struct nandc_host * host, struct mtd_partition* ptable, u32 nr_parts);
u32     nandc_init_mtd_partition(struct nandc_host* host);
u32     nandc_host_add_nand_chip(struct nandc_host * host,  u32 chipselect, struct nandc_spec *spec);
u32     nandc_host_init_cluster(struct nandc_host *host, struct nandc_reg_desc* reg_tbl, u8 *bitstbl);
u32     nandc_host_set_chipparam(struct nandc_host * host);
u32     nandc_host_init_regfield(struct nandc_host *host, u32 regsize);
u32     nandc_nand_create_host(struct nandc_host  **host);
int     nand_spec_info_resume(struct mtd_info *mtd, struct nand_spec_shared_mem *spec_shmem);

struct nandc_host*  nandc_native_host_create(struct nandc_init_info* init_info);

extern struct nandc_init_info   nandc4_init_info;
extern struct nandc_init_info   nandc6_init_info;
extern struct nand_interface    nandc_nand_funcs;
extern struct nandc_init_info  *nandc_init_seed;
extern int *nandc_block_buffer;

u32 nandc_bbm_real_addr(FSZ addr_virtual, FSZ *addr_real);

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_INC_H_*/


