/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  secBoot.c
*
*   作    者 :  wuzechun
*
*   描    述 :  安全启动的总流程控制
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "sys.h"
#include "string.h"
#include "secBoot.h"
#include "OnChipRom.h"
#include "efuse.h"
#include "emmcMain.h"
#include "hsuart.h"
#include "apDl2.h"
#include "spiDev.h"
#include "ioinit.h"
#include "md5.h"
#include "hi_base.h"
#include "hi_efuse.h"
#include "hi_nandc.h"
#include "nand.h"

extern void usb3_driver_init(void);

/*****************************************************************************
* 函 数 名  : go
*
* 功能描述  : 跳转函数
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void go(FUNCPTR entry)
{
    (entry) ();
}

/*****************************************************************************
* 函 数 名  : usbBoot
*
* 功能描述  : usb自举
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void usbBoot()
{
    print_info("\r\nUSB_boot!" );

    /* 软标志有效或自举管脚为低电平，进入自举流程*/
    usb3_driver_init();

    /* USB自举退出,复位系统,重新尝试 */
    print_info("\r\nUSB_boot returns, reboot..." );

    setErrno(SYS_ERR_USB_BOOT_RETURNS);

    wdtRebootDelayMs(TIME_DELAY_MS_1000_FOR_UBOOT_RETURN);
}
/*****************************************************************************
* 函 数 名  : hsicBoot
*
* 功能描述  : hsic自举
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
#if HSIC_CONFIG==YES
extern void hsic_works(void);
void hsicBoot()
{
    /* 软标志有效或自举管脚为低电平，进入自举流程*/
    hsic_works();

    /* USB自举退出,复位系统,重新尝试 */
    print_info("\r\nHSIC_boot returns, reboot..." );

    setErrno(SYS_ERR_HSIC_BOOT_RETURNS);

    wdtRebootDelayMs(TIME_DELAY_MS_1000_FOR_UBOOT_RETURN);
}
#endif
/*****************************************************************************
* 函 数 名  : idioIdentify
*
* 功能描述  : 通过校验输入数据的hash值是 否
*             与其签名用RSA解密后的值一样
*             来校验输入数据的合法性 *
*
* 输入参数  : dataAddr--输入数据的地址
*             dataLen--输入数据的长度
*             pubKey--公钥
*             pIdio--输入数据签名的指针
* 输出参数  :
*
* 返 回 值  : OK/ !OK
*
* 其它说明  :
*
*****************************************************************************/
int idioIdentify(UINT32 dataAddr, UINT32 dataLen, KEY_STRUCT *pubKey, UINT32* pIdio)
{
    UINT32 sha256Hash[SHA256_HASH_SIZE];      /*用来存放SHA256值的临时buffer*/
    UINT32 rsa[IDIO_LEN/4];                   /*用来存放RSA加密值的临时buffer*/
    int i;

    memset(sha256Hash, 0, SHA256_HASH_SIZE*4);
    memset(rsa, 0, IDIO_LEN);

    /*通过SHA256计算输入数据(data)的hash值*/
    if(OK != SHA256Hash(dataAddr, dataLen, sha256Hash))
    {
        print_info("\r\nhash calc err!" );
        return !OK;
    }

    /*使用公钥(pubKey),通过RSA解密出输入签名(dataIdioAddr)的明文*/
    if(OK != RSA(pubKey, pIdio, rsa))
    {
        print_info("\r\nrsa calc err!" );
        return !OK;
    }

    /*比较二者是否相同*/
    for(i = 0; i < SHA256_HASH_SIZE; i++)
    {
        if( sha256Hash[i] != rsa[i] )
        {
            return !OK;
        }
    }

    return OK;
}
/*****************************************************************************
* 函 数 名  : timerInit
*
* 功能描述  : Timer Init
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void timerInit()
{
    volatile UINT32 value;
	/* timer disable */
	writel(0x1, TIMER0_REGOFF_CLEAR);
	value = readl(TIMER0_REGOFF_CTRL);
	writel(value&(~0x80), TIMER0_REGOFF_CTRL);

	writel(0x1, TIMER2_REGOFF_CLEAR);
	value = readl(TIMER2_REGOFF_CTRL);
	writel(value&(~0x80), TIMER2_REGOFF_CTRL);

	
    /* select clock frequency to 19.2M  */
	writel(0x280, TIMER_DISABLE_ADRR);
	writel(readl(TIMER_CLK_SEL) | 0x5, TIMER_CLK_SEL);
	writel(0x280, TIMER_ENABLE_ADRR);


#ifdef START_TIME_TEST
    /* disable interrupt */
    value = INREG32(TIMER5_REGOFF_CTRL);
    value |= TIMER_CTRL_INTMASK;
    OUTREG32(TIMER5_REGOFF_CTRL, value);

    /* set load count */
    OUTREG32(TIMER5_REGOFF_LOAD, TIMER5_INIT_VALUE);

    value = INREG32(CRG_CLK_SEL2);
    value |= (1 << CRG_CLK_SEL2_TIMER5);
    OUTREG32(CRG_CLK_SEL2, value);

    do
    {
        value = INREG32(TIMER5_REGOFF_CTRL);
        value &= TIMER_CTRL_EN_ACK;
    }while(value != TIMER_CTRL_EN_ACK);

    /* enable interrupt */
    value = INREG32(TIMER5_REGOFF_CTRL);
    value |= TIMER_CTRL_EN;
    OUTREG32(TIMER5_REGOFF_CTRL, value);
#endif
}


/*****************************************************************************
* 函 数 名  : secCheck
*
* 功能描述  : 安全校验总入口
*
* 输入参数  : ulImageAddr--映像地址,必须字对齐
*                  eImageTye--镜像类型
* 输出参数  :
*
* 返 回 值  : SEC_IMAGE_LEN_ERROR   镜像长度过长
*             SEC_EFUSE_READ_ERROR   Efuse读取错误
*             SEC_EFUSE_NOT_WRITE    Efuse未烧写
*             SEC_SHA_CALC_ERROR     SHA计算错误
*             SEC_ROOT_CA_ERROR      根CA校验错误
*             SEC_OEMCA_ERROR        OEM CA校验错误
*             SEC_IMAGE_ERROR        镜像签名校验错误
*
* 其它说明  :
*
*****************************************************************************/
int secCheck(UINT32 ulImageAddr, IMAGE_TYPE_E eImageTye)
{
    int i;
    UINT32 blLen = 0;
    KEY_STRUCT *pstKeyRoot;
    KEY_STRUCT *pstKeyBl;                       /*映像的公钥*/
    UINT32 sha256HashCa[SHA256_HASH_SIZE];      /*用来存放SHA256值的临时buffer*/
    UINT32 md5HashEfuse[MD5_HASH_SIZE];      /*用来存放SHA256值的MD5值的临时buffer*/
    UINT32 md5HashCa[MD5_HASH_SIZE];            /*用来存放SHA256值的MD5值的临时buffer*/

    UINT32 imageIdioAddr;                   /*镜像的签名的地址*/
    UINT32 imageCaAddr;                     /*映像的CA的地址*/
    UINT32 imageCaIdioAddr;                 /*映像的CA签名的地址*/

    MD5_CTX md5_contex;

    volatile tOcrShareData *pShareData = (tOcrShareData*)M3_SRAM_SHARE_DATA_ADDR;

     /*获取bootload.bin文件的长度(字节数)*/
     if(IMAGE_TYPE_DOWNLOAD != eImageTye)
     {
        blLen = *(volatile UINT32 *)(ulImageAddr+BL_LEN_INDEX);
     }
     else
     {
        blLen = *(volatile UINT32 *)(ulImageAddr+DOWNLOAD_BL_LEN_INDEX);
     }

    /* 判断长度是否合法:不为零/字对齐/不翻转/不过大 */
    if((0 == blLen)
        || (blLen % 4)
        || (blLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN < blLen)
        || ((IMAGE_TYPE_DOWNLOAD != eImageTye) /* 若为USB自举版本，无需校验版本长度 */
            && (blLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN > BOOTLOAD_SIZE_MAX)))
    {
    	print_info_with_u32("\r\nSec image len err: 0x", blLen);
        return SEC_IMAGE_LEN_ERROR;
    }

    /* 获取镜像签名的位置 */
    imageIdioAddr = ulImageAddr + blLen;

    /* 获取OEM CA的位置 */
    imageCaAddr = imageIdioAddr + IDIO_LEN;

    /* 获取OEM CA签名的位置 */
    imageCaIdioAddr = imageCaAddr + OEM_CA_LEN;

    /* 根CA已经在RAM初始化程序中校验通过，加载程序不需再校验根CA */
    if(IMAGE_TYPE_DOWNLOAD != eImageTye)
    {
        /* 读取Efuse中的根CA的Hash值 */
        if(OK != efuseRead(EFUSE_GRP_ROOT_CA, md5HashEfuse, MD5_HASH_SIZE))
        {
            print_info("\r\nEfuse read err!");
            return SEC_EFUSE_READ_ERROR;
        }

        /* 检查Efuse是否已烧写 */
        if(EFUSE_NOT_WRITED == efuseWriteCheck(md5HashEfuse, MD5_HASH_SIZE))
        {
            pShareData->bRootCaInited = FALSE;
            pShareData->bSecEn = FALSE;         /* Efuse未烧写,无法获得根CA不使能安全校验 */
            print_info("\r\nEfuse not write, sec will be disabled!" );
            return SEC_EFUSE_NOT_WRITE;
        }

        pstKeyRoot = (KEY_STRUCT *)(ulImageAddr + ROOT_CA_INDEX);
        /* 计算根公钥的Hash值 */
        if(OK != SHA256Hash((UINT32)pstKeyRoot, ROOT_CA_LEN, sha256HashCa))
        {
            print_info("\r\nsha256 err!");
            return SEC_SHA_CALC_ERROR;
        }

        MD5Init(&md5_contex);
        MD5Update(&md5_contex, (unsigned char *)sha256HashCa, sizeof(sha256HashCa));
        MD5Final(&md5_contex, (unsigned char*)md5HashCa);

        /* 比较根公钥的Hash值 */
        for(i=0; i<MD5_HASH_SIZE; i++)
        {
            if(md5HashEfuse[i] != md5HashCa[i])
            {
                print_info("\r\nRoot CA check err!");
                return SEC_ROOT_CA_ERROR;
            }
        }

        /* 保存根公钥，共享给BootLoader */
        pShareData->pRootKey = pstKeyRoot;
        pShareData->bRootCaInited = TRUE;

    }
    else
    {
        /* 防止未运行RAM初始化程序就直接运行USB加载程序 */
        if(!pShareData->bRootCaInited)
        {
            print_info("root ca not inited!");
            return SEC_ROOT_CA_ERROR;
        }
    }

    /*鉴权映像证书*/
    if(OK != idioIdentify(imageCaAddr, OEM_CA_LEN, pShareData->pRootKey, (UINT32*)imageCaIdioAddr))
    {
        print_info("\r\nOEM CA check err!" );
        return SEC_OEMCA_ERROR;
    }

    /*鉴权bootload映像*/
    pstKeyBl = (KEY_STRUCT *)imageCaAddr;

    if(OK != idioIdentify(ulImageAddr, blLen, pstKeyBl, (UINT32*)imageIdioAddr))
    {
        print_info("\r\nimage check err!" );
        return SEC_IMAGE_ERROR;
    }

    return SEC_SUCCESS;
}

#if PLATFORM==PLATFORM_PORTING

void testHash256()
{
    INT32 i;
    UINT32 hash[SHA256_HASH_SIZE] = { 0 };
    UINT32 data[128] = { 0 };

    for (i = 0; i < sizeof(data)/sizeof(data[0]); i++)
    {
        data[i] = i;
    }

    if(OK != SHA256Hash((UINT32)data, sizeof(data), hash))
    {
        print_info("\r\nsha256 err!");
    }
    else
    {
        for (i = 0; i < SHA256_HASH_SIZE; i++)
        {
            print_info_with_u32("\r\n 1: ", hash[i]);
        }
    }

    if(OK != SHA256Hash((UINT32)data, sizeof(data), hash))
    {
        print_info("\r\nsha256 err!");
    }
    else
    {
        for (i = 0; i < SHA256_HASH_SIZE; i++)
        {
            print_info_with_u32("\r\n 2: ", hash[i]);
        }
    }
}

void testEfuseRead()
{
    INT32 i, ret;
    UINT32 value = 0;

    for (i = 0; i < 8; i++)
    {
        ret =efuseRead(i, &value, 1);
        if (ret)
        {
            print_info_with_u32("\r\nefuse read error, ret = ", ret);
        }
        else
        {
            print_info_with_u32("\r\nvalue = ", value);
        }
    }
}

void testEfuseWrite()
{
    UINT32 value = EFUSEC_SEC_EN;
/*
这里是ROOT CA的哈希值
    UINT32 efuseValue[] =
    {
        0x73A0460D, 0xF26B85FB, 0xC0BA0AD1, 0x4A059D28,
        0x203092C0, 0xAB9095AB, 0x573F5A43, 0xE5F34A6C
    };
*/
    UINT32 efuseValue[] =
    {
        0x45003997, 0x68229d1a, 0xec67902c, 0xf4240b7a
    };

    if (efuseWrite(0, efuseValue, sizeof(efuseValue)/sizeof(efuseValue[0])))
    {
        print_info("\r\nfail to write efuse");
    }
    else
    {
        print_info("\r\nsucceed to write efuse");
    }

    /* 启用安全校验 */
    efuseWrite(EFUSE_GRP_CFG, &value, 1);
}

#endif

#ifndef ONCHIPROM_TEST
/*****************************************************************************
* 函 数 名  : secBoot
*
* 功能描述  :   安全启动C入口函数
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void secBoot(void)
{
    volatile UINT32 ulBootModeVal = 0x00;

    volatile tOcrShareData *pShareData = (tOcrShareData*)M3_SRAM_SHARE_DATA_ADDR;
    int retVal = 0x00;
    int BOOT_RST_Addr = 0x00;

#if PLATFORM==PLATFORM_PORTING

    //testEfuseRead();
    //testHash256();

#endif

    /* 初始化 */
    timerInit();

    pShareData->errno = 0;
    pShareData->SHA256Hash    = NULL;
    pShareData->RSA         = NULL;
    pShareData->idioIdentify = NULL;
    pShareData->bSecEn = FALSE;
    pShareData->bRootCaInited = FALSE;
    pShareData->pRootKey    = NULL;
    pShareData->ulOcrInitedFlag = FALSE;
	pShareData->bsp_nand_get_spec_and_save = NULL;

    /* 读取Efuse安全校验配置，获取安全标志 */
    if(EFUSEC_SEC_EN == (INREG32(EFUSEC_HW_CFG) & EFUSEC_SEC_EN_MASK))
    {
        pShareData->bSecEn = TRUE;
    }

    /*判断USB自举软件标志是否有效*/
    if((AUTO_ENUM_FLAG_VALUE == pShareData->ulEnumFlag)
        || (SC_AUTO_ENUM_EN == (INREG32(SC_STAT0) & SC_AUTO_ENUM_EN_BITMASK)))
    {
        /* 软标志有效或自举管脚为低电平，先清除标志,再进入自举流程*/
        /*pShareData->ulEnumFlag = 0;*/ /* 不清除标志,供探针程序检查自举原因 */
        usbBoot();
    }

    /*读取BOOTMODE*/
    ulBootModeVal = INREG32(SC_STAT0) & SC_BOOTMODE_BITMASK;
    switch(ulBootModeVal)
    {
        case BOOT_MODE_NAND_ID:
        {
            print_info("\r\nNF id boot!" );
            retVal = nandReadBl(M3_TCM_BL_ADDR, NAND_PARSE_ID_MODE, &(pShareData->nandspec));
            break;
        }
		case BOOT_MODE_NAND_BOOT:
		{
            print_info("\r\nNF boot only!" );
            /* Nand默认在Boot模式下,直接拷贝bootload映像 */
            retVal = nandReadBl(M3_TCM_BL_ADDR, ONLY_NAND_BOOT_MODE, &(pShareData->nandspec));
            break;
		}
		
        case BOOT_MODE_NAND_EFUSE:
		{
            print_info("\r\nNF efuse boot!" );
            retVal = nandReadBl(M3_TCM_BL_ADDR, EFUSE_BOOT_MODE, &(pShareData->nandspec));
            break;
		}
#if HSIC_CONFIG==YES
       case BOOT_MODE_AP_HSIC:
       {
            print_info("\r\nHSIC boot!" );

            /*通过HSIC将bootload映像读取*/
            hsicBoot();

            break;
        }
#endif
#if SPI_CONFIG==YES
       case BOOT_MODE_SPI:
       {
            print_info("\r\nSPI boot!" );

            /*从SPI中将bootload映像读取*/
            retVal = spiDevReadBl((UINT8*)M3_TCM_BL_ADDR);

            break;
        }
#endif
#if EMMC_CONFIG==YES
        case BOOT_MODE_EMMC:
		{
            print_info("\r\nEMMC boot!" );
            /*搬运bootload映像*/
            retVal = emmc_read_bootloader(M3_TCM_BL_ADDR);

            break;
		}
#endif        
#if HSUART_CONFIG==YES
        case BOOT_MODE_AP_HSUART:
        {
            print_info("\r\nHSUART boot!" );

            /* 不再返回 */
            retVal = apDlMain(M3_TCM_BL_ADDR);

            /* 防止apDlMain返回,这里打印错误 */
            print_info_with_u32("\r\napDlMain exec err, ret:" , (UINT32)retVal);
            setErrno(SYS_ERR_AP_DL_RETURNS);
            /* 延时复位 */
            wdtRebootDelayMs(TIME_DELAY_MS_4000_FOR_AP_RETURNS);

            break;  /*lint !e527*/
        }
#endif
        default:
        {
#if PLATFORM==PLATFORM_PORTING

            /* PORTING版本使用此模式烧写EFUSE */
            print_info_with_u32("\r\nbootmode err, will write efuse on porting..." , ulBootModeVal);
            testEfuseWrite();
            usbBoot();
            break;

#else
            /* 复位,以防止Stick形态没有串口,从复位可以得知错误 */
            print_info_with_u32("\r\nbootmode err:" , (ulBootModeVal>>SC_BOOTMODE_BITPOS));
            setErrno(SYS_ERR_BOOT_MODE);
            /* 延时复位 */
            wdtRebootDelayMs(TIME_DELAY_MS_3000_FOR_BOOT_MODE);
            break;
#endif
        }
    }

    if(OK != retVal)
    {
        print_info_with_u32("\r\nBL read err, ret:", (UINT32)retVal);
        setErrno(retVal);

        usbBoot();
    }

    /*判断FLASH /E2/MMC是否烧入映像*/
    if(BL_CHECK_INSTRUCTION != *(volatile UINT32 *)BL_CHECK_ADDR)
    {
        /*映像未烧入，向AP返回Nack, 或跳入USB自举流程*/
        print_info("\r\nimage not program!" );
        setErrno(SEC_NO_IMAGE);
        usbBoot();
    }

    ocrShareSave();

    /*非安全启动，直接运行*/
    if(!pShareData->bSecEn)
    {
        print_info("\r\nUnSec_boot!" );
        /*非安全，跳入TCM执行*/
        BOOT_RST_Addr = *(volatile UINT32 *)BOOT_RST_ADDR_PP;
        go((FUNCPTR)(M3_TCM_BL_ADDR + BOOT_RST_Addr));
    }

    /*安全启动，进行安全校验*/
    retVal = secCheck((UINT32)M3_TCM_BL_ADDR, IMAGE_TYPE_BOOTLOAD);

#ifdef START_TIME_TEST
    print_info_with_u32("\r\ntime(ms):", (TIMER5_INIT_VALUE - INREG32(TIMER5_REGOFF_VALUE))/MS_TICKS);
#endif

    switch(retVal)
    {
        case SEC_SUCCESS:/*安全校验通过*/
            print_info("\r\nSec check ok!" );  /*lint !e616*/
            /* 进入下一case运行BootLoader */
        case SEC_EFUSE_NOT_WRITE:     /*lint !e825*/ /*EFUSE 未烧写*/
            /* 跳到TCM mem执行BOOTLOAD */
            BOOT_RST_Addr = *(volatile UINT32 *)BOOT_RST_ADDR_PP;
            go((FUNCPTR)(M3_TCM_BL_ADDR + BOOT_RST_Addr));

            break;

        case SEC_SHA_CALC_ERROR:  /* Hash计算不通过 */
        case SEC_OEMCA_ERROR:  /* OEM CA校验不通过 */
        case SEC_IMAGE_ERROR:   /* 映像校验不通过 */
        case SEC_ROOT_CA_ERROR:/* 根CA校验错误 */
        case SEC_IMAGE_LEN_ERROR:/*安全版本长度错误*/
            print_info("\r\nSec check err!" );
            setErrno(retVal);
            usbBoot();

            break;

        case SEC_EFUSE_READ_ERROR:/*Efuse读取失败,使用看门狗复位,再次尝试读取*/
            print_info("\r\nEfuse read err, reboot...");
            setErrno(SYS_ERR_EFUSE_READ);
            wdtRebootDelayMs(TIME_DELAY_MS_2000_FOR_EFUSE_READERR);
            break;

        default:
            print_info_with_u32("\r\nunhandered ret:",(UINT32)retVal);
            setErrno(SYS_ERR_SEC_UNKNOWN_RET);
            /* 延时1s */
            wdtRebootDelayMs(TIME_DELAY_MS_5000_FOR_SEC_UNKNOWN_RET);
            /*break;     */  /*for pc lint*/

    }

    /* 异常情形设置错误码 */
    setErrno(retVal);

}
#else
void secBoot(void)
{
    print_info("\r\nSEC test...");
}
#endif

