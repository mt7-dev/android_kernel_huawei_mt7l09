
#include <product_config.h>
#include <bsp_reset.h>


#ifndef CONFIG_CSHELL   /* ´ò×® */

int cshell_mode_reset_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int usrdata)
{
	eparam = eparam;
	usrdata = usrdata;
	return 0;
}

#endif
