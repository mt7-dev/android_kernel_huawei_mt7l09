/*************************************************************************
*   版权所有(C) 2008-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_kdf_ioctrl.c
*
*   作    者 :  wangxiandong
*
*   描    述 :  使用unlocked_ioctl导出kdf驱动控制功能
*
*   修改记录 :  2013年06月19日  v1.00  wangxiandong 创建
*************************************************************************/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <osl_types.h>
#include "kdf.h"
#include "balong_kdf_ioctrl.h"

/*lint --e{21,774,831} */
//extern void kdf_reg_init(void);
extern int kdf_init(void);
extern int kdf_hash_make(void * sha_s_addr, u32 sha_s_len);
int kdfdev_open(struct inode * inode, struct file * fp);
int kdfdev_release(struct inode * inode, struct file * fp);
long kdfdev_unlocked_ioctl(struct file * fp, unsigned int cmd, unsigned long arg);

static const struct file_operations kdf_fops =
{
	.owner = THIS_MODULE,
	.open  = kdfdev_open,
	.unlocked_ioctl = kdfdev_unlocked_ioctl,
	.release = kdfdev_release,
};
struct cdev chardev;
static int kdfdev_major = KDFDEV_MAJOR;
struct kdf_dev_mem * kdf_devp = NULL;
module_param(kdfdev_major, int, S_IRUGO);

int kdfdev_open(struct inode * inode, struct file * fp)
{
	unsigned int dev_num = iminor(inode);
	if(dev_num >= KDFDEV_NR_DEVS)
	{
		return -ENODEV;
	}
	fp->private_data = kdf_devp;

	return 0;
}

int kdfdev_release(struct inode * inode, struct file * fp)
{
	return 0;
}
/*lint -save -e26*/
/*lint --e{718,746,737,731}*/
long kdfdev_unlocked_ioctl(struct file * fp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int ret = 0;
	u32 regval1 = 0;
	u32 regval2 = 0;
	u32 data_size = 0;

	if(_IOC_TYPE(cmd) != KDFDEV_IOC_MAGIC)
	{
		return -EINVAL;
	}
	if(_IOC_NR(cmd) > KDFDEV_NR_DEVS)
	{
		return -EINVAL;
	}
	if(_IOC_DIR(cmd) & _IOC_READ)
	{
		if(0 != access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)))
			return -EFAULT;
	}
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
	{
		if(0 != access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)))
			return -EFAULT;
	}

	if(KDF_IOC_GETSTATUS == cmd)
	{
		regval1 = readl(CIPHER_REGBASE_ADDR + CIPHER_CHNBDQ_RWPTR(KDF_CHN_NUM));/*lint !e666*/
		regval2 = readl(CIPHER_REGBASE_ADDR + CIPHER_CHN_ENABLE(KDF_CHN_NUM));/*lint !e666*/
		ret = ((regval1 & 0x3FF) == ((regval1 & 0x3FF0000) >> 16)) && (0 == regval2 & (0x1UL << 31)) ? 1 : 0; /*lint !e514*/
		err = (int)copy_to_user((int*)arg, (void*)&ret, sizeof(int));
	}
	else if(KDF_IOC_SETDATA == cmd)
	{
		data_size = *(u32*)arg;
		err = (int)copy_from_user((void*)(fp->private_data),(void*)(arg + sizeof(u32)), data_size);
		err = (int)kdf_hash_make(fp->private_data, data_size);
	}
	else if(KDF_IOC_SETKEY == cmd)
	{
		err = (int)copy_from_user((void*)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + KDF_SHA_KEY_LENGTH * KEY_INDEX),(void*)arg, KDF_SHA_KEY_LENGTH);
	}
	else if(KDF_IOC_GETHASH == cmd)
	{
		err = (int)copy_to_user((void*)arg, (void*)(CIPHER_REGBASE_ADDR + CIPHER_KEYRAM_OFFSET + KDF_SHA_KEY_LENGTH * KEY_INDEX), KDF_SHA_KEY_LENGTH);
	}
	else
	{
		err = -1;
	}
	return err;
}
/*lint -restore +e26*/

int kdfdev_init(void)
{
	int result = 0;

	if(kdf_init())
		return -1;

	dev_t devno = MKDEV(kdfdev_major,0);/*lint !e701*/
	if(kdfdev_major)
	{
		result = register_chrdev_region(devno, KDFDEV_NR_DEVS, "kdfdev");
	}
	else
	{
		result = alloc_chrdev_region(&devno, 0, KDFDEV_NR_DEVS, "kdfdev");
		kdfdev_major = MAJOR(devno);
	}
	if(result < 0)
	{
		return result;
	}

	kdf_devp = kmalloc(KDFDEV_NR_DEVS * sizeof(struct kdf_dev_mem), GFP_KERNEL);
	if(!kdf_devp)
	{ 
		unregister_chrdev_region(devno, KDFDEV_NR_DEVS);
		return -ENOMEM;
	}
	memset(kdf_devp, 0, (KDFDEV_NR_DEVS * sizeof(struct kdf_dev_mem)));

	kdf_devp->size = KDFDEV_MEM_SIZE;
	kdf_devp->buf = kmalloc(KDFDEV_MEM_SIZE, GFP_KERNEL);
	if(!(kdf_devp->buf))
	{
		kfree(kdf_devp);
		kdf_devp = NULL;
		unregister_chrdev_region(devno, KDFDEV_NR_DEVS);
		return -ENOMEM;
	}
	memset(kdf_devp->buf, 0, KDFDEV_MEM_SIZE);

	cdev_init(&chardev, &kdf_fops);
	chardev.owner = THIS_MODULE;
	/*chardev.ops =  &kdf_fops;*/

	return cdev_add(&chardev, MKDEV(kdfdev_major, 0), KDFDEV_NR_DEVS);/*lint !e701*/
}

void kdfdev_exit(void)
{
	cdev_del(&chardev);
	kfree(kdf_devp->buf);
	kfree(kdf_devp);
	unregister_chrdev_region(MKDEV(kdfdev_major, 0), KDFDEV_NR_DEVS);/*lint !e701*/
}

MODULE_AUTHOR("W00228729");
MODULE_LICENSE("GPL");
