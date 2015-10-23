/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <vxWorks.h>
#include <intLib.h>
#include <logLib.h>
#include <memLib.h>
#include "semLib.h"
#include "drv_audio.h"
#include "bsp_audio.h"
#include "audio_test.h"
#include "bsp_sio.h"
#include "soc_interrupts.h"
#include "hi_sio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* DMA通道中断处理函数指针数组, 由DRV_DMA_RegisterIsr赋值 */
DRV_DMA_CXISR_STRU g_astDrvDmaCxIntIsr[DRV_DMA_MAX_CHANNEL_NUM];

signed short   g_mic_in_audio_data_a[160] = {0};
signed short   g_mic_in_audio_data_b[160] = {0};
signed short   g_spk_out_audio_data_a[160] = {0};
signed short   g_spk_out_audio_data_b[160] = {0};

int   *g_psMicPcm = NULL;
int   *g_psSpkPcm = NULL;

unsigned short *record_buff = NULL;
unsigned short *record_buff_pos = NULL;
unsigned short *play_buff_pos = NULL;
unsigned int work_mode = DRV_SIO_MODE_PCM;

DRV_DMA_CXCFG_STRU g_mic_in_dma_cfg[2];
DRV_DMA_CXCFG_STRU g_spk_out_dma_cfg[2];

/*****************************************************************************
  3 函数实现
*****************************************************************************/
void micin_isr(drv_dma_int_type enIntType, unsigned int uwPara)
{
	unsigned int ulDestAddr, ulDestAddrB;
	ulDestAddrB = (unsigned int)&g_mic_in_audio_data_b[0];

	ulDestAddr = DMA_REG_READ(DRV_DMA_CX_DES_ADDR(15));

	if((ulDestAddr >= (ulDestAddrB - sizeof(unsigned int)))
	   &&(ulDestAddr < (ulDestAddrB + (((unsigned int)(int)160 * sizeof(signed short))
	   - sizeof(unsigned int)))))
	   {
	        g_psMicPcm = (int*)g_mic_in_audio_data_a;
	   }
	   else
	   {
	        g_psMicPcm = (int*)g_mic_in_audio_data_b;
	   }

}

void spkout_isr(drv_dma_int_type enIntType, unsigned int uwPara)
{
	unsigned int ulSrcAddr, ulSrcAddrB;

	ulSrcAddrB = (unsigned int)&g_spk_out_audio_data_b[0];

	ulSrcAddr = DMA_REG_READ(DRV_DMA_CX_SRC_ADDR(14));

	if((ulSrcAddr >= (ulSrcAddrB - sizeof(unsigned int)))
	   &&(ulSrcAddr < (ulSrcAddrB + (((unsigned short)160 * sizeof(signed short))
	   - sizeof(unsigned int)))))
	   {
	        g_psSpkPcm = (int*)g_spk_out_audio_data_a;
	   }
	   else
	   {
	        g_psSpkPcm = (int*)g_spk_out_audio_data_b;
	   }

}

/* porting验证复用配置 */
void mux_cfg(void)
{
    unsigned int tmp = 0;

    tmp = *(volatile int *)0x9001000c;
    tmp &= ~(0xfc0);
    *(volatile int *)0x9001000c = tmp;

    tmp = *(volatile int *)0x90010410;
    tmp |= (0x18);
    *(volatile int *)0x90010410 = tmp;
}

/* 1:I2S,0:PCM */
unsigned int audio_test_init(int mode, int chn_num)
{
    int ret = BSP_OK;
    int fd = NULL;
    int tmp = IOCTL_AUDIO_PCM_MODE_SET;

#if 0
    mux_cfg();

    /*open audio dev*/
    ret = audio_create("/dev/codec0", 0);
    if (BSP_OK != ret)
    {
        logMsg("\r audio_create  fail, result = 0x%x\n",ret,0,0,0,0,0);
        return BSP_ERROR;
    }
#endif
    ret = audio_open("/dev/codec0", 0, 0);
    if (BSP_OK != ret)
    {
        logMsg("\r audio_create  fail, result = 0x%x\n",ret,0,0,0,0,0);
        return BSP_ERROR;
    }

    tmp = mode?(IOCTL_AUDIO_I2S_MODE_SET):(IOCTL_AUDIO_PCM_MODE_SET);
    /* audio_ioctl(fd, IOCTL_AUDIO_PCM_MODE_SET, 0); */
    audio_ioctl(fd, tmp, 0);

    audio_ioctl(fd, IOCTL_AUDIO_SAMPLE_RATE_SET, (AUDIO_SAMPLE_RATE)SAMPLE_RATE_8K);
    audio_ioctl(fd, IOCTL_AUDIO_VOICE_OPEN, NULL);

    audio_ioctl(fd, IOCTL_AUDIO_IN_DEV_SELECT, HEADSET);
    audio_ioctl(fd, IOCTL_AUDIO_OUT_DEV_SELECT, HEADSET);

    /* sio 初始化 */
    ret = bsp_sio_init();
    if (BSP_OK != ret)
    {
        logMsg("\r bsp_sio_init  fail, result = 0x%x\n",ret,0,0,0,0,0);
        return BSP_ERROR;
    }

    work_mode= mode;
    tmp = mode?(DRV_SIO_MODE_I2S):(DRV_SIO_MODE_PCM);
    /* bsp_sio_set_mode(DRV_SIO_MODE_I2S); */
    bsp_sio_set_mode(tmp);

    bsp_sio_set_chn(chn_num);

    ret = bsp_sio_open(DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER|DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER, 0, 0);
    if (BSP_OK != ret)
    {
        logMsg("\r bsp_sio_init  fail, result = 0x%x\n",ret,0,0,0,0,0);
        return BSP_ERROR;
    }

    /* dma 初始化 */
    bsp_dma_init();

}

int audio_record_isr(drv_dma_int_type enIntType, unsigned int uwPara)
{
    int uwRet;
    DRV_DMA_CXCFG_STRU   stCfg;
    unsigned int tmp_reg = 0;

    if(DRV_DMA_INT_TYPE_TC1 != enIntType)
    {
        logMsg("drv_dma_int_type: 0x%x\n",enIntType,0,0,0,0,0);
        return -2;
    }
    record_buff_pos= record_buff_pos + 160;

    tmp_reg = work_mode?(HI_SIO_I2S_DUAL_RX_CHN_OFFSET):(HI_SIO_PCM_RD_OFFSET);

    memset(&stCfg, 0, sizeof(DRV_DMA_CXCFG_STRU));

    if((record_buff_pos - record_buff) >= 0x200000 / 4)
        return -1;

    stCfg.uhwACount = 320;

    stCfg.uwSrcAddr = HI_SIO_REG_ADDR(tmp_reg);
    /* stCfg.uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_PCM_RD_OFFSET); */

    stCfg.uwDstAddr = (unsigned int)record_buff_pos;
    stCfg.uwConfig  = DRV_DMA_SIO_MEM_CFG;

    uwRet = bsp_dma_startwithcfg(15,
              &stCfg,
              audio_record_isr,
              0);

    return uwRet;
}

int audio_play_isr(drv_dma_int_type enIntType, unsigned int uwPara)
{
    int uwRet;
    DRV_DMA_CXCFG_STRU   stCfg = {0};
    unsigned int tmp_reg = 0;

    if(DRV_DMA_INT_TYPE_TC1 != enIntType)
    {
        logMsg("drv_dma_int_type: 0x%x\n",enIntType,0,0,0,0,0);
        return -2;
    }

    play_buff_pos= play_buff_pos + 160;

    tmp_reg = work_mode?(HI_SIO_I2S_DUAL_TX_CHN_OFFSET):(HI_SIO_PCM_XD_OFFSET);

    memset(&stCfg, 0, sizeof(DRV_DMA_CXCFG_STRU));

    if((play_buff_pos - record_buff) >= 0x200000 / 4)
        return -1;

    stCfg.uhwACount = 320;
    stCfg.uwSrcAddr = (unsigned int)play_buff_pos;
    /* stCfg.uwDstAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_DUAL_TX_CHN_OFFSET); */
    stCfg.uwDstAddr = HI_SIO_REG_ADDR(tmp_reg);
    stCfg.uwConfig  = DRV_DMA_MEM_SIO_CFG;

    uwRet = bsp_dma_startwithcfg(14,
              &stCfg,
              audio_play_isr,
              0);

    return uwRet;
}

unsigned int audio_play_test()
{
    unsigned int uwRet;
    DRV_DMA_CXCFG_STRU                  stCfg;
    unsigned int tmp_reg = 0;

    memset(&stCfg, 0, sizeof(DRV_DMA_CXCFG_STRU));
    play_buff_pos = record_buff;

    tmp_reg = work_mode?(HI_SIO_I2S_DUAL_TX_CHN_OFFSET):(HI_SIO_PCM_XD_OFFSET);

    stCfg.uhwACount = 320;
    stCfg.uwSrcAddr = (unsigned int)play_buff_pos;

    stCfg.uwDstAddr = HI_SIO_REG_ADDR(tmp_reg);
    /* stCfg.uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_PCM_XD_OFFSET); */
    stCfg.uwConfig  = DRV_DMA_MEM_SIO_CFG;

    uwRet = bsp_dma_startwithcfg(14,
              &stCfg,
              audio_play_isr,
              0);
    return uwRet;
}

int audio_record_test()
{
    unsigned int uwRet;
    DRV_DMA_CXCFG_STRU                  stCfg;
    unsigned int tmp_reg = 0;

    record_buff = (unsigned short *)malloc(2*1024*1024);

    memset((unsigned char*)record_buff, 0, 2*1024*1024);
    record_buff_pos = record_buff;

    tmp_reg = work_mode?(HI_SIO_I2S_DUAL_RX_CHN_OFFSET):(HI_SIO_PCM_RD_OFFSET);

    memset(&stCfg, 0, sizeof(DRV_DMA_CXCFG_STRU));

    if(NULL == record_buff)
    {
        return -1;
    }

    stCfg.uhwACount = 320;
    stCfg.uwSrcAddr = HI_SIO_REG_ADDR(tmp_reg);
    stCfg.uwDstAddr = (unsigned int)record_buff_pos;
    stCfg.uwConfig  = DRV_DMA_SIO_MEM_CFG;

    uwRet = bsp_dma_startwithcfg(15,
              &stCfg,
              audio_record_isr,
              0);
    return uwRet;
}

int audio_pcm_loop_test()
{
    int uwRet;

    g_psMicPcm = g_mic_in_audio_data_a;
    g_psSpkPcm = g_spk_out_audio_data_a;

    memset(g_mic_in_dma_cfg,0,sizeof(g_mic_in_dma_cfg));

    g_mic_in_dma_cfg[0].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_mic_in_dma_cfg[1]);
    g_mic_in_dma_cfg[0].uhwACount = 320;  /*窄带AMR每10ms采样80个点*/
    g_mic_in_dma_cfg[0].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_PCM_RD_OFFSET);
    g_mic_in_dma_cfg[0].uwDstAddr = (unsigned int)&g_mic_in_audio_data_a[0];
    g_mic_in_dma_cfg[0].uwConfig  = DRV_DMA_SIO_MEM_CFG;

    g_mic_in_dma_cfg[1].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_mic_in_dma_cfg[0]);
    g_mic_in_dma_cfg[1].uhwACount = 320;  /*窄带AMR每10ms采样80个点*/
    g_mic_in_dma_cfg[1].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_PCM_RD_OFFSET);
    g_mic_in_dma_cfg[1].uwDstAddr = (unsigned int)&g_mic_in_audio_data_b[0];
    g_mic_in_dma_cfg[1].uwConfig  = DRV_DMA_SIO_MEM_CFG;


    memset(g_spk_out_dma_cfg,0,sizeof(g_spk_out_dma_cfg));

    g_spk_out_dma_cfg[0].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_spk_out_dma_cfg[1]);
    g_spk_out_dma_cfg[0].uhwACount = 320;  /*窄带AMR每10ms采样80个点*/
    g_spk_out_dma_cfg[0].uwSrcAddr = (unsigned int)&g_mic_in_audio_data_b[0];
    g_spk_out_dma_cfg[0].uwDstAddr = HI_SIO_REG_ADDR(HI_SIO_PCM_XD_OFFSET);

    g_spk_out_dma_cfg[0].uwConfig  = DRV_DMA_MEM_SIO_CFG;

    g_spk_out_dma_cfg[1].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_spk_out_dma_cfg[0]);
    g_spk_out_dma_cfg[1].uhwACount = 320;  /*窄带AMR每10ms采样80个点*/
    g_spk_out_dma_cfg[1].uwSrcAddr = (unsigned int)&g_mic_in_audio_data_a[0];
    g_spk_out_dma_cfg[1].uwDstAddr = HI_SIO_REG_ADDR(HI_SIO_PCM_XD_OFFSET);

    g_spk_out_dma_cfg[1].uwConfig  = DRV_DMA_MEM_SIO_CFG;
	uwRet = bsp_dma_startwithcfg(15,
              &g_mic_in_dma_cfg[0],
              0,
              0);

    uwRet = bsp_dma_startwithcfg(14,
              &g_spk_out_dma_cfg[0],
              0,
              0);

    return uwRet;
}

int audio_i2s_loop_test()
{
    int uwRet;

    g_psMicPcm = g_mic_in_audio_data_a;
    g_psSpkPcm = g_spk_out_audio_data_a;

    memset(g_mic_in_dma_cfg,0,sizeof(g_mic_in_dma_cfg));

    g_mic_in_dma_cfg[0].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_mic_in_dma_cfg[1]);
    g_mic_in_dma_cfg[0].uhwACount = 320;
    g_mic_in_dma_cfg[0].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_DUAL_RX_CHN_OFFSET);
    g_mic_in_dma_cfg[0].uwDstAddr = (unsigned int)&g_mic_in_audio_data_a[0];
    g_mic_in_dma_cfg[0].uwConfig  = DRV_DMA_SIO_MEM_CFG;

    g_mic_in_dma_cfg[1].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_mic_in_dma_cfg[0]);
    g_mic_in_dma_cfg[1].uhwACount = 320;
    g_mic_in_dma_cfg[1].uwSrcAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_DUAL_RX_CHN_OFFSET);
    g_mic_in_dma_cfg[1].uwDstAddr = (unsigned int)&g_mic_in_audio_data_b[0];
    g_mic_in_dma_cfg[1].uwConfig  = DRV_DMA_SIO_MEM_CFG;


    memset(g_spk_out_dma_cfg,0,sizeof(g_spk_out_dma_cfg));


    g_spk_out_dma_cfg[0].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_spk_out_dma_cfg[1]);
    g_spk_out_dma_cfg[0].uhwACount = 320;
    g_spk_out_dma_cfg[0].uwSrcAddr = (unsigned int)&g_mic_in_audio_data_b[0];
    g_spk_out_dma_cfg[0].uwDstAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_DUAL_TX_CHN_OFFSET);

    g_spk_out_dma_cfg[0].uwConfig  = DRV_DMA_MEM_SIO_CFG;

    g_spk_out_dma_cfg[1].uwLli = DRV_DMA_LLI_LINK((unsigned int)&g_spk_out_dma_cfg[0]);
    g_spk_out_dma_cfg[1].uhwACount = 320;
    g_spk_out_dma_cfg[1].uwSrcAddr = (unsigned int)&g_mic_in_audio_data_a[0];
    g_spk_out_dma_cfg[1].uwDstAddr = HI_SIO_REG_ADDR(HI_SIO_I2S_DUAL_TX_CHN_OFFSET);

    g_spk_out_dma_cfg[1].uwConfig  = DRV_DMA_MEM_SIO_CFG;
	uwRet = bsp_dma_startwithcfg(15,
              &g_mic_in_dma_cfg[0],
              0,
              0);

    uwRet = bsp_dma_startwithcfg(14,
              &g_spk_out_dma_cfg[0],
              0,
              0);

    return uwRet;
}

void bsp_dma_init( void )
{
    DRV_DMA_CXISR_STRU                 *pstDmaCxIsr = DRV_DMA_GetCxIsrPtr();

    /*初始化g_astDrvDmaCxIntIsr为全零*/
    memset(pstDmaCxIsr, 0, sizeof(DRV_DMA_CXISR_STRU)*DRV_DMA_MAX_CHANNEL_NUM);

    DMA_REG_WRITEBIT(DRV_SC_DMA_SEL_CTRL_ADDR, 2, 2, 1);

    /* 清中断状态 */
    DMA_REG_WRITE(DRV_DMA_INT_ERR1, 0xffff);
    DMA_REG_WRITE(DRV_DMA_INT_ERR2, 0xffff);
    DMA_REG_WRITE(DRV_DMA_INT_ERR3, 0xffff);
    DMA_REG_WRITE(DRV_DMA_INT_TC1, 0xffff);
    DMA_REG_WRITE(DRV_DMA_INT_TC2, 0xffff);

    /* 屏蔽设置 */
    DMA_REG_WRITE(DRV_DMA_INT_ERR1_MASK, 0xc000);
    DMA_REG_WRITE(DRV_DMA_INT_ERR2_MASK, 0xc000);
    DMA_REG_WRITE(DRV_DMA_INT_ERR3_MASK, 0xc000);
    DMA_REG_WRITE(DRV_DMA_INT_TC1_MASK, 0xc000);
    DMA_REG_WRITE(DRV_DMA_INT_TC2_MASK, 0xc000);

    /*注册DMA ISR*/
    (void)intConnect ((VOIDFUNCPTR *)INT_LVL_EDMAC0, (VOIDFUNCPTR)bsp_dma_isr, 0);

    (void)intEnable(INT_LVL_EDMAC0);
}

/*****************************************************************************
 函 数 名  : bsp_dma_startwithcfg
 功能描述  : 附带详细配置的DMA通道启动
 输入参数  : uhwChannelNo   - 通道号, 取值0-15
             *pstCfg        - 配置参数, 若使用链表则链表结点需要32bytes对齐
             pfIntHandleFunc- DMA中断回调函数指针
             uwPara         - DMA中断回调函数的回调参数
 输出参数  : 无
 返 回 值  : unsigned int     - VOS标准返回值，BSP_ERROR(失败)/BSP_OK(成功)
 调用函数  :
 被调函数  :
*****************************************************************************/
unsigned int bsp_dma_startwithcfg(
                unsigned short              uhwChannelNo,
                DRV_DMA_CXCFG_STRU     *pstCfg,
                drv_dma_int_func        pfIntHandleFunc,
                unsigned int              uwPara)
{
    unsigned int                          uwChannelMask   = (0x1L << uhwChannelNo);
    DRV_DMA_CXISR_STRU                 *pstCxIsr        = DRV_DMA_GetCxIsrPtr();

    /* 检查参数是否非法 */
    if (uhwChannelNo >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        return BSP_ERROR;
    }

    /*写0到DRV_DMA_CX_CONFIG(uhwChannelNo) bit0禁止通道*/
    DMA_REG_WRITEBIT(DRV_DMA_CX_CONFIG((unsigned int)uhwChannelNo),
                  DMA_BIT0,
                  DMA_BIT0,
                  0);

    /*写通道X当前一维传输剩余的Byte数,[15,0]*/
    DMA_REG_WRITEBIT(DRV_DMA_CX_CNT0((unsigned int)uhwChannelNo),
                  DMA_BIT0,
                  DMA_BIT15,
                  pstCfg->uhwACount);

    /*写通道X当前二维传输剩余的Array个数,[31,16]*/
    DMA_REG_WRITEBIT(DRV_DMA_CX_CNT0((unsigned int)uhwChannelNo),
                  DMA_BIT16,
                  DMA_BIT31,
                  pstCfg->uhwBCount);

    /*写通道X当前三维传输剩余的Frame数,[15,0]*/
    DMA_REG_WRITEBIT(DRV_DMA_CX_CNT1((unsigned int)uhwChannelNo),
                  DMA_BIT0,
                  DMA_BIT15,
                  pstCfg->uhwCCount);

    /*写通道X的二维源地址偏移量[31,16]及目的地址偏移量[15,0]*/
    DMA_REG_WRITEBIT(DRV_DMA_CX_BINDX((unsigned int)uhwChannelNo),
                  DMA_BIT0,
                  DMA_BIT15,
                  pstCfg->uhwDstBIndex);
    DMA_REG_WRITEBIT(DRV_DMA_CX_BINDX((unsigned int)uhwChannelNo),
                  DMA_BIT16,
                  DMA_BIT31,
                  pstCfg->uhwSrcBIndex);

    /*写通道X的三维源地址偏移量[31,16]及目的地址偏移量[15,0]*/
    DMA_REG_WRITEBIT(DRV_DMA_CX_CINDX((unsigned int)uhwChannelNo),
                  DMA_BIT0,
                  DMA_BIT15,
                  pstCfg->uhwDstCIndex);
    DMA_REG_WRITEBIT(DRV_DMA_CX_CINDX((unsigned int)uhwChannelNo),
                  DMA_BIT16,
                  DMA_BIT31,
                  pstCfg->uhwSrcCIndex);

    /*写通道X的源地址[31,0]及目的地址[31,0]*/
    DMA_REG_WRITE(DRV_DMA_CX_DES_ADDR((unsigned int)uhwChannelNo), pstCfg->uwDstAddr);
    DMA_REG_WRITE(DRV_DMA_CX_SRC_ADDR((unsigned int)uhwChannelNo), pstCfg->uwSrcAddr);

    /*写通道X的链表地址配置*/
    DMA_REG_WRITE(DRV_DMA_CX_LLI((unsigned int)uhwChannelNo), pstCfg->uwLli);

    /*清除通道X的各种中断状态*/
    DMA_REG_WRITE(DRV_DMA_INT_TC1_RAW, uwChannelMask);
    DMA_REG_WRITE(DRV_DMA_INT_TC2_RAW, uwChannelMask);
    DMA_REG_WRITE(DRV_DMA_INT_ERR1_RAW, uwChannelMask);
    DMA_REG_WRITE(DRV_DMA_INT_ERR2_RAW, uwChannelMask);
    DMA_REG_WRITE(DRV_DMA_INT_ERR3_RAW, uwChannelMask);

    /*若回调函数非空,则保存该值*/
    if (NULL != pfIntHandleFunc)
    {
        pstCxIsr[uhwChannelNo].pfFunc  = pfIntHandleFunc;
        pstCxIsr[uhwChannelNo].uwPara  = uwPara;

        /*依次写(uhwChannelNo对应bit为1)到如下寄存器打开HiFi相应DMA通道中断屏蔽*/
        DMA_REG_WRITEBIT(DRV_DMA_INT_ERR1_MASK, uhwChannelNo, uhwChannelNo, 1);
        DMA_REG_WRITEBIT(DRV_DMA_INT_ERR2_MASK, uhwChannelNo, uhwChannelNo, 1);
        DMA_REG_WRITEBIT(DRV_DMA_INT_ERR3_MASK, uhwChannelNo, uhwChannelNo, 1);
        DMA_REG_WRITEBIT(DRV_DMA_INT_TC1_MASK, uhwChannelNo, uhwChannelNo, 1);
        DMA_REG_WRITEBIT(DRV_DMA_INT_TC2_MASK, uhwChannelNo, uhwChannelNo, 1);

    }

    /*写通道X的配置*/
    DMA_REG_WRITE(DRV_DMA_CX_CONFIG((unsigned int)uhwChannelNo), pstCfg->uwConfig);
    return BSP_OK;

}

/*****************************************************************************
 函 数 名  : bsp_dma_stop
 功能描述  : 附带详细配置的DMA通道启动
 输入参数  :

 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_dma_stop(unsigned short uhwChannelNo)
{
    DRV_DMA_CXISR_STRU                 *pstCxIsr        = DRV_DMA_GetCxIsrPtr();

    bsp_edma_channel_stop(uhwChannelNo);

    /* 写0到DRV_DMA_CX_CONFIG(uhwChannelNo) bit0 停止对应DMA通道 */
    DMA_REG_WRITEBIT(DRV_DMA_CX_CONFIG((unsigned int)uhwChannelNo), DMA_BIT0, DMA_BIT0, 0);

    /*依次写(uhwChannelNo对应bit为0)到如下寄存器屏蔽HiFi相应DMA通道中断*/
    DMA_REG_WRITEBIT(DRV_DMA_INT_ERR1_MASK, uhwChannelNo, uhwChannelNo, 0);
    DMA_REG_WRITEBIT(DRV_DMA_INT_ERR2_MASK, uhwChannelNo, uhwChannelNo, 0);
    DMA_REG_WRITEBIT(DRV_DMA_INT_ERR3_MASK, uhwChannelNo, uhwChannelNo, 0);
    DMA_REG_WRITEBIT(DRV_DMA_INT_TC1_MASK, uhwChannelNo, uhwChannelNo, 0);
    DMA_REG_WRITEBIT(DRV_DMA_INT_TC2_MASK, uhwChannelNo, uhwChannelNo, 0);

    /*清除全局变量中对应通道处理内容*/
    memset(&pstCxIsr[uhwChannelNo],0,sizeof(DRV_DMA_CXISR_STRU));
}

/*****************************************************************************
 函 数 名  : bsp_dma_isr
 功能描述  : DMA通道中断
 输入参数  :

 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_dma_isr(void)
{
    unsigned int                          i;
    unsigned int                          uwErr1;
    unsigned int                          uwErr2;
    unsigned int                          uwErr3;
    unsigned int                          uwTc1;
    unsigned int                          uwTc2;
    unsigned int                          uwIntMask;
    unsigned int                          uwIntState;
    drv_dma_int_type                    enIntType;
    DRV_DMA_CXISR_STRU                 *pstDmaCxIsr = DRV_DMA_GetCxIsrPtr();

    /*读取HIFI CPU的DMA通道中断状态*/
    uwIntState  = DMA_REG_READ(DRV_DMA_INT_STAT);

    /*若通道状态全0表示全通道无中断或中断已处理,直接退出*/
    if (0 == uwIntState)
    {
        return;
    }

    /*读取各中断寄存器,查询各中断状态,包括Tc1/Tc2/Err1/Err2/Err3*/
    uwErr1      = DMA_REG_READ(DRV_DMA_INT_ERR1);
    uwErr2      = DMA_REG_READ(DRV_DMA_INT_ERR2);
    uwErr3      = DMA_REG_READ(DRV_DMA_INT_ERR3);
    uwTc1       = DMA_REG_READ(DRV_DMA_INT_TC1);
    uwTc2       = DMA_REG_READ(DRV_DMA_INT_TC2);

    /*写uwIntState依次到各寄存器清除中断,清除本次查询到的通道中断*/
    DMA_REG_WRITE(DRV_DMA_INT_TC1_RAW, uwIntState);
    DMA_REG_WRITE(DRV_DMA_INT_TC2_RAW, uwIntState);
    DMA_REG_WRITE(DRV_DMA_INT_ERR1_RAW, uwIntState);
    DMA_REG_WRITE(DRV_DMA_INT_ERR2_RAW, uwIntState);
    DMA_REG_WRITE(DRV_DMA_INT_ERR3_RAW, uwIntState);

    /*遍历调用各通道注册的回调处理函数*/
    for ( i = 0; i < 16; i++)
    {
        uwIntMask = 0x1L << i;

        /*若该通道有中断产生(对应比特为1)*/
        if (uwIntState & uwIntMask)
        {
            if (NULL != pstDmaCxIsr[i].pfFunc)
            {
                /*若ERR1中断状态对应比特为1*/
                if (uwErr1 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_ERR1;
                }
                /*若ERR2中断状态对应比特为1*/
                else if (uwErr2 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_ERR2;
                }
                /*若ERR3中断状态对应比特为1*/
                else if (uwErr3 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_ERR3;
                }
                /*若TC1中断状态对应比特为1*/
                else if (uwTc1 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_TC1;
                }
                /*若TC2中断状态对应比特为1*/
                else if (uwTc2 & uwIntMask)
                {
                    enIntType = DRV_DMA_INT_TYPE_TC2;
                }
                /*未知中断*/
                else
                {
                    enIntType = DRV_DMA_INT_TYPE_BUTT;
                }

                /*调用注册的中断处理函数*/
                pstDmaCxIsr[i].pfFunc(enIntType, pstDmaCxIsr[i].uwPara);
            }
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

