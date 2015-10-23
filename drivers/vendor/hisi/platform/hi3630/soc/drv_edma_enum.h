
/*该头文件专为TLdsp和Hifi提供，其他模块应引用drv_edma.h*/
#ifndef __DRV_EDMA_ENUM_H__
#define __DRV_EDMA_ENUM_H__

#define	EDMA_CH16_0	    0
#define	EDMA_CH16_1	    1
#define	EDMA_CH16_2	    2
#define	EDMA_CH16_3	    3
#define	EDMA_CH16_4	    4
#define	EDMA_CH16_5	    5
#define	EDMA_CH16_6	    6
#define	EDMA_CH16_7	    7
#define	EDMA_CH16_8	    8
#define	EDMA_CH16_9	    9
#define	EDMA_CH16_10	10
#define	EDMA_CH16_11	11
#define	EDMA_CH16_12	12
#define	EDMA_CH16_13	13
#define	EDMA_CH16_14	14
#define	EDMA_CH16_15	15
#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT
#define	EDMA_CH4_0	    16
#define	EDMA_CH4_1	    17
#define	EDMA_CH4_2	    18
#define	EDMA_CH4_3	    19
#define	EDMA_CH_END	    20
#else
#define	EDMA_CH4_0	    (0x1000)
#define	EDMA_CH4_1	    (0x1000)
#define	EDMA_CH4_2	    (0x1000)
#define	EDMA_CH4_3	    (0x1000)
#define	EDMA_CH_END	    16
#endif

#define	EDMA_CH_LDSP_API_USED_0 EDMA_CH16_0
#define	EDMA_CH_LDSP_API_USED_1 EDMA_CH16_1
#define	EDMA_CH_LDSP_LCS_SDR    EDMA_CH16_2
#define	EDMA_CH_LDSP_CSU_SDR    EDMA_CH16_3
#define	EDMA_CH_LDSP_EMU_SDR    EDMA_CH16_4
#define	EDMA_CH_LDSP_NV_LOADING EDMA_CH16_5
/****arm与ldsp复用****低功耗备份恢复  m2m */
#define	EDMA_CH_LOAD_LDSP_TCM  EDMA_CH_LDSP_API_USED_0
#define	EDMA_CH_LOAD_TDSP_TCM  EDMA_CH_LDSP_API_USED_1
/****gudsp与ldsp复用****GU CQI搬移 与 LDSP LCS */
#define	EDMA_CH_GUDSP_MEMORY_3  EDMA_CH_LDSP_LCS_SDR

/*EDMA_CH_GUDSP_MEMORY_9  ==  BBP_GRIF_PHY，
GUDSP实现BBP搬数，用于校准 复用GU BBP DEBUG采数功能
其他通道均为 M2M */
#define	EDMA_CH_GUDSP_MEMORY_9  EDMA_CH16_6
#define	EDMA_CH_GUDSP_MEMORY_7  EDMA_CH16_7
/****gudsp与gudsp复用****GU DSP加载与 GU AHB加载 */
#define	EDMA_CH_GUDSP_MEMORY_8  EDMA_CH_GUDSP_MEMORY_7


#define	EDMA_CH_GUDSP_MEMORY_1  EDMA_CH16_8
#define	EDMA_CH_GUDSP_MEMORY_2  EDMA_CH16_9
#define	EDMA_CH_GUDSP_MEMORY_4  EDMA_CH16_10
#define	EDMA_CH_GUDSP_MEMORY_6  EDMA_CH16_11

/* SIM卡0 接收和发送 / SIM卡1 接收和发送 */
#define	EDMA_CH_DRV_SIM_0       EDMA_CH16_12
#define	EDMA_CH_DRV_SIM_1       EDMA_CH16_13
/* 接收来自axi_monitor的采集数据 */
#define	EDMA_CH_DRV_AXIMON      EDMA_CH16_14

/*===================================================*/
/* HIFI SIO 收发 */
#define	EDMA_CH_HIFI_SIO_TX     (-1)/*EDMA_CH16_8*/
#define	EDMA_CH_HIFI_SIO_RX     (-1)/*EDMA_CH16_9*/
/*高速串口发送/接收使用	用于蓝牙语音场景*/
#define	EDMA_CH_HSUART_TX       (-1)/*EDMA_CH16_12*/
#define	EDMA_CH_HSUART_RX       (-1)/*EDMA_CH16_13*/
/* SPI1发送，刷LCD屏 */
#define	EDMA_CH_DRV_LCD         (-1)/*EDMA_CH16_14*/

#endif

