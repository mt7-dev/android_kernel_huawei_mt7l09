
#include <product_config.h>
#include "../cshell/cshell.h"


#ifndef CONFIG_CSHELL   /* ´ò×® */

void cshell_send_data(char* data, int length)
{
	data   = data;
	length = length;
}

void cshell_register_hook(cprint_hook hook)
{
	hook = hook;
}

#endif
