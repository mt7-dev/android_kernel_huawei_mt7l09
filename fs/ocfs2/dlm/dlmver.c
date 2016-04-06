

#include <linux/module.h>
#include <linux/kernel.h>

#include "dlmver.h"

#define DLM_BUILD_VERSION "1.5.0"

#define VERSION_STR "OCFS2 DLM " DLM_BUILD_VERSION

void dlm_print_version(void)
{
	printk(KERN_INFO "%s\n", VERSION_STR);
}

MODULE_DESCRIPTION(VERSION_STR);

MODULE_VERSION(DLM_BUILD_VERSION);
