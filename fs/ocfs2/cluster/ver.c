

#include <linux/module.h>
#include <linux/kernel.h>

#include "ver.h"

#define CLUSTER_BUILD_VERSION "1.5.0"

#define VERSION_STR "OCFS2 Node Manager " CLUSTER_BUILD_VERSION

void cluster_print_version(void)
{
	printk(KERN_INFO "%s\n", VERSION_STR);
}

MODULE_DESCRIPTION(VERSION_STR);

MODULE_VERSION(CLUSTER_BUILD_VERSION);
