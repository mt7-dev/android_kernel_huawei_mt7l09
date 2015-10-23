

/*lint --e{537} */
#include <stdio.h>
#include <string.h>
#include <MemoryMap.h>
#include <product_config.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>
#include <bsp_dsp.h>

/*****************************************************************************
 结构名    : PS_PHY_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 物理层和PS的共享地址，OAM需要获取并传给TTF或者初始化后由PHY读取
*****************************************************************************/
typedef struct
{
    /*物理层初始化,PS只读*/
    unsigned int uwProtectWord1;                      /*0x5a5a5a5a
 */
    unsigned int uwPhyGttfShareAddr;                  /* GDSP与GTTF共享内存接口地址,物理层初始化
 */

    /*PS负责初始化,PHY只读*/
    unsigned int uwProtectWord2;                      /*0x5a5a5a5a，PS填写
 */
    unsigned int uwHsupaUpaccMacePduBaseAddr;         /*HSUPA_UPACC_MACE_PDU_BASE_ADDR
 */
    unsigned int uwEcsArmDspBufferSize;               /*ECS_ARM_DSP_BUFFER_SIZE
 */
    unsigned int uwSocTimeSliceAddr;                  /*时标读取地址
 */

    /*AXI addr*/
    unsigned int uwStatusFlagAddr;                    /*HIFI/DSP STATUS_FLAG_ADDR
 */
    unsigned int uwTempratureAddr;                    /*温保
 */

    unsigned long uwHsdpaWttfBufAddr;                  /* HSDPA WTTF共享buf地址 */
    /*保留，扩展用。以后可以把平台相关的地址放到这里传递进来*/
    unsigned long uwReserved[15];
}PS_PHY_DYN_ADDR_SHARE_STRU;

/*****************************************************************************
 结构名    : PHY_HIFI_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 物理层和HIFI的共享地址，OAM需要拷贝到HIFI的BOOT区共享数据中的相应结构体
*****************************************************************************/
typedef struct
{
    unsigned int uwProtectWord;           /*0x5a5a5a5a
 */
    unsigned int uwHifiPhyMboxHeadAddr;   /*HIFI和PHY的共享区定义，PHY负责初始化（须静态初始化）
 */
    unsigned int uwHifiPhyMboxBodyAddr;
    unsigned int uwPhyHifiMboxHeadAddr;   /*PHY和HIFI的共享区定义，PHY负责初始化（须静态初始化）
 */
    unsigned int uwPhyHifiMboxBodyAddr;
    unsigned int uwReserved[3];
}PHY_HIFI_DYN_ADDR_SHARE_STRU;


/*****************************************************************************
 结构名    : PS_PHY_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DSP上电时OAM需要往DSP的".dsp_share_addr"段写如下结构的数据，起始地址由DRV解析ZSP bin件后传给OAM
*****************************************************************************/
typedef struct
{
    PS_PHY_DYN_ADDR_SHARE_STRU      stPsPhyDynAddr;     /*PS和PHY的共享区定义 */
    PHY_HIFI_DYN_ADDR_SHARE_STRU    stPhyHifiDynAddr;   /*PHY和HIFI的共享区定义 */
}DSP_SHARE_ADDR_STRU;

/************************************************************************
 * FUNCTION
 *       BSP_GUDSP_UpateShareInfo
 * DESCRIPTION
 *       完成DSP共享的动态信息获取和更新功能
 * INPUTS
 *       NONE
 * OUTPUTS
 *       BSP_OK/BSP_ERR
 *************************************************************************/
extern int BSP_GUDSP_UpateShareInfo(unsigned long shareAddr);
int BSP_GUDSP_UpateShareInfo(unsigned long shareAddr)
{
    DSP_SHARE_ADDR_STRU      *pstDspShareInfo = (DSP_SHARE_ADDR_STRU*)shareAddr;

    if(0x5A5A5A5A != pstDspShareInfo->stPsPhyDynAddr.uwProtectWord1)
    {
        printf("\r\n BSP_GUDSP_UpateShareInfo: ZSP Share Info uwProtectWord1 Error.");
        return -1;     /*异常退出后会重启*/
    }

    /*填写L2使用的地址空间大小*/
    pstDspShareInfo->stPsPhyDynAddr.uwHsupaUpaccMacePduBaseAddr   = ECS_UPA_BASE_ADDR;
    pstDspShareInfo->stPsPhyDynAddr.uwEcsArmDspBufferSize         = ECS_UPA_SIZE;

    pstDspShareInfo->stPsPhyDynAddr.uwHsdpaWttfBufAddr            = ECS_ARM_DSP_BUFFER_BASE_ADDR;

    /*当前信息写入完成标志*/
    pstDspShareInfo->stPsPhyDynAddr.uwProtectWord2    = 0x5A5A5A5A;

    return 0;
}


/*****************************************************************************
 函 数 名  : bsp_dsp_is_hifi_exist
 功能描述  : 获取CODEC功能状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1:  CODEC存在
             0：CODEC 不存在
*****************************************************************************/
int bsp_dsp_is_hifi_exist(void)
{
    int ret = 0;
    DRV_MODULE_SUPPORT_STRU   stSupportNv = {0};

    ret = (int)bsp_nvm_read(NV_ID_DRV_MODULE_SUPPORT, (u8*)&stSupportNv, sizeof(DRV_MODULE_SUPPORT_STRU));
    if (ret)
        ret = 0;
    else
        ret = (int)stSupportNv.hifi;

    return ret;
}

#ifndef CONFIG_DSP

int bsp_bbe_load_muti(void)
{
    return 0;
}

int bsp_bbe_run(void)
{
    return 0;
}

int bsp_bbe_stop(void)
{
    return 0;
}

int bsp_bbe_power_on(void)
{
    return 0;
}

int bsp_bbe_power_off(void)
{
    return 0;
}

int bsp_bbe_unreset(void)
{
    return 0;
}

int bsp_bbe_reset(void)
{
    return 0;
}

int bsp_bbe_clock_enable(void)
{
    return 0;
}

int bsp_bbe_clock_disable(void)
{
    return 0;
}

int bsp_bbe_is_power_on(void)
{
    return 0;
}

int bsp_bbe_is_clock_enable(void)
{
    return 0;
}

void bsp_dsp_init(void)
{
    return;
}

int DRV_GET_CODEC_FUNC_STATUS(void)
{
#if (FEATURE_HW_CODEC== FEATURE_ON)
    return 1;
#else
	return 0;
#endif
}

#endif

