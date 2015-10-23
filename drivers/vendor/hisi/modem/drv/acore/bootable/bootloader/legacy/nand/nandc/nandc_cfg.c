


#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"
/*控制NANDC_TRACE打印信息的级别，默认情况下面NANDC_TRACE是打印error和warning信息*/
u32 nandc_trace_mask = (u32)(NANDC_TRACE_ALL | NANDC_TRACE_ERRO | NANDC_TRACE_WARNING);


/*控制nandc控制器初始化的配置信息,通过宏来判断是V400还是V600的nandc控制器*/
#if defined (NANDC_USE_V400)

struct nandc_init_info  *nandc_init_seed = &nandc4_init_info;

#elif defined (NANDC_USE_V600)

struct nandc_init_info  *nandc_init_seed = &nandc6_init_info;

#else

struct nandc_init_info  *nandc_init_seed = NULL;

#endif

/*设置nandc模块的打印级别*/
/*使能nandc模块的normal级别的打印*/
void nandc_enable_trace(void)
{
    nandc_trace_mask = (nandc_trace_mask | NANDC_TRACE_NORMAL);
}

void nandc_disable_trace(void)
{
    nandc_trace_mask = (nandc_trace_mask &(~NANDC_TRACE_NORMAL));
}

#ifdef __cplusplus
}
#endif


