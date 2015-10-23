
#ifndef __BSP_DSP_H__
#define __BSP_DSP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bsp_shared_ddr.h>
#ifdef __VXWORKS__
#include <bsp_dpm.h>
#endif



#define DSP_IMAGE_NAME "DSP"
#define DSP_IMAGE_STATE_OK (0x5312ABCD)

#define HIFI_MEM_BEGIN_CHECK32_DATA (0x55AA55AA)
#define HIFI_MEM_END_CHECK32_DATA   (0xAA55AA55)

#define HIFI_SEC_MAX_NUM            (32)
#define HIFI_DYNAMIC_SEC_MAX_NUM    (HIFI_SEC_MAX_NUM)

#define HIFI_SHARE_MEMORY_ADDR      (SHM_MEM_HIFI_ADDR)
#define HIFI_SHARE_MEMORY_SIZE      (SHM_MEM_HIFI_SIZE)


/*****************************************************************************
 实 体 名  : DRV_HIFI_IMAGE_SEC_TYPE_ENUM
 功能描述  : 镜像段类型
*****************************************************************************/
enum DRV_HIFI_IMAGE_SEC_TYPE_ENUM {
    DRV_HIFI_IMAGE_SEC_TYPE_CODE = 0,        /* 代码 */
    DRV_HIFI_IMAGE_SEC_TYPE_DATA,            /* 数据 */
    DRV_HIFI_IMAGE_SEC_TYPE_BUTT,
};

/*****************************************************************************
 实 体 名  : DRV_HIFI_IMAGE_SEC_LOAD_ENUM
 功能描述  : 镜像段加载属性
*****************************************************************************/
enum DRV_HIFI_IMAGE_SEC_LOAD_ENUM {
    DRV_HIFI_IMAGE_SEC_LOAD_STATIC = 0,      /* 单次加载, 即解复位前加载 */
    DRV_HIFI_IMAGE_SEC_LOAD_DYNAMIC,         /* 多次加载, 由其他机制加载 */
    DRV_HIFI_IMAGE_SEC_UNLOAD,               /* 不需要底软加载 */
    DRV_HIFI_IMAGE_SEC_LOAD_BUTT,
};

/*****************************************************************************
 实 体 名  : DRV_HIFI_IMAGE_SEC_STRU
 功能描述  : 镜像段定义
*****************************************************************************/
struct drv_hifi_image_sec
{
    unsigned short                      sn;              /* 编号 */
    unsigned char                       type;            /* 类型 */
    unsigned char                       load_attib;      /* 加载属性 */
    unsigned long                       src_offset;      /* 在文件中的偏移, bytes */
    unsigned long                       des_addr;        /* 加载目的地址, bytes */
    unsigned long                       size;            /* 段长度, bytes */
};

/*****************************************************************************
 实 体 名  : drv_hifi_image_head
 功能描述  : 镜像文件头定义
*****************************************************************************/
struct drv_hifi_image_head
{
    char                                time_stamp[24]; /* 镜像编译时间 */
    unsigned long                       image_size;     /* 镜像文件大小, bytes */
    unsigned int                        sections_num;   /* 文件包含段数目 */
    struct drv_hifi_image_sec           sections[HIFI_SEC_MAX_NUM];    /* 段信息, 共sections_num个 */
};

/*****************************************************************************
 实 体 名  : drv_hifi_sec_addr_info
 功能描述  : Hifi动态加载段地址结构
*****************************************************************************/
struct drv_hifi_sec_addr
{
    unsigned int  sec_source_addr;	/*段的源地址*/
    unsigned int  sec_length;	    /*段的长度*/
    unsigned int  sec_dest_addr;    /*段的目的地址*/
};

/*****************************************************************************
 实 体 名  : drv_hifi_sec_load_info
 功能描述  : Hifi动态加载段结构
*****************************************************************************/
#define HIFI_SEC_DATA_LENGTH (HIFI_SHARE_MEMORY_SIZE \
                               - HIFI_DYNAMIC_SEC_MAX_NUM*sizeof(struct drv_hifi_sec_addr) \
                               - 3*sizeof(unsigned int))

struct drv_hifi_sec_load_info
{
    unsigned int             sec_magic;         /*段信息开始的保护字*/
    unsigned int             sec_num;           /*段的个数*/
    struct drv_hifi_sec_addr sec_addr_info[HIFI_DYNAMIC_SEC_MAX_NUM]; /*段的地址信息*/
    char                     sec_data[HIFI_SEC_DATA_LENGTH];          /*段信息*/
};

int bsp_bbe_load_muti(void);

int bsp_bbe_run(void);

int bsp_bbe_stop(void);

int bsp_bbe_store(void);

int bsp_bbe_wait_store_ok(void);

int bsp_bbe_restore(void);

int bsp_bbe_wait_restore_ok(void);

int bsp_bbe_clock_enable(void);

int bsp_bbe_clock_disable(void);

int bsp_bbe_unreset(void);

int bsp_bbe_reset(void);

int bsp_bbe_power_on(void);

int bsp_bbe_power_off(void);

int bsp_bbe_is_clock_enable(void);

int bsp_bbe_is_power_on(void);

int bsp_bbe_is_tcm_accessible(void);

int bsp_dsp_clock_enable(void);

int bsp_dsp_clock_disable(void);

int bsp_dsp_unreset(void);

int bsp_dsp_reset(void);

int bsp_dsp_pll_status(void);

int bsp_dsp_pll_enable(void);

int bsp_dsp_pll_disable(void);

void bsp_bbe_chose_pll(u32 flag);

int bsp_msp_bbe_store(void);

int bsp_msp_bbe_restore(void);

int bsp_msp_wait_edma_ok(void);

#ifdef __VXWORKS__
int bsp_bbe_dpm_prepare(struct dpm_device *dev);

int bsp_bbe_dpm_suspend_late(struct dpm_device *dev);

int bsp_bbe_dpm_resume_early(struct dpm_device *dev);

int bsp_bbe_dpm_complete(struct dpm_device *dev);
#endif

int bsp_dsp_is_hifi_exist(void);


int bsp_dsp_info(void);



int bsp_hifi_run(void);

int bsp_hifi_stop(void);

int bsp_hifi_restore(void);

int bsp_load_modem(void);
int bsp_load_modem_dsp(void);

/* the interface below is just for myself */

extern void drv_hifi_fill_mb_info(unsigned int addr);
void bsp_dsp_init(void);



#ifdef __cplusplus
}
#endif

#endif

