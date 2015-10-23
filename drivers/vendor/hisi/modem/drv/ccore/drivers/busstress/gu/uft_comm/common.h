

#include <vxWorks.h>
#include <stdio.h>
#include "BasicFunc.h"
#include "baseaddr.h"



void DelayRandom();

void DelayRandomSet(UINT32 DelayMin,UINT32 DelayMax);

void GetFilePath(INT8 *GenDir, INT8 *FileName);

int DownLoadFile(UINT32 Des_addr, INT8 *FileName );

int CevaLoop();

int CevaRandom();

int CevaReset();

int CevaBoot();
//int ZspLoad(char* ZSPFileName);

//int ZspBoot(char* ZSPFileName);

//int ZspLoad1(char* ZSPFileName);

//void downtxt(UINT32 des_addr, INT8 *FileName );

//int txt_cmp( UINT32 src_addr, INT8 *FileName );
