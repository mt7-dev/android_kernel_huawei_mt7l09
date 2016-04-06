#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>


#include "drv_acm.h"
#include "bsp_acm.h"
#include "drv_udi.h"

struct adp_acm_stat {
    int stat_open_err;
    long stat_open_last_err;
    int stat_read_err;
    int stat_read_last_err;
    int stat_write_err;
    int stat_write_last_err;
    int stat_ioctl_err;
    int stat_ioctl_last_err;
    int stat_wait_close;
};

struct adp_acm_context {
    char* dev_name;
    int is_open;
    atomic_t opt_cnt;
    struct adp_acm_stat stat;
};


static struct adp_acm_context adp_acm_ctx[] = {
    {"/dev/acm_ctrl",          /*UDI_USB_ACM_CTRL*/ 0, {0}, {0}},
#ifdef CONFIG_USB_F_BALONG_ACM
    {"/dev/acm_at",            /*UDI_USB_ACM_AT*/ 0, {0}, {0}},
#else
    {"/dev/ttyGS0",            /*UDI_USB_ACM_AT*/ 0, {0}, {0}},
#endif
    {"/dev/acm_c_shell",       /*UDI_USB_ACM_SHELL*/ 0, {0}, {0}},
    {"/dev/acm_4g_diag",       /*UDI_USB_ACM_LTE_DIAG*/ 0, {0}, {0}},
    {"/dev/acm_3g_diag",       /*UDI_USB_ACM_OM*/ 0, {0}, {0}},
    {"/dev/acm_modem",         /*UDI_USB_ACM_MODEM*/ 0, {0}, {0}},
    {"/dev/acm_gps",           /*UDI_USB_ACM_GPS*/ 0, {0}, {0}},
    {"/dev/acm_a_shell",       /*UDI_USB_ACM_3G_GPS*/ 0, {0}, {0}},
    {"/dev/acm_err",           /*UDI_USB_ACM_3G_PCVOICE*/ 0, {0}, {0}},
    {"/dev/acm_voice",         /*UDI_USB_ACM_PCVOICE*/ 0, {0}, {0}},
};
static int stat_invalid_devid = 0;
static int stat_invalid_filp = 0;

#define ADP_ACM_FILP_INVALID(filp) (!(filp)  || IS_ERR(filp) || !filp->private_data)

/*****************************************************************************
* 函 数 名  : bsp_acm_open
*
* 功能描述  : 打开设备(数据通道)
*
* 输入参数  : dev_id: 设备id
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
void* bsp_acm_open(u32 dev_id)
{
    char* name;
    struct file* filp;

    if (dev_id >= sizeof(adp_acm_ctx) / sizeof(struct adp_acm_context)) {
        stat_invalid_devid++;
	return NULL;
    }
    name = adp_acm_ctx[dev_id].dev_name;

    filp = filp_open(name, O_RDWR, 0);
    if (IS_ERR(filp)) {
        adp_acm_ctx[dev_id].stat.stat_open_err++;
        adp_acm_ctx[dev_id].stat.stat_open_last_err = (long)filp;
	return NULL;
    }
    filp->private_data = &adp_acm_ctx[dev_id];
    adp_acm_ctx[dev_id].is_open = 1;

    return filp;
}

/*****************************************************************************
* 函 数 名  : bsp_acm_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acm_close(void* handle)
{
    struct file* filp = (struct file*)handle;
    struct adp_acm_context* ctx;

    if (unlikely(ADP_ACM_FILP_INVALID(filp))) {
        stat_invalid_filp++;
        return -EINVAL;
    }

    ctx = (struct adp_acm_context*)filp->private_data;
    ctx->is_open = 0;

    /* wait for file opt complete */
    while(atomic_read(&ctx->opt_cnt)) {
        ctx->stat.stat_wait_close++;
        msleep(10);
    }

    filp_close(filp, NULL);
    return 0;
}

/*****************************************************************************
* 函 数 名  : bsp_acm_write
*
* 功能描述  : 数据写
*
* 输入参数  : handle:  设备的handle
*             buf: 地址
*             size: 大小
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
s32 bsp_acm_write(void* handle, void *buf, u32 size)
{
    mm_segment_t old_fs;
    struct file* filp = (struct file*)handle;
    struct adp_acm_context* ctx;
    int status;
    loff_t pos = 0;

    if (unlikely(ADP_ACM_FILP_INVALID(filp))) {
        stat_invalid_filp++;
        return -EINVAL;
    }
    ctx = (struct adp_acm_context*)filp->private_data;

    atomic_inc(&ctx->opt_cnt);
    if (unlikely(!ctx->is_open || !(filp->f_path.dentry))) {
        status = -ENXIO;
        goto write_ret;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    status = vfs_write(filp, (void __force __user *)buf, size, &pos);
    set_fs(old_fs);

write_ret:
    atomic_dec(&ctx->opt_cnt);
    if (status < 0) {
        ctx->stat.stat_write_err++;
        ctx->stat.stat_write_last_err = status;
    }
    return status;
}

/*****************************************************************************
* 函 数 名  : bsp_acm_read
*
* 功能描述  : 数据读
*
* 输入参数  : handle:  设备的handle
*             buf: 地址
*             size: 大小
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
s32 bsp_acm_read(void* handle, void *buf, u32 size)
{
    mm_segment_t old_fs;
    struct file* filp = (struct file*)handle;
    struct adp_acm_context* ctx;
    int status;
    loff_t pos = 0;

    if (unlikely(ADP_ACM_FILP_INVALID(filp))) {
        stat_invalid_filp++;
        /* protect system running, usr often use read in while(1). */
        msleep(10);
        return -EINVAL;
    }
    ctx = (struct adp_acm_context*)filp->private_data;

    atomic_inc(&ctx->opt_cnt);
    if (unlikely(!ctx->is_open || !(filp->f_path.dentry))) {
        status = -ENXIO;
        goto read_ret;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    status = vfs_read(filp, (void __force __user *)buf, size, &pos);
    set_fs(old_fs);

read_ret:
    atomic_dec(&ctx->opt_cnt);
    if (status <= 0) {
        /* protect system running, usr often use read in while(1). */
        msleep(100);
        ctx->stat.stat_read_err++;
        ctx->stat.stat_read_last_err = status;
    }
    return status;
}

/*****************************************************************************
* 函 数 名  : bsp_acm_ioctl
*
* 功能描述  : 数据通道属性配置
*
* 输入参数  : handle: 设备的handle
*             u32Cmd: IOCTL命令码
*             pParam: 操作参数
* 输出参数  :
*
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acm_ioctl(void* handle, u32 cmd, void *para)
{
    mm_segment_t old_fs;
    struct file* filp = (struct file*)handle;
    struct adp_acm_context* ctx;
    int status;

    if (unlikely(ADP_ACM_FILP_INVALID(filp))) {
        stat_invalid_filp++;
        return -EINVAL;
    }
    ctx = (struct adp_acm_context*)filp->private_data;

    atomic_inc(&ctx->opt_cnt);
    if (unlikely(!ctx->is_open || !(filp->f_path.dentry))) {
        status = -ENXIO;
        goto ioctl_ret;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    status = do_vfs_ioctl(filp, 0, (unsigned int)cmd, (unsigned long)para);
    set_fs(old_fs);

ioctl_ret:
    atomic_dec(&ctx->opt_cnt);
    if (status < 0) {
        ctx->stat.stat_ioctl_err++;
        ctx->stat.stat_ioctl_last_err = status;
    }
    return status;
}

void acm_adp_dump(void)
{
    int i;

    printk("stat_invalid_devid      :%d\n", stat_invalid_devid);
    printk("stat_invalid_filp       :%d\n", stat_invalid_filp);
    for (i = 0; i < sizeof(adp_acm_ctx) / sizeof(struct adp_acm_context); i++) {
        printk("==== dump dev:%s ====\n", adp_acm_ctx[i].dev_name);
        printk("is_open             :%d\n", adp_acm_ctx[i].is_open);
        printk("opt_cnt             :%d\n", atomic_read(&adp_acm_ctx[i].opt_cnt));
        printk("stat_open_err       :%d\n", adp_acm_ctx[i].stat.stat_open_err);
        printk("stat_open_last_err  :%ld\n", adp_acm_ctx[i].stat.stat_open_last_err);
        printk("stat_read_err       :%d\n", adp_acm_ctx[i].stat.stat_read_err);
        printk("stat_read_last_err  :%d\n", adp_acm_ctx[i].stat.stat_read_last_err);
        printk("stat_write_err      :%d\n", adp_acm_ctx[i].stat.stat_write_err);
        printk("stat_write_last_err :%d\n", adp_acm_ctx[i].stat.stat_write_last_err);
        printk("stat_ioctl_err      :%d\n", adp_acm_ctx[i].stat.stat_ioctl_err);
        printk("stat_ioctl_last_err :%d\n", adp_acm_ctx[i].stat.stat_ioctl_last_err);
        printk("stat_wait_close     :%d\n", adp_acm_ctx[i].stat.stat_wait_close);
        printk("\n");
    }
}

