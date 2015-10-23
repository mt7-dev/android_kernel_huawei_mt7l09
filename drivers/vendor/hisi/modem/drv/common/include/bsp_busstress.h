#include "bsp_memmap.h"
#include "hi_dsp.h"
#include "osl_bio.h"
#include "osl_types.h"

#ifndef TM_STRESS_BASE_H
#define TM_STRESS_BASE_H

/*A9 TEST PARA*/
#define A9_DATA_CACHE_SIZE					(128*1024)
#define AXI_MEM_SIZE_FOR_A9				(1024)
#define DDR_MEM_SIZE_FOR_A9          (1024)

#define BBE16_LOCAL_MEM_SIZE_FOR_A9          (1024)
#define BBE16_LOCAL_MEM_START_ADDR_FOR_A9_A	(BBE_TCM_ADDR + 2*BBE16_LOCAL_MEM_SIZE_FOR_A9)

#define HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE  (512)
#define HIFI_LOCAL_MEM_FOR_MCORE_DST_ADDR   (HI_HIFI2DMEM0_BASE_ADDR  + HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE)
#define HIFI_LOCAL_MEM_FOR_ACORE_SRC_ADDR   (HIFI_LOCAL_MEM_FOR_MCORE_DST_ADDR + HIFI_LOCAL_MEM_FOR_ARM_MEMCPY_SIZE)

/*AXI MEM 64KB*/
#define AXI_MEM_FOR_MEMCPY_SIZE        (128)
#define MEM_FOR_EDMAC_BUSSTRESS SRAM_BUSSTRESS_ADDR
#define AXI_MEM_64_SIZE_FOR_EDMAC  (6*1024)

#define MEM_FOR_BBE16_BUSSTRESS (SRAM_BUSSTRESS_ADDR +AXI_MEM_64_SIZE_FOR_EDMAC)/*used in ccore no need to convert into phy addr*/
#define AXI_MEM_64_SIZE_FOR_BBE16  (1024)

#define AXI_MEM_64_SRC_FOR_HIFI (MEM_FOR_BBE16_BUSSTRESS +AXI_MEM_64_SIZE_FOR_BBE16)
#define AXI_MEM_64_DST_FOR_HIFI (AXI_MEM_64_SRC_FOR_HIFI + AXI_MEM_FOR_MEMCPY_SIZE)

#define AXI_MEM_FOR_HIFI_SRC_ADDR      (AXI_MEM_64_DST_FOR_HIFI + AXI_MEM_FOR_MEMCPY_SIZE)
#define AXI_MEM_FOR_HIFI_DST_ADDR      (AXI_MEM_FOR_HIFI_SRC_ADDR + AXI_MEM_FOR_MEMCPY_SIZE)

#define AXI_MEM_64_SRC_FOR_A9_CCORE (AXI_MEM_FOR_HIFI_DST_ADDR + AXI_MEM_FOR_MEMCPY_SIZE)
#define AXI_MEM_64_DST_FOR_A9_CCORE (AXI_MEM_64_SRC_FOR_A9_CCORE + AXI_MEM_FOR_MEMCPY_SIZE)

#define AXI_MEM_64_VSRC_FOR_A9_ACORE (AXI_MEM_64_DST_FOR_A9_CCORE + AXI_MEM_FOR_MEMCPY_SIZE)
#define AXI_MEM_64_VDST_FOR_A9_ACORE (AXI_MEM_64_VSRC_FOR_A9_ACORE + AXI_MEM_FOR_MEMCPY_SIZE)

/*test ddr*/
#define BBE16_DDR_BASEADDR               SHM_MEM_BUSSSTRESS_ADDR
#define BBE16_DDR_SIZE               		(1024)
#define HIFI_DDR_BASEADDR               (BBE16_DDR_BASEADDR +BBE16_DDR_SIZE)
#define HIFI_DDR_SIZE               		(64*1024)

#define PRINT_ERROR() do{\
	printf(">>>line %d :%s failed \n",__LINE__,(int)__FUNCTION__);\
	return ERROR;\
	}while(0)

#define LOGMSG_ERROR() do{\
	logMsg(">>>line %d FUNC %s fail\n",__LINE__,(int)__FUNCTION__,0,0,0,0);\
	}while(0)

typedef enum tagUseSpaceType
{
	DDR = 0,
	BBE16 = 1,
	AXIMEM = 2,
	DDR_BBE16_AXIMEM = 3,
	DSP = 4,
	HIFI = 5
}Use_Space_Type;

typedef enum tagTestModule
{
	GMAC = 0,
	ACC = 1,
	CIPHER = 2,
	KDF =3,
	DMA_DSP =4,
	EDMAC = 5,
	IPF_UL = 6,
	IPF_DL = 7,
	LCD = 8
}TestModule;
typedef enum tagTestSwitch
{
	TEST_STOP = 0,
	TEST_RUN =1
}TestSwitch;

typedef struct tagPsSendPacket
{
    int s32SendDataAddr;
    int s32SendDataLen;
    unsigned int pNext;	
}PsSendPacket;


int memcpyTestProcess(unsigned int uiSrc,unsigned int uiDst,int iSize);

typedef struct bus_stress_test_tag
{
	unsigned int uiSrcAddr;
	unsigned int uiDstAddr;
	unsigned int uiSizeBytes;
	unsigned int uiBusWidth;
	unsigned int uiErrCount;
}BUS_STRESS_TEST_STRU;

void *alloc(signed sz);
int get_test_switch_stat(int module_id);
int set_test_switch_stat(int module_id, int run_or_stop);

#endif /*TM_STRESS_BASE_H*/

