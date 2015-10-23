#include <osl_types.h>
#include <linux/dma-mapping.h>
#include <bsp_edma.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef __EMI_BALONG__
#define __EMI_BALONG__

#define EMI_BUF_WRT_CMD         0           /*读出/写入命令,相对EMI_BUF_BASE_ADDR偏移地址*/
#define EMI_WRT_DATA_8BIT       0x01        /*读出/写入数据, 8-bit,相对EMI_BUF_BASE_ADDR偏移地址*/
#define EMI_WRT_DATA_16BIT      0x02        /*读出/写入数据, 16-bit,相对EMI_BUF_BASE_ADDR偏移地址*/


#if 0
#define emiCmdOut8(cmd)		(iowrite8(cmd, HI_EMIMEM_REGBASE_ADDR_VIRT + EMI_BUF_WRT_CMD))

#define emiDataOut8(data)		(iowrite8(data, HI_EMIMEM_REGBASE_ADDR_VIRT + EMI_WRT_DATA_8BIT))

#define emiCmdIn8()		(ioread8(HI_EMIMEM_REGBASE_ADDR_VIRT + EMI_BUF_WRT_CMD))

#define emiDataIn8()		(ioread8(HI_EMIMEM_REGBASE_ADDR_VIRT + EMI_WRT_DATA_8BIT))
#endif

/* 时序选择*/
typedef enum tag_emi_scdul_mode_e
{
    EMI_TFT_8BIT       = 0,         /* TFT I8080时序,8-bit数据格式*/
    EMI_TFT_9BIT,                   /* TFT I8080时序,9-bit数据格式*/
    EMI_STN_8BIT_M6800,             /* STN M6800时序,8-bit数据格式*/
    EMI_STN_8BIT_I8080,             /* LCD I8080时序,8-bit数据格式*/
    EMI_LCD_8BIT,                  /* LCD M6800时序,8-bit数据格式*/
    EMI_SCHEDULING_BUTT
}emi_scdul_mode_e;

/* 低功耗: 寄存器备份、恢复 */
typedef struct
{
    u32     smbidcyr0;
    u32     smbwst1r0;
    u32     smbwst2r0;
    u32     smbwstoenr0;
    u32     smbwstwenr0;
    u32     smbcr0;
    u32     smbsr0;
}emi_reg_t;
#if 0
typedef struct
{
    unsigned int     smbidcyr0;
    unsigned int     smbwst1r0;
    unsigned int     smbwst2r0;
    unsigned int     smbwstoenr0;
    unsigned int     smbwstwenr0;
    unsigned int     smbcr0;
    unsigned int     smbsr0;
}emi_reg_t;
#endif

void emi_reg_read(u32 reg, u32* value);
void emi_reg_write(u32 reg, u32 value);
void emi_reg_show(u32 reg);
s32 emi_config(emi_scdul_mode_e EMI_MODE);
void emi_enable_clk(void);
void emi_disable_clk(void);
void emiCmdOut8(u8 cmd);
void emiDataOut8(u8 data);
u8 emiCmdIn8(void);
u8 emiDataIn8(void);
s32 emi_edma_transfer(dma_addr_t src,unsigned long len);

#endif

#ifdef __cplusplus
}
#endif