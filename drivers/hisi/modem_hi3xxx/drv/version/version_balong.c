
/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  product_version_info.c
*
*   作    者 :  zuofenghua
*
*   描    述 :  用于维护硬件版本号相关信息
*
*   修改记录 :  2013年2月23日  v1.00  zuofenghua  创建
*
*************************************************************************/
/*lint --e{537}*/
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#endif

#if defined(__OS_VXWORKS__)
#include <string.h>
#endif

#include <product_config.h>
#include <hi_syssc_interface.h>
#include <osl_types.h>
#include <bsp_sram.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <comm_nv_def.h>
#include <bsp_version.h>
#include <bsp_nvim.h>

/*
*--------------------------------------------------------------*
* 宏定义                                                       *
*--------------------------------------------------------------*
*/

/*全局变量，保存产品信息,version 初始化配置nv值*/
PRODUCT_INFO_NV_STRU huawei_product_info =
{
	0
};
/*lint -save -e958*/
static __inline__ int bsp_version_default_set(void)
{
    u32 iret;
	u32 hw_ver;/*硬件版本号*/

	hw_ver=*(u32 *)HW_VERSION_ADDR;/*get hardware version*/

    /*read nv,get the exc protect setting*/
    /*lint -save -e26 -e119*/
    iret = bsp_nvm_read(NV_ID_DRV_NV_VERSION_INIT,(u8*)&huawei_product_info,sizeof(PRODUCT_INFO_NV_STRU));
	/*lint -restore*/

    if(NV_OK != iret)
    {
        ver_print_error("version init read nv error,not set,use the default config!\n");
        return VER_ERROR;
    }

	/*未查找到hwVer*/
	if(hw_ver!=huawei_product_info.index){
		ver_print_error("product info is not defined, pls check product_info_tbl!HKADC read:0x%x,nv read:0x%x\n",hw_ver,huawei_product_info.index);
		huawei_product_info.index = hw_ver;
		return VER_ERROR;
	}

	return VER_OK;

}
int bsp_productinfo_init(void)
{
	int iret=0;

	/*TODO:后续如果有子版本号，需要适配*/

    /*清除局部结构变量的内存，即初始化为全0*/
    memset((void *)(&huawei_product_info), 0, sizeof(PRODUCT_INFO_NV_STRU));
	iret = bsp_version_default_set();
	if(VER_ERROR==iret){
		ver_print_error("bsp version version_default_set fail!\n");
	}

	ver_print_error("bsp version init OK!\n");
	return iret;
}


char * bsp_version_get_hardware(void)
{
	unsigned int len;
	static bool b_geted=false;
	static char hardware_version[VERSION_MAX_LEN];
	char hardware_sub_ver = 0;

	/*单板为硬核，软核，或者UDP*/
	if((HW_VER_INVALID == huawei_product_info.index))
	{
		ver_print_error("init error\n");
    	return NULL;
	}

	if(!b_geted){
        /*lint -save -e18 -e718 -e746*/
		len = (unsigned int)(strlen(huawei_product_info.hwVer) + strlen(" Ver.X"));
        /*lint -restore*/

	    hardware_sub_ver = (char)huawei_product_info.hwIdSub+'A';

		(void)memset((void *)hardware_version, 0, len);
		/* coverity[secure_coding] */
		strncat(strncat(hardware_version, huawei_product_info.hwVer, strlen(huawei_product_info.hwVer)), " Ver.", strlen(" Ver."));
		*((hardware_version + len) - 1) = hardware_sub_ver;
		*(hardware_version + len) = 0;

		b_geted=true;
	}
	/*子版本号待确认*/

	return (char *)hardware_version;

}
char * bsp_version_get_product_inner_name(void)
{
	unsigned int len;
	static bool b_geted=false;
	static char product_inner_name[VERSION_MAX_LEN];

	if( HW_VER_INVALID == huawei_product_info.index)
	{
		return NULL;
	}

	if(!b_geted){
		len = (unsigned int)(strlen(huawei_product_info.name)+ strlen(huawei_product_info.namePlus));

		(void)memset((void*)product_inner_name, 0, len);

		/* coverity[secure_coding] */
		strncat(strncat(product_inner_name,huawei_product_info.name, strlen(huawei_product_info.name)), huawei_product_info.namePlus, strlen(huawei_product_info.namePlus));

		*(product_inner_name+len) = 0;
		b_geted=true;
	}

	return  (char*)product_inner_name;

}
char * bsp_version_get_product_out_name(void)
{
	if(HW_VER_INVALID == huawei_product_info.index)
	{
		return NULL;
	}

	return (char *)huawei_product_info.productId;
}


char * bsp_version_get_pcb(void)
{
	if(HW_VER_INVALID == huawei_product_info.index)
	{
		return NULL;
	}

	return (char *)huawei_product_info.hwVer;
}


char * bsp_version_get_dloadid(void)
{
	if(HW_VER_INVALID == huawei_product_info.index)
	{
		return NULL;
	}

	return (char *)huawei_product_info.dloadId;
}


char * bsp_version_get_build_date_time(void)
{
	static char * build_date   = __DATE__ ", " __TIME__;
	return build_date;
}


char * bsp_version_get_chip(void)
{
	return (char *)PRODUCT_CFG_CHIP_SOLUTION_NAME;
}


char * bsp_version_get_firmware(void)
{
	u32 iret = VER_OK;
	static NV_SW_VER_STRU nv_sw_ver={0};

	/*读取NV项中的软件版本号*/
    /*lint -save -e26 -e119*/
	iret=bsp_nvm_read(NV_ID_DRV_NV_DRV_VERSION_REPLACE_I, \
				(u8 *)&nv_sw_ver,
				sizeof(NV_SW_VER_STRU));
    /*lint -restore*/

    if(NV_OK != iret){/*如果获取失败，则返回默认版本号*/
		ver_print_info("get NV_SW_VERSION_REPLACE failed!\n");
		return (char*)PRODUCT_DLOAD_SOFTWARE_VER;
	}
	else{
		if(0 == nv_sw_ver.nvStatus){
			ver_print_info("nv_sw_ver.nvStatus :%d\n",nv_sw_ver.nvStatus);
			return (char*)PRODUCT_DLOAD_SOFTWARE_VER;
		}
	}

	return (char *)nv_sw_ver.nv_version_info;
}


char * bsp_version_get_hash(void)
{
	return CFG_PRODUCT_TAG_ID;
}


char * bsp_version_get_iso(void)
{
	/**TODO:  暂时先返回这个数值,后续看isover如何存储*/

	#define ISO_VER "00.000.00.000.00"
	return (char *)ISO_VER;
}
char * bsp_version_get_webui(void)
{
	/**TODO:  同isover*/
	return NULL;
}
char * bsp_version_get_release(void)
{
	/**TODO:  */
	return PRODUCT_VERSION_STR;
}
int bsp_version_get_hwversion_index(void)
{
    int hw_ver=0;
	hw_ver=*(int *)HW_VERSION_ADDR;/*get hardware version*/

	return hw_ver;
}
/*****************************************************************************
* 函数	: bsp_get_board_chip_type
* 功能	: get board type and chip type
* 输入	: void
* 输出	: void
* 返回	: BOARD_TYPE_E
*****************************************************************************/
u32 bsp_version_get_board_chip_type(void)
{
    unsigned int type = 0xffffffff;
	type = (u32)bsp_version_get_hwversion_index();

	if(HW_VER_INVALID==type){
		ver_print_error("[bsp_get_board_chip_type]:get hw version failed!hw_ver:0x%x\n",type);
		return type;
	}

	/*若为udp，只返回UDP硬件版本号，屏蔽射频扣板信息*/
	if(HW_VER_UDP_MASK==(type & HW_VER_UDP_MASK)){
		return HW_VER_PRODUCT_UDP;
	}

	/*若为k3v3UDP,只返回UDP硬件版本号*/
	if(HW_VER_K3V3_UDP_MASK == (type & HW_VER_K3V3_UDP_MASK)){
		return HW_VER_K3V3_UDP;
	}

	/*若为k3v3FPGA,只返回FPGA硬件版本号*/
	if(HW_VER_K3V3_FPGA_MASK == (type & HW_VER_K3V3_FPGA_MASK)){
		return HW_VER_K3V3_FPGA;
	}

	
	/*若为k3v3 plus,只返回UDP硬件版本号*/
	if(HW_VER_K3V3_PLUS_UDP_MASK == (type & HW_VER_K3V3_PLUS_UDP_MASK)){
		return HW_VER_K3V3_UDP;/*暂时适配，后续删除*/
	}

	if(HW_VER_V711_UDP_MASK == (type & HW_VER_V711_UDP_MASK)){
		return HW_VER_V711_UDP;
	}

	return type;
}
u32 bsp_version_get_chip_type(void)
{
	return get_hi_version_id_version_id();
}

#ifdef __KERNEL__
char* bsp_dload_get_dload_version(void)
{
	return (char *)DLOAD_VERSION;
}


int bsp_dload_get_dload_info(struct dload_info_type* dload_info)
{
	char *dload_iso = NULL;
	char *product_name = NULL;
	char *firmware = NULL;

	/*获取iso version*/
	dload_iso = bsp_version_get_iso();
	if(NULL == dload_iso){
		ver_print_error("get iso ver is null!\n");
		return VER_ERROR;
	}
	/* coverity[secure_coding] */
	strncat(dload_info->iso_ver,dload_iso,strlen(dload_iso));

	/*获取dload id(升级中使用的产品名)*/
	product_name = bsp_version_get_dloadid();
	if(NULL == product_name){
		ver_print_error("get product name is null\n");
		return VER_ERROR;
	}
	/* coverity[secure_coding] */
	strncat(dload_info->product_name,product_name,strlen(product_name));

	/*获取software_version*/
	firmware = bsp_version_get_firmware();
	if(NULL == firmware){
		ver_print_error("get firmware version is null\n");
		return VER_ERROR;
	}
	/* coverity[secure_coding] */
	strncat(dload_info->software_ver,firmware,strlen(firmware));

	return VER_OK;

}
/*需要保证在nv模块初始化之后*/
module_init(bsp_productinfo_init);

#endif
/*****************************************************************************
* 函数	: bsp_get_board_actual_type
* 功能	: get board actual type 
* 输入	: void
* 输出	: void
* 返回	: BOARD_ACTUAL_TYPE_E       BBIT/SFT/ASIC
*
* 其它       : 无
*
*****************************************************************************/
BOARD_ACTUAL_TYPE_E bsp_get_board_actual_type(void)
{
	u32 chip_ver = 0;

	chip_ver = bsp_version_get_chip_type();

	switch(chip_ver){
		case CHIP_VER_P531_ASIC:
		case CHIP_VER_P532_ASIC:
			return BOARD_TYPE_BBIT;

		case CHIP_VER_HI6950_ASIC:			
		case CHIP_VER_HI3630_ASIC:
		case CHIP_VER_HI6921_ASIC:
		case CHIP_VER_HI6930_ASIC:
		case CHIP_VER_HI3635_ASIC:
			return BOARD_TYPE_ASIC;

		case CHIP_VER_HI6950_SFT:
		case CHIP_VER_HI3635_SFT:
			return BOARD_TYPE_SFT;

		default:
			return BOARD_TYPE_MAX;		
	}
}


int bsp_version_debug(void)
{
	int ret=0;
#ifdef __KERNEL__
	struct dload_info_type info={"\0","\0","\0"};
#endif

	/*判断version初始化是否成功*/
    if(huawei_product_info.index == HW_VER_INVALID){
		ver_print_error("huawei_product_info init failed!HW_VERSION_ADDR:0x%x\n",*(u32 *)HW_VERSION_ADDR);
		return VER_ERROR;
	}
	ver_print_error("\n***************************************************\n");
	ver_print_error("\n*************huawei_product_info*******************\n");
	ver_print_error("\n***************************************************\n");
	ver_print_error("Hardware index	:0x%x\n",huawei_product_info.index);
	ver_print_error("hw_Sub_ver		:0x%x\n",huawei_product_info.hwIdSub);
	ver_print_error("Inner name 	:%s\n",huawei_product_info.name);
	ver_print_error("name plus 		:%s\n",huawei_product_info.namePlus);
	ver_print_error("HardWare ver	:%s\n",huawei_product_info.hwVer);
	ver_print_error("DLOAD_ID		:%s\n",huawei_product_info.dloadId);
	ver_print_error("Out name		:%s\n",huawei_product_info.productId);

	ver_print_error("\n***************************************************\n");
	ver_print_error("\n*************from func get*******************\n");
	ver_print_error("\n***************************************************\n");
	ver_print_error("Hardware index	:0x%x\n",bsp_version_get_hwversion_index());
	ver_print_error("Chip Type   	:0x%x\n",bsp_version_get_chip_type());
	ver_print_error("HardWare ver	:%s\n",bsp_version_get_hardware());
	ver_print_error("Inner name		:%s\n",bsp_version_get_product_inner_name());
	ver_print_error("Out name		:%s\n",bsp_version_get_product_out_name());
	ver_print_error("PCB_ver		:%s\n",bsp_version_get_pcb());
	ver_print_error("DLOAD_ID		:%s\n",bsp_version_get_dloadid());
	ver_print_error("Build_time		:%s\n",bsp_version_get_build_date_time());
	ver_print_error("Chip_ver		:%s\n",bsp_version_get_chip());
	ver_print_error("Firmware		:%s\n",bsp_version_get_firmware());
	ver_print_error("CommitID		:%s\n",bsp_version_get_hash());
	ver_print_error("ISO_ver		:%s\n",bsp_version_get_iso());
	ver_print_error("WebUI			:%s\n",bsp_version_get_webui());
	ver_print_error("Release_ver	:%s\n",bsp_version_get_release());

#ifdef __KERNEL__

	ver_print_error("dload_version	:%s\n",bsp_dload_get_dload_version());
	ret=bsp_dload_get_dload_info(&info);
#endif
	return VER_OK|ret;
}
/*lint -restore*/

