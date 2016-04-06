

#include <linux/module.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include "ver.h"

#define OCFS2_BUILD_VERSION "1.5.0"

#define VERSION_STR "OCFS2 " OCFS2_BUILD_VERSION

void ocfs2_print_version(void)
{
	printk(KERN_INFO "%s\n", VERSION_STR);
}

MODULE_DESCRIPTION(VERSION_STR);

MODULE_VERSION(OCFS2_BUILD_VERSION);
