#include <vxWorks.h>
#include <mmuLib.h>
#include <errno.h>
#include <cacheLib.h>
#include <private/classLibP.h>
#include <private/objLibP.h>

#include <vmLib.h>
#include <cacheLib.h>
#include <private/memPartLibP.h>
#include <private/vmLibP.h>
#include <stdlib.h>
#include "dmem.h"

extern MMU_LIB_FUNCS	mmuLibFuncs;		  /* initialized by mmuLib.c */

#define MMU_ATTR_NORMAL_NONCACHEABLE_MSK (MMU_ATTR_SPL_MSK | MMU_ATTR_CACHE_MSK) /*mmuArmArch6PalLib.h*/
#define MMU_ATTR_NORMAL_NONCACHEABLE     (MMU_ATTR_SPL_0 | MMU_ATTR_CACHE_OFF) /*mmuArmArch6PalLib.h*/

void* dmem_malloc(unsigned int bytes)
{
	return KMEM_ALIGNED_ALLOC(ROUND_UP(bytes, MMU_PAGE_SIZE_GET()), MMU_PAGE_SIZE_GET());
}
void dmem_free(void* p)
{
	free(p);
}
void* dmem_cache_dma_malloc(unsigned int bytes)
{
	STATUS state =0;
	unsigned int alloc_size = ROUND_UP(bytes, MMU_PAGE_SIZE_GET());
	void* p = KMEM_ALIGNED_ALLOC(alloc_size, MMU_PAGE_SIZE_GET());
	if (p)
	{
		state = VM_STATE_SET(NULL, p, alloc_size
			, MMU_ATTR_VALID_MSK | MMU_ATTR_SPL_MSK | MMU_ATTR_PROT_MSK | MMU_ATTR_NORMAL_NONCACHEABLE_MSK 
			, MMU_ATTR_VALID | MMU_ATTR_NO_BLOCK | MMU_ATTR_SUP_RW | MMU_ATTR_NORMAL_NONCACHEABLE);
		if (state != OK )
		{
			free(p);
			return NULL;
		}
	}
	return p;
}
STATUS dmem_cache_dma_free(void* p)
{
	STATUS state;
	state = VM_STATE_SET(NULL, p, MEM_BLOCK_SIZE(p)
		, MMU_ATTR_CACHE_MSK | MMU_ATTR_PROT_MSK
		, MMU_ATTR_CACHE_DEFAULT | MMU_ATTR_SUP_RW);
	free(p);
	return state;
}
STATUS dmem_lock(void* p, unsigned int size)
{
	STATUS state;
	if (ROUND_UP(p, MMU_PAGE_SIZE_GET()) != p || ROUND_UP(size, MMU_PAGE_SIZE_GET()) != size)
	{
		return ERROR;
	}
	vmEnable(FALSE);
	state = VM_STATE_SET(NULL, p, size
		, MMU_ATTR_PROT_MSK
		, MMU_ATTR_PROT_SUP_READ);
	vmEnable(TRUE);
	return state;
}

STATUS dmem_unlock(void* p, UINT size)
{
	STATUS state;
	if (ROUND_UP(p, MMU_PAGE_SIZE_GET()) != p || ROUND_UP(size, MMU_PAGE_SIZE_GET()) != size)
	{
		return ERROR;
	}
	vmEnable(FALSE);
	state = VM_STATE_SET(NULL, p, size
		, MMU_ATTR_PROT_MSK
		, MMU_ATTR_SUP_RW);
	vmEnable(TRUE);
	return state;
}
