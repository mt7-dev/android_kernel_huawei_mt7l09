
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _PTABLE_API_H_
#define _PTABLE_API_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

extern u32 ptable_replace_ram_table(u32 data_tbl, u32 size);

extern u32 ptable_replace_flash_table(void);

extern u32 ptable_read_by_type(FSZ flash_addr, u32 data_buf, u32 image_type, u32 length, u32 *skip_len);

extern u32 ptable_write_by_type(FSZ flash_addr ,u32 data_buf, u32 image_type,  u32 length, u32 *skip_len);

extern u32 ptable_parse_mtd_partitions(struct mtd_partition** mtd_parts, u32 *nr_parts);    
       
extern u32 ptable_dload_write(u32 data_buf, u32 name_idx, u32 this_offset , u32 length);

extern u32 ptable_get_table_changed(void);

extern void ptable_set_table_changed(u32 is_changed);

extern u32 ptable_dload_read(u32 data_buf, u32 name_idx, u32 this_offset , u32 length);

extern void ptable_dload_init(void);

#ifdef __cplusplus
}
#endif

#endif  /*_PTABLE_API_H_*/



