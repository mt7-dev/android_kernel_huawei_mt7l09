
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/io.h>
#include <linux/err.h>

#include <osl_types.h>

#include <bsp_sram.h>
#include <bsp_cipher.h>
#include <soc_onchiprom.h>

#include "bsp_sec_rsa.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CIPHER_BSWAP32(val) \
        (((val) >> 24) | (((val) >> 8) & 0x0000ff00) | \
        (((val) << 8) & 0x00ff0000) | ((val) << 24))

extern void bsp_power_reboot_recovery(void);

int bsp_sec_calc_sha256(u32 dataAddr, u32 dataLen, u32* hash)
{
    int i, ret;
    u32 length;
    u32 key[8];

    memset((void*)hash, 0, sizeof(key));
    while (dataLen)
    {
        length = dataLen > 512 ? 512 : dataLen;
        memcpy((void*)key, (void*)hash, sizeof(key));

        ret = kdf_hash_setkey((void*)key);
        if (ret) {
            printk(KERN_ERR"fail to set cipher key\r\n");
            return ret;
        }

        ret = kdf_hash_make((void*)dataAddr, length);
        if (ret) {
            printk(KERN_ERR"fail to make hash\r\n");
            return ret;
        }

        ret = kdf_result_hash_get((void*)hash);
        if (ret) {
            printk(KERN_ERR"fail to get hash\r\n");
            return ret;
        }

        dataLen -= length;
        dataAddr += length;
    }

    for (i = 0; i < 8; i++) {
        hash[i] = CIPHER_BSWAP32(hash[i]);
    }

    return 0;
}

int bsp_sec_calc_rsa(KEY_STRUCT *pubKey, u32* pIndata, u32* pOutdata)
{
    if ((NULL == pubKey) || (NULL == pIndata) || (NULL == pOutdata))
        return !OK;

    NN_ModExpex((NN_DIGIT_EX *)pOutdata, (NN_DIGIT_EX *)pIndata, (NN_DIGIT_EX *)(pubKey->e),
        MAX_N_LENGTH, (NN_DIGIT_EX *)(pubKey->n), MAX_N_LENGTH);

    return OK;
}

int bsp_sec_idio_identify(u32 dataAddr, u32 dataLen, KEY_STRUCT *pubKey, u32* pIdio)
{
    int i, ret;
    u32 hash[8];
    u32 rsa[IDIO_LEN/4];

    memset(hash, 0, 8*4);
    memset(rsa, 0, IDIO_LEN);

    ret = bsp_sec_calc_sha256(dataAddr, dataLen, hash);
    if (ret) {
        return ret;
    }

    ret = bsp_sec_calc_rsa(pubKey, pIdio, rsa);
    if (ret) {
        return ret;
    }

    for(i = 0; i < 8; i++) {
        if(hash[i] != rsa[i]) {
            printk(KERN_ERR"the hash is:\r\n");
            for (i = 0; i < 8; i++) {
                printk(KERN_ERR"%#x", hash[i]);
            }
            printk(KERN_ERR"the corect result is:\r\n");
            for (i = 0; i < 8; i++) {
                printk(KERN_ERR"%#x", rsa[i]);
            }

            return -1;
        }
    }

    return 0;
}

/* we use ssdma(sha-160) before, and we'll use cipher(sha-256) form now on.
 * we should use sha-256 to make idio for all images checked in kernel.
 */
int bsp_sec_check(u32 image_addr, u32 image_length)
{
    u32 oem_ca_addr, oem_ca_idio_addr, image_idio_addr;

    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;

    /* if the sec check is disabled, return */
    if ((1 != share_date->bSecEn) || (1 != share_date->bRootCaInited))
    {
        printk(KERN_INFO"sec disable\r\n");
        return 0;
    }

    /* check oem ca */
    oem_ca_addr = image_addr + image_length + IDIO_LEN;
    oem_ca_idio_addr = oem_ca_addr + OEM_CA_LEN;
    if (bsp_sec_idio_identify(oem_ca_addr, OEM_CA_LEN, (KEY_STRUCT *)SRAM_SEC_ROOTCA_ADDR, (u32*)oem_ca_idio_addr))
    {
        printk(KERN_ERR"oem ca check error, reboot...\r\n");
        bsp_power_reboot_recovery();
        return -1;
    }

    /* check image */
    image_idio_addr = image_addr + image_length;
    if (bsp_sec_idio_identify(image_addr, image_length, (KEY_STRUCT *)oem_ca_addr, (u32*)image_idio_addr))
    {
        printk(KERN_ERR"image check error, reboot...\r\n");
        bsp_power_reboot_recovery();
        return -1;
    }

    return 0;
}

static int __init bsp_sec_init(void)
{
    int ret = 0;
    ret = kdf_hash_init();
    if (ret)
    {
        printk(KERN_ERR"fail to init kdf, error code %d\r\n", ret);
        return ret;
    }
#if 0
    ret = kdfdev_init();
    if (ret)
    {
        printk(KERN_ERR"fail to init kdf dev, error code %d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}

static void __exit bsp_sec_exit(void)
{
#if 0
    kdfdev_exit();
#endif
}

/*arch_initcall*/
subsys_initcall(bsp_sec_init);
module_exit(bsp_sec_exit);

MODULE_AUTHOR("z00227143@huawei.com");
MODULE_DESCRIPTION("HIS Balong V7R2 Image Check");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
}
#endif

