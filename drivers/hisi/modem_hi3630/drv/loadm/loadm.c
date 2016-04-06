/******************************************************************************
  File Name       : loadm.c
  Description     : load modem image(vxworks image),run in ccore
  History         :
******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/mtd/mtd.h>
#include <linux/dma-mapping.h>
#include <linux/decompress/generic.h>
#include <asm/dma-mapping.h>

#include "product_config.h"
#include "loadm.h"
#include "bsp_memmap.h"
#include "ptable_com.h"
#include "hi_common.h"
#include "bsp_nandc.h"
#include <bsp_ipc.h>
#include <bsp_sec.h>
#include <soc_onchiprom.h>
#include <bsp_shared_ddr.h>


static void* g_ccore_entry = NULL;


/*****************************************************************************
 函 数 名  : 加载ccore镜像
 功能描述  : load partition image
 输入参数  : struct ST_PART_TBL *part 对应的分区
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
#ifndef BSP_CONFIG_HI3630
int his_modem_load_vxworks(char *part_name)
{
    int ret = 0;
    int offset = 0;
    int skip_len = 0;
    u32 image_total_length = 0;
    void *image_load_addr = 0;
    decompress_fn inflate_fn = NULL;

    struct image_head head;

    hi_trace(HI_INFO, ">>loading:%s.....\r\n", part_name);

    ret = bsp_nand_read(part_name, (FSZ)0, &head, sizeof(struct image_head) , &skip_len);
    if (NAND_OK != ret)
    {
        hi_trace(HI_ERR, "fail to read vxworks image head, error code 0x%x\r\n", ret);
        return NAND_ERROR;
    }

    if (memcmp(head.image_name, CCORE_IMAGE_NAME, sizeof(CCORE_IMAGE_NAME)))
    {
        hi_trace(HI_ERR, "vxworks image error!!.\r\n");
        return NAND_ERROR;
    }

    if (head.image_length + 2*IDIO_LEN + OEM_CA_LEN > PRODUCT_CFG_FLASH_CCORE_LEN)
    {
        hi_trace(HI_ERR, "loadsize is incorrect, 0x%x!\r\n",
            head.image_length + 2*IDIO_LEN + OEM_CA_LEN);
        return NAND_ERROR;
    }

    g_ccore_entry = ioremap_cached(head.load_addr, DDR_MCORE_SIZE - (MCORE_TEXT_START_ADDR - DDR_MCORE_ADDR));
    if(!g_ccore_entry)
    {
        hi_trace(HI_ERR, "ioremap failed.\r\n");
        return NAND_ERROR;
    }

    offset += sizeof(struct image_head) + skip_len;
    image_total_length = (u32)head.image_length + 2*IDIO_LEN + OEM_CA_LEN;

    if (head.is_compressed)
    {
        image_load_addr = g_ccore_entry - (MCORE_TEXT_START_ADDR - DDR_MCORE_ADDR)
            + DDR_MCORE_SIZE - image_total_length;
    }
    else
    {
        image_load_addr = g_ccore_entry;
    }

    ret = bsp_nand_read(part_name, offset, image_load_addr, image_total_length, &skip_len);
    if(NAND_OK != ret)
    {
        hi_trace(HI_ERR, "fail to read vxworks image, error code 0x%x\r\n", ret);
        goto exit;
    }

    ret = bsp_sec_check(image_load_addr, head.image_length);
    if (ret)
    {
        hi_trace(HI_ERR, "fail to check vxworks image, error code 0x%x\r\n", ret);
        goto exit;
    }

    if (head.is_compressed)
    {
        hi_trace(HI_INFO, ">>start to decompress vxworks image ...\r\n");
        inflate_fn = decompress_method((const unsigned char *)image_load_addr, 2, NULL);
        if (inflate_fn)
        {
            ret = inflate_fn((unsigned char*)image_load_addr,
                head.image_length, NULL, NULL, (unsigned char*)g_ccore_entry,
                NULL, (void(*)(char*))printk);
            if (ret)
            {
                hi_trace(HI_ERR, "fail to decompress vxworks image, error code 0x%x\r\n", ret);
                goto exit;
            }
        }
        else
        {
            hi_trace(HI_ERR, "fail to get decompress method\r\n");
            goto exit;
        }
    }

    /* flush cache */
    __dma_single_cpu_to_dev_noverify((const void *)g_ccore_entry,
                     DDR_MCORE_SIZE - (MCORE_TEXT_START_ADDR - DDR_MCORE_ADDR),
                     CACHE_DMA_TO_DEVICE);

    hi_trace(HI_INFO, ">>load vxworks ok, entey %#x, length %#x\r\n", head.load_addr, head.image_length);

exit:
    iounmap((void volatile *)g_ccore_entry);

    return ret;
}
#else
int his_modem_load_vxworks(char *part_name)
{
    int ret = 0;
    int offset = 0;
    int skip_len = 0;
    u32 image_total_length = 0;
    void *image_load_addr = 0;

    struct image_head head;

    hi_trace(HI_INFO, ">>loading:%s.....\r\n", part_name);

    ret = bsp_nand_read(part_name, (FSZ)0, &head, sizeof(struct image_head) , &skip_len);
    if (NAND_OK != ret)
    {
        hi_trace(HI_ERR, "fail to read vxworks image head, error code 0x%x\r\n", ret);
        return NAND_ERROR;
    }

    if (memcmp(head.image_name, CCORE_IMAGE_NAME, sizeof(CCORE_IMAGE_NAME)))
    {
        hi_trace(HI_ERR, "vxworks image error!!.\r\n");
        return NAND_ERROR;
    }

    offset += sizeof(struct image_head) + skip_len;
    image_total_length = (u32)head.image_length;

    image_load_addr = g_ccore_entry;

    ret = bsp_nand_read(part_name, offset, image_load_addr, image_total_length, &skip_len);
    if(NAND_OK != ret)
    {
        hi_trace(HI_ERR, "fail to read vxworks image, error code 0x%x\r\n", ret);
        goto exit;
    }

    hi_trace(HI_INFO, ">>load vxworks ok, entey %#x, length %#x\r\n", head.load_addr, head.image_length);

exit:

    return ret;
}
#endif

#ifdef BSP_CONFIG_HI3630
#ifdef HI3630_FASTBOOT_MODEM
#define PARTITION_MODEM_NAME                "block2mtd: /dev/block/mmcblk0p26"
#else
#define PARTITION_MODEM_NAME                "block2mtd: /dev/block/mmcblk0p30"
#endif

int bsp_load_modem(void)
{
    return his_modem_load_vxworks(PARTITION_MODEM_NAME);
}

#endif

long g_debug_level = HI_INFO;

static struct platform_device his_modem_device = {
    .name = "his_modem",
    .id = 0,
    .dev = {
    .init_name = "his_modem",
    },
};

static ssize_t log_level_set(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count)
{
    int status = count;
    long val = 0;

    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    g_debug_level = val;

    hi_trace(HI_DEBUG, "log_level_set 0x%x \r\n", (unsigned int)val);

    return status;
}

static ssize_t log_level_get(struct device *dev, struct device_attribute *attr,
                               char *buf)
{
    long val = 0;

    val = g_debug_level ;
    hi_trace(HI_DEBUG, "log_level_get \r\n");

    return snprintf(buf, sizeof(val), "%ld\n", val);
}

static ssize_t modem_entry_store(struct device *dev, struct device_attribute *attr,
                                       const char *buf, size_t count)
{
    int status = count;
    long val = 0;

    if ((strict_strtol(buf, 10, &val) < 0))
        return -EINVAL;

    /* ccore_entry_addr = val; */
    hi_trace(HI_DEBUG, "modem_entry_store 0x%x \r\n", (unsigned int)val);

    return status;
}

static ssize_t modem_entry_show(struct device *dev, struct device_attribute *attr,
                                        char *buf)
{
    void* val = 0;

    val = g_ccore_entry ;
    hi_trace(HI_DEBUG, "modem_entry_show \r\n");

    return snprintf(buf, sizeof(val), "%p\n", val);
}

static DEVICE_ATTR(log_level, S_IWUSR | S_IRUGO,
                                    log_level_get,
                                    log_level_set);

static DEVICE_ATTR(entry_addr, S_IWUSR | S_IRUGO,
                                    modem_entry_show,
                                    modem_entry_store);

static struct attribute *his_modem_attributes[] = {
        &dev_attr_log_level.attr,
        &dev_attr_entry_addr.attr,
        NULL
};

static const struct attribute_group his_modem_group = {
    .attrs = his_modem_attributes,
};

#if 0
/*****************************************************************************
 函 数 名  : his_modem_activate_ccore
 功能描述  : 激活C核
 输入参数  : ccore_entry_addr:c核入口地址
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
static void his_modem_activate_ccore(u32 ccore_entry_addr)
{
    hi_ccore_set_entry_addr(ccore_entry_addr);
    hi_ccore_active();
    hi_trace(HI_DEBUG, "end hi_ccore_active\n");
}
#endif

int his_modem_ipc_send(void)
{
    int ret = 0;

    ret = bsp_ipc_int_send(IPC_CORE_MCORE, IPC_MCU_INT_SRC_CCPU_START);
#if 0
    rproc_msg_t msg = 5;

    ret = RPROC_ASYNC_SEND(HISI_RPROC_LPM3, &msg, 4, ASYNC_MSG, NULL, NULL);
#endif

    return ret;
}

static int __init his_modem_probe(struct platform_device *pdev)
{
    int ret = 0;
#ifdef BSP_CONFIG_HI3630
    u32 image_load_addr = 0;
    u32 image_length = 0;
extern unsigned int is_load_modem(void);

    /* 如果是升级模式，则不激活modem */
    if(!is_load_modem())
    {
        hi_trace(HI_ERR, "upload or charge mode, will not start modem.\r\n");
        return 0;
    }
    if (readl((void*)SHM_MEM_LOADM_ADDR) != LOAD_MODEM_OK_FLAG)
    {
        hi_trace(HI_ERR, "modem image not found\r\n");
        return -1;
    }

    image_load_addr = readl((void*)SHM_MEM_LOADM_ADDR + 4);
    image_length = readl((void*)SHM_MEM_LOADM_ADDR + 8);

    g_ccore_entry = ioremap_nocache(image_load_addr, image_length);
    if(!g_ccore_entry)
    {
        hi_trace(HI_ERR, "ioremap failed. image size: 0x%X\r\n", image_length);
        return NAND_ERROR;
    }

#else
    struct ST_PART_TBL * mc_part = NULL;

    hi_trace(HI_DEBUG, "his_modem_probe.\r\n");

    mc_part = find_partition_by_name(PTABLE_VXWORKS_NM);
    if(NULL == mc_part)
    {
        hi_trace(HI_ERR, "load ccore image succeed\r\n");
        return -EAGAIN;
    }

    if(NAND_OK == his_modem_load_vxworks(mc_part->name))
    {
        hi_trace(HI_DEBUG, "load ccore image succeed\r\n");
    }
    else
    {
        hi_trace(HI_ERR, "load ccore image failed\r\n");
        return -EAGAIN;
    }

    ret = sysfs_create_group(&pdev->dev.kobj, &his_modem_group);
    if (0 != ret)
    {
        hi_trace(HI_ERR,"create his modem sys filesystem node failed.\n");
        return -ENXIO;
    }

#endif

    ret = his_modem_ipc_send();
    if (ret)
    {
        hi_trace(HI_ERR, "load ccore image failed, ret=0x%x\r\n", ret);
    }
    else
    {
        hi_trace(HI_ERR, "load ccore image succeed\r\n");
    }

    return ret;

}

static int his_modem_resume(struct platform_device *dev)
{
    return 0;
}

static void his_modem_shutdown(struct platform_device *dev)
{
    return;
}

static int his_modem_suspend(struct platform_device *dev, pm_message_t state)
{
    return 0;
}

static int his_modem_remove(struct platform_device *dev)
{
    return 0;
}

static struct platform_driver his_modem_drv = {
    .probe      = his_modem_probe,
    .remove     = his_modem_remove,
    .suspend    = his_modem_suspend,
    .resume     = his_modem_resume,
    .shutdown   = his_modem_shutdown,
    .driver     = {
        .name     = "his_modem",
        .owner    = THIS_MODULE,
    },
};

static int __init his_modem_init_driver(void)
{
    int ret = 0;

    hi_trace(HI_DEBUG, "his_modem_init_driver.\r\n");

    ret = platform_device_register(&his_modem_device);
    if(ret)
    {
        hi_trace(HI_ERR, "register his_modem device failed\r\n");
        return ret;
    }

    ret = platform_driver_register(&his_modem_drv);
    if(ret)
    {
        hi_trace(HI_ERR, "register his_modem driver failed\r\n");
        platform_device_unregister(&his_modem_device);
    }

    return ret;

}

static void __exit his_modem_exit_driver(void)
{
    platform_driver_unregister(&his_modem_drv);
    platform_device_unregister(&his_modem_device);
}

/* arch_initcall(his_modem_init_driver);*/
module_init(his_modem_init_driver);
module_exit(his_modem_exit_driver);

MODULE_AUTHOR("x00181146@huawei.com");
MODULE_DESCRIPTION("HIS Balong V7R2 Modem load ");
MODULE_LICENSE("GPL");

