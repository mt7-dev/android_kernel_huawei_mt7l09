
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

/*#define MODULE_MAIN          NAND*/
#define SUB_MODULE              NANDC_TRACE_PTABLE

#include "ptable_def.h"
#include "ptable_com.h"
#include "ptable_inc.h"
#include <boot/flash.h>
#include <boot/boot.h>

struct ptentry_ex pt_find;

/******************************************************************************************
 * FUNC NAME:
 * @flash_find_ptn() - external API:
 *
 * PARAMETER:
 * @name   -[input] which partition to be find for fastboot
 *
 * RETURN:
 *    partition info of struct ptentry
 *
 * DESCRIPTION:
 *     find fastboot partition informatin from flash partition table in AXI memory
 *
 * CALL FUNC:
 *****************************************************************************************/
struct ptentry_ex *flash_find_ptnex(const char *name)
{
    struct ST_PART_TBL *part = NULL;

    part = (struct ST_PART_TBL *)SHM_MEM_PTABLE_ADDR;

    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        if((0 == strcmp(name ,  part->name)) && (DATA_VALID == ptable_get_validity(part)))
        {
            pt_find.ptn.flags = 0;
            pt_find.ptn.length = part->capacity;
            pt_find.ptn.start = part->offset;
            pt_find.ptn.property = part->property;
			pt_find.entry = part->entry;
			pt_find.loadaddr = part->loadaddr;
			pt_find.image = part->image;
			pt_find.property = part->property;

            /* coverity[secure_coding] */
            strcpy(pt_find.ptn.name, part->name);

            return &pt_find;
        }

        part++;
    }

    return 0;
}

struct ptentry *flash_find_ptn(const char *name)
{
	struct ptentry_ex * ex = flash_find_ptnex(name);

	if(ex)
		return &ex->ptn;
	else
		return 0;
}

#ifdef __cplusplus
}
#endif




