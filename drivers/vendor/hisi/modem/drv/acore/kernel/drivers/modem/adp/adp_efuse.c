#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include "product_config.h"
#include "bsp_om.h"
#include "drv_efuse.h"
#include "bsp_efuse.h"
#include "hi_efuse.h"

#ifdef CONFIG_EFUSE

int DRV_GET_DIEID(unsigned char* buf,int length)
{
    unsigned int i = 0;
    int ret = 0;
    
    u32 *buf_die_id = (u32*)buf;
    if(NULL == buf)
    {
        efuse_print_error("die id buf is error.\n");
        return EFUSE_ERROR;
    }

    if(length < EFUSE_DIEID_LEN)
    {
        efuse_print_error("die id lenth is error.\n");
        return EFUSE_ERROR;
    }

    memset(buf, 0, EFUSE_DIEID_LEN);
    
    pr_info("efuse read start group %d length %d.\n", EFUSE_GRP_DIEID, EFUSE_DIEID_SIZE);

    if(0 != (ret = bsp_efuse_read((u32*)buf, EFUSE_GRP_DIEID, EFUSE_DIEID_SIZE)))
    {
        efuse_print_error("die id read efuse error.\n");
        return READ_EFUSE_ERROR;
    }

    for(i = 0;i < EFUSE_DIEID_SIZE;i++)
    {
        pr_info("efuse end buf[%d] is 0x%x.\n", i, buf[i]);
    }
    pr_info("efuse read end ret %d\n", ret);

    buf_die_id[EFUSE_DIEID_SIZE-1] &= (((u32)0x1 << EFUSE_DIEID_BIT) - 1);
    
    return EFUSE_OK;
}



#ifdef HI_K3_EFUSE
int DRV_GET_CHIPID(unsigned char* buf,int length)
{
    unsigned int i = 0;
    int ret = 0;
    
    if(NULL == buf)
    {
        efuse_print_error("die id buf is error.\n");
        return EFUSE_ERROR;
    }

    if(length < EFUSE_CHIPID_LEN)
    {
        efuse_print_error("die id lenth is error.\n");
        return EFUSE_ERROR;
    }

    memset(buf, 0, EFUSE_CHIPID_LEN);
    
    pr_info("efuse read start group %d length %d.\n", EFUSE_GRP_CHIPID, EFUSE_CHIPID_SIZE);
    
    if(0 != (ret = bsp_efuse_read((u32*)buf, EFUSE_GRP_CHIPID, EFUSE_CHIPID_SIZE)))
    {
        efuse_print_error("chip id read efuse error.\n");
        return READ_EFUSE_ERROR;
    }
    
    for(i = 0;i < EFUSE_CHIPID_SIZE;i++)
    {
        pr_info("efuse end buf[%d] is 0x%x.\n", i, buf[i]);
    }
    pr_info("efuse read end ret %d\n", ret);

    return EFUSE_OK;
}
#else
int DRV_GET_CHIPID(unsigned char* buf,int length)
{
    return EFUSE_OK;
}

#endif

void DRV_DIEID_TEST(void)
{
    int i = 0;
    char *buf = kzalloc(EFUSE_DIEID_LEN, GFP_KERNEL);
    if(NULL == buf)
    {
        efuse_print_info("alloc mem is fail.\n");
        return;
    }
    
    if(EFUSE_OK == DRV_GET_DIEID(buf, EFUSE_DIEID_LEN))
    {
        for(i = 0;i < EFUSE_DIEID_LEN;i++)
        {
            efuse_print_info("0x%x \n", *(buf+i));
        }

    }
    else
    {
        efuse_print_error("DRV_GET_DIEID is fail.\n");
    }
    
    kfree(buf);
}
#else

int DRV_GET_DIEID(unsigned char* buf,int length)
{
    return EFUSE_OK;
}

int DRV_GET_CHIPID(unsigned char* buf,int length)
{
    return EFUSE_OK;
}

#endif

