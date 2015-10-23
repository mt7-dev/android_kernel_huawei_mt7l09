/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_VERSION_H__
#define __DRV_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_comm.h"

/*************************VERSION START*****************************************
规则:
硬件版本号由32bit表示，其中从高到低，第一个字节
表示是否为UDP，若为0表示为非UDP，0表示为UDP。后面
三个字节分别由HKADC读出，非UDP单板第二个字节表示
产品类型(0:E5,1:stick,2:Phone,3:pad,4:cpe,5:mod,6~9:resv),
后面两个字节为大、小版本号，UDPP单板的
低三个字节用于标识射频扣板版本号。

注意:
V9R1中，硬件版本号为int类型，为适配，将最高bit位设置为0;
Porting板\P531单板\SFT 无HKADC版本号为软件设定
********************************************************************************/

typedef enum tagProductType
{
	PRODUCT_TYPE_STICK = 0,
	PRODUCT_TYPE_MOD,
	PRODUCT_TYPE_E5,
	PRODUCT_TYPE_CPE,
	PRODUCT_TYPE_PAD,
	PRODUCT_TYPE_PHONE
}eProductType;

#define HW_VER_PRODUCT_TYPE_MASK	(BSP_U32)0x00FF0000
#define HW_VER_PRODUCT_TYPE_OS		16

/* 硬件产品信息适配 */
#define HW_VER_INVALID              (BSP_U32)0X7FFFFFFF

/* Porting板*/
#define HW_VER_PRODUCT_PORTING      (BSP_U32)0X00FFFFFE

/*P531 fpga单板*/
#define HW_VER_PRODUCT_P531_FPGA	(BSP_U32)0X00FFFFFD

/* SFT */
#define HW_VER_PRODUCT_SFT	        (BSP_U32)0x00FFFFFC

/*p531 asic va*/
#define HW_VER_PRODUCT_P531_ASIC_VA (BSP_U32)0X00FFFFFB

/*p531 asic vc*/
#define HW_VER_PRODUCT_P531_ASIC_VC (BSP_U32)0X00FFFFFA

/*DCM E5*/
#define HW_VER_PRODUCT_E5379		(BSP_U32)0X00000001

/*测试板*/
#define HW_VER_PRODUCT_UDP			(BSP_U32)0X7FFFFFFB
#define HW_VER_UDP_MASK				(BSP_U32)0X7F000000
#define HW_VER_UN_UDP_MASK			(BSP_U32)0X00FFFFFF

/* RF 扣板 */
#define HW_VER_PRODUCT_UDP_RF_HI6361EVB5_VER_D_RF_T	(BSP_U32)0X7F000100/*编号为1j, wangjiaan 提供*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF6_VB			(BSP_U32)0X7F000908/*编号无,wangjiaan 提供*/
#define HW_VER_PRODUCT_UDP_RF_HI4511RFU1_VER_A		(BSP_U32)0X7F000907/*K3V3RFU1 ,yuanshutian*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF4_VB			(BSP_U32)0X7F000906/*编号为2c, wangjiaan 提供*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF5_VA_RF1		(BSP_U32)0X7F000905/*编号无,wangjiaan 提供*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF7B			(BSP_U32)0X7F000608/*yueminjie 提供68*/
#define HW_VER_PRODUCT_UDP_RF_K3V3RFU2_TI_VER_A		(BSP_U32)0X7F000605/*K3V3RFU2_VA(TI),yuanshutian*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF7			(BSP_U32)0X7F000604/*编号为2m,wangjiaan 提供58*/
#define HW_VER_PRODUCT_UDP_RF_K3V3RFU2_RFMD_VER_A	(BSP_U32)0X7F000602/*K3V3RFU2_VA(RFMD),yuanshutian*/
#define HW_VER_PRODUCT_UDP_RF_HL1EMULTR_VER_A		(BSP_U32)0X7F000507/*编号无,yuanshutian 提供*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF5_VA_RF		(BSP_U32)0X7F000505/*编号无,wangjiaan 提供*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2RF5_VA_RF2		(BSP_U32)0X7F000504/*编号无,wangjiaan 提供*/
#define HW_VER_PRODUCT_UDP_RF_HIBV7R2DCM2_VB		(BSP_U32)0X7F000501/*编号为2e, wangjiaan 提供61*/
#define HW_VER_PRODUCT_UDP_RF_HI6361SEVB5_VER_D_RF_T		(BSP_U32)0X7F000702/*使用EVB5扣板通过改电阻编号为72, 刘道明给出*/

/*K3V3_FPGA*/
#define HW_VER_K3V3_FPGA			(BSP_U32)0X33333333
#define HW_VER_K3V3_FPGA_MASK		(BSP_U32)0X33000000
#define HW_VER_K3V3_FPGA_UN_MASK	(BSP_U32)0X00FFFFFF

/*K3V3 UDP*/
#define HW_VER_K3V3_UDP				(BSP_U32)0X3FFFFFFB
#define HW_VER_K3V3_UDP_MASK        (BSP_U32)0X3F000000
#define HW_VER_K3V3_UN_UDP_MASK     (BSP_U32)0X00FFFFFF

/*V711 UDP*/
#define HW_VER_V711_UDP				(BSP_U32)0X71FFFFFB
#define HW_VER_V711_UDP_MASK        (BSP_U32)0X71000000
#define HW_VER_V711_UN_UDP_MASK     (BSP_U32)0X00FFFFFF

/*K3V3 RF 扣板 */
#define HW_VER_K3V3_UDP_RF_HI6361EVB5_VER_D_RF_T	(BSP_U32)0X3F000100/*编号为1j, wangjiaan 提供*/
#define HW_VER_K3V3_UDP_RF_HI6361EVB5_VER_D_RF_T_B	(BSP_U32)0X3F000606/*编号为1j, wangjiaan 提供*/
#define HW_VER_K3V3_RF_HIBV7R2RF7					(BSP_U32)0X3F000604/*编号为2m,wangjiaan 提供58*/
#define HW_VER_K3V3__UDP_RF_K3V3RFU2_RFMD_VER_A		(BSP_U32)0X3F000602/*K3V3RFU2_VA(RFMD),yuanshutian*/


/*VERSIONINFO_I数据结构中版本字符串最大有效字符长度*/
#ifndef VER_MAX_LENGTH
#define VER_MAX_LENGTH                  30
#endif

/*memVersionCtrl接口操作类型*/
#define VERIONREADMODE                  0
#define VERIONWRITEMODE                 1

#define BUILD_DATE_LEN  12
#define BUILD_TIME_LEN  12

/*组件类型*/
typedef enum
{
    VER_BOOTLOAD = 0,
    VER_BOOTROM =1,
    VER_NV =2 ,
    VER_VXWORKS =3,
    VER_DSP =4 ,
    VER_PRODUCT_ID =5 ,
    VER_WBBP =6 ,
    VER_PS =7,
    VER_OAM =8,
    VER_GBBP =9 ,
    VER_SOC =10,
    VER_HARDWARE =11,
    VER_SOFTWARE =12,
    VER_MEDIA =13,
    VER_APP =14,
    VER_ASIC =15,
    VER_RF =16,
    VER_PMU =17,
    VER_PDM = 18,
    VER_PRODUCT_INNER_ID = 19,
    VER_INFO_NUM =20
}COMP_TYPE_I;

typedef enum{
     BOARD_TYPE_LTE_ONLY    = 0,
     BOARD_TYPE_GUL
}BOARD_TYPE_E;

typedef enum{
     PV500_CHIP             = 0,
     V7R1_CHIP              = 1,
     PV500_PILOT_CHIP,
     V7R1_PILOT_CHIP,
     V7R2_CHIP				=5
}BSP_CHIP_TYPE_E;

typedef struct  tagUE_SW_BUILD_VER_INFO_STRU
{
    BSP_U16 ulVVerNo;        /* V部分 */
    BSP_U16 ulRVerNo;        /*  R部分*/
    BSP_U16 ulCVerNo;        /* C部分*/
    BSP_U16 ulBVerNo;         /* B部分*/
    BSP_U16 ulSpcNo;          /* SPC部分*/
    BSP_U16 ulCustomVer;     /* 客户需定制部分, 针对硬件接口低8bit PCB号,高8bitHW号*/
    BSP_U32 ulProductNo;     /* such as porting,CPE, ... */
    BSP_S8 acBuildDate[BUILD_DATE_LEN];  /* build日期,*/
    BSP_S8 acBuildTime[BUILD_TIME_LEN];  /* build时间*/
} UE_SW_BUILD_VER_INFO_STRU; /* 内部版本*/

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* 最大版本长度 30 字符+ \0 */
}VERSIONINFO_I;


extern int bsp_version_get_hwversion_index(void);
/*****************************************************************************
* 函 数 名  : BSP_HwGetVerMain
* 功能描述  : 获取产品版本号
* 输入参数  :
* 输出参数  :
* 返 回 值  :
* 修改记录  :
*****************************************************************************/
#define BSP_HwGetVerMain() bsp_version_get_hwversion_index()

#define DRV_GET_HW_VERSION_INDEX()       bsp_version_get_hwversion_index()

/*****************************************************************************
* 函 数 名  : BSP_GetProductName
* 功能描述  : 获取产品名称
* 输入参数  : char* pProductName,字符串指针，保证不小于32字节
*             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
* 返 回 值  : 0：正确，非0: 失败
* 修改记录  :
*****************************************************************************/
extern BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);


/*****************************************************************************
 函 数 名  : DRV_GET_PLATFORM_INFO
 功能描述  : 获取芯片的版本号
 输入参数  : 无
 输出参数  : u32PlatformInfo:芯片的版本号
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID DRV_GET_PLATFORM_INFO(unsigned long *u32PlatformInfo);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetChipType
*
* 功能描述  : 获取芯片类型
*
* 输入参数  : BSP_VOID
*
* 输出参数  : 无
*
* 返 回 值  : 芯片类型
*             PV500_CHIP:PV500芯片
*             V7R1_CHIP: V7R1芯片
*
* 其它说明  : 无
*
*****************************************************************************/
extern BSP_CHIP_TYPE_E BSP_OM_GetChipType(BSP_VOID);

/*****************************************************************************
 函 数 名  : BSP_InitPlatformVerInfo
 功能描述  : 初始化芯片的版本号
 输入参数  : 无
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID BSP_InitPlatformVerInfo(BSP_VOID);
#define DRV_INIT_PLATFORM_VER()	BSP_InitPlatformVerInfo()
/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardType
*
* 功能描述  : 获取单板类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
extern BOARD_TYPE_E bsp_get_board_mode_type(void);
#define BSP_OM_GetBoardType() 	bsp_get_board_mode_type()

/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 返回当前产品类型
 输入参数   : 无
 输出参数   : 无
 返 回 值   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
extern BSP_U32 BSP_MNTN_ProductTypeGet(void);
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()

/*****************************************************************************
* 函 数 名  : BSP_GetBuildVersion
*
* 功能描述  :
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
extern UE_SW_BUILD_VER_INFO_STRU* BSP_GetBuildVersion(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_HwGetHwVersion
* 功能描述  : 获取硬件版本名称
* 输入参数  : BSP_CHAR* pHwVersion,字符串指针，保证不小于32字节
* 输出参数  : 无
* 返 回 值  : 无
* 修改记录  :
*****************************************************************************/
extern BSP_S32 BSP_HwGetHwVersion (char* pFullHwVersion, BSP_U32 ulLength);

/*****************************************************************************
 函 数 名  : BSP_GU_GetVerTime
 功能描述  : 获取版本编译时间
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
#define DRV_GET_VERSION_TIME()    bsp_version_get_build_date_time()

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* 最大版本长度 30 字符+ \0 */
}VERSIONINFO;

/*****************************************************************************
 函 数 名  : BSP_MNTN_MemVersionCtrl
 功能描述  : 组件版本读写接口。
 输入参数  : pcData：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存地址；
                     当Mode为写的时候，为调用者准备写入的组件版本信息的地址。
             ucLength：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存大小；
                       当Mode为写的时候，为调用者准备写入的组件版本信息的字符数（不包括'\0'）。
             ucType：版本信息ID，
             ucMode：0：读取指定ID的组件版本信息；1：写入指定ID的组件版本信息。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int bsp_version_memversion_ctrl(char *pcData, unsigned char ucLength, COMP_TYPE_I ucType, unsigned char ucMode);
#define DRV_MEM_VERCTRL(pcData,ucLength,ucType,ucMode)  \
                          bsp_version_memversion_ctrl(pcData,ucLength,ucType,ucMode)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFullHwVersion
 功能描述  : 硬件完整版本读接口。
 输入参数  : pFullHwVersion：为调用者待保存返回的硬件完整版本信息的内存首地址；
             ulLength      ：为调用者待保存返回的硬件完整版本信息的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
#define  DRV_GET_FULL_HW_VER(pFullHwVersion,ulLength)  \
               BSP_HwGetHwVersion(pFullHwVersion,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetProductIdInter
 功能描述  : 产品名称完整版本读接口。
 输入参数  : pProductIdInter：为调用者待保存返回的产品名称完整版本的内存首地址；
             ulLength       ：为调用者待保存返回的产品名称完整版本的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/

extern int BSP_MNTN_GetProductIdInter(char * pProductIdInter, unsigned int ulLength);
#define  DRV_GET_PRODUCTID_INTER_VER(pProductIdInter,ulLength)  \
              BSP_MNTN_GetProductIdInter(pProductIdInter,ulLength)

/*****************************************************************************
* 函 数 名  : BSP_GetProductInnerName
* 功能描述  : 获取内部产品名称
* 输入参数  : char* pProductIdInter,    字符串指针
*             BSP_U32 ulLength,         缓冲区长度
* 输出参数  : 无
* 返 回 值  : 0：正确，非0: 失败
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_GetProductInnerName (char * pProductIdInter, unsigned int ulLength);

/*****************************************************************************
 函 数 名  : BSP_MNTN_VersionQueryApi
 功能描述  : 查询所有组件的版本号。
 输入参数  : ppVersionInfo：待保存的版本信息地址。
 输出参数  : ucLength：待返回的数据的字节数。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int bsp_version_get_verall(void ** ppVersionInfo, unsigned int * ulLength);
#define  DRV_VER_QUERY(ppVersionInfo,ulLength)  \
              bsp_version_get_verall (ppVersionInfo,ulLength)

/*****************************************************************************
* 函 数 名  : bsp_get_firmware_version
*
* 功能描述  : 获取软件版本号
*
* 输入参数  : 无
* 输出参数  :
*
 返 回 值  : 软件版本号字符串指针
* 修改记录  :
*
*****************************************************************************/
char * bsp_version_get_firmware(void);

/*****************************************************************************
* 函 数 名  : bsp_get_release_version
*
* 功能描述  : 获取release_version
*
* 输入参数  : 无
* 输出参数  :
*
 返 回 值  : release_version字符串的指针
* 修改记录  :
*
*****************************************************************************/
char * bsp_version_get_release(void);

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetHwGpioInfo
 功能描述  : Get flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_MNTN_GetHwGpioInfo(unsigned char *pGpioInfo, unsigned long usLength );
#define DRV_GET_GPIO_INFO(pGpioInfo, usLength)    BSP_MNTN_GetHwGpioInfo(pGpioInfo, usLength )

/*************************VERSION END  *****************************/

#ifdef __cplusplus
}
#endif

#endif

