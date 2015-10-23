/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <boot/boot.h>
#include <boot/uart.h>
#include <boot/tags.h>
#include <boot/flash.h>
#include <boot/board.h>
#include <boot/usb.h>

#include <boot/bootimg.h>

#include <balongv7r2/config.h>
#include <balongv7r2/mddrc.h>
#include <bsp_sec.h>
#include <bsp_sram.h>
#include <soc_onchiprom.h>
#include "config.h"
#include "ptable_com.h"
#include "hi_common.h"
#include "balongv7r2/preboot.h"
#include "bsp_memmap.h"
#include "ddm_phase.h"
#include "bsp_nvim.h"
#include "bsp_pmu.h"
#include "bsp_coul.h"
#include <product_config.h>
#include <bsp_m3.h>
#include <soc_interrupts.h>
#include <bsp_shared_ddr.h>
#include <bsp_sram.h>
#include <hi_uart.h>
#include <time.h>
#include <drv_nv_def.h>
#include <drv_nv_id.h>
#include <hi_smartstar.h>
#include <soc_interrupts_m3.h>
#include <balongv7r2/nnex.h>
#include <balongv7r2/types.h>
#include "bsp_dump_drv.h"

extern unsigned BOOTLOADER_HEAP_ADDR;
extern unsigned RECOVERY_ENTRY;

/*add for recovery boot*/

#define RECOVERY_ANAME PTABLE_RECOVERYA_NM
#define RECOVERY_BNAME PTABLE_RECOVERYB_NM
#ifndef NULL
#define NULL ((void*)0)
#endif

extern void flash_erase_all(void);

struct ST_PART_TBL * ptable_get_part_no_head(void)
{
    return (struct ST_PART_TBL *)(SHM_MEM_PTABLE_ADDR + \
             PTABLE_HEAD_SIZE) ;
}

char* get_recovery_part(void)
{

	struct ST_PART_TBL *part = NULL;
	int count_a=-1;
	int count_b=-1;

	part=(struct ST_PART_TBL *)ptable_get_part_no_head();

	if(NULL==part)
	{
		cprintf("Error:can't get part\n");
		return NULL;
	}
	while(strcmp(PTABLE_END_STR, part->name))
	{
		if(!strcmp(RECOVERY_ANAME, part->name))
		{
			count_a=part->count;
		}
		else if(!strcmp(RECOVERY_BNAME, part->name))
		{
			count_b=part->count;
		}
		else{
			;
		}
		part++;
	}

	if(count_a<0||count_b<0){
		cprintf("recovery-A count or recovery-B count not found!\n ");
		return NULL;
	}
	return count_a>=count_b?RECOVERY_ANAME:RECOVERY_BNAME;
}

int modify_recovery_count(void)
{
	struct ST_PART_TBL *part = NULL;
	/* coverity[returned_null] */
	char *name=get_recovery_part();

	part=(struct ST_PART_TBL *)ptable_get_part_no_head();

	if(NULL==part)
	{
		cprintf("Error:can't get part\n");
		return -1;
	}
	while(strcmp(PTABLE_END_STR, part->name))
	{
		if(!strcmp(name, part->name))
		{
			part->count=part->count-2;
			//if(part->count<0)
				//part->count=0;
			break;
		}
		else{
			;
		}
		part++;
	}

	if(strcmp(PTABLE_END_STR, part->name)){
		return 0;
	}
	return -1;
}
static unsigned create_atags(unsigned taddr, const char *cmdline,
                         unsigned raddr, unsigned rsize)
{
    unsigned n = 0;
    unsigned *tags = (unsigned *) taddr;

	/* coverity[deref_ptr_in_call] */
	unsigned cmd_len = strlen(cmdline) + 1;
	cmd_len = (cmd_len + 3) & (~3) / 4;
	unsigned tags_size = (10 + cmd_len) * 4;

	tags = (unsigned *) alloc(tags_size);

    // ATAG_CORE
    tags[n++] = 2;
    tags[n++] = 0x54410001;
#ifdef CONFIG_ARM_ENABLE_DTS
	if(rsize) {
        // ATAG_MEM
        tags[n++] = 4;
        tags[n++] = 0x54410002;
        tags[n++] = DDR_ACORE_ADDR;
        tags[n++] = DDR_ACORE_SIZE;
    }
#endif
    if(rsize) {
        // ATAG_INITRD2
        tags[n++] = 4;
        tags[n++] = 0x54420005;
        tags[n++] = raddr;
        tags[n++] = rsize;
    }

/*
    if((pcount = flash_get_ptn_count())){
        ptentry *ptn;
        unsigned pn;
        unsigned m = n + 2;

        for(pn = 0; pn < pcount; pn++) {
            ptn = flash_get_ptn(pn);
            memcpy(tags + m, ptn, sizeof(ptentry));
            m += (sizeof(ptentry) / sizeof(unsigned));
        }

        tags[n + 0] = m - n;
        tags[n + 1] = 0x4d534d70;
        n = m;
    }
*/
#ifndef CONFIG_ARM_ENABLE_DTS
    if(cmdline && cmdline[0]) {
        const char *src;
        char *dst;
        unsigned len = 0;

        dst = (char*) (tags + n + 2);
        src = cmdline;
        while((*dst++ = *src++)) len++;

        len++;
        len = (len + 3) & (~3);

            // ATAG_CMDLINE
        tags[n++] = 2 + (len / 4);
        tags[n++] = 0x54410009;

        n += (len / 4);
    }
#endif
        // ATAG_NONE
    tags[n++] = 0;
    tags[n++] = 0;

	cprintf("tags: %x, %d=%d\n",tags, tags_size, n);
	return (unsigned)tags;
}

static void boot_linux_at(unsigned kaddr, unsigned atags)
{
    void (*entry)(unsigned,unsigned,unsigned) = (void*) kaddr;

    entry(0, board_machtype(), atags);
}

/* attention: this function will not return if the auto enum flag is set */
extern void flash_erase_all(void);
/*判断一下是否是先贴后烧*/
unsigned int is_usb_burn_trapflag(void)
{
    static unsigned int is_burn_init =0;
    static unsigned int is_burn_flag =0;
    if(!is_burn_init)
    {
        if(readl(OCR_AUTO_ENUM_FLAG_ADDR) == AUTO_ENUM_FLAG_VALUE)
        {
            is_burn_flag =1;
        }
        else
        {
            is_burn_flag =0;
        }
        is_burn_init =1;
        writel(0, OCR_AUTO_ENUM_FLAG_ADDR);
    }
    return is_burn_flag ;
}

int usb_burn_trap(void)
{
    unsigned int flag = 0;
    unsigned int offset = 0;
    boot_img_hdr *hdr;
    unsigned n;
    unsigned tags;
    const char *cmdline;
    if(!is_usb_burn_trapflag())
    {
        return 0;
    }

    /* erase whole nand flash */
    flash_erase_all();

    hdr = (void*)&RECOVERY_ENTRY;
    offset += (unsigned int)(&RECOVERY_ENTRY);

    if(memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE))
    {
	    cprintf("no recovery to burn, return...\n");
        return 1;
    }

    offset += FLASH_PAGE_SIZE;
    n = (hdr->kernel_size + (FLASH_PAGE_SIZE - 1)) & (~(FLASH_PAGE_SIZE - 1));
    memcpy((void*) hdr->kernel_addr, (void*)offset, n);
    offset += n;

    n = (hdr->ramdisk_size + (FLASH_PAGE_SIZE - 1)) & (~(FLASH_PAGE_SIZE - 1));
    memcpy((void*) hdr->ramdisk_addr, (void*)offset, n);
    offset += n;

    dprintf("\nkernel  @ %x (%d bytes)\n", hdr->kernel_addr, hdr->kernel_size);
    dprintf("ramdisk @ %x (%d bytes)\n\n\n", hdr->ramdisk_addr, hdr->ramdisk_size);

    if(hdr->cmdline[0]) {
        cmdline = (char*) hdr->cmdline;
    } else {
        cmdline = board_cmdline();
        if(cmdline == 0) {
            cmdline = "mem=50M console=null";
        }
    }
    cprintf("cmdline = '%s'\n", cmdline);

	tags = create_atags(ADDR_TAGS, cmdline,
                 hdr->ramdisk_addr, hdr->ramdisk_size);

    boot_linux_at(hdr->kernel_addr, tags);
    return 1;
}

extern int bsp_calc_hash(unsigned long * s_addr, long size, unsigned long * key_addr,
	unsigned long * out_hash, unsigned long * out_hash_len);

int bsp_sec_hash(unsigned int dataAddr, unsigned int dataLen, unsigned int* hash)
{
#ifdef FEATURE_RSA_LOCAL
    unsigned int length = 0;
    unsigned int hmacKey[SHA256_HASH_SIZE];

    memset((void*)hmacKey, 0, sizeof(hmacKey));
    memset((void*)hash, 0, sizeof(hmacKey));

    return bsp_calc_hash((unsigned long *)dataAddr, dataLen,
	(unsigned long *)hmacKey, (unsigned long *)hash, (unsigned long *)&length);
#else
    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;

    return share_date->SHA256Hash(dataAddr, dataLen, hash);
#endif
}

int bsp_sec_rsa(KEY_STRUCT *pubKey, UINT32* pIndata, UINT32* pOutdata)
{
#ifdef FEATURE_RSA_LOCAL
#else
    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;
#endif

    if ((NULL == pubKey) || (NULL == pIndata) || (NULL == pOutdata))
        return -1;

#ifdef FEATURE_RSA_LOCAL
    NN_ModExpex(pOutdata, pIndata, pubKey->e, MAX_N_LENGTH, pubKey->n, MAX_N_LENGTH);
#else
    share_date->RSA(pubKey, pIndata, pOutdata);
#endif

    return 0;
}

int bsp_sec_idio_check(unsigned int dataAddr, unsigned int dataLen, KEY_STRUCT *pubKey, unsigned int* pIdio)
{
    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;

    unsigned int sha256Hash[SHA256_HASH_SIZE];      /*用来存放SHA256值的临时buffer*/
    unsigned int rsa[IDIO_LEN/4];                   /*用来存放RSA加密值的临时buffer*/

    int i, ret;

    /* if not boot from chip or the sec check is disabled, return */
    if (1 != share_date->bSecEn)
    {
        cprintf("\r\nsec disable\r\n");
        return 0;
    }

    if (1 != share_date->bRootCaInited)
    {
        cprintf("\r\nroot ca not inited\r\n");
        return -1;
    }

    memset(sha256Hash, 0, SHA256_HASH_SIZE*4);
    memset(rsa, 0, IDIO_LEN);

    ret = bsp_sec_hash(dataAddr, dataLen, sha256Hash);
    if (ret)
    {
        cprintf("\r\nhash calc error\r\n");
        return ret;
    }

    ret = bsp_sec_rsa(pubKey, pIdio, rsa);
    if (ret)
    {
        cprintf("\r\nrsa calc error\r\n");
        return ret;
    }

    for (i = 0; i < SHA256_HASH_SIZE; i++)
    {
        if(sha256Hash[i] != rsa[i])
            return -1;
    }

    return 0;
}

int bsp_sec_check(unsigned int image_addr, unsigned int image_length)
{
    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;

    unsigned int oem_ca_addr, oem_ca_idio_addr, image_idio_addr;

    /* if not boot from chip or the sec check is disabled, return */
    if ((1 != share_date->bSecEn) || (1 != share_date->bRootCaInited))
    {
        cprintf("\r\nsec disable\r\n");
        return 0;
    }

    /* check oem ca */
    oem_ca_addr = image_addr + image_length + IDIO_LEN;
    oem_ca_idio_addr = oem_ca_addr + OEM_CA_LEN;
    if (bsp_sec_idio_check(oem_ca_addr, OEM_CA_LEN, share_date->pRootKey, (u32*)oem_ca_idio_addr))
    {
        cprintf("\r\noem ca check error, usb boot...\r\n");
        writel(AUTO_ENUM_FLAG_VALUE, OCR_AUTO_ENUM_FLAG_ADDR);
        usb_burn_trap();
        return -1;
    }

    /* check image */
    image_idio_addr = image_addr + image_length;
    if (bsp_sec_idio_check(image_addr, image_length, (KEY_STRUCT *)oem_ca_addr, (u32*)image_idio_addr))
    {
        cprintf("\r\nimage check error, usb boot...\r\n");
        writel(AUTO_ENUM_FLAG_VALUE, OCR_AUTO_ENUM_FLAG_ADDR);
        usb_burn_trap();
        return -1;
    }

    return 0;
}

/*add for mcore image length parse*/
#define MCORE_IMAGE_FLAG_OFFSET         (0x20)
#define MCORE_IMAGE_FLAG_SIZE           (0x8)
#define MCORE_HEADER_SIZE               (2048)
#define NAND_ONE_PAGE_SIZE              (4096)
#define M3_TCM_BASE_ADDR                (0x10000000)
#define M3_VTOR_SIZE                    (144 * 4)
#ifdef M3_IMAGE_LEN_ADDR
#undef M3_IMAGE_LEN_ADDR
#define M3_IMAGE_LEN_ADDR               (M3_TCM_BASE_ADDR + M3_VTOR_SIZE)
#endif

unsigned char raw_header[FLASH_PAGE_SIZE];
struct ptentry pt_find_temp;

void cstest(void)
{
	*(unsigned int *)0x4fe10000 = 0xe320f003;
    hi_ccore_reset();
	hi_ccore_set_entry_addr(0x4fe10000);
    hi_ccore_active();
}

void astest(void)
{
#ifdef CONFIG_PMIC_HI6551
    bsp_hi6551_reg_write(HI6551_IRQ2_OFFSET, 0x0C);
#endif

    void (*entry)(void) = (void*) 0x4fe10004;
	*(unsigned int *)0x4fe10004 = 0xe320f003;
	hi_acore_set_entry_addr(0x4fe10004);
	entry();
}

int boot_vxworks_from_flash(void)
{
	struct ptentry *ptn = 0;
	unsigned offset = 0;
	unsigned int skip_len = 0;
    struct image_head head;

    cprintf("boot vxworks from flash\n");

	if((ptn = flash_find_ptn(PTABLE_VXWORKS_NM)) == 0) {
        cprintf("CAN NOT FIND VXWORKS PARTITION\n");
        return -1;
    }

    if (flash_read(ptn, offset, &head, sizeof(struct image_head), &skip_len))
    {
        cprintf("CANNOT READ BOOT IMAGE HEADER\n");
        return -1;
    }
    offset += sizeof(struct image_head);
    offset += skip_len;

	cprintf("ptn:%x , ptn->loadaddr = %x\n", ptn, head.load_addr);

	if(flash_read(ptn, offset, (void*)(head.load_addr), head.image_length, &skip_len)) {

        cprintf("CANNOT READ VXWORKS IMAGE\n");
        return -1;
    }

    hi_ccore_reset();
	hi_ccore_set_entry_addr(head.load_addr);
    hi_ccore_active();

	return 0;
}

int boot_rtx_cm3_from_flash(void)
{
	struct ptentry_ex *ptn = 0;
	unsigned offset = 0;
	unsigned int skip_len = 0;

    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;
    struct image_head head;

    /* sec check */
    if ((1 == share_date->bSecEn) && (1 == share_date->bRootCaInited))
    {
        memcpy((void*)SRAM_SEC_ROOTCA_ADDR, (void*)(share_date->pRootKey), ROOT_CA_LEN);

        share_date->pRootKey = (KEY_STRUCT *)SRAM_SEC_ROOTCA_ADDR;
    }

    cprintf("boot m3image from flash\n");

	if ((ptn = (struct ptentry_ex *)flash_find_ptn(PTABLE_M3IMG_NM)) == 0) {

        cprintf("no m3image partion found\n");
        return -1;
    }

	cprintf("ptn:%x , ptn->start = %x ptn->length = %x\n", ptn, ptn->ptn.start, ptn->ptn.length);
	cprintf("ptn:%x , ptn->loadaddr = %x ptn->entry = %x\n", ptn, ptn->loadaddr, ptn->entry);

    if (flash_read(&ptn->ptn, offset, &head, sizeof(struct image_head), &skip_len))

    {
        cprintf("CANNOT READ BOOT IMAGE HEADER\n");
        return -1;
    }
    offset += sizeof(struct image_head);
    offset += skip_len;

    if (flash_read(&ptn->ptn, offset, (void*)(head.load_addr),

        head.image_length + 2*IDIO_LEN + OEM_CA_LEN, &skip_len))
    {
        cprintf("CANNOT READ KERNEL IMAGE\n");
        return -1;
    }

    /* sec check */
    if ((1 == share_date->bSecEn) && (1 == share_date->bRootCaInited))
    {
        if (bsp_sec_check(head.load_addr, head.image_length))
        {
            cprintf("SEC CHECK ERROR\n");
            return -1;
        }
    }

	hi_mcore_reset();
	hi_mcore_active();

	return 0;
}

int boot_linux_from_flash(void)
{
    boot_img_hdr *hdr = (void*) raw_header;
    unsigned n;
    ptentry *p =0;
    unsigned offset = 0;
    const char *cmdline;
	unsigned int skip_len = 0;
	unsigned tags;

    tOcrShareData *share_date = (tOcrShareData *)SRAM_SEC_SHARE;
    struct image_head head;

    /* shut down the usb here, kernel will init usb again */
    usb_shutdown();

    ddm_phase_boot_score("start boot linux",__LINE__);

    cprintf("boot linux from flash\n");

    if((p = flash_find_ptn(PTABLE_KERNEL_NM)) == 0) {
        cprintf("NO BOOT PARTITION\n");
        return -1;
    }

    /* sec check */
    if ((1 == share_date->bSecEn) && (1 == share_date->bRootCaInited))
    {
        if (flash_read(p, offset, (void *)&head, sizeof(struct image_head), &skip_len))
        {
            cprintf("CANNOT READ BOOT IMAGE HEADER\n");
            return -1;
        }
        offset += sizeof(struct image_head);
        offset += skip_len;

        if (flash_read(p, offset, (void*)(head.load_addr),
            head.image_length + 2*IDIO_LEN + OEM_CA_LEN, &skip_len))
        {
            cprintf("CANNOT READ KERNEL IMAGE\n");
            return -1;
        }

        if (bsp_sec_check(head.load_addr, head.image_length))
        {
            cprintf("SEC CHECK ERROR\n");
            return -1;
        }
        skip_len = 0;
    }
    offset = sizeof(struct image_head);

    if(flash_read(p, offset, (void*)raw_header, FLASH_PAGE_SIZE,&skip_len)) {
        cprintf("CANNOT READ BOOT IMAGE HEADER\n");
        return -1;
    }
    offset += FLASH_PAGE_SIZE;
    offset += skip_len;

    if(memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
	        cprintf("INVALID BOOT IMAGE HEADER\n");
        return -1;
    }

    n = (hdr->kernel_size + (FLASH_PAGE_SIZE- 1)) & (~(FLASH_PAGE_SIZE - 1));
    if(flash_read(p, offset, (void*) (hdr->kernel_addr), n, &skip_len)) {
        cprintf("CANNOT READ KERNEL IMAGE\n");
        return -1;
    }
    offset += n;
    offset += skip_len;

    n = (hdr->ramdisk_size + (FLASH_PAGE_SIZE - 1)) & (~(FLASH_PAGE_SIZE - 1));
    if(flash_read(p, offset, (void*)(hdr->ramdisk_addr), n, &skip_len)) {
        cprintf("CANNOT READ RAMDISK IMAGE\n");
        return -1;
    }
    offset += n;

    dprintf("\nkernel  @ %x (%d bytes)\n", hdr->kernel_addr, hdr->kernel_size);
    dprintf("ramdisk @ %x (%d bytes)\n\n\n", hdr->ramdisk_addr, hdr->ramdisk_size);

    if(hdr->cmdline[0]) {
		char* cmdbuffer = NULL;
        cmdbuffer = alloc(CMD_LINE_SIZE);
        cmdline = alloc( strlen((char*)hdr->cmdline) + CMD_LINE_SIZE );

	    cprintf("hdr->cmdline = '%s'\n", (char*)hdr->cmdline);
        memcpy((void*)cmdline, hdr->cmdline, strlen( (char*)hdr->cmdline));
        memcpy((void*)cmdline + strlen((char*)hdr->cmdline ), " ", 1);
        memset(cmdbuffer, 0, CMD_LINE_SIZE);
        if( creat_cmdline( (char*)hdr->cmdline, cmdbuffer ) )
        {
            cprintf("creat_cmdline fail\n");
            cmdline = (const char *)&(hdr->cmdline[0]);
        }
        memcpy((void*)(cmdline + strlen((char*)hdr->cmdline) + 1), cmdbuffer, strlen(cmdbuffer) + 1);
    } else {
        cmdline = board_cmdline();
        if(cmdline == 0) {
            cmdline = "mem=50M console=null";
        }
    }
    cprintf("cmdline = '%s'\n", cmdline);

    cprintf("\nBooting Linux\n");

	tags = create_atags(ADDR_TAGS, cmdline,
                 hdr->ramdisk_addr, hdr->ramdisk_size);

    ddm_phase_boot_score("end boot linux",__LINE__);

    boot_linux_at(hdr->kernel_addr, tags);
    return 0;
}

int boot_recovery(void)
{
    boot_img_hdr *hdr = (void*) raw_header;
    unsigned n;
    ptentry *p =0;
    unsigned offset = 0;
    const char *cmdline;
    unsigned int skip_len = 0;
    unsigned tags;
    char *pname;
    cprintf("boot recovey from flash\n");

    pname=get_recovery_part();
    if(NULL==pname){
	cprintf("FIND PTABLE FAIL\n");
	return -1;
    }

    cprintf("pname:%s\n",pname);
    if((p = flash_find_ptn(pname)) == 0) {
        cprintf("NO BOOT PARTITION\n");
        return -1;
    }

    if(flash_read(p, offset, (void*)raw_header, FLASH_PAGE_SIZE, &skip_len)) {
        cprintf("CANNOT READ BOOT IMAGE HEADER\n");
        return -1;
    }
    offset += FLASH_PAGE_SIZE;
    offset += skip_len;

    if(memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
	        cprintf("INVALID BOOT IMAGE HEADER\n");
        return -1;
    }

    n = (hdr->kernel_size + (FLASH_PAGE_SIZE - 1)) & (~(FLASH_PAGE_SIZE - 1));
    if(flash_read(p, offset, (void*)(hdr->kernel_addr), n, &skip_len)) {
        cprintf("CANNOT READ KERNEL IMAGE\n");
        return -1;
    }
    offset += n;
    offset += skip_len;

    n = (hdr->ramdisk_size + (FLASH_PAGE_SIZE - 1)) & (~(FLASH_PAGE_SIZE - 1));
    if(flash_read(p, offset, (void*)(hdr->ramdisk_addr), n, &skip_len)) {
        cprintf("CANNOT READ RAMDISK IMAGE\n");
        return -1;
    }
    offset += n;

    dprintf("\nkernel  @ %x (%d bytes)\n", hdr->kernel_addr, hdr->kernel_size);
    dprintf("ramdisk @ %x (%d bytes)\n\n\n", hdr->ramdisk_addr, hdr->ramdisk_size);

    if(hdr->cmdline[0]) {
		char* cmdbuffer = NULL;
        cmdbuffer = alloc(CMD_LINE_SIZE);
        cmdline = alloc( strlen((char*)hdr->cmdline) + CMD_LINE_SIZE );

	    cprintf("hdr->cmdline = '%s'\n", (char*)hdr->cmdline);
        memcpy((void*)cmdline, hdr->cmdline, strlen( (char*)hdr->cmdline));
        memcpy((void*)cmdline + strlen((char*)hdr->cmdline ), " ", 1);
        memset(cmdbuffer, 0, CMD_LINE_SIZE);
        if( creat_cmdline( (char*)hdr->cmdline, cmdbuffer ) )
        {
            cprintf("creat_cmdline fail\n");
            cmdline = (const char *)&(hdr->cmdline[0]);
        }
        memcpy((void*)(cmdline + strlen((char*)hdr->cmdline) + 1), cmdbuffer, strlen(cmdbuffer) + 1);
    }else {
        cmdline = board_cmdline();
        if(cmdline == 0) {
            cmdline = "mem=50M console=null";
        }
    }
    cprintf("cmdline = '%s'\n", cmdline);

    cprintf("\nBooting %s\n",p->name);

	tags = create_atags(ADDR_TAGS, cmdline,
                 hdr->ramdisk_addr, hdr->ramdisk_size);

    boot_linux_at(hdr->kernel_addr, tags);
    return 0;
}

int boot_recovery_from_flash(void)
{
	int ret=0;

    /* shut down the usb here, kernel will init usb again */
    usb_shutdown();

	ret=boot_recovery();
	if(0==ret)
	{
		return 0;
	}

	if(modify_recovery_count()){
		cprintf("modify recovery count failed!\n");
		return -1;
	}

	if(boot_recovery()){
		cprintf("the another recovery image is error!\n");
		return -1;
	}

	return 0;

}

void usbloader_init(void);
const char *get_fastboot_version(void);

extern unsigned linux_type;
extern unsigned linux_tags;

static unsigned revision = 0;

char serialno[32];

void dump_smem_info(void);
#if 0
static void tag_dump(unsigned tag, void *data, unsigned sz, void *cookie)
{
    dprintf("tag type=%x data=%x size=%x\n", tag, (unsigned) data, sz);
}

static struct tag_handler tag_dump_handler = {
    .func = tag_dump,
    .type = 0,
};

void xdcc_putc(unsigned x)
{
    while (dcc_putc(x) < 0) ;
}
#endif
#define SERIALNO_STR "androidboot.serialno="
#define SERIALNO_LEN strlen(SERIALNO_STR)

static int boot_from_flash = 1;

void key_changed(unsigned int key, unsigned int down)
{
    if(!down) return;
    if(key == BOOT_KEY_STOP_BOOT) boot_from_flash = 0;
}
#if 0
static int tags_okay(unsigned taddr)
{
    unsigned *tags = (unsigned*) taddr;

    if(taddr != ADDR_TAGS) return 0;
    if(tags[0] != 2) return 0;
    if(tags[1] != 0x54410001) return 0;

    return 1;
}
#endif
extern void console_poll_for_single_char();
void select_uart_or_shell()
{
	u32 ret;
	DRV_UART_SHELL_FLAG nv_stru;
	unsigned int loops = 0;
	const unsigned long uart_array[3][2] = {{HI_UART0_REGBASE_ADDR, INT_LVL_UART0}, {HI_UART1_REGBASE_ADDR, INT_LVL_UART1}, {HI_UART2_REGBASE_ADDR, INT_LVL_UART2}};
	const unsigned long uart_m3_array[3] = {M3_UART0_INT, M3_UART1_INT, M3_UART2_INT};

	ret = bsp_nvm_read(NV_ID_DRV_UART_SHELL_FLAG, (u8 *)&nv_stru, sizeof(DRV_UART_SHELL_FLAG));
	if(ret)//if fail to read nv, set the default value to variable
	{
		nv_stru.wait_usr_sele_uart = 1;
		nv_stru.a_core_uart_num    = 1;
		nv_stru.c_core_uart_num    = 2;
		nv_stru.m_core_uart_num    = 0;
		nv_stru.a_shell            = 0;
		nv_stru.c_shell            = 0;
		nv_stru.uart_at            = 0;
		nv_stru.extendedbits       = 0;
	}

	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_USB_ASHELL = nv_stru.a_shell ? PRT_FLAG_EN_MAGIC_A : 0;
	//((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_USB_CSHELL = nv_stru.c_shell ? PRT_FLAG_EN_MAGIC_M : 0;
	*(u32*)SHM_MEM_CHSELL_FLAG_ADDR = nv_stru.c_shell ? PRT_FLAG_EN_MAGIC_M : 0;
	/*A CORE*/
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[0].flag = UART_USE_FLAG;
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[0].base_addr = uart_array[nv_stru.a_core_uart_num][0];
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[0].interrupt_num = uart_array[nv_stru.a_core_uart_num][1];

	/*C CORE*/
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].flag = 0;
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].base_addr = uart_array[nv_stru.c_core_uart_num][0];
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].interrupt_num = uart_array[nv_stru.c_core_uart_num][1];

	/*M CORE*/
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[2].flag = 0;
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[2].base_addr = uart_array[nv_stru.m_core_uart_num][0];
	((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[2].interrupt_num = uart_m3_array[nv_stru.m_core_uart_num];

//	cprintf("\n%X\n\n %X\n\n", ret, *(unsigned int *)&nv_stru);

	if(nv_stru.uart_at)
	{
		*(u32*)SHM_MEM_AT_FLAG_ADDR = AT_UART_USE_FLAG;
	}else{
		*(u32*)SHM_MEM_AT_FLAG_ADDR = 0;
	}

	if(nv_stru.wait_usr_sele_uart)
	{
		cprintf("Please distribute uart with command L/V/M...\n");
//		boot_register_poll_func(console_poll_for_single_char);
		while(loops++ <= 50)
		{
			console_poll_for_single_char();
		}
	}
}

//#define CPUFREQ_CUR_PROFILE						(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_CUR_CPUFREQ_PROFILE)
//#define CPUFREQ_MAX_PROFILE_LIMIT			(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_MAX_CPUFREQ_PROFILE)
//#define CPUFREQ_MIN_PROFILE_LIMIT			(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_MIN_CPUFREQ_PROFILE)

void save_last_profile(void)
{
    writel(*(u32*)SRAM_MAX_CPUFREQ_PROFILE_ADDR, DUMP_FASTBOOT_ADDR);
    writel(*(u32*)SRAM_MAX_CPUFREQ_PROFILE_ADDR, DUMP_FASTBOOT_ADDR + 4);
    writel(*(u32*)SRAM_MIN_CPUFREQ_PROFILE_ADDR, DUMP_FASTBOOT_ADDR + 8);
}
int _main(unsigned zero, unsigned type, unsigned tags)
{
    int n;

    UNUSED(zero);
    UNUSED(type);
    UNUSED(tags);
    save_last_profile();
    board_init();

	linux_type = board_machtype();
#ifndef BSP_CONFIG_EDA
    cprintf("USB FastBoot:  V%s\n", get_fastboot_version());
    cprintf("Machine ID:    %d v%d\n", linux_type, revision);
    cprintf("Build Date:    "__DATE__", "__TIME__"\n\n");

    cprintf("Serial Number: %s\n\n", serialno[0] ? serialno : "UNKNOWN");

	cprintf("Heap:0x%x -- 0x%x, %d\n",BOOTLOADER_HEAP_ADDR, get_heap_next()
			, get_heap_next()- BOOTLOADER_HEAP_ADDR);
#endif
    ddm_phase_boot_score("end flash_init",__LINE__);

    bsp_pmu_set_by_nv();

	select_uart_or_shell();
    /* scan the keyboard a bit */
    for(n = 0; n < 50; n++) {
        boot_poll();
    }

#ifndef BSP_CONFIG_EDA
	cprintf("    heap:0x%x -- 0x%x, %d\n",BOOTLOADER_HEAP_ADDR, get_heap_next()
			, get_heap_next()- BOOTLOADER_HEAP_ADDR);
#endif
	//uartload_init();

    /* attention: this function will not return if the auto enum flag is set */
    if (!usb_burn_trap()) {
        fastboot_preboot();
    }

    usbloader_init();
    ddm_phase_boot_score("end usbloader_init",__LINE__);
    /* coverity[no_escape] */
    for(;;) {
        boot_poll();
    }
    return 0;
}

