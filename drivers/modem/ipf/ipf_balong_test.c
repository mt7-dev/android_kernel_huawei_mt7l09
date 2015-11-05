#ifdef __cplusplus
extern "C" {
#endif

#ifdef __VXWORKS__
#include <vxWorks.h>
#include <cacheLib.h>
#include <intLib.h>
#include <logLib.h>
#include <string.h>
#include <taskLib.h>
#include <memLib.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include "arm_pbxa9.h"
#include "product_config.h"

#include "TtfDrvInterface.h"
#else 
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/delay.h>
#endif
#include <drv_ipf.h>
#include <bsp_ipf.h>
#include <bsp_hardtimer.h>
#ifdef CONFIG_MODULE_BUSSTRESS
#include <bsp_busstress.h>
#endif
//#define printcmpresult

typedef enum tagIPF_ADQ_CONFIG_E
{
    CONFIG_AD = 0,
    STOP_CONFIG_AD = 1
}IPF_ADQ_CONFIG_E;

typedef struct tagStreamIPv4Header
{
    BSP_U32 TOL:16;
    BSP_U32 TOS:8;
    BSP_U32 HeaderLen:4;
    BSP_U32 Vers:4; 
    BSP_U32 offset:13;
    BSP_U32 MF:1;
    BSP_U32 DF:1;
    BSP_U32 ReservFlag:1;
    BSP_U32 ID:16;
    BSP_U32 Checksum:16;
    BSP_U32 Prot:8;
    BSP_U32 TTL:8;    
    BSP_U32 SourceAddr;
    BSP_U32 DestAddr;
}StreamIPv4Header;

typedef struct tagStreamUDP
{
    BSP_U32 DstPort:16;
    BSP_U32 SrcPort:16;
    BSP_U32 Checksum:16;
    BSP_U32 Length:16;    
}StreamUDP;

StreamIPv4Header ipf_ipv4_head;
StreamUDP ipf_udp;

IPF_ADQ_CONFIG_E ipf_config_ad_flag = CONFIG_AD;

#ifdef __VXWORKS__
BSP_U32 g_u32UlError=0;
BSP_U32 g_u32UlFltError=0;
BSP_U32 g_u32UlTotalDataNum = 0;
BSP_U32 g_u32UlTotalBDNum = 0;
struct tagIPF_AD_DESC_S ADulshort[IPF_ULAD0_DESC_SIZE];
struct tagIPF_AD_DESC_S ADullong[IPF_ULAD1_DESC_SIZE];
#endif

#ifdef __KERNEL__
BSP_U32 g_u32DlError=0;
BSP_U32 g_u32DlFltError=0;
BSP_U32 g_u32DlTotalDataNum = 0;
BSP_U32 g_u32DlTotalBDNum = 0;
struct tagIPF_AD_DESC_S ADdlshort[IPF_DLAD0_DESC_SIZE];
struct tagIPF_AD_DESC_S ADdllong[IPF_DLAD1_DESC_SIZE];
#endif

BSP_BOOL g_u32IpfStAllInit = BSP_FALSE;
BSP_BOOL g_u32IpfStUlInit = BSP_FALSE;

BSP_BOOL g_u32IpfStDlInit = BSP_FALSE;



BSP_S32 IPF_ST_DL_FlushAD(IPF_ADQ_EMPTY_E eAdqEmptyDl);

void ipf_ad_config_flag(IPF_ADQ_CONFIG_E flag)
{
	ipf_config_ad_flag = flag;
	if(STOP_CONFIG_AD == ipf_config_ad_flag)
	{
		IPF_PRINT("Stop adq config at this core. \n");
	}
	else
	{
		IPF_PRINT("Restore adq config at this core. \n");
#ifdef __VXWORKS__
		IPF_ST_UL_FlushAD(IPF_AD_0);
		IPF_ST_UL_FlushAD(IPF_AD_1);
#endif
#ifdef __KERNEL__
		IPF_ST_DL_FlushAD(IPF_AD_0);
		IPF_ST_DL_FlushAD(IPF_AD_1);
#endif
	}
}

#ifdef __VXWORKS__
int g_IpfUlTaskId = 0;

BSP_U8* g_pu8STUlData = BSP_NULL;

SEM_ID g_IpfUlTaskSemId;
SEM_ID g_IpfUlIntSemId;

BSP_S32 CmpDMAData(BSP_U32* p1, BSP_U32* p2, BSP_U32 u32Len);

/*CH0为上行通道，故编到Ccore*/
void print_CH0_BUFFED_AD_Num()
{
	HI_IPF_CH0_ADQ0_STAT_T ad0_stat;
	HI_IPF_CH0_ADQ1_STAT_T ad1_stat;
	ad0_stat.u32 = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_STAT_OFFSET);
	ad1_stat.u32 = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_STAT_OFFSET);
	if((ad0_stat.bits.ul_adq0_buf_epty == 1)&&(ad0_stat.bits.ul_adq0_buf_full == 1))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD0_BUF_FLAG_ERROR.\n");
	}
	else if(ad0_stat.bits.ul_adq0_buf_full == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD0_BUF_NUM 2,2 unused AD in AD_Buff.\n");
	}
	else if(ad0_stat.bits.ul_adq0_buf_epty == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD0_BUF_NUM 0,0 unused AD in AD_Buff.\n");
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD0_BUF_NUM 1,1 unused AD in AD_Buff.\n");
	}
	
	if((ad1_stat.bits.ul_adq1_buf_epty == 1)&&(ad1_stat.bits.ul_adq1_buf_full == 1))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD1_BUF_FLAG_ERROR.\n");
	}
	else if(ad1_stat.bits.ul_adq1_buf_full == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD1_BUF_NUM 2,2 unused AD in AD_Buff.\n");
	}
	else if(ad1_stat.bits.ul_adq1_buf_epty == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD1_BUF_NUM 0,0 unused AD in AD_Buff.\n");
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH0_AD1_BUF_NUM 1,1 unused AD in AD_Buff.\n");
	}
	return;
}

BSP_S32 IpfUlIntCb()
{
    semGive(g_IpfUlIntSemId);
    return OK;
}
BSP_S32 IpfUlIntSTCb(BSP_VOID)
{
    print_CH0_BUFFED_AD_Num();
    return OK;
}

BSP_S32 IpfUlIntCb101()
{    
	BSP_U32 i,j;
	BSP_U32 u32AD0Num;
	BSP_U32 u32AD1Num;
	BSP_U32 u32Num = IPF_ULRD_DESC_SIZE;
	BSP_S32 s32Ret = 0;
	IPF_RD_DESC_S stRd[IPF_ULRD_DESC_SIZE];
	IPF_PRINT("Enter IpfUlIntCb101  \n");
	BSP_U8* p=NULL;
	i = 0;
	BSP_IPF_GetUlRd(&u32Num, stRd);
	IPF_PRINT("Data transfer Num:%d \n",u32Num);
	for(j = 0; j < u32Num; j++)
	{	
		s32Ret = CmpDMAData((BSP_U32 *)g_pu8STUlData, (BSP_U32 *)stRd[j].u32OutPtr, stRd[j].u16PktLen);
		/*数据比对*/
		if(0 != s32Ret)
		{
		    IPF_PRINT("Comp error \n");
		    IPF_PRINT("g_pu8STUlData\n");
		    IPF_PRINT("===================================\n");
		    p=g_pu8STUlData;
		    for(i=0; i < stRd[j].u16PktLen; i++)
		    {
		        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
		        i = i+4;
		    }
		    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));
		    IPF_PRINT("stRd[%d].u32OutPtr\n",j);
                  IPF_PRINT("===================================\n");
                  IPF_PRINT("stRd[%d].u32OutPtr =%x\n",j,stRd[j].u32OutPtr);
		    p=(BSP_U8 *)stRd[j].u32OutPtr;
		    for(i=0; i<stRd[j].u16PktLen; i++)
		    {
		        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
		        i = i+4;
		    }
		    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));
		g_u32UlError++;
		}			
	}

	print_CH0_BUFFED_AD_Num();
	BSP_IPF_GetUlAdNum(&u32AD0Num, &u32AD1Num);
	bsp_trace(BSP_LOG_LEVEL_FATAL, BSP_MODU_IPF,
		          "\r cb101 AD0Num:%d AD1Num:%d \n",u32AD0Num,u32AD1Num,0,0,0,0,0);

	bsp_trace(BSP_LOG_LEVEL_FATAL, BSP_MODU_IPF,
                        "\r cb101 Data transfer success! Num:%d \n",i,0,0,0,0,0,0);

    return OK;    
}
BSP_S32 IpfUlIntCb111(BSP_VOID)
{
	BSP_U32 j;
	BSP_U32 u32AD0Num;
	BSP_U32 u32AD1Num;
	BSP_U32 u32Num = IPF_DLRD_DESC_SIZE;
	IPF_RD_DESC_S stRd[IPF_DLRD_DESC_SIZE];
	
	BSP_IPF_GetUlRd(&u32Num, stRd);

	IPF_PRINT("Data transfer Num:%d \n",u32Num);
	
	for(j = 0; j < u32Num; j++)
	{			
		/*过滤结果*/
		if(stRd[j].u16Result == 0x3f)
		{
			IPF_PRINT("stRd[j].u16Result ERROR !value is 0x%x \n",j,stRd[j].u16Result);
			g_u32UlError++;
		}			
	}
	g_u32UlTotalBDNum += u32Num;

	BSP_IPF_GetUlAdNum(&u32AD0Num, &u32AD1Num);
	
	IPF_PRINT("Data transfer success! Num: %d \n AD0Num:%d AD1Num:%d \n",j,u32AD0Num,u32AD1Num);
	IPF_PRINT("Data transfer ERROR Num:%d \n",g_u32UlError);
    return OK;
}

BSP_S32 IPF_ST_UL_FlushAD(IPF_ADQ_EMPTY_E eAdqEmptyUl)
{
	BSP_S32 s32Ret;  
	if(CONFIG_AD != ipf_config_ad_flag)
	{
		return OK;
	}
	if(IPF_AD_0 == eAdqEmptyUl)
	{
		s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,IPF_ULAD0_DESC_SIZE - 4,&ADulshort[0]);
		if(s32Ret != IPF_SUCCESS)
		{
			IPF_PRINT("TEST ERROR BSP_IPF_ConfigDlAd :%d s32ret = %x\n",__LINE__, s32Ret);
			return ERROR;
		}
	}
	else if(IPF_AD_1 == eAdqEmptyUl)
	{
		s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,IPF_ULAD1_DESC_SIZE - 4,&ADullong[0]);

		if(s32Ret != IPF_SUCCESS)
		{
			IPF_PRINT("TEST ERROR BSP_IPF_ConfigUlAd:%d s32ret = %x\n",__LINE__, s32Ret);
			return ERROR;
		}
	}
	else
	{
		return ERROR;
	}
	return OK;
}
BSP_S32 IPF_ST_UL_FlushOneAD(BSP_U32 AD0Num, BSP_U32 AD1Num)
{
    BSP_S32 s32Ret;  
    if(CONFIG_AD !=  ipf_config_ad_flag)
    {
        return OK;
    }

    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,AD0Num,&ADulshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT("TEST ERROR BSP_IPF_ConfigDlAd :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }

    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,AD1Num,&ADullong[0]);

	if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT("TEST ERROR BSP_IPF_ConfigUlAd:%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
	return OK;
}

BSP_S32 IPF_ST_INIT(BSP_VOID)
{
    IPF_COMMON_PARA_S stCommPara;
    BSP_S32 s32Ret = 0;

    /*初始化时IP过滤的参数配置*/
    stCommPara.bEspSpiDisable = 1;      /* ESP SPI是否参与匹配控制信号 */
    stCommPara.bAhSpiDisable = 1;       /* AH SPI是否参与匹配控制信号 */
    stCommPara.bEspAhSel= 0;           /* 0选ESP，1选AH */
    stCommPara.bIpv6NextHdSel = 0;
    stCommPara.eMaxBurst = IPF_BURST_16;       /* BURST最大长度 */
    stCommPara.bSpWrrModeSel = 0;
    stCommPara.bSpPriSel = 0;          /* SP优先级选择 */
    stCommPara.bFltAddrReverse = 1;        /* 配置为0,地址为大端 */
    stCommPara.bFilterSeq = 0;         /* 过滤器配置顺序指示 */
    stCommPara.bAdReport = 0; /*上报AD第二个字指针*/
#ifdef IPF_NO_FILTER_TEST
    stCommPara.bMultiFilterChainEn = 0;/*单寄存器链模式*/
    stCommPara.bMultiModeEn = 0;/*静态业务模式*/
#endif
    stCommPara.bMultiFilterChainEn = 1;/*多寄存器链模式*/
    stCommPara.bMultiModeEn = 1;/*动态业务模式*/
	
    s32Ret = BSP_IPF_Init(&stCommPara) ;
    if (s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_Init :%d   s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
    g_u32IpfStAllInit = BSP_TRUE;
    
    return OK;
}

#ifdef CONFIG_MODULE_BUSSTRESS
BSP_VOID IPF_UL_Task_1(BSP_VOID)
{
    BSP_U32 u32Num = 0;
    IPF_RD_DESC_S stRd[IPF_ULRD_DESC_SIZE];
    BSP_U32 i = 0;
    BSP_U32 u32AD0Num,u32AD1Num;
    BSP_S32 s32Ret;
    while(1)
    { 
        memset(stRd, 0x0, sizeof(IPF_RD_DESC_S) * IPF_ULRD_DESC_SIZE);
        u32Num = IPF_ULRD_DESC_SIZE;
        semTake(g_IpfUlIntSemId, WAIT_FOREVER);
#if 0
        if(*g_ipftestdebug)
        {
            IPF_PRINT(" IPF_UL_Task_1 in \n");
        }
#endif
        BSP_IPF_GetUlRd(&u32Num, stRd);
        for(i = 0; i < u32Num; i++)
        {
            g_u32UlTotalBDNum++;
        }
    s32Ret = BSP_IPF_GetUlAdNum(&u32AD0Num,&u32AD1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetUlAdNum :%d\n",__LINE__);
        return;
    }
    if(u32AD0Num > 10)
    {
        s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,u32AD0Num,&ADulshort[0]);
        if(IPF_SUCCESS != s32Ret)
        {
            IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
        }
        for(i=0;i < u32AD0Num;i++)
        {
            memset((BSP_U32*)ADulshort[i].u32OutPtr1, 0, 404);
        }
    }
    if(u32AD1Num > 10)
    {
        s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,u32AD1Num,&ADullong[0]);
        if(IPF_SUCCESS != s32Ret)
        {
            IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
        }
        for(i=0;i < u32AD1Num;i++)
        {
            memset((BSP_U32*)ADullong[i].u32OutPtr1, 0, 1500);
        }

    }
#if 0
        if(*g_ipftestdebug)
        {
            IPF_PRINT(" IPF_UL_Task_1 out \n");
        }
#endif
        semGive(g_IpfUlTaskSemId);
    }
}
BSP_S32 IPF_ST_UL_INIT_STRESS(BSP_VOID)
{
    if((g_IpfUlIntSemId = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
    {
        IPF_PRINT(" TESTERROR    semBCreate :%d\n",__LINE__);
        return ERROR;
    }

    if((g_IpfUlTaskSemId = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
    {
        IPF_PRINT(" TESTERROR    semBCreate :%d\n",__LINE__);
        return ERROR;
    }

    g_IpfUlTaskId = taskSpawn("IpfUlTask1", 132, 2000, 20000, (FUNCPTR) IPF_UL_Task_1,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    return OK;

}
#endif

BSP_S32 IPF_ST_UL_INIT(BSP_VOID)
{
    IPF_CHL_CTRL_S stCtrl;
    IPF_FILTER_CONFIG_S stUlFilterInfo[50];
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;


    ipf_ipv4_head.Vers = 4;
    ipf_ipv4_head.HeaderLen = 5;
    ipf_ipv4_head.TOS = 3;
    ipf_ipv4_head.TOL = 1200;
    ipf_ipv4_head.ID = 1;
    ipf_ipv4_head.ReservFlag = 0;
    ipf_ipv4_head.DF = 0;
    ipf_ipv4_head.MF = 0;
    ipf_ipv4_head.offset = 0;
    ipf_ipv4_head.TTL = 128;
    ipf_ipv4_head.Prot = 17; /* UDP */
    ipf_ipv4_head.Checksum = 0xEC3A;
    ipf_ipv4_head.SourceAddr = 0xACA84649;
    ipf_ipv4_head.DestAddr = 0x0202014E;

    ipf_udp.SrcPort = 5088;
    ipf_udp.DstPort = 2923;
    ipf_udp.Checksum = 0;
    ipf_udp.Length = ipf_ipv4_head.TOL;
	for(i=0; i < IPF_ULAD0_DESC_SIZE; i++)
	{
    		ADulshort[i].u32OutPtr0 = (BSP_U32)NULL;
		ADulshort[i].u32OutPtr0 = (BSP_U32)malloc(404);
		ADulshort[i].u32OutPtr1 = ADulshort[i].u32OutPtr0;
		if((BSP_U32)NULL == ADulshort[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            return ERROR;
			}
	}
	for(i=0; i < IPF_ULAD1_DESC_SIZE; i++)
	{
    		ADullong[i].u32OutPtr0 = (BSP_U32)NULL;
		ADullong[i].u32OutPtr0 = (BSP_U32)malloc(1500);
		ADullong[i].u32OutPtr1 = ADullong[i].u32OutPtr0;
		if((BSP_U32)NULL == ADullong[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error1;
			}
	}
	
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,IPF_ULAD0_DESC_SIZE - 4,&ADulshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd :%d s32Ret = %x\n",__LINE__, s32Ret);
        return BSP_ERROR;
    }    

    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,IPF_ULAD1_DESC_SIZE - 4,&ADullong[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd :%d s32Ret = %x\n",__LINE__, s32Ret);
        return BSP_ERROR;
    }    

       memset(stUlFilterInfo, 0x0, 50*sizeof(IPF_FILTER_CONFIG_S));

    for(i=0;i<24;i++)
    {
        stUlFilterInfo[i].u32FilterID = i;
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0xffffffff;
    }
    
    stUlFilterInfo[24].u32FilterID = 172;
    stUlFilterInfo[24].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0x23C01;
    *(BSP_U32*)(stUlFilterInfo[24].stMatchInfo.u8SrcAddr) = 0xACA84649;
    *(BSP_U32*)(stUlFilterInfo[24].stMatchInfo.u8DstAddr) = 0x0202014E;
    *(BSP_U32*)(stUlFilterInfo[24].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
    stUlFilterInfo[24].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = 1;
    stUlFilterInfo[24].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = 5100;
    stUlFilterInfo[24].stMatchInfo.unDstPort.Bits.u16DstPortLo = 10;
    stUlFilterInfo[24].stMatchInfo.unDstPort.Bits.u16DstPortHi = 5100;
    stUlFilterInfo[24].stMatchInfo.unTrafficClass.u32TrafficClass  = 3|(0xFF<<8);
    stUlFilterInfo[24].stMatchInfo.u32LocalAddressMsk = 0x00;
    stUlFilterInfo[24].stMatchInfo.unNextHeader.u32Protocol = 1;
    stUlFilterInfo[24].stMatchInfo.unFltCodeType.Bits.u16Type = 2;
    stUlFilterInfo[24].stMatchInfo.unFltCodeType.Bits.u16Code = 3;
    stUlFilterInfo[24].stMatchInfo.u32FltSpi = 4;


    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    stCtrl.bDataChain = BSP_FALSE;
    stCtrl.bEndian = BSP_TRUE; /* 小端 */
    stCtrl.eIpfMode = IPF_MODE_FILTERANDTRANS;
    stCtrl.u32WrrValue = 0;
    s32Ret = BSP_IPF_ConfigUlChannel(&stCtrl);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlChannel :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }

    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, stUlFilterInfo, 25);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
	
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM1_ULFC, stUlFilterInfo, 25);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }

    s32Ret = BSP_IPF_ChannelEnable(IPF_CHANNEL_UP, BSP_TRUE);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
	IPF_PRINT("IPF_ST_UL_INIT BSP_IPF_ChannelEnable success %d \n",__LINE__);
    return OK;
error1:
	IPF_PRINT("IPF_ST_UL_INIT ERROR1");
	do
	{
		i--;
		free((BSP_VOID *)ADulshort[i].u32OutPtr0);
		
	}while(i);
	return ERROR;
}

BSP_S32 IPF_ST_DL_INIT(BSP_VOID)
{
    IPF_CHL_CTRL_S stCtrl;
    IPF_FILTER_CONFIG_S stDlFilterInfo[50];
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;
	
    ipf_ipv4_head.Vers = 4;
    ipf_ipv4_head.HeaderLen = 5;
    ipf_ipv4_head.TOS = 3;
    ipf_ipv4_head.TOL = 1200;
    ipf_ipv4_head.ID = 1;
    ipf_ipv4_head.ReservFlag = 0;
    ipf_ipv4_head.DF = 0;
    ipf_ipv4_head.MF = 0;
    ipf_ipv4_head.offset = 0;
    ipf_ipv4_head.TTL = 128;
    ipf_ipv4_head.Prot = 17; /* UDP */
    ipf_ipv4_head.Checksum = 0xEC3A;
    ipf_ipv4_head.SourceAddr = 0xACA84649;
    ipf_ipv4_head.DestAddr = 0x0202014E;

    ipf_udp.SrcPort = 5088;
    ipf_udp.DstPort = 2923;
    ipf_udp.Checksum = 0;
    ipf_udp.Length = ipf_ipv4_head.TOL;
    memset(stDlFilterInfo, 0x0, 50*sizeof(IPF_FILTER_CONFIG_S));
    for(i=0;i<49;i++)
    {
        stDlFilterInfo[i].u32FilterID = i;
        stDlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0xffffffff;
    }
    
    stDlFilterInfo[49].u32FilterID = 172;
    stDlFilterInfo[49].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0x23C01;
    *(BSP_U32*)(stDlFilterInfo[49].stMatchInfo.u8SrcAddr) = 0xACA84649;
    *(BSP_U32*)(stDlFilterInfo[49].stMatchInfo.u8DstAddr) = 0x0202014E;
    *(BSP_U32*)(stDlFilterInfo[49].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
    stDlFilterInfo[49].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = 1;
    stDlFilterInfo[49].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = 5100;
    stDlFilterInfo[49].stMatchInfo.unDstPort.Bits.u16DstPortLo = 10;
    stDlFilterInfo[49].stMatchInfo.unDstPort.Bits.u16DstPortHi = 5100;
    stDlFilterInfo[49].stMatchInfo.unTrafficClass.u32TrafficClass  = 3|(0xFF<<8);
    stDlFilterInfo[49].stMatchInfo.u32LocalAddressMsk = 0x00;
    stDlFilterInfo[49].stMatchInfo.unNextHeader.u32Protocol = 1;
    stDlFilterInfo[49].stMatchInfo.unFltCodeType.Bits.u16Type = 2;
    stDlFilterInfo[49].stMatchInfo.unFltCodeType.Bits.u16Code = 3;
    stDlFilterInfo[49].stMatchInfo.u32FltSpi = 4;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    stCtrl.bDataChain = BSP_TRUE;
    stCtrl.bEndian = BSP_TRUE; /* 小端 */
    stCtrl.eIpfMode = IPF_MODE_FILTERONLY;
    stCtrl.u32WrrValue = 0;
    s32Ret = BSP_IPF_ConfigDlChannel(&stCtrl);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlChannel :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }

    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_DLFC, stDlFilterInfo, 50);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
    


    s32Ret = BSP_IPF_ChannelEnable(IPF_CHANNEL_DOWN, BSP_TRUE);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
    return OK;
}
#endif


#ifdef __KERNEL__
struct semaphore g_IpfDlTaskSemId;
struct semaphore g_IpfDlIntSemId;
struct task_struct * g_IpfDlTaskId;

BSP_U8  * g_pu8STDlData = BSP_NULL;

typedef int (*IPFTHREADFN)(void *pdata);
/*CH1为下行通道，故编到Acore*/
BSP_VOID print_CH1_BUFFED_AD_Num(BSP_VOID)
{
	HI_IPF_CH1_ADQ0_STAT_T ad0_stat;
	HI_IPF_CH1_ADQ1_STAT_T ad1_stat;
	ad0_stat.u32 = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_STAT_OFFSET);
	ad1_stat.u32 = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_STAT_OFFSET);

	if((ad0_stat.bits.dl_adq0_buf_epty == 1)&&(ad0_stat.bits.dl_adq0_buf_full == 1))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD0_BUF_FLAG_ERROR.\n");
	}
	else if(ad0_stat.bits.dl_adq0_buf_full == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD0_BUF_NUM 2,2 unused AD in AD_Buff.\n");
	}
	else if(ad0_stat.bits.dl_adq0_buf_epty == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD0_BUF_NUM 0,0 unused AD in AD_Buff.\n");
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD0_BUF_NUM 1,1 unused AD in AD_Buff.\n");
	}
	
	if((ad1_stat.bits.dl_adq1_buf_epty == 1)&&(ad1_stat.bits.dl_adq1_buf_full == 1))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD1_BUF_FLAG_ERROR.\n");
	}
	else if(ad1_stat.bits.dl_adq1_buf_full == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD1_BUF_NUM 2,2 unused AD in AD_Buff.\n");
	}
	else if(ad1_stat.bits.dl_adq1_buf_epty == 1)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD1_BUF_NUM 0,0 unused AD in AD_Buff.\n");
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r CH1_AD1_BUF_NUM 1,1 unused AD in AD_Buff.\n");
	}
	return;
}


#ifdef CONFIG_MODULE_BUSSTRESS

BSP_S32 IpfDlIntCb(BSP_VOID)
{
    up(&g_IpfDlIntSemId);
    return OK;
}
BSP_VOID IPF_DL_Task_1(BSP_VOID)
{
    BSP_U32 u32Num = 0;
    IPF_RD_DESC_S stRd[IPF_ULRD_DESC_SIZE];
    BSP_U32 i = 0;
    BSP_U32 u32AD0Num,u32AD1Num,u32AD0wptr,u32AD1wptr;
    BSP_S32 s32Ret;
    while(1)
    { 
        memset(stRd, 0x0, sizeof(IPF_RD_DESC_S) * IPF_ULRD_DESC_SIZE);
        u32Num = IPF_ULRD_DESC_SIZE;
        down(&g_IpfDlIntSemId);
#if 0
        if(*g_ipftestdebug)
        {
            IPF_PRINT("====in====\n");
        }
#endif
        BSP_IPF_GetDlRd(&u32Num, stRd);
        for(i = 0; i < u32Num; i++)
        {
            g_u32DlTotalBDNum++;
        }
	 s32Ret = BSP_IPF_GetDlAdNum(&u32AD0Num, &u32AD1Num);
	 {
            if(IPF_SUCCESS != s32Ret)
            {
      	  		    IPF_PRINT(" TESTERROR    Get dl ad num fail :%d\n",__LINE__);
            }
	 }
        if(u32AD0Num > 10)
        {
        u32AD0wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET );
        if((IPF_ULAD0_DESC_SIZE -u32AD0wptr) >= u32AD0Num)
        {
            s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,u32AD0Num,&ADdlshort[u32AD0wptr]);
            if(IPF_SUCCESS != s32Ret)
            {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
            }
        }
        else
        {
            s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,(IPF_ULAD0_DESC_SIZE-u32AD0wptr),&ADdlshort[u32AD0wptr]);
            if(IPF_SUCCESS != s32Ret)
            {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
            }
            s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,(u32AD0Num-(IPF_ULAD0_DESC_SIZE-u32AD0wptr)),&ADdlshort[0]);
            if(IPF_SUCCESS != s32Ret)
            {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
            }
        }
        for(i=0;i < u32AD0Num;i++)
        {
            memset((BSP_U8 *)ADdlshort[u32AD0wptr].u32OutPtr1, 1, 438);
		u32AD0wptr = ((u32AD0wptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32AD0wptr + 1) : 0;		
        }
    }
    if(u32AD1Num > 10)
    {
	u32AD1wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET );
	if((IPF_ULAD1_DESC_SIZE-u32AD1wptr) >= u32AD1Num)
	{
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,u32AD1Num,&ADdllong[u32AD1wptr]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
      	  		    IPF_PRINT(" u32AD1Num = %u   u32AD1wptr = %u \n",u32AD1Num,u32AD1wptr);
			    return;
	      	 }
	}
	else
	{
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,(IPF_ULAD1_DESC_SIZE-u32AD1wptr),&ADdllong[u32AD1wptr]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
			    return;
	      	 }
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,(u32AD1Num-(IPF_ULAD1_DESC_SIZE-u32AD1wptr)),&ADdllong[0]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
			    return;
	      	 }
	}
        for(i=0;i < u32AD1Num;i++)
        {
            memset((BSP_U8 *)ADdllong[u32AD1wptr].u32OutPtr1, 2, 1500);
		u32AD1wptr = ((u32AD1wptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32AD1wptr + 1) : 0;		
        }
    }
#if 0
        if(*g_ipftestdebug)
        {
            IPF_PRINT("====out====\n");
        }
#endif
        up(&g_IpfDlTaskSemId);
    }
}
BSP_S32 IPF_ST_DL_INIT_STRESS(BSP_VOID)
{
    sema_init(&g_IpfDlTaskSemId, 0);
    sema_init(&g_IpfDlIntSemId, 0);

    g_IpfDlTaskId = kthread_run((IPFTHREADFN)IPF_DL_Task_1, BSP_NULL, "IpfDlTask");  

    return OK;
}
#endif

BSP_S32 CmpDMAData(BSP_U32* p1, BSP_U32* p2, BSP_U32 u32Len);
BSP_S32 IpfDlIntSTCb(BSP_VOID)
{
    print_CH1_BUFFED_AD_Num();
    return OK;
}
/*用于验证AD_BUFF的有效性*/
BSP_S32 IpfDlIntCb300(BSP_VOID)
{

	BSP_U32 u32AD0Num;
	BSP_U32 u32AD1Num;
	BSP_U32 u32Num = IPF_DLRD_DESC_SIZE;
	IPF_RD_DESC_S stRd[IPF_DLRD_DESC_SIZE];
	
	BSP_IPF_GetDlRd(&u32Num, stRd);

	IPF_PRINT("Data transfer Num:%d \n",u32Num);
	
	print_CH1_BUFFED_AD_Num();

	BSP_IPF_GetDlAdNum(&u32AD0Num, &u32AD1Num);
	IPF_PRINT("Data transfer success! Num: %u \n AD0Num:%u AD1Num:%d \n",u32Num,u32AD0Num,u32AD1Num);
	IPF_PRINT("Data transfer ERROR Num:%d \n",g_u32DlError);
    return OK;
}

BSP_S32 IpfDlIntCb301(BSP_VOID)
{
	BSP_U32 i,j;
	BSP_U32 u32AD0Num;
	BSP_U32 u32AD1Num;
	BSP_U32 u32Num = IPF_DLRD_DESC_SIZE;
	BSP_S32 s32Ret = 0;
	IPF_RD_DESC_S stRd[IPF_DLRD_DESC_SIZE];
	BSP_U32* p = NULL;
	
	BSP_IPF_GetDlRd(&u32Num, stRd);

	IPF_PRINT("Data transfer Num:%d \n",u32Num);
	
	for(j = 0; j < u32Num; j++)
	{	
		//IPF_PRINT("stRd[%d].u32OutPtr =%x\n",j,stRd[j].u32OutPtr);
		if(NULL == (BSP_U32 *)stRd[j].u32OutPtr)
		{
			//IPF_PRINT("stRd[%d].u32OutPtr =NULL !\n",i);
			return ERROR;
		}
		s32Ret = CmpDMAData((BSP_U32 *)g_pu8STDlData, (BSP_U32 *)stRd[j].u32OutPtr, stRd[j].u16PktLen);
		
		/*数据比对*/
		if(0 != s32Ret)
		{
		    bsp_trace(BSP_LOG_LEVEL_FATAL, BSP_MODU_IPF, "\r Data transfer unmatch! Num:%d \n",j,0,0,0,0,0,0);
		    IPF_PRINT("Comp error \n");
		    IPF_PRINT("g_pu8STDlData\n");
		    IPF_PRINT("===================================\n");
		    p=(BSP_U32 *)g_pu8STDlData;
		    for(i=0; i<25; i++)
		    {
		        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
		        i = i+4;
		    }
		    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));
		    IPF_PRINT("stRd[%d].u32OutPtr\n",j);
                  IPF_PRINT("===================================\n");
                  IPF_PRINT("stRd[%d].u32OutPtr =%x\n",j,stRd[j].u32OutPtr);
		    p=(BSP_U32 *)stRd[j].u32OutPtr;
		    for(i=0; i<25; i++)
		    {
		        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
		        i = i+4;
		    }
		    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));

		}			
	}

	BSP_IPF_GetDlAdNum(&u32AD0Num, &u32AD1Num);
	IPF_PRINT("Data transfer success! Num: %d \n AD0Num:%d AD1Num:%d \n",j,u32AD0Num,u32AD1Num);
	IPF_PRINT("Data transfer ERROR Num:%d \n",g_u32DlError);
    return OK;
}

BSP_S32 IpfDlIntCb302(BSP_VOID)
{
	BSP_U32 i,j;
	BSP_U32 u32AD0Num;
	BSP_U32 u32AD1Num;
	BSP_U32 u32Num = IPF_DLRD_DESC_SIZE;
	BSP_S32 s32Ret = 0;
	IPF_RD_DESC_S stRd[IPF_DLRD_DESC_SIZE];
	
	BSP_IPF_GetDlRd(&u32Num, stRd);

	IPF_PRINT("Data transfer Num:%d \n",u32Num);
	
	for(j = 0; j < u32Num; j++)
	{	
		IPF_PRINT("stRd[%d].u32OutPtr =%x stRd[%d].u16PktLen =%u \n",j,stRd[j].u32OutPtr,j,stRd[j].u16PktLen);
		if(NULL == (BSP_U32 *)stRd[j].u32OutPtr)
		{
			IPF_PRINT("stRd[%d].u32OutPtr =NULL !\n",j);
			return ERROR;
		}
		s32Ret = CmpDMAData((BSP_U32 *)g_pu8STDlData, (BSP_U32 *)stRd[j].u32OutPtr, stRd[j].u16PktLen);
		
		/*数据比对*/
		if(0 != s32Ret)
		{
		#ifdef printcmpresult
		    IPF_PRINT("Comp error \n");
		    IPF_PRINT("g_pu8STDlData\n");
		    IPF_PRINT("===================================\n");
		    p=(BSP_U32 *)g_pu8STDlData;
		    for(i=0; i<200; i++)
		    {
		        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
		        i = i+4;
		    }
		    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));
		    IPF_PRINT("stRd[%d].u32OutPtr\n",j);
                  IPF_PRINT("===================================\n");
                  IPF_PRINT("stRd[%d].u32OutPtr =%x\n",j,stRd[j].u32OutPtr);
		    p=(BSP_U32 *)stRd[j].u32OutPtr;
		    for(i=0; i<200; i++)
		    {
		        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
		        i = i+4;
		    }
		    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));
		#endif
                  g_u32DlError++;
		}			
	}

	BSP_IPF_GetDlAdNum(&u32AD0Num, &u32AD1Num);
	IPF_PRINT("Data transfer success! Num: %d \n AD0Num:%d AD1Num:%d \n",j,u32AD0Num,u32AD1Num);
	IPF_PRINT("Data transfer ERROR Num:%d \n",g_u32DlError);
	if(u32AD0Num > 10)
	{
	        s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,u32AD0Num-1,&ADdlshort[0]);
	        if(IPF_SUCCESS != s32Ret)
	        {
	            IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlAd0 :%d\n",__LINE__);
	        }
	        for(i=0;i < u32AD0Num;i++)
	        {
	            memset((BSP_U32 *)ADdlshort[i].u32OutPtr1, 0, 438);
	        }
	}
	if(u32AD1Num > 10)
	{
	        s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,u32AD1Num-1,&ADdllong[0]);
	        if(IPF_SUCCESS != s32Ret)
	        {
	            IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlAd1 :%d\n",__LINE__);
	        }
	        for(i=0;i < u32AD1Num;i++)
	        {
	            memset((BSP_U32 *)ADdllong[i].u32OutPtr1, 0, 1500);
	        }

	}

	return OK;
}

BSP_S32 IpfDlIntCb304(BSP_VOID)
{
	BSP_U32 j;
	BSP_U32 u32AD0Num;
	BSP_U32 u32AD1Num;
	BSP_U32 u32Num = IPF_DLRD_DESC_SIZE;
	IPF_RD_DESC_S stRd[IPF_DLRD_DESC_SIZE];
	
	BSP_IPF_GetDlRd(&u32Num, stRd);

	IPF_PRINT("Data transfer Num:%d \n",u32Num);
	
	for(j = 0; j < u32Num; j++)
	{	
		
		/*过滤结果*/
		if(stRd[j].u16Result == 0x3f)
		{
			IPF_PRINT("stRd[%d].u16Result ERROR ! value is 0x%x \n",j,stRd[j].u16Result);
			g_u32DlError++;
		}			
	}

	BSP_IPF_GetDlAdNum(&u32AD0Num, &u32AD1Num);
	
	IPF_PRINT("Data transfer success! Num: %d \n AD0Num:%d AD1Num:%d \n",j,u32AD0Num,u32AD1Num);
	IPF_PRINT("Data transfer ERROR Num:%d \n",g_u32DlError);
    return OK;
}


BSP_S32 IPF_ST_DL_FlushAD(IPF_ADQ_EMPTY_E eAdqEmptyDl)
{

	BSP_S32 s32Ret;  
	if(CONFIG_AD !=  ipf_config_ad_flag)
	{
		return OK;
	}
	if(IPF_EMPTY_ADQ0 == eAdqEmptyDl)
	{
		s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,IPF_DLAD0_DESC_SIZE-4,&ADdlshort[0]);
		if(s32Ret != IPF_SUCCESS)
		{
		IPF_PRINT(" TESTERROR    IPF_ST_UL_FlashAD :%d s32ret = %x\n",__LINE__, s32Ret);
		return ERROR;
		}
	}
	else if(IPF_EMPTY_ADQ1 == eAdqEmptyDl)
	{
		s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,IPF_DLAD1_DESC_SIZE-4,&ADdllong[0]);
		if(s32Ret != IPF_SUCCESS)
		{
			IPF_PRINT(" TESTERROR    IPF_ST_UL_FlashAD :%d s32ret = %x\n",__LINE__, s32Ret);
			return ERROR;
		}
	}
	else
	{
		return ERROR;
	}
	return OK;
}

BSP_S32 IPF_ST_DL_FlushOneAD(BSP_U32 AD0Num, BSP_U32 AD1Num)
{
    BSP_S32 s32Ret;  
    if(CONFIG_AD !=  ipf_config_ad_flag)
    {
        return OK;
    }

    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,AD0Num,&ADdlshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT("TEST ERROR BSP_IPF_ConfigDlAd :%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }

    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,AD1Num,&ADdllong[0]);

	if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT("TEST ERROR BSP_IPF_ConfigUlAd:%d s32ret = %x\n",__LINE__, s32Ret);
        return ERROR;
    }
	return OK;
}


BSP_S32 IPF_ST_DL_ACore_INIT(BSP_VOID)
{
	BSP_S32 s32Ret;
	BSP_U32 i;
	BSP_VOID * p_vir_addr;

    ipf_ipv4_head.Vers = 4;
    ipf_ipv4_head.HeaderLen = 5;
    ipf_ipv4_head.TOS = 3;
    ipf_ipv4_head.TOL = 1200;
    ipf_ipv4_head.ID = 1;
    ipf_ipv4_head.ReservFlag = 0;
    ipf_ipv4_head.DF = 0;
    ipf_ipv4_head.MF = 0;
    ipf_ipv4_head.offset = 0;
    ipf_ipv4_head.TTL = 128;
    ipf_ipv4_head.Prot = 17; /* UDP */
    ipf_ipv4_head.Checksum = 0xEC3A;
    ipf_ipv4_head.SourceAddr = 0xACA84649;
    ipf_ipv4_head.DestAddr = 0x0202014E;

    ipf_udp.SrcPort = 5088;
    ipf_udp.DstPort = 2923;
    ipf_udp.Checksum = 0;
    ipf_udp.Length = ipf_ipv4_head.TOL;

	for(i=0; i < IPF_DLAD0_DESC_SIZE; i++)
	{    	
    		ADdlshort[i].u32OutPtr0 = (BSP_U32)NULL;    
		p_vir_addr = kmalloc(438, GFP_KERNEL);    
		memset(p_vir_addr,0x0,438);
		ADdlshort[i].u32OutPtr0 = virt_to_phys(p_vir_addr);/*dma_map_single(NULL, p_vir_addr, 438, DMA_TO_DEVICE);    */    
		ADdlshort[i].u32OutPtr1 = (BSP_U32)p_vir_addr;

		dma_map_single(NULL, p_vir_addr, 438, DMA_TO_DEVICE);

		if(NULL == (BSP_U32 *)ADdlshort[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error1;
			}
	}
	for(i=0; i < IPF_DLAD1_DESC_SIZE; i++)
	{
    		ADdllong[i].u32OutPtr0 = (BSP_U32)NULL;
		p_vir_addr = kmalloc(1500, GFP_KERNEL);  
		memset(p_vir_addr,0x0,1500);
		ADdllong[i].u32OutPtr0 = virt_to_phys(p_vir_addr);        
    		ADdllong[i].u32OutPtr1 = (BSP_U32)p_vir_addr;    

		dma_map_single(NULL, p_vir_addr ,1500,DMA_TO_DEVICE);
		if(NULL == (BSP_U32 *)ADdllong[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error1;
			}
	}
	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,IPF_DLAD0_DESC_SIZE-2,&ADdlshort[0]);
	if(s32Ret != IPF_SUCCESS)
	{
		IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlAd :%d s32Ret = %x\n",__LINE__, s32Ret);
		return BSP_ERROR;
	}    
	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,IPF_DLAD1_DESC_SIZE-2,&ADdllong[0]);
	if(s32Ret != IPF_SUCCESS)
	{
		IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlAd :%d s32Ret = %x\n",__LINE__, s32Ret);
		return BSP_ERROR;
	}
    return OK;

error1:
	do
	{
		i--;
		kfree((BSP_U32 *)(ADdlshort[i].u32OutPtr0));
	}while(i);
	return ERROR;

	do
	{
		i--;
		kfree((BSP_U32 *)(ADdllong[i].u32OutPtr0));
	}while(i);
	for(i=0; i < IPF_DLAD0_DESC_SIZE; i++)
	{
		kfree((BSP_U32 *)(ADdlshort[i].u32OutPtr0));
	}
	return ERROR;
}
#endif
BSP_S32 CmpDMAData(BSP_U32* p1, BSP_U32* p2, BSP_U32 u32Len)
{
    BSP_U32 i = 0;
    for(i=0; i<u32Len/4; i++)
    {
        if(*(BSP_U32*)(p1+i) != *(BSP_U32*)(p2+i))
        {
            return ERROR;
        }
    }
    return 0;
}

/****************************************
* 0   两核各自都用的测试用例
* 1   先执行M核后执行A核用例
* 2   C   核的测试用例
* 3   先执行A核后执行M核用例
* x2x   压力测试用例
****************************************/
/*检验是否Init成功*/
BSP_S32  BSP_IPF_AINIT(BSP_VOID)
{
	BSP_S32 ret;
	IPF_PRINT("Start call acore init \n");
	ret = ipf_init();
	return ret;
}

BSP_S32 BSP_IPF_ST_000(BSP_VOID)
{
	BSP_U32 ipf_Shared_ddr_start = SHM_MEM_IPF_ADDR;
	
	BSP_U32 ipf_Shared_ddr_ul_start = IPF_ULBD_MEM_ADDR;

	BSP_U32 ipf_Shared_ddr_filter_pwc_start = IPF_PWRCTL_BASIC_FILTER_ADDR;

	BSP_U32 ipf_Shared_ddr_pwc_info_start = IPF_PWRCTL_INFO_ADDR;

	BSP_U32 ipf_Shared_ddr_dlcdrptr = IPF_DLCDRPTR_MEM_ADDR;

	BSP_U32 ipf_Shared_ddr_debug_dlcd_start = IPF_DEBUG_DLCD_ADDR;
	BSP_U32 ipf_Shared_ddr_debug_dlcd_size = IPF_DEBUG_DLCD_SIZE;
	BSP_U32 ipf_Shared_ddr_end = IPF_DEBUG_INFO_END_ADDR;
	BSP_U32 ipf_reg_basic_addr = HI_IPF_REGBASE_ADDR;
	BSP_U32 ipf_reg_basic_addr_virt = HI_IPF_REGBASE_ADDR_VIRT;
		
	IPF_PRINT("ipf_Shared_ddr_start                    value is 0x%x \n", ipf_Shared_ddr_start);
	IPF_PRINT("ipf_Shared_ddr_ul_start                value is 0x%x \n", ipf_Shared_ddr_ul_start);
	IPF_PRINT("ipf_Shared_ddr_filter_pwc_start     value is 0x%x \n", ipf_Shared_ddr_filter_pwc_start);
	IPF_PRINT("ipf_Shared_ddr_pwc_info_start      value is 0x%x \n", ipf_Shared_ddr_pwc_info_start);
	IPF_PRINT("ipf_Shared_ddr_dlcdrptr                value is 0x%x \n", ipf_Shared_ddr_dlcdrptr);
	IPF_PRINT("ipf_Shared_ddr_debug_dlcd_start   value is 0x%x \n", ipf_Shared_ddr_debug_dlcd_start);
	IPF_PRINT("ipf_Shared_ddr_debug_dlcd_size    value is 0x%x \n", ipf_Shared_ddr_debug_dlcd_size);
	IPF_PRINT("ipf_Shared_ddr_end                     value is 0x%x \n", ipf_Shared_ddr_end);
	IPF_PRINT("ipf_reg_basic_addr                     value is 0x%x \n", ipf_reg_basic_addr);
	IPF_PRINT("ipf_reg_basic_addr_virt                     value is 0x%x \n", ipf_reg_basic_addr_virt);
	return IPF_SUCCESS;
}
/* test BSP_IPF_ConfigTimeout */
BSP_S32 BSP_IPF_ST_001(BSP_VOID)
{
    BSP_U32 u32Timeout = 0;
    BSP_U32 u32OldTime = 0;
    BSP_S32 s32ret = 0;

    u32OldTime = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_TIME_OUT_OFFSET);

    s32ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        goto error;
    }

    u32Timeout = 1;
    s32ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        goto error;
    }

    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_TIME_OUT_OFFSET) != 0x10001)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        goto error;
    }

    u32Timeout = 0xffff;
    s32ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        goto error;
    }
	

    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_TIME_OUT_OFFSET) != 0x1ffff)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        goto error;
    }

    u32Timeout = 0x10000;
    s32ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        goto error;
    }

    ipf_writel(u32OldTime, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_TIME_OUT_OFFSET);
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);
    return OK;
error:
    ipf_writel(u32OldTime, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_TIME_OUT_OFFSET);
    return ERROR;
}


BSP_S32 BSP_IPF_ST_002(BSP_VOID)
{
    BSP_S32 s32Ret;
    BSP_U32 u32MaxLen = 0;
    BSP_U32 u32MinLen = 0;

    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetPktLen1 :%d\n",__LINE__);
        return ERROR;
    }

    u32MaxLen = 0x5;
    u32MinLen = 0x6;
    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetPktLen2 :%d\n",__LINE__);
        return ERROR;
    }

    u32MaxLen = 0x345;
    u32MinLen = 0x123;
    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetPktLen3 :%d\n",__LINE__);
        return ERROR;
    }
    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_PKT_LEN_OFFSET) != 0x3450123)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetPktLen4 :%d\n",__LINE__);
        return ERROR;
    }

    u32MaxLen = 1500;
    u32MinLen = 40;
    s32Ret = BSP_IPF_SetPktLen(u32MaxLen,u32MinLen);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetPktLen5 :%d\n",__LINE__);
        return ERROR;
    }
    
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);
    return OK;
}


BSP_S32 BSP_IPF_ST_003(BSP_VOID)
{
    BSP_IPF_Help();
    BSP_IPF_Dump_BDInfo(0);
    BSP_IPF_Dump_BDInfo(1);
    BSP_IPF_Dump_RDInfo(0);
    BSP_IPF_Dump_RDInfo(1);
    BSP_IPF_Dump_ADInfo(0,0);
    BSP_IPF_Dump_ADInfo(0,1);
    BSP_IPF_Dump_ADInfo(1,0);
    BSP_IPF_Dump_ADInfo(1,1);
    BSP_IPF_Info(0);
    BSP_IPF_Info(1);
    BSP_IPF_MEM();
    return IPF_SUCCESS;
}

BSP_S32 BSP_IPF_ST_005(BSP_VOID)
{
    BSP_S32 s32Ret;
    BSP_U32 i;
IPF_AD_DESC_S *TempAD;
#ifdef __VXWORKS__
	for(i=0; i < IPF_ULAD0_DESC_SIZE; i++)
	{
    		ADulshort[i].u32OutPtr0 = (BSP_U32)NULL;
		ADulshort[i].u32OutPtr0 = (BSP_U32)malloc(404);
		ADulshort[i].u32OutPtr1 = ADulshort[i].u32OutPtr0;
		if((BSP_U32)NULL == ADulshort[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error1;
			}
	}
	
	for(i=0; i < IPF_ULAD1_DESC_SIZE; i++)
	{
    		ADullong[i].u32OutPtr0 = (BSP_U32)NULL;
		ADullong[i].u32OutPtr0 = (BSP_U32)malloc(1500);
		ADullong[i].u32OutPtr1 = ADulshort[i].u32OutPtr0;

		if((BSP_U32)NULL == ADullong[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error2;
			}
	}


    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,IPF_ULAD0_DESC_SIZE-IPF_ADQ_RESERVE_NUM,&ADulshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }
	TempAD = (IPF_AD_DESC_S*)(IPF_ULAD0_MEM_ADDR);

	for(i=0; i <IPF_ULAD0_DESC_SIZE-IPF_ADQ_RESERVE_NUM;i++)
	{

		if( ADulshort[i].u32OutPtr0 != TempAD[i].u32OutPtr0)
			{
			  IPF_PRINT("ST_005_TEST7 ADprt err %d  ADNum %x MEM %d AD %x\n",
			  __LINE__,i, (ADulshort[i].u32OutPtr0), TempAD[i].u32OutPtr0);
			}
	}
        IPF_PRINT(" BSP_IPF_ST_005_TEST7 comp %d\n",__LINE__);

		
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,IPF_ULAD1_DESC_SIZE-IPF_ADQ_RESERVE_NUM,&ADullong[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }
	for(i=0; i < IPF_ULAD1_DESC_SIZE-IPF_ADQ_RESERVE_NUM;i++)
	{
	TempAD = (IPF_AD_DESC_S*)(IPF_ULAD1_MEM_ADDR);
		if(ADullong[i].u32OutPtr0 != TempAD[i].u32OutPtr0)
			{
			  IPF_PRINT(" ST_005_TEST8 ADprt err %d  ADNum %x MEM %d AD %x\n",
			  __LINE__,i,(ADullong[i].u32OutPtr0),  TempAD[i].u32OutPtr0);
			}
	}

	IPF_PRINT(" BSP_IPF_ST_005_TEST8 comp %d\n",__LINE__);



    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,IPF_ULAD0_DESC_SIZE,&ADulshort[0]);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
        IPF_PRINT(" BSP_IPF_ST_005_TEST1 comp %d\n",__LINE__);
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,IPF_ULAD1_DESC_SIZE,&ADullong[0]);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
        IPF_PRINT(" BSP_IPF_ST_005_TEST2 comp %d\n",__LINE__);
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,1,NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
        IPF_PRINT(" BSP_IPF_ST_005_TEST3 comp %d\n",__LINE__);
	
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,1,NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
        IPF_PRINT(" BSP_IPF_ST_005_TEST4 comp %d\n",__LINE__);
		
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,0,&ADulshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }

	
    s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,0,&ADullong[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }
        IPF_PRINT(" BSP_IPF_ST_005_TEST6 comp %d\n",__LINE__);
	
	for(i=0; i < IPF_ULAD0_DESC_SIZE; i++)
	{
		free((BSP_U32 *)ADulshort[i].u32OutPtr0);
	}

	for(i=0; i < IPF_ULAD1_DESC_SIZE; i++)
	{
		free((BSP_U32 *)ADullong[i].u32OutPtr0);
	}

    IPF_PRINT(" BSP_IPF_ST_005 SUCCESS");
    return OK;

error1:
	do
	{
		i--;
		free((BSP_U32 *)ADulshort[i].u32OutPtr0);
	}while(i);
	return ERROR;
error2:
	do
	{
		i--;
		free((BSP_U32 *)ADullong[i].u32OutPtr0);
	}while(i);
	for(i=0; i < IPF_ULAD0_DESC_SIZE; i++)
	{
		free((BSP_U32 *)ADulshort[i].u32OutPtr0);
	}
	return ERROR;


#else
	for(i=0; i < IPF_DLAD0_DESC_SIZE; i++)
	{
    		ADdlshort[i].u32OutPtr0 = (BSP_U32)NULL;
		ADdlshort[i].u32OutPtr0 = (BSP_U32)kmalloc(438,GFP_KERNEL);
		ADdlshort[i].u32OutPtr1 = ADdlshort[i].u32OutPtr0;

		if(NULL == (BSP_U32 *)ADdlshort[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error1;
			}
	}
	for(i=0; i < IPF_DLAD1_DESC_SIZE; i++)
	{
    		ADdllong[i].u32OutPtr0 = (BSP_U32)NULL;
		ADdllong[i].u32OutPtr0 = (BSP_U32)kmalloc(1500,GFP_KERNEL);
		ADdllong[i].u32OutPtr1 = ADdllong[i].u32OutPtr0;

		if(NULL == (BSP_U32 *)ADdllong[i].u32OutPtr0)
			{
		            IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT ADMEMmallocfail:%d\n",__LINE__);
		            goto error2;
			}
	}
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,IPF_DLAD0_DESC_SIZE-IPF_ADQ_RESERVE_NUM,&ADdlshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR7    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }

	TempAD = (IPF_AD_DESC_S*)(IPF_DLAD0_MEM_ADDR);

	for(i=0; i < IPF_DLAD0_DESC_SIZE-IPF_ADQ_RESERVE_NUM;i++)
	{

		if(ADdlshort[i].u32OutPtr0 != TempAD[i].u32OutPtr0)
			{
			  IPF_PRINT("ST_005_TEST7 ADprt err %d  ADNum %d MEM %d AD %d\n",
			  __LINE__,i, (ADdlshort[i].u32OutPtr0), TempAD[i].u32OutPtr0);
			}
	}
        IPF_PRINT(" BSP_IPF_ST_005_TEST7 comp %d\n",__LINE__);
	
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,IPF_DLAD1_DESC_SIZE-IPF_ADQ_RESERVE_NUM,&ADdllong[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR8    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }

	TempAD = (IPF_AD_DESC_S*)(IPF_DLAD1_MEM_ADDR);

	for(i=0; i < IPF_DLAD1_DESC_SIZE-IPF_ADQ_RESERVE_NUM;i++)
	{

		if(ADdllong[i].u32OutPtr0 != TempAD[i].u32OutPtr0)
			{
			  IPF_PRINT("ST_005_TEST8 ADprt err %d  ADNum %d MEM %d AD %d\n",
			  __LINE__,i, (ADdllong[i].u32OutPtr0), TempAD[i].u32OutPtr0);
			}
	}
        IPF_PRINT(" BSP_IPF_ST_005_TEST8 comp %d\n",__LINE__);
		
		for(i=0; i < IPF_DLAD0_DESC_SIZE; i++)
	{
		kfree((BSP_U32 *)(ADdlshort[i].u32OutPtr0));
	}

	for(i=0; i < IPF_DLAD1_DESC_SIZE; i++)
	{
		kfree((BSP_U32 *)(ADdllong[i].u32OutPtr0));
	}

    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,IPF_ULAD0_DESC_SIZE,&ADdlshort[0]);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR1   BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,IPF_ULAD1_DESC_SIZE,&ADdllong[0]);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR2    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,1,NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR3    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,1,NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR4    BSP_IPF_ConfigUlAd:%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,0,&ADdlshort[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR5    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,0,&ADdllong[0]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR6    BSP_IPF_ConfigUlAd :%d\n",__LINE__);
        return ERROR;
    }
	

	    return OK;
error1:
	do
	{
		i--;
		kfree((BSP_U32 *)(ADdlshort[i].u32OutPtr0));
	}while(i);
	return ERROR;
error2:
	do
	{
		i--;
		kfree((BSP_U32 *)(ADdllong[i].u32OutPtr0));
	}while(i);
	for(i=0; i < IPF_ULAD0_DESC_SIZE; i++)
	{
		kfree((BSP_U32 *)(ADdlshort[i].u32OutPtr0));
	}
	return ERROR;

#endif
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);
}


	
#ifdef __VXWORKS__
extern BSP_S32 BSP_IPF_ST_111_INIT(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_112_INIT(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_113_INIT(BSP_VOID);

/* test BSP_IPF_Init */
BSP_S32 BSP_IPF_ST_201(BSP_VOID)
{
    BSP_S32 s32Ret;
    BSP_U32 u32OldPara = 0;
    IPF_COMMON_PARA_S stCommPara;

    u32OldPara = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CTRL_OFFSET); 
    s32Ret = BSP_IPF_Init(BSP_NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_Init1 :%d\n",__LINE__);
        goto error;
    }

    stCommPara.bAdReport = 1; /*上报数据指针*/
    stCommPara.bMultiFilterChainEn = 0;/*多寄存器链模式*/
    stCommPara.bMultiModeEn = 0;/*动态业务模式*/
    stCommPara.bEspSpiDisable = BSP_TRUE;
    stCommPara.bAhSpiDisable = BSP_TRUE;
    stCommPara.bEspAhSel = BSP_TRUE;
    stCommPara.bIpv6NextHdSel = BSP_TRUE;
    stCommPara.eMaxBurst = IPF_BURST_8;
    stCommPara.bSpWrrModeSel = BSP_TRUE;
    stCommPara.bSpPriSel = BSP_TRUE;
    stCommPara.bFltAddrReverse = BSP_TRUE;
    stCommPara.bFilterSeq = BSP_TRUE;       

    s32Ret = BSP_IPF_Init(&stCommPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_Init2 :%d\n",__LINE__);
        goto error;
    }
	
    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CTRL_OFFSET) != 0x13DF)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_Init3 :%d  %x\n",__LINE__,*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CTRL_OFFSET));
        goto error;
    }

    stCommPara.bEspSpiDisable = BSP_FALSE;
    stCommPara.bAhSpiDisable = BSP_FALSE;
    stCommPara.bEspAhSel = BSP_FALSE;
    stCommPara.bIpv6NextHdSel = BSP_FALSE;
    stCommPara.eMaxBurst = IPF_BURST_4;
    stCommPara.bSpWrrModeSel = BSP_FALSE;
    stCommPara.bSpPriSel = BSP_FALSE;
    stCommPara.bFltAddrReverse = BSP_FALSE;
    stCommPara.bFilterSeq = BSP_FALSE;       
    stCommPara.bAdReport = 0; /*上报数据指针*/
    stCommPara.bMultiFilterChainEn = 1;/*多寄存器链模式*/
    stCommPara.bMultiModeEn = 1;/*动态业务模式*/

    s32Ret = BSP_IPF_Init(&stCommPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_Init4 :%d\n",__LINE__);
        goto error;
    }	
    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CTRL_OFFSET) != 0xC20)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_Init5 :%d %x\n",__LINE__,*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CTRL_OFFSET));
       goto error;
    }

    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CTRL_OFFSET); 
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);    
    return OK;
error:
    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CTRL_OFFSET); 
    return ERROR;
}

BSP_S32 BSP_IPF_ST_202(BSP_VOID)
{
    BSP_S32 s32Ret;
    BSP_U32 u32OldPara = 0;
    IPF_CHL_CTRL_S stCtrl;

    u32OldPara = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET); 
    
    s32Ret = BSP_IPF_ConfigUlChannel(BSP_NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlChannel :%d\n",__LINE__);
        goto error;
    }

    stCtrl.bDataChain = BSP_TRUE;
    stCtrl.bEndian = BSP_TRUE;
    stCtrl.eIpfMode = IPF_MODE_FILTERONLY;
    stCtrl.u32WrrValue = 1000;
    s32Ret = BSP_IPF_ConfigUlChannel(&stCtrl);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlChannel :%d\n",__LINE__);
        goto error;
    }
	
    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH0_CTRL_OFFSET) != 0x3E8000D)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlChannel :%d\n",__LINE__);
        goto error;
    }

    stCtrl.bDataChain = BSP_FALSE;
    stCtrl.bEndian = BSP_FALSE;
    stCtrl.eIpfMode = IPF_MODE_FILTERANDTRANS;
    stCtrl.u32WrrValue = 100;
    s32Ret = BSP_IPF_ConfigUlChannel(&stCtrl);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlChannel :%d\n",__LINE__);
        goto error;
    }

	
    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH0_CTRL_OFFSET) != 0x640000)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlChannel :%d\n",__LINE__);
        goto error;
    }

    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET); 
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);    
    return OK;
error:
    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET); 
    return ERROR;
}

/* test BSP_IPF_ConfigDlChannel */
BSP_S32 BSP_IPF_ST_203(BSP_VOID)
{
    BSP_S32 s32Ret;
    BSP_U32 u32OldPara = 0;
    IPF_CHL_CTRL_S stCtrl;

    u32OldPara = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET); 
    
    s32Ret = BSP_IPF_ConfigDlChannel(BSP_NULL);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlChannel :%d\n",__LINE__);
        goto error;
    }

    stCtrl.bDataChain = BSP_TRUE;
    stCtrl.bEndian = BSP_TRUE;
    stCtrl.eIpfMode = IPF_MODE_FILTERONLY;
    stCtrl.u32WrrValue = 1000;
    s32Ret = BSP_IPF_ConfigDlChannel(&stCtrl);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlChannel :%d\n",__LINE__);
        goto error;
    }


    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH1_CTRL_OFFSET) != 0x3E8000D)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlChannel :%d\n",__LINE__);
        goto error;
    }

    stCtrl.bDataChain = BSP_FALSE;
    stCtrl.bEndian = BSP_FALSE;
    stCtrl.eIpfMode = IPF_MODE_FILTERANDTRANS;
    stCtrl.u32WrrValue = 100;
    s32Ret = BSP_IPF_ConfigDlChannel(&stCtrl);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlChannel :%d\n",__LINE__);
        goto error;
    }
	
	
    if(*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH1_CTRL_OFFSET) != 0x640000)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDlChannel :%d\n",__LINE__);
        goto error;
    }

    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET); 
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);    
    return OK;
error:
    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET); 
    return ERROR;
}

/* test BSP_IPF_ChannelEnable */
BSP_S32 BSP_IPF_ST_204(BSP_VOID)
{
    IPF_CHANNEL_TYPE_E eChanType = IPF_CHANNEL_MAX;
    BSP_BOOL bFlag = BSP_FALSE;
    BSP_U32 u32OldPara = 0;
    BSP_S32 s32Ret;

    u32OldPara = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET); 
    
    s32Ret = BSP_IPF_ChannelEnable(eChanType, bFlag);
    if(s32Ret != BSP_ERR_IPF_INVALID_PARA)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }

    /* test  IPF_CHANNEL_UP */
    eChanType = IPF_CHANNEL_UP;
    bFlag = BSP_TRUE;
    s32Ret = BSP_IPF_ChannelEnable(eChanType, bFlag);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }
    if((*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH_EN_OFFSET) & 0x1) != 0x1)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }
    bFlag = BSP_FALSE;
    s32Ret = BSP_IPF_ChannelEnable(eChanType, bFlag);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }
    if((*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH_EN_OFFSET) & 0x1) != 0x0)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }

    /* test  IPF_CHANNEL_DOWN */
    eChanType = IPF_CHANNEL_DOWN;
    bFlag = BSP_TRUE;
    s32Ret = BSP_IPF_ChannelEnable(eChanType, bFlag);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }
    if((*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH_EN_OFFSET) & 0x2) != 0x2)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }
    bFlag = BSP_FALSE;
    s32Ret = BSP_IPF_ChannelEnable(eChanType, bFlag);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }
    if((*(BSP_U32*)(HI_IPF_REGBASE_ADDR_VIRT +HI_IPF_CH_EN_OFFSET) & 0x2) != 0x0)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ChannelEnable :%d\n",__LINE__);
        goto error;
    }

    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET); 
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);    
    return OK;
 error:  
    ipf_writel(u32OldPara, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET); 
    return ERROR;    
}

BSP_S32 BSP_IPF_ST_ULAD(BSP_VOID)
{    
	BSP_U32* u32AD0Num = NULL;
	BSP_U32* u32AD1Num = NULL;
	BSP_S32 s32Ret = 0;
	s32Ret = BSP_IPF_GetUlAdNum(u32AD0Num, u32AD1Num);
	if(IPF_SUCCESS != s32Ret)
	{
  		  IPF_PRINT("BSP_IPF_GetUlAdNum faild \n");
	}
  		  IPF_PRINT("BSP_IPF_GetUlAdNum AD0 = %d,AD1 = %d \n",u32AD0Num, u32AD1Num);
	return s32Ret;
}


/*接收准备*/
BSP_S32 BSP_IPF_ST_100_INIT(BSP_VOID)
{
    BSP_U32 u32Timeout = 100;
    BSP_S32 s32Ret = 0;
    BSP_U32 u32AD0Num = 0;
    BSP_U32 u32AD1Num = 0;
    BSP_U16 u16Len = 1500;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;

    g_pu8STUlData = (BSP_U8*)malloc(u16Len);
    if(g_pu8STUlData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
		
    memset(g_pu8STUlData, 0x0, u16Len);
    memcpy(g_pu8STUlData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(g_pu8STUlData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    IPF_PRINT("sizeof(StreamIPv4Header) = %d  sizeof(StreamUDP) = %d \n", 
                      sizeof(StreamIPv4Header), sizeof(StreamUDP));


    s32Ret = IPF_ST_UL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }
 	BSP_IPF_GetUlAdNum(&u32AD0Num, &u32AD1Num);
	if(IPF_SUCCESS != s32Ret)
	{
  		  IPF_PRINT("BSP_IPF_GetUlAdNum faild \n");
	}
  		  IPF_PRINT("BSP_IPF_GetUlAdNum AD0 = %d,AD1 = %d \n",u32AD0Num, u32AD1Num);
    s32Ret = BSP_IPF_ConfigTimeout(u32Timeout);
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUlIntCb101);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }
	
    s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
        return ERROR;
    }
	
    IPF_PRINT(" A核可以输入测试用例\n");

    return OK;
}

BSP_S32 BSP_IPF_ST_101_INIT(BSP_VOID)
{
    return BSP_IPF_ST_100_INIT();
}

BSP_S32 BSP_IPF_ST_102_INIT(BSP_VOID)
{
    return BSP_IPF_ST_100_INIT();
}

BSP_S32 BSP_IPF_ST_103_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
   
    s32Ret = IPF_ST_UL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUlIntCb101);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }
        s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_ConfigTimeout(0xFFFF);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }

    IPF_PRINT(" A核可以输入测试用例\n");


    s32Ret = BSP_IPF_ConfigTimeout(500);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }
    
    return OK;
}

BSP_S32 BSP_IPF_ST_111_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
   
    s32Ret = IPF_ST_UL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUlIntCb111);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }    
    s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
        return ERROR;
    }

    IPF_PRINT(" A核可以输入测试用例\n");
    return OK;
}

BSP_S32 BSP_IPF_ST_112_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
	
    s32Ret = IPF_ST_UL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUlIntCb111);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }    
    s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
        return ERROR;
    }

    IPF_PRINT(" A核可以输入测试用例\n");
    return OK;
}

BSP_S32 BSP_IPF_ST_113_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
   
    s32Ret = IPF_ST_UL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUlIntCb111);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }    
    s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_ConfigTimeout(0xFFFF);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }
    IPF_PRINT(" A核可以输入测试用例\n");
    return OK;
}




#ifdef CONFIG_MODULE_BUSSTRESS
 /*ipf ul stress test for auto test*/
BSP_U32 ipf_ul_stress_test_cnt = 0;
BSP_S32 ipf_ul_stress_test_task_id = 0;
BSP_S32 ipf_ul_stress_test_routine()
{
	while(get_test_switch_stat(IPF_UL))
	{
	    semTake(g_IpfUlTaskSemId, WAIT_FOREVER);
	    ipf_ul_stress_test_cnt++;
	}
	return OK;
}

BSP_S32 ipf_ul_stress_test_init(BSP_S32 task_priority)
{
	BSP_S32 s32Ret = 0;

	set_test_switch_stat(IPF_UL,TEST_RUN);
	
	s32Ret = IPF_ST_UL_INIT();
	if(s32Ret != OK)
	{
	    IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
	    return ERROR;
	}
	
	s32Ret = IPF_ST_UL_INIT_STRESS();
	if(s32Ret != OK)
	{
	    IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
	    return ERROR;
	}
	
	s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUlIntCb);
	if(s32Ret != IPF_SUCCESS)
	{
	    IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
	    return ERROR;
	}
	s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
	if(s32Ret != IPF_SUCCESS)
	{
	    IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
	    return ERROR;
	}

	ipf_ul_stress_test_cnt = 0;
	
	ipf_ul_stress_test_task_id = taskSpawn ("ipfUlStressTask", task_priority, 0, 20000, (FUNCPTR)ipf_ul_stress_test_routine, 0,
	                                    0, 0, 0,0, 0, 0, 0, 0, 0);
	if(ipf_ul_stress_test_task_id == BSP_ERROR)
	{
		printf("taskSpawn ipfUlTask fail.\n");
		return ERROR;
	}

	IPF_PRINT(" A核可以输入ipf_ul_stress_test_start\n");
	
	return OK;
}

BSP_S32 ipf_ul_stress_test_start(BSP_S32 task_priority)
{
	return ipf_ul_stress_test_init(task_priority);
}

BSP_VOID ipf_ul_stress_test_stop()
{
    set_test_switch_stat(IPF_UL,TEST_STOP);
    while(BSP_OK == taskIdVerify(ipf_ul_stress_test_task_id))
    {
        taskDelay(100);
    }   
    semDelete(g_IpfUlIntSemId);
    semDelete(g_IpfUlTaskSemId);
    taskDelete(g_IpfUlTaskId);
}
#endif

BSP_S32 IpfUltestIntCb(void)
{
    BSP_U32 u32Num = IPF_ULRD_DESC_SIZE;
    IPF_RD_DESC_S stRd[IPF_ULRD_DESC_SIZE];
    BSP_U32 u32RecivedData = 0;
    BSP_U32 u32AD0Num = 0;
    BSP_U32 u32AD1Num = 0;
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;

    /*处理RD*/
    BSP_IPF_GetUlRd(&u32Num, stRd);
    for(i = 0; i < u32Num; i++)
    {
        s32Ret = CmpDMAData((BSP_U32 *)g_pu8STUlData, (BSP_U32 *)stRd[i].u32OutPtr, stRd[i].u16PktLen);
        /*数据比对*/
        if(!s32Ret)
        {
            g_u32UlError++;
        }
        if(stRd[i].u16Result != 0x3f)
        {
            g_u32UlFltError++;
        }
	u32RecivedData += stRd[i].u16PktLen;
    }

    /*统计结果*/
    g_u32UlTotalDataNum += u32RecivedData/1024;
    g_u32UlTotalBDNum += u32Num;
    s32Ret = BSP_IPF_GetUlAdNum(&u32AD0Num,&u32AD1Num);
    if(IPF_SUCCESS != s32Ret)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetUlAdNum :%d\n",__LINE__);
        return ERROR;
    }
    if(u32AD0Num > 10)
    {
        s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_0,u32AD0Num,&ADulshort[0]);
        if(IPF_SUCCESS != s32Ret)
        {
            IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
        }
        for(i=0;i < u32AD0Num;i++)
        {
            memset((BSP_VOID *)ADulshort[i].u32OutPtr1, 0, 404);
        }
    }
    if(u32AD1Num > 10)
    {
        s32Ret = BSP_IPF_ConfigUlAd(IPF_AD_1,u32AD1Num,&ADullong[0]);
        if(IPF_SUCCESS != s32Ret)
        {
            IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
        }
        for(i=0;i < u32AD1Num;i++)
        {
            memset((BSP_VOID *)ADullong[i].u32OutPtr1, 0, 1500);
        }

    }
    return OK;
}

BSP_S32 IpfUltestIntCb122(void)
{
    BSP_U32 u32Num = IPF_ULRD_DESC_SIZE;
    IPF_RD_DESC_S stRd[IPF_ULRD_DESC_SIZE];
    BSP_U32 u32RecivedData = 0;
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;

    /*处理RD*/
    BSP_IPF_GetUlRd(&u32Num, stRd);
    for(i = 0; i < u32Num; i++)
    {
        s32Ret = CmpDMAData((BSP_U32 *)g_pu8STUlData, (BSP_U32 *)stRd[i].u32OutPtr, stRd[i].u16PktLen);
        /*数据比对*/
        if(!s32Ret)
        {
            g_u32UlError++;
        }
        if(stRd[i].u16Result != 0x3f)
        {
            g_u32UlFltError++;
        }
	u32RecivedData += stRd[i].u16PktLen;
    }

    /*统计结果*/
    g_u32UlTotalDataNum += u32RecivedData/1024;
    g_u32UlTotalBDNum += u32Num;
    return OK;
}


BSP_VOID BSP_IPF_ST_121_Result(BSP_VOID)
{
		IPF_PRINT(" g_u32UlError(BDs) :%d\n",g_u32UlError);
		IPF_PRINT(" g_u32UlFltError(BDs) :%d\n",g_u32UlFltError);
		IPF_PRINT(" g_u32UlTotalDataNum(KB) :%d\n",g_u32UlTotalDataNum);
		IPF_PRINT(" g_u32UlTotalBDNum(BDs) :%d\n",g_u32UlTotalBDNum);
}

BSP_VOID BSP_IPF_ST_122_Result(BSP_VOID)
{
	BSP_IPF_ST_121_Result();
}

/*上行压力测试C核准备函数*/
BSP_S32 BSP_IPF_ST_121_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U16 u16Len = 1500;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
	
    g_pu8STUlData = (BSP_U8*)malloc(u16Len);
    if(g_pu8STUlData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset(g_pu8STUlData, 0x0, u16Len);
    memcpy(g_pu8STUlData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(g_pu8STUlData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    s32Ret = IPF_ST_UL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUltestIntCb);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }    
    s32Ret = BSP_IPF_RegisterAdqEmptyUlCb(IPF_ST_UL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyUlCb :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_ConfigTimeout(0x65);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }
    IPF_PRINT(" A核可以输入压力测试用例\n");
    return OK;
}


/*上行压力测试C核准备函数*/
BSP_S32 BSP_IPF_ST_122_INIT(BSP_VOID)
{

    BSP_S32 s32Ret = 0;
	
    s32Ret = BSP_IPF_RegisterWakeupUlCb(IpfUltestIntCb122);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupUlCb :%d\n",__LINE__);
        return ERROR;
    }
    s32Ret = BSP_IPF_ST_121_INIT();
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ST_121_INIT :%d\n",__LINE__);
        return ERROR;
    }    

    return IPF_SUCCESS;
}

BSP_S32 CmpFilterInfo(IPF_FILTER_CONFIG_S* p1, IPF_FILTER_CONFIG_S* p2)
{
    BSP_U32 i = 0;
    IPF_MATCH_INFO_S * p1t = (IPF_MATCH_INFO_S *)(&(p1->stMatchInfo));
    IPF_MATCH_INFO_S * p2t = (IPF_MATCH_INFO_S *)(&(p2->stMatchInfo));
    BSP_U32 *u32p1 = (BSP_U32 *)p1t;
    BSP_U32 *u32p2 = (BSP_U32 *)p2t;
    BSP_U32 u32Len = 0; 
    u32Len = sizeof(IPF_MATCH_INFO_S);
    u32Len = u32Len/4;

    /*过滤器配置信息里不包括下一个过滤器域，
    该域由底软动态分配，故比较时屏蔽该域*/
    (p2t->unFltChain).Bits.u16NextIndex = (p1t->unFltChain).Bits.u16NextIndex;
	
    for(i=0; i<u32Len; i++)
    {
        if(*(u32p1 + i) != *(u32p2 + i))
        {
            return ERROR;
        }
    }
    return 0;
}
BSP_S32 CmpMatchInfo(IPF_MATCH_INFO_S * p1t,IPF_MATCH_INFO_S * p2t)
{
    BSP_U32 i = 0;
    BSP_U32 *u32p1 = (BSP_U32 *)p1t;
    BSP_U32 *u32p2 = (BSP_U32 *)p2t;
    BSP_U32 u32Len = 0; 
    u32Len = sizeof(IPF_MATCH_INFO_S);
    u32Len = u32Len/4;
	
    for(i=0; i<u32Len; i++)
    {
        if(*(u32p1 + i) != *(u32p2 + i))
        {
            return ERROR;
        }
    }
    return 0;
}

BSP_VOID PrintFilterInfo(IPF_FILTER_CONFIG_S* pu32FilterInfo)
{
    BSP_U32 i = 0;
    BSP_U32* p;
	
    BSP_U32 u32Len = sizeof(IPF_MATCH_INFO_S);
    p = (BSP_U32 *)(&(pu32FilterInfo->stMatchInfo));
    u32Len = u32Len/4;
	
    IPF_PRINT("0x%08x\n",pu32FilterInfo->u32FilterID);
    for(i=0; i < u32Len; i++)
    {
        IPF_PRINT("0x%08x    ", *(p+i));
        if((i%4) == 0)
        {
            IPF_PRINT("\n");
        }
    }
    IPF_PRINT("\n");
}

BSP_VOID PrintMatchInfo(IPF_MATCH_INFO_S * pu32MatchInfo)
{
    BSP_U32 i = 0;
    BSP_U32* p;
	
    BSP_U32 u32Len = sizeof(IPF_MATCH_INFO_S);
    p = (BSP_U32 *)(pu32MatchInfo);
    u32Len = u32Len/4;
	
    for(i=0; i < u32Len; i++)
    {
        IPF_PRINT("0x%08x    ", *(p+i));
        if((i%4) == 0)
        {
            IPF_PRINT("\n");
        }
    }
    IPF_PRINT("\n");
}
s32 check_filter_backup_status(void)
{
    s32 ret;
    u32 i,j,k,filter_serial;
    IPF_MATCH_INFO_S* hard_filter_info;
    u32 *hard_filter_info_addr;
    IPF_MATCH_INFO_S* backup_filter_info;
    u32* backup_info_addr = NULL;
    hard_filter_info = (IPF_MATCH_INFO_S*)malloc(sizeof(IPF_MATCH_INFO_S));
    if(NULL == hard_filter_info)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r pstMatchInfo malloc ERROR! \n");
        return IPF_ERROR;
    }
    hard_filter_info_addr = (u32*)hard_filter_info;

    for(i=0 ; i < IPF_MODEM_MAX; i++)
    {
        backup_info_addr = (u32 *)(IPF_PWRCTL_BASIC_FILTER_ADDR + i*sizeof(IPF_MATCH_INFO_S));
        backup_filter_info = (IPF_MATCH_INFO_S*)backup_info_addr;
        filter_serial = i;
        k = 0;
        do
        {
            if(filter_serial < IPF_BF_NUM)
            {
                ipf_writel(filter_serial, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 
                for(j=0; j<(sizeof(IPF_MATCH_INFO_S)/4); j++)
                {
                    *hard_filter_info_addr = ipf_readl((HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET+j*4)); 
                    hard_filter_info_addr++;
                }
                k++;
                ret = CmpMatchInfo(backup_filter_info, hard_filter_info);
                if(ret != 0)
                {
                    IPF_PRINT("chainNum = %d \n",i);
                    IPF_PRINT("filterNum = %d \n",filter_serial);
                    IPF_PRINT("========BACKUPED_FILTER_INFO=======\n");
                    PrintMatchInfo(backup_filter_info);
                    IPF_PRINT("========FILTER_ACTUAL_INFO=======\n");
                    PrintMatchInfo(hard_filter_info);
                }
            hard_filter_info_addr = (u32*)hard_filter_info;
            }
            filter_serial = backup_filter_info->unFltChain.Bits.u16NextIndex;
            backup_filter_info = (IPF_MATCH_INFO_S*)(IPF_PWRCTL_BASIC_FILTER_ADDR + filter_serial*sizeof(IPF_MATCH_INFO_S));
            backup_info_addr = (u32 *)backup_filter_info;
            if(k >= IPF_BF_NUM)
            {
                IPF_PRINT("Filter end error!\n");
                return IPF_ERROR;
            }
        }while(filter_serial != IPF_TAIL_INDEX);
    }
	return IPF_SUCCESS;
}

s32 check_filter_zero_status(IPF_FILTER_CHAIN_TYPE_E filter_serial)
{
    ipf_writel(filter_serial, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 

    if((IPF_TAIL_INDEX == ipf_readl( HI_IPF_REGBASE_ADDR_VIRT + HI_FLT_CHAIN_OFFSET))
		&(0 == ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_FLT_RULE_CTRL_OFFSET)))
    {
       IPF_PRINT(" empty list next index error\n");
	return IPF_ERROR;
    }
    return IPF_SUCCESS;
}
/* test  测试一个上行过滤器设置和查询 */
BSP_S32 BSP_IPF_ST_205(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo = {0};
    IPF_FILTER_CONFIG_S stUlFilterInfo1 = {0};
    BSP_S32 s32Ret= 0;
    
    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    memset(&stUlFilterInfo, 0x0, sizeof(IPF_FILTER_CONFIG_S));
    stUlFilterInfo.u32FilterID = 50;/*Basic filter,psid*/  
    stUlFilterInfo.stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0x13361;
    *(BSP_U32*)(stUlFilterInfo.stMatchInfo.u8SrcAddr) = 0x020113C6;/*00xC6130102;*/  
    *(BSP_U32*)(stUlFilterInfo.stMatchInfo.u8DstAddr) = 0x010113C6;/*00xC6130101;*/  
    *(BSP_U32*)(stUlFilterInfo.stMatchInfo.u8DstMsk) = 0xFFFFFFFF;    
    stUlFilterInfo.stMatchInfo.unSrcPort.Bits.u16SrcPortLo = 10;
    stUlFilterInfo.stMatchInfo.unSrcPort.Bits.u16SrcPortHi = 30;
    stUlFilterInfo.stMatchInfo.unDstPort.Bits.u16DstPortLo = 10;
    stUlFilterInfo.stMatchInfo.unDstPort.Bits.u16DstPortHi = 30;
    stUlFilterInfo.stMatchInfo.unTrafficClass.Bits.u8TrafficClass = 3;
    stUlFilterInfo.stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
    stUlFilterInfo.stMatchInfo.u32LocalAddressMsk = 0;
    stUlFilterInfo.stMatchInfo.unNextHeader.u32Protocol = 1;
    stUlFilterInfo.stMatchInfo.unFltCodeType.Bits.u16Type = 2;
    stUlFilterInfo.stMatchInfo.unFltCodeType.Bits.u16Code = 3;
    stUlFilterInfo.stMatchInfo.unFltChain.Bits.u16FltPri = 100;
    stUlFilterInfo.stMatchInfo.unFltChain.Bits.u16NextIndex = 200;
    stUlFilterInfo.stMatchInfo.u32FltSpi = 4;

    /* 第一次配置上行，PS ID是50 */
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, &stUlFilterInfo, 1);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }       
    if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, 50, &stUlFilterInfo1) != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
    }
    stUlFilterInfo.stMatchInfo.unFltChain.Bits.u16NextIndex = 511;
    
    s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo1));
    if(s32Ret != 0)
    {
        IPF_PRINT(" TESTERROR    CmpFilterInfo :%d\n",__LINE__);

        IPF_PRINT("========in=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)(&stUlFilterInfo));
        IPF_PRINT("========out=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)(&stUlFilterInfo1));
        return ERROR;   
    }

    /* 第二次配置上行，PS ID是30 */
    stUlFilterInfo.u32FilterID = 30;
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, &stUlFilterInfo, 1);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }       
    if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, 30, &stUlFilterInfo1) != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);

        IPF_PRINT("========in=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo));
        IPF_PRINT("========out=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo1));

        return ERROR;   
    }
    stUlFilterInfo.stMatchInfo.unFltChain.Bits.u16NextIndex = 511;
    
    s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo1));
    if(s32Ret != 0)
    {
        IPF_PRINT(" TESTERROR    CmpFilterInfo :%d\n",__LINE__);

        IPF_PRINT("========in=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfo);
        IPF_PRINT("========out=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfo1);
        return ERROR;   
    }

    /* 查询没有配置的Filter返回错误 */
    if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, 90, (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo1)) != IPF_ERROR)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
        return ERROR;
    }
    check_filter_backup_status();
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);    
    if(BSP_IPF_SetFilter(IPF_MODEM0_ULFC, (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), 0) != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
        return ERROR;   
    }
    check_filter_backup_status();
    check_filter_zero_status(IPF_MODEM0_ULFC);
    return OK; 
}

/* test  测试2个过滤器设置和查询(Ps ID 一致 ) */
BSP_S32 BSP_IPF_ST_206(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo[2];
    IPF_FILTER_CONFIG_S stUlFilterInfoCheck= {0};
    IPF_FILTER_CONFIG_S stDlFilterInfo[2];
    IPF_FILTER_CONFIG_S stDlFilterInfoCheck= {0};
    BSP_S32 s32Ret = 0;
    BSP_U32 i;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }
    for(i=0;i<2;i++)
		{
		    memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
		    stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0x13361;
		    *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = 0x020113C6;/*00xC6130102;*/  
		    *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = 0x010113C6;/*00xC6130101;*/  
		    *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
		    stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = 10;
		    stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = 30;
		    stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = 10;
		    stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = 30;
		    stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = 3;
		    stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
		    stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
		    stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = 1;
		    stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = 2;
		    stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = 3;
		    stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = 100;
		    stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = 200;
		    stUlFilterInfo[i].stMatchInfo.u32FltSpi = 10;
		    memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
		    stUlFilterInfo[i].u32FilterID = 50+i;
		    

		    memcpy(&stDlFilterInfo, &stUlFilterInfo, 2*sizeof(IPF_FILTER_CONFIG_S));
		    stDlFilterInfo[i].stMatchInfo.u32FltSpi = 20;
    	}
	    stDlFilterInfo[1].stMatchInfo.unFltChain.Bits.u16NextIndex = 511;

    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_DLFC, (IPF_FILTER_CONFIG_S *)(&stDlFilterInfo), 2);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }  
        check_filter_backup_status();

    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), 2);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }
        check_filter_backup_status();

    if(BSP_IPF_GetFilter(IPF_MODEM0_DLFC, 50, (IPF_FILTER_CONFIG_S *)(&stDlFilterInfoCheck)) != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
        return ERROR; 
    }
       
    s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stDlFilterInfo), (IPF_FILTER_CONFIG_S *)(&stDlFilterInfoCheck));
    if(s32Ret != 0)
    {
        IPF_PRINT(" TESTERROR    CmpFilterInfo :%d\n",__LINE__);
        IPF_PRINT("filterNum = %d \n",i);
        IPF_PRINT("========in=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfo[i]);
        IPF_PRINT("========out=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfoCheck);

        return ERROR; 
    }

    if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, 50, (IPF_FILTER_CONFIG_S *)(&stUlFilterInfoCheck)) != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
        return ERROR; 
    }
    stUlFilterInfo[1].stMatchInfo.unFltChain.Bits.u16NextIndex = 511;
    
    s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), (IPF_FILTER_CONFIG_S *)(&stUlFilterInfoCheck));
    if(s32Ret != 0)
    {
        IPF_PRINT(" TESTERROR    CmpFilterInfo :%d\n",__LINE__);
        IPF_PRINT("filterNum = %d \n",i);
        IPF_PRINT("========in=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo[i]));
        IPF_PRINT("========out=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfoCheck));

        return ERROR; 
    }
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), 0);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    } 
    check_filter_backup_status();

    check_filter_zero_status(IPF_MODEM0_ULFC);
	
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_DLFC, (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), 0);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    } 
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);        
    check_filter_backup_status();

    check_filter_zero_status(IPF_MODEM0_DLFC);

    return OK; 
}


/* test  测试配置150个上行过滤器并查询比较 150个，调用过滤器类型标志，
用到了扩展过滤器 */
BSP_S32 BSP_IPF_ST_207(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo[150];
    IPF_FILTER_CONFIG_S stUlFilterInfoCheck;
    BSP_U32 u32BasicFilterNum = 50;
    BSP_U32 u32FilterNum = 150;
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    for(i = 0; i < u32BasicFilterNum; i++)
    {
        memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        stUlFilterInfo[i].u32FilterID = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
        stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i;
        stUlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }
	
    for(i = u32BasicFilterNum; i < u32FilterNum; i++)
    {
        memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        stUlFilterInfo[i].u32FilterID = i+2+0x10000;
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
        stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i;
        stUlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }
	
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, stUlFilterInfo, u32FilterNum);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }
    check_filter_backup_status();

    for(i = u32BasicFilterNum; i < u32FilterNum; i++)
    {
        memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        stUlFilterInfo[i].u32FilterID = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
        stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i;
        stUlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }
    for(i = 0; i < u32FilterNum; i++)
    {
        if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, i+2, &stUlFilterInfoCheck) != IPF_SUCCESS)
        {
            IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
            return ERROR; 
        }
        if(i<u32FilterNum-1)
        {
            stUlFilterInfoCheck.stMatchInfo.unFltChain.Bits.u16NextIndex = i+1;
        }
        else
        {
            stUlFilterInfoCheck.stMatchInfo.unFltChain.Bits.u16NextIndex = 511;
        }
        
        s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfoCheck), (IPF_FILTER_CONFIG_S *)(&stUlFilterInfo[i]));
        if(s32Ret != 0)
        {
            IPF_PRINT("filterNum = %d \n",i);
            IPF_PRINT("========in=======\n");
            PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfo[i]);
            IPF_PRINT("========out=======\n");
            PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfoCheck);
        }
    }
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, stUlFilterInfo, 0);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    } 
    check_filter_backup_status();

    check_filter_zero_status(IPF_MODEM0_ULFC);
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);       
    return OK; 
}

/* test  测试配置150个下行过滤器全部设置为基本测试基本转扩展是否有效，并查询比较 */
BSP_S32 BSP_IPF_ST_208(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stDlFilterInfo[150];
    IPF_FILTER_CONFIG_S stDlFilterInfoCheck;
    BSP_U32 u32FilterNum = 150;
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;
    
    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    for(i = 0; i < u32FilterNum; i++)
    {
        memset(&stDlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        stDlFilterInfo[i].u32FilterID = i+2;
        stDlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stDlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stDlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stDlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stDlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stDlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stDlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stDlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
		
        stDlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stDlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stDlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;

		
        stDlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stDlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stDlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stDlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stDlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i+200;
        stDlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }

		  
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_DLFC, stDlFilterInfo, u32FilterNum);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }
    check_filter_backup_status();

    for(i = 0; i < u32FilterNum; i++)
    {
        if(BSP_IPF_GetFilter(IPF_MODEM0_DLFC, i+2, &stDlFilterInfoCheck) != IPF_SUCCESS)
        {
            IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
            return ERROR; 
        }
        if(i<u32FilterNum-1)
        {
            stDlFilterInfoCheck.stMatchInfo.unFltChain.Bits.u16NextIndex = i+1;
        }
        else
        {
            stDlFilterInfoCheck.stMatchInfo.unFltChain.Bits.u16NextIndex = 511;
        }

        s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stDlFilterInfoCheck), (IPF_FILTER_CONFIG_S *)(&stDlFilterInfo[i]));
        if(s32Ret != 0)
        {
            IPF_PRINT("filterNum = %d \n",i);
            IPF_PRINT("========in=======\n");
            PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stDlFilterInfo[i]);
            IPF_PRINT("========out=======\n");
            PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stDlFilterInfoCheck);
            return ERROR; 
        }
    }

    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_DLFC, stDlFilterInfo, 0);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    } 
    check_filter_backup_status();

    check_filter_zero_status(IPF_MODEM0_DLFC);
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);           
    return OK; 
}

/* test  测试过滤器设置和查询  */
BSP_S32 BSP_IPF_ST_209(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo = {0};
    IPF_FILTER_CONFIG_S stUlFilterInfoCheck= {0};
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    memset(&stUlFilterInfo, 0x0, sizeof(IPF_FILTER_CONFIG_S));
    stUlFilterInfo.u32FilterID = 3;
    stUlFilterInfo.stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = 0x13361;
    *(BSP_U32*)(stUlFilterInfo.stMatchInfo.u8SrcAddr) = 0x020113C6;/*00xC6130102;*/
    *(BSP_U32*)(stUlFilterInfo.stMatchInfo.u8DstAddr) = 0x010113C6;/*00xC6130101;*/
    *(BSP_U32*)(stUlFilterInfo.stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
    stUlFilterInfo.stMatchInfo.unSrcPort.Bits.u16SrcPortLo = 10;
    stUlFilterInfo.stMatchInfo.unSrcPort.Bits.u16SrcPortHi = 30;
    stUlFilterInfo.stMatchInfo.unDstPort.Bits.u16DstPortLo = 10;
    stUlFilterInfo.stMatchInfo.unDstPort.Bits.u16DstPortHi = 30;
	
    stUlFilterInfo.stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
    stUlFilterInfo.stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
    stUlFilterInfo.stMatchInfo.u32LocalAddressMsk = 0;
	
    stUlFilterInfo.stMatchInfo.unNextHeader.u32Protocol = 1;
    stUlFilterInfo.stMatchInfo.unFltCodeType.Bits.u16Type = 2;
    stUlFilterInfo.stMatchInfo.unFltCodeType.Bits.u16Code = 3;
    stUlFilterInfo.stMatchInfo.u32FltSpi = 4;

    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, &stUlFilterInfo, 1);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);    
        return ERROR;   
    }
    check_filter_backup_status();
    if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, 3, &stUlFilterInfoCheck) != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
        return ERROR; 
    }
    stUlFilterInfo.stMatchInfo.unFltChain.Bits.u16NextIndex = 511;    
   
    s32Ret = CmpFilterInfo((IPF_FILTER_CONFIG_S *)(&stUlFilterInfo), (IPF_FILTER_CONFIG_S *)(&stUlFilterInfoCheck));
    if(s32Ret != 0)
    {
        IPF_PRINT("========in=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfo);
        IPF_PRINT("========out=======\n");
        PrintFilterInfo((IPF_FILTER_CONFIG_S*)&stUlFilterInfoCheck);
        return ERROR; 
    }    
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, &stUlFilterInfo, 0);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    } 
    check_filter_backup_status();
    if(BSP_IPF_GetFilter(IPF_MODEM0_ULFC, 3, &stUlFilterInfoCheck) != IPF_ERROR)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_GetFilter :%d\n",__LINE__);
        return ERROR; 
    }
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, &stUlFilterInfo, 0);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    } 
    check_filter_backup_status();

    check_filter_zero_status(IPF_MODEM0_ULFC);
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);          
    return OK; 

}

/*配置基本扩展过滤器交替的场景*/
BSP_S32 BSP_IPF_ST_210(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo[150];
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    for(i = 0; i < 150; i++)
    {
        memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        if(0 == i)
        {
            stUlFilterInfo[i].u32FilterID = i+2;
        }
        else if(0 == i%2)
        {
            stUlFilterInfo[i].u32FilterID = i+2;
        }
        else
        {
            stUlFilterInfo[i].u32FilterID = i+2+0x10000;
        }
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
        stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i;
        stUlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }
	
	
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, stUlFilterInfo, 150);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }
    check_filter_backup_status();
    
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);       
    return OK; 
}


/*配置满全基本过滤器场景*/
BSP_S32 BSP_IPF_ST_211(BSP_VOID)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo[57];
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    for(i = 0; i < 57; i++)
    {
        memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        stUlFilterInfo[i].u32FilterID = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
        stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i;
        stUlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }
	
	
    s32Ret = BSP_IPF_SetFilter(IPF_MODEM0_ULFC, stUlFilterInfo, 57);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }
    check_filter_backup_status();
    
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);       
    return OK; 
}



/*配置基本加扩展场景*/
BSP_S32 BSP_IPF_ST_212(u32 basic_num, u32 ext_num, u32 chain_head)
{
    IPF_FILTER_CONFIG_S stUlFilterInfo[511];
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;

    if(g_u32IpfStAllInit != BSP_TRUE)
    {
        s32Ret = IPF_ST_INIT();
        if(s32Ret != OK)
        {
            IPF_PRINT(" TESTERROR    IPF_ST_INIT :%d\n",__LINE__);
            return ERROR;
        }
    }

    for(i = 0; i < (basic_num+ext_num); i++)
    {
        memset(&stUlFilterInfo[i], 0x0, sizeof(IPF_FILTER_CONFIG_S));
        if(i < basic_num)
        {
            stUlFilterInfo[i].u32FilterID = i+2;
        }
	else
	{
            stUlFilterInfo[i].u32FilterID = i+2+0x10000;
	}
        stUlFilterInfo[i].stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl = (i+1000)&0xFFFFFFF3;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8SrcAddr) = i+2000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstAddr) = i+4000;
        *(BSP_U32*)(stUlFilterInfo[i].stMatchInfo.u8DstMsk) = 0xFFFFFFFF;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unSrcPort.Bits.u16SrcPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortLo = i+10;
        stUlFilterInfo[i].stMatchInfo.unDstPort.Bits.u16DstPortHi = i+30;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClass = i+3;
        stUlFilterInfo[i].stMatchInfo.unTrafficClass.Bits.u8TrafficClassMask = 0xFF;
        stUlFilterInfo[i].stMatchInfo.u32LocalAddressMsk = 0;
        stUlFilterInfo[i].stMatchInfo.unNextHeader.u32Protocol = i+1;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Type = i+2;
        stUlFilterInfo[i].stMatchInfo.unFltCodeType.Bits.u16Code = i+3;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16FltPri = i+100;
        stUlFilterInfo[i].stMatchInfo.unFltChain.Bits.u16NextIndex = i;
        stUlFilterInfo[i].stMatchInfo.u32FltSpi = i+4;
    }
	
	
    s32Ret = BSP_IPF_SetFilter(chain_head, stUlFilterInfo, (basic_num+ext_num));
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_SetFilter :%d\n",__LINE__);
        return ERROR;   
    }
    check_filter_backup_status();
    
    IPF_PRINT("%s : SUCCESS\n",__FUNCTION__);       
    return OK; 
}




TTF_MEM_ST* GetDLSrcBufCb(BSP_U32 u32Len)
{
    TTF_MEM_ST* pstTTF;
    pstTTF = (TTF_MEM_ST*)cacheDmaMalloc(sizeof(TTF_MEM_ST) + u32Len);
    if(pstTTF == BSP_NULL)
    printf("GetPsBufCb malloc pstTTF error\n");
    
    pstTTF->pData = (BSP_U8*)pstTTF + sizeof(TTF_MEM_ST);
    pstTTF->usUsed = u32Len;
    if(pstTTF->pData == BSP_NULL)
    printf("GetPsBufCb malloc pstTTF->pData error\n");
    
    return pstTTF;
}

void ipf_set_sche_mode(u32 sche_mode)
{
    u32 control_info = 0;
    control_info = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CTRL_OFFSET);
    control_info &= 0xFFFFDFFF;//set sche ctrl to BD only

    if(0 == sche_mode)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r ipf_set_sche_mode to BD only \n");
    }
    else
    {
        control_info |= 0x00002000;
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r ipf_set_sche_mode to BD AD RD \n");
    }

    /* 配置上下行通道的AD阀值*/
    ipf_writel(control_info, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CTRL_OFFSET);
    return;
}

/* 2个TTF结点，配2个BD 一长一短，用于验证AD回退的有效性*/
BSP_S32 BSP_IPF_ST_300(BSP_VOID)
{
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    IPF_CONFIG_DLPARAM_S stDlPara[2];
    TTF_MEM_ST stSrcTtf;
    TTF_MEM_ST stSrcTtfShort;
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Num = 2;
    BSP_S32 s32Ret = 0;
   
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }
	
    stSrcTtf.usUsed = 1000;
    stSrcTtf.pData = cacheDmaMalloc(stSrcTtf.usUsed);
    stSrcTtf.pNext = BSP_NULL;
    if(stSrcTtf.pData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }
    stSrcTtfShort.usUsed = 200;
    stSrcTtfShort.pData = cacheDmaMalloc(stSrcTtfShort.usUsed);
    stSrcTtfShort.pNext = BSP_NULL;
    if(stSrcTtfShort.pData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = stSrcTtf.usUsed;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset(stSrcTtf.pData, 0x0, stSrcTtf.usUsed);
    memcpy(stSrcTtf.pData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(stSrcTtf.pData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    
    stDlPara[0].u32Data = (BSP_U32)&stSrcTtf;
    stDlPara[0].u16UsrField1= 1200;
    stDlPara[0].u16Len= 200;

    /*设中断，仅搬移*/
    stDlPara[0].u16Attribute= 7; 

    cacheFlush(DATA_CACHE,stSrcTtf.pData,1000);


    stIPHeader.TOL = stSrcTtfShort.usUsed;

    memset(stSrcTtfShort.pData, 0x0, stSrcTtfShort.usUsed);
    memcpy(stSrcTtfShort.pData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(stSrcTtfShort.pData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    stDlPara[1].u32Data = (BSP_U32)(&stSrcTtfShort);
    stDlPara[1].u16UsrField1= 200;
    stDlPara[1].u16Len= 200;

    /*设中断，仅搬移*/
    stDlPara[1].u16Attribute= 7; 

    cacheFlush(DATA_CACHE,stSrcTtfShort.pData,200);

	
    s32Ret = BSP_IPF_ConfigDownFilter(u32Num, (IPF_CONFIG_DLPARAM_S*)&stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 
    /*
    IPF_PRINT("stSrcTtf.pData = %x \n stDlPara.u32Data = %x \n &stSrcTtf = %x \n",stSrcTtf.pData,stDlPara.u32Data,&stSrcTtf);
    */
    return OK;
    
error1:
    cacheDmaFree((BSP_U32 *)stSrcTtf.pData);
    return ERROR;
}


/* 1个TTF结点，配1个BD */
BSP_S32 BSP_IPF_ST_301(BSP_VOID)
{
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    IPF_CONFIG_DLPARAM_S stDlPara;
    TTF_MEM_ST stSrcTtf;
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Num = 1;
    BSP_S32 s32Ret = 0;
   
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }
	
    stSrcTtf.usUsed = 1000;
    stSrcTtf.pData = cacheDmaMalloc(stSrcTtf.usUsed);
    stSrcTtf.pNext = BSP_NULL;
    if(stSrcTtf.pData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = stSrcTtf.usUsed;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset(stSrcTtf.pData, 0x0, stSrcTtf.usUsed);
    memcpy(stSrcTtf.pData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(stSrcTtf.pData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    
    stDlPara.u32Data = (BSP_U32)(&stSrcTtf);
    stDlPara.u16UsrField1= 200;
    stDlPara.u16Len= 1000;

    /*设中断，仅搬移*/
    stDlPara.u16Attribute= 7; 

    cacheFlush(DATA_CACHE,stSrcTtf.pData,1000);

    s32Ret = BSP_IPF_ConfigDownFilter(u32Num, &stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 
    IPF_PRINT("stSrcTtf.pData = %x \n stDlPara.u32Data = %x \n &stSrcTtf = %x \n",stSrcTtf.pData,stDlPara.u32Data,&stSrcTtf);
	
    return OK;
    
error1:
    cacheDmaFree((BSP_VOID *)(stSrcTtf.pData));
    return ERROR;
}

/* 5个TTF结点，配1个BD */
BSP_S32 BSP_IPF_ST_302(BSP_VOID)
{
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    IPF_CONFIG_DLPARAM_S stDlPara;
    TTF_MEM_ST stSrcTtf[5];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_S32 s32Ret = 0;
    BSP_U32 u32Num = 1;
	
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }

    pu8IPData = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData1 = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
    pu8IPData2 = (BSP_U8*)cacheDmaMalloc(100);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
    pu8IPData3 = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
    pu8IPData4 = (BSP_U8*)cacheDmaMalloc(300);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = 1000;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset(pu8IPData, 0x0, 200);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memset(pu8IPData1, 0x0, 200);
    memset(pu8IPData2, 0x0, 100);
    memset(pu8IPData3, 0x0, 200);
    memset(pu8IPData4, 0x0, 300);
	
    stSrcTtf[0].usUsed = 200;
    stSrcTtf[0].pData = pu8IPData;
    stSrcTtf[0].pNext = &stSrcTtf[1];

    stSrcTtf[1].usUsed = 200;
    stSrcTtf[1].pData = pu8IPData1;
    stSrcTtf[1].pNext = &stSrcTtf[2];

    stSrcTtf[2].usUsed = 100;
    stSrcTtf[2].pData = pu8IPData2;
    stSrcTtf[2].pNext = &stSrcTtf[3];

    stSrcTtf[3].usUsed = 200;
    stSrcTtf[3].pData = pu8IPData3;
    stSrcTtf[3].pNext = &stSrcTtf[4];

    stSrcTtf[4].usUsed = 300;
    stSrcTtf[4].pData = pu8IPData4;
    stSrcTtf[4].pNext = BSP_NULL;
	
    stDlPara.u32Data = (BSP_U32)stSrcTtf;
    stDlPara.u16UsrField1= 200;
    stDlPara.u16Len= 1000;
    /*设中断，仅搬移*/
    stDlPara.u16Attribute= 7; 
	
    s32Ret = BSP_IPF_ConfigDownFilter(u32Num, &stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 
    taskDelay(200);
    cacheDmaFree(pu8IPData4);
    cacheDmaFree(pu8IPData3);
    cacheDmaFree(pu8IPData2);
    cacheDmaFree(pu8IPData1);
    cacheDmaFree(pu8IPData);
	
    return OK;
    
error1:
    cacheDmaFree(pu8IPData3);
error2:
    cacheDmaFree(pu8IPData2);
error3:
    cacheDmaFree(pu8IPData1);
error4:
    cacheDmaFree(pu8IPData);

    return ERROR;

}

/* 5个TTF结点，配62个BD 仅搬移*/
BSP_S32 BSP_IPF_ST_3031(BSP_VOID)
{
    BSP_U32 BDNUM = 60;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    IPF_CONFIG_DLPARAM_S stDlPara[BDNUM];
    TTF_MEM_ST stSrcTtf[5];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;
	
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }
    pu8IPData = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData1 = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
    pu8IPData2 = (BSP_U8*)cacheDmaMalloc(100);
    if(pu8IPData2 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
    pu8IPData3 = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData3 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
    pu8IPData4 = (BSP_U8*)cacheDmaMalloc(300);
    if(pu8IPData4 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = 1000;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;

    memset(pu8IPData, 0x0, 200);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memset(pu8IPData1, 0x0, 200);
    memset(pu8IPData2, 0x0, 100);
    memset(pu8IPData3, 0x0, 200);
    memset(pu8IPData4, 0x0, 300);
	    stSrcTtf[0].usUsed = 200;
	    stSrcTtf[0].pData = pu8IPData;
	    stSrcTtf[0].pNext = &stSrcTtf[1];

	    stSrcTtf[1].usUsed = 200;
	    stSrcTtf[1].pData = pu8IPData1;
	    stSrcTtf[1].pNext = &stSrcTtf[2];

	    stSrcTtf[2].usUsed = 100;
	    stSrcTtf[2].pData = pu8IPData2;
	    stSrcTtf[2].pNext = &stSrcTtf[3];

	    stSrcTtf[3].usUsed = 200;
	    stSrcTtf[3].pData = pu8IPData3;
	    stSrcTtf[3].pNext = &stSrcTtf[4];

	    stSrcTtf[4].usUsed = 300;
	    stSrcTtf[4].pData = pu8IPData4;
	    stSrcTtf[4].pNext = BSP_NULL;
	for(i=0;i < BDNUM;i++)
	{
	
	    stDlPara[i].u32Data = (BSP_U32)stSrcTtf;
	    stDlPara[i].u16UsrField1= 200+i;
	    stDlPara[i].u16Len= 1000;

	    /*设中断，仅搬移*/
	    stDlPara[i].u16Attribute= 6; 
	}	
	    stDlPara[61].u16Attribute= 7; 
    s32Ret = BSP_IPF_ConfigDownFilter(BDNUM-1, stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 
    taskDelay(200);
    cacheDmaFree(pu8IPData4);
    cacheDmaFree(pu8IPData3);
    cacheDmaFree(pu8IPData2);
    cacheDmaFree(pu8IPData1);
    cacheDmaFree(pu8IPData);
    return OK;
    
error1:
    cacheDmaFree(pu8IPData3);
error2:
    cacheDmaFree(pu8IPData2);
error3:
    cacheDmaFree(pu8IPData1);
error4:
    cacheDmaFree(pu8IPData);

    return ERROR;

}

/* 5个TTF结点，配31个BD 过滤搬移*/
BSP_S32 BSP_IPF_ST_3032(BSP_VOID)
{
    BSP_U32 BDNUM =  31;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    IPF_CONFIG_DLPARAM_S stDlPara[BDNUM];
    TTF_MEM_ST stSrcTtf[5];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;
	
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }

    pu8IPData = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData1 = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
    pu8IPData2 = (BSP_U8*)cacheDmaMalloc(100);
    if(pu8IPData2 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
    pu8IPData3 = (BSP_U8*)cacheDmaMalloc(200);
    if(pu8IPData3 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
    pu8IPData4 = (BSP_U8*)cacheDmaMalloc(300);
    if(pu8IPData4 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = 1000;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
	
    memset(pu8IPData, 0x0, 200);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memset(pu8IPData1, 0x0, 200);
    memset(pu8IPData2, 0x0, 100);
    memset(pu8IPData3, 0x0, 200);
    memset(pu8IPData4, 0x0, 300);
	    stSrcTtf[0].usUsed = 200;
	    stSrcTtf[0].pData = pu8IPData;
	    stSrcTtf[0].pNext = &stSrcTtf[1];

	    stSrcTtf[1].usUsed = 200;
	    stSrcTtf[1].pData = pu8IPData1;
	    stSrcTtf[1].pNext = &stSrcTtf[2];

	    stSrcTtf[2].usUsed = 100;
	    stSrcTtf[2].pData = pu8IPData2;
	    stSrcTtf[2].pNext = &stSrcTtf[3];

	    stSrcTtf[3].usUsed = 200;
	    stSrcTtf[3].pData = pu8IPData3;
	    stSrcTtf[3].pNext = &stSrcTtf[4];

	    stSrcTtf[4].usUsed = 300;
	    stSrcTtf[4].pData = pu8IPData4;
	    stSrcTtf[4].pNext = BSP_NULL;
	for(i=0;i < BDNUM;i++)
	{
	
	    stDlPara[i].u32Data = (BSP_U32)stSrcTtf;
	    stDlPara[i].u16UsrField1= 200+i;
	    stDlPara[i].u16Len= 1000;
	    /*设中断，仅搬移*/
	    stDlPara[i].u16Attribute= 0; 
	}	
	    stDlPara[29].u16Attribute= 1; 
    s32Ret = BSP_IPF_ConfigDownFilter(BDNUM, stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 
    taskDelay(200);
	
    return OK;
    
error1:
    cacheDmaFree(pu8IPData3);
error2:
    cacheDmaFree(pu8IPData2);
error3:
    cacheDmaFree(pu8IPData1);
error4:
    cacheDmaFree(pu8IPData);

    return ERROR;

}


/* 5个TTF结点，配60个BD */
BSP_S32 BSP_IPF_ST_304(BSP_VOID)
{
    BSP_U32 BDNUM = 60;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U8* pu8IPData[BDNUM];
    BSP_U32 u32Len = 1000;
    IPF_CONFIG_DLPARAM_S stDlPara[BDNUM];
    TTF_MEM_ST stSrcTtf[BDNUM][5];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Num = BDNUM;
    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u32Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;

    
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }

    for(i = 0; i < BDNUM; i++)
    {
        pu8IPData[i] = (BSP_U8*)cacheDmaMalloc(u32Len);
        if(pu8IPData[i] == BSP_NULL)
        {
            IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        }

        
        memset(pu8IPData[i], 0x0, u32Len);
        memcpy(pu8IPData[i], &stIPHeader, sizeof(StreamIPv4Header));
        memcpy(pu8IPData[i]+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

        stSrcTtf[i][0].usUsed = 200;
        stSrcTtf[i][0].pData = pu8IPData[i];
        stSrcTtf[i][0].pNext = &stSrcTtf[i][1];

        stSrcTtf[i][1].usUsed = 200;
        stSrcTtf[i][1].pData = pu8IPData[i] + stSrcTtf[i][0].usUsed;
        stSrcTtf[i][1].pNext = &stSrcTtf[i][2];

        stSrcTtf[i][2].usUsed = 100;
        stSrcTtf[i][2].pData = pu8IPData[i] + stSrcTtf[i][1].usUsed;
        stSrcTtf[i][2].pNext = &stSrcTtf[i][3];

        stSrcTtf[i][3].usUsed = 200;
        stSrcTtf[i][3].pData = pu8IPData[i] + stSrcTtf[i][2].usUsed;
        stSrcTtf[i][3].pNext = &stSrcTtf[i][4];

        stSrcTtf[i][4].usUsed = 300;
        stSrcTtf[i][4].pData = pu8IPData[i] + stSrcTtf[i][3].usUsed;
        stSrcTtf[i][4].pNext = BSP_NULL;
    
        stDlPara[i].u32Data= (BSP_U32)stSrcTtf[i];
        stDlPara[i].u16Attribute= 0x23;
        stDlPara[i].u16UsrField1 = 200; 
        stDlPara[i].u16Len= 1000; 
#if 0
        if(*g_ipftestdebug)
        {
            IPF_PRINT("pstDstTtf = 0x%x, pData = 0x%x\n", pstDstTtf[i], pstDstTtf[i]->pData);
        }
#endif
    }
    
    s32Ret = BSP_IPF_ConfigDownFilter(u32Num, stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 

    taskDelay(200);
    
    for(i = 0; i < BDNUM; i++)
    {
        if(pu8IPData[i] != BSP_NULL)
        {
            cacheDmaFree(pu8IPData[i]);
        }
    }
	
    return OK;
error1:
    for(i = 0; i < BDNUM; i++)
    {
        if(pu8IPData[i] != BSP_NULL)
        {
            cacheDmaFree(pu8IPData[i]);
        }
    }
    return ERROR;
}


BSP_S32 BSP_IPF_DlSend_n2(BSP_U32 SendTimes,BSP_U32 SendNum)
{
    IPF_CONFIG_DLPARAM_S stDlPara[IPF_DLBD_DESC_SIZE];
    BSP_S32 s32Ret = 0;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_U32 j = SendTimes;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    BSP_U32 u32Len = 200;
    TTF_MEM_ST stSrcTtf[5];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 DlSendBDNum = 0;
	
    pu8IPData = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData1 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
    pu8IPData2 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData2 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
    pu8IPData3 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData3 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
    pu8IPData4 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData4 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u32Len*5;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;

    memset(pu8IPData, 0x0, u32Len);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memset(pu8IPData1, 0x0, u32Len);
    memset(pu8IPData2, 0x0, u32Len);
    memset(pu8IPData3, 0x0, u32Len);
    memset(pu8IPData4, 0x0, u32Len);
    stSrcTtf[0].usUsed = u32Len;
    stSrcTtf[0].pData = pu8IPData;
    stSrcTtf[0].pNext = &stSrcTtf[1];

    stSrcTtf[1].usUsed = u32Len;
    stSrcTtf[1].pData = pu8IPData1;
    stSrcTtf[1].pNext = &stSrcTtf[2];

    stSrcTtf[2].usUsed = u32Len;
    stSrcTtf[2].pData = pu8IPData2;
    stSrcTtf[2].pNext = &stSrcTtf[3];

    stSrcTtf[3].usUsed = u32Len;
    stSrcTtf[3].pData = pu8IPData3;
    stSrcTtf[3].pNext = &stSrcTtf[4];

    stSrcTtf[4].usUsed = u32Len;
    stSrcTtf[4].pData = pu8IPData4;
    stSrcTtf[4].pNext = BSP_NULL;

    while(j > 0)
    {
	u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
	
	if(u32BDNum > SendNum)
	{
		for(i=0;i < IPF_DLBD_DESC_SIZE;i++)
		{
		    stDlPara[i].u32Data = (BSP_U32)stSrcTtf;
		    stDlPara[i].u16UsrField1= 200;
		    stDlPara[i].u16Len= u32Len*5;
		    /*设中断，仅搬移*/
		    stDlPara[i].u16Attribute= 0x6; 
		}	
		stDlPara[SendNum-1].u16Attribute= 0x7; 

		s32Ret = BSP_IPF_ConfigDownFilter(SendNum, stDlPara);
		if(s32Ret != IPF_SUCCESS)    
		{
			IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
			goto error0;
		} 
		j--;
		DlSendBDNum += (u32BDNum-1);
	}
    }
	IPF_PRINT("DlSendBDNum(BDs) =%u",DlSendBDNum);
	return OK;
error0:
    cacheDmaFree(pu8IPData4);
error1:
    cacheDmaFree(pu8IPData3);
error2:
    cacheDmaFree(pu8IPData2);
error3:
    cacheDmaFree(pu8IPData1);
error4:
    cacheDmaFree(pu8IPData);

    return ERROR;

}

BSP_S32 BSP_IPF_DlSend(void)
{
	return BSP_IPF_DlSend_n2(10000,60);
}
BSP_S32 BSP_IPF_ST_321(BSP_VOID)
{
    BSP_U32 s32Ret;
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
	/*起一个进程，不停地发东西*/
    g_IpfUlTaskId = taskSpawn("IpfDlTask1", 132, 2000, 20000, (FUNCPTR) BSP_IPF_DlSend,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
return OK;
}
BSP_S32 BSP_IPF_ST_321_n2(BSP_U32 sendtimes,BSP_U32 sendnum)
{
    BSP_U32 s32Ret;
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
	/*起一个进程，不停地发东西*/
    g_IpfUlTaskId = taskSpawn("IpfDlTask1", 132, 2000, 20000, (FUNCPTR) BSP_IPF_DlSend_n2,
                                sendtimes, sendnum, 0, 0, 0, 0, 0, 0, 0, 0);
return OK;
}



/* 1个TTF结点，配1个BD 用于验证ADQ的rptr移动是否成功修正 */
BSP_S32 BSP_IPF_ST_331(BSP_VOID)
{
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    IPF_CONFIG_DLPARAM_S stDlPara[IPF_ULBD_DESC_SIZE];
    TTF_MEM_ST stSrcTtf[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
    BSP_U32 u32Num = 2;
    BSP_S32 s32Ret = 0;
    unsigned int i;
	
    s32Ret = IPF_ST_DL_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
    {
        if(u32BDNum != IPF_DLBD_DESC_SIZE)
        {
            IPF_PRINT(" %d :  下行队列不为空u32BDNum = %d,  u32CDNum = %d\n"
            ,__LINE__, u32BDNum, u32CDNum);
        } 
    }
	
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    memcpy(&stIPHeader, &ipf_ipv4_head, sizeof(StreamIPv4Header));
    memcpy(&stUDP, &ipf_udp, sizeof(StreamUDP));
	
    stSrcTtf[0].usUsed = 200;
    stSrcTtf[1].usUsed = 1200;

    for(i=0; i<2; i++)    
    {
	    stSrcTtf[i].pData = cacheDmaMalloc(stSrcTtf[i].usUsed);
	    stSrcTtf[i].pNext = BSP_NULL;
	    if(stSrcTtf[i].pData == BSP_NULL)
	    {
	        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
	        return ERROR;
	    }
		
	    memset(stSrcTtf[i].pData, 0x0, stSrcTtf[i].usUsed);
	    memcpy(stSrcTtf[i].pData, &stIPHeader, sizeof(StreamIPv4Header));
	    memcpy(stSrcTtf[i].pData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	    
	    stDlPara[i].u32Data = (BSP_U32)(&stSrcTtf[i]);
	    stDlPara[i].u16UsrField1= 200;
	    stDlPara[i].u16Len= stSrcTtf[i].usUsed;

	    /*设中断，仅搬移*/
	    stDlPara[i].u16Attribute= 7; 
	    cacheFlush(DATA_CACHE,stSrcTtf[i].pData,stSrcTtf[i].usUsed);
    }
    s32Ret = BSP_IPF_ConfigDownFilter(u32Num, stDlPara);
    if(s32Ret != IPF_SUCCESS)    
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
        goto error1;
    } 
    taskDelay(200);
	
    return OK;
    
error1:
    for(i=0; i<2; i++)    
    {
        cacheDmaFree((BSP_VOID *)(stSrcTtf[i].pData));
    }
    return ERROR;
}

s32 BSP_IPF_ST_331_n(u32 n)
{	
	s32 ret;
	u32 ad0_rptr_last,ad0_rptr_current,ad1_rptr_last,ad1_rptr_current;
	ad0_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
	ad1_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
	while(n>0)
	{
		ret = BSP_IPF_ST_331();
		taskDelay(2);

		ad0_rptr_current = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
		ad1_rptr_current = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
		if((IPF_DLAD0_DESC_SIZE-1) == ad0_rptr_last)
		{
			if(0 != ad0_rptr_current)
			{
				IPF_PRINT(" TESTERROR AD0 add error\n");
				return IPF_ERROR;
			}
		}
		else if(ad0_rptr_current != ad0_rptr_last+1)
		{
			IPF_PRINT(" TESTERROR AD0 add error\n");
			return IPF_ERROR;
		}
		ad0_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
		
		if((IPF_DLAD1_DESC_SIZE-1) == ad1_rptr_last)
		{
			if(0 != ad1_rptr_current)
			{
				IPF_PRINT(" TESTERROR AD1 add error\n");
				return IPF_ERROR;
			}
		}
		else if(ad1_rptr_current != ad1_rptr_last+1)
		{
			IPF_PRINT(" TESTERROR AD1 add error\n");
			return IPF_ERROR;
		}
		ad1_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
		n--;
	}
	return IPF_SUCCESS;
}


BSP_S32 ipf_ccore_init_for_test(BSP_VOID)
{
    BSP_U32 u32BDSize[IPF_CHANNEL_MAX] = {IPF_ULBD_DESC_SIZE, IPF_DLBD_DESC_SIZE};
    BSP_U32 u32RDSize[IPF_CHANNEL_MAX] = {IPF_ULRD_DESC_SIZE, IPF_DLRD_DESC_SIZE};
    BSP_U32 u32ADCtrl[IPF_CHANNEL_MAX] = {IPF_ADQ_DEFAULT_SEETING,IPF_ADQ_DEFAULT_SEETING};

    BSP_U32 u32IntMask0 = 0;
    BSP_U32 u32IntMask1 = 0;
    BSP_U32 u32ChanCtrl[IPF_CHANNEL_MAX] = {0,0};
    BSP_U32 u32IntStatus = 0;
    BSP_U32 u32Timeout = 0;
    BSP_U32 u32IntGate = 0;
    BSP_U32 u32DMAOutstanding = 3;	

IPF_UL_S g_stIpfUl = {0};
IPF_DL_S g_stIpfDl = {0};


    /* 初始化全局结构体 */
    memset((BSP_VOID*)IPF_ULBD_MEM_ADDR, 0x0, SHM_MEM_IPF_SIZE - IPF_DLDESC_SIZE);/* [false alarm]:fortify disable */

    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;
    g_stIpfUl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_ULAD0_MEM_ADDR;
    g_stIpfUl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_ULAD1_MEM_ADDR;
    g_stIpfUl.pu32IdleBd = (BSP_U32*)IPF_ULBD_IDLENUM_ADDR;
    *(BSP_U32*)IPF_ULBD_IDLENUM_ADDR = IPF_ULBD_DESC_SIZE;
    
    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;
    g_stIpfDl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_DLAD0_MEM_ADDR;
    g_stIpfDl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_DLAD1_MEM_ADDR;
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;
    g_stIpfDl.u32IpfCdRptr = (BSP_U32*) IPF_DLCDRPTR_MEM_ADDR;
    *(g_stIpfDl.u32IpfCdRptr) = 0;

    g_stIpfDl.u32IdleBd = IPF_DLBD_DESC_SIZE;
	
    
    /* 配置扩展过滤器的起始地址 */	
    ipf_writel((BSP_U32)IPF_EXT_FILTER_ADDR, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_EF_BADDR_OFFSET);
    
    /* 配置超时配置寄存器，使能超时中断，设置超时时间 */
    u32Timeout = TIME_OUT_CFG | (TIME_OUT_ENABLE);
    ipf_writel(u32Timeout, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_TIME_OUT_OFFSET);

    /* 配置时钟门控配置寄存器，设置为自动门控*/
    u32IntGate = GATE_CFG;
#if (defined(BSP_CONFIG_HI3630))
 //   ipf_writel(u32IntGate, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_GATE_OFFSET);/* [false alarm]:fortify disable */
#endif
    /* 清除上下行队列 */
    u32ChanCtrl[IPF_CHANNEL_UP] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET);
    u32ChanCtrl[IPF_CHANNEL_UP] |= 0x30;
    ipf_writel(u32ChanCtrl[IPF_CHANNEL_UP], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET);

    u32ChanCtrl[IPF_CHANNEL_DOWN] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET);
    u32ChanCtrl[IPF_CHANNEL_DOWN] |= 0x30;
    ipf_writel(u32ChanCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET);

    /* 配置上行通道的AD、BD和RD深度 */
    ipf_writel(u32BDSize[IPF_CHANNEL_UP]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_SIZE_OFFSET);
    ipf_writel(u32RDSize[IPF_CHANNEL_UP]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_SIZE_OFFSET);
    ipf_writel(u32ADCtrl[IPF_CHANNEL_UP], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ_CTRL_OFFSET);

    /* 上行通道的BD和RD起始地址*/    
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfBDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfRDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfADQ0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_BASE_OFFSET);
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfADQ1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_BASE_OFFSET);
	
    /* 配置中断屏蔽,只开5个中断，结果上报，结果超时，RD下溢，AD0、AD1空 */
    u32IntMask0 = IPF_INT_OPEN0;  /* 0号为Modem CPU */
    u32IntMask1 = IPF_INT_OPEN1;
    ipf_writel(u32IntMask0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_MASK0_OFFSET);
    ipf_writel(u32IntMask1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_MASK1_OFFSET);

    /* 清中断 */
    u32IntStatus = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
    ipf_writel(u32IntStatus, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
    /*开DMA通道的Outstanding读写*/
    ipf_writel(u32DMAOutstanding, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_DMA_CTRL1_OFFSET);


    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"[IPF] ipf init success\n");
	
    return IPF_SUCCESS;
    
}

#ifdef CONFIG_MODULE_BUSSTRESS
 /*ipf dl stress test*/
BSP_U32 ipf_dl_stress_test_cnt = 0;
BSP_S32 ipf_dl_stress_test_task_id = 0;

#define BDNUM   IPF_DLBD_DESC_SIZE
StreamIPv4Header stIPHeader;
StreamUDP stUDP;
BSP_U8* pu8IPData[BDNUM] = {0};
IPF_CONFIG_DLPARAM_S stDlPara[BDNUM];
TTF_MEM_ST stSrcTtf[BDNUM][5];
TTF_MEM_ST* pstDstTtf[BDNUM];

BSP_S32 ipf_dl_stress_test_routine(BSP_S32 dtime)
{
    IPF_CONFIG_DLPARAM_S stDlPara[IPF_DLBD_DESC_SIZE];
    BSP_S32 s32Ret = 0;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    BSP_U32 u32Len = 200;
    TTF_MEM_ST stSrcTtf[5];
    BSP_U32 u32CDNum = 0;
    BSP_U32 u32BDNum = 0;
	
    pu8IPData = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData1 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
    pu8IPData2 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData2 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
    pu8IPData3 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData3 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
    pu8IPData4 = (BSP_U8*)cacheDmaMalloc(u32Len);
    if(pu8IPData4 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u32Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;

    memset(pu8IPData, 0x0, u32Len);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
//    memset(pu8IPData1, 0x1, u32Len);
//    memset(pu8IPData2, 0x2, u32Len);
//    memset(pu8IPData3, 0x3, u32Len);
//    memset(pu8IPData4, 0x4, u32Len);
    stSrcTtf[0].usUsed = 200;
    stSrcTtf[0].pData = pu8IPData;
    stSrcTtf[0].pNext = &stSrcTtf[1];

    stSrcTtf[1].usUsed = 200;
    stSrcTtf[1].pData = pu8IPData1;
    stSrcTtf[1].pNext = &stSrcTtf[2];

    stSrcTtf[2].usUsed = 100;
    stSrcTtf[2].pData = pu8IPData2;
    stSrcTtf[2].pNext = &stSrcTtf[3];

    stSrcTtf[3].usUsed = 200;
    stSrcTtf[3].pData = pu8IPData3;
    stSrcTtf[3].pNext = &stSrcTtf[4];

    stSrcTtf[4].usUsed = 200;
    stSrcTtf[4].pData = pu8IPData4;
    stSrcTtf[4].pNext = BSP_NULL;
 
    while(get_test_switch_stat(IPF_DL))
    {
	u32BDNum = BSP_IPF_GetDlBDNum(&u32CDNum);
	if(u32BDNum < IPF_DLBD_DESC_SIZE-4)
	{
	    taskDelay(dtime);
	    continue;
	} 
	else
	{
		for(i=0;i <= 63;i++)
		{
		    stDlPara[i].u32Data = (u32)stSrcTtf;
		    stDlPara[i].u16UsrField1= 200+i;
		    stDlPara[i].u16Len= 900;
		    /*设中断，仅搬移*/
		    stDlPara[i].u16Attribute= 6; 
		}	
		stDlPara[u32BDNum-3].u16Attribute= 7; 
		s32Ret = BSP_IPF_ConfigDownFilter(u32BDNum-2, stDlPara);
		if(s32Ret != IPF_SUCCESS)    
		{
			IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d\n",__LINE__);
			goto error0;
		} 
	}

    }	
    return OK;
error0:
    cacheDmaFree(pu8IPData4);
error1:
    cacheDmaFree(pu8IPData3);
error2:
    cacheDmaFree(pu8IPData2);
error3:
    cacheDmaFree(pu8IPData1);
error4:
    cacheDmaFree(pu8IPData);
    return ERROR;
}

BSP_S32 ipf_dl_stress_test_start(BSP_S32 task_priority,BSP_S32 dtime)
{
	BSP_S32 s32Ret = 0;

	set_test_switch_stat(IPF_DL,TEST_RUN);
	
	s32Ret = IPF_ST_DL_INIT();
	
	ipf_dl_stress_test_task_id = taskSpawn ("ipfDlStressTask", task_priority, 0, 20000, (FUNCPTR)ipf_dl_stress_test_routine, dtime,
	                                    0, 0, 0,0, 0, 0, 0, 0, 0);
	if(ipf_dl_stress_test_task_id == BSP_ERROR)
	{
		printf("taskSpawn ipfDlStressTask fail.\n");
		return ERROR;
	}    
    return OK;
}
BSP_S32 ipf_dl_stress_test_stop()
{
	set_test_switch_stat(IPF_DL,TEST_STOP);
	while(BSP_OK == taskIdVerify(ipf_dl_stress_test_task_id))
	{
	    taskDelay(100);
	}	
	return OK;
}

#endif

/*ipf数传复位回调测试，
主要用于测试数传停止是否成功
进行数传，预期返回失败*/
BSP_VOID BSP_IPF_ST_501_CCORE(BSP_VOID)
{
    IPF_CONFIG_DLPARAM_S stDlPara[IPF_ULBD_DESC_SIZE];	
    BSP_S32 s32Ret;
    /*调用ConfigUpFilter*/
    s32Ret = BSP_IPF_ConfigDownFilter(5, stDlPara);

    if(s32Ret != BSP_ERR_IPF_CCORE_RESETTING)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigDownFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
    }    
}


#endif/*End of vxworks code*/


#ifdef __KERNEL__

BSP_S32 BSP_IPF_ST_100(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[2];
    BSP_U32 u32Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U8* pu8shortData = BSP_NULL;
    BSP_U16 u16Len = 1500;
    BSP_U16 u16shortLen = 300;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num !=  IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }
    pu8shortData = (BSP_U8*)kmalloc(u16shortLen,GFP_KERNEL);
    if(pu8shortData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;

        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    memset(pu8shortData, 0x0, u16shortLen);
    memcpy(pu8shortData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8shortData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x102;
    stUlPara[0].u32UsrField3 = 0;
    /*链0只搬移无中断*/
    stUlPara[0].u16Attribute = 0x06;
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8shortData, u16shortLen, DMA_TO_DEVICE);        
    stUlPara[1].u32Data = (BSP_U32)pBuff;
    stUlPara[1].u16Len = u16shortLen;
    stUlPara[1].u16UsrField1 = 0;
    stUlPara[1].u32UsrField2 = 0x102;
    stUlPara[1].u32UsrField3 = 0;
    /*链0只搬移无中断*/
    stUlPara[1].u16Attribute = 0x06;
	
    s32Ret = BSP_IPF_ConfigUpFilter(2, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    return OK;    
}


/*发送函数*/
BSP_S32 BSP_IPF_ST_101(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_U32 u32UlBdprt = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 1500;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_VOID* pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    
    u32UlBdprt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WPTR_OFFSET);
    IPF_PRINT("BDQ_WPTR IS %d \n",u32UlBdprt);
	
    u32UlBdprt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_RPTR_OFFSET);
    IPF_PRINT("BDQ_RPTR IS %d \n",u32UlBdprt);


    if(u32Num != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT("BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);

    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    IPF_PRINT("sizeof(StreamIPv4Header) = %d  sizeof(StreamUDP) = %d \n", 
                      sizeof(StreamIPv4Header), sizeof(StreamUDP));

    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x101;
    stUlPara[0].u32UsrField3= 0;
    /*链0只搬移无中断*/
    stUlPara[0].u16Attribute = 0x6;
	
    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
    }
    stUlPara[IPF_ULBD_DESC_SIZE-1].u32UsrField3 = 1;
    stUlPara[31].u16Attribute= 0x7;

    /*调用ConfigUpFilter*/
    s32Ret = BSP_IPF_ConfigUpFilter(34, stUlPara);

    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return BSP_ERROR;
    }    
	
    return OK;    
}


BSP_S32 BSP_IPF_ST_102(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[2];
    BSP_U32 u32Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U8* pu8shortData = BSP_NULL;
    BSP_U16 u16Len = 1500;
    BSP_U16 u16shortLen = 300;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num !=  IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }
    pu8shortData = (BSP_U8*)kmalloc(u16shortLen,GFP_KERNEL);
    if(pu8shortData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    memset(pu8shortData, 0x0, u16shortLen);
    memcpy(pu8shortData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8shortData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x102;
    stUlPara[0].u32UsrField3 = 0;
    /*链0只搬移无中断*/
    stUlPara[0].u16Attribute = 0x06;
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8shortData, u16shortLen, DMA_TO_DEVICE);        
    stUlPara[1].u32Data = (BSP_U32)pBuff;
    stUlPara[1].u16Len = u16shortLen;
    stUlPara[1].u16UsrField1 = 0;
    stUlPara[1].u32UsrField2 = 0x102;
    stUlPara[1].u32UsrField3 = 0;
    /*链0只搬移无中断*/
    stUlPara[1].u16Attribute = 0x06;
	
    s32Ret = BSP_IPF_ConfigUpFilter(2, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num != IPF_ULBD_DESC_SIZE - 1)
    {
        IPF_PRINT("BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    stUlPara[0].u16UsrField1 = 1;
    stUlPara[0].u32UsrField3 = 3;
    /*链0过滤只搬移，设中断。*/
    stUlPara[0].u16Attribute = 0x01;

    s32Ret = BSP_IPF_ConfigUpFilter(1, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    return OK;    
}


BSP_S32 BSP_IPF_ST_102sn(BSP_U32 u32sendnum)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U8* pu8shortData = BSP_NULL;
    BSP_U16 u16shortLen = 300;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num !=  IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    pu8Data = (BSP_U8*)kmalloc(u16shortLen,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }
    pu8shortData = (BSP_U8*)kmalloc(u16shortLen,GFP_KERNEL);
    if(pu8shortData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }
    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16shortLen;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16shortLen);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    memset(pu8shortData, 0x0, u16shortLen);
    memcpy(pu8shortData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8shortData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (void *)pu8Data, u16shortLen, DMA_TO_DEVICE);        
for(i=0;i<IPF_ULBD_DESC_SIZE;i++)
{
    stUlPara[i].u32Data = (BSP_U32)pBuff;
    stUlPara[i].u16Len = u16shortLen;
    stUlPara[i].u16UsrField1 = 0;
    stUlPara[i].u32UsrField2 = 0x102;
    stUlPara[i].u32UsrField3 = 0;
    /*链0只搬移无中断*/
    stUlPara[i].u16Attribute = 0x06;
}		
    stUlPara[u32sendnum-2].u16Attribute = 0x07;
    s32Ret = BSP_IPF_ConfigUpFilter(u32sendnum, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    return OK;    
}


BSP_S32 BSP_IPF_ST_103(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 300;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num !=  IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    for(i=0;i<IPF_ULBD_DESC_SIZE;i++)
    {
        stUlPara[i].u32Data = (BSP_U32)pBuff;
        stUlPara[i].u16Len = u16Len;
        stUlPara[i].u16UsrField1 = 0;
        stUlPara[i].u32UsrField2 = 0x102;
        stUlPara[i].u32UsrField3 = 0;
        /*链0只搬移无中断*/
        stUlPara[i].u16Attribute = 0x06;
    }		
    stUlPara[31].u16Attribute = 0x07;
    s32Ret = BSP_IPF_ConfigUpFilter(34, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    return OK;    
}

/*发送函数*/
BSP_S32 BSP_IPF_ST_104(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_U32 u32UlBdprt = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 300;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_VOID* pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    
    u32UlBdprt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WPTR_OFFSET);
    IPF_PRINT("BDQ_WPTR IS %d \n",u32UlBdprt);
	
    u32UlBdprt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_RPTR_OFFSET);
    IPF_PRINT("BDQ_RPTR IS %d \n",u32UlBdprt);


    if(u32Num != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT("BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);

    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    IPF_PRINT("sizeof(StreamIPv4Header) = %d  sizeof(StreamUDP) = %d \n", 
                      sizeof(StreamIPv4Header), sizeof(StreamUDP));

    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x101;
    stUlPara[0].u32UsrField3= 0;
    /*链0只搬移无中断*/
    stUlPara[0].u16Attribute = 0x6;
	
    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
    }
    stUlPara[IPF_ULBD_DESC_SIZE-1].u32UsrField3 = 1;
    stUlPara[31].u16Attribute= 0x7;

    /*调用ConfigUpFilter，设定终端上报位*/
    s32Ret = BSP_IPF_ConfigUpFilter(34, stUlPara);

    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return BSP_ERROR;
    }    
    /*调用ConfigUpFilter，没有设定终端上报位*/
    s32Ret = BSP_IPF_ConfigUpFilter(34, stUlPara);

    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return BSP_ERROR;
    }    
    return OK;    
}

/*发送函数*/
BSP_S32 BSP_IPF_ST_105(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_U32 u32UlBdprt = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 300;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_VOID* pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    
    u32UlBdprt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WPTR_OFFSET);
    IPF_PRINT("BDQ_WPTR IS %d \n",u32UlBdprt);
	
    u32UlBdprt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_RPTR_OFFSET);
    IPF_PRINT("BDQ_RPTR IS %d \n",u32UlBdprt);


    if(u32Num != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT("BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);

    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    IPF_PRINT("sizeof(StreamIPv4Header) = %d  sizeof(StreamUDP) = %d \n", 
                      sizeof(StreamIPv4Header), sizeof(StreamUDP));

    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x101;
    stUlPara[0].u32UsrField3= 0;
    /*链0只搬移无中断*/
    stUlPara[0].u16Attribute = 0x6;
	
    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
    }
    stUlPara[IPF_ULBD_DESC_SIZE-1].u32UsrField3 = 1;
    stUlPara[31].u16Attribute= 0x7;

    /*调用ConfigUpFilter，设定终端上报位*/
    s32Ret = BSP_IPF_ConfigUpFilter(30, stUlPara);

    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return BSP_ERROR;
    }    
    /*调用ConfigUpFilter，没有设定终端上报位*/
    s32Ret = BSP_IPF_ConfigUpFilter(30, stUlPara);

    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return BSP_ERROR;
    }    
    return OK;    
}

/*以下代码测试过滤搬移*/
BSP_S32 BSP_IPF_ST_111(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 1500;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_VOID *pBuff;
	
    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);

    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, 1500);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x211;
    stUlPara[0].u32UsrField3 = 0;
    stUlPara[0].u16Attribute = 0x00;
	
    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
        stUlPara[i].u16Attribute = 0x00;
    }
    for(i = 10; i < IPF_ULBD_DESC_SIZE-10; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
        stUlPara[i].u16Attribute = 0x10;
    }

    stUlPara[IPF_ULBD_DESC_SIZE-5].u32UsrField3= 1;
    stUlPara[IPF_ULBD_DESC_SIZE-5].u16Attribute= 1;
	
    s32Ret = BSP_IPF_ConfigUpFilter(IPF_ULBD_DESC_SIZE-5, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    return OK;
}

BSP_S32 BSP_IPF_ST_112(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[1];
    BSP_U32 u32Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 1500;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num !=  IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, 1500);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x212;
    stUlPara[0].u32UsrField3 = 0;
    stUlPara[0].u16Attribute= 0;
	
    s32Ret = BSP_IPF_ConfigUpFilter(1, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT("%d TESTERROR    BSP_IPF_ConfigUpFilter : s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num != IPF_ULBD_DESC_SIZE - 1)
    {
        IPF_PRINT(" %d BSP_IPF_GetUlBDNum : u32Num = %d\n",__LINE__, u32Num);
    }

    stUlPara[0].u16UsrField1 = 1;
    stUlPara[0].u32UsrField3 = 3;
	
    stUlPara[0].u16Attribute= 1;
	
    s32Ret = BSP_IPF_ConfigUpFilter(1, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT("%d  TESTERROR    BSP_IPF_ConfigUpFilter :s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    return OK;
}

BSP_S32 BSP_IPF_ST_113(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 1500;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 i = 0;
    BSP_VOID *pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT("BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);

    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, 1500);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    pBuff = (BSP_U32 *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x213;
    stUlPara[0].u32UsrField3 = 0;
    stUlPara[0].u16Attribute = 0;
    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
        stUlPara[i].u16Attribute = 0;

    }
    stUlPara[31].u16Attribute = 1;
    stUlPara[IPF_ULBD_DESC_SIZE-1].u16Attribute = 1;
    s32Ret = BSP_IPF_ConfigUpFilter(32, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num < (IPF_ULBD_DESC_SIZE - 32))
    {
        IPF_PRINT(" BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    stUlPara[IPF_ULBD_DESC_SIZE-1].u32UsrField3 = 3;
    s32Ret = BSP_IPF_ConfigUpFilter(IPF_ULBD_DESC_SIZE - 32, &stUlPara[32]);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return ERROR;
    }
#if 0
    if(*g_ipftestdebug)
    {
        for(i = 0; i < IPF_ULBD_DESC_SIZE; i++)
        {
            IPF_PRINT(" test: %d  %x, %d\n", i, stUlPara[i].u32UsrField2 ,stUlPara[i].u32UsrField3);
        }
    }
#endif
    s32Ret = BSP_IPF_ConfigTimeout(500);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }
    return OK;
}

/*dpm测试*/
#ifdef CONFIG_PM
BSP_S32 BSP_IPF_ST_130(BSP_VOID)
{
    dpm_suspend_start(PMSG_SUSPEND);
    dpm_suspend_end(PMSG_SUSPEND);
    dpm_resume_start(PMSG_RESUME);
    dpm_resume_end(PMSG_RESUME);
    return OK;
	
}
#endif





BSP_S32 BSP_IPF_UlSend(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32BDNum = 0;
    BSP_U32 i = 0;
    BSP_U32 j = 10000;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 640;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);  
    for(i=0;i<IPF_ULBD_DESC_SIZE;i++)
    {
        stUlPara[i].u32Data = (BSP_U32)pBuff;
        stUlPara[i].u16Len = u16Len-i*9;
        stUlPara[i].u16UsrField1 = 0;
        stUlPara[i].u32UsrField2 = 0x102;
        stUlPara[i].u32UsrField3 = 0;
        /*链0只搬移无中断*/
        stUlPara[i].u16Attribute = 0x06;
    }		

    while(j>0)
    {
        u32BDNum = BSP_IPF_GetUlBDNum();
        if(u32BDNum > 20)
        {
            stUlPara[u32BDNum-3].u16Attribute = 0x07;
            s32Ret = BSP_IPF_ConfigUpFilter(u32BDNum-2, stUlPara);
            if(s32Ret != IPF_SUCCESS)
            {
                IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
                kfree(pu8Data);
                return ERROR;
            }
            j--;
        }
    }
    return OK;    
}

/*上行数传测试，规避队头阻塞版本*/
BSP_S32 BSP_IPF_UlSend122(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32BDNum = 0;
    BSP_U32 i = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 640;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_VOID *pBuff;
    BSP_U32 j = 10000;

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
       
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
	
    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);  
    for(i=0;i<IPF_ULBD_DESC_SIZE;i++)
    {
        stUlPara[i].u32Data = (BSP_U32)pBuff;
        stUlPara[i].u16Len = u16Len-i*9;
        stUlPara[i].u16UsrField1 = 0;
        stUlPara[i].u32UsrField2 = 0x102;
        stUlPara[i].u32UsrField3 = 0;
        /*链0只搬移无中断*/
        stUlPara[i].u16Attribute = 0x06;
    }		

    while(j>0)
    {
        u32BDNum = BSP_IPF_GetUlDescNum();
        if(u32BDNum > 1)
        {
            stUlPara[u32BDNum-1].u16Attribute = 0x07;
            s32Ret = BSP_IPF_ConfigUpFilter(u32BDNum, stUlPara);
            if(s32Ret != IPF_SUCCESS)
            {
                IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
                kfree(pu8Data);
                return ERROR;
            }
            j--;
        }

       else if(u32BDNum == 1)
        {
            stUlPara[u32BDNum-1].u16Attribute = 0x07;
            s32Ret = BSP_IPF_ConfigUpFilter(u32BDNum, stUlPara);
            if(s32Ret != IPF_SUCCESS)
            {
                IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
                kfree(pu8Data);
                return ERROR;
            }
            j--;
        }
    }
    return OK;    
}



BSP_S32 BSP_IPF_ST_121(BSP_VOID)
{

    /*起一个进程，不停地发东西*/
    g_IpfDlTaskId = kthread_run((IPFTHREADFN)BSP_IPF_UlSend, BSP_NULL, "IpfUlTask");  
    return OK;

}



BSP_S32 BSP_IPF_ST_122(BSP_VOID)
{

    /*起一个进程，不停地发东西*/
    g_IpfDlTaskId = kthread_run((IPFTHREADFN)BSP_IPF_UlSend122, BSP_NULL, "IpfUlTask");  
    return OK;

}

/*发送函数*/
BSP_S32 BSP_IPF_ST_131(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];
    BSP_U32 u32Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U8* pu8Data = BSP_NULL;
    BSP_U16 u16Len = 1200;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    unsigned int i;
    BSP_VOID* pBuff;

    u32Num = BSP_IPF_GetUlBDNum();
    if(u32Num != IPF_ULBD_DESC_SIZE)
    {
        IPF_PRINT("BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
    }

    pu8Data = (BSP_U8*)kmalloc(u16Len,GFP_KERNEL);
    if(pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    malloc :%d\n",__LINE__);
        return ERROR;
    }

    memcpy(&stIPHeader, &ipf_ipv4_head, sizeof(StreamIPv4Header));
    memcpy(&stUDP, &ipf_udp, sizeof(StreamUDP));
        
    memset(pu8Data, 0x0, u16Len);
    memcpy(pu8Data, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8Data+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

    IPF_PRINT("sizeof(StreamIPv4Header) = %d  sizeof(StreamUDP) = %d \n", 
                      sizeof(StreamIPv4Header), sizeof(StreamUDP));

    pBuff = (BSP_VOID *)dma_map_single(NULL, (BSP_VOID *)pu8Data, u16Len, DMA_TO_DEVICE);        
    stUlPara[0].u32Data = (BSP_U32)pBuff;
    stUlPara[0].u16Len = u16Len;
    stUlPara[0].u16UsrField1 = 0;
    stUlPara[0].u32UsrField2 = 0x101;
    stUlPara[0].u32UsrField3= 0;
    stUlPara[0].u16Attribute = 0x7;
	
    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
    {
        memcpy(&stUlPara[i], &stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
        stUlPara[i].u16UsrField1 = i;
    }
    stUlPara[1].u16Len = 200;

    /*调用ConfigUpFilter*/
    s32Ret = BSP_IPF_ConfigUpFilter(2, stUlPara);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
        kfree(pu8Data);
        return BSP_ERROR;
    }    
	
    return OK;    
}
s32 BSP_IPF_ST_131_n(u32 n)
{	
	s32 ret;
	u32 ad0_rptr_last,ad0_rptr_current,ad1_rptr_last,ad1_rptr_current;
	ad0_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
	ad1_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
	while(n>0)
	{
		ret = BSP_IPF_ST_131();
		mdelay(2);
		ad0_rptr_current = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
		ad1_rptr_current = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
		if((IPF_ULAD0_DESC_SIZE-1) == ad0_rptr_last)
		{
			if(0 != ad0_rptr_current)
			{
				IPF_PRINT(" TESTERROR AD0 add error\n");
				return IPF_ERROR;
			}
		}
		else if(ad0_rptr_current != ad0_rptr_last+1)
		{
			IPF_PRINT(" TESTERROR AD0 add error\n");
			return IPF_ERROR;
		}
		ad0_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
		
		if((IPF_ULAD1_DESC_SIZE-1) == ad1_rptr_last)
		{
			if(0 != ad1_rptr_current)
			{
				IPF_PRINT(" TESTERROR AD1 add error\n");
				return IPF_ERROR;
			}
		}
		else if(ad1_rptr_current != ad1_rptr_last+1)
		{
			IPF_PRINT(" TESTERROR AD1 add error\n");
			return IPF_ERROR;
		}
		ad1_rptr_last = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
		n--;
	}
	return IPF_SUCCESS;
}


#ifdef CONFIG_MODULE_BUSSTRESS
/*ipf ul stress test*/
IPF_CONFIG_ULPARAM_S g_stUlPara[IPF_ULBD_DESC_SIZE];
BSP_U32 g_u32Num = 0;
BSP_U8* g_pu8Data = BSP_NULL;
BSP_U16 g_u16Len = 1500;
StreamIPv4Header g_stIPHeader;
StreamUDP g_stUDP;	
BSP_S32 ipf_ul_stress_test_stat = 0;
BSP_S32 ipf_ul_stress_test_interval = 0;
BSP_S32 ipf_ul_stress_test_routine(BSP_VOID)
{

    BSP_S32 s32Ret = 0;
    BSP_U32 i = 0;
    BSP_U8* pBuff = NULL;
    BSP_U32 u32StressSendNum = 60;
    IPF_PRINT("ipf_ul_stress_test_routine running\n");
    g_pu8Data = (BSP_U8*)kmalloc(g_u16Len,GFP_KERNEL);
    if(g_pu8Data == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR    BSP_SMALLOC :%d\n",__LINE__);
        return ERROR;
    }
    while(ipf_ul_stress_test_stat)
    {
	    g_u32Num = BSP_IPF_GetUlBDNum();
	    if(g_u32Num < u32StressSendNum)
	    {
#if 0
		IPF_PRINT(" TESTERROR    BSP_IPF_GetUlBDNum :%d u32Num = %d\n",__LINE__, u32Num);
#endif
		 msleep(ipf_ul_stress_test_interval);
		 continue;
	    }
				


	    g_stIPHeader.Vers = 4;
	    g_stIPHeader.HeaderLen = 5;
	    g_stIPHeader.TOS = 3;
	    g_stIPHeader.TOL = g_u16Len;
	    g_stIPHeader.ID = 1;
	    g_stIPHeader.ReservFlag = 0;
	    g_stIPHeader.DF = 0;
	    g_stIPHeader.MF = 0;
	    g_stIPHeader.offset = 0;
	    g_stIPHeader.TTL = 128;
	    g_stIPHeader.Prot = 17; /* UDP */
	    g_stIPHeader.Checksum = 0xEC3A;
	    g_stIPHeader.SourceAddr = 0xACA84649;
	    g_stIPHeader.DestAddr = 0x0202014E;

	    g_stUDP.SrcPort = 5088;
	    g_stUDP.DstPort = 2923;
	    g_stUDP.Checksum = 0;
	    g_stUDP.Length = g_stIPHeader.TOL;
	        
	    memset(g_pu8Data, 0x0, 1500);
	    memcpy(g_pu8Data, &g_stIPHeader, sizeof(StreamIPv4Header));
	    memcpy(g_pu8Data+sizeof(StreamIPv4Header), &g_stUDP, sizeof(StreamUDP));
#if 0
	    IPF_PRINT("sizeof(StreamIPv4Header) = %d  sizeof(StreamUDP) = %d \n", 
	                      sizeof(StreamIPv4Header), sizeof(StreamUDP));
#endif
	    pBuff = (BSP_U8 *)dma_map_single(NULL, (void *)g_pu8Data, g_u16Len, DMA_TO_DEVICE);        

	    g_stUlPara[0].u32Data = (BSP_U32)pBuff;
	    g_stUlPara[0].u16Len = g_u16Len;
	    g_stUlPara[0].u16Attribute = 0x06;
	    for(i = 1; i < IPF_ULBD_DESC_SIZE; i++)
	    {
	        memcpy(&g_stUlPara[i], &g_stUlPara[0], sizeof(IPF_CONFIG_ULPARAM_S));
	        g_stUlPara[i].u16UsrField1 = i;
	    }
	    g_stUlPara[u32StressSendNum-1].u16Attribute= 0x7;

	    s32Ret = BSP_IPF_ConfigUpFilter(u32StressSendNum, g_stUlPara);
	    if(s32Ret != IPF_SUCCESS)
	    {
	        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
	        kfree(g_pu8Data);
	        return ERROR;
	    }
    }
    return OK;    
}
BSP_VOID set_ipf_ul_test_stat(BSP_S32 run_or_stop)
{
	ipf_ul_stress_test_stat = run_or_stop;
}

BSP_VOID ipf_ul_stress_test_start(BSP_S32 interval)
{
	ipf_ul_stress_test_interval = interval;	
	set_ipf_ul_test_stat(1);
	kthread_run((IPFTHREADFN)ipf_ul_stress_test_routine,BSP_NULL,"ipf_ul_stest_task");
	IPF_PRINT("test start stat \n");
}

BSP_VOID ipf_ul_stress_test_stop(BSP_VOID)
{
	set_ipf_ul_test_stat(0);
	msleep(2000);
}
#endif
BSP_S32 BSP_IPF_ST_300_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 u16Len = 1000;
	
    g_pu8STDlData = kmalloc(u16Len,GFP_KERNEL);
    if(g_pu8STDlData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset((BSP_U32 *)g_pu8STDlData, 0x0, u16Len);
    memcpy((BSP_U32 *)g_pu8STDlData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

	
    s32Ret = IPF_ST_DL_ACore_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    
    s32Ret = BSP_IPF_RegisterWakeupDlCb(IpfDlIntCb300);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IPF_ST_DL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    IPF_PRINT(" M核可以输入测试用例\n");

    
    return OK;
}
BSP_S32 BSP_IPF_ST_301_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 u16Len = 1000;
	
    g_pu8STDlData = kmalloc(u16Len,GFP_KERNEL);
    if(g_pu8STDlData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u16Len;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset((BSP_U32 *)g_pu8STDlData, 0x0, u16Len);
    memcpy((BSP_U32 *)g_pu8STDlData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));

	
    s32Ret = IPF_ST_DL_ACore_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    
    s32Ret = BSP_IPF_RegisterWakeupDlCb(IpfDlIntCb301);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IPF_ST_DL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    IPF_PRINT(" M核可以输入测试用例\n");

    
    return OK;
}

BSP_S32 BSP_IPF_ST_302_INIT(BSP_VOID)
{    
    BSP_S32 s32Ret = 0;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 u16Len = 1000;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    BSP_U32 u32Len = 200;
	
    g_pu8STDlData = kmalloc(u16Len,GFP_KERNEL);
    if(g_pu8STDlData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }

    pu8IPData1 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
	
    pu8IPData2 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData2 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
	
    pu8IPData3 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData3 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
	
    pu8IPData4 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData4 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error5;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = 1000;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset(pu8IPData, 0x0, 200);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memset(pu8IPData1, 0x0, 200);
    memset(pu8IPData2, 0x0, 100);
    memset(pu8IPData3, 0x0, 200);
    memset(pu8IPData4, 0x0, 300);

    memset((BSP_U32 *)g_pu8STDlData, 0x0, u16Len);
    memcpy((BSP_U32 *)g_pu8STDlData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+200, pu8IPData1, 200);
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+400, pu8IPData2, 100);
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+500, pu8IPData3, 200);
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+700, pu8IPData4, 300);
    s32Ret = IPF_ST_DL_ACore_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    
    s32Ret = BSP_IPF_RegisterWakeupDlCb(IpfDlIntCb302);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IPF_ST_DL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    IPF_PRINT(" M核可以输入测试用例\n");
	kfree(pu8IPData1);
	kfree(pu8IPData2);
	kfree(pu8IPData3);
	kfree(pu8IPData4);
    return OK;
error5:
	kfree(pu8IPData3);
error4:
	kfree(pu8IPData2);
error3:
	kfree(pu8IPData1);
error2:
	kfree(pu8IPData);
error1:
	kfree(g_pu8STDlData);
    return ERROR;
}

BSP_S32 BSP_IPF_ST_303_INIT(BSP_VOID)
{
    return BSP_IPF_ST_302_INIT();
}
BSP_S32 BSP_IPF_ST_304_INIT(BSP_VOID)
{    
    BSP_S32 s32Ret = 0;
    s32Ret = IPF_ST_DL_ACore_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
        return ERROR;
    }
    
    s32Ret = BSP_IPF_RegisterWakeupDlCb(IpfDlIntCb304);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IPF_ST_DL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }   
    IPF_PRINT(" M核可以输入测试用例\n");

    
    return OK;
}

#ifdef CONFIG_MODULE_BUSSTRESS
/*ipf dl stress test for auto test*/
BSP_S32 ipf_dl_stress_test_stat = 0;
BSP_U32 ipf_dl_test_cnt = 0;
BSP_VOID show_ipf_dl_test_cnt(BSP_VOID)
{
	printk("ipf_dl_test_cnt:%d\n",ipf_dl_test_cnt);
}
BSP_VOID ipf_dl_stress_test_routine(BSP_VOID)
{
	while(ipf_dl_stress_test_stat)
	{
		down(&g_IpfDlTaskSemId);
		ipf_dl_test_cnt++;
	}
}
BSP_VOID set_ipf_dl_test_stat(BSP_S32 run_or_stop)
{
	ipf_dl_stress_test_stat = run_or_stop;
}
	
BSP_S32 ipf_dl_stress_test_init(BSP_VOID)
{
	BSP_S32 s32Ret = 0;
	
	s32Ret = IPF_ST_DL_ACore_INIT();
	if(s32Ret != OK)
	{
	    IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
	    return ERROR;
	}
	s32Ret = IPF_ST_DL_INIT_STRESS();
	if(s32Ret != OK)
	{
	    IPF_PRINT(" TESTERROR    IPF_ST_DL_INIT :%d\n",__LINE__);
	    return ERROR;
	}

	s32Ret = BSP_IPF_RegisterWakeupDlCb(IpfDlIntCb);
	if(s32Ret != IPF_SUCCESS)
	{
	    IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
	    return ERROR;
	}  
	s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IPF_ST_DL_FlushAD);
	if(s32Ret != IPF_SUCCESS)
	{
	    IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
	    return ERROR;
	}   
	ipf_dl_test_cnt = 0;
	
	set_ipf_dl_test_stat(1);
	kthread_run((IPFTHREADFN)ipf_dl_stress_test_routine,BSP_NULL,"ipf_dl_stest_task");
	return OK;
}

BSP_S32 ipf_dl_stress_test_start(BSP_VOID)
{
	return ipf_dl_stress_test_init();
}

BSP_VOID ipf_dl_stress_test_stop(BSP_VOID)
{
	set_ipf_dl_test_stat(0);
	msleep(2000);
}
#endif

BSP_S32 IpfDltestIntCb(void)
{
    BSP_U32 u32Num = IPF_DLRD_DESC_SIZE;
    BSP_U32 i = 0;
    BSP_U32 j = 0;
    BSP_U32 u32RecivedData = 0;
    BSP_U32 u32AD0Num = 0;
    BSP_U32 u32AD1Num = 0;
    BSP_S32 s32Ret = 0;
    BSP_U32 *p = NULL;
    IPF_RD_DESC_S stRd[IPF_DLRD_DESC_SIZE];
    BSP_U32	u32AD0wptr;
    BSP_U32	u32AD1wptr;
	/*处理RD*/
        BSP_IPF_GetDlRd(&u32Num, stRd);
#if 1
        for(j = 0; j < u32Num; j++)
        {	
       /* 	IPF_PRINT("stRd[%d].u32OutPtr =%x stRd[%d].u16PktLen =%u \n",j,stRd[j].u32OutPtr,j,stRd[j].u16PktLen);*/
		dma_map_single(NULL, (void*)stRd[j].u32OutPtr, stRd[j].u16PktLen, DMA_FROM_DEVICE);

		if(0 == stRd[j].u32OutPtr)
		{
        		IPF_PRINT("stRd[%d].u32OutPtr =%x !return ERROR\n",j,stRd[j].u32OutPtr);
			return ERROR;
		}
		s32Ret = CmpDMAData((BSP_U32 *)g_pu8STDlData, (BSP_U32 *)stRd[j].u32OutPtr, stRd[j].u16PktLen);
		u32RecivedData += stRd[j].u16PktLen;
		/*数据比对*/
		if(0 != s32Ret)
		{
			g_u32DlError++;
/*			if(0 == g_u32DlError%2247)*/
			if(0)
			{
				    IPF_PRINT("g_pu8STDlData,u32Num = %u\n",u32Num);
				    IPF_PRINT("===================================\n");
				    p=(BSP_U32 *)g_pu8STDlData;
				    for(i=0; i<4; i++)
				    {
				        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
				        i = i+4;
				    }
				    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));
					
		                  IPF_PRINT("Err stRd[%d].u32OutPtr =%x\n",j,stRd[j].u32OutPtr);
		                  IPF_PRINT("===================================\n");
				    p=(BSP_U32 *)stRd[j].u32OutPtr;
				    for(i=0; i<4; i++)
				    {
				        IPF_PRINT("0x%08x    0x%08x    0x%08x    0x%08x\n", *(p+i), *(p+i+1), *(p+i+2), *(p+i+3));
				        i = i+4;
				    }
				    IPF_PRINT("0x%08x    0x%08x\n", *(p+i), *(p+i+1));

			}
		}
		if(stRd[j].u16Result != 0x3f)
		{
			g_u32DlFltError++;
		}	
        }
#endif
	u32RecivedData = u32RecivedData/1024;
	/*统计结果*/
	g_u32DlTotalDataNum += u32RecivedData;
	g_u32DlTotalBDNum += u32Num;
	s32Ret = BSP_IPF_GetDlAdNum(&u32AD0Num,&u32AD1Num);

	if(IPF_SUCCESS != s32Ret)
	{
		IPF_PRINT(" TESTERROR    BSP_IPF_GetDlAdNum :%d\n",__LINE__);
	}

	
    if(u32AD0Num > 10)
    {
	u32AD0wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET );
	if((IPF_ULAD0_DESC_SIZE -u32AD0wptr) >= u32AD0Num)
	{
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,u32AD0Num,&ADdlshort[u32AD0wptr]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
	      	 }
	}
	else
	{
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,(IPF_ULAD0_DESC_SIZE-u32AD0wptr),&ADdlshort[u32AD0wptr]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
	      	 }
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_0,(u32AD0Num-(IPF_ULAD0_DESC_SIZE-u32AD0wptr)),&ADdlshort[0]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd0 :%d\n",__LINE__);
	      	 }
	}
#if 0
        for(i=0;i < u32AD0Num;i++)
        {
		u32AD0wptr = ((u32AD0wptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32AD0wptr + 1) : 0;		
		dma_unmap_single(NULL, ADdlshort[u32AD0wptr].u32OutPtr1, 438, DMA_TO_DEVICE);
        }
#endif
    }
    if(u32AD1Num > 10)
    {
	u32AD1wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET );
	if((IPF_DLAD1_DESC_SIZE-u32AD1wptr) >= u32AD1Num)
	{
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,u32AD1Num,&ADdllong[u32AD1wptr]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
      	  		    IPF_PRINT(" u32AD1Num = %u   u32AD1wptr = %u \n",u32AD1Num,u32AD1wptr);
			    return ERROR;
	      	 }
	}
	else
	{
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,(IPF_DLAD1_DESC_SIZE-u32AD1wptr),&ADdllong[u32AD1wptr]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
			    return ERROR;
	      	 }
        	s32Ret = BSP_IPF_ConfigDlAd(IPF_AD_1,(u32AD1Num-(IPF_DLAD1_DESC_SIZE-u32AD1wptr)),&ADdllong[0]);
	        if(IPF_SUCCESS != s32Ret)
       	 {
      	  		    IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUlAd1 :%d\n",__LINE__);
			    return ERROR;
	      	 }
	}
#if 0
        for(i=0;i < u32AD1Num;i++)
        {
		u32AD1wptr = ((u32AD1wptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32AD1wptr + 1) : 0;		
		dma_unmap_single(NULL, ADdllong[u32AD1wptr].u32OutPtr1, 1500, DMA_TO_DEVICE);

        }
#endif
    }

	return OK;
}
BSP_VOID BSP_IPF_ST_321_Result(BSP_VOID)
{
		IPF_PRINT(" g_u32DlError(BDs) :%d\n",g_u32DlError);
		IPF_PRINT(" g_u32DlFltError(BDs) :%d\n",g_u32DlFltError);
		IPF_PRINT(" g_u32DlTotalDataNum(KB) :%d\n",g_u32DlTotalDataNum);
		IPF_PRINT(" g_u32DlTotalBDNum(BDs) :%d\n",g_u32DlTotalBDNum);
}

/*下行压力测试A核准备函数*/
BSP_S32 BSP_IPF_ST_321_INIT(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    StreamIPv4Header stIPHeader;
    StreamUDP stUDP;
    BSP_U32 u16Len = 1000;
    BSP_U8* pu8IPData;
    BSP_U8* pu8IPData1;
    BSP_U8* pu8IPData2;
    BSP_U8* pu8IPData3;
    BSP_U8* pu8IPData4;
    BSP_U32 u32Len = 200;
	
    g_pu8STDlData = kmalloc(u16Len,GFP_KERNEL);
    if(g_pu8STDlData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        return ERROR;
    }

    pu8IPData = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error1;
    }

    pu8IPData1 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData1 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error2;
    }
	
    pu8IPData2 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData2 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error3;
    }
	
    pu8IPData3 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData3 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error4;
    }
	
    pu8IPData4 = (BSP_U8*)kmalloc(u32Len,GFP_KERNEL);
    if(pu8IPData4 == BSP_NULL)
    {
        IPF_PRINT(" TESTERROR  cacheDmaMalloc :%d\n",__LINE__);
        goto error5;
    }

    stIPHeader.Vers = 4;
    stIPHeader.HeaderLen = 5;
    stIPHeader.TOS = 3;
    stIPHeader.TOL = u32Len*5;
    stIPHeader.ID = 1;
    stIPHeader.ReservFlag = 0;
    stIPHeader.DF = 0;
    stIPHeader.MF = 0;
    stIPHeader.offset = 0;
    stIPHeader.TTL = 128;
    stIPHeader.Prot = 17; /* UDP */
    stIPHeader.Checksum = 0xEC3A;
    stIPHeader.SourceAddr = 0xACA84649;
    stIPHeader.DestAddr = 0x0202014E;

    stUDP.SrcPort = 5088;
    stUDP.DstPort = 2923;
    stUDP.Checksum = 0;
    stUDP.Length = stIPHeader.TOL;
    memset(pu8IPData, 0x0, u32Len);
    memcpy(pu8IPData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(pu8IPData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memset(pu8IPData1, 0x0, u32Len);
    memset(pu8IPData2, 0x0, u32Len);
    memset(pu8IPData3, 0x0, u32Len);
    memset(pu8IPData4, 0x0, u32Len);

    memset((BSP_U32 *)g_pu8STDlData, 0x0, u16Len);
    memcpy((BSP_U32 *)g_pu8STDlData, &stIPHeader, sizeof(StreamIPv4Header));
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header), &stUDP, sizeof(StreamUDP));
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+u32Len, pu8IPData1, u32Len);
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+u32Len*2, pu8IPData2, u32Len);
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+u32Len*3, pu8IPData3, u32Len);
    memcpy(g_pu8STDlData+sizeof(StreamIPv4Header)+u32Len*4, pu8IPData4, u32Len);

    s32Ret = IPF_ST_DL_ACore_INIT();
    if(s32Ret != OK)
    {
        IPF_PRINT(" TESTERROR    IPF_ST_UL_INIT :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_RegisterWakeupDlCb(IpfDltestIntCb);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterWakeupDlCb :%d\n",__LINE__);
        return ERROR;
    }    
	
    s32Ret = BSP_IPF_RegisterAdqEmptyDlCb(IPF_ST_DL_FlushAD);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_RegisterAdqEmptyDlCb :%d\n",__LINE__);
        return ERROR;
    }

    s32Ret = BSP_IPF_ConfigTimeout(0x65);
    if(s32Ret != IPF_SUCCESS)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigTimeout :%d\n",__LINE__);
        return ERROR;
    }

    IPF_PRINT(" M核可以输入测试用例\n");
	kfree(pu8IPData1);
	kfree(pu8IPData2);
	kfree(pu8IPData3);
	kfree(pu8IPData4);
    return OK;
error5:
	kfree(pu8IPData3);
error4:
	kfree(pu8IPData2);
error3:
	kfree(pu8IPData1);
error2:
	kfree(pu8IPData);
error1:
	kfree(g_pu8STDlData);
    return ERROR;
   
}

/*ipf数传复位回调测试，初始化ipf
用于测试数传停止是否成功*/
BSP_VOID BSP_IPF_ST_501_INIT(BSP_U32 reset_flag)
{
	if(0 == reset_flag)
	{
		BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_ALLOW);
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r eanble ipf transfer. \n");
	}
	else
	{
		BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_FORBID);
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r eanble ipf transfer. \n");
	}
}



/*ipf数传复位回调测试，
用于测试数传停止是否成功
进行数传，预期返回失败*/
BSP_VOID BSP_IPF_ST_501_ACORE(BSP_VOID)
{
    IPF_CONFIG_ULPARAM_S stUlPara[IPF_ULBD_DESC_SIZE];	
    BSP_S32 s32Ret;
    /*调用ConfigUpFilter*/
    s32Ret = BSP_IPF_ConfigUpFilter(5, stUlPara);

    if(s32Ret != BSP_ERR_IPF_CCORE_RESETTING)
    {
        IPF_PRINT(" TESTERROR    BSP_IPF_ConfigUpFilter :%d s32Ret = %x\n",__LINE__, s32Ret);
    }
    else
    {
        IPF_PRINT(" SUCCESS \n");
    }

}

void print_ad_status(void)
{

	u32 current_ad_wptr,current_ad_rptr,current_ad_offset,current_ad_state;

	/*打印下行AD读写指针位置，确定预读状态*/
	current_ad_wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);
	current_ad_rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
	current_ad_state = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_STAT_OFFSET);
	if(current_ad_state & IPF_ADQ_BUF_EPT_MASK)
	{
		current_ad_offset = 0; 
	}
	else if(current_ad_state & IPF_ADQ_BUF_FULL_MASK)
	{
		current_ad_offset = 2; 
	}
	else
	{
		current_ad_offset = 1;    
	}

	IPF_PRINT(" ADQ0	current_ad_wptr =%u	current_ad_rptr = %u	current_ad_offset = %u \n",current_ad_wptr, current_ad_rptr, current_ad_offset);

	current_ad_wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);
	current_ad_rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
	current_ad_state = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_STAT_OFFSET);
	if(current_ad_state & IPF_ADQ_BUF_EPT_MASK)
	{
		current_ad_offset = 0; 
	}
	else if(current_ad_state & IPF_ADQ_BUF_FULL_MASK)
	{
		current_ad_offset = 2; 
	}
	else
	{
		current_ad_offset = 1;    
	}

	IPF_PRINT(" ADQ1	current_ad_wptr =%u	current_ad_rptr = %u	current_ad_offset = %u \n",current_ad_wptr, current_ad_rptr, current_ad_offset);

}

/*ipf数传复位回调测试，
检测内存使用情况，查看能否正常释放
在用例中反复分配和释放内存，
注意覆场景(回退个数，回退到0之前)
用例使用开关AD的方式调整AD读写指针*/
	IPF_AD_DESC_S ad0_buffer[IPF_DLAD0_DESC_SIZE];
	IPF_AD_DESC_S ad1_buffer[IPF_DLAD1_DESC_SIZE];
	IPF_AD_DESC_S ad_result_buffer[IPF_DLAD0_DESC_SIZE];
s32 BSP_IPF_ST_502(u32 target_ad_wptr, u32 target_ad_rptr, u32 ad_len)
{

	u32 u32ADCtrl[IPF_CHANNEL_MAX] = {0,0};
	u32 i;
	s32 ret;
	u32 ad_num=0;
	

	for(i=0; i<IPF_DLAD0_DESC_SIZE; i++)
	{
		ad0_buffer[i].u32OutPtr0= i+0xf0000;
		ad0_buffer[i].u32OutPtr1= i*2+0xf0000;
	}
	for(i=0; i<IPF_DLAD0_DESC_SIZE; i++)
	{
		ad1_buffer[i].u32OutPtr0 = i+0xff0000;
		ad1_buffer[i].u32OutPtr1 = i*2+0xff0000;
	}

	/*禁止下行AD，即将下行通道调整为v7r1模式，清空ad队列*/
	u32ADCtrl[IPF_CHANNEL_DOWN] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);
	u32ADCtrl[IPF_CHANNEL_DOWN] &= IPF_ADQ_EN_MASK;
	u32ADCtrl[IPF_CHANNEL_DOWN] |= (IPF_NO_ADQ);

	/*关闭ADQ通道，用于防止产生ADQ预取*/
	ipf_writel(u32ADCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);

	/*清空ad队列*/
	memset((void *)IPF_DLAD0_MEM_ADDR, 0xFFFFFFFF, IPF_DLAD0_MEM_SIZE);
	memset((void *)IPF_DLAD1_MEM_ADDR, 0xFFFFFFFF, IPF_DLAD1_MEM_SIZE);

	/*reset unreset ipf*/
	ipf_writel(1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_OFFSET); 

	ipf_writel(0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_OFFSET); 

	
	/*打印当前AD队列情况*/
	print_ad_status();
	
	/*将AD读写指针配置为特定值*/
	ipf_writel(target_ad_wptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);
	ipf_writel(target_ad_rptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
	ipf_writel(target_ad_wptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);
	ipf_writel(target_ad_rptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
	
	/*允许下行AD*/
	u32ADCtrl[IPF_CHANNEL_UP] |= (IPF_BOTH_ADQ_EN);
	u32ADCtrl[IPF_CHANNEL_DOWN] |= (IPF_BOTH_ADQ_EN);
	ipf_writel(u32ADCtrl[IPF_CHANNEL_UP], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ_CTRL_OFFSET);
	ipf_writel(u32ADCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);
	

	/*通过configAD给代码配入特定个数个AD，每个ad填入特定值，记录填入的ad数*/
	ret = BSP_IPF_ConfigDlAd(IPF_AD_0, ad_len, ad0_buffer);
	if(IPF_SUCCESS != ret)
	{
		IPF_PRINT("BSP_IPF_ConfigDlAd0 fail \n");
		return IPF_ERROR;
	}

	ret = BSP_IPF_ConfigDlAd(IPF_AD_1, ad_len, ad1_buffer);
	if(IPF_SUCCESS != ret)
	{
		IPF_PRINT("BSP_IPF_ConfigDlAd1 fail \n");
		return IPF_ERROR;
	}

	/*打印下行AD读写指针位置，确定预读状态*/
	print_ad_status();
	
	/*调用底软接口获取需要释放的ad*/
	ret = BSP_IPF_GetUsedDlAd(IPF_AD_0, &ad_num, ad_result_buffer);
	if(IPF_SUCCESS != ret)
	{
		IPF_PRINT("BSP_IPF_GetUsedDlAd0 fail \n");
		return IPF_ERROR;
	}

	/*与configad配给ipf的ad进行比较，查看是否正确*/

	if(ad_num != ad_len)
	{
		IPF_PRINT("AD0 num get unequal to AD num set. \n ad_get_num is %u; ad_set_num is %u; \n", ad_num, ad_len);
	}
	for(i=0; i<ad_num; i++)
	{
		if((ad_result_buffer[i].u32OutPtr0 != ad0_buffer[i].u32OutPtr0)
			||(ad_result_buffer[i].u32OutPtr1 != ad0_buffer[i].u32OutPtr1))
		{
			IPF_PRINT("ad0_buffer[%u].u32OutPtr0 = %u; ad_result_buffer[%u].u32OutPtr0 = %u \n",i,ad0_buffer[i].u32OutPtr0,i,ad_result_buffer[i].u32OutPtr0);
			IPF_PRINT("ad0_buffer[%u].u32OutPtr1 = %u; ad_result_buffer[%u].u32OutPtr1 = %u \n",i,ad0_buffer[i].u32OutPtr1,i,ad_result_buffer[i].u32OutPtr1);
		}
	}

	/*调用底软接口获取需要释放的ad*/
	ret = BSP_IPF_GetUsedDlAd(IPF_AD_1, &ad_num, ad_result_buffer);
	if(IPF_SUCCESS != ret)
	{
		IPF_PRINT("BSP_IPF_GetUsedDlAd1 fail \n");
		return IPF_ERROR;
	}
	
	/*与configad配给ipf的ad进行比较，查看是否正确*/

	if(ad_num != ad_len)
	{
		IPF_PRINT("AD1 num get unequal to AD num set. \n ad_get_num is %u; ad_set_num is %u; \n", ad_num, ad_len);
	}
	for(i=0; i<ad_num; i++)
	{
		if((ad_result_buffer[i].u32OutPtr0 != ad1_buffer[i].u32OutPtr0)
			||(ad_result_buffer[i].u32OutPtr1 != ad1_buffer[i].u32OutPtr1))
		{
			IPF_PRINT("ad1_buffer[%u].u32OutPtr0 = %u; ad_result_buffer[%u].u32OutPtr0 = %u \n",i,ad1_buffer[i].u32OutPtr0,i,ad_result_buffer[i].u32OutPtr0);
			IPF_PRINT("ad1_buffer[%u].u32OutPtr1 = %u; ad_result_buffer[%u].u32OutPtr1 = %u \n",i,ad1_buffer[i].u32OutPtr1,i,ad_result_buffer[i].u32OutPtr1);
		}
	}
	return IPF_SUCCESS;
}

/*ipf数传复位回调测试，
模拟复位全流程，本函数停止数传
reset、unreset ipf
调用Acore ipf接口回调重新初始化acore ipf
*/
BSP_VOID BSP_IPF_ST_503(BSP_VOID)
{
	BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_FORBID);
	/*省略了释放内存的步骤是因为
	1.相关函数已经在case 502中充分测试过了
	2.释放内存是为了防止内存泄露，用例中不用考虑这种场景*/

	/*reset unreset ipf*/
	ipf_writel(1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_OFFSET); 

	ipf_writel(0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_OFFSET); 

	
	/*run ipf acore reg reinit*/
	BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_ALLOW);

	BSP_IPF_DlRegReInit();


	bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r run ipf_init on ccore shell then run testcase 321 and 121\n");
}

EXPORT_SYMBOL(BSP_IPF_AINIT);
EXPORT_SYMBOL(BSP_IPF_ST_001);
EXPORT_SYMBOL(BSP_IPF_ST_002);
EXPORT_SYMBOL(BSP_IPF_ST_101);
EXPORT_SYMBOL(BSP_IPF_ST_102);
EXPORT_SYMBOL(BSP_IPF_ST_102sn);
EXPORT_SYMBOL(BSP_IPF_ST_103);
EXPORT_SYMBOL(BSP_IPF_ST_301_INIT);
EXPORT_SYMBOL(BSP_IPF_ST_302_INIT);
EXPORT_SYMBOL(BSP_IPF_ST_304_INIT);

#ifdef CONFIG_MODULE_BUSSTRESS
EXPORT_SYMBOL(ipf_ul_stress_test_start);
EXPORT_SYMBOL(ipf_dl_stress_test_start);
EXPORT_SYMBOL(ipf_dl_stress_test_init);
#endif
/*module_param(g_ipftestdebug, int, 0);*/
MODULE_AUTHOR("luting00168886");
MODULE_DESCRIPTION("IPF TEST MODULE");
MODULE_LICENSE("GPL");
#endif





#ifdef __cplusplus
}
#endif


