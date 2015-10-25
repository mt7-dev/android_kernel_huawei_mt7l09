

#ifndef _SECURE_IMAGE_H_
#define _SECURE_IMAGE_H_

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#define EMMC_BLOCK_SIZE         512


#define PTN_VRL_P_NAME			    "block2mtd: /dev/block/mmcblk0p1"
#define PTN_VRL_M_NAME			    "block2mtd: /dev/block/mmcblk0p2"
#define PTN_MODEM_NAME			    "block2mtd: /dev/block/mmcblk0p26"
#define PTN_MODEM_DSP_NAME			"block2mtd: /dev/block/mmcblk0p27"

#define CCORE_IMAGE_NAME            "VXWORKS"
#define DSP_IMAGE_NAME              "DSP"

#define SECBOOT_MODEM_VRL_OFFSET    27

#define SECBOOT_PTN_VRL_P_START		(128 * 2)
#define SECBOOT_PTN_VRL_M_START		(256 * 2)
#define SECBOOT_PTN_VRLP_SIZE		(128 * 2)
#define SECBOOT_PTN_VRLM_SIZE		(128 * 2)

#define SECBOOT_PTN_MODEM_START		(212*1024*2)
#define SECBOOT_PTN_MODEM_SIZE		(60*1024*2)

#define MODEM_TDS_SIZE		  		(0x40000)		/* 0.25 MB*/
#define MODEM_DSP_SIZE				(0x180000)		/* 1.5 MB*/
	
/*primary vrl + secondary vrl = 4k*/
#define SECBOOT_VRL_SIZE		(0x1000)
#define SECBOOT_VRL_MAXNUM	    32

#define sec_print_err(fmt, ...)   (printk(KERN_ERR "[sec]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

#define SEC_OK      			(0)
#define SEC_ERROR  				(-1)

struct modem_head
{
	unsigned int image_length;
    unsigned int image_addr;
};

s32 load_modem_image(void);
	
#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif /* _SECURE_IMAGE_H_ */

