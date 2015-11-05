
/*lint --e{537} */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include <hi_dsp.h>
#include <hi_onoff.h>
#include <ptable_com.h>

#include <bsp_om.h>
#include <bsp_dsp.h>
#include <bsp_ipc.h>
#include <bsp_icc.h>
#include <bsp_sec.h>
#include <bsp_sram.h>
#include <bsp_nandc.h>
#include <bsp_shared_ddr.h>
#include <drv_mailbox.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>

#include <product_config.h>

#ifdef __cplusplus
extern "C" {
#endif



int bsp_dsp_is_hifi_exist(void)
{
    int ret = 0;
    DRV_MODULE_SUPPORT_STRU   stSupportNv = {0};

    ret = (int)bsp_nvm_read(NV_ID_DRV_MODULE_SUPPORT, (u8*)&stSupportNv, sizeof(DRV_MODULE_SUPPORT_STRU));
    if (ret)
        ret = 0;
    else
        ret = (int)stSupportNv.hifi;

    return ret;
}

#ifdef CONFIG_HIFI

static void bsp_hifi_init_share_memory(struct drv_hifi_sec_load_info *section_info)
{
    writel(HIFI_MEM_BEGIN_CHECK32_DATA, (u32)section_info);
    writel(HIFI_MEM_BEGIN_CHECK32_DATA, (u32)section_info + HIFI_SHARE_MEMORY_SIZE - sizeof(u32));
}

static int bsp_hifi_check_sections(struct drv_hifi_image_head *img_head,
                                 struct drv_hifi_image_sec *img_sec)
{
    if ((img_sec->sn >= img_head->sections_num)
        || (img_sec->src_offset + img_sec->size > img_head->image_size)
        || (img_sec->type >= (unsigned char)DRV_HIFI_IMAGE_SEC_TYPE_BUTT)
        || (img_sec->load_attib >= (unsigned char)DRV_HIFI_IMAGE_SEC_LOAD_BUTT)) {
        return -1;
    }

    return 0;
}

int bsp_hifi_load_sections(void *hifi_image)
{
    int ret = 0;
    u32 i = 0, dynamic_section_num = 0, dynamic_section_data_offset = 0;

    void *section_virtual_addr = NULL;

    struct drv_hifi_sec_load_info *section_info = NULL;

    struct drv_hifi_image_head *hifi_head = (struct drv_hifi_image_head *)hifi_image;

    section_info = (struct drv_hifi_sec_load_info *)HIFI_SHARE_MEMORY_ADDR;

    bsp_hifi_init_share_memory(section_info);

    for (i = 0; i < hifi_head->sections_num; i++)
    {
        if (bsp_hifi_check_sections(hifi_head, &(hifi_head->sections[i])))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "error hifi image section, index: %d\r\n", i);
            ret = -1;
            goto err_unmap;
        }

        if (DRV_HIFI_IMAGE_SEC_LOAD_STATIC == hifi_head->sections[i].load_attib)
        {
        	/* 静态加载 */
            if ((hifi_head->sections[i].des_addr < DDR_HIFI_ADDR) ||
                ((hifi_head->sections[i].des_addr + hifi_head->sections[i].size) > (DDR_HIFI_ADDR + DDR_HIFI_SIZE)))
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP,
                    "error hifi address, %x \r\n", hifi_head->sections[i].des_addr);
                return -1;
            }
			/* 指向镜像存放的ddr   地址 */
            section_virtual_addr = (void*)ioremap_nocache(hifi_head->sections[i].des_addr,
                hifi_head->sections[i].size);
            if (NULL == section_virtual_addr)
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to io remap, %d \r\n", __LINE__);
                return -ENOMEM;
            }
			/* 镜像拷贝(源->   目的) */
            memcpy(section_virtual_addr,
                   (void*)((char*)hifi_head + hifi_head->sections[i].src_offset),
                   hifi_head->sections[i].size);
            iounmap(section_virtual_addr);
        }
        else if (DRV_HIFI_IMAGE_SEC_LOAD_DYNAMIC == hifi_head->sections[i].load_attib)
        {
        	/* 动态加载 */
            if (dynamic_section_data_offset + hifi_head->sections[i].size > HIFI_SEC_DATA_LENGTH)
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP,
                    "hifi dynamic section too large\r\n");
                return -1;
            }
            /* copy data to share addr */
			/* 将镜像拷贝到段信息结构体，供其他机制加载 */
            memcpy((void*)&(section_info->sec_data[dynamic_section_data_offset]),
                   (void*)((char*)hifi_head + hifi_head->sections[i].src_offset),
                   hifi_head->sections[i].size);

            /* update section info */
			/* 更新段地址 */
            section_info->sec_addr_info[dynamic_section_num].sec_source_addr
                = SHD_DDR_V2P((u32)&(section_info->sec_data[dynamic_section_data_offset]));
			/* 更新段长度 */
            section_info->sec_addr_info[dynamic_section_num].sec_length
                = hifi_head->sections[i].size;
			/* 更新段目的地址 ，DDR   地址*/
            section_info->sec_addr_info[dynamic_section_num].sec_dest_addr
                = hifi_head->sections[i].des_addr;

            dynamic_section_data_offset += hifi_head->sections[i].size;
            dynamic_section_num++;
        }
        else if (DRV_HIFI_IMAGE_SEC_UNLOAD == hifi_head->sections[i].load_attib)
        {
        	/* 无须底软加载 */

            section_virtual_addr = (void*)ioremap_nocache(hifi_head->sections[i].des_addr,
                hifi_head->sections[i].size);
            if (NULL == section_virtual_addr)
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to io remap, %d \r\n", __LINE__);
                return -ENOMEM;
            }
			/* 将信息放入邮箱中 */
            drv_hifi_fill_mb_info((u32)section_virtual_addr);
            iounmap(section_virtual_addr);
        }
        else
        {
        	/* 加载方式有误 */
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP,
                "unknown section attribute %d\r\n", hifi_head->sections[i].load_attib);
            ret = -1;
            goto err_unmap;
        }
    }
    section_info->sec_num = dynamic_section_num;

    ret = bsp_ipc_int_send(IPC_CORE_MCORE, IPC_MCU_INT_SRC_HIFI_PU);
    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP,
            "send hifi pu ipc error %d\r\n", ret);
        goto err_unmap;
    }

err_unmap:

    return ret;
}
#endif

static int bsp_dsp_load_image(char* part_name)
{
    int ret = 0;
    u32 offset = 0;
    u32 skip_len = 0;

    void *bbe_ddr_addr = NULL;
    void *tds_data_addr = NULL;

    /*coverity[var_decl] */
    struct image_head head;

#ifdef CONFIG_HIFI
    struct drv_hifi_image_head *hifi_head = NULL;
    void *hifi_image = NULL;
#endif

    /* clean ok flag */
    writel(0, (void*)SHM_MEM_DSP_FLAG_ADDR);
	/* 指向一块DDR   空间用于存放镜像和配置数据 */
    bbe_ddr_addr = (void*)ioremap_nocache(DDR_TLPHY_IMAGE_ADDR, DDR_TLPHY_IMAGE_SIZE);
	
    tds_data_addr = (void*)ioremap_nocache(DDR_LPHY_SDR_ADDR + 0x1C0000, 0x40000);
    if ((NULL == bbe_ddr_addr) || (NULL == tds_data_addr))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to io remap, %d \r\n", __LINE__);
        ret = -ENOMEM;
        goto err_unmap;
    }
	/* 获得在nand   中的bbe   镜像头 */
    if (NAND_OK != bsp_nand_read(part_name,  0, (char*)&head, sizeof(struct image_head), &skip_len))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to load dsp image head\r\n");
        ret = NAND_ERROR;
        goto err_unmap;
    }

    /*coverity[uninit_use_in_call] */
	/* 判断是否找到dsp   镜像 */
    if (memcmp(head.image_name, DSP_IMAGE_NAME, sizeof(DSP_IMAGE_NAME)))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "dsp image not found\r\n");
        goto err_unmap;
    }
#if 0
    if (head.load_addr != DDR_TLPHY_IMAGE_ADDR)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP,
            "dsp image not match, ddr addr asume to 0x%x\r\n",
            DDR_TLPHY_IMAGE_ADDR);
        goto err_no_part;
    }
#endif

    offset += LPHY_BBE16_MUTI_IMAGE_OFFSET + sizeof(struct image_head) + skip_len;
	/* 将镜像从nand   读入申请的ddr   中 */
    if (NAND_OK == bsp_nand_read(part_name, offset, (char*)bbe_ddr_addr, LPHY_BBE16_MUTI_IMAGE_SIZE, &skip_len))
    {
        printk(KERN_INFO"succeed to load dsp image, address: 0x%x, size: 0x%x\r\n",
            DDR_TLPHY_IMAGE_ADDR, DDR_TLPHY_IMAGE_SIZE);
    }
    else
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to load dsp image\r\n");
        ret = NAND_ERROR;
        goto err_unmap;
    }

    offset += LPHY_BBE16_MUTI_IMAGE_SIZE + skip_len;
	/* 将td   配置信息从nand   读入申请的ddr   中 */
    if (NAND_OK == bsp_nand_read(part_name, offset, (char*)tds_data_addr, TPHY_BBE16_CFG_DATA_SIZE, &skip_len))
    {
        printk(KERN_INFO"succeed to load TD config data, address: 0x%x, size: 0x%x\n",
            DDR_LPHY_SDR_ADDR + 0x1C0000, 0x40000);
    }
    else
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to load TD config data\n");
        ret = NAND_ERROR;
        goto err_unmap;
    }

#ifdef CONFIG_HIFI
    if (bsp_dsp_is_hifi_exist())
    {
    	/* 指向hifi   镜像头 */
        hifi_head = (struct drv_hifi_image_head *)kmalloc(sizeof(struct drv_hifi_image_head), GFP_KERNEL);
        if (NULL == (void *)hifi_head)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to malloc hifi head space\n");
            ret = -1;
            goto err_hifi;
        }

        offset += TPHY_BBE16_CFG_DATA_SIZE + skip_len;
		/* 从nand   中读取hifi_head    */
        if (NAND_OK != bsp_nand_read(part_name, offset, (char*)hifi_head,
            sizeof(struct drv_hifi_image_head), &skip_len))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to load hifi image\n");
            ret = NAND_ERROR;
            goto err_hifi;
        }

        if (hifi_head->sections_num > HIFI_SEC_MAX_NUM)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "hifi image head error\n");
            kfree(hifi_head);
            ret = -1;
            goto err_hifi;
        }

        hifi_image = vmalloc(hifi_head->image_size);
        if (NULL == (void *)hifi_image)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to malloc hifi image space, size: 0x%x\n", hifi_head->image_size);
            kfree(hifi_head);
            ret = -1;
            goto err_hifi;
        }
		/* 从nand   中读取hifi   镜像到hifi_image    */
        if (NAND_OK != bsp_nand_read(part_name, offset, (char*)hifi_image, hifi_head->image_size, &skip_len))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "fail to load hifi image\n");
            kfree(hifi_head);
            vfree(hifi_image);
            ret = NAND_ERROR;
            goto err_hifi;
        }
		/* 释放 */
        kfree(hifi_head);

        if (bsp_hifi_load_sections(hifi_image))
        {
            vfree(hifi_image);
            ret = -1;
            goto err_hifi;
        }

        vfree(hifi_image);
    }
err_hifi:

#endif

    /* set the ok flag of dsp image */
    writel(DSP_IMAGE_STATE_OK, (void*)SHM_MEM_DSP_FLAG_ADDR);

err_unmap:
    if (NULL != bbe_ddr_addr)
        iounmap(bbe_ddr_addr);
    if (NULL != tds_data_addr)
        iounmap(tds_data_addr);

    return ret;
}

#ifdef HI_ONOFF_PHONE

#define PARTITION_MODEM_DSP_NAME                "block2mtd: /dev/block/mmcblk0p27"

int bsp_load_modem_dsp(void)
{
    return bsp_dsp_load_image(PARTITION_MODEM_DSP_NAME);
}

#else

int __init bsp_dsp_probe(struct platform_device *pdev)
{
    int ret = 0;

#ifndef HI_ONOFF_PHONE
    struct ST_PART_TBL* dsp_part = NULL;
	/* 通过模块名来查找相应模块的镜像 */
    dsp_part = find_partition_by_name(PTABLE_DSP_NM);
    if(NULL == dsp_part)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "load ccore image succeed\r\n");
        ret = -EAGAIN;
        goto err_no_part;
    }
	/* 通过镜像名来加载dsp  镜像 */
    ret = bsp_dsp_load_image(dsp_part->name);

err_no_part:

#endif

    return ret;
}

static struct platform_device bsp_dsp_device = {
    .name = "bsp_dsp",
    .id = 0,
    .dev = {
    .init_name = "bsp_dsp",
    },
};

static struct platform_driver bsp_dsp_drv = {
    .probe      = bsp_dsp_probe,
    .driver     = {
        .name     = "bsp_dsp",
        .owner    = THIS_MODULE,
    },
};

static int bsp_dsp_acore_init(void);
static void bsp_dsp_acore_exit(void);

static int __init bsp_dsp_acore_init(void)
{
    int ret = 0;

    ret = platform_device_register(&bsp_dsp_device);
    if(ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "register his_modem device failed\r\n");
        return ret;
    }

    ret = platform_driver_register(&bsp_dsp_drv);
    if(ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DSP, "register his_modem driver failed\r\n");
        platform_device_unregister(&bsp_dsp_device);
    }

    return ret;
}

static void __exit bsp_dsp_acore_exit(void)
{
    platform_driver_unregister(&bsp_dsp_drv);
    platform_device_unregister(&bsp_dsp_device);
}

module_init(bsp_dsp_acore_init);
module_exit(bsp_dsp_acore_exit);

MODULE_AUTHOR("z00227143@huawei.com");
MODULE_DESCRIPTION("HIS Balong V7R2 DSP load");
MODULE_LICENSE("GPL");

#endif

#ifdef __cplusplus
}
#endif
