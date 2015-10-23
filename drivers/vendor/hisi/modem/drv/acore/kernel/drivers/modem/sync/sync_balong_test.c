#include <drv_sync.h>
#include <bsp_om.h>
void sync_test01(void)
{
	s32 ret = 0;
	ret = BSP_SYNC_Give(SYNC_MODULE_TEST01);
	if(ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SYNC,"sync_test01 failed\n");
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SYNC,"sync_test01 pass\n");
	}
	return;
}
void sync_test02(void)
{
	s32 ret = 0;
	u32 state = 0;
	ret = BSP_SYNC_Lock(SYNC_MODULE_TEST02, &state,1000);
	if(ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SYNC,"sync_test02 BSP_SYNC_Lock failed\n");
	}
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SYNC,"sync_test02 state = %d\n",state);
	if(state == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SYNC,"sync_test02 state ==1\n");
		return;
	}
	else
	{
		ret = BSP_SYNC_UnLock(SYNC_MODULE_TEST02, state);
		if(ret)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SYNC,"sync_test02 BSP_SYNC_Lock failed\n");
		}
	}
	return;
}
