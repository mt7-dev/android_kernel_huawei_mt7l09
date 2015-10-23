/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  adp_lowpower_mntn.c
*
*   作    者 :  xujingcui
*
*   描    述 : 本文件用于接口适配
*
*   修改记录 :  2013年6月10日  v1.00 xujingcui创建
*************************************************************************/
#include <bsp_lowpower_mntn.h>

struct lock_info hi6930_lockinfo;

void update_awake_time_stamp(void)
{
    return;
}
void check_awake_time_limit(void)
{
    return;
}
void bsp_save_lowpower_status(void)
{
	return;
}
void bsp_dump_lowpower_status(void)
{
	return;
}
void bsp_show_lowpower_status(void)
{
	return;
}
void bsp_dump_bus_error_status(void)
{
	return;
}

