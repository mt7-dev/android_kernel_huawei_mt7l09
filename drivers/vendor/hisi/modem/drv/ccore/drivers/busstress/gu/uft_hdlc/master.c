#include "master.h"
#include "upacc.h"
//#include "../ipf/ipf.h"
#include "hdlc_for_bbit.h"
#include "osl_bio.h"
#include "product_config.h"
#define STRESS_TEST_BYTE 1024
#define _UNIT_DEBUG 

UINT32 addr_src_upacc = 0;
UINT32 addr_des_upacc = 0;
UINT32 addr_src_cicom0 = 0;
UINT32 addr_des_cicom0 = 0;
UINT32 src_addr_hdlc = 0;
UINT32 des_addr_hdlc = 0;

int upacc_test(unsigned int  data_len_upacc)
{
	//int i = 0;
	upacc_trans_config(addr_src_upacc, addr_des_upacc, data_len_upacc);
    set32( UPACC_BASEADDR, 0x2 );
    if(upacc_trans_check(addr_src_upacc, addr_des_upacc, data_len_upacc))
    {
    	 printf("upacc_trans_check fail\n");
    	return 1;
	}
     if(upacc_trans_check(addr_src_upacc, addr_des_upacc, data_len_upacc))
	{
    	    printf("upacc_trans_check fail\n");
    	   return 1;
	}
	return 0;
}
int cicom_test(unsigned int data_len_cicom0)
{
	cicom_v500_trans_config(addr_src_cicom0, addr_des_cicom0, data_len_cicom0);
    cicom_v500_trans_en();
   if(cicom_v500_trans_check(addr_src_cicom0, addr_des_cicom0, data_len_cicom0))
   	{
   		 printf("cicom_v500_trans_check fail\n");
    		return 1;
   	}
	return 0;
}
#if _HDLC_TRANS
int hdlc_test()
{
	 mem_clr(src_addr_hdlc, 0x400);  //clear ipf source data memory
        mem_clr(des_addr_hdlc, 0x400);   //clear ipf destination data memory
       return hdlcv200_frm_def_test_bbit(src_addr_hdlc, des_addr_hdlc);
}
#endif

int gu_master_test(void)
{
   
    //int loop_temp;
    //UINT32 temp_data;
    
    //int init_data = 0x5a5a5a5a;
    int i=0;
    //UINT32 EdmacDataWidth=0;
    //UINT32 BurstType;
    //UINT32 EdmacBurstType = 0;
    //UINT32 EdmacTransferSize;
    //UINT32 BBPBurstType = 0;
	 
    //UINT32 BBPTransferSize;
   // UINT32 high_addr,data;
    //UINT32 reg_read,addr;
    
    UINT32 data_len_upacc = STRESS_TEST_BYTE;
    UINT32 data_len_cicom0 = STRESS_TEST_BYTE;
    if (!(addr_src_upacc && addr_des_upacc && addr_src_cicom0 && addr_des_cicom0 && src_addr_hdlc && des_addr_hdlc))
    {
        printf("cacheDmaMalloc Fail!\n");
        return ERROR;
    }
    
    #if _UPACC_TRANS 
    i+=upacc_test(data_len_upacc);
    #endif
    
    #if _CICOM0_TRANS
    i+=cicom_test(data_len_cicom0);
    #endif
        
    #if _HDLC_TRANS
    i+=hdlc_test();
    #endif
    
    if(i==0)
    {   
#ifdef _UNIT_DEBUG    
        printf("gu_master_test pass\n");
#endif
        return 0;
    } 
    else
    {
        printf("gu_master_test fail\n");
        return 1;
    }

}

void gu_master_routine(int delayticks)
{
	while(get_test_switch_stat(8))
	{
		gu_master_test();
		taskDelay(delayticks);
	}
	return ;
}
int  gu_stress_test_task_id = -1;
int gu_master_stress_test_start(int task_priority,int delayticks)
{
	//UINT32 reg = 0x1F80000;  
	/*?a?¡ä??*/
	writel(SYSCTRL_CRG_VALUE, SYSCTRL_CRG);  

	addr_src_upacc = cacheDmaMalloc(STRESS_TEST_BYTE);
	addr_des_upacc = cacheDmaMalloc(STRESS_TEST_BYTE);
	addr_src_cicom0 = cacheDmaMalloc(STRESS_TEST_BYTE);
	addr_des_cicom0 = cacheDmaMalloc(STRESS_TEST_BYTE);
	src_addr_hdlc = cacheDmaMalloc(STRESS_TEST_BYTE);
	des_addr_hdlc = cacheDmaMalloc(STRESS_TEST_BYTE);
	set_test_switch_stat(8,1);

	gu_stress_test_task_id = taskSpawn ("guTask", task_priority, 0, 80000, (FUNCPTR)gu_master_routine, delayticks,
	                                    0, 0, 0,0, 0, 0, 0, 0, 0);
	if(gu_stress_test_task_id == -1)
	{
		printf("taskSpawn gmacSendTask fail.\n");
	}

	return gu_stress_test_task_id;	
}

int gu_master_stress_test_stop()
{
	set_test_switch_stat(8,0);
	while(0 == taskIdVerify(gu_stress_test_task_id))
	{
		taskDelay(100);
	}
	return 0;
}

