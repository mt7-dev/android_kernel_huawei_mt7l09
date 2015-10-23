
#include <osl_types.h>
#include <osl_bio.h>
#include <soc_memmap.h>
#include <soc_onchiprom.h>

#include <hi_nandc.h>
#include <hi_wdt.h>
#include <hi_uart.h>
#include <hi_syssc.h>

#include <bsp_sram.h>

#define FASTBOOT_LENGTH_INDEX (11*4)

void print_info(const char *buffer)
{
    int loop;
    volatile u32 status = 0;
    while (*buffer != '\0')
    {
        /* wait for the tx fifo is empty and the uart is idle */
        loop = 0x10000;
        while ((status == 0) && loop--)
        {
            status = readl(HI_UART0_REGBASE_ADDR + UART_REGOFF_USR) & 0x05;
        }

        writel((u32)(*buffer), HI_UART0_REGBASE_ADDR + UART_REGOFF_RBR);
        buffer++;
    }
}

void bsp_wdt_reboot()
{
    /* enable watchdog */
    writel(0x69200000, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL2_OFFSET);
    writel(0x69200009, HI_SYSCTRL_BASE_ADDR + HI_SC_CTRL2_OFFSET);

    /* unlock watchdog */
    writel(0x1ACCE551, HI_WDT_BASE_ADDR + HI_WDG_LOCK_OFFSET);

    /* set watchdog load */
    writel(0x1000, HI_WDT_BASE_ADDR + HI_WDG_LOAD_OFFSET);

    /* enable reset and interrupt */
    writel(0x03, HI_WDT_BASE_ADDR + HI_WDG_CONTROL_OFFSET);

    /* lock watchdog */
    writel(0, HI_WDT_BASE_ADDR + HI_WDG_LOCK_OFFSET);

    /* wait for reboot */
    /* coverity[no_escape] */
    while (1);
}

/* this function won't return if the fastboot image is wrong */
void bsp_sec_check_fastboot(u32 fastboot_addr)
{
    u32 i, fastboot_length, oem_ca_addr, image_idio_addr, oem_ca_idio_addr;

    tOcrShareData *share_date = (tOcrShareData *)M3_TCM_SHARE_DATA_ADDR;

    /* copy share data to new place */
    for (i = 0; i < M3_TCM_SHARE_DATA_SIZE; i += 4)
        writel(readl(M3_TCM_SHARE_DATA_ADDR + i), SRAM_SEC_SHARE + i);

    /* if not boot from chip or the sec check is disabled, return */
    if ((1 != share_date->bSecEn) || (1 != share_date->bRootCaInited))
    {
        print_info("\r\nsec disable\r\n");
        return;
    }

    /* for read nand, set oob_len_sel 0 */
    writel(HI_NANDC_REGBASE_ADDR + HI_NFC_OOB_SEL_OFFSET , 1);

    /* check oem ca */
    fastboot_length = *(u32*)(fastboot_addr + FASTBOOT_LENGTH_INDEX);
    oem_ca_addr = fastboot_addr + fastboot_length + IDIO_LEN;
    oem_ca_idio_addr = oem_ca_addr + OEM_CA_LEN;
    if (share_date->idioIdentify(oem_ca_addr, OEM_CA_LEN, share_date->pRootKey, (u32*)oem_ca_idio_addr))
    {
        print_info("\r\noem ca check error, reboot...\r\n");
        share_date->ulEnumFlag = AUTO_ENUM_FLAG_VALUE;
        bsp_wdt_reboot();
    }

    /* check fastboot image */
    image_idio_addr = fastboot_addr + fastboot_length;
    if (share_date->idioIdentify(fastboot_addr, fastboot_length, (KEY_STRUCT *)oem_ca_addr, (u32*)image_idio_addr))
    {
        print_info("\r\nimage check error, reboot...\r\n");
        share_date->ulEnumFlag = AUTO_ENUM_FLAG_VALUE;
        bsp_wdt_reboot();
    }

    print_info("\r\ncheck fastboot ok\r\n");
}

