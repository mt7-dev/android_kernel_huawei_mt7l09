

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/buffer_head.h>
#include <linux/kobject.h>

#include "ctree.h"
#include "disk-io.h"
#include "transaction.h"

/* /sys/fs/btrfs/ entry */
static struct kset *btrfs_kset;

int btrfs_init_sysfs(void)
{
	btrfs_kset = kset_create_and_add("btrfs", NULL, fs_kobj);
	if (!btrfs_kset)
		return -ENOMEM;
	return 0;
}

void btrfs_exit_sysfs(void)
{
	kset_unregister(btrfs_kset);
}

