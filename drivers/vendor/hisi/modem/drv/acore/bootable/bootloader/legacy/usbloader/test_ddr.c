#include <boot/boot.h>
#include <boot/uart.h>
#include <boot/tags.h>
#include <boot/flash.h>
#include <boot/board.h>
#include <boot/usb.h>

#include <boot/bootimg.h>


#define UINT32  unsigned long
#define UINT8   unsigned char

#define DDR_ADDR    		0x50010000
#define DDR_SIZE    		(64 * 1024 * 1024) //test size is 512M.If you want to change test size, please fix here!
#define PRINT_SIZE    		(4 * 1024)//4k
#define PRINT_SIZE_LARGE	(4 * 1024 * 1024)//4M

void cprintf(const char *fmt, ...);

#define PRINT_INFO cprintf
void WriteMember64k(UINT32 dwData)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	//volatile UINT32 *pAddr = (UINT32 *)0x18000000;//100M´¦
	volatile UINT32 dwLen = (4 * 1024 * 1024) >> 2;/* 4 bits is 1 word, so >> 2 */

	for(i = 0; i < dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");
		*pAddr = dwData;
		pAddr++;
	}
	PRINT_INFO("\n");
}

UINT32 CmpMember64k(UINT32 dwData)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	//volatile UINT32 *pAddr = (UINT32 *)0x18000000;//100M´¦
	volatile UINT32 dwLen = (4 * 1024 * 1024) >> 2;
	volatile UINT32 tmp;


	for( i = 0; i < dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");

		tmp = *pAddr;
		if (dwData != tmp) {
			PRINT_INFO("CmpMember:Addr 0x%x: expect 0x%x, Actual 0x%x.\r\n", pAddr, dwData, tmp);
		}
		pAddr++;
	}
	PRINT_INFO("\n");

	return 0xFFFFFFFF;
}

void WriteMember(UINT32 dwData)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	volatile UINT32 dwLen = DDR_SIZE >> 2;

	for(i = 0; i<dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");
		*pAddr = dwData;
		pAddr++;
	}
	PRINT_INFO("\n");
}

UINT32 CmpAndWriteMember(UINT32 dwData, UINT32 dwNewData)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	volatile UINT32 dwLen = DDR_SIZE >> 2;
	volatile UINT32 tmp;

	for(i = 0; i < dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");

		tmp = *pAddr;
		if(dwData != tmp) {
			/* return (UINT32)(pAddr); */
			PRINT_INFO("CmpAndWriteMember:Addr 0x%x: expect 0x%x, Actual 0x%x.\r\n", pAddr, dwData, tmp);
		}
		*pAddr = dwNewData;
		pAddr++;
	}
	PRINT_INFO("\n");

	return 0xFFFFFFFF;
}


UINT32 CmpMember(UINT32 dwData)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	volatile UINT32 dwLen = DDR_SIZE >> 2;
	volatile UINT32 tmp;

	for(i=0; i<dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");

		tmp = *pAddr;
		if(dwData != tmp) {
			/* return (UINT32)(pAddr); */
			PRINT_INFO("CmpMember:Addr 0x%x: expect 0x%x, Actual 0x%x.\r\n", pAddr, dwData, tmp);
		}
		pAddr++;
	}

	PRINT_INFO("\n");
	return 0xFFFFFFFF;
}


void WriteMemberAddr(void)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	volatile UINT32 dwLen = DDR_SIZE >> 2;

	for(i=0; i<dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");
		*pAddr = (UINT32)pAddr;
		pAddr++;
	}
	PRINT_INFO("\n");
}


UINT32 CmpAndWriteMemberAddr(void)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	volatile UINT32 dwLen = DDR_SIZE >> 2;
	volatile UINT32 tmp;

	for(i = 0; i < dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");

		tmp = *pAddr;
		if((UINT32)pAddr != tmp) {
			/* return (UINT32)(pAddr); */
			PRINT_INFO("CmpAndWriteMemberAddr:Addr 0x%x: expect 0x%x, Actual 0x%x.\r\n", pAddr, pAddr, tmp);
		}
		*pAddr = ~((UINT32)pAddr);
		pAddr++;
	}
	PRINT_INFO("\n");

	return 0xFFFFFFFF;
}

UINT32 CmpMemberRevAddr(void)
{
	volatile UINT32 i;
	volatile UINT32 *pAddr = (UINT32 *)DDR_ADDR;
	volatile UINT32 dwLen = DDR_SIZE >> 2;
	volatile UINT32 tmp;

	for(i=0; i<dwLen; i++) {
		if (i % PRINT_SIZE_LARGE == 0)
			PRINT_INFO(".");
		tmp = *pAddr;
		if((~((UINT32)pAddr)) != tmp) {
			/* return (UINT32)(pAddr); */
			PRINT_INFO("CmpMemberRevAddr:Addr 0x%x: expect 0x%x, Actual 0x%x.\r\n", pAddr, ~(UINT32)pAddr, tmp);
		}
		pAddr++;
	}

	PRINT_INFO("\n");

	return 0xFFFFFFFF;
}

/******************************************************************************
Function:           DDR_training_test
Description:        training DDR
Input:              none
Output:             none
Return:             1 success
		0 fail
******************************************************************************/
UINT32 DDR_training_test(void)
{
	volatile UINT32 dwResult;
	volatile UINT32 dwData;
	volatile UINT32 i,j,k;
	unsigned long value = 0;

	// test 1
#if 0
	PRINT_INFO("**** tc1: check training\n");

	i = readl(
		+0xd4) & 0xffff;
	j = readl(REG_BASE_DDRC_CFG+0xd8) & 0xffff;
	k = readl(REG_BASE_DDRC_CFG+0x250);

	if (i == 0xf00 && j == 0xf00 && k != 0)
		PRINT_INFO("**** tc1: ok\n");
	else {
		PRINT_INFO("**** tc1: failed, 0xd4 = 0x%x, 0xd8 = 0x%x, 0x250 = %d\n", i, j, k);
		return 0;
	}
	PRINT_INFO("------------------------------------------------------\n");
#endif

	PRINT_INFO("**** tc2: write 64k '0'\n");

	dwData = 0x00000000;
	WriteMember64k(dwData);
	dwResult = CmpMember64k(dwData);

	if(0xFFFFFFFF != dwResult) {
		PRINT_INFO("**** tc2: failed\n");
		return 0;
	} else {
		PRINT_INFO("**** tc2:ok\n");
	}
	PRINT_INFO("------------------------------------------------------\n");


	PRINT_INFO("**** tc3: write 64k '1'\n");
	dwData = 0xffffffff;
	WriteMember64k(dwData);
	dwResult = CmpMember64k(dwData);

	if(0xFFFFFFFF != dwResult) {
		PRINT_INFO("**** tc3: failed\n");
		return 0;
	} else {
		PRINT_INFO("**** tc3:ok\n");
	}
	PRINT_INFO("------------------------------------------------------\n");

	PRINT_INFO("**** tc4: write 1 giga '0'\n");
	dwData = 0x00000000;
	WriteMember(dwData);
	dwResult = CmpAndWriteMember(dwData, 0xFFFFFFFF);
	if(0xFFFFFFFF != dwResult) {
		PRINT_INFO("**** tc4: failed\n");
		return 0;
	} else {
		PRINT_INFO("**** tc4:ok\n");
	}
	PRINT_INFO("------------------------------------------------------\n");


	PRINT_INFO("**** tc5: cmp 1 giga '1'\n");
	dwData = 0xFFFFFFFF;
	dwResult = CmpMember(dwData);
	if(0xFFFFFFFF != dwResult) {
		PRINT_INFO("**** tc5: failed\n");
		return 0;
	} else {
		PRINT_INFO("**** tc5:ok\n");
	}
	PRINT_INFO("------------------------------------------------------\n");


	PRINT_INFO("**** tc6: write 1 giga 'addr'\n");
	/* second write address test */
	WriteMemberAddr();
	dwResult = CmpAndWriteMemberAddr();
	if(0xFFFFFFFF != dwResult) {
		PRINT_INFO("**** tc6: failed\n");
		return 0;
	} else {
		PRINT_INFO("**** tc6:ok\n");
	}
	PRINT_INFO("------------------------------------------------------\n");


	PRINT_INFO("**** tc7: cmd 1 giga 'addr'\n");
	dwResult = CmpMemberRevAddr();
	if(0xFFFFFFFF != dwResult) {
		PRINT_INFO("**** tc7: failed\n");
		return 0;
	} else {
		PRINT_INFO("**** tc7:ok\n");
	}
	PRINT_INFO("------------------------------------------------------\n");


	PRINT_INFO("training test OK\n");
	return 1;
}




