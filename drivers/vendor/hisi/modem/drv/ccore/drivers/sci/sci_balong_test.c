/* Copyright (c) 2004-2008 Wind River Systems, Inc.
*
* The right to copy, distribute, modify or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
*/



/*
DESCRIPTION:
none

TEST SPECIFICATION

\cs
<module>
<modUnderTest>  sci </modUnderTest>
<component>  INCLUDE_TM_SCI </component>
<minVxWorksVer> 5.5        </minVxWorksVer>
<maxVxWorksVer>  .*        </maxVxWorksVer>
<arch>           .*        </arch>
<cpu>            .*        </cpu>
<bsp>            .*        </bsp>
</module>
\ce

KNOWN LIMITATIONS: NONE

*/

/* includes */
#include <vxWorks.h>
#include <sysLib.h>
#include <logLib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <semLib.h>
#include <intLib.h>
#include <tickLib.h>
#include "osl_types.h"
#include "bsp_sci.h"
#include "drv_sci.h"
#include "osl_bio.h"

/*lint -e525*/


/*根据所使用的测试卡，置1 编译相关代码
置0 不会编译相关测试代码
*/
#define  SCI_DEBUG_PRINT     (1)
#define  USE_USIM_CARD_TEST  (1)
#define  USE_SIM_CARD_TEST   (0)
#define BSP_REG_SETBITS(base, reg, pos, bits, val) (BSP_REG(base, reg) = (BSP_REG(base, reg) & (~((((u32)1 << (bits)) - 1) << (pos)))) \
                                                                         | ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))
#define BSP_REG_GETBITS(base, reg, pos, bits) ((BSP_REG(base, reg) >> (pos)) & (((u32)1 << (bits)) - 1))

#ifndef WAIT_FOREVER
#define WAIT_FOREVER	(-1)
#endif



typedef enum
{
	V_IMPERATIVE =  0, /* Imperative - can't be filtered */
	V_ERROR      =  1, /* Test ERROR - why can't continue */
	V_ABORT      =  1, /* Test ABORT - why aborted */
	V_FAIL       =  2, /* Test FAIL - reason for failure */
	V_PASS       =  3, /* Test PASS - progress  */
	V_SKIP       =  3, /* Test SKIP - why test case should be skipped */
	V_GENERAL    =  4  /* General information */
} VXTEST_VERBOSITY;

typedef enum       /* VXTEST_STATUS - return value from test case */
{
	VXTEST_PASS   = 0,   /* Test passed perfectly */
	VXTEST_FAIL   = 1,   /* Test result differed from expected */
	VXTEST_ERROR  = 2,   /* DEPRECATED - use VXTEST_ABORT instead */
	VXTEST_ABORT  = 3,   /* Inconclusive - test couldn't run properly */    
	VXTEST_SKIP   = 4    /* Test is skipped  */
} VXTEST_STATUS;

#define USR_CLK_FREQ   (48)
#define SCI_RESET_NORMAL_DELAY_TIME     (sysClkRateGet())


#define SCI_ATR_MAX_LEN                 (32)
#define SCI_DATA_MAX_LEN                (256)
#define SCI_CARD_TYPE_UNKOWN            (0)
#define SCI_CARD_TYPE_SIM               (1)
#define SCI_CARD_TYPE_USIM              (2)
#define SCI_CARD_STATUS_NO_CARD         (0)
#define SCI_CARD_STATUS_CARD_NOT_ACTIVE (1)
#define SCI_CARD_STATUS_CARD_READY      (2)
#define SCI_CARD_STATUS_CARD_BUSY       (3)
#define SCI_CARD_CLK_OPEN               (0)
#define SCI_CARD_CLK_STOP               (1)
#define SCI_CARD_VOLTAGE_180            (180)
#define SCI_CARD_VOLTAGE_300            (300)
#define SCI_TIME_STOP_CFG_NOT_SUPPORT   (0)
#define SCI_TIME_STOP_CFG_HIGH_LEVEL    (5)
#define SCI_TIME_STOP_CFG_LOW_LEVEL     (9)

#define SCI_RESET_NO_CARD_DELAY_TIME    (2*sysClkRateGet())

#define SCI_FILE_TYPE_BINARY            (1)
#define SCI_FILE_TYPE_LINEAR            (2)
#define SCI_FILE_TYPE_CYCLE             (3)


/*typedef*/
typedef struct _sci_data_list
{
	BSP_U32 length;
	BSP_U8  data[SCI_DATA_MAX_LEN];
}sci_data_list;


BSP_U32 g_cardtype = 0;
BSP_U8 g_curFCPlen = 0;

BSP_U32 g_SciUsrClkID = 0xffffffff;

/*global var*/
SEM_ID sciSemCardDeactiveSuccess;
SEM_ID sciSemCardResetSuccess;
SEM_ID sciSemReadData;
SEM_ID sciSemTxErr;
BSP_U8 sciRealATR[SCI_ATR_MAX_LEN] = {0};
BSP_U8 sciUSimExpectATR[] = {0x3b,0x9f,0x94,0x80,0x1f,0xc7,0x80,0x31,0xe0,0x73,0xfe,0x21,0x11,0x63,0x3f,0x1,0x11,0x83,0x7,0x90,0x0,0xe7};
BSP_U8 sciSimExpectATR[] = {0x3b,0x3f,0x94,0x0,0x80,0x69,0xaf,0x3,0x19,0x1,0x93,0x0,0x0,0x6,0xe,0x83,0x3e,0x9f,0x16};
/*名字虽然时tick，但是单位已经更换为时钟频率*/
//BSP_U32 tickReadDataStart = 0;
//BSP_U32 tickReadDataEnd = 0;
//BSP_U32 currentReadDataTick = 0;
BSP_U32 tickDeactiveSuccessStart = 0;
BSP_U32 tickDeactiveSuccessEnd = 0;
BSP_U32 currentDeactiveSuccessTick = 0;
BSP_U32 tickResetSuccessStart = 0;
BSP_U32 tickResetSuccessEnd = 0;
BSP_U32 currentResetSuccessTick = 0;
/*sendDataList与recvDataList应该时一一对应的*/
sci_data_list simSendDataList[] = {
	{0,{0}},
	{0,{0}}
};
sci_data_list simRecvDataList[] = {
	{0,{0}},
	{0,{0}}
};
BSP_S32 simRecvExpectTicks[] = {
	2000,
	0
};
sci_data_list usimSendDataList[] = {
	{0,{0}},
	{0,{0}},
	{0,{0}}
};
sci_data_list usimRecvDataList[] = {
	{0,{0}},
	{0,{0}},
	{0,{0}}
};
BSP_S32 usimRecvExpectTicks[] = {
	2000,
	0
};
extern SCI_SET_SELECT_STR g_sci_set_select;
// 20130306--Parity Error Detect Modify Begin:
BSP_BOOL bParityDetectFlag = FALSE;
// 20130306--Parity Error Detect Modify End!
/********************Test Helper Declaration********************/
BSP_S32 tmSciSemCreate(void);
BSP_S32 tmSciSemDelete(void);
/*
tmSciStatusCheck:检查卡状态是否符合预期，用于正常功能测试。
*/
BSP_S32 tmSciStatusCheck(BSP_U32 expectCardStatus);
BSP_S32 tmSciStatusPrint(void);
/*
tmSciClkStatusCheck:检查卡时钟状态是否符合预期，用于正常功能测试。
*/
BSP_S32 tmSciClkStatusCheck(BSP_U32 expectClkStatus);
/*
sci_memcmp_ext:内存比较扩展函数，不一致时返回ERROR，
同时打印不一致的位置；一致时返回OK.
*/
int sci_memcmp_ext(const void* mem1,const void* mem2,int size);
/*
tmSciATRCheck:检查获取到的ATR长度和内容是否和预期一致。
*/
BSP_S32 tmSciATRCheck();
BSP_S32 tmSciRegEvent(Sci_Event_Func pfunc);
BSP_U32 sci_event_func_001(BSP_U32 u32Event,BSP_VOID* para);
BSP_S32 tmSciResetDeactive(BSP_U32 cardType);
BSP_S32 tmSciSendRecvData(BSP_U32 cardType,
						  BSP_U32 sendLen,
						  BSP_U8* sendData,
						  BSP_U32 preReadLen,
						  BSP_U8* recvData,
						  BSP_S32 waitTicks/*wait time between send and preread*/);
BSP_S32 tmSciSendMaxLenData(BSP_U32 cardType,
							BSP_U32 sendLen,
							BSP_U8* sendData,
							BSP_U32 delayTime);
BSP_S32 tmSciGeneralSendRecvData(BSP_U32 cardType,
								 BSP_U32  sendLen,
								 BSP_U8*  sendData,
								 BSP_S32  waitTicks/*wait time between send and preread*/,
								 BSP_U32  preReadLen,
								 BSP_BOOL recvOnce,
								 BSP_S32  expectRecvLen,
								 BSP_U8*  expectRecvData,
								 BSP_S32  expectRecvTicks);
BSP_S32 tmSciVoltageCheck(BSP_S32 expectVoltage);
/*
testType: 0--->由1.8V切换到3.0V
: 1--->3.0V的前提下复位卡再获取电压值
: 2--->3.0V的前提下去激活卡再获取电压值
: 3--->3.0V的前提下再调用电压切换接口
: 4--->卡去激活后进行电压切换
: 5--->卡发送数据过程中进行电压切换
: 6--->卡预读数据过程中进行电压切换
: 7--->卡接收数据过程中进行电压切换
*/
BSP_S32 tmSciChangeVoltage(BSP_U32 cardType,BSP_U32 testType);
/*
testType:0--->停止时钟后发送数据
1--->接收数据过程中时钟停止
2--->停止时钟后复位卡
3--->停止时钟后去激活卡
4--->停止时钟情况下，电压切换
5--->卡不支持停止时钟操作，停止时钟
6--->发送过程中时钟停止,预读数据过程中时钟停止
7--->时钟停止后再时钟停止
*/
BSP_S32 tmSciClkStopTest(BSP_S32 testType,
						 BSP_U32 cardType,
						 BSP_S32 cardVoltage,
						 BSP_U32 timeStopCfg,
						 BSP_U32 sendLen,
						 BSP_U8* sendData,
						 BSP_U32 preReadLen,
						 BSP_U8* recvData,
						 BSP_S32 waitTicks/*wait time between send and preread*/);
BSP_S32 tmSciGetATR(BSP_U32 cardType,
					BSP_U32 timeStopCfg,
					BSP_U32 sendLen,
					BSP_U8* sendData,
					BSP_U32 preReadLen,
					BSP_U8* recvData,
					BSP_U8  expectATRLen,
					BSP_U8* expectATR);
/*
testType:0--->发送数据过程中复位卡
1--->预读数据过程中复位卡
2--->接收数据过程中复位卡
3--->发送数据过程中去激活卡
4--->预读数据过程中去激活卡
5--->接收数据过程中去激活卡
*/
BSP_S32 tmSciResetDeactiveExtTest(BSP_S32 testType,
								  BSP_U32 cardType,
								  BSP_U32 sendLen,
								  BSP_U8* sendData,
								  BSP_U32 preReadLen,
								  BSP_U8* recvData);
BSP_S32 tmSciRemoveAndInsertAfterDeactive(BSP_U32 cardType);
BSP_S32 tmSciNotInsertCardTest(void);
BSP_S32 tmSciInsertCardNotReset(void);
BSP_S32 tmSciErrorCodeExtTest(void);
BSP_S32 tmSciSendRecvPrepare(BSP_U32 type);
BSP_S32 tmUSimSendRecvDataAndTimeStopPerfTest(BSP_BOOL classSwitch,BSP_U32 times);
BSP_S32 tmUSimResetPerfTest(BSP_U32 times);
BSP_S32 tmUSimResetDeactivePerfTest(BSP_U32 times);
// added by yangzhi for universal test

BSP_S32 tmSciSendRecvPrepareWithReLength(BSP_U32 type, BSP_U32* relength);
// added by zhouluojun for file select test
BSP_S32 tmSciPrepare(BSP_U32 cardType);
BSP_S32 tmSciAppPrepare();
BSP_S32 tmSciSelectAdf();
BSP_U32 tstSCI_Init();
void  tstSCI_DeInit();
BSP_S32 tmSciReadEFdir(BSP_U8 *pLen, BSP_U8 *data);
BSP_S32 tmSciReadFile(BSP_U32 FileType, BSP_U8* fid, BSP_U8 record, BSP_U8 *pLen, BSP_U8 *data);
BSP_S32 tmSciSelectFile(BSP_U8* cmd, BSP_U8* fid);
BSP_S32 tmSciEnd();
BSP_S32 tmSciSendRecvDataNew(BSP_U32 cardType,
						  BSP_U32 sendLen,
						  BSP_U8* sendData,
						  BSP_U32 preReadLen,
						  BSP_U8* recvData,
						  BSP_S32 waitTicks);



#define SCI_RECV_ALL()\
do{\
    BSP_U32 ret;\
    BSP_U32 l;\
	BSP_U8  d[SCI_DATA_MAX_LEN];\
	ret = bsp_sci_all_data_read(&l,d);\
 	if(ret != OK)\
	{\
		printf("(%s)%d: bsp_sci_all_data_read fail,ret:%x\n",__FUNCTION__, __LINE__, ret);\
		return ERROR;\
	}\
	printf("realRecvLen:%d\n",l);\
	for(i = 0; i <l; i++)\
	{\
		printf(" 0x%x",d[i]);\
	}\
    printf("\n");\
}while(0)

#if SCI_DEBUG_PRINT
#define SCI_PRINT_DATA(l,dary)\
do{\
    BSP_U32 k;\
    printf("%s(%d): recv data: ", __FUNCTION__, __LINE__);\
	for(k = 0; k <(BSP_U32)l; k++)\
	{\
		printf(" 0x%x",dary[k]);\
	}\
    printf("\n");\
}while(0)

#else

#define SCI_PRINT_DATA(l,d)

#endif

#define SCI_CHECK_DATA(ss,ds,l)\
do{\
	if(sci_memcmp_ext((void*)ss,(void*)ds,l) != 0)\
	{\
		printf("%s(%d): recv data is not equal to expect.\n", __FUNCTION__, __LINE__);\
		return ERROR;\
	}\
}while(0)

#define TAG() printf("%s(%d)\n", __FUNCTION__, __LINE__);

#if 0
BSP_VOID ULTimeStart()
{
    BSP_USRCLK_Alloc(&g_ModemClkId);
    BSP_USRCLK_Connect((FUNCPTR)ClkIntFunc, 0, g_ModemClkId);    
    BSP_USRCLK_Enable(g_ModemClkId);
    BSP_USRCLK_TimerStart(g_ModemClkId, 0xffffffff);
    g_ModemUlIpNumStart = g_ModemUlIpNum;
}
#endif 


void testReset()
{
	BSP_U32 u32Result = 0;
	UINT32 ulCardStatus;
	u32Result = bsp_sci_reset(COLD_RESET);
	logMsg("\r\n bsp_sci_reset u32Result = %d",u32Result,0,0,0,0,0);
	taskDelay(50);
	ulCardStatus = bsp_sci_card_status_get();
	logMsg("ulCardStatus = %d\n", (BSP_U32) ulCardStatus,0,0,0,0,0);
}

void testGetStatus()
{
	UINT32 ulCardStatus;

	ulCardStatus = bsp_sci_card_status_get();
	logMsg("ulCardStatus = %d\n", (BSP_U32) ulCardStatus,0,0,0,0,0);
}


BSP_U32 sci_get_timestart()
{
	BSP_U32 timestart = 0;
	
#if 0
	(BSP_VOID)BSP_USRCLK_RateSet(1,g_SciUsrClkID);
	(BSP_VOID)BSP_USRCLK_Enable(g_SciUsrClkID);
	 BSP_USRCLK_TimerValue(g_SciUsrClkID,&timestart);
#endif

	return timestart;
}
BSP_U32 sci_get_timeover()
{
	BSP_U32 timeover = 0;
	
#if 0
	//BSP_USRCLK_TimerValue(g_SciUsrClkID,&timeover);
	(BSP_VOID)BSP_USRCLK_Disable(g_SciUsrClkID);
#endif

	return timeover;
}
BSP_U32 tstSCI_Init()
{
    BSP_U32 u32Ret = OK;
    
    //u32Ret == BSP_USRCLK_Alloc(&g_SciUsrClkID);
	g_SciUsrClkID = 1;
    
    if(u32Ret == OK)
    {
        return BSP_OK;
    }
    else
    {
        return u32Ret;
    }
}
void tstSCI_DeInit()
{
    //BSP_USRCLK_Free(g_SciUsrClkID);
	g_SciUsrClkID = 0xffffffff;
}
/**********************Test Case Implementation*************************/
/*未插卡，未激活，卡类型未知，卡状态为未激活*/
/*******************************************************************************/
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_AUTO_TEST */
VXTEST_STATUS tstSciCardNotInsert(void)
{
	BSP_U32 expectCardStatus = SCI_CARD_STATE_NOCARD;

	if(tmSciStatusCheck(expectCardStatus) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*未插卡，激活，
*激活后无延时
* 目前实现为卡类型未知，卡状态为未激活
*激活后有延时
* 目前实现为卡类型未知，卡状态为无卡
* */
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_AUTO_TEST */
VXTEST_STATUS tstSciCardNotInsertAndReset1(void)
{
    BSP_U32 expectCardStatus = SCI_CARD_STATE_NOCARD;

	BSP_S32 s32Ret = 0;

	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		return VXTEST_FAIL;
	}
	if(tmSciStatusCheck(expectCardStatus) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_AUTO_TEST */
VXTEST_STATUS tstSciCardNotInsertAndReset2(void)
{
	BSP_U32 expectCardStatus = SCI_CARD_STATE_NOCARD;
	BSP_U32 s32Ret = 0;

	s32Ret = bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		return VXTEST_FAIL;
	}
	/*TODO：未插入卡，复位卡，该接口为连续尝试7次左右，这个时间是可预期的，需要蒙存给出*/
	taskDelay(SCI_RESET_NO_CARD_DELAY_TIME);
	if(tmSciStatusCheck(expectCardStatus) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*插卡，未激活，卡类型未知，卡状态为未激活*/
/*******************************************************************************
*
* tstSciCardInsertNotReset - tstSciCardInsertNotReset
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciCardInsertNotReset(void)
{
	BSP_U32 expectCardStatus = SCI_CARD_STATE_NOCARD;

	if(tmSciStatusCheck(expectCardStatus) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciInsertCardNotReset - tstSciInsertCardNotReset
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciInsertCardNotReset(void)
{
	if(tmSciInsertCardNotReset() != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}

BSP_U8 g_recvData[] = {0xc0,0x0,0x0,0xc,0xc0,0x7f,0x20,0x2,0x0,0xf,0x55,0xff,0x1,0x15,0xb1,0x0,
0x15,0x6,0x0,0x83,0x8a,0x83,0x8a,0x0,0x80,0x83,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x90,0x0};

#if USE_USIM_CARD_TEST
/**************************************************************
USIM(白卡)相关测试用例
****************************************************************/
#if 0
BSP_U8 g_recvData[] = {0xc0,0x0,0x0,0xc,0xc0,0x7f,0x20,0x2,0x0,0xf,0x55,0xff,0x1,0x15,0x31,0x0,
0x15,0x6,0x0,0x83,0x8a,0x83,0x8a,0x0,0x86,0x83,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x90,0x0};
#endif
/*******************************************************************************
*
* tstSciUSimResetDeactive - tstSciUSimResetDeactive
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimResetDeactive(void)
{
	if(tmSciResetDeactive(SCI_CARD_TYPE_USIM) != OK)
	{
		return VXTEST_FAIL;
	}
	printf("ok.\n");
	return VXTEST_PASS;

}
/*******************************************************************************
*
* tstSciUSimSendRecvData - tstSciUSimSendRecvData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimSendRecvData(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8 sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 recvLen = 37;
	BSP_S32 waitTicks = 0;
      BSP_S32 s32Ret = 0;

/*	if(tmSciSendRecvData(SCI_CARD_TYPE_USIM,sendLen,sendData,recvLen,g_recvData,waitTicks) != OK)
	{
		return VXTEST_FAIL;
	}*/
	s32Ret = tmSciGeneralSendRecvData(SCI_CARD_TYPE_USIM,
		sendLen,
		sendData,
		waitTicks,
		recvLen,
		TRUE,
		recvLen,
		g_recvData,
		4);
        if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimWaitAMomentBeforePreRead - tstSciUSimWaitAMomentBeforePreRead
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimWaitAMomentBeforePreRead(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 recvLen = 37;

	BSP_S32 waitTicks = 4;
	if(tmSciSendRecvData(SCI_CARD_TYPE_USIM,sendLen,sendData,recvLen,g_recvData,waitTicks) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;

}
/*******************************************************************************
*
* tstSciUSimPreReadLenIsHalf - tstSciUSimPreReadLenIsHalf
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimPreReadLenIsHalf(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_S32 expectRecvLen = 37;
	BSP_S32 expectRecvTicks = 4;
	BSP_S32 waitTicks = 0;

	BSP_U32 preReadLen = (BSP_U32)expectRecvLen/2;
	BSP_BOOL recvOnce = TRUE;
	BSP_S32 s32Ret = 0;
	s32Ret = tmSciGeneralSendRecvData(SCI_CARD_TYPE_USIM,
		sendLen,
		sendData,
		waitTicks,
		preReadLen,
		recvOnce,
		expectRecvLen,
		g_recvData,
		expectRecvTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimPreReadLenIsZero - tstSciUSimPreReadLenIsZero
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimPreReadLenIsZero(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_S32 expectRecvLen = 37;
	BSP_S32 expectRecvTicks = 6;
	BSP_S32 waitTicks = 0;
	BSP_U32 preReadLen = 0;
	BSP_BOOL recvOnce = TRUE;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciGeneralSendRecvData(SCI_CARD_TYPE_USIM,
		sendLen,
		sendData,
		waitTicks,
		preReadLen,
		recvOnce,
		expectRecvLen,
		g_recvData,
		expectRecvTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimRecvTwice - tstSciUSimRecvTwice
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimRecvTwice(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_S32 expectRecvLen = 37;
	BSP_S32 expectRecvTicks = 4;
	BSP_S32 waitTicks = 0;
	BSP_U32 preReadLen = (BSP_U32)expectRecvLen;
	BSP_BOOL recvOnce = FALSE;
	BSP_S32 s32Ret = 0;
	s32Ret = tmSciGeneralSendRecvData(SCI_CARD_TYPE_USIM,
		sendLen,
		sendData,
		waitTicks,
		preReadLen,
		recvOnce,
		expectRecvLen,
		g_recvData,
		expectRecvTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimSendMaxLen - tstSciUSimSendMaxLen
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimSendMaxLen()
{
	BSP_S32 s32Ret = 0;
	BSP_U32 sendLen = 255;// 256
	BSP_U8  sendData[256] = {0};
	s32Ret = tmSciSendMaxLenData(SCI_CARD_TYPE_USIM,sendLen,sendData,150);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimPreReadBig - tstSciUSimPreReadBig
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimPreReadBig(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_S32 expectRecvLen = 37;
	//BSP_S32 expectRecvTicks = 4;
	BSP_S32 expectRecvTicks = 5;

	BSP_S32 waitTicks = 0;
	BSP_U32 preReadLen = (BSP_U32)expectRecvLen + 1;
	BSP_BOOL recvOnce = TRUE;
	BSP_S32 s32Ret = 0;
	s32Ret = tmSciGeneralSendRecvData(SCI_CARD_TYPE_USIM,
		sendLen,
		sendData,
		waitTicks,
		preReadLen,
		recvOnce,
		expectRecvLen,
		g_recvData,
		expectRecvTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimChangeV - tstSciUSimChangeV
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimChangeV(void)
{
	if(tmSciChangeVoltage(SCI_CARD_TYPE_USIM,0) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimResetAfterChangeV - tstSciUSimResetAfterChangeV
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimResetAfterChangeV(void)
{
	if(tmSciChangeVoltage(SCI_CARD_TYPE_USIM,1) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimDeactiveAfterChangeV - tstSciUSimDeactiveAfterChangeV
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimDeactiveAfterChangeV(void)
{
	if(tmSciChangeVoltage(SCI_CARD_TYPE_USIM,2) != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimV180TimeStopSendRecvData - tstSciUSimV180TimeStopSendRecvData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimV180TimeStopSendRecvData(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(0,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,g_recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimV300TimeStopSendRecvData - tstSciUSimV300TimeStopSendRecvData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimV300TimeStopSendRecvData(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(0,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_300,
		timeStopCfg,sendLen,sendData,preReadLen,g_recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimTimeStopRecvData - tstSciUSimTimeStopRecvData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimTimeStopRecvData(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(1,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,g_recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimTimeStopThenReset - tstSciUSimTimeStopThenReset
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimTimeStopThenReset(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = usimSendDataList[0].length;
	BSP_U8* sendData = usimSendDataList[0].data;
	BSP_U32 preReadLen = usimRecvDataList[0].length;
	BSP_U8* recvData = usimRecvDataList[0].data;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(2,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimTimeStopThenDeactive - tstSciUSimTimeStopThenDeactive
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimTimeStopThenDeactive(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = usimSendDataList[0].length;
	BSP_U8* sendData = usimSendDataList[0].data;
	BSP_U32 preReadLen = usimRecvDataList[0].length;
	BSP_U8* recvData = usimRecvDataList[0].data;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(3,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimTimeStopThenChangeV - tstSciUSimTimeStopThenChangeV
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimTimeStopThenChangeV(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = usimSendDataList[0].length;
	BSP_U8* sendData = usimSendDataList[0].data;
	BSP_U32 preReadLen = usimRecvDataList[0].length;
	BSP_U8* recvData = usimRecvDataList[0].data;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(4,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimGetATR - tstSciUSimGetATR
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimGetATR(void)
{
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;

	BSP_U8  expectATRLen = 22;
	BSP_U8*  expectATR = sciUSimExpectATR;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciGetATR(cardType,timeStopCfg,sendLen,
		sendData,preReadLen,g_recvData,
		expectATRLen,expectATR);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSendDataThenReset - tstSciSendDataThenReset
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSendDataThenReset()
{
	BSP_S32 testType = 0;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 s32Ret = 0;
	s32Ret = tmSciResetDeactiveExtTest(testType,cardType,sendLen,sendData,preReadLen,g_recvData);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciReceiveAfterReset - tstSciReceiveAfterReset
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciReceiveAfterReset()
{
	BSP_S32 testType = 2;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciResetDeactiveExtTest(testType,cardType,sendLen,sendData,preReadLen,g_recvData);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSendDataThenDeactive - tstSciSendDataThenDeactive
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSendDataThenDeactive()
{
	BSP_S32 testType = 3;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciResetDeactiveExtTest(testType,cardType,sendLen,sendData,preReadLen,g_recvData);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciReceiveAfterDeactive - tstSciReceiveAfterDeactive
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciReceiveAfterDeactive()
{
	BSP_S32 testType = 5;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciResetDeactiveExtTest(testType,cardType,sendLen,sendData,preReadLen,g_recvData);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciReSendAfterSend - tstSciReSendAfterSend
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciReSendAfterSend()
{
	BSP_S32 testType = 6;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciResetDeactiveExtTest(testType,cardType,sendLen,sendData,preReadLen,g_recvData);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSendDataAndRecvAll - tstSciSendDataAndRecvAll
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSendDataAndRecvAll()
{
	BSP_S32 testType = 7;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 s32Ret = 0;

	s32Ret = tmSciResetDeactiveExtTest(testType,cardType,sendLen,sendData,preReadLen,g_recvData);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}

/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_AUTO_TEST */
VXTEST_STATUS tstSciUSimRemoveAndInsertAfterDeactive()
{
	if(tmSciRemoveAndInsertAfterDeactive(SCI_CARD_TYPE_USIM) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimReChangeV - tstSciUSimReChangeV
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimReChangeV(void)
{
	if(tmSciChangeVoltage(SCI_CARD_TYPE_USIM,3) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciChangeVAfterDeactive - tstSciChangeVAfterDeactive
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciChangeVAfterDeactive(void)
{
	if(tmSciChangeVoltage(SCI_CARD_TYPE_USIM,4) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciChangeVAfterSendData - tstSciChangeVAfterSendData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciChangeVAfterSendData(void)
{
	if(tmSciChangeVoltage(SCI_CARD_TYPE_USIM,5) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimTimeStopAfterSendData - tstSciUSimTimeStopAfterSendData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimTimeStopAfterSendData(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = 5;
	BSP_U8 sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(6,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,g_recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciUSimReTimeStop - tstSciUSimReTimeStop
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimReTimeStop(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 sendLen = simSendDataList[0].length;
	BSP_U8* sendData = simSendDataList[0].data;
	BSP_U32 preReadLen = simRecvDataList[0].length;
	BSP_U8* recvData = simRecvDataList[0].data;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(7,SCI_CARD_TYPE_USIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}

void initializeArrayToVisibleChar(BSP_U8 charArray[],BSP_U32 array_len)
{
    BSP_S32 i = 0;
    BSP_U8 minVisibleChar = 0x20; /*space*/
    BSP_U8 maxVisibleChar = 0x7E;  /*~*/
    for(i = 0; i < (BSP_S32)array_len; i++)
    {
        if(i < minVisibleChar)
        {
            charArray[i] = minVisibleChar + 1;/*!*/
        }
        else if(i > maxVisibleChar)
        {
            charArray[i] = maxVisibleChar;
        }
        else
        {
            charArray[i] = (BSP_U8)i;
        }
    }
}
/*
预处理
send: 0x00 0xa4 0x4 0x4 0xc
recv: 0xa4
send: 0xa0 0x0 0x0 0x0 0x87 0x10 0x2 0xff 0xff 0xff 0xff 0x89
recv: 0x61 xx
send: 0x00, 0xc0, 0x00, 0x00, 0xXX
recv: 0xc0
recv: [...]                                 //0xXX长度的字节，内容不需要验证
recv: 0x90 0x00                             //这3次数据可以一次性接收
分两部分，顺序执行

select EFsms:
send: 0x00, 0xA4, 0x08, 0x04, 0x04
recv: 0xA4
send: 0x7f, 0xff, 0x6f, 0x3c
recv: 0x61 0xXX                             //0xXX后面数据的长度，和卡相关，不固定
send: 0x00, 0xc0, 0x00, 0x00, 0xXX          //将上个命令中返回的0xXX填进去
recv: 0xc0
recv: [...]                                 //0xXX长度的字节，内容不需要验证
recv: 0x90 0x00                             //这3次数据可以一次性接收

write EFsms:
send: 0x00, 0xdc, 0x01, 0x04, 0xb0
recv: 0xdc
send: [...]                                 //176(0xb0)个字节，内容不限
recv: 0x90, 0x00
*/
BSP_S32 tmSciWriteEFsms(BSP_U32 cardType)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_S32 i = 0;
	/*preprocess*/
	BSP_U8 send_0_preprocess[] = {0x00,0xa4,0x4,0x4,0xc};
	//BSP_U8 send_0_preprocess[] = {0x00,0xa4,0x0,0x0,0xc};
	BSP_U8 recv_0_preprocess[] = {0xa4};
	BSP_U8 recv_0_preprocess_pre_len = 1;
	BSP_U8 send_1_preprocess[] = {0xa0,0x0,0x0,0x0,0x87,0x10,0x2,0xff,0xff,0xff,0xff,0x89};
	BSP_U8 recv_1_preprocess[] = {0x61,0x2f}; //最后一个参数与卡相关，不固定
	BSP_U8 recv_1_preprocess_pre_len = 2;
	BSP_U8 send_2_preprocess[] = {0x00, 0xc0, 0x00, 0x00, 0x2f};//最后一个参数为后面接收的字节数
	BSP_U8 recv_2_preprocess[] = {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                              0x90,0x00};
	BSP_U8 recv_2_preprocess_pre_len = 1/*0xc0*/ + recv_1_preprocess[1] + 2/*0x90 0x00*/;
	/*select EFsms*/
	BSP_U8 send_0_select[] = {0x00, 0xA4, 0x08, 0x04, 0x04};
	BSP_U8 recv_0_select[] = {0xA4};
	BSP_U8 recv_0_select_pre_len = 1;
	BSP_U8 send_1_select[] = {0x7f, 0xff, 0x6f, 0x3c};
	BSP_U8 recv_1_select[] = {0x61, 0x26};//最后一个参数与卡相关，不固定
    BSP_U8 recv_1_select_pre_len = 2;
	BSP_U8 send_2_select[] = {0x00, 0xc0, 0x00, 0x00, 0x26};//最后一个参数为后面接收的字节数
	BSP_U8 recv_2_select[] = {0xc0, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00,
	                          0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00,
	                          0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00,
	                          0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00,
	                          0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x90,
	                          0x00};
    BSP_U8 recv_2_select_pre_len = 1/*0xc0*/ + recv_1_select[1] + 2/*0x90 0x00*/;
    /*write EFsms*/
    BSP_U8 send_0_write[] = {0x00, 0xdc, 0x01, 0x04, 0xb0};
    BSP_U8 recv_0_write[] = {0xdc};
    BSP_U8 recv_0_write_pre_len = 1;
    BSP_U8 send_1_write_max_len = 176;
    BSP_U8 send_1_write[176] = {0x0};
    BSP_U8 recv_1_write[] = {0x90, 0x00};
    BSP_U8 recv_1_write_pre_len = 2;

    if(cardType == SCI_CARD_TYPE_SIM)
    {
        printf("与卡中数据相关，只支持白卡\n");
        return ERROR;
    }
    initializeArrayToVisibleChar(send_1_write,send_1_write_max_len);
	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	/*send_recv_0_preprocess*/
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_0_preprocess),send_0_preprocess);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint(); //cacelled by yangzhi for universal test
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();  //cacelled by yangzhi for universal test

    s32Ret = bsp_sci_data_read_sync(recv_0_preprocess_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	if(sci_memcmp_ext((void*)recv_0_preprocess,(void*)recvBuf,recv_0_preprocess_pre_len) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
    /*send_recv_1_select*/
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_1_preprocess),send_1_preprocess);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();

	s32Ret = bsp_sci_data_read_sync(recv_1_preprocess_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	/*if(sci_memcmp_ext((void*)recv_1_preprocess,(void*)recvBuf,recv_1_preprocess_pre_len - 1) != OK)
	{
		tmSciSemDelete();
		return ERROR;
	}  //cacelled by yangzhi for universal card test in 2010-11-16
    send_recv_2_select*/
    if(recvBuf[recv_1_preprocess_pre_len - 1] != send_2_preprocess[recv_1_preprocess_pre_len - 1]) 
	{	
		recv_2_preprocess_pre_len = recvBuf[recv_1_preprocess_pre_len - 1]+3;
		send_2_preprocess[recv_1_preprocess_pre_len - 1] = recvBuf[recv_1_preprocess_pre_len - 1];
    }
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_2_preprocess),send_2_preprocess);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();

	s32Ret = bsp_sci_data_read_sync(recv_2_preprocess_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	if((recvBuf[0] != recv_2_preprocess[0])
	||(recvBuf[recv_2_preprocess_pre_len - 2] != recv_2_preprocess[recv_2_preprocess_pre_len - 2])
	||(recvBuf[recv_2_preprocess_pre_len - 1] != recv_2_preprocess[recv_2_preprocess_pre_len - 1]))
	{
	    printf("recv_2_preprocess_pre_len:%d\n",recv_2_preprocess_pre_len);
	    printf("0:0x%x\n",recv_2_preprocess[0]);
	    printf("%d:0x%x\n",recv_2_preprocess_pre_len - 2,recv_2_preprocess[recv_2_preprocess_pre_len - 2]);
	    printf("%d:0x%x\n",recv_2_preprocess_pre_len - 1,recv_2_preprocess[recv_2_preprocess_pre_len - 1]);
	    printf("data cmp fail.\n");
	}

	/*send_recv_0_select*/
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_0_select),send_0_select);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",recv_0_select_pre_len);
#endif
	s32Ret = bsp_sci_data_read_sync(recv_0_select_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	for(i =0; i <recv_0_select_pre_len ; i++)
	{
		printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	}
#endif
	if(sci_memcmp_ext((void*)recv_0_select,(void*)recvBuf,recv_0_select_pre_len) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
    /*send_recv_1_select*/
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_1_select),send_1_select);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",recv_1_select_pre_len);
#endif
	s32Ret = bsp_sci_data_read_sync(recv_1_select_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	for(i =0; i <recv_1_select_pre_len ; i++)
	{
		printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	}
#endif
    if(recvBuf[recv_1_select_pre_len - 1] != send_2_select[recv_1_select_pre_len - 1]) 
	{	
		recv_2_select_pre_len = recvBuf[recv_1_select_pre_len - 1]+3;
		send_2_select[recv_1_select_pre_len - 1] = recvBuf[recv_1_select_pre_len - 1];
    }
/*	if(sci_memcmp_ext((void*)recv_1_select,(void*)recvBuf,recv_1_select_pre_len - 1) != OK)
	{
		tmSciSemDelete();
		return ERROR;
	}*/
    /*send_recv_2_select*/
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_2_select),send_2_select);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",recv_2_select_pre_len);
#endif
	s32Ret = bsp_sci_data_read_sync(recv_2_select_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	for(i =0; i <recv_2_select_pre_len ; i++)
	{
		printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	}
#endif
	if((recvBuf[0] != recv_2_select[0])//||(recvBuf[recv_2_select_pre_len - 2] != recv_2_select[recv_2_select_pre_len - 2])
	||(recvBuf[recv_2_select_pre_len - 1] != recv_2_select[recv_2_select_pre_len - 1]))
	{
	    printf("recv_2_preprocess_pre_len:%d\n",recv_2_select_pre_len);
	    printf("0:0x%x\n",recv_2_select[0]);
//	    printf("%d:0x%x\n",recv_2_select_pre_len - 2,recv_2_select[recv_2_select_pre_len - 2]);
	    printf("%d:0x%x\n",recv_2_select_pre_len - 1,recv_2_select[recv_2_select_pre_len - 1]);
	    printf("data cmp fail.\n");
	}

	/*send_recv_0_write*/
	s32Ret = (BSP_S32)bsp_sci_data_send(sizeof(send_0_write),send_0_write);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",recv_0_write_pre_len);
#endif
	s32Ret = bsp_sci_data_read_sync(recv_0_write_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	for(i =0; i <recv_0_write_pre_len ; i++)
	{
		printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	}
#endif
	if(sci_memcmp_ext((void*)recv_0_write,(void*)recvBuf,recv_0_write_pre_len) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	/*send_recv_1_write*/
	s32Ret = (BSP_S32)bsp_sci_data_send(send_1_write_max_len,send_1_write);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(10);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",recv_1_write_pre_len);
#endif
	s32Ret = bsp_sci_data_read_sync(recv_1_write_pre_len, recvBuf);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	for(i =0; i <recv_1_write_pre_len ; i++)
	{
		printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	}
#endif
	if(sci_memcmp_ext((void*)recv_1_write,(void*)recvBuf,recv_1_write_pre_len) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}
/*******************************************************************************
*
* tstSciErrorCodeExtTest - tstSciErrorCodeExtTest
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciErrorCodeExtTest(void)
{
	if(tmSciErrorCodeExtTest() != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_STRESS_TEST */
VXTEST_STATUS tstSciSendRecvTimeStopPerfTest(void)
{

	if(tmUSimSendRecvDataAndTimeStopPerfTest(BSP_FALSE,1000) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_STRESS_TEST */
VXTEST_STATUS tstSciSendRecvTimeStopPerfTestAfterClassSwitch(void)
{

	if(tmUSimSendRecvDataAndTimeStopPerfTest(BSP_TRUE,1000) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_STRESS_TEST */
VXTEST_STATUS tstSciResetPerfTest(void)
{
	if(tmUSimResetPerfTest(1000) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/* the function has been modified by yangzhi for auto test in 2010-10-29,in which the auto function-head has been cacelled as also as
a judgement of BSP_STRESS_TEST */
VXTEST_STATUS tstSciResetDeactivePerfTest(void)
{
	if(tmUSimResetDeactivePerfTest(1000) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
#endif
#if USE_SIM_CARD_TEST
/**************************************************************
SIM(黄卡)相关测试用例
****************************************************************/
/*******************************************************************************
*
* tstSciSimResetDeactive - tstSciSimResetDeactive
* \cs
* <testCase>
*     <timeout>     10000     </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimResetDeactive(void)
{
	BSP_U8 atrLen = 19;
	BSP_U8* atr = sciSimExpectATR;
	if(tmSciResetDeactive(SCI_CARD_TYPE_SIM) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSimSendRecvData - tstSciSimSendRecvData
* \cs
* <testCase>
*     <timeout>     10000     </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimSendRecvData(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x16};
	BSP_U32 recvLen = 25;
	BSP_U8  recvData[] = {0xc0,0x0,0x0,0x0,0x0,0x7f,0x20,0x2,0x0,0x0,0x0,0x0,0x0,0x9,0x91,0x0,
		0x12,0x8,0x0,0x83,0x8a,0x83,0x8a,0x90,0x0};
	BSP_S32 waitTicks = 0;
	if(tmSciSendRecvData(SCI_CARD_TYPE_SIM,sendLen,sendData,recvLen,recvData,waitTicks) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSimV300TimeStopSendRecvData - tstSciSimV300TimeStopSendRecvData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimV300TimeStopSendRecvData(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_LOW;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x16};
	BSP_U32 preReadLen = 25;
	BSP_U8  recvData[] = {0xc0,0x0,0x0,0x0,0x0,0x7f,0x20,0x2,0x0,0x0,0x0,0x0,0x0,0x9,0x91,0x0,
		0x12,0x8,0x0,0x83,0x8a,0x83,0x8a,0x90,0x0};
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(0,SCI_CARD_TYPE_SIM,SCI_CARD_VOLTAGE_300,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*
SIM时钟停止方式请查看刘蒙存相关文档获取
目前使用的SIM的时钟停止方式为低电平
最佳验证方式是，时钟停止后用示波器测量
*/
/*******************************************************************************
*
* tstSciSimTimeStopRecvData - tstSciSimTimeStopRecvData
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimTimeStopRecvData(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_LOW;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x16};
	BSP_U32 preReadLen = 25;
	BSP_U8  recvData[] = {0xc0,0x0,0x0,0x0,0x0,0x7f,0x20,0x2,0x0,0x0,0x0,0x0,0x0,0x9,0x91,0x0,
		0x12,0x8,0x0,0x83,0x8a,0x83,0x8a,0x90,0x0};
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(1,SCI_CARD_TYPE_SIM,SCI_CARD_VOLTAGE_300,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSimTimeStopThenReset - tstSciSimTimeStopThenReset
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimTimeStopThenReset(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_LOW;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x16};
	BSP_U32 preReadLen = 25;
	BSP_U8  recvData[] = {0xc0,0x0,0x0,0x0,0x0,0x7f,0x20,0x2,0x0,0x0,0x0,0x0,0x0,0x9,0x91,0x0,
		0x12,0x8,0x0,0x83,0x8a,0x83,0x8a,0x90,0x0};
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(2,SCI_CARD_TYPE_SIM,SCI_CARD_VOLTAGE_300,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSimTimeStopThenDeactive - tstSciSimTimeStopThenDeactive
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimTimeStopThenDeactive(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_LOW;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x16};
	BSP_U32 preReadLen = 25;
	BSP_U8  recvData[] = {0xc0,0x0,0x0,0x0,0x0,0x7f,0x20,0x2,0x0,0x0,0x0,0x0,0x0,0x9,0x91,0x0,
		0x12,0x8,0x0,0x83,0x8a,0x83,0x8a,0x90,0x0};
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(3,SCI_CARD_TYPE_SIM,SCI_CARD_VOLTAGE_300,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
/*******************************************************************************
*
* tstSciSimTimeStopThenChangeV - tstSciSimTimeStopThenChangeV
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciSimTimeStopThenChangeV(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 timeStopCfg = SCI_CLK_STATE_LOW;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x16};
	BSP_U32 preReadLen = 25;
	BSP_U8  recvData[] = {0xc0,0x0,0x0,0x0,0x0,0x7f,0x20,0x2,0x0,0x0,0x0,0x0,0x0,0x9,0x91,0x0,
		0x12,0x8,0x0,0x83,0x8a,0x83,0x8a,0x90,0x0};

	BSP_S32 waitTicks = 0;

	s32Ret = tmSciClkStopTest(4,SCI_CARD_TYPE_SIM,SCI_CARD_VOLTAGE_180,
		timeStopCfg,sendLen,sendData,preReadLen,recvData,waitTicks);
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}

	return VXTEST_PASS;
}
#endif
/*********************Test Helper Implementation*************************/
BSP_S32 tmSciSemCreate(void)
{
	int localErrno = 0;
    BSP_U32 u32Ret;
    
    u32Ret = tstSCI_Init();
    if(BSP_OK != u32Ret)
    {
        printf("g_SciUsrClkID alloc failed, it's value is %d\n",g_SciUsrClkID);
        printf("u32Ret is %d\n",u32Ret);
        return ERROR;
    }
	sciSemCardDeactiveSuccess = semBCreate(SEM_Q_FIFO,(SEM_B_STATE)SEM_EMPTY);
	if(sciSemCardDeactiveSuccess == NULL )
	{
		printf("1.semBCreate fail,errno:%x\n",localErrno);
        tstSCI_DeInit();
		return ERROR;
	}
	sciSemCardResetSuccess = semBCreate(SEM_Q_FIFO,(SEM_B_STATE)SEM_EMPTY);
	if(sciSemCardResetSuccess == NULL)
	{
		printf("2.semBCreate fail,errno:%x\n",localErrno);
        tstSCI_DeInit();
		semDelete(sciSemCardDeactiveSuccess);
		return ERROR;
	}
	sciSemReadData = semBCreate(SEM_Q_FIFO,(SEM_B_STATE)SEM_EMPTY);
	if(sciSemReadData == NULL)
	{
		printf("3.semBCreate fail,errno:%x\n",localErrno);
        tstSCI_DeInit();
		semDelete(sciSemCardDeactiveSuccess);
		semDelete(sciSemCardResetSuccess);
		return ERROR;
	}
	sciSemTxErr = semBCreate(SEM_Q_FIFO,(SEM_B_STATE)SEM_EMPTY);
	if(sciSemTxErr == NULL)
	{
		printf("4.semBCreate fail,errno:%x\n",localErrno);
        tstSCI_DeInit();
		semDelete(sciSemCardDeactiveSuccess);
		semDelete(sciSemCardResetSuccess);
		semDelete(sciSemReadData);
		return ERROR;
	}
	return OK;
}
BSP_S32 tmSciSemDelete(void)
{
    if(0xffffffff != g_SciUsrClkID)
    {
        //printf("free g_SciUsrClkID, and it's value is %d\n",g_SciUsrClkID);
        tstSCI_DeInit();
        //printf("afer free, g_SciUsrClkID is %d\n",g_SciUsrClkID);
    }
	semDelete(sciSemCardDeactiveSuccess);
	semDelete(sciSemCardResetSuccess);
	semDelete(sciSemReadData);
	semDelete(sciSemTxErr);

	return OK;
}
/*
tmSciStatusCheck:检查卡状态是否符合预期，用于正常功能测试。
*/
BSP_S32 tmSciStatusCheck(BSP_U32 expectCardStatus)
{
	BSP_U32 realCardStatus = 0;


	realCardStatus = bsp_sci_card_status_get();

	if(realCardStatus != expectCardStatus)
	{
		printf("real card status is not equal to expect.\n");
		printf("real card status:0x%x.\n",realCardStatus);
		printf("expect card status:0x%x.\n",expectCardStatus);
		return ERROR;
	}

	return OK;
}
BSP_S32 tmSciStatusPrint(void)
{
	BSP_U32 realCardStatus = 0;

	realCardStatus = bsp_sci_card_status_get();
#if SCI_DEBUG_PRINT
	printf("real card status:0x%x.\n",realCardStatus);
#endif
	return OK;
}
/*
tmSciClkStatusCheck:检查卡时钟状态是否符合预期，用于正常功能测试。
*/
BSP_S32 tmSciClkStatusCheck(BSP_U32 expectClkStatus)
{
	BSP_U32 realClkStatus = 0;
	BSP_S32 s32Ret;

	s32Ret = (BSP_S32)bsp_sci_get_clk_status(&realClkStatus);
	if(s32Ret != OK)
	{
		printf("bsp_sci_get_clk_status fail,ret:%x.\n",s32Ret);
		return ERROR;
	}

	if(realClkStatus != expectClkStatus)
	{
		printf("real clk status is not equal to expect.\n");
		printf("real clk status:%d,expect clk status:%d.\n",realClkStatus,expectClkStatus);
		return ERROR;
	}

	return OK;
}

/*
sci_memcmp_ext:内存比较扩展函数，不一致时返回ERROR，
同时打印不一致的位置；一致时返回OK.
*/
int sci_memcmp_ext(const void* mem1,const void* mem2,int size)
{
	int i = 0;
	for(i = 0; i < size; i++)
	{
		if(*((char*)mem1+i) != *((char*)mem2+i))
		{
			printf("diff_pos:%d,addr1(%x):%x,addr2(%x):%x\n",i,(BSP_S32)((char*)mem1+i),*((char*)mem1+i),(BSP_S32)((char*)mem2+i),*((char*)mem2+i));
			return ERROR;
		}
	}
	return OK;
}
/*
tmSciATRCheck:检查获取到的ATR长度和内容是否和预期一致。
*/
BSP_S32 tmSciATRCheck()
{
	BSP_S32 s32Ret = 0;
	BSP_U8 realATRLen = 0;
	BSP_U8 T0 = 0;
    BSP_U8 K = 0;
    SCI_ATRInfo stSCIATRInfo;
    
	memset((void*)sciRealATR,0,SCI_ATR_MAX_LEN);
	s32Ret = (BSP_S32)bsp_sci_atr_get(&realATRLen,sciRealATR,&stSCIATRInfo);
	if(s32Ret != OK)
	{
		printf("bsp_sci_atr_get fail,ret:%x\n",s32Ret);
		return ERROR;
	}
    T0 = sciRealATR[1];
    K = (BSP_U32)(T0&0xf);
    if(0x80 != sciRealATR[realATRLen-K])
    {
        printf("T1 is not equal to 0x80.\n");
        return ERROR;
    }
    else
    {
        printf("T1 is 0x80.\n"); 
        return OK;
    }
}
BSP_S32 tmSciRegEvent(Sci_Event_Func pfunc)
{
	BSP_S32 s32Ret = 0;
	s32Ret = (BSP_S32)bsp_sci_reg_event(SCI_EVENT_CARD_RESET_SUCCESS,pfunc);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reg_event CARD_RESET_SUCCESS fail,ret:%x\n",s32Ret);
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reg_event(SCI_EVENT_CARD_DEACTIVE_SUCCESS,pfunc);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reg_event CARD_DEACTIVE_SUCCESS fail,ret:%x\n",s32Ret);
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reg_event(SCI_EVENT_CARD_READ_DATA,pfunc);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reg_event CARD_READ_DATA fail,ret:%x\n",s32Ret);
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reg_event(SCI_EVENT_CARD_TX_ERR,pfunc);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reg_event CARD_TX_ERR fail,ret:%x\n",s32Ret);
		return ERROR;
	}
	return OK;
}

BSP_U32 sci_event_func_001(BSP_U32 u32Event,BSP_VOID* para)
{
	switch(u32Event)
	{
	case SCI_EVENT_CARD_DEACTIVE_SUCCESS:
		tickDeactiveSuccessEnd = sci_get_timeover();
		currentDeactiveSuccessTick = tickDeactiveSuccessEnd - tickDeactiveSuccessStart;
#if SCI_DEBUG_PRINT
		logMsg("SCI_EVENT_CARD_DEACTIVE_SUCCESS arise.\n",0,0,0,0,0,0);
#endif
		semGive(sciSemCardDeactiveSuccess);
		break;
	case SCI_EVENT_CARD_RESET_SUCCESS:
		tickResetSuccessEnd = sci_get_timeover();
		currentResetSuccessTick = tickResetSuccessEnd - tickResetSuccessStart;
#if SCI_DEBUG_PRINT
		logMsg("SCI_EVENT_CARD_RESET_SUCCESS arise.\n",0,0,0,0,0,0);
#endif
		semGive(sciSemCardResetSuccess);
		break;
	case SCI_EVENT_CARD_READ_DATA:
		//tickReadDataEnd = sci_get_timeover();
		//currentReadDataTick = tickReadDataEnd - tickReadDataStart;
#if SCI_DEBUG_PRINT
		logMsg("SCI_EVENT_CARD_READ_DATA arise.\n",0,0,0,0,0,0);
#endif
		//semGive(sciSemReadData);
		break;
	case SCI_EVENT_CARD_TX_ERR:
		logMsg("SCI_EVENT_CARD_TX_ERR arise.\n",0,0,0,0,0,0);
		semGive(sciSemTxErr);
		break;
	default:
		logMsg("Unknown EVENT arise.\n",0,0,0,0,0,0);
		break;
	}
	return OK;
}

BSP_S32 tmSciResetDeactive(BSP_U32 cardType)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 realClkStatus = 0;
	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
            printf("here:%d\n",__LINE__);
#endif

	if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
            printf("here:%d\n",__LINE__);
#endif

	if(tmSciATRCheck() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
            printf("here:%d\n",__LINE__);
#endif

	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);

	if(tmSciStatusCheck(SCI_CARD_STATE_BUSY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
        printf("here:%d\n",__LINE__);
#endif
	s32Ret = (BSP_S32)bsp_sci_get_clk_status(&realClkStatus);
	if(s32Ret != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_get_clk_status not fail,ret:%x.\n",s32Ret);
		return ERROR;
	}
#if SCI_DEBUG_PRINT
        printf("here:%d\n",__LINE__);
#endif

	if(tmSciSemDelete() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	
	return OK;
}

BSP_S32 tmSciVoltageCheck(BSP_S32 expectVoltage)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 realVoltage = 0;

	s32Ret = (BSP_S32)bsp_sci_show_vltg_val(&realVoltage);
	if(s32Ret != OK)
	{
		printf("bsp_sci_show_vltg_val fail,ret:%x\n",s32Ret);
		return ERROR;
	}
	if(realVoltage != (BSP_U32)expectVoltage)
	{
		printf("real voltage is not equal to expect.\n");
		printf("real:%d,expect:%d\n",realVoltage,expectVoltage);
		return ERROR;
	}
	return OK;
}
/*
testType: 0--->由1.8V切换到3.0V
: 1--->3.0V的前提下复位卡再获取电压值
: 2--->3.0V的前提下去激活卡再获取电压值
: 3--->3.0V的前提下再调用电压切换接口
: 4--->卡去激活后进行电压切换
: 5--->卡发送数据过程中进行电压切换
: 6--->卡预读数据过程中进行电压切换
: 7--->卡电压切换后接收数据
*/

BSP_S32 tmSciGetATR(BSP_U32 cardType,
					BSP_U32 timeStopCfg,
					BSP_U32 sendLen,
					BSP_U8* sendData,
					BSP_U32 preReadLen,
					BSP_U8* recvData,
					BSP_U8  expectATRLen,
					BSP_U8* expectATR)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 recvLen = 0;
	BSP_S32 s32Ret = 0;
	BSP_U8 atrLen = 0;
	BSP_U32 tmpLen = 255; // 256
	BSP_U8  tmpData[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 recvAllLen = 0;
    BSP_U32 realReLength = 0;
    SCI_ATRInfo stSCIATRInfo;
	
	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
	if(s32Ret != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	taskDelay(5);
	/*时钟停止后获取ATR*/
	if(tmSciATRCheck() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}

	s32Ret = (BSP_S32)bsp_sci_data_send(tmpLen,tmpData);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	/*发送数据后获取ATR*/
	if(tmSciATRCheck() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(150);
	//(BSP_VOID)semTake(sciSemReadData,150);
	(BSP_VOID)tmSciStatusPrint();
	s32Ret = (BSP_S32)bsp_sci_all_data_read(&recvAllLen,recvBuf);
	if(s32Ret != OK)
	{
		printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("recvAllLen:%d\n",recvAllLen);
	printf("recvBuf[0]:0x%x\n",recvBuf[0]);
	printf("recvBuf[1]:0x%x\n",recvBuf[1]);
#endif
	/*接收数据后获取ATR*/
	if(tmSciATRCheck() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&realReLength);
	if(s32Ret != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	memset((void*)recvBuf,0,preReadLen);
	if(sendData[sendLen-1] != realReLength) 
	{	
		preReadLen = realReLength+3;
		sendData[sendLen-1] = (BSP_U8)realReLength;
    }
	s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	//tickReadDataStart = sci_get_timestart();
	taskDelay(150);
	//(BSP_VOID)semTake(sciSemReadData,150);
	s32Ret = (BSP_S32)bsp_sci_all_data_read(&recvLen,recvBuf);
	if(s32Ret != OK)
	{
		printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	/*接收全部数据后获取ATR*/
	if(tmSciATRCheck() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((recvBuf[recvLen-1]!=0x0) || recvBuf[recvLen-2]!=0x90)
	{
		printf("read all data fail.\n");
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
/*	if(recvLen != preReadLen)
	{
		printf("after read data event,bsp_sci_all_data_read.\n");
		printf("recvlen is not equal to preReadLen.\n");
		printf("recvLen:%d,preReadLen:%d\n",recvLen,preReadLen);
		tmSciSemDelete();
		return ERROR;
	}
	if(sci_memcmp_ext((void*)recvData,(void*)recvBuf,recvLen) != OK)
	{
		tmSciSemDelete();
		return ERROR;
	}*/
	if(OK == tmSciVoltageCheck(SCI_CARD_VOLTAGE_180))
	{
	    s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
	    if(s32Ret != OK)
	    {
		    (BSP_VOID)tmSciSemDelete();
		    return ERROR;
	    }
	}
	/*电压切换后打印卡状态*/
    (BSP_VOID)tmSciStatusPrint();
	/*电压切换后获取ATR*/
	if(tmSciATRCheck() != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
//	semTake(sciSemCardResetSuccess,WAIT_FOREVER); 
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	taskDelay(20);
	/*去激活后等待20ms打印卡状态*/
	(BSP_VOID)tmSciStatusPrint();
	/*去激活后获取ATR*/
	s32Ret = (BSP_S32)bsp_sci_atr_get(&atrLen,recvBuf,&stSCIATRInfo);
	if(s32Ret != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_atr_get not fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}
/*
testType:0--->发送数据后复位卡
2--->复位卡后接收数据
3--->发送数据后去激活卡
5--->去激活卡后接收数据
6--->发送数据过程中再发送数据
7--->预读数据过程中再预读数据
*/
BSP_S32 tmSciResetDeactiveExtTest(BSP_S32 testType,
								  BSP_U32 cardType,
								  BSP_U32 sendLen,
								  BSP_U8* sendData,
								  BSP_U32 preReadLen,
								  BSP_U8* recvData)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 tmpLen = 255; //256
	BSP_U8 tmp_buf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 tmpSendLen = 255; // 256
	BSP_U8 tmp_send_buf[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_U32 recvlength = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif	
	switch(testType)
	{
	case 0:
		s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmp_send_buf);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 2:
		{
			s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&recvlength);
			if(s32Ret != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(sendData[sendLen-1] != recvlength) 
	        {	
		        preReadLen = recvlength+3;
		        sendData[sendLen-1] = (BSP_U8)recvlength;
            }
			s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
			if(s32Ret != OK)
			{
				printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
			taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
			if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
			if(s32Ret != BSP_ERR_SCI_NODATA)
			{
				printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		break;
	case 3:
		{
		    s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&recvlength);
			if(s32Ret != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(sendData[sendLen-1] != recvlength) 
	        {	
		        preReadLen = recvlength+3;
		        sendData[sendLen-1] = (BSP_U8)recvlength;
            }
			s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmp_send_buf);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = (BSP_S32)bsp_sci_deactive();
			if(s32Ret != BSP_ERR_SCI_NOTREADY)
			{
				printf("bsp_sci_deactive not fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
/*			if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}*/
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif	
			s32Ret = (BSP_S32)bsp_sci_all_data_read(&tmpLen,tmp_buf);
			if(s32Ret != OK)
			{
				printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		break;
	case 5:
#if 1
		{
			s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&recvlength);
			if(s32Ret != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
            if(sendData[sendLen-1] != recvlength) 
	        {	
		        preReadLen = recvlength+3;
		        sendData[sendLen-1] = (BSP_U8)recvlength;
            }
			s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = (BSP_S32)bsp_sci_deactive();
			if(s32Ret != OK)
			{
				printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
			if(tmSciStatusCheck(SCI_CARD_STATE_BUSY) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = bsp_sci_data_read_sync(tmpLen, tmp_buf);
			if(s32Ret != BSP_ERR_SCI_NOTINIT)
			{
				printf("BSP_SCI_Receive not fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
#endif
		break;
	case 6:
		{
			s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmp_send_buf);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmp_send_buf);
			if(s32Ret != BSP_ERR_SCI_NOTREADY)
			{
				printf("bsp_sci_data_send not fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			/*if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}*/
			s32Ret = (BSP_S32)bsp_sci_all_data_read(&tmpLen,tmp_buf);
			if(s32Ret != OK)
			{
				printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		break;
	case 7:
		{
			s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmp_send_buf);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			/*if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}*/
			s32Ret = (BSP_S32)bsp_sci_all_data_read(&tmpLen,tmp_buf);
			if(s32Ret != OK)
			{
				printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		break;
	default:
		printf("testType is not support.\n");
		return ERROR;
	}
#if 1
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	//taskDelay(200);
#endif
	s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&recvlength);
	if(s32Ret != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(sendData[sendLen-1] != recvlength) 
	{	
		preReadLen = recvlength+3;
		sendData[sendLen-1] = (BSP_U8)recvlength;
    }
	s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	//tickReadDataStart = sci_get_timestart();
	s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
	if(s32Ret != OK)
	{
		printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
/*	if(sci_memcmp_ext((void*)recvData,(void*)recvBuf,preReadLen) != OK)
	{
		tmSciSemDelete();
		return ERROR;
	}*/
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}
BSP_S32 tmSciRemoveAndInsertAfterDeactive(BSP_U32 cardType)
{
	BSP_S32 s32Ret = 0;
	BSP_U8  input_buf[10] = {0};

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	printf("please remove and insert card,then input any key to continue.\n");
	scanf("%s",input_buf);
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}

BSP_S32 tmSciNotInsertCardTest(void)
{
	BSP_U32 testLen = 10;
	BSP_U8  atrTestLen = 0;
	BSP_U8  testBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 clkStatus = 0;
	BSP_U32 event = 0;
	BSP_U32 volValue = 0;
	BSP_U32 s32Ret = 0;
    SCI_ATRInfo stSCIATRInfo;
	if((s32Ret = bsp_sci_reset(COLD_RESET)) != (BSP_U32)ERROR)
	{
		printf("1.bsp_sci_reset not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret =bsp_sci_atr_get(&atrTestLen,testBuf,&stSCIATRInfo)) != (BSP_U32)ERROR)
	{
		printf("2.bsp_sci_atr_get not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_data_send(testLen,testBuf)) != (BSP_U32)ERROR)
	{
		printf("3.bsp_sci_data_send not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_data_read_sync(testLen,testBuf)) != (BSP_U32)ERROR)
	{
		printf("5.BSP_SCI_Receive not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_all_data_read(&testLen,testBuf)) != (BSP_U32)ERROR)
	{
		printf("6.bsp_sci_all_data_read not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_vltg_class_switch()) != (BSP_U32)ERROR)
	{
		printf("7.bsp_sci_vltg_class_switch not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_show_vltg_val(&volValue)) != (BSP_U32)ERROR)
	{
		printf("8.bsp_sci_show_vltg_val not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = (BSP_U32)bsp_sci_clk_status_cfg(timeStopCfg)) != (BSP_U32)ERROR)
	{
		printf("9.bsp_sci_clk_status_cfg not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_deactive()) != (BSP_U32)ERROR)
	{
		printf("10.bsp_sci_deactive not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_get_clk_status(&clkStatus))!= (BSP_U32)ERROR)
	{
		printf("11.bsp_sci_deactive not fail.ret:%d\n",s32Ret);
	}
	if((s32Ret = bsp_sci_reg_event(event,sci_event_func_001))!= (BSP_U32)ERROR)
	{
		printf("12.bsp_sci_reg_event not fail.ret:%d\n",s32Ret);
	}
	return OK;
}

BSP_S32 tmSciInsertCardNotReset(void)
{
	BSP_U32 testLen = 10;
	BSP_U8  atrTestLen = 0;
	BSP_U8  testBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 timeStopCfg = SCI_CLK_STATE_HIGH;
	BSP_U32 clkStatus = 0;
	BSP_U32 volValue = 0;
	BSP_U32 s32Ret = 0;
    SCI_ATRInfo stSCIATRInfo;

	s32Ret =bsp_sci_atr_get(&atrTestLen,testBuf,&stSCIATRInfo);
	if(s32Ret != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_atr_get not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = bsp_sci_data_send(testLen,testBuf)) != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_data_send not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = (BSP_U32)bsp_sci_data_read_sync(testLen, testBuf)) != BSP_ERR_SCI_NOTINIT)
	{
		printf("BSP_SCI_Receive not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = bsp_sci_all_data_read(&testLen,testBuf)) != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_all_data_read not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = bsp_sci_vltg_class_switch()) != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_vltg_class_switch not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = bsp_sci_show_vltg_val(&volValue)) != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_show_vltg_val not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = (BSP_U32)bsp_sci_clk_status_cfg(timeStopCfg)) != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_clk_status_cfg not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = bsp_sci_deactive()) != BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_deactive not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	if((s32Ret = bsp_sci_get_clk_status(&clkStatus))!= BSP_ERR_SCI_NOTINIT)
	{
		printf("bsp_sci_deactive not fail.ret:%d\n",s32Ret);
		return ERROR;
	}
	return OK;
}
/*
* BSP_ERR_SCI_CURRENT_VLTG_ERR暂无法构造
*/
BSP_S32 tmSciErrorCodeExtTest(void)
{
	BSP_U32 testLen = 10;
	BSP_U8  atrTestLen = 0;
	BSP_U8  testBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 timeStopCfg = SCI_CLK_STATE_BUTT;
	BSP_S32 s32Ret = 0;
    SCI_ATRInfo stSCIATRInfo;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	if((s32Ret = (BSP_S32)bsp_sci_vltg_class_switch()) != BSP_ERR_SCI_CURRENT_STATE_ERR)
	{
		printf("bsp_sci_vltg_class_switch not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	if((s32Ret =(BSP_S32)bsp_sci_atr_get(NULL,testBuf,&stSCIATRInfo)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_atr_get not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	if((s32Ret =(BSP_S32)bsp_sci_atr_get(&atrTestLen,NULL,&stSCIATRInfo)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_atr_get not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
        printf("here:%d\n",__LINE__);
#endif

	if((s32Ret = (BSP_S32)bsp_sci_data_send(0,testBuf)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_data_send not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = (BSP_S32)bsp_sci_data_send(257,testBuf)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_data_send not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = (BSP_S32)bsp_sci_data_send(testLen,NULL)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_data_send not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = bsp_sci_data_read_sync(testLen+1, testBuf)) != BSP_ERR_SCI_NODATA)
	{
		printf("BSP_SCI_Receive not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	if((s32Ret = bsp_sci_data_read_sync(0, testBuf)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("BSP_SCI_Receive not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	if((s32Ret = bsp_sci_data_read_sync(testLen, NULL)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("BSP_SCI_Receive not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	if((s32Ret = (BSP_S32)bsp_sci_all_data_read(NULL,testBuf)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_all_data_read not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = (BSP_S32)bsp_sci_all_data_read(&testLen,NULL)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_all_data_read not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = (BSP_S32)bsp_sci_show_vltg_val(NULL)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_show_vltg_val not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = bsp_sci_clk_status_cfg(timeStopCfg)) != BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_clk_status_cfg not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = (BSP_S32)bsp_sci_get_clk_status(NULL))!= BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_deactive not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if((s32Ret = (BSP_S32)bsp_sci_reg_event(1,NULL))!= BSP_ERR_SCI_INVALIDPARA)
	{
		printf("bsp_sci_reg_event not fail.ret:%d\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}
/*
type:--->SCI_CARD_TYPE_USIM
USIM   白卡
1.unsigned char  ucSend0[5] = {0xa0,0xa4,0x00,0x00,0x02};//return 1
2.unsigned char  ucSend1[2] = {0x3f,0x00};//return 2
3.unsigned char  ucSend2[5] = {0xa0,0xc0,0x00,0x00,0x20};//return 61
完成上述3步的前2步，使卡处于发送接收的第3步
:--->SCI_CARD_TYPE_SIM
SIM    黄卡
*/
BSP_S32 tmSciSendRecvPrepare(BSP_U32 cardType)
{
	BSP_U32 realRecvLen = 0;
	BSP_U8 realRecvData[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 s32Ret = 0;
	BSP_U32 send0_len = 5;
	BSP_U8  send0[5] = {0xa0,0xa4,0x00,0x00,0x02};
	BSP_U32 recv0_len = 1;
//	BSP_U8  recv0[1] = {0xa4};
	BSP_U32 send1_len = 2;
	BSP_U8 send1[2] = {0x7f,0x20};
	BSP_U32 recv1_len = 2;
	BSP_S32 i = 0;

#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	(BSP_VOID)tmSciStatusPrint();
	printf("here:%d %d\n",__LINE__, send0_len);
	s32Ret = bsp_sci_data_send(send0_len,send0);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		return ERROR;
	}
	s32Ret = bsp_sci_data_read_sync(recv0_len,realRecvData);
    if(s32Ret != BSP_OK)
	{
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
		printf("ret code:0x%x\n",s32Ret);
#endif
		s32Ret = bsp_sci_all_data_read(&realRecvLen,realRecvData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
			return ERROR;
		}
		printf("realRecvLen:%d\n",realRecvLen);
		for(i = 0; i <(BSP_S32)realRecvLen; i++)
		{
			printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
		}
		return ERROR;
	}
    for(i = 0; i <(BSP_S32)recv0_len; i++)
		{
			printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
		}	
/*	if(sci_memcmp_ext((void*)realRecvData,(void*)recv0,recv0_len) != 0)
	{
		printf("recv0 data is not equal to expect.\n");
		return ERROR;
	}   */ //cacelled by yangzhi for universal test in 2010-11-11

	s32Ret = bsp_sci_data_send(send1_len,send1);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		return ERROR;
	}
    s32Ret = bsp_sci_data_read_sync(send1_len,realRecvData);
    if(s32Ret != OK)
	{
		s32Ret = bsp_sci_all_data_read(&realRecvLen,realRecvData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("realRecvLen:%d\n",realRecvLen);
#endif
		for(i = 0; i <(BSP_S32)realRecvLen; i++)
		{
			printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
		}
		return ERROR;
	}
	for(i = 0; i <(BSP_S32)recv1_len; i++)
		{
			printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
		}

	/*if(sci_memcmp_ext((void*)realRecvData,(void*)recv1,recv1_len) != 0)
	{
		printf("recv1 data is not equal to expect.\n");
		return ERROR;
	}*/ //cacelled by yangzhi for universal test in 2010-11-11
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	return OK;
}
BSP_S32 tmUSimSendRecvDataAndTimeStopPerfTest(BSP_BOOL classSwitch,BSP_U32 times)
{
	BSP_U32 i = 0;
	BSP_U32 sendLen = 5;
	BSP_U8 sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_U8* recvData = g_recvData;
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_U32 cardType = SCI_CARD_TYPE_USIM;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(classSwitch)
	{
		s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		if(s32Ret != OK)
		{
			printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_300) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	for(i = 0; i < times; i++)
	{
		printf("#################round:%u#################\n",i);
		s32Ret = bsp_sci_clk_status_cfg(SCI_CLK_STATE_HIGH);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(10);
		s32Ret = tmSciSendRecvPrepare(cardType);
		if(s32Ret != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = bsp_sci_clk_status_cfg(SCI_CLK_STATE_HIGH);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(10);
		s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)tmSciStatusPrint();
		//tickReadDataStart = sci_get_timestart();

		s32Ret = (BSP_S32)bsp_sci_data_read_sync(preReadLen,recvBuf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}

		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(sci_memcmp_ext((void*)recvData,(void*)recvBuf,(BSP_S32)preReadLen) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
	}
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}
/*
PV400测试结果
-> tmUSimResetPerfTest 100
复位平均时间为:42766714(us)
value = 0 = 0x0
-> tmUSimResetDeactivePerfTest 100
复位去激活平均时间为:42681517(us)
value = 0 = 0x0
*/
BSP_S32 tmUSimResetPerfTest(BSP_U32 times)
{
	BSP_U32 i = 0;
	BSP_S32 s32Ret = 0;
	BSP_S32 totalTicks = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	for(i = 0; i < times; i++)
	{
		tickResetSuccessStart = sci_get_timestart();
		s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		totalTicks += (BSP_S32)currentResetSuccessTick;
	}
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	printf("复位平均时间为:%d(us)\n",(totalTicks/(BSP_S32)USR_CLK_FREQ)/(BSP_S32)times);
	return OK;
}
BSP_S32 tmUSimResetDeactivePerfTest(BSP_U32 times)
{
	BSP_U32 i = 0;
	BSP_S32 s32Ret = 0;
	BSP_S32 totalTicks = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	for(i = 0; i < times; i++)
	{
		tickResetSuccessStart = sci_get_timestart();
		s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		tickDeactiveSuccessStart = sci_get_timestart();
		s32Ret = (BSP_S32)bsp_sci_deactive();
		if(s32Ret != OK)
		{
			printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
		totalTicks += (BSP_S32)(currentResetSuccessTick + currentDeactiveSuccessTick);
	}
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	printf("复位去激活平均时间为:%d(us)\n",(totalTicks/(BSP_S32)USR_CLK_FREQ)/(BSP_S32)times);
	return OK;
}

/*************************************************************************************
*
* tstSciUSimSendRecvDataTest - tstSciUSimSendRecvDataTest
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUSimSendRecvDataTest(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8 sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 recvLen = 37;
	BSP_S32 waitTicks = 0;

	if(tmSciSendRecvData(SCI_CARD_TYPE_USIM,sendLen,sendData,recvLen,g_recvData,waitTicks) != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;
}

BSP_S32 tmSciSendRecvData(BSP_U32 cardType,
						  BSP_U32 sendLen,
						  BSP_U8* sendData,
						  BSP_U32 preReadLen,
						  BSP_U8* recvData,
						  BSP_S32 waitTicks/*wait time between send and preread*/)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_S32 i = 0;
	BSP_S32 recvlength = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
//	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
//	taskDelay(1);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = tmSciSendRecvPrepareWithReLength(cardType,(BSP_U32 *)&recvlength);
	if(s32Ret != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
    printf("recvlength:%d\n",*(&recvlength));
#endif
    if(sendData[sendLen-1] != recvlength) 
	{	
		preReadLen = (BSP_U32)recvlength+3;
		sendData[sendLen-1] = (BSP_U8)recvlength;
    }
	s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(waitTicks);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen after recvprpare:%d\n",preReadLen);
#endif
    s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
	if(s32Ret != BSP_OK)
	{
		printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	for(i =0; i <(BSP_S32)preReadLen ; i++)
	{
		printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	}
#endif

/*	if(sci_memcmp_ext((void*)recvData,(void*)recvBuf,preReadLen) != OK)
	{
		tmSciSemDelete();
		return ERROR;
	}*/ //cacelled by yangzhi for universal card test
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",preReadLen);
#endif
	if((0x90 != recvBuf[preReadLen-2]) || (0 != recvBuf[preReadLen-1]))
	{
	    (BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
            printf("here:%d\n",__LINE__);
#endif
    taskDelay(SCI_RESET_NORMAL_DELAY_TIME);

	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}


/*
testType:0--->停止时钟后发送数据
1--->时钟停止后接收数据
2--->停止时钟后复位卡
3--->停止时钟后去激活卡
4--->停止时钟情况下，电压切换
5--->卡不支持停止时钟操作，停止时钟
6--->发送过程中时钟停止
7--->时钟停止后再时钟停止
*/
BSP_S32 tmSciClkStopTest(BSP_S32 testType,
						 BSP_U32 cardType,
						 BSP_S32 cardVoltage,
						 BSP_U32 timeStopCfg,
						 BSP_U32 sendLen,
						 BSP_U8* sendData,
						 BSP_U32 preReadLen,
						 BSP_U8* recvData,
						 BSP_S32 waitTicks/*wait time between send and preread*/)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_U32 tmpSendLen = 256;
	BSP_U8  tmpSendData[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 tmpRecvLen = 0;
	BSP_U32 realReLength = 0;
//	int i;

	if(tmSciSemCreate() != OK)
	{
        printf("line 4010\n");
        return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
        printf("line 4015\n");
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
        printf("line 4030\n");
		return ERROR;
	}
	switch(testType)
	{
	case 0:
		{
			BSP_U32 realVolValue = 0;
			BSP_U32 i = 0;
			/*check voltage right for test*/
			s32Ret = (BSP_S32)bsp_sci_show_vltg_val(&realVolValue);
			if(s32Ret != OK)
			{
				printf("bsp_sci_show_vltg_val fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(realVolValue != (BSP_U32)cardVoltage)
			{
			    #if SCI_DEBUG_PRINT
			        printf("here:%d\n",__LINE__);
                #endif
				if(cardVoltage == SCI_CARD_VOLTAGE_300)
				{
					s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
					if(s32Ret != OK)
					{
						printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
						(BSP_VOID)tmSciSemDelete();
						return ERROR;
					}
					(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
					taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
				}
				else
				{
					printf("current voltage is SCI_CARD_VOLTAGE_300.\n");
					printf("please use other card support SCI_CARD_VOLTAGE_180.\n");
					(BSP_VOID)tmSciSemDelete();
					//return ERROR;
					return OK;
				}
			}
			s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
			if(s32Ret != OK)
			{
				printf("bsp_sci_clk_status_cfg fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			taskDelay(5);
#if SCI_DEBUG_PRINT
			printf("here:%d\n",__LINE__);
#endif
			if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&realReLength);
			if(s32Ret != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(sendData[sendLen-1] != realReLength) 
	        {	
		        preReadLen = realReLength+3;
		        sendData[sendLen-1] = (BSP_U8)realReLength;
            }
			s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			taskDelay(waitTicks);
			//tickReadDataStart = sci_get_timestart();
			s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
			for(i=0; i<preReadLen; i++)
			{
			    printf("recvBuf 0x%x\n",recvBuf[i]);
			}
#if SCI_DEBUG_PRINT
			printf("here:%d\n",__LINE__);
#endif
		}
		break;
	case 1:
		{
			s32Ret = tmSciSendRecvPrepareWithReLength(cardType,&realReLength);
			if(s32Ret != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			if(sendData[sendLen-1] != realReLength) 
	        {	
		        preReadLen = realReLength+3;
		        sendData[sendLen-1] = (BSP_U8)realReLength;
            }
			s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
			if(s32Ret != OK)
			{
				printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			(BSP_VOID)tmSciStatusPrint();
			taskDelay(waitTicks);
			//tickReadDataStart = sci_get_timestart();
			
			s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
			if(s32Ret != OK)
			{
				printf("bsp_sci_clk_status_cfg fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			taskDelay(5);
			if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			taskDelay(5);
#if SCI_DEBUG_PRINT
			printf("here:%d\n",__LINE__);
#endif
            s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
			if(s32Ret != BSP_ERR_SCI_NODATA)
			{
				printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		break;
	case 2:
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != OK)
		{
			printf("bsp_sci_clk_status_cfg fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		taskDelay(100);
		if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		break;
	case 3:
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != OK)
		{
			printf("bsp_sci_clk_status_cfg fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(5);
		if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_deactive();
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
		if(tmSciStatusCheck(SCI_CARD_STATE_BUSY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 4:
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_180) != OK)
		{
			printf("current voltage is not 1.8V.\n");
			printf("can't excute classSwitch.\n");
			printf("now reset card for test.\n");
			s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
			if(s32Ret != OK)
			{
				printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
			taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		}
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != OK)
		{
			printf("bsp_sci_clk_status_cfg fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(5);
		if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(OK == tmSciVoltageCheck(SCI_CARD_VOLTAGE_180)){
		    s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		    if(s32Ret != OK)
		    {
			    printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			    (BSP_VOID)tmSciSemDelete();
			    return ERROR;
		    }
		}
//		semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_300) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(5);
		if(OK == tmSciVoltageCheck(SCI_CARD_VOLTAGE_180))
		{
			if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
			{
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		break;
	case 5:
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != BSP_ERR_SCI_UNSUPPORTED)
		{
			printf("bsp_sci_clk_status_cfg not fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(waitTicks);
		//tickReadDataStart = sci_get_timestart();
		
		s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(sci_memcmp_ext((void*)recvData,(void*)recvBuf,(BSP_S32)preReadLen) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 6:
		s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmpSendData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != BSP_ERR_SCI_NOTREADY)
		{
			printf("bsp_sci_clk_status_cfg not fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(tmSciClkStatusCheck(SCI_CARD_CLK_OPEN) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		/*if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}*/
		
        //taskDelay(150);
		//(BSP_VOID)semTake(sciSemReadData,150);
		s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
		if(s32Ret != BSP_ERR_SCI_NODATA)
		{
		   printf("bsp_sci_data_read_sync  test fail ,ret:%x\n",s32Ret);
		}
		s32Ret = (BSP_S32)bsp_sci_all_data_read(&tmpRecvLen,recvBuf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
#if 0

		//printf("tmpRecvLen = %d\n",tmpRecvLen);
		if(tmSciStatusCheck(cardType,SCI_CARD_STATUS_CARD_READY) != OK)
		{
			tmSciSemDelete();
			return ERROR;
		}
		if(sci_memcmp_ext((void*)tmpExpRecvData,(void*)recvBuf,tmpRecvLen) != OK)
		{
			tmSciSemDelete();
			return ERROR;
		} 
#endif
		break;
	case 7:
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != OK)
		{
			printf("bsp_sci_clk_status_cfg fail firstly,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(5);
		if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = bsp_sci_clk_status_cfg(timeStopCfg);
		if(s32Ret != OK)
		{
			printf("bsp_sci_clk_status_cfg fail secondly,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(5);
		if(tmSciClkStatusCheck(SCI_CARD_CLK_STOP) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	default:
		printf("testType is not support.\n");
		return ERROR;
	}
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}

BSP_S32 tmSciGeneralSendRecvData(BSP_U32 cardType,
								 BSP_U32  sendLen,
								 BSP_U8*  sendData,
								 BSP_S32  waitTicks/*wait time between send and preread*/,
								 BSP_U32  preReadLen,
								 BSP_BOOL recvOnce,
								 BSP_S32  expectRecvLen,
								 BSP_U8*  expectRecvData,
								 BSP_S32  expectRecvTicks)
{
	BSP_U8 realRecvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_S32 i = 0;
	BSP_S32 realrecvlength = 0;
	// 20130306--Parity Error Detect Modify Begin:
	BSP_S32 u32ParityValue = 0;
	// 20130306--Parity Error Detect Modify End!

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = tmSciSendRecvPrepareWithReLength(cardType,(BSP_U32 *)&realrecvlength);
	if(s32Ret != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	} 
	if(sendData[sendLen-1] != realrecvlength)  // added by yangzhi for universal test
	{	
		expectRecvLen = realrecvlength+3;
		sendData[sendLen-1] = (BSP_U8)realrecvlength;
    }
	s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay(waitTicks);
	//tickReadDataStart = sci_get_timestart();
#if SCI_DEBUG_PRINT
	printf("preReadLen:%d\n",preReadLen);
#endif
	if(preReadLen == 0)
	{
		BSP_U32 realLen = 0;

        if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
        
		s32Ret = bsp_sci_data_read_sync(preReadLen, realRecvBuf);
		if(s32Ret != BSP_ERR_SCI_INVALIDPARA)
		{
		    printf("bsp_sci_data_read_sync fail,ret:%x\n",s32Ret);
		}
		
        //taskDelay(expectRecvTicks);
		(BSP_VOID)semTake(sciSemReadData,expectRecvTicks);
		s32Ret = (BSP_S32)bsp_sci_all_data_read(&realLen,realRecvBuf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_ReadAll fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(realLen != (BSP_U32)expectRecvLen)
		{
			printf("realLen is not equal to expectRecvLen.\n");
			printf("realLen:%d,expectRecvLen:%d\n",realLen,expectRecvLen);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
	}
	else if(preReadLen == (BSP_U32)expectRecvLen)
	{
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(recvOnce)
		{
			s32Ret = bsp_sci_data_read_sync((BSP_U32)expectRecvLen, realRecvBuf);
			if(s32Ret != OK)
			{
				printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
		}
		else
		{
			//BSP_U32 firstLen = (BSP_U32)(expectRecvLen*0.618);
            BSP_S32 firstLen = 7;

			s32Ret = bsp_sci_data_read_sync(firstLen, realRecvBuf);
			if(s32Ret != BSP_OK)
			{
				printf("BSP_SCI_Receive not fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			#if SCI_DEBUG_PRINT
	        for(i =0; i <(BSP_S32)firstLen ; i++)
	        {
		        printf("realRecvData[%d]:%x\n",i,realRecvBuf[i]);
	        }
            #endif
			// 20130306--Parity Error Detect Modify Begin:
			if(TRUE == bParityDetectFlag)
			{
				/*
				u32ParityValue = apBIT_GET(SCI_REG_CTRL0, PL131_REGCTRL0_RX);
				if(0==u32ParityValue)
				{
					apBIT_SET(SCI_REG_CTRL0, PL131_REGCTRL0_RX, 1);
				}
				else
				{
					apBIT_SET(SCI_REG_CTRL0, PL131_REGCTRL0_RX, 0);
				}*/
				
				u32ParityValue = BSP_REG_GETBITS(SCI_REG_CTRL0, 0, 4, 1);
				if(0==u32ParityValue)
				{
					BSP_REG_SETBITS(SCI_REG_CTRL0, 0, 4, 1, 1);
				}
				else
				{
					BSP_REG_SETBITS(SCI_REG_CTRL0, 0, 4, 1, 0);
				}
				
				s32Ret = bsp_sci_data_read_sync((BSP_U32)expectRecvLen-firstLen, realRecvBuf+firstLen);
				if(s32Ret == OK)
				{
					printf("BSP_SCI_Receive Parity teset fail,ret:%x\n",s32Ret);
					(BSP_VOID)tmSciSemDelete();
					u32ParityValue = BSP_REG_GETBITS(SCI_REG_CTRL0, 0, 4, 1);
					if(0==u32ParityValue)
					{
						BSP_REG_SETBITS(SCI_REG_CTRL0, 0, 4, 1, 1);
					}
					else
					{
						BSP_REG_SETBITS(SCI_REG_CTRL0, 0, 4, 1, 0);
					}
					return ERROR;
				}
				u32ParityValue = BSP_REG_GETBITS(SCI_REG_CTRL0, 0, 4, 1);
				if(0==u32ParityValue)
				{
					BSP_REG_SETBITS(SCI_REG_CTRL0, 0, 4, 1, 1);
				}
				else
				{
					BSP_REG_SETBITS(SCI_REG_CTRL0, 0, 4, 1, 0);
				}
			}
			else
			{
			s32Ret = bsp_sci_data_read_sync((BSP_U32)expectRecvLen-firstLen, realRecvBuf+firstLen);
			if(s32Ret != OK)
			{
				printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
				(BSP_VOID)tmSciSemDelete();
				return ERROR;
			}
			#if SCI_DEBUG_PRINT
	    for(i =0; i <expectRecvLen ; i++)
	    {
		    printf("realRecvData[%d]:%x\n",i,realRecvBuf[i]);
	    }
        #endif
			}
			// 20130306--Parity Error Detect Modify End!
		}
	}
	else if((BSP_S32)preReadLen < expectRecvLen)
	{
		BSP_U8 tmp_buf[SCI_DATA_MAX_LEN] = {0};
		BSP_U32 tmp_len = 0;
		BSP_U32 secLen = (BSP_U32)expectRecvLen - preReadLen;

		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("preReadLen:%d,buf:%x\n",preReadLen,(BSP_S32)tmp_buf);
#endif
		s32Ret = bsp_sci_data_read_sync(preReadLen, tmp_buf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		//s32Ret = semTake(sciSemReadData,expectRecvTicks*(1-(BSP_S32)ratio));
		/*errorNo = errnoGet();
        printf(" s32Ret is %d\n",s32Ret);
		if(errorNo != S_objLib_OBJ_TIMEOUT) //(s32Ret != ERROR)||
		{
			printf("semTake not Error,errorNo:%x\n",errorNo);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}*/
		memset((void*)realRecvBuf,0,SCI_DATA_MAX_LEN);
		memcpy((void*)realRecvBuf,(void*)tmp_buf,preReadLen);
		#if SCI_DEBUG_PRINT
	    for(i =0; i <(BSP_S32)preReadLen ; i++)
	    {
		    printf("realRecvData[%d]:%x\n",i,realRecvBuf[i]);
	    }
        #endif
		if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		taskDelay(20);
		memset((void*)tmp_buf,0,preReadLen);
		s32Ret = (BSP_S32)bsp_sci_all_data_read(&tmp_len,tmp_buf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(secLen != tmp_len)
		{
			printf("seclen:%d, tmp_len:%d.\n",secLen,tmp_len);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		memcpy((void*)(realRecvBuf + preReadLen),(void*)tmp_buf,secLen);
		#if SCI_DEBUG_PRINT
	    for(i =0; i <expectRecvLen ; i++)
	    {
		    printf("realRecvData[%d]:%x\n",i,realRecvBuf[i]);
	    }
        #endif
	}
	else if((BSP_S32)preReadLen > expectRecvLen)
	{
		int localErrno = 0;
		int status = 0;
		BSP_U32 realLen = 0;

		status = semTake(sciSemReadData,expectRecvTicks);
		localErrno = errnoGet();
		if((status != ERROR)||(localErrno != S_objLib_OBJ_TIMEOUT))
		{
			printf("semTake not timeout,ret:%x,errno:%x\n",status,localErrno);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_all_data_read(&realLen,realRecvBuf);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_ReadAll fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		if(realLen != (BSP_U32)expectRecvLen)
		{
			printf("realLen is not equal to expectRecvLen.\n");
			printf("realLen:%d,expectRecvLen:%d\n",realLen,expectRecvLen);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
	}
	else
	{
		printf("preReadLen(=%d) is invalid.\n",preReadLen);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	/*if(sci_memcmp_ext((void*)expectRecvData,(void*)realRecvBuf,expectRecvLen) != OK)
	{
		tmSciSemDelete();
		return ERROR;
	}*/ //cacelled by yangzhi for universal card test
#if SCI_DEBUG_PRINT
	printf("expectRecvLen:%d\n",expectRecvLen); // modified by yangzhi for universal test in 11.12
#endif
    if(expectRecvLen >=2)
    {
		if((0x90 != realRecvBuf[expectRecvLen-2]) || (0 != realRecvBuf[expectRecvLen-1]))
		{
		    (BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
    }
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}

BSP_S32 tmSciSendMaxLenData(BSP_U32 cardType,
							BSP_U32 sendLen,
							BSP_U8* sendData,
							BSP_U32 delayTime)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 recvLen = 0;
	BSP_S32 s32Ret = 0;
	BSP_S32 i = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)tmSciStatusPrint();
	taskDelay((BSP_S32)delayTime);
	s32Ret = (BSP_S32)bsp_sci_all_data_read(&recvLen,recvBuf);
	if(s32Ret != OK)
	{
		printf("BSP_SCI_Receive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
/*	#if SCI_DEBUG_PRINT  //cacelled by yangzhi in 2010-11-12
	    for(i =0; i <37 ; i++)
	    {
		    printf("realRecvData[%d]:%x\n",i,recvBuf[i]);
	    }
    #endif*/
	(BSP_VOID)tmSciStatusPrint();
	if(recvLen != 0)
	{
		printf("send max len cmd(a invalid cmd),reclen:%d.\n",recvLen);
		for(i =0; i < (BSP_S32)recvLen; i++)
		{
			printf("recvBuf[%d]:0x%x\n",i,recvBuf[i]);
		}
		//tmSciSemDelete();
		//return ERROR;
	}
	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}

BSP_S32 tmSciChangeVoltage(BSP_U32 cardType,BSP_U32 testType)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 sendLen = 5;
	BSP_U8 sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_U32 preReadLen = 37;
	BSP_U8* recvData = g_recvData;
	BSP_U8  realData[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 tmpSendLen = 256;
	BSP_U8  tmpSendData[SCI_DATA_MAX_LEN] = {0};
	BSP_U32 voltageVal = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
   if((testType <= 3) && (OK == tmSciVoltageCheck(SCI_CARD_VOLTAGE_180)))
	{
		s32Ret =(BSP_S32)bsp_sci_vltg_class_switch();
		if(s32Ret != OK)
		{
			printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif   
	switch(testType)
	{
	case 0:
		//semTake(sciSemCardResetSuccess,WAIT_FOREVER);luting
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
#endif
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_300) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 1:
/*		s32Ret = bsp_sci_vltg_class_switch();
		if(s32Ret != OK)
		{
			printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			tmSciSemDelete();
			return ERROR;
		}*/
		//semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_180) != OK)
		{
            if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_300) != OK)
            {
			    (BSP_VOID)tmSciSemDelete();
			    return ERROR;
            }
		}
		break;
	case 2:
/*		s32Ret = bsp_sci_vltg_class_switch();
		if(s32Ret != OK)
		{
			printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			tmSciSemDelete();
			return ERROR;
		}*/
		//semTake(sciSemCardResetSuccess,WAIT_FOREVER); cacelled by yangzhi in 2010-11-15
printf("here:%d\n",__LINE__);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
printf("here:%d\n",__LINE__);
		s32Ret = (BSP_S32)bsp_sci_deactive();
		if(s32Ret != OK)
		{
			printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
		{
			BSP_U32 realVoltage = 0;

			s32Ret = (BSP_S32)bsp_sci_show_vltg_val(&realVoltage);
			if(s32Ret != BSP_ERR_SCI_NOTINIT)
			{
				printf("bsp_sci_show_vltg_val not fail,ret:%x\n",s32Ret);
				return ERROR;
			}
		}
		return OK;
	case 3:
/*		s32Ret = bsp_sci_vltg_class_switch();
		if(s32Ret != OK)
		{
			printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			tmSciSemDelete();
			return ERROR;
		}*/
		//semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_300) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		/*if(s32Ret != BSP_ERR_SCI_VLTG_HIGHEST)
		{
			printf("already 3V invoke classSwitch not fail.ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}*/
		if(s32Ret != 1)
		{
			printf("already 3V invoke classSwitch not fail.ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 4:
		s32Ret = (BSP_S32)bsp_sci_deactive();
#if SCI_DEBUG_PRINT
		printf("here:%d,after deactive\n",__LINE__);
#endif		
		(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	    if(tmSciSemDelete() != OK)
	    {
		    return ERROR;
	    }
		s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		if(s32Ret != BSP_ERR_SCI_NOTINIT)
		{
			printf("bsp_sci_deactive invoke classSwitch not fail.\n");
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		return OK;
	case 5:
		s32Ret = (BSP_S32)bsp_sci_show_vltg_val(&voltageVal);
		if(s32Ret != OK)
		{
			printf("bsp_sci_show_vltg_val fail,ret:%x\n",s32Ret);
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmpSendData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)tmSciStatusPrint();
		s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		if(s32Ret != BSP_ERR_SCI_NOTREADY)
		{
			printf("BUSY invoke classSwitch not fail.\n");
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,150);
		if(tmSciVoltageCheck((BSP_S32)voltageVal) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 6:
		s32Ret = (BSP_S32)bsp_sci_show_vltg_val(&voltageVal);
		if(s32Ret != OK)
		{
			printf("bsp_sci_show_vltg_val fail,ret:%x\n",s32Ret);
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_data_send(tmpSendLen,tmpSendData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}

		(BSP_VOID)tmSciStatusPrint();
		s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		if(s32Ret != BSP_ERR_SCI_NOTREADY)
		{
			printf("BUSY invoke classSwitch not fail.\n");
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,150);
		if(tmSciVoltageCheck((BSP_S32)voltageVal) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	case 7:
		s32Ret = tmSciSendRecvPrepare(cardType);
		if(s32Ret != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_data_read_sync(preReadLen,realData);
		if(s32Ret != OK)
		{
			printf("BSP_SCI_PreReceive fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		s32Ret = (BSP_S32)bsp_sci_vltg_class_switch();
		if(s32Ret != OK)
		{
			printf("bsp_sci_vltg_class_switch fail,ret:%x\n",s32Ret);
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);
		taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
		if(tmSciVoltageCheck(SCI_CARD_VOLTAGE_300) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}

		if(sci_memcmp_ext((void*)recvData,(void*)realData,(BSP_S32)preReadLen) != OK)
		{
			(BSP_VOID)tmSciSemDelete();
			return ERROR;
		}
		break;
	default:
		printf("testType is not support.\n");
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}
	return OK;
}
//
BSP_S32 tmSciSendRecvPrepareWithReLength(BSP_U32 cardType, BSP_U32* relength)
{
	BSP_U32 realRecvLen = 0;
	BSP_U8 realRecvData[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_U32 send0_len = 5;
	BSP_U8 send0[5] = {0xa0,0xa4,0x00,0x00,0x02};
	BSP_U32 recv0_len = 1;
//	BSP_U8 recv0[1] = {0xa4};
	BSP_U32 send1_len = 2;
	BSP_U8 send1[2] = {0x7f,0x20};
	BSP_U32 recv1_len = 2;
	BSP_S32 i = 0;
	BSP_S32 recv1length = 0;

#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	(BSP_VOID)tmSciStatusPrint();
	printf("here:%d %d\n",__LINE__, send0_len);
	s32Ret = (BSP_S32)bsp_sci_data_send(send0_len,send0);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		return ERROR;
	}
//	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
//	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
	s32Ret = (BSP_S32)bsp_sci_data_read_sync(recv0_len,realRecvData);
    if(s32Ret != BSP_OK)
	{
#if SCI_DEBUG_PRINT
		printf("here:%d\n",__LINE__);
		printf("ret code:0x%x\n",s32Ret);
#endif
		s32Ret = (BSP_S32)bsp_sci_all_data_read(&realRecvLen,realRecvData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
			return ERROR;
		}
		printf("realRecvLen:%d\n",realRecvLen);
		for(i = 0; i <(BSP_S32)realRecvLen; i++)
		{
			printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
		}
		return ERROR;
	}
    for(i = 0; i <(BSP_S32)recv0_len; i++)
	{
		printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
	}	
/*	if(sci_memcmp_ext((void*)realRecvData,(void*)recv0,recv0_len) != 0)
	{
		printf("recv0 data is not equal to expect.\n");
		return ERROR;
	}   */ //cacelled by yangzhi for universal test in 2010-11-11

	s32Ret = (BSP_S32)bsp_sci_data_send(send1_len,send1);
	if(s32Ret != OK)
	{
		printf("bsp_sci_data_send fail,ret:%x\n",s32Ret);
		return ERROR;
	}
//	taskDelay(SCI_RESET_NORMAL_DELAY_TIME);
    s32Ret = (BSP_S32)bsp_sci_data_read_sync(recv1_len,realRecvData);
    if(s32Ret != OK)
	{
		s32Ret = (BSP_S32)bsp_sci_all_data_read(&realRecvLen,realRecvData);
		if(s32Ret != OK)
		{
			printf("bsp_sci_all_data_read fail,ret:%x\n",s32Ret);
			return ERROR;
		}
#if SCI_DEBUG_PRINT
		printf("realRecvLen:%d\n",realRecvLen);
#endif
		for(i = 0; i <(BSP_S32)realRecvLen; i++)
		{
			printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
		}
		return ERROR;
	}
	for(i = 0; i <(BSP_S32)recv1_len; i++)
	{
		printf("realRecvData[%d]:0x%x\n",i,realRecvData[i]);
	}
	recv1length = realRecvData[recv1_len-1];
#if SCI_DEBUG_PRINT
		printf("recv1length:%d\n",recv1length);
#endif	
/*    if(recv1[1] != realRecvData[recv1_len-1])
	{
		printf("real recv length for GET REPONSE is:0x%x\n",recv1length);
		*relength = recv1length;
	}*/
	*relength = (BSP_U32)recv1length;
	/*if(sci_memcmp_ext((void*)realRecvData,(void*)recv1,recv1_len) != 0)
	{
		printf("recv1 data is not equal to expect.\n");
		return ERROR;
	}*/ //cacelled by yangzhi for universal test in 2010-11-11
#if SCI_DEBUG_PRINT
	printf("here:%d\n",__LINE__);
#endif
	return OK;
}
//
BSP_S32 tmSciEnd()
{
	BSP_S32 s32Ret = 0;

    taskDelay(SCI_RESET_NORMAL_DELAY_TIME);

	tickDeactiveSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_deactive();
	if(s32Ret != OK)
	{
		printf("bsp_sci_deactive fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardDeactiveSuccess,WAIT_FOREVER);
	if(tmSciSemDelete() != OK)
	{
		return ERROR;
	}

    return OK;
}
//
BSP_S32 tmSciSelectFile(BSP_U8* cmd, BSP_U8* fid)
{
//	BSP_U32 realRecvLen = 0;
	BSP_U8  realRecvData[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_U8  recv0[1] = {0xa4};
	BSP_U8  recv1[1] = {0x61};
	BSP_U8  send1[5] = {0x00,0xc0,0x00,0x00,0x00};
	BSP_U32 i = 0;

    SCI_PRINT_DATA(5, cmd);
	s32Ret = (BSP_S32)bsp_sci_data_send(5, cmd);
	if(s32Ret != OK)
	{
		printf("(%s)%d: bsp_sci_data_send fail,ret:%x\n",__FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}
    
	s32Ret = (BSP_S32)bsp_sci_data_read_sync(1, realRecvData);
    if(s32Ret != BSP_OK)
	{
        SCI_RECV_ALL();
		return ERROR;
	}
    SCI_PRINT_DATA(1, realRecvData);
    SCI_CHECK_DATA(realRecvData, recv0, 1);
    
	s32Ret = (BSP_S32)bsp_sci_data_send(cmd[4], fid);
	if(s32Ret != OK)
	{
		printf("(%s)%d: bsp_sci_data_send fail,ret:%x\n",__FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}
    s32Ret = (BSP_S32)bsp_sci_data_read_sync(2, realRecvData);
    if(s32Ret != OK)
	{
        SCI_RECV_ALL();
		return ERROR;
	}
    SCI_PRINT_DATA(2, realRecvData);
    SCI_CHECK_DATA(realRecvData, recv1, 1);

    if(SCI_CARD_TYPE_SIM == g_cardtype)
    {
        send1[0] = 0xa0;
    }
    send1[4]=realRecvData[1];
    g_curFCPlen = send1[4];
	s32Ret = (BSP_S32)bsp_sci_data_send(5, send1);
	if(s32Ret != OK)
	{
		printf("(%s)%d: bsp_sci_data_send fail,ret:%x\n",__FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}
    
    s32Ret = (BSP_S32)bsp_sci_data_read_sync(send1[4]+3, realRecvData);
    if(s32Ret != OK)
	{
        SCI_RECV_ALL();
		return ERROR;
	}
    SCI_PRINT_DATA((send1[4]+3), realRecvData);

	return OK;
}
BSP_S32 tmSciReadFile(BSP_U32 FileType, BSP_U8* fid, BSP_U8 record, BSP_U8 *pLen, BSP_U8 *data)
{
//	BSP_U32 realRecvLen = 0;
	BSP_U8  realRecvData[SCI_DATA_MAX_LEN] = {0};
	BSP_S32 s32Ret = 0;
	BSP_U8  selectCmd[5] = {0x00,0xa4,0x00,0x04,0x02};////p2 from 0x0 to 0x04
	BSP_U8  read0[5] = {0x00,0xb0,0x00,0x00,0x00};
	BSP_U8  recv1[1] = {0x6c};
	BSP_U8  recv2[2] = {0x90, 0x00};
	BSP_U32 i = 0;

    if(SCI_CARD_TYPE_SIM == g_cardtype)
    {
        selectCmd[0] = 0xa0;
        selectCmd[3] = 0x04;
    }
	s32Ret = tmSciSelectFile(selectCmd, fid);
	if(s32Ret != OK)
	{
		printf("(%s)%d: tmSciSelectFile fail,ret:%x\n",__FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}

    if(SCI_CARD_TYPE_SIM == g_cardtype)
    {
        read0[0] = 0xa0;
    }
    if(SCI_FILE_TYPE_BINARY != FileType)
    {
        read0[1] = 0xb2;
        read0[2] = record;
        read0[3] = 0x04;
    }
	s32Ret = (BSP_S32)bsp_sci_data_send(5, read0);
	if(s32Ret != OK)
	{
		printf("(%s)%d: bsp_sci_data_send fail,ret:%x\n",__FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}
    
	s32Ret = (BSP_S32)bsp_sci_data_read_sync(2, realRecvData);
    if(s32Ret != BSP_OK)
	{
        SCI_RECV_ALL();
		return ERROR;
	}
    SCI_PRINT_DATA(2, realRecvData);
    SCI_CHECK_DATA(realRecvData, recv1, 1);

    read0[4] = realRecvData[1];
    *pLen = read0[4];
	s32Ret = (BSP_S32)bsp_sci_data_send(5, read0);
	if(s32Ret != OK)
	{
		printf("(%s)%d: bsp_sci_data_send fail,ret:%x\n",__FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}
    
    s32Ret = (BSP_S32)bsp_sci_data_read_sync(read0[4]+3, realRecvData);
    if(s32Ret != OK)
	{
        SCI_RECV_ALL();
		return ERROR;
	}
    SCI_PRINT_DATA((read0[4]+3), realRecvData);
    SCI_CHECK_DATA((realRecvData+read0[4]+1), recv2, 2);

    memcpy(data, realRecvData+1, *pLen);

	return OK;
}
// 
BSP_S32 tmSciReadEFdir(BSP_U8 *pLen, BSP_U8 *data)
{
	BSP_S32 s32Ret = 0;
    BSP_U8  fid[2] = {0x2f, 0x00};
    BSP_U8  aucRID[5] = {0xA0,0x00,0x00,0x00,0x87};
    BSP_U8  data_len;
    BSP_U8  au8Data[100];

	s32Ret = tmSciReadFile(SCI_FILE_TYPE_LINEAR, fid, 1, &data_len, au8Data);
	if(s32Ret != OK)
	{
		printf("(%s)%d: tmSciSelectFile fail,ret:%x\n",__FUNCTION__, __LINE__, s32Ret);
		return ERROR;
	}

    if((0x61 != au8Data[0])||(0x4f != au8Data[2]))    /*判断内容合法性*/
    {
		printf("(%s)%d: invalid AID\n",__FUNCTION__, __LINE__);
		return ERROR;
    }

    if(0 != memcmp(au8Data+4, aucRID, sizeof(aucRID)))
    {
		printf("(%s)%d: invalid AID\n",__FUNCTION__, __LINE__);
		return ERROR;
    }
    
    memcpy(data, au8Data+4, au8Data[3]);    /*保存相关信息*/
    *pLen = au8Data[3];

	return OK;
}
//
BSP_S32 tmSciSelectAdf()
{
	BSP_S32 s32Ret = 0;
    BSP_U8  adf_len;
    BSP_U8  adf_name[20];
	BSP_U8  selectCmd[5] = {0x00,0xa4,0x04,0x04,0x00};
//	BSP_S32 i = 0;

	s32Ret = tmSciReadEFdir(&adf_len, adf_name);
	if(s32Ret != OK)
	{
		printf("(%s)%d: tmSciReadEFdir fail,ret:%x\n", __FUNCTION__, __LINE__, s32Ret);
		return ERROR;
	}

    selectCmd[4] = adf_len;
	s32Ret = tmSciSelectFile(selectCmd, adf_name);
	if(s32Ret != OK)
	{
		printf("(%s)%d: tmSciSelectFile fail,ret:%x\n", __FUNCTION__, __LINE__, s32Ret);
		return ERROR;
	}

	return OK;
}
//
BSP_S32 tmSciAppPrepare()
{
    if(tmSciSelectAdf() != OK)
    {
        return ERROR;
    }

    return OK;
}
/*
type:--->SCI_CARD_TYPE_USIM
USIM   白卡
1.unsigned char  ucSend0[5] = {0xa0,0xa4,0x00,0x00,0x02};//return 1
2.unsigned char  ucSend1[2] = {0x3f,0x00};//return 2
3.unsigned char  ucSend2[5] = {0xa0,0xc0,0x00,0x00,0x20};//return 61
完成上述3步的前2步，使卡处于发送接收的第3步
:--->SCI_CARD_TYPE_SIM
SIM    黄卡
*/

BSP_S32 tmSciPrepare(BSP_U32 cardType)
{
	BSP_S32 s32Ret = 0;

	if(tmSciSemCreate() != OK)
	{
		return ERROR;
	}
	if(tmSciRegEvent(sci_event_func_001) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	tickResetSuccessStart = sci_get_timestart();
	s32Ret = (BSP_S32)bsp_sci_reset(COLD_RESET);
	if(s32Ret != OK)
	{
		printf("bsp_sci_reset fail,ret:%x\n",s32Ret);
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
	(BSP_VOID)semTake(sciSemCardResetSuccess,WAIT_FOREVER);

	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		(BSP_VOID)tmSciSemDelete();
		return ERROR;
	}
    g_cardtype = cardType;

    if(SCI_CARD_TYPE_USIM == g_cardtype)
    {
    	printf("CARD TYPE: usim card!\n");
    }

    return OK;
}

//
BSP_S32 tmSciSendRecvDataNew(BSP_U32 cardType,
						  BSP_U32 sendLen,
						  BSP_U8* sendData,
						  BSP_U32 preReadLen,
						  BSP_U8* recvData,
						  BSP_S32 waitTicks/*wait time between send and preread*/)
{
	BSP_U8 recvBuf[SCI_DATA_MAX_LEN] = {0};
	BSP_U8 excp_data[2] = {0x90, 0x00};
	BSP_S32 s32Ret = 0;
//	BSP_S32 i = 0;
//	BSP_U32 tmp_len = 0;

	s32Ret = (BSP_S32)bsp_sci_data_send(sendLen,sendData);
	if(s32Ret != OK)
	{
		printf("%s(%d): bsp_sci_data_send fail,ret:%x\n", __FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}

	(BSP_VOID)tmSciStatusPrint();
	taskDelay(waitTicks);
	//tickReadDataStart = sci_get_timestart();

    s32Ret = bsp_sci_data_read_sync(preReadLen, recvBuf);
	if(s32Ret != BSP_OK)
	{
		printf("%s(%d): BSP_SCI_Receive fail,ret:%x\n", __FUNCTION__, __LINE__,s32Ret);
		return ERROR;
	}
    SCI_PRINT_DATA(preReadLen, recvBuf);

	if(tmSciStatusCheck(SCI_CARD_STATE_READY) != OK)
	{
		printf("%s(%d): tmSciStatusCheck fail,\n", __FUNCTION__, __LINE__);
		return ERROR;
	}

	if(sci_memcmp_ext((void*)(recvBuf+preReadLen-2), (void*)excp_data, 2) != OK)
	{
		printf("%s(%d): sci_memcmp_ext fail,\n", __FUNCTION__, __LINE__);
		return ERROR;
	}

	return OK;
}
/*******************************************************************************
*
* tstSciUsimWriteEFsms - tstSciUsimWriteEFsms
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUsimWriteEFsms(void)
{
	BSP_S32 s32Ret = 0;
	BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0x80,0xf2,0x00,0x00,0x22};
	BSP_U32 recvLen = 37;
	BSP_S32 waitTicks = 0;

	s32Ret = tmSciPrepare(SCI_CARD_TYPE_USIM);
	if(s32Ret != OK)
	{
        printf("%s(%d): tmSciPrepare FAILED!\n", __FUNCTION__, __LINE__);
		return VXTEST_ERROR;
	}

	s32Ret = tmSciAppPrepare();
	if(s32Ret != OK)
	{
        printf("%s(%d): tmSciAppPrepare FAILED!\n", __FUNCTION__, __LINE__);
		(BSP_VOID)tmSciEnd();
		return VXTEST_ERROR;
	}

    recvLen = g_curFCPlen+3;
    sendData[4] = g_curFCPlen;
	if(tmSciSendRecvDataNew(SCI_CARD_TYPE_USIM,sendLen,sendData,recvLen,g_recvData,waitTicks) != OK)
	{
		(BSP_VOID)tmSciEnd();
		return VXTEST_FAIL;
	}

	s32Ret = tmSciEnd();
	if(s32Ret != OK)
	{
		return VXTEST_ERROR;
	}

	return VXTEST_PASS;
}

/*******************************************************************************
*
* tstSciUsimRecordSave - tstSciUsimRecordSave
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUsimRecordSave(void)
{
	BSP_S32 s32Ret = 0;
    BSP_U32 sendLen = 5;
	BSP_U8  sendData[] = {0xa0,0xc0,0x00,0x00,0x22};

    s32Ret = bsp_sci_reset(COLD_RESET);
    if(s32Ret != OK)
	{
        printf("%s(%d): tstSciUsimRecordSave reset FAILED!\n", __FUNCTION__, __LINE__);
		
	}

    s32Ret = bsp_sci_data_send(sendLen, sendData);
    if(s32Ret != OK)
	{
        printf("%s(%d): tstSciUsimRecordSave send func FAILED!\n", __FUNCTION__, __LINE__);
		
	}

	s32Ret = bsp_sci_record_data_save(SCI_LOG_RESET_FAIL);
	if(s32Ret != OK)
	{
        printf("%s(%d): tstSciUsimRecordSave data save FAILED!\n", __FUNCTION__, __LINE__);
		
	}

    s32Ret = (s32)bsp_sci_deactive();
    if(s32Ret != OK)
	{
        printf("%s(%d): tstSciUsimRecordSave deactive FAILED!\n", __FUNCTION__, __LINE__);
		
	}

	return VXTEST_PASS;
}
// 20130306--Parity Error Detect Modify Begin:

/*******************************************************************************
*
* tstSciUsimParityDetect - tstSciUsimParityDetect
* \cs
* <testCase>
*     <timeout>     10000    </timeout>
*     <reentrant>   FALSE       </reentrant>
*     <memCheck>    TRUE        </memCheck>
*     <destructive> FALSE       </destructive>
* </testCase>
* \ce
*
* RETURNS: VXTEST_PASS if test passes; VXTEST_FAIL otherwise
*
*/
VXTEST_STATUS tstSciUsimParityDetect(void)
{
	BSP_U32 sendLen = 5;
	BSP_U8	sendData[] = {0xa0,0xc0,0x00,0x00,0x22};
	BSP_S32 expectRecvLen = 37;
	BSP_S32 expectRecvTicks = 4;
	BSP_S32 waitTicks = 0;
	BSP_U32 preReadLen = (BSP_U32)expectRecvLen;
	BSP_BOOL recvOnce = FALSE;
	BSP_S32 s32Ret = 0;

	bParityDetectFlag = TRUE;

	s32Ret = tmSciGeneralSendRecvData(SCI_CARD_TYPE_USIM,
		sendLen,
		sendData,
		waitTicks,
		preReadLen,
		recvOnce,
		expectRecvLen,
		g_recvData,
		expectRecvTicks);

	bParityDetectFlag = FALSE; 
	
	if(s32Ret != OK)
	{
		return VXTEST_FAIL;
	}
	return VXTEST_PASS;

}

VXTEST_STATUS tstSciUsimGetSCICfg(void)
{
    SCI_SLOT sci_slot0;
    SCI_SLOT sci_slot1;
    bsp_sci_get_slot_state(&sci_slot0, &sci_slot1);
    sci_print_error("CARD SLOT0 CFG is %d\n",sci_slot0);
    sci_print_error("CARD SLOT1 CFG is %d\n",sci_slot1);
	return VXTEST_PASS;
      
}
extern unsigned int g_SciNVValue ;

VXTEST_STATUS tstSciUsimSlotSwitch(void)
{
    SCI_SLOT sci_slot0;
    SCI_SLOT sci_slot1;
    SCI_CFG_STRU * p_sci0_cfg = NULL;
    SCI_CFG_STRU * p_sci1_cfg = NULL;
    

    if(SCI_DUAL != ((NV_SCI_CFG_UNION)g_SciNVValue).cfg.card_num)
    {
        sci_print("not support double card \n");
        return VXTEST_FAIL;
    }


    p_sci0_cfg = (SCI_CFG_STRU *)((int)&_sci_cfg_data_start + \
                                (int)(sizeof(SCI_CFG_STRU) * CARD0));
    p_sci1_cfg = (SCI_CFG_STRU *)((int)&_sci_cfg_data_start + \
                                (int)(sizeof(SCI_CFG_STRU) *  CARD1));
    sci_print_error("SCI CFG before slot switch\n");
    sci_print_error("p_sci0_cfg->card_id =%d\n",p_sci0_cfg->card_id);
    sci_print_error("p_sci0_cfg->sci_id =%d\n",p_sci0_cfg->sci_id);
    sci_print_error("p_sci0_cfg->base_addr =%d\n",p_sci0_cfg->base_addr);
    sci_print_error("p_sci0_cfg->int_number =%d\n",p_sci0_cfg->int_number);
    sci_print_error("p_sci0_cfg->clk_en_bit =%d\n",p_sci0_cfg->clk_en_bit);
    sci_print_error("p_sci0_cfg->clk_dis_reg =%d\n",p_sci0_cfg->clk_dis_reg);
    sci_print_error("p_sci0_cfg->record_enum =%d\n",p_sci0_cfg->record_enum);
    sci_print_error("p_sci0_cfg->icc_chnl =%d\n",p_sci0_cfg->icc_chnl);
    sci_print_error("p_sci0_cfg->record_file =%s\n",p_sci0_cfg->record_file);
    sci_print_error("p_sci0_cfg->clk_gate =%d\n",p_sci0_cfg->clk_gate);
    sci_print_error("p_sci0_cfg->sleep_vote =%d\n",p_sci0_cfg->sleep_vote);
    sci_print_error("p_sci0_cfg->pwr_type =%s\n",p_sci0_cfg->pwr_type);
    sci_print_error("p_sci0_cfg->syncTaskName =%s\n",p_sci0_cfg->syncTaskName);

    sci_print_error("p_sci1_cfg->card_id =%d\n",p_sci1_cfg->card_id);
    sci_print_error("p_sci1_cfg->sci_id =%d\n",p_sci1_cfg->sci_id);
    sci_print_error("p_sci1_cfg->base_addr =%d\n",p_sci1_cfg->base_addr);
    sci_print_error("p_sci1_cfg->int_number =%d\n",p_sci1_cfg->int_number);
    sci_print_error("p_sci1_cfg->clk_en_bit =%d\n",p_sci1_cfg->clk_en_bit);
    sci_print_error("p_sci1_cfg->clk_dis_reg =%d\n",p_sci1_cfg->clk_dis_reg);
    sci_print_error("p_sci1_cfg->record_enum =%d\n",p_sci1_cfg->record_enum);
    sci_print_error("p_sci1_cfg->icc_chnl =%d\n",p_sci1_cfg->icc_chnl);
    sci_print_error("p_sci1_cfg->record_file =%s\n",p_sci1_cfg->record_file);
    sci_print_error("p_sci1_cfg->clk_gate =%d\n",p_sci1_cfg->clk_gate);
    sci_print_error("p_sci1_cfg->sleep_vote =%d\n",p_sci1_cfg->sleep_vote);
    sci_print_error("p_sci1_cfg->pwr_type =%s\n",p_sci1_cfg->pwr_type);
    sci_print_error("p_sci1_cfg->syncTaskName =%s\n",p_sci1_cfg->syncTaskName);

    bsp_sci_get_slot_state(&sci_slot0, &sci_slot1);
    if(sci_slot0 == 0)
    {
        sci_slot0 = 1;
        sci_slot1 = 0;
    }
    else
    {
        sci_slot0 = 0;
        sci_slot1 = 1;
    }
    
    bsp_sci_slot_switch(sci_slot0, sci_slot1);
    
    sci_print_error("SCI CFG after slot switch\n");
    sci_print_error("p_sci0_cfg->card_id =%d\n",p_sci0_cfg->card_id);
    sci_print_error("p_sci0_cfg->sci_id =%d\n",p_sci0_cfg->sci_id);
    sci_print_error("p_sci0_cfg->base_addr =%d\n",p_sci0_cfg->base_addr);
    sci_print_error("p_sci0_cfg->int_number =%d\n",p_sci0_cfg->int_number);
    sci_print_error("p_sci0_cfg->clk_en_bit =%d\n",p_sci0_cfg->clk_en_bit);
    sci_print_error("p_sci0_cfg->clk_dis_reg =%d\n",p_sci0_cfg->clk_dis_reg);
    sci_print_error("p_sci0_cfg->record_enum =%d\n",p_sci0_cfg->record_enum);
    sci_print_error("p_sci0_cfg->icc_chnl =%d\n",p_sci0_cfg->icc_chnl);
    sci_print_error("p_sci0_cfg->record_file =%s\n",p_sci0_cfg->record_file);
    sci_print_error("p_sci0_cfg->clk_gate =%d\n",p_sci0_cfg->clk_gate);
    sci_print_error("p_sci0_cfg->sleep_vote =%d\n",p_sci0_cfg->sleep_vote);
    sci_print_error("p_sci0_cfg->pwr_type =%s\n",p_sci0_cfg->pwr_type);
    sci_print_error("p_sci0_cfg->syncTaskName =%s\n",p_sci0_cfg->syncTaskName);

    sci_print_error("p_sci1_cfg->card_id =%d\n",p_sci1_cfg->card_id);
    sci_print_error("p_sci1_cfg->sci_id =%d\n",p_sci1_cfg->sci_id);
    sci_print_error("p_sci1_cfg->base_addr =%d\n",p_sci1_cfg->base_addr);
    sci_print_error("p_sci1_cfg->int_number =%d\n",p_sci1_cfg->int_number);
    sci_print_error("p_sci1_cfg->clk_en_bit =%d\n",p_sci1_cfg->clk_en_bit);
    sci_print_error("p_sci1_cfg->clk_dis_reg =%d\n",p_sci1_cfg->clk_dis_reg);
    sci_print_error("p_sci1_cfg->record_enum =%d\n",p_sci1_cfg->record_enum);
    sci_print_error("p_sci1_cfg->icc_chnl =%d\n",p_sci1_cfg->icc_chnl);
    sci_print_error("p_sci1_cfg->record_file =%s\n",p_sci1_cfg->record_file);
    sci_print_error("p_sci1_cfg->clk_gate =%d\n",p_sci1_cfg->clk_gate);
    sci_print_error("p_sci1_cfg->sleep_vote =%d\n",p_sci1_cfg->sleep_vote);
    sci_print_error("p_sci1_cfg->pwr_type =%s\n",p_sci1_cfg->pwr_type);
    sci_print_error("p_sci1_cfg->syncTaskName =%s\n",p_sci1_cfg->syncTaskName);
    return VXTEST_PASS;
}

VXTEST_STATUS tstSciUsimResetSlotSwitch(void)
{
     SCI_SLOT sci_slot0;
     SCI_SLOT sci_slot1;
     SCI_CFG_STRU * p_sci0_cfg = NULL;
     SCI_CFG_STRU * p_sci1_cfg = NULL;
     
     u32 ulCardStatus;
     u32 ulCardStatus1;
    
     if(SCI_DUAL != ((NV_SCI_CFG_UNION)g_SciNVValue).cfg.card_num)
     {
         sci_print("not support double card \n");
         return VXTEST_FAIL;
     }
    
    
     p_sci0_cfg = (SCI_CFG_STRU *)((int)&_sci_cfg_data_start + \
                                 (int)(sizeof(SCI_CFG_STRU) * CARD0));
     p_sci1_cfg = (SCI_CFG_STRU *)((int)&_sci_cfg_data_start + \
                                 (int)(sizeof(SCI_CFG_STRU) *  CARD1));
     bsp_sci_get_slot_state(&sci_slot0, &sci_slot1);
     if(sci_slot0 == 0)
     {
         sci_slot0 = 1;
         sci_slot1 = 0;
     }
     else
     {
         sci_slot0 = 1;
         sci_slot1 = 0;
     }
     
     bsp_sci_slot_switch(sci_slot0, sci_slot1);
     bsp_sci_reset(COLD_RESET);
     I1_bsp_sci_reset(COLD_RESET);


	ulCardStatus = bsp_sci_card_status_get();
    ulCardStatus1 = I1_bsp_sci_card_status_get();
	if(ulCardStatus == SCI_CARD_STATE_READY || ulCardStatus1 == SCI_CARD_STATE_READY)
    {
        
        return VXTEST_PASS;
    }   
        
     return VXTEST_FAIL;

}


/*lint +e525*/

// 20130306--Parity Error Detect Modify End!
