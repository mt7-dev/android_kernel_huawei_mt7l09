
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _PTABLE_INC_H_
#define _PTABLE_INC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define NFCDBGLVL(LVL)          (SUB_MODULE | NANDC_TRACE_##LVL)

#include "nandc_balong.h"
#include "ptable_def.h"
#include "ptable_com.h"
#include "ptable_api.h"


extern char ptable_name[];

extern u32 ptable_ensure(void);

extern struct ST_PTABLE_HEAD * ptable_get_ram_head(void);

extern struct ST_PART_TBL * ptable_get_ram_data(void);

extern struct ST_PART_TBL * ptable_find_by_type(u32 part_type, struct ST_PART_TBL * ptable);

extern struct ST_PART_TBL * ptable_find_older_part(u32 type_idx, struct ST_PART_TBL * ptable, u32  inc_count, u32 set_valid);

extern u32 ptable_get_validity(struct ST_PART_TBL * ptable);

extern void ptable_set_validity(struct ST_PART_TBL * ptable, u32 set_valid);

#ifdef __KERNEL__

extern struct ST_PART_TBL * ptable_find_theother_by_type(u32 type_idx, struct ST_PART_TBL * part);

extern u32 ptable_get_range(u32 part_type, u32* start , u32* end);

extern void ptable_set_integrality(u32 integrality);

extern struct ST_PART_TBL * ptable_find_by_name(char* name_to_find, struct ST_PART_TBL * ptable);


#endif

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/



