/******************************************************************************
  File Name       : balong_early_fs.c
  Description     : mount yaffs filesystem in kernel,when init is not run
  History         :
******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/fs.h>
#include "ptable_com.h"
#include <linux/syscalls.h>

#define MDEV_FS(m_dev_id, dev_index)  ((m_dev_id << 8) | dev_index)

typedef struct mount_early {
    unsigned int img_type;
    char mount_point[32];
}mount_early;

mount_early mount_list[] = {{IMAGE_OM,        "/modem_log"},
#if (PTABLE_YAFFS_ONLINE_LEN != 0)
                            {IMAGE_ONLINE,    "/online/"},
#endif
                            {IMAGE_NVIMG,     "/mnvm2:0/"}
                            };
/*****************************************************************************
 函 数 名  : find_early_partition
 功能描述  : read partition info from AXI memory,获取需要挂载的分区
 输入参数  : 无
 输出参数  :
 返 回 值  : ST_PART_TBL
 调用函数  :
 被调函数  :
*****************************************************************************/
static struct ST_PART_TBL *  find_early_partition(unsigned int img_type)
{
    struct ST_PART_TBL *part = NULL;

    part = (struct ST_PART_TBL *)(SHM_MEM_PTABLE_ADDR + \
             PTABLE_HEAD_SIZE);

    /*poll search partition talbe*/
    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        /*find and load excutable image first*/
        if(img_type == part->image )
            break;
        part++;
    }

    if(0 == strcmp(PTABLE_END_STR, part->name))
        return NULL;

    return  part;
}

/*****************************************************************************
 函 数 名  : mount_early_partition
 功能描述  : 挂载列表中的分区
 输入参数  : mount_item:挂载列表
 输出参数  :
 返 回 值  : 成功/失败
 调用函数  :
 被调函数  :
*****************************************************************************/
static int mount_early_partition(mount_early *mount_item)
{
    struct mtd_info *mtd;
    int rt = 0;
    char mount_name[32] ={0};
    struct ST_PART_TBL *part = NULL;

    part = find_early_partition(mount_item->img_type);
    if(part != NULL){
        mtd = get_mtd_device_nm(part->name);
    	if (IS_ERR(mtd)) {
        	printk("get_mtd_device_nm error.\n");
        	return PTR_ERR(mtd);
        }

        snprintf(mount_name, sizeof(mount_name) - 1, "/dev/block/mtdblock%d", mtd->index);
        printk(KERN_DEBUG "going to mount %s  mount point %s\n", mount_name, mount_item->mount_point);

        if((rt = sys_mkdir(mount_item->mount_point, S_IRUSR | S_IRGRP)) < 0)
        {
            printk(KERN_ERR "create dir failed %s ret 0x%x\n", mount_item->mount_point, rt);
            return rt ;
        }

        rt = sys_mknod(mount_name, S_IFBLK|S_IRWXU|S_IRWXG|S_IRWXO, MDEV_FS(31, mtd->index));
        if(rt < 0)
        {
            printk(KERN_ERR "mknod failed %s ret 0x%x\n", mount_name, rt);
            return rt ;
        }

        rt = sys_mount(mount_name, mount_item->mount_point, "yaffs2", 0, NULL);
        if(rt < 0)
        {
            printk(KERN_ERR "mount failed %s  %s ret 0x%x 0x%x\n", mount_name, \
                mount_item->mount_point, rt, MKDEV(31,mtd->index));
            return rt ;
        }

        return 0;
    }else{
        printk(KERN_ERR "no find nv dload partition!!!\n");
        return 1 ;
    }
}

#ifdef CONFIG_BALOMG_EARLY_FS_TEST
/*****************************************************************************
 函 数 名  : mount_early_rw_test
 功能描述  : 用于测试
 输入参数  :
 输出参数  :
 返 回 值  : 成功/失败
 调用函数  :
 被调函数  :
*****************************************************************************/
int mount_early_rw_test()
{
    int rt = 0, cnt = 0;
    unsigned int fd = 0;

    char pstream[50] = "first file please input something !!!~";
    int pos = 0;

    /*just for test*/
    fd = sys_open("/modem_log/test.txt", O_RDWR | O_CREAT, 0666);
    if(fd < 0)
    {
       printk(KERN_ERR "sys_open %s/test.txt fail \n", "/modem_log");
       return;
    }

    sys_lseek(fd, 0, SEEK_END);

    rt = sys_write(fd, pstream, sizeof(pstream));
    if(rt < 0)
    {
       printk(KERN_ERR "sys_write  fail\n");
       return rt;
    }

    pos = sys_lseek(fd, 0, SEEK_CUR);

    memset(pstream, 0, sizeof(pos));

    sys_lseek(fd, pos + 10 - sizeof(pstream), SEEK_SET);

    rt = sys_read(fd, pstream, sizeof(pstream) - 10);
    if(rt < 0)
    {
       printk(KERN_ERR "sys_read  fail\n");
       return rt;
    }

    printk(KERN_ERR "sys_read /modem_log/test.txt %s \n", pstream);

    return 0;
}
#endif

static int __init mount_early_init( void )
{
    int rt = 0, cnt = 0;

    printk(KERN_ERR "start mount early partition\n");

    rt = sys_mkdir("/dev/block",S_IRWXU|S_IRWXG|S_IRWXO);
    if(rt < 0)
    {
        printk(KERN_ERR "sys_mkdir /dev/block fail\n");
        return rt;
    }

    for(cnt = 0; cnt < sizeof(mount_list)/sizeof(mount_early); cnt++)
    {
        if(0 != (rt = mount_early_partition(&mount_list[cnt])))
        {
            printk(KERN_ERR "mount_early_partition failed!!! 0x%x\n", rt);
            return rt;
        }
    }

#ifdef CONFIG_BALOMG_EARLY_FS_TEST
    return mount_early_rw_test();
#endif

    return 0;
}

static void __exit  mount_early_exit( void )
{
    return;
}

EXPORT_SYMBOL(mount_early_init);
EXPORT_SYMBOL(mount_early_exit);

module_init(mount_early_init);
module_exit(mount_early_exit);

MODULE_LICENSE("GPL");

