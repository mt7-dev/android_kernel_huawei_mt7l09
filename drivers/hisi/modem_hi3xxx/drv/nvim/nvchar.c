/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: nvchar.c                                                        */
/*                                                                           */
/* Author: yuyangyang                                                        */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2013-3-12                                                           */
/*                                                                           */
/* Description:   nv read&write user interface                               */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Modification: Create this file                                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/*lint --e{533,830}*/
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

#include "nv_comm.h"



#define NVCHAR_DEVICE_NAME    "nv"
#define NVCHAR_DEVICE_CLASS   "nv_class"
#define NVCHAR_MAX_ID         0xffff
#define IOCTRL_MAGIC           'c'


#define IOCTRL_NVFLUSH        _IOW(IOCTRL_MAGIC, 0, struct nv_data_stru)
#define IOCTRL_NVBACKUP       _IOW(IOCTRL_MAGIC, 1, struct nv_data_stru)
#define IOCTRL_NVREVERT       _IOR(IOCTRL_MAGIC, 2, struct nv_data_stru)
#define IOCTRL_NVSETDEFATULT  _IOW(IOCTRL_MAGIC, 2, struct nv_data_stru)

#define NV_MAX_USER_BUFF_LEN      2048


struct nv_dev
{
    unsigned int nvid;
    struct cdev cdev;
};

struct nv_data_stru
{
    unsigned int nvid;
    unsigned int len;
    unsigned char  data[NV_MAX_USER_BUFF_LEN];
};

#define NV_HEAD_LEN            (sizeof(struct nv_data_stru) - NV_MAX_USER_BUFF_LEN)

static struct nv_dev nv_cdev = {0};
static unsigned int nvchar_major;
static struct class* nv_class;
/*lint -save -e438*/
/*lint -save -e745 -e601 -e49 -e65 -e64 -e533 -e830*/
int nvdev_open(struct inode * inode, struct file * file);
int nvdev_release(struct inode * inode, struct file * file);
ssize_t nvdev_read(struct file *file, char __user *buf, size_t count,
			loff_t *ppos);
ssize_t nvdev_write(struct file *file, const char __user *buf, size_t count,
			loff_t *ppos);

static struct file_operations nv_fops = {
    .owner   = THIS_MODULE,
    .read    = nvdev_read,
    .write   = nvdev_write,
    .open    = nvdev_open,
    .release = nvdev_release,
};

int nvdev_open(struct inode *inode, struct file *file)/*lint !e830*/
{
    struct nv_dev* dev;

    dev = (struct nv_dev*)container_of(inode->i_cdev,struct nv_dev,cdev);
    file->private_data = dev;
    return 0;
}
int nvdev_release(struct inode *inode, struct file *file)
{
    inode = inode;
    file  = file;
    return 0;
}

/*lint -restore +e745 +e601 +e49 +e65 +e64 +e533 +e830*/
/*
data stru:
    ---------------------------------------------
    | NV ID | data length |     data      |
    ---------------------------------------------
*/

/*lint -save --e{529,527}*/
ssize_t nvdev_read(struct file *file,char __user *buf, size_t count,
			loff_t *ppos)
{
    ssize_t ret;
    struct nv_data_stru* kbuf = NULL;


    if((NULL == buf)||(count <= NV_HEAD_LEN)||(count > NV_MAX_USER_BUFF_LEN))/*lint !e737*/
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }

    kbuf = (struct nv_data_stru*)kmalloc(count+1,GFP_KERNEL);
    if(NULL == kbuf)
    {
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    if(copy_from_user(kbuf,buf,count))
    {
        kfree(kbuf);
        return -1;
    }

    /* coverity[tainted_data] */
    ret = (ssize_t)bsp_nvm_read(kbuf->nvid,kbuf->data,kbuf->len);
    if(ret)
    {
        kfree(kbuf);
        return ret;
    }

    ret = (ssize_t)copy_to_user(buf,kbuf,count);
    if(ret)
    {
        kfree(kbuf);
        return ret;
    }

    kfree(kbuf);
    return (ssize_t)count;
}
/*lint -restore*/

/*lint -save --e{529,527}*/
ssize_t nvdev_write(struct file *file,const char __user *buf, size_t count,
			loff_t *ppos)
{
    ssize_t ret;
    struct nv_data_stru* kbuf = NULL;

    if((NULL == buf)||(count <= NV_HEAD_LEN)||(count > NV_MAX_USER_BUFF_LEN))/*lint !e737*/
    {
        printk("invalid parameter!\n");
        return BSP_ERR_NV_INVALID_PARAM;
    }

    kbuf = (struct nv_data_stru*)kmalloc(count+1,GFP_KERNEL);
    if(NULL == kbuf)
    {
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    if(copy_from_user(kbuf,buf,count))
    {
        printk("copy from user fail!\n");
        kfree(kbuf);
        return -1;
    }

    /* coverity[tainted_data] */
    ret = (ssize_t)bsp_nvm_write(kbuf->nvid,kbuf->data,kbuf->len);
    if(ret)
    {
        kfree(kbuf);
        return ret;
    }

    kfree(kbuf);
    return (ssize_t)count;
}
/*lint -restore*/
/*lint -save -e527 -e533*/
void nvchar_dev_setup_cdev(struct nv_dev* dev,struct file_operations* fops)
/*lint -restore*/
{
    int ret;
    unsigned int devno = MKDEV(nvchar_major,0);


    cdev_init(&dev->cdev,fops);
    dev->cdev.owner= THIS_MODULE;
    dev->cdev.ops = fops;

    ret = cdev_add(&dev->cdev,devno,1);
    if(ret)
    {
        printk("add nv fail!\n");
        return ;
    }

    nv_class = class_create(THIS_MODULE,NVCHAR_DEVICE_CLASS);
    if(IS_ERR(nv_class))
    {
        printk("class create failed!\n");
        return ;
    }

    device_create(nv_class, NULL, devno, NULL, NVCHAR_DEVICE_NAME);
}



/*nv device init*/
/*lint -save --e{529}*/
int nvchar_init(void)
{
    int ret;
    dev_t dev = 0;

    /*dynamic dev num use*/
    ret = alloc_chrdev_region(&dev,0,1,NVCHAR_DEVICE_NAME);
    nvchar_major = MAJOR(dev);

    if(ret)
    {
        printk("nvchar failed alloc :%d\n",nvchar_major);
        return ret;
    }
    /*setup dev*/
    nvchar_dev_setup_cdev(&nv_cdev,&nv_fops);

    printk("nvchar init end! major: 0x%x\n",nvchar_major);
    return NV_OK;
}
/*lint -restore*/


#if 0
void nvchar_exit(void)
{
    cdev_del(&(nv_cdev.cdev));
    class_destroy(nv_class);
    unregister_chrdev_region(MKDEV(nvchar_major,0),1);
}
#endif


MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("Balong NV driver for Hisilicon");
MODULE_LICENSE("GPL");

/*lint -restore*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
