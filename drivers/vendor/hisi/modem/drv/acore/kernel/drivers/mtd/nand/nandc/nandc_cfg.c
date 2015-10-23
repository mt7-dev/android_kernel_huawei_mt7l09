


#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"

u32 nandc_trace_mask = (u32)(NANDC_TRACE_ALL | NANDC_TRACE_ERRO | NANDC_TRACE_WARNING);

/*****************************************************************************/

/*****************************************************************************/

#if defined (NANDC_USE_V400)

struct nandc_init_info  *nandc_init_seed = &nandc4_init_info;

#elif defined (NANDC_USE_V600)

struct nandc_init_info  *nandc_init_seed = &nandc6_init_info;

#else

struct nandc_init_info  *nandc_init_seed = NULL;

#endif

int *nandc_block_buffer = (int*)RVDS_TRACE_WRITE_BUFF ;//0xc6000000; /*for RVDS trace write*/

void nandc_set_trace(u32 traceMask)
{
    nandc_trace_mask = traceMask;
}

u32 nandc_get_trace(void)
{
    return nandc_trace_mask;
}

#ifdef __cplusplus
}
#endif


