

#ifdef __cplusplus
extern "C"
{
#endif
/*lint -save -e537*/
#include <vxWorks.h>
#include <stdlib.h>
#include <logLib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memLib.h>
#include <string.h>
#include "bsp_dump.h"
#include "bsp_nvim.h"
#include "bsp_clk.h"
#include "utrace_balong.h"
/*lint -restore*/
/* ��ʼ����־ */
u32 g_ul_init_flag = false;
/* ���������־ */
u32 g_ul_capt_start_flag = false;
/* ETB���ݵ�����ַ */
u32 g_ul_etb_data_base = 0;
/* ETB���ݷ���ռ䳤�� */
u32 g_ul_etb_data_len = 0;
/* ETB���ݳ��� */
u32 g_ul_etb_len = 0;

void utrace_reg_read(trace_config_enum_u32 config, u32 reg, u32 * value);
void utrace_reg_write(trace_config_enum_u32 config, u32 reg, u32 value);
void utrace_reg_getbits(trace_config_enum_u32 config, u32 reg, u32 pos, u32 bits, u32 * value);
void utrace_reg_setbits(trace_config_enum_u32 config, u32 reg, u32 pos, u32 bits, u32 value);
void utrace_reg_unlock(void);
void utrace_reg_lock(void);
void utrace_stm_config(u32 port);
void utrace_stm_enable(void);
void utrace_stm_disable(void);
void utrace_funnel_config(funnel_port_enum_u32 port);
void utrace_ptm_config(trace_config_enum_u32 index);
void utrace_ptm_enable(trace_config_enum_u32 index);
void utrace_ptm_disable(trace_config_enum_u32 index);
void utrace_etb_config(void);
void utrace_etb_enable(void);
s32  utrace_etb_stop(void);
void utrace_etb_disable(void);
void utrace_etr_config(etr_config_t * config);
void utrace_etr_enable(void);
s32  utrace_etr_disable(void);
s32  utrace_dump_etb(void);
void utrace_capt_config(void);
s32  bsp_utrace_init(void);
void bsp_utrace_stop(void);
void bsp_utrace_suspend(void);
void bsp_utrace_resume(void);
void utrace_show_etb_base(void);

/*****************************************************************************
 �� �� ��  : utrace_reg_read
 ��������  : �Ĵ���������
 �������  : trace_config_enum_u32 config
             u32 reg
 �������  : u32 * value
 �� �� ֵ  : void
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_read(trace_config_enum_u32 config, u32 reg, u32 * value)
{
    /* ��������Ŀ���ȡ��Ӧ�Ĵ��� */
    switch(config)
    {
#ifndef BSP_CONFIG_HI3630 /* V7R2 */
        case TRACE_STM:
            *value = readl(STM_REG_BASE + reg);
            break;

        case TRACE_PTM0:
            *value = readl(PTM0_REG_BASE + reg);
            break;

        case TRACE_ETF:
            *value = readl(ETF_REG_BASE + reg);
            break;

        case TRACE_ETR:
            *value = readl(ETR_REG_BASE + reg);
            break;
#endif
        case TRACE_PTM1:
            *value = readl(PTM1_REG_BASE + reg);
            break;
        
        case TRACE_FUNNEL:
            *value = readl(FUNNEL_REG_BASE + reg);
            break;

        default:
            break;
    }
}

/*****************************************************************************
 �� �� ��  : utrace_reg_write
 ��������  : �Ĵ���д����
 �������  : trace_config_enum_u32 config
             u32 reg
             u32 value
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_write(trace_config_enum_u32 config, u32 reg, u32 value)
{
    /* ��������Ŀ��д����Ӧ�Ĵ��� */
    switch(config)
    {
#ifndef BSP_CONFIG_HI3630 /* V7R2 */
        case TRACE_STM:
            writel(value, STM_REG_BASE + reg);
            break;

        case TRACE_PTM0:
            writel(value, PTM0_REG_BASE + reg);
            break;

        case TRACE_ETF:
            writel(value, ETF_REG_BASE + reg);
            break;

        case TRACE_ETR:
            writel(value, ETR_REG_BASE + reg);
            break;
#endif
        case TRACE_PTM1:
            writel(value, PTM1_REG_BASE + reg);
            break;
        
        case TRACE_FUNNEL:
            writel(value, FUNNEL_REG_BASE + reg);
            break;

        default:
            break;
    }
}

/*****************************************************************************
 �� �� ��  : utrace_reg_getbits
 ��������  : �Ĵ�����λ����
 �������  : trace_config_enum_u32 config
             u32 reg
             u32 pos
             u32 bits
 �������  : u32 * value
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_getbits(trace_config_enum_u32 config, u32 reg, u32 pos, u32 bits, u32 * value)
{
    u32 reg_value = 0;

    /* ��������Ŀ���ȡ��Ӧ�Ĵ��� */
    utrace_reg_read(config, reg, &reg_value);
    /* ���ݼĴ���ֵȡ��Ӧλ */
    *value = (reg_value >> pos) & (((u32)1 << (bits)) - 1);
}

/*****************************************************************************
 �� �� ��  : utrace_reg_setbits
 ��������  : �Ĵ���дλ����
 �������  : trace_config_enum_u32 config
             u32 reg
             u32 pos
             u32 bits
             u32 value
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_setbits(trace_config_enum_u32 config, u32 reg, u32 pos, u32 bits, u32 value)
{
    u32 reg_value = 0;

    /* ��������Ŀ���ȡ��Ӧ�Ĵ��� */
    utrace_reg_read(config, reg, &reg_value);
    /* ����д��Ĵ�����Ŀ��ֵ */
    reg_value = (reg_value & (~((((u32)1 << (bits)) - 1) << (pos)))) | ((u32)((value) & (((u32)1 << (bits)) - 1)) << (pos));
    /* д��Ŀ�ļĴ��� */
    utrace_reg_write(config, reg, reg_value);
}

/*****************************************************************************
 �� �� ��  : utrace_funnel_config
 ��������  : funnel���ýӿ�
 �������  : funnel_port_enum_u32 port --- ��Ҫʹ�ܵĶ˿�
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_funnel_config(funnel_port_enum_u32 port)
{
    /* ʹ��funnel��Ӧ�˿� */
    utrace_reg_setbits(TRACE_FUNNEL, FUNNEL_CTRL, port, 1, 1);
}

/*****************************************************************************
 �� �� ��  : utrace_ptm_config
 ��������  : PTM���ýӿ�
 �������  : trace_config_enum_u32 index --- APPA9 PTM or MODMEA9 PTM
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_ptm_config(trace_config_enum_u32 ptm_index)
{
    /* ���PowerDown */
    utrace_reg_setbits(ptm_index, PTM_CTRL, 0, 1, 0x0);
    /* ����Program Bit, bit10 */
    utrace_reg_setbits(ptm_index, PTM_CTRL, 10, 1, 0x1);
    /* ����Trace ID */
    utrace_reg_write(ptm_index, PTM_TRACEID, ptm_index + 1);
#if 0
    /* ���õ�ַ�� */
    utrace_reg_write(ptm_index, PTM_ACVR(0), DDR_SOCP_ADDR);
    utrace_reg_write(ptm_index, PTM_ACVR(1), DDR_SOCP_ADDR + DDR_SOCP_SIZE);
#endif
    /* ���õ�ַ�������� */
    utrace_reg_write(ptm_index, PTM_ACTR(0), 0x1);
    utrace_reg_write(ptm_index, PTM_ACTR(1), 0x1);
    /* ����TraceEnable Event(�������) */
    utrace_reg_write(ptm_index, PTM_TEEVR, 0x376f);
    /* ����TraceEnable������(�������) */
    utrace_reg_write(ptm_index, PTM_TECR, 0x01000000);
    /* ����Trigger Event */
    utrace_reg_write(ptm_index, PTM_TRIGGER, 0x406F);
    /* ����Timestamp Event */
    utrace_reg_write(ptm_index, PTM_TSEVR, 0x406F);
    /* ȥʹ��Timestamp */
    utrace_reg_setbits(ptm_index, PTM_CTRL, 28, 1, 0x0);
    /* ����Context ID, Bit14:15 */
    utrace_reg_setbits(ptm_index, PTM_CTRL, 14, 2, 0x3);
    /* ����Sync Frequency�Ĵ��� */
    utrace_reg_write(ptm_index, PTM_SYNCFR, 0x400);
}

/*****************************************************************************
 �� �� ��  : utrace_ptm_enable
 ��������  : PTMʹ��
 �������  : trace_config_enum_u32 index --- APPA9 PTM or MODMEA9 PTM
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_ptm_enable(trace_config_enum_u32 ptm_index)
{
    utrace_reg_setbits(ptm_index, PTM_CTRL, 10, 1, 0x0);
}

/*****************************************************************************
 �� �� ��  : utrace_ptm_disable
 ��������  : PTMȥʹ��
 �������  : trace_config_enum_u32 index --- APPA9 PTM or MODMEA9 PTM
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_ptm_disable(trace_config_enum_u32 ptm_index)
{
    utrace_reg_setbits(ptm_index, PTM_CTRL, 10, 1, 0x1);
}

#ifndef BSP_CONFIG_HI3630 /* V7R2 */
/*****************************************************************************
 �� �� ��  : utrace_reg_unlock
 ��������  : STM, FUNNEL, ETF, ETR���üĴ���Ĭ��Ϊ����״̬����Ҫ�ڳ�ʼ��ʱ���н���
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_unlock(void)
{
    /* PTM1���� */
    utrace_reg_write(TRACE_PTM1, PTM_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* FUNNEL���� */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* ETF���� */
    utrace_reg_write(TRACE_ETF, ETF_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* ETR���� */
    utrace_reg_write(TRACE_ETR, ETR_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
}

/*****************************************************************************
 �� �� ��  : utrace_reg_lock
 ��������  : STM, FUNNEL, ETF, ETR�������֮������
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_lock(void)
{
    /* PTM1���� */
    utrace_reg_write(TRACE_PTM1, PTM_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* FUNNEL���� */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* ETF���� */
    utrace_reg_write(TRACE_ETF, ETF_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* ETR���� */
    utrace_reg_write(TRACE_ETR, ETR_LOCK_ACCESS, UTRACE_LOCK_CODE);
}

/*****************************************************************************
 �� �� ��  : utrace_stm_config
 ��������  : STM���ýӿ�
 �������  : u32 port --- ���ö˿�
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_stm_config(u32 port)
{
    /* ATB Triggerʹ�� */
    utrace_reg_write(TRACE_STM, STM_TRIGGER_CTRL, 0x8);
    /* stimulus portʹ�� */
    utrace_reg_setbits(TRACE_STM, STM_PORT_ENABLE, port, 1, (u32)1 << port);
    /* stimulus trigger portʹ�� */
    utrace_reg_setbits(TRACE_STM, STM_PORT_TRIGGER_ENABLE, port, 1, (u32)1 << port);
    /* Trace ID���� */
    utrace_reg_setbits(TRACE_STM, STM_TRACE_CTRL, 16, 7, TRACE_STM + 1);
}

/*****************************************************************************
 �� �� ��  : utrace_stm_enable
 ��������  : STMʹ��
 �������  :
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_stm_enable(void)
{
    utrace_reg_setbits(TRACE_STM, STM_TRACE_CTRL, 0, 1, 1);
}

/*****************************************************************************
 �� �� ��  : utrace_stm_disable
 ��������  : STMȥʹ��
 �������  :
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_stm_disable(void)
{
    utrace_reg_setbits(TRACE_STM, STM_TRACE_CTRL, 0, 1, 0);
}

/*****************************************************************************
 �� �� ��  : utrace_etb_config
 ��������  : ETB���ýӿڣ�����ETB��������
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_etb_config(void)
{
    /* ����ΪETF��ӲFIFOģʽ */
    //utrace_reg_setbits(TRACE_ETF, ETF_MODE, 0, 2, TMC_MODE_HARDWARE_FIFO);
    /* ����ΪETB��ѭ��buffer */
    utrace_reg_setbits(TRACE_ETF, ETF_MODE, 0, 2, TMC_MODE_CIRCULAR_BUFF);
    /* ����FFCR EnTl(bit1), EnFt(bit0) */
    utrace_reg_setbits(TRACE_ETF, ETR_FORMAT_FLUSH_CTRL, 0, 2, 0x3);
    /* ����ˮ�� */
    utrace_reg_write(TRACE_ETF, ETF_BUF_WATER_MARK, 0x3);
}

/*****************************************************************************
 �� �� ��  : utrace_etr_config
 ��������  : ETR���ýӿڣ�����ETR��������
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_etr_config(etr_config_t * config)
{
    /* ����ETR DDR����ռ��С */
    utrace_reg_write(TRACE_ETR, ETR_RAM_SIZE, (config->buf_size)/4);
    /* ����ETR����ģʽ */
    utrace_reg_setbits(TRACE_ETR, ETF_MODE, 0, 2, config->mode);
    /* ����FFCR EnTl(bit1), EnFt(bit0) */
    utrace_reg_setbits(TRACE_ETR, ETR_FORMAT_FLUSH_CTRL, 0, 2, 0x3);
    /* ����ˮ�� */
    utrace_reg_write(TRACE_ETR, ETF_BUF_WATER_MARK, UTRACE_WATER_MARK);
    /* ����DDR��ַ */
    utrace_reg_write(TRACE_ETR, ETR_DATA_BUF_ADDR, config->buf_addr);
}

/*****************************************************************************
 �� �� ��  : utrace_etb_enable
 ��������  : ETBʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_etb_enable(void)
{
    utrace_reg_write(TRACE_ETF, ETR_CTRL, 0x1);
}

/*****************************************************************************
 �� �� ��  : utrace_etb_stop
 ��������  : ETBֹͣ�ɼ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
s32 utrace_etb_stop(void)
{
    int i;
    u32 reg_value;

    /* FFCR StopOnFl */
    utrace_reg_setbits(TRACE_ETF, ETF_FORMAT_FLUSH_CTRL, 12, 1, 1);
    /* FFCR FlushMem */
    utrace_reg_setbits(TRACE_ETF, ETF_FORMAT_FLUSH_CTRL, 6, 1, 1);

    /* �ȴ�TMCReady */
    for(i=0; i<MAX_WAIT_CNT; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_STATUS, &reg_value);
        /* bit2ΪTMCReadyָʾλ */
        if(0 != (reg_value & 0x4))
        {
            break;
        }
    }

    /* ��ʱ�ж� */
    if(i >= MAX_WAIT_CNT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: stop etb timeout\n", __FUNCTION__);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : utrace_etb_disable
 ��������  : ETBȥʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_etb_disable(void)
{
    /* ȥʹ��ETB */
    utrace_reg_write(TRACE_ETF, ETF_CTRL, 0);
}

/*****************************************************************************
 �� �� ��  : utrace_etr_enable
 ��������  : ETRʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_etr_enable(void)
{
    utrace_reg_write(TRACE_ETR, ETR_CTRL, 0x1);
}

/*****************************************************************************
 �� �� ��  : utrace_etr_disable
 ��������  : ETRȥʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : s32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
s32 utrace_etr_disable(void)
{
    int i;
    u32 reg_value;

    /* FFCR StopOnFl */
    utrace_reg_setbits(TRACE_ETR, ETR_FORMAT_FLUSH_CTRL, 12, 1, 1);
    /* FFCR FlushMem */
    utrace_reg_setbits(TRACE_ETR, ETR_FORMAT_FLUSH_CTRL, 6, 1, 1);
    /* �ȴ�TMCReady */
    for(i=0; i<MAX_WAIT_CNT; i++)
    {
        utrace_reg_read(TRACE_ETR, ETR_STATUS, &reg_value);
        /* bit2ΪTMCReadyָʾλ */
        if(0 != (reg_value & 0x4))
        {
            break;
        }
    }

    /* ��ʱ�ж� */
    if(i >= MAX_WAIT_CNT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: stop etr timeout\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* ȥʹ��ETR */
    utrace_reg_write(TRACE_ETR, ETR_CTRL, 0);

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : utrace_dump_etb
 ��������  : ϵͳ�쳣��ֹͣTrace�ɼ���������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
s32 utrace_dump_etb(void)
{
    u32     reg_value;
    u32     i;
    u32  *  data;

    utrace_reg_unlock();
    
    /* ֹͣETB */
    if(BSP_OK != utrace_etb_stop())
    {
        utrace_reg_lock();
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: stop etb fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
#if 0
    /* �����ݶ��գ�����ETB��дָ����� */
    for(i=0; i<UTRACE_ONSTART_BUF_SIZE/4; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_RAM_RD_DATA, &reg_value);
        if(reg_value == 0xffffffff)
        {
            break;
        }                
    }

    utrace_etb_disable();
#endif    
    memset((void *)g_ul_etb_data_base, 0x0, (unsigned int)g_ul_etb_data_len);
    data = (u32 *)(g_ul_etb_data_base + 8);
    for(i=0; i<UTRACE_ONSTART_BUF_SIZE/4; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_RAM_RD_DATA, &reg_value);
        *data = reg_value;
        data++;
        if(reg_value == 0xffffffff)
        {
            break;
        }           
    }

    /* 0-3�ֽڴ�ű�ʶ�� */
    *((u32 *)g_ul_etb_data_base) = (u32)UTRACE_MAGIC_NUM;
    /* 4-7���ֽڴ��ETB���ݳ��� */
    *((u32 *)g_ul_etb_data_base + 1) = i*4; /* [false alarm]:����Fority���� */

    printf("%s: dump etb data ok(dst_buf addr: 0x%x)\n", __FUNCTION__, g_ul_etb_data_base);
    
    utrace_reg_lock();

    return BSP_OK;
}


/*****************************************************************************
 �� �� ��  : utrace_capt_config
 ��������  : Trace�ɼ������ýӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_capt_config(void)
{
    utrace_reg_unlock();
    utrace_etb_disable();
    utrace_etb_config();
    utrace_etb_enable();
    utrace_funnel_config(FUNNEL_PTM_MODEMA9_PORT);
    utrace_ptm_disable(TRACE_PTM1);
    utrace_ptm_config(TRACE_PTM1);
    utrace_ptm_enable(TRACE_PTM1);
    utrace_reg_lock();
}

/*****************************************************************************
 �� �� ��  : bsp_utrace_init
 ��������  : ��ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : s32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
s32 bsp_utrace_init(void)
{
    dump_nv_s    dump_nv;
    char *       dst_buf;
    u32          len;
    struct clk * cs_clk;

    if(true == g_ul_init_flag)
    {
        return BSP_OK;
    }

	cs_clk = clk_get(NULL, "cs_clk");
	if(BSP_OK != clk_enable(cs_clk))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: CoreSight clk open failed.\n", __FUNCTION__);
		return BSP_ERROR;
	}

    /* ��ȡ�쳣������ */
    if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_UTRACE, &dst_buf, &len))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  get dump buffer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
    if(len < UTRACE_ONSTART_BUF_SIZE)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  dump buffer(0x%x) less than data buffer(0x%x)\n", __FUNCTION__, len, UTRACE_ONSTART_BUF_SIZE);
        return BSP_ERROR;
    }
    g_ul_etb_data_base = (u32)dst_buf;
    g_ul_etb_data_len  = len;

    /* ͨ��NV���Ƽ���Ƿ����� */
    if(BSP_OK != bsp_nvm_read(NVID_DUMP, (u8 *)&dump_nv, sizeof(dump_nv_s)))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  read nv %d fail\n", __FUNCTION__, NVID_DUMP);
        return BSP_ERROR;
    }

    /* ���Modem A9 */
    if((1 == dump_nv.traceOnstartFlag) && (MODEMA9_ONLY == dump_nv.traceCoreSet))
    {
        g_ul_capt_start_flag = true;
        utrace_capt_config();
    }    
    else
    {
        g_ul_init_flag = true;
        return BSP_OK;
    }

    /* EXC DUMPע�� */
    if(bsp_dump_register_hook(DUMP_SAVE_MOD_UTRACE, (dump_save_hook)utrace_dump_etb) != BSP_OK)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  dump register fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
    
    g_ul_init_flag = true;

    printf("utrace init ok\n");
    
    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : bsp_utrace_stop
 ��������  : ����ӿڣ�ֹͣTrace�ɼ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void bsp_utrace_stop(void)
{
#ifndef BSP_CONFIG_HI3630 /* V7R2 */
    if(true == g_ul_capt_start_flag)
    {
        utrace_reg_unlock();
        utrace_ptm_disable(TRACE_PTM1);
        utrace_reg_lock();
    }
#endif
}

/*****************************************************************************
 �� �� ��  : utrace_show_etb_base
 ��������  : debug�ӿڣ���ȡETB���ݴ�Ż���ַ
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_show_etb_base(void)
{   
    printf("etb base addr: 0x%x len: 0x%x\n", g_ul_etb_data_base, g_ul_etb_data_len);
}

/*****************************************************************************
 �� �� ��  : bsp_utrace_suspend
 ��������  : �͹������̣�ȥʹ��Trace�ɼ�������ETB����
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void bsp_utrace_suspend(void)
{
#if 0    
    int i;
    u32 reg_value;
#endif
    if(false == g_ul_capt_start_flag)
    {
        return;
    }

    utrace_reg_unlock();
    utrace_ptm_disable(TRACE_PTM1);
    (void)utrace_etb_stop();
    utrace_etb_disable();
#if 0    
    for(i=0; i<UTRACE_ONSTART_BUF_SIZE/4; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_RAM_RD_DATA, &reg_value);
        if(reg_value == 0xffffffff)
        {
            break;
        }
        *((u32 *)g_ul_etb_data_base + i) = reg_value;
    }

    g_ul_etb_len = i;
#endif    
    utrace_reg_lock();
}

/*****************************************************************************
 �� �� ��  : bsp_utrace_resume
 ��������  : �͹������̣��ָ�Trace�ɼ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void bsp_utrace_resume(void)
{
#if 0    
    int i;
    u32 reg_value;
#endif
    if(false == g_ul_capt_start_flag)
    {
        return;
    }
    
    utrace_reg_unlock();
    utrace_etb_disable();
    utrace_etb_config();
#if 0
    for(i=0; i<(int)g_ul_etb_len; i++)
    {
        reg_value = *((u32 *)g_ul_etb_data_base + i);
        utrace_reg_write(TRACE_ETF, ETF_RAM_WR_DATA, reg_value);
    }
#endif    
    utrace_etb_enable();
    utrace_funnel_config(FUNNEL_PTM_MODEMA9_PORT);
    utrace_ptm_disable(TRACE_PTM1);
    utrace_ptm_config(TRACE_PTM1);
    utrace_ptm_enable(TRACE_PTM1);

    utrace_reg_lock();
}

#else
/*****************************************************************************
 �� �� ��  : utrace_reg_unlock
 ��������  : STM, FUNNEL, ETF, ETR���üĴ���Ĭ��Ϊ����״̬����Ҫ�ڳ�ʼ��ʱ���н���
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_unlock(void)
{
    /* PTM1���� */
    utrace_reg_write(TRACE_PTM1, PTM_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* FUNNEL���� */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
}

/*****************************************************************************
 �� �� ��  : utrace_reg_lock
 ��������  : STM, FUNNEL, ETF, ETR�������֮������
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void utrace_reg_lock(void)
{
    /* PTM1���� */
    utrace_reg_write(TRACE_PTM1, PTM_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* FUNNEL���� */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_LOCK_CODE);
}

s32 bsp_utrace_init(void)
{
    dump_nv_s    dump_nv;

    if(true == g_ul_init_flag)
    {
        return BSP_OK;
    }

    /* ͨ��NV���Ƽ���Ƿ����� */
    if(BSP_OK != bsp_nvm_read(NVID_DUMP, (u8 *)&dump_nv, sizeof(dump_nv_s)))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  read nv %d fail\n", __FUNCTION__, NVID_DUMP);
        return BSP_ERROR;
    }
    
    /* ���Modem A9 */
    if((1 == dump_nv.traceOnstartFlag) && (MODEMA9_ONLY == dump_nv.traceCoreSet))
    {
        g_ul_capt_start_flag = true;
    }
    
    g_ul_init_flag = true;

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : bsp_utrace_suspend
 ��������  : �͹������̣�ȥʹ��Trace�ɼ�������ETB����

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void bsp_utrace_suspend(void)
{
    if(false == g_ul_capt_start_flag)
    {
        return;
    }

    utrace_reg_unlock();
    utrace_ptm_disable(TRACE_PTM1);
    utrace_reg_lock();
}


/*****************************************************************************
 �� �� ��  : bsp_utrace_resume
 ��������  : �͹������̣��ָ�Trace�ɼ�

 �޸���ʷ      :
  1.��    ��   : 2013��3��26��
    ��    ��   : f
    �޸�����   : �����ɺ���

*****************************************************************************/
void bsp_utrace_resume(void)
{
    if(false == g_ul_capt_start_flag)
    {
        return;
    }
    
    utrace_reg_unlock();
    utrace_funnel_config(FUNNEL_PTM_MODEMA9_PORT);
    utrace_ptm_disable(TRACE_PTM1);
    utrace_ptm_config(TRACE_PTM1);
    utrace_ptm_enable(TRACE_PTM1);
    utrace_reg_lock();
}

void bsp_utrace_stop(void)
{
    bsp_utrace_suspend();
}

#endif

#ifdef __cplusplus
}
#endif
