
#ifndef __DRV_DSP_H__
#define __DRV_DSP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  函数名:       int bsp_bbe_load_muti()
  函数描述:     加载镜像至TCM
  输入参数:     无
  输出参数:     无
  返回值:       成功：0
                失败：负数
*******************************************************************************/
extern int bsp_bbe_load_muti(void);
#define DRV_BBE_LOAD() bsp_bbe_load_muti()
/* 兼容老接口 */
#define BSP_LoadDsp() DRV_BBE_LOAD()

/*******************************************************************************
  函数名:       int bsp_bbe_run()
  函数描述:     激活DSP0
  输入参数:     无
  输出参数:     无
  返回值:       成功：0
                失败：负数
*******************************************************************************/
extern int bsp_bbe_run(void);
#define DRV_BBE_RUN() bsp_bbe_run()
/* 兼容老接口 */
#define BSP_RunDsp() DRV_BBE_RUN()

/* GU DSP接口适配 */
extern int BSP_GUDSP_UpateShareInfo(unsigned long shareAddr);
#define DRV_BSP_UPDATE_DSP_SHAREINFO(addr) BSP_GUDSP_UpateShareInfo(addr)

/*****************************************************************************
 函 数 名  : DRV_GET_CODEC_FUNC_STATUS
 功能描述  : 获取CODEC功能状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1:  CODEC存在
             0：CODEC 不存在
*****************************************************************************/
int bsp_dsp_is_hifi_exist(void);
#define DRV_GET_CODEC_FUNC_STATUS() bsp_dsp_is_hifi_exist()

/* 适配V9R1打桩 */
static INLINE int ZSP_Init(void)
{
    return OK;
}

static INLINE int DRV_DSP_SHARE_ADDR_GET(void)
{
    return OK;
}

static INLINE int DRV_DSP_DRX_GPIO_VAL_SET(UINT32 val)
{
    return OK;
}

static INLINE int BSP_TDS_GetDynTableAddr(void)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_GUSYS_ArmPllStatusGet
 功能描述  : 读取ARM PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
static INLINE unsigned int DRV_GET_ARM_PLL_STATUS(void)
{
    return OK;     /* 打桩 */
}

extern int bsp_bbe_stop(void);

extern int bsp_bbe_power_on(void);

extern int bsp_bbe_power_off(void);

extern int bsp_bbe_unreset(void);

extern int bsp_bbe_reset(void);

extern int bsp_bbe_clock_enable(void);

extern int bsp_bbe_clock_disable(void);

extern int bsp_bbe_adp_restore(void);

extern int bsp_bbe_adp_store(void);

extern int bsp_bbe_adp_get_state(void);

extern int bsp_msp_bbe_restore(void);

extern int bsp_msp_bbe_store(void);

extern int bsp_msp_wait_edma_ok(void);

/*****************************************************************************
 Prototype       : DRV_PWRCTRL_DSP_RUN
 Description     : 配置DSP为RUN模式
 Input           : None
 Output          : None.
 Return Value    : 0: 成功
                   -1: 失败
*****************************************************************************/
#define DRV_PWRCTRL_DSP_RUN() bsp_bbe_run()
/*****************************************************************************
 Prototype       : DRV_PWRCTRL_DSP_HOLD
 Description     : 配置DSP为stall模式
 Input           : None
 Output          : None.
 Return Value    : 0: 成功
                   -1: 失败
*****************************************************************************/
#define DRV_PWRCTRL_DSP_HOLD() bsp_bbe_stop()
/*****************************************************************************
 Prototype       : DRV_EDMA_DSP_MemRestore
 Description     : 恢复BBE16公共TCM
 Input           : None
 Output          : None.
 Return Value    : 0: 成功
                   -1 失败
*****************************************************************************/
#define DRV_EDMA_DSP_MemRestore() bsp_msp_bbe_restore()
/*****************************************************************************
 Prototype       : DRV_EDMA_DSP_MemRestore
 Description     : 保存BBE16公共TCM
 Input           : None
 Output          : None.
 Return Value    : 0: 成功
                   -1: 失败
*****************************************************************************/
#define DRV_EDMA_DSP_MemStore() bsp_msp_bbe_store()
/*****************************************************************************
 Prototype       : DRV_EDMA_DSP_StateGet
 Description     : 检查BBE16 TCM保存恢复是否完成
 Input           : None
 Output          : None.
 Return Value    : 1: DSP TCM保存恢复完成
*****************************************************************************/
#define DRV_EDMA_DSP_StateGet() bsp_msp_wait_edma_ok()
/*****************************************************************************
 Prototype       : DRV_PWRCTRL_DSP_UNRESET
 Description     : 解复位BBE16
 Input           : None
 Output          : None.
*****************************************************************************/
#define DRV_PWRCTRL_DSP_UNRESET() bsp_bbe_unreset()
/*****************************************************************************
 Prototype       : DRV_PWRCTRL_DSP_UNRESET
 Description     : 复位BBE16
 Input           : None
 Output          : None.
*****************************************************************************/
#define DRV_PWRCTRL_DSP_RESET() bsp_bbe_reset()
/*****************************************************************************
 Prototype       : DRV_PWRCTRL_DSP_CLKENABLE
 Description     : 打开BBE16时钟
 Input           : None
 Output          : None.
*****************************************************************************/
#define DRV_PWRCTRL_DSP_CLKENABLE() bsp_bbe_clock_enable()
/*****************************************************************************
 Prototype       : DRV_PWRCTRL_DSP_CLKDISABLE
 Description     : 关闭BBE16时钟
 Input           : None
 Output          : None.
*****************************************************************************/
#define DRV_PWRCTRL_DSP_CLKDISABLE() bsp_bbe_clock_disable()
#ifdef __cplusplus
}
#endif

#endif

