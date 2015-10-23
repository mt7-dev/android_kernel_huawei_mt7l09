/* includes */
#ifdef DRV_BUILD_SEPARATE


#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include "drv_udi.h"
#include "drv_acm.h"



#define ACM_DEV                 "/acm/"
#define CONFIG_FD_CDC_ACM       8
#define BSP_ACM_SIO_NUM         3

/* 是否测试 Modem 测试用例 */
#define MODEM_TEST 0

#ifdef BSP_ACM_MAX_NUM
#undef BSP_ACM_MAX_NUM
#define BSP_ACM_MAX_NUM         8
#else
#define BSP_ACM_MAX_NUM         8
#endif

//#define BSP_ACM_MAX_NUM CONFIG_FD_CDC_ACM
//#define BSP_ACM_SIO_NUM CONFIG_FD_CDC_ACM
#define ACM_MODEM_TYPE 0x6

#define RUN_BASE_TEST(case_description,case_code) do{\
    if(case_code != BSP_OK)\
    {\
        return BSP_ERROR;\
    }\
    return BSP_OK;\
	}while(0)
#if 1//INCLUDE_SYSTEM_TEST



#if 1//defined(BSP_ACM_SIO_NUM) && !defined(BSP_ACM_SIO_NUM_SAMPLE)
static unsigned int sg_AcmDevTbl[CONFIG_FD_CDC_ACM+1]=
{
    UDI_ACM_OM_ID,
    UDI_ACM_AT_ID,
    //UDI_ACM_CTRL_ID,
    UDI_ACM_LTE_DIAG_ID,
    #if defined (BOARD_ASIC) && defined (VERSION_V3R2)
    UDI_ACM_GPS_ID,
    #endif
    0
};

typedef void (*jtest_func)(void *arg);

typedef struct {
    void *arg;
    jtest_func func;
    const char *name;
} jtest_arg_t;

typedef struct {
	BSP_S32 s32Fd;
	BSP_U32 u32Cnt;
	BSP_U32 u32AcmNum;
	BSP_U32 u32MaxSize;
	BSP_BOOL bCacheAlign;
	BSP_BOOL bRead;
} jtest_rw_block_t;

BSP_S32 saved_s32Fd[CONFIG_FD_CDC_ACM] = {0};
jtest_rw_block_t* taskarg[CONFIG_FD_CDC_ACM];
jtest_rw_block_t* taskargwt[CONFIG_FD_CDC_ACM];
char *gCompare_buf[256];
BSP_S32 readdone_times = 0,gAcmNum =0;

#define BSP_ACM_TEST_CHECK_RET(ret) \
do{\
    if (0 != ret)\
    {\
        printk("ACM ERROR, line:%d, ret:%d\n", __LINE__, ret);\
        return ret;\
    }\
}while(0)

#ifdef __KERNEL__
#define TG_ACM_MALLOC(size, flag) kmalloc(size, GFP_KERNEL)
#define TG_ACM_FREE(ptr) kfree(ptr)
#define TG_ACM_GET_LIGN_ADDR(ptr) ((BSP_U8*)((BSP_U32)ptr + (32 - ((BSP_U32)ptr & 31))))

#endif


BSP_U8* sg_AcmDevName[BSP_ACM_MAX_NUM] = {0};

/*随机生成[a,b]区间的一个数*/
static int acm_rand(int a,int b)
{
#if 0
	double u_ran = 0.0;
	int uab_ran = 0;
    unsigned int seed = clock();

	srand(seed);
	u_ran = rand()/(double)RAND_MAX;
	uab_ran = (int)((b-a+1)*u_ran) + a;
	return uab_ran;
#endif
    return 0;
}

static int ReadAll(int fd, char* buff, int len)
{
    int left = len;
    //int readLen = 0;
    int ret = 0;
    char* pCur = buff;

    //printk("udi_read all len:%d\n", len);
    do{
        ret = udi_read(fd, pCur, left);
        //printk("__ readed size:%d __\n", ret);
        if (ret < 0)
        {
            break;
        }
        left -= ret;
        pCur += ret;
    }while(left > 0);
    //printk("ReadAll BSP_OK, fd:%d, len:%d, first Data:0x%x\n", fd, len - left, *buff);
    return ret;
}

static int WriteAll(int fd, char* buff, int len)
{
    int left = len;
    //int readLen = 0;
    int ret = 0;
    char* pCur = buff;

    do{
        printk("WriteAll :  begin udi_write len:%d\n", left);
        ret = udi_write(fd, pCur, left);
        if (ret <= 0)
        {
            break;
        }

        left -= ret;
        pCur += ret;
        printk("WriteAll:  udi_writed len:%d, left:%d\n", ret, left);
    }while(left > 0);
    //printk("Write All, fd:%d, len:%d, first Data:0x%x\n", fd, len - left, *buff);
    return ret;
}

BSP_S32 Realloc(BSP_VOID)
{
    BSP_S32 s32Fd = 0;
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    UDI_OPEN_PARAM stOpenParam = {0};

    stOpenParam.devid = sg_AcmDevTbl[2];
    s32Fd = udi_open(&stOpenParam);
    if (s32Fd != BSP_ERROR)
    {
        printk("ACM_3 relloc buffer\n");
        stBuffInfo.u32BuffNum = 8;
        stBuffInfo.u32BuffSize = 2048;
        if(udi_ioctl(s32Fd, ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo) != BSP_OK)
        {
            printk("relloc buffer error\n");
            udi_close(s32Fd);
            return OK;
        }

        udi_close(s32Fd);
    }
    else
    {
        return BSP_ERROR;
    }
    return OK;
}
#ifdef CONFIG_SYN_HSFC_HSIC
static unsigned int sg_HSICAcmDevTbl[HSIC_FD_CDC_ACM_NUMS+1]=
{
    #ifdef USB_HSIC_SUPPORT_NCM
    UDI_ACM_HSIC_ACM0_ID,
    UDI_ACM_HSIC_ACM2_ID,
    UDI_ACM_HSIC_ACM4_ID,
    #else
    UDI_ACM_HSIC_ACM0_ID,
    UDI_ACM_HSIC_ACM1_ID,
    UDI_ACM_HSIC_ACM2_ID,
    UDI_ACM_HSIC_ACM3_ID,
    UDI_ACM_HSIC_ACM4_ID,
    UDI_ACM_HSIC_ACM5_ID,
    #endif
    UDI_ACM_HSIC_ACM6_ID,
    UDI_ACM_HSIC_ACM7_ID,
    UDI_ACM_HSIC_ACM8_ID,
    UDI_ACM_HSIC_ACM9_ID,
    UDI_ACM_HSIC_ACM10_ID,
    UDI_ACM_HSIC_ACM11_ID,
    UDI_ACM_HSIC_ACM12_ID,
    UDI_ACM_HSIC_ACM13_ID,
	UDI_ACM_HSIC_ACM14_ID,
    0
};
typedef struct {
       BSP_S32 openinfo;
	BSP_S32 readinfo;
	BSP_U32 writeinfo;
	BSP_U32 memallocinfo;
	BSP_U32 Maxsize;
	BSP_U32 circletimes;
	BSP_U32 totalcircletimes;
	BSP_U32 finished;
	BSP_U32 u32AcmNum;
} jtest_rw_state;
BSP_S32 s32Fd[HSIC_FD_CDC_ACM_NUMS] = {0};
jtest_rw_state *test_state[HSIC_FD_CDC_ACM_NUMS]={0};
jtest_rw_block_t* taskarg_HSIC[HSIC_FD_CDC_ACM_NUMS]={0};

void acm_oneport_read_send(int port, int size)
{
    char *buf;
    int i,j;
    UDI_OPEN_PARAM stOpenParam = {0};
    buf = kmalloc(size, GFP_KERNEL);
    if(buf == NULL)
    {
        os_printf("kmalloc error\n");
        test_state[port]->memallocinfo= 0;
        return;
    }
    memset(buf, 0, size);
if (s32Fd[port] == 0)
   {
    stOpenParam.devid = sg_HSICAcmDevTbl[port];
    s32Fd[port] = udi_open(&stOpenParam);
    if (s32Fd[port] == BSP_ERROR)
      {
        os_printf("udi_open / udi_close ACM:0x%x port=%d error\n", stOpenParam.devid,port);
        test_state[port]->openinfo =0;
        goto mem_free;
      }
        test_state[port]->openinfo =s32Fd[port];
    }
  if (ReadAll(s32Fd[port], buf, size) < 0)
    {
        os_printf("udi_read error port =%d\n",port);
        test_state[port]->readinfo = 0;
        goto port_close;
    }
    msleep(10);
  if (WriteAll(s32Fd[port], buf, size)<0)
    {
        os_printf("udi_write port =%d\n",port);
        test_state[port]->writeinfo = 0;
        goto port_close;
    }
     msleep(10);
port_close:
if (s32Fd[port] != 0)
{
    udi_close(s32Fd[port]);
   }
mem_free:
  if (NULL != buf)
    {
        kfree(buf);
    }
}
void acm_circle_rw_test(void* arg)
{
    int i,j;
	jtest_rw_block_t* taskarg_HSIC= (jtest_rw_block_t*)arg;
	BSP_U32 u32Cnt = taskarg_HSIC->u32Cnt;
	BSP_U32 u32AcmNum = taskarg_HSIC->u32AcmNum;
	BSP_U32 u32MaxSize = taskarg_HSIC->u32MaxSize;
	test_state[u32AcmNum] = (jtest_rw_state *)kmalloc(sizeof(jtest_rw_state),GFP_KERNEL);
	if(NULL==test_state[u32AcmNum])
	{
		os_printf("can not alloc struct test_state\n");
		return;
	}
	test_state[u32AcmNum]->Maxsize= u32MaxSize;
    test_state[u32AcmNum]->openinfo = 1;
    test_state[u32AcmNum]->readinfo = 1;
    test_state[u32AcmNum]->writeinfo = 1;
    test_state[u32AcmNum]->memallocinfo = 1;
    test_state[u32AcmNum]->Maxsize= u32MaxSize;
    test_state[u32AcmNum]->circletimes= 0;
    test_state[u32AcmNum]->totalcircletimes= 0;
    test_state[u32AcmNum]->finished= 0;
    test_state[u32AcmNum]->u32AcmNum= u32AcmNum;
	for (j=1; j <=u32Cnt ;j++)
	{
	 test_state[u32AcmNum]->totalcircletimes = j;
     for (i=1; i<=u32MaxSize; i++)
     {
       acm_oneport_read_send(u32AcmNum, i);
       test_state[u32AcmNum]->circletimes = i;
     }
    }
     test_state[u32AcmNum]->finished= 1;
	 os_printf("acm_circle_rw_test port =%d finshished totalcircletimes=%d \n",u32AcmNum,u32Cnt);
}
void showacminfo(int count)
{
	int i;
	if (count>=HSIC_FD_CDC_ACM_NUMS)
		{
                    printk("showacminfo  count  value error\n");
		      return;
	}
	for(i=0;i<count;i++)
	{
	 if (test_state[i] != NULL)
	 	{
			 printk("acm_circle_rw_test  finshished test_state[%d]->openinfo =%d \n",i,test_state[i]->openinfo );
			 printk("acm_circle_rw_test  finshished test_state[%d]->readinfo =%d \n",i, test_state[i]->readinfo );
			 printk("acm_circle_rw_test  finshished test_state[%d]->writeinfo =%d \n",i,test_state[i]->writeinfo );
			 printk("acm_circle_rw_test  finshished test_state[%d]->memallocinfo =%d \n",i, test_state[i]->memallocinfo );
			 printk("acm_circle_rw_test  finshished test_state[%d]->Maxsize =%d \n",i,test_state[i]->Maxsize );
			 printk("acm_circle_rw_test  finshished test_state[%d]->circletimes =%d \n",i, test_state[i]->circletimes );
			 printk("acm_circle_rw_test  finshished test_state[%d]->totalcircletimes =%d \n",i,test_state[i]->totalcircletimes );
			 printk("acm_circle_rw_test  finshished test_state[%d]->finished =%d \n\n\n",i, test_state[i]->finished );
	 	}
   }
}
void showacminfoExt(int count)
{
	if (count>=HSIC_FD_CDC_ACM_NUMS)
		{
                    printk("showacminfo  count  value error\n");
		      return;
	}

     if (test_state[count] != NULL)
     	{
		 int i = count;
		 printk("\n acm_circle_rw_test  finshished port= %d\n",count );
		 printk("acm_circle_rw_test  finshished test_state[%d]->openinfo =%d \n",i,test_state[i]->openinfo );
		 printk("acm_circle_rw_test  finshished test_state[%d]->readinfo =%d \n",i, test_state[i]->readinfo );
		 printk("acm_circle_rw_test  finshished test_state[%d]->writeinfo =%d \n",i,test_state[i]->writeinfo );
		 printk("acm_circle_rw_test  finshished test_state[%d]->memallocinfo =%d \n",i, test_state[i]->memallocinfo );
		 printk("acm_circle_rw_test  finshished test_state[%d]->Maxsize =%d \n",i,test_state[i]->Maxsize );
		 printk("acm_circle_rw_test  finshished test_state[%d]->circletimes =%d \n",i, test_state[i]->circletimes );
		 printk("acm_circle_rw_test  finshished test_state[%d]->totalcircletimes =%d \n",i,test_state[i]->totalcircletimes );
		 printk("acm_circle_rw_test  finshished test_state[%d]->finished =%d \n\n\n",i, test_state[i]->finished );
		 printk("================================================\n",count );
     	}
}
void acm_read_send(int readport, int writeport,int size)
{
	BSP_S32 s32Fd[2] = {0};
	char *buf;
	char  *bufwrite;
	int i,j;
	UDI_OPEN_PARAM stOpenParam = {0};
	if (readport==writeport)
	{
	   os_printf("readport==writeport error\n");
	   return;
	}
	buf = kmalloc(size, GFP_KERNEL);
	if(buf == NULL)
	{
	   os_printf("kmalloc buf error\n");
	   return;
	}
	memset(buf, 0, size);
	bufwrite = kmalloc(size, GFP_KERNEL);
	if(bufwrite == NULL)
	{
	   os_printf("kmalloc bufwrite error\n");
	   return;
	}
	memset(bufwrite, 0, size);
	 stOpenParam.devid = sg_HSICAcmDevTbl[readport];
	s32Fd[0] = udi_open(&stOpenParam);
	if (s32Fd[0] == BSP_ERROR)
	{
	   os_printf("udi_open / udi_close ACM:0x%x error\n", stOpenParam.devid);
	   goto mem_free;
	}
	stOpenParam.devid = sg_HSICAcmDevTbl[writeport];
	s32Fd[1] = udi_open(&stOpenParam);
	if (s32Fd[1]== BSP_ERROR)
	{
	   os_printf("udi_open / udi_close ACM:0x%x error\n", stOpenParam.devid);
	   goto mem_free;
	}
	j= udi_read(s32Fd[0], buf, size);
	os_printf("udi_read j= %d\n",j);
	if (j<0)
	{
	   os_printf("udi_read error\n\n\n\n");
	   goto port_close;
	}
	msleep(10);
	memcpy(buf, bufwrite, j);
	i = udi_write(s32Fd[1], bufwrite, j);
	os_printf("udi_write i= %d\n",i);
	if (i<0)
	{
	   os_printf("udi_write error\n\n\n\n\n");
	   goto port_close;
	}
	msleep(10);
	port_close:

	  udi_close(s32Fd[0]);
	  udi_close(s32Fd[1]);

	mem_free:
	if (NULL != buf)
	{
	  kfree(buf);
	}
	if (NULL != bufwrite)
	{
	  kfree(bufwrite);
	}
}
void acm_test_infinite(int readport, int writeport,int maxsize)
{
do{
      acm_read_send(readport,writeport,maxsize);
}while(1);
}
BSP_S32 ACM_RW_CIRCLE_TEST(BSP_U32 u32MaxSize, BSP_U32 ACM_NUM,BSP_U32 Circletimes)
{
   result_t rc = 0;
     if ((ACM_NUM ==1)||(ACM_NUM ==3)||(ACM_NUM ==5)||(ACM_NUM>=HSIC_FD_CDC_ACM_NUMS))
    	{
            printk("ACM_RW_CIRCLE_TEST  port num error \n");
	     return;
	}
	taskarg_HSIC[ACM_NUM] = (jtest_rw_block_t *)kmalloc(sizeof(jtest_rw_block_t),GFP_KERNEL);
	if(NULL==taskarg_HSIC[ACM_NUM])
	{
		os_printf("can not alloc struct jtest_rw_block_t\n");
		rc = -1;
		return;
	}
	taskarg_HSIC[ACM_NUM]->bCacheAlign = 0;
	taskarg_HSIC[ACM_NUM]->bRead = 0;
	taskarg_HSIC[ACM_NUM]->s32Fd =0;
	taskarg_HSIC[ACM_NUM]->u32Cnt =Circletimes;
	taskarg_HSIC[ACM_NUM]->u32AcmNum = ACM_NUM;
	taskarg_HSIC[ACM_NUM]->u32MaxSize = u32MaxSize;
	rc =  kernel_thread(acm_circle_rw_test, taskarg_HSIC[ACM_NUM], CLONE_VM);
	if (rc < 0)
	{
		os_printf("thread create failed\n");
	}
	return;
}
void acm_send_data(int port, int size, int off)
{
    BSP_S32 s32Fd;
    char *buf;
    int j;
    int sent_len;
    printk("qijiwen %s port:%d size:%d\n", port, size);
    UDI_OPEN_PARAM stOpenParam = {0};
    buf = kmalloc(size, GFP_KERNEL);
    if(buf == NULL)
    {
        os_printf("kmalloc error\n");
        return;
    }
    for(j=0;j<size;j++)
    {
        buf[j] = j + off;
        os_printf("0x%x ",buf[j]);
    }
    printk("\nqijiwen data to send end\n");
    stOpenParam.devid = UDI_ACM_CTRL_ID + port ;
    s32Fd = udi_open(&stOpenParam);
    if (s32Fd == BSP_ERROR)
    {
        os_printf("udi_open / udi_close ACM:0x%x error\n", stOpenParam.devid);
        goto mem_free;
    }
    if ((sent_len = udi_write(s32Fd, buf, size)) < 0)
    {
        os_printf("udi_write error\n");
        goto port_close;
    }
    printk("qijiwen sent_len:%d\n", sent_len);
port_close:
    udi_close(s32Fd);
mem_free:
    kfree(buf);
}
void acm_receive_data(int port, int size)
{
    BSP_S32 s32Fd;
    char *buf;
    int j;
    int received, total = 0;
    printk("qijiwen port:%d size:%d\n", port, size);
    UDI_OPEN_PARAM stOpenParam = {0};
    buf = kmalloc(size, GFP_KERNEL);
    if(buf == NULL)
    {
        os_printf("kmalloc error\n");
        return;
    }
    memset(buf, 0, size);
    stOpenParam.devid = UDI_ACM_CTRL_ID + port ;
    s32Fd = udi_open(&stOpenParam);
    if (s32Fd == BSP_ERROR)
    {
        os_printf("udi_open / udi_close ACM:0x%x error\n", stOpenParam.devid);
        goto mem_free;
    }
    while(total < size)
    {
        if ((received = udi_read(s32Fd, buf + total, size - total)) < 0)
        {
            os_printf("udi_read error\n");
            goto port_close;
        }
        printk("qijiwen data received %d:\n", received);
        total += received;
    }
    for(j=0;j<size;j++)
    {
        printk("0x%x ",buf[j]);
    }
    os_printf("\ndata received end\n");
port_close:
    udi_close(s32Fd);
mem_free:
    kfree(buf);
}

struct semaphore  HSICACMReadComplete[HSIC_FD_CDC_ACM_NUMS];
struct semaphore  HSICACMWriteComplete[HSIC_FD_CDC_ACM_NUMS];
struct semaphore  HSICACMReadAsyncComplete[HSIC_FD_CDC_ACM_NUMS];
struct semaphore  HSICACMWriteAsyncComplete[HSIC_FD_CDC_ACM_NUMS];



VOID HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_1(VOID)
{

    up(&HSICACMReadAsyncComplete[0]);
}
VOID HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_2(VOID)
{
    up(&HSICACMReadAsyncComplete[1]);
}
VOID HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_3(VOID)
{
    up(&HSICACMReadAsyncComplete[2]);
}
VOID HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_4(VOID)
{
    up(&HSICACMReadAsyncComplete[3]);
}
VOID HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_5(VOID)
{
    up(&HSICACMReadAsyncComplete[4]);
}VOID HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_6(VOID)
{
    up(&HSICACMReadAsyncComplete[5]);
}


ACM_READ_DONE_CB_T sg_HSICAcmReadBaseCB[HSIC_FD_CDC_ACM_NUMS] = {
    HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_1,
    HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_2,
    HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_3,
    HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_4,
    HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_5,
    HSIC_ACM_LONG_TIME_READ_ASYNC_DONE_CB_6,
};



VOID HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_1(VOID)
{
    up(&HSICACMWriteAsyncComplete[0]);
}
VOID HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_2(VOID)
{
    up(&HSICACMWriteAsyncComplete[1]);
}
VOID HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_3(VOID)
{
    up(&HSICACMWriteAsyncComplete[2]);
}
VOID HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_4(VOID)
{
    up(&HSICACMWriteAsyncComplete[3]);
}
VOID HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_5(VOID)
{
    up(&HSICACMWriteAsyncComplete[4]);
}VOID HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_6(VOID)
{
    up(&HSICACMWriteAsyncComplete[5]);
}

ACM_READ_DONE_CB_T sg_HSICAcmWriteBaseCB[HSIC_FD_CDC_ACM_NUMS] = {
    HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_1,
    HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_2,
    HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_3,
    HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_4,
    HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_5,
    HSIC_ACM_LONG_TIME_Write_ASYNC_DONE_CB_6,
};

int ACM_HSIC_READ_WRITE_ASYNC_Task(void* arg)
{
    jtest_rw_block_t* taskarg = (jtest_rw_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 u32TestLen = taskarg->u32MaxSize;
    BSP_U32 ACM_NUM = taskarg->u32AcmNum;
    BSP_U32 totaltimes = taskarg->u32Cnt;
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    //BSP_S32 s32Times = 10000;

    printk("ASYNC_READ read len:%d\n", u32TestLen);

    sema_init(&HSICACMReadAsyncComplete[ACM_NUM], 0);
    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_READ_CB, sg_HSICAcmReadBaseCB[ACM_NUM]) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d, line:%d\n", ACM_NUM, s32Fd, __LINE__);
	 return;
    }

   sema_init(&HSICACMWriteAsyncComplete[ACM_NUM], 0);
    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_WRITE_CB, sg_HSICAcmWriteBaseCB[ACM_NUM]) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d, line:%d\n", ACM_NUM, s32Fd, __LINE__);
	 return;
    }


    while(1)
    {

        down(&HSICACMReadAsyncComplete[ACM_NUM]);
        ret = udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", ACM_NUM);
	     test_state[ACM_NUM]->readinfo = 0;
            s32Status = -1;
	     return;
        }

	 if ((ACM_NUM ==1)||(ACM_NUM ==3)||(ACM_NUM ==5))
	 	{
                 stRWInfo.u32Size = 0;
	          stRWInfo.pDrvPriv=NULL;
	 	}

        ret = udi_ioctl(s32Fd, ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", ACM_NUM);
	     test_state[ACM_NUM]->writeinfo= 0;
            s32Status = -1;
            return;
        }
        down(&HSICACMWriteAsyncComplete[ACM_NUM]);

	 test_state[ACM_NUM]->circletimes++;
	 if (test_state[ACM_NUM]->circletimes==test_state[ACM_NUM]->Maxsize)
	{
            test_state[ACM_NUM]->totalcircletimes++;
	     test_state[ACM_NUM]->circletimes= 0;
	}
	 if(test_state[ACM_NUM]->totalcircletimes == totaltimes)
	 {
		test_state[ACM_NUM]->finished= 1;
		printk("acm_circle_rw_test port =%d finshished totalcircletimes=%d \n",ACM_NUM,totaltimes);
		return;
	 }
    }

    return s32Status;
}


BSP_S32 BSP_ACM_HSIC_READ_WRITE_ASYNC(BSP_U32 u32MaxSize, BSP_U32 ACM_NUM,BSP_U32 Circletimes)
{
    char *buf;
    result_t rc = 0;

    UDI_OPEN_PARAM stOpenParam = {0};
    if (((ACM_NUM !=1)&&(ACM_NUM !=3)&&(ACM_NUM !=5))||(ACM_NUM>=HSIC_FD_CDC_ACM_NUMS))
    	{
            printk("BSP_ACM_HSIC_READ_WRITE_ASYNC  port num error \n");
	     return;
	}

    taskarg_HSIC[ACM_NUM] = (jtest_rw_block_t *)kmalloc(sizeof(jtest_rw_block_t),GFP_KERNEL);
    test_state[ACM_NUM] = (jtest_rw_state *)kmalloc(sizeof(jtest_rw_state),GFP_KERNEL);
   if((NULL==taskarg_HSIC[ACM_NUM])||(NULL==test_state[ACM_NUM]))
  {
		printk("can not alloc struct jtest_rw_block_t  or jtest_rw_state\n");
		return -1;
   }

   if (s32Fd[ACM_NUM] == 0)
   {
    stOpenParam.devid = sg_HSICAcmDevTbl[ACM_NUM];
    s32Fd[ACM_NUM] = udi_open(&stOpenParam);
    if (s32Fd[ACM_NUM] == BSP_ERROR)
      {
        os_printf("udi_open / udi_close ACM:0x%x port=%d error\n", stOpenParam.devid,ACM_NUM);

      }
    }

	taskarg_HSIC[ACM_NUM]->bCacheAlign = 0;
	taskarg_HSIC[ACM_NUM]->bRead = 0;
	taskarg_HSIC[ACM_NUM]->s32Fd =s32Fd[ACM_NUM];
	taskarg_HSIC[ACM_NUM]->u32Cnt =Circletimes;
	taskarg_HSIC[ACM_NUM]->u32AcmNum = ACM_NUM;
	taskarg_HSIC[ACM_NUM]->u32MaxSize = u32MaxSize;

	test_state[ACM_NUM]->Maxsize= u32MaxSize;
	test_state[ACM_NUM]->openinfo = s32Fd[ACM_NUM];
	test_state[ACM_NUM]->readinfo = 1;
	test_state[ACM_NUM]->writeinfo = 1;
	test_state[ACM_NUM]->memallocinfo = 1;
	test_state[ACM_NUM]->circletimes= 0;
	test_state[ACM_NUM]->totalcircletimes= 0;
	test_state[ACM_NUM]->finished= 0;
	test_state[ACM_NUM]->u32AcmNum= ACM_NUM;

	 kernel_thread(ACM_HSIC_READ_WRITE_ASYNC_Task,taskarg_HSIC[ACM_NUM],CLONE_VM);

	if (rc < 0)
	{
		os_printf("thread create failed\n");
	}

       return BSP_OK;

}
#endif

BSP_S32 BSP_ACM_CMP_MEM_WITH_VALUE(BSP_U8* pAddr, BSP_U32 u32Size, BSP_U8 u8CmpValue, BSP_U32 u32Line)
{
    BSP_U32 u32Cnt = 0;
    for(; u32Cnt < u32Size; pAddr++,u32Cnt++)
    {
        if (*pAddr != u8CmpValue)
        {
            printk("data:%d cmp fail, data in buffer:0x%x, value:0x%x, line:%d\n", u32Cnt, *pAddr, u8CmpValue, u32Line);
            return BSP_ERROR;
        }
    }
    return OK;
}

struct semaphore  READ_COMPLETE[6];

VOID BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_1(VOID)
{
    printk("<read CB> acm test, acm:%d read done\n", 1);
    up(&READ_COMPLETE[0]);
}
VOID BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_2(VOID)
{
    printk("<read CB> acm test, acm:%d read done\n", 2);
    up(&READ_COMPLETE[1]);
}
VOID BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_3(VOID)
{
    printk("<read CB> acm test, acm:%d read done\n", 3);
    up(&READ_COMPLETE[2]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_4(VOID)
{
    printk("<read CB> acm test, acm:%d read done\n", 4);
    up(&READ_COMPLETE[3]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_5(VOID)
{
    printk("<read CB> acm test, acm:%d read done\n", 5);
    up(&READ_COMPLETE[4]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_6(VOID)
{
    printk("<read CB> acm test, acm:%d read done\n", 6);
    up(&READ_COMPLETE[5]);
}

ACM_READ_DONE_CB_T sg_AcmReadBaseCB[6] = {
    BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_1,
    BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_2,
    BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_3,
    BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_4,
    BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_5,
    BSP_ACM_TEST_ASYNC_BASE_READ_DONE_CB_6,
};

BSP_S32 BSP_ACM_TEST_ASYNC_READ_BASE(BSP_U32 u32Len, BSP_U32 u32CheckLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = ((u32Len + 32)&(~0x1F));
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("TEST_ASYNC_READ buffer size:%d, read len:%d\n", u32TestLen, u32Len);

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	 stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
	 sema_init(&READ_COMPLETE[i], 0);
#if 0
        if((READ_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating READ_COMPLETE0 semaphore  ");
            goto ACM_ERR_ASYNC_BASE;
        }
#endif
        if(s32Fd[i] != BSP_ERROR)
        {
            ret = udi_ioctl(s32Fd[i], ACM_IOCTL_SET_READ_CB, sg_AcmReadBaseCB[i]);
            if (ret != BSP_OK)
            {
                printk("udi_ioctl ACM:%d error:%d, line:%d\n", i, ret, __LINE__);
                goto ACM_ERR_ASYNC_BASE;
            }
        }
        else
        {
            printk("udi_open ACM:%d error:%d, line:%d\n", i, s32Fd[i], __LINE__);
            goto ACM_ERR_ASYNC_BASE;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&READ_COMPLETE[i]);

        ret = udi_ioctl(s32Fd[i], ACM_IOCTL_GET_RD_BUFF, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_ERR_ASYNC_BASE;
        }
        /* 处理数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(stRWInfo.pBuffer, stRWInfo.u32Size, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, stRWInfo.u32Size, *stRWInfo.pBuffer);
        }
        else
        {
            s32Status = BSP_ERROR;
        }

        /* 归还buffer */
        ret = udi_ioctl(s32Fd[i], ACM_IOCTL_RETURN_BUFF, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_ERR_ASYNC_BASE;
        }
    }

ACM_ERR_ASYNC_BASE:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            if(u32CheckLen < u32Len)
            {
                stBuffInfo.u32BuffNum = 8;
                stBuffInfo.u32BuffSize = 2048;
                if(udi_ioctl(s32Fd[i], ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo) != BSP_OK)
                {
                    printk("relloc buffer error\n");
                }
            }
            udi_close(s32Fd[i]);
        }
    }

    return s32Status;
}

BSP_S32 BSP_ACM_TEST_BLOCK_READ_BASE(BSP_U32 u32TestLen, BSP_U32 u32CheckLen, BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);
    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM + 32,1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        bufFree = buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    }
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	 stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        if ((0 == u32TestLen && ret > 0) || (0 != u32TestLen && ret < 0))
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i], u32CheckLen, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            if(u32CheckLen < u32TestLen)
            {
                printk("relloc buffer\n");
                stBuffInfo.u32BuffNum = 8;
                stBuffInfo.u32BuffSize = 2048;
                if(udi_ioctl(s32Fd[i], ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo) != BSP_OK)
                {
                    printk("relloc buffer error\n");
                }
            }
            mdelay(500);
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_BLOCK_READ_ALL_BASE(BSP_U32 u32TestLen, BSP_U32 u32CheckLen, BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);
    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        buf = bufFree = TG_ACM_MALLOC( u32BufferLen*BSP_ACM_SIO_NUM,1);
    }
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	 stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = ReadAll(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i], u32CheckLen, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            if(u32CheckLen < u32TestLen)
            {
                printk("relloc buffer\n");
                stBuffInfo.u32BuffNum = 8;
                stBuffInfo.u32BuffSize = 2048;
                if(udi_ioctl(s32Fd[i], ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo) != BSP_OK)
                {
                    printk("relloc buffer error\n");
                }
            }
            mdelay(500);
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(BSP_U32 u32TestLen1, BSP_U32 u32TestLen2, BSP_U32 u32CheckLen1,  BSP_U32 u32CheckLen2,BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen1 + u32TestLen2 + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen1+u32TestLen2);
    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        bufFree = buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    }
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	 stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen1);
        if ((0 == u32TestLen1 && ret > 0) || (0 != u32TestLen1 && ret < 0))
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i], u32CheckLen1, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
        mdelay(500);
        ret = udi_read(s32Fd[i], pUsedBuf[i]+u32TestLen1, u32TestLen2);
        if ((0 == u32TestLen2 && ret > 0) || (0 != u32TestLen2 && ret < 0))
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i]+u32TestLen1, u32CheckLen2, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            if((u32CheckLen1+u32CheckLen2) < (u32TestLen1+u32TestLen2))
            {
                printk("relloc buffer\n");
                stBuffInfo.u32BuffNum = 8;
                stBuffInfo.u32BuffSize = 2048;
                if(udi_ioctl(s32Fd[i], ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo) != BSP_OK)
                {
                    printk("relloc buffer error\n");
                }
            }
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_BLOCK_READ_TWICE_ALL_BASE(BSP_U32 u32TestLen1, BSP_U32 u32TestLen2, BSP_U32 u32CheckLen1,  BSP_U32 u32CheckLen2,BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen1 + u32TestLen2 + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen1+u32TestLen2);
    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        buf = bufFree = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM+1,1);
    }
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	 stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen1);
        printk("len = %d \n", ret);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i], u32CheckLen1, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }

        ret = ReadAll(s32Fd[i], pUsedBuf[i]+u32TestLen1, u32TestLen2);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i]+u32TestLen1, u32CheckLen2, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, u32CheckLen2, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            if((u32CheckLen1+u32CheckLen2) < (u32TestLen1+u32TestLen2))
            {
                printk("relloc buffer\n");
                stBuffInfo.u32BuffNum = 8;
                stBuffInfo.u32BuffSize = 2048;
                if(udi_ioctl(s32Fd[i], ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo) != BSP_OK)
                {
                    printk("relloc buffer error\n");
                }
            }
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}



BSP_S32 BSP_ACM_TEST_INIT(VOID)
{
    BSP_S32 i;
    BSP_U8* pStrName;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    UDI_OPEN_PARAM stOpenParam = {0};

    if (NULL != sg_AcmDevName[0])
    {
        return 0;
    }
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pStrName = TG_ACM_MALLOC(32, 1);
        if (NULL == pStrName)
        {
            printk("BSP_ACM_TEST_INIT malloc fail\n");
            return -1;
        }
        memset(pStrName, 0, 32);
        sprintf(pStrName, "%s%d", ACM_DEV, i+1);
        sg_AcmDevName[i] = pStrName;
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i] != BSP_ERROR)
        {
            /* 默认设置为不拷贝属性 */
            udi_ioctl(s32Fd[i], ACM_IOCTL_WRITE_DO_COPY, 0);
            udi_close(s32Fd[i]);
        }
    }
    return 0;
}

BSP_S32 BSP_ACM_TEST_000(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};

    if (0 != BSP_ACM_TEST_INIT())
    {
        printk("BSP_ACM_TEST_000 Fail\n");
        goto BSP_ACM_TEST_000_ERR;
    }
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	 stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i] != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
        else
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto BSP_ACM_TEST_000_ERR;
        }
    }
BSP_ACM_TEST_000_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

	RUN_BASE_TEST(" 打开关闭虚拟串口设备 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_000_CYC(VOID)
{
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Times = 10;
    BSP_S32 i, j;
    BSP_S32 s32Status = 0;
    UDI_OPEN_PARAM stOpenParam = {0};

    for(j = 0; j < s32Times; j++)
    {
         for (i = 0; i < BSP_ACM_SIO_NUM; i++)
         {
	     stOpenParam.devid = sg_AcmDevTbl[i];
	     s32Fd[i] = udi_open(&stOpenParam);
            if(s32Fd[i] != BSP_ERROR)
            {
              if(udi_close(s32Fd[i]) != BSP_OK)
              {
                  printk("acm : %d  udi_close error\n", i);
                  s32Status = -1;
              }
            }
            else
            {
                printk("udi_open  ACM:%d error:%d\n", i, s32Fd[i]);
                s32Status = -1;
            }
            s32Fd[i] = 0;
         }
    }

    RUN_BASE_TEST(" 循环打开关闭虚拟串口设备 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_000_OPEN1_CLOSE2(VOID)
{
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    BSP_S32 s32Status = 0;
    UDI_OPEN_PARAM stOpenParam = {0};

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
        if(s32Fd[i] != BSP_ERROR)
        {
            if(udi_close(s32Fd[i]) != BSP_OK)
            {
                printk(" acm: %d  first udi_close error  \n", i);
                s32Status = -1;
            }
            if(udi_close(s32Fd[i]) != BSP_OK)
            {
                printk(" acm: %d  secend udi_close error  \n", i);
                s32Status = -1;
            }
        }
        else
        {
            printk("udi_open ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
        }
    }

    RUN_BASE_TEST(" 打开1次关闭2次虚拟串口设备 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_000_OPEN2_CLOSE1(VOID)
{
    BSP_S32 s32FdOk[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    BSP_S32 s32Status = 0;
    UDI_OPEN_PARAM stOpenParam = {0};

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32FdOk[i] = udi_open(&stOpenParam);
        if(s32FdOk[i] != BSP_ERROR)
        {
            if((s32Fd[i] = udi_open(&stOpenParam)) != BSP_ERROR)
            {
                printk("udi_open ACM Error:open success at the second time/n");
                s32Status = -1;
            }
        }
        else
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
        }
        if(udi_close(s32FdOk[i]) != BSP_OK)
        {
            s32Status = -1;
        }
    }

    RUN_BASE_TEST(" 打开2次关闭1次虚拟串口设备 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_001(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(200, 200, FALSE);
	RUN_BASE_TEST(" 同步读小于512字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_001_ALIGN(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(200, 200, TRUE);
	RUN_BASE_TEST(" 同步读小于512字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_001_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(200, 200);
    RUN_BASE_TEST(" 异步读小于512字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_002(VOID)
{
    BSP_S32 s32Status = 0;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(512, 512, FALSE);
	RUN_BASE_TEST(" 读等于512字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_002_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(512, 512, TRUE);
	RUN_BASE_TEST(" 读等于512字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_002_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(512, 512);
    RUN_BASE_TEST(" 异步读等于512字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_003(VOID)
{
    BSP_S32 s32Status = 0;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(513, 513, FALSE);
	RUN_BASE_TEST(" 读大于于512字节(但非512整数倍)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_003_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(513, 513, TRUE);
	RUN_BASE_TEST(" 读大于于512字节(但非512整数倍)的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_003_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(513, 513);
    RUN_BASE_TEST(" 异步读大于于512字节(但非512整数倍)的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_004(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(1024, 1024, FALSE);
	RUN_BASE_TEST(" 读大于于512字节(是512整数倍)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_004_ALIGN(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(1024, 1024, TRUE);
	RUN_BASE_TEST(" 读大于于512字节(是512整数倍)的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_004_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(1024, 1024);
    RUN_BASE_TEST(" 异步读大于于512字节(是512整数倍)的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_005(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(2048, 2048, FALSE);
	RUN_BASE_TEST(" 读大于于512字节( 边界值)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_005_ALIGN(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(2048, 2048, TRUE);
	RUN_BASE_TEST(" 读大于于512字节( 边界值 )的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_005_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(2048, 2048);
    RUN_BASE_TEST(" 异步读大于于512字节( 边界值 )的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_006(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(0, 0, FALSE);
	RUN_BASE_TEST(" 同步读0 字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_006_ALIGN(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(0, 0, TRUE);
	RUN_BASE_TEST(" 同步读0 字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_006_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(0, 0);
    RUN_BASE_TEST(" 异步读0 字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_007(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(2048+1, 2048, FALSE);
    RUN_BASE_TEST(" 同步读2049 字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_007_ALIGN(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(2048+1, 2048, TRUE);
    RUN_BASE_TEST(" 同步读2049 字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_007_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(2048+1, 2048);
    RUN_BASE_TEST(" 异步读2049 字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_008(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(201, 200, FALSE);
    RUN_BASE_TEST(" 同步读数据，读201，收200(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_008_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(201, 200);
    RUN_BASE_TEST(" 异步读数据，读201，收到200 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_009(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(200, 312, 200, 312, FALSE);
    RUN_BASE_TEST(" 同步读两次数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_009_ASYNC(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_ASYNC_READ_BASE(200, 512);
    RUN_BASE_TEST(" 异步读数据 ，读200，收到512", s32Status);
}

BSP_S32 BSP_ACM_TEST_009_CLOSE(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(200, 200, FALSE);
    if(s32Status != BSP_OK)
    {
        printk("first read error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(312, 312, FALSE);
    RUN_BASE_TEST(" 同步读两次数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_010(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(200, 400, 200, 312, FALSE);
    RUN_BASE_TEST(" 同步读两次数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_010_CLOSE(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(200, 200, FALSE);
    if(s32Status != BSP_OK)
    {
        printk("first read error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(400, 312, FALSE);
    RUN_BASE_TEST(" 同步读两次数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_011(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(200, 400, 200, 200, FALSE);
    RUN_BASE_TEST(" 同步读两次数据第一次读200，第二次读400(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_011_ALL(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_ALL_BASE(200, 400, 200, 400, FALSE);
    RUN_BASE_TEST(" 同步读两次数据第一次读200，第二次读400(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_011_CLOSE(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(200, 200, FALSE);
    if(s32Status != BSP_OK)
    {
        printk("first read error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(400, 200, FALSE);
    RUN_BASE_TEST(" 同步读两次数据第一次读200，第二次读400(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_011_CLOSE_ALL(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(200, 200, FALSE);
    if(s32Status != BSP_OK)
    {
        printk("first read error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_BLOCK_READ_ALL_BASE(400, 400, FALSE);
    RUN_BASE_TEST(" 同步读两次数据第一次读200，第二次读400(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_012(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(512, 3, 512, 3, FALSE);
    RUN_BASE_TEST(" 同步读两次数据第一次读512，第二次读3(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_012_CLOSE(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(512, 512, FALSE);
    if(s32Status != BSP_OK)
    {
        printk("first read error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_BLOCK_READ_BASE(3, 3, FALSE);
    RUN_BASE_TEST(" 同步读两次数据第一次读512，第二次读3(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_013(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(200, 0, 200, 0, FALSE);
    RUN_BASE_TEST(" 同步读数据第一次读200，第二次读0(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_014(VOID)
{
    BSP_S32 s32Status;

    s32Status = BSP_ACM_TEST_BLOCK_READ_TWICE_BASE(512, 0, 512, 0, FALSE);
    RUN_BASE_TEST(" 同步读数据第一次读512，第二次读0(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_015(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 200;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        printk("malloc buf error\n");
        return BSP_ERROR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto BSP_ACM_TEST_015_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i] != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != BSP_ERROR)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
        }
    }

BSP_ACM_TEST_015_ERR:
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    RUN_BASE_TEST(" 同步关闭串口后读200 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_BLOCK_WRITE_BASE(BSP_U32 u32TestLen, BSP_U32 u32CheckLen, BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    bufFree = TG_ACM_MALLOC((u32BufferLen*BSP_ACM_SIO_NUM)+32,1);
    buf = TG_ACM_GET_LIGN_ADDR(bufFree);

    if (NULL == buf)
    {
        goto ACM_BLOCK_WRITE_BASE_ERR;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (bCacheAlign)
        {
            pUsedBuf[i] = buf + i * u32BufferLen;
        }
        else
        {
            pUsedBuf[i] = buf+1 + i * u32BufferLen;
        }

        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_WRITE_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        //ret = WriteAll(s32Fd[i], pUsedBuf[i], u32TestLen);
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != u32CheckLen)
        {
            printk("ACM:%d write error,ret=%d, u32CheckLen=%d pUsedBuf[i]=0x%x\n", i,ret,u32CheckLen,(BSP_U32)pUsedBuf[i]);
            s32Status = -1;
            goto ACM_BLOCK_WRITE_BASE_ERR;
        }
    }

ACM_BLOCK_WRITE_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_BLOCK_WRITE_TWICE_BASE(BSP_U32 u32TestLen1, BSP_U32 u32TestLen2, BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32BufferLen = ((u32TestLen1+u32TestLen2 + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC((u32BufferLen*BSP_ACM_SIO_NUM),1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        buf = bufFree = TG_ACM_MALLOC( (u32BufferLen*BSP_ACM_SIO_NUM+1),1);
    }

    if (NULL == buf)
    {
        goto ACM_BLOCK_WRITE_BASE_ERR;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_WRITE_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        //ret = WriteAll(s32Fd[i], pUsedBuf[i], u32TestLen);
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen1);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_WRITE_BASE_ERR;
        }
        ret = udi_write(s32Fd[i], pUsedBuf[i]+u32TestLen1, u32TestLen2);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            continue;
            //goto ACM_BLOCK_WRITE_BASE_ERR;
        }
    }

ACM_BLOCK_WRITE_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}

struct semaphore   WRITE_COMPLETE[6];

VOID BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_1(char* pDonebuff, int size)
{
    printk("<write CB> acm test, acm:%d write OK\n", 1);
    up(&WRITE_COMPLETE[0]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_2(char* pDonebuff, int size)
{
    printk("<write CB> acm test, acm:%d write OK\n", 2);
    up(&WRITE_COMPLETE[1]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_3(char* pDonebuff, int size)
{
    printk("<write CB> acm test, acm:%d write OK\n", 3);
    up(&WRITE_COMPLETE[2]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_4(char* pDonebuff, int size)
{
    printk("<write CB> acm test, acm:%d write OK\n", 4);
    up(&WRITE_COMPLETE[3]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_5(char* pDonebuff, int size)
{
    printk("<write CB> acm test, acm:%d write OK\n", 5);
    up(&WRITE_COMPLETE[4]);
}

VOID BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_6(char* pDonebuff, int size)
{
    printk("<write CB> acm test, acm:%d write OK\n", 6);
    msleep(10);
    up(&WRITE_COMPLETE[5]);
}

ACM_WRITE_DONE_CB_T sg_AcmWriteBaseCB[6] = {
    BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_1,
    BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_2,
    BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_3,
    BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_4,
    BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_5,
    BSP_ACM_TEST_ASYNC_BASE_WRITE_DONE_CB_6,
};

BSP_S32 BSP_ACM_TEST_ASYNC_WRITE_BASE(BSP_U32 u32TestLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F));
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("TEST_ASYNC_WRITE buffer size:%d, write len:%d\n", u32BufferLen, u32TestLen);
    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_ERR_ASYNC_WRITE_BASE;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
	 sema_init(&WRITE_COMPLETE[i], 0);
#if 0
        if((WRITE_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
#endif
        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_WRITE_CB, sg_AcmWriteBaseCB[i]) != BSP_OK)
        {
            printk("udi_ioctl ACM:%d error:%d, line:%d\n", i, s32Fd[i], __LINE__);
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stRWInfo.pBuffer = pUsedBuf[i];
        stRWInfo.u32Size = u32TestLen;
        ret = udi_ioctl(s32Fd[i], ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    /* 只等最后一个就可以*/
    //for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&WRITE_COMPLETE[BSP_ACM_SIO_NUM-1]);
    }

ACM_ERR_ASYNC_WRITE_BASE:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i] != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_ASYNC_WRITE_TWICE_BASE(BSP_U32 u32TestLen1, BSP_U32 u32TestLen2)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen1+u32TestLen2 + 32)&(~0x1F));
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("TEST_ASYNC_WRITE buffer size:%d, write len:%d\n", u32BufferLen, u32TestLen1+u32TestLen2);
    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_ERR_ASYNC_WRITE_BASE;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
	 sema_init(&WRITE_COMPLETE[i], 0);
#if 0
        if((WRITE_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
#endif
        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_WRITE_CB, sg_AcmWriteBaseCB[i]) != BSP_OK)
        {
            printk("udi_ioctl ACM:%d error:%d, line:%d\n", i, s32Fd[i], __LINE__);
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stRWInfo.pBuffer = pUsedBuf[i];
        stRWInfo.u32Size = u32TestLen1;
        ret = udi_ioctl(s32Fd[i], ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&WRITE_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stRWInfo.pBuffer = pUsedBuf[i]+u32TestLen1;
        stRWInfo.u32Size = u32TestLen2;
        ret = udi_ioctl(s32Fd[i], ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_ERR_ASYNC_WRITE_BASE;
        }
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&WRITE_COMPLETE[i]);
    }

ACM_ERR_ASYNC_WRITE_BASE:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i] != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}


BSP_S32 BSP_ACM_TEST_016(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(200, 200, FALSE);
    RUN_BASE_TEST(" 向主机写入小于512字节(非512整数倍)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_016_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(200, 200, TRUE);
	RUN_BASE_TEST(" 向主机写入小于512字节(非512整数倍)的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_016_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(200);
	RUN_BASE_TEST(" 向主机异步写入小于512字节(非512整数倍)的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_017(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(512, 512, FALSE);
	RUN_BASE_TEST(" 向主机写入512 字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_017_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(512, 512, TRUE);
	RUN_BASE_TEST(" 向主机写入512 字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_017_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(512);
	RUN_BASE_TEST(" 向主机异步写入512 字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_018(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(513, 513, FALSE);
	RUN_BASE_TEST(" 向主机写入513 字节(但非512整数倍)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_018_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(513, 513, TRUE);
	RUN_BASE_TEST(" 向主机写入513 字节(但非512整数倍)的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_018_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(513);
	RUN_BASE_TEST(" 向主机异步写513 字节(但非512整数倍)的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_019(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(1024, 1024, FALSE);
	RUN_BASE_TEST(" 向主机写入1024 字节(且为512整数倍)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_019_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(1024, 1024, FALSE);
	RUN_BASE_TEST(" 向主机写入1024 字节(且为512整数倍)的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_019_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(1024);
	RUN_BASE_TEST(" 向主机异步写入1024 字节(且为512整数倍)的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_020(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(2048, 2048, FALSE);
	RUN_BASE_TEST(" 向主机写入2048 字节(且为512整数倍)的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_020_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(2048, 2048, FALSE);
	RUN_BASE_TEST(" 向主机写入2048 字节(且为512整数倍)的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_020_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(2048);
	RUN_BASE_TEST(" 向主机异步写入2048 字节(且为512整数倍)的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_021(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(0, 0, FALSE);
    if(s32Status == -1)
    {
         s32Status = 0;
    }
    RUN_BASE_TEST(" 向主机写入0 字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_021_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(0, 0, FALSE);
    if(s32Status == -1)
    {
         s32Status = 0;
    }
    RUN_BASE_TEST(" 向主机写入0 字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_021_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(0);
    if(s32Status == -1)
    {
         s32Status = 0;
    }
    RUN_BASE_TEST(" 向主机异步写入0 字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_022(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(2049, 2048, FALSE);
	RUN_BASE_TEST(" 向主机写入2049 字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_022_ALIGN(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(2049, 2048, FALSE);
	RUN_BASE_TEST(" 向主机写入2049 字节的数据(buffer cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_022_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(2049);
	RUN_BASE_TEST(" 向主机异步写入2049 字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_023(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(200, 200, FALSE);
	RUN_BASE_TEST(" 向主机写入200 字节的数据(buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_023_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(200);
	RUN_BASE_TEST(" 向主机异步写入200 字节的数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_024(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_TWICE_BASE(224, 288, FALSE);
	RUN_BASE_TEST(" 向主机同步两次写入数据224+288 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_024_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_TWICE_BASE(224, 288);
	RUN_BASE_TEST(" 向主机异步两次写入数据224+288 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_025(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_TWICE_BASE(512, 1, FALSE);
	RUN_BASE_TEST(" 向主机同步两次写入数据512+1 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_025_CLOSE(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(512, 512, FALSE);
    if(s32Status != BSP_OK)
    {
        printk("first write error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BASE(1, 1,  FALSE);
    RUN_BASE_TEST(" 向主机同步两次写入数据512+1 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_025_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_TWICE_BASE(512, 1);
	RUN_BASE_TEST(" 向主机异步两次写入数据512+1 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_025_ASYNC_CLOSE(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(512);
    if(s32Status != BSP_OK)
    {
        printk("first write error\n");
	 return BSP_ERROR;
    }
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_BASE(1);
    RUN_BASE_TEST(" 向主机异步两次写入数据512+1 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_026(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_TWICE_BASE(512, 0, FALSE);
    if(s32Status == -1)
    {
         s32Status = 0;
    }
	RUN_BASE_TEST(" 向主机同步两次写入数据512+0 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_026_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_ASYNC_WRITE_TWICE_BASE(512, 0);
     if(s32Status == -1)
    {
         s32Status = 0;
    }
	RUN_BASE_TEST(" 向主机异步两次写入数据512+0 (buffer 非cache对齐) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_027(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32TestLen = 200;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    buf = TG_ACM_MALLOC((u32BufferLen*BSP_ACM_SIO_NUM),1);

    if (NULL == buf)
    {
        printk("malloc buf error\n");
        return BSP_ERROR;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i] != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != BSP_ERROR)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
        }
    }

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    RUN_BASE_TEST(" 同步关闭串口后写200 字节数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_027_ASYNC(BSP_VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    BSP_U32 u32TestLen = 200;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F));
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("TEST_ASYNC_WRITE buffer size:%d, write len:%d\n", u32BufferLen, u32TestLen);
    buf = TG_ACM_MALLOC( u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        printk("malloc error\n");
        return BSP_ERROR;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_WRITE_CB, sg_AcmWriteBaseCB[i]) != BSP_OK)
        {
            printk("udi_ioctl ACM:%d error:%d, line:%d\n", i, s32Fd[i], __LINE__);
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stRWInfo.pBuffer = pUsedBuf[i];
        stRWInfo.u32Size = u32TestLen;
        ret = udi_ioctl(s32Fd[i], ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret != BSP_ERROR)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
        }
    }

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}



static BSP_S32 BSP_ACM_ReadInParts(BSP_S32 s32Fd, BSP_U8* pBuffer, BSP_U32 u32Size, BSP_U32 u32PartSize)
{
    BSP_S32 ret;
    BSP_S32 s32Left = (BSP_S32)u32Size;
    BSP_U8* pCur = pBuffer;
    BSP_U32 u32ReadSize;

    while(s32Left > 0)
    {
        u32ReadSize = (s32Left > u32PartSize) ? (u32PartSize) : (s32Left);
        ret = udi_read(s32Fd, pCur, u32ReadSize);
        if (ret < 0)
        {
            printk("read data fail: 0x%x\n", ret);
            return BSP_ERROR;
        }
        s32Left -= ret;
        pCur+= ret;
    }
    return OK;
}

static BSP_S32 BSP_ACM_TEST_BLOCK_READ_PARTS_BASE(BSP_U32 u32TestLen, BSP_U32 u32PartSize)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32BufferLen = u32TestLen;
    UDI_OPEN_PARAM stOpenParam = {0};

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_PARTS_BASE_ERR;
    }
    printk("BLOCK_READ buffer size:%d, read len:%d, buf:0x%x\n", u32BufferLen, u32TestLen, (BSP_U32)buf);
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);
    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i] == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_PARTS_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = BSP_ACM_ReadInParts(s32Fd[i], pUsedBuf[i], u32TestLen, u32PartSize);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_PARTS_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pUsedBuf[i], u32TestLen, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, u32TestLen, *pUsedBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }

ACM_BLOCK_PARTS_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_BLOCK_WRITE_BIG_BASE(BSP_U32 u32TestLen,  BSP_BOOL bCacheAlign)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC((u32BufferLen*BSP_ACM_SIO_NUM),1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        bufFree = buf = TG_ACM_MALLOC((u32BufferLen*BSP_ACM_SIO_NUM+1),1);
    }

    if (NULL == buf)
    {
        goto ACM_BLOCK_WRITE_BASE_ERR;
    }
    memset(buf, 0x5a, (u32BufferLen*BSP_ACM_SIO_NUM));
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	 stOpenParam.devid = sg_AcmDevTbl[i];
	 s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_WRITE_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = WriteAll(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_WRITE_BASE_ERR;
        }
    }

ACM_BLOCK_WRITE_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return s32Status;
}


BSP_S32 BSP_ACM_TEST_028(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_READ_PARTS_BASE(2048, 100);
    RUN_BASE_TEST(" 主机发送较大数据, 单板端分几次接收(2048 / 100) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_029(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BIG_BASE(2048, FALSE);
    RUN_BASE_TEST(" 单板发送较大数据, 主机端分几次接收(2048 / 100) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_030(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_READ_PARTS_BASE(2048, 1024);
	RUN_BASE_TEST(" 主机发送较大数据, 单板端分几次接收(2048 / 1024) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_031(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BIG_BASE(2048, FALSE);
    RUN_BASE_TEST(" 单板发送较大数据, 主机端分几次接收(2048 / 1024) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_032(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_READ_PARTS_BASE(4095, 100);
    RUN_BASE_TEST(" 主机发送较大数据, 单板端分几次接收(4095 / 100) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_033(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BIG_BASE(4095, FALSE);
    RUN_BASE_TEST(" 单板发送较大数据, 主机端分几次接收(4095 / 100) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_034(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_READ_PARTS_BASE(4096, 1);
    RUN_BASE_TEST(" 主机发送较大数据, 单板端分几次接收(4095 / 100) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_035(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BIG_BASE(4096, FALSE);
    RUN_BASE_TEST(" 单板发送较大数据, 主机端分几次接收(4095 / 100) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_036(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_READ_PARTS_BASE(8192, 2048);
    RUN_BASE_TEST(" 主机发送较大数据, 单板端分几次接收(8192 / 2048) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_037(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_BLOCK_WRITE_BIG_BASE(10243, FALSE);
    RUN_BASE_TEST(" 单板发送较大数据, 主机端分几次接收(10243 / 1333) ", s32Status);
}

struct semaphore    TASK_READ_COMPLETE[BSP_ACM_MAX_NUM];
struct semaphore    TASK_WRITE_COMPLETE[BSP_ACM_MAX_NUM];

typedef struct {
	BSP_S32 s32Fd;
	BSP_U32 u32TestLen;
	BSP_U32 acm;
	BSP_VOID * pRCB;
	BSP_VOID * pWCB;
	BSP_S32* s32pRStatus;
	BSP_S32* s32pWStatus;
	BSP_BOOL bRead;
} acm_rw_block_t;

BSP_VOID taskRead(BSP_S32 s32Fd, BSP_U32 u32TestLen,  BSP_U32 acm, BSP_S32* s32pStatus)
{
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC( u32BufferLen,1);

    if (NULL == buf)
    {
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen);

    ret = udi_read(s32Fd, buf, u32TestLen);
    if (ret < 0)
    {
        printk("ACM:read error\n");
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }

    /* 比较数据 */
    if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(buf, u32TestLen, 0x5a, __LINE__))
    {
        printk("acm test,  read OK, size:%d, data:0x%x, \n",  ret, *buf);
    }
    else
    {
        s32Status = -1;
    }
    printk("read task ok\n");

ACM_BLOCK_BASE_ERR:

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    *s32pStatus = s32Status;
    up(&TASK_READ_COMPLETE[acm]);
}

BSP_VOID taskReadAsync(BSP_S32 s32Fd, BSP_U32 u32TestLen, BSP_U32 acm, BSP_VOID * pCB,  BSP_S32* s32pStatus)
{
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("ASYNC_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);
    sema_init(&READ_COMPLETE[acm], 0);
#if 0
    if((READ_COMPLETE[acm] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
    {
        printk("  Error in creating READ_COMPLETE semaphore  \n");
        goto ACM_ASYNC_BASE_READ_ERR;
    }
#endif
    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_READ_CB, pCB) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d, line:%d\n", acm, s32Fd, __LINE__);
        goto ACM_ASYNC_BASE_READ_ERR;
    }

    down(&READ_COMPLETE[acm]);

    ret = udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM:%d read error\n", acm);
        s32Status = -1;
        goto ACM_ASYNC_BASE_READ_ERR;
    }

    /* 比较数据 */
    if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(stRWInfo.pBuffer, stRWInfo.u32Size, 0x5a, __LINE__))
    {
        printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", acm, stRWInfo.u32Size, *stRWInfo.pBuffer);
    }
    else
    {
        s32Status = -1;
    }

    /* 归还buffer */
    ret = udi_ioctl(s32Fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM:%d read error\n", acm);
        s32Status = -1;
        goto ACM_ASYNC_BASE_READ_ERR;
    }

ACM_ASYNC_BASE_READ_ERR:
    *s32pStatus = s32Status;
    up(&TASK_READ_COMPLETE[acm]);
}


BSP_VOID taskWrite(BSP_S32 s32Fd, BSP_U32 u32TestLen, BSP_U32 acm, BSP_S32* s32pStatus)
{
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("BLOCK_WRITEbuffer size:%d, write len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);

    if (NULL == buf)
    {
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0x5a, u32BufferLen);

    mdelay(500);

    ret = udi_write(s32Fd, buf, u32TestLen);
    if (ret < 0)
    {
        printk("ACM:write error\n");
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }
    printk("write ok\n");

ACM_BLOCK_BASE_ERR:

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    *s32pStatus = s32Status;
    up(&TASK_WRITE_COMPLETE[acm]);
}

BSP_VOID taskWriteAsync(BSP_S32 s32Fd, BSP_U32 u32TestLen, BSP_U32 acm, BSP_VOID * pCB, BSP_S32* s32pStatus)
{
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("ASYNC_WRITE buffer size:%d, write len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);

    if (NULL == buf)
    {
        s32Status = -1;
        goto ACM_ASYNC_BASE_WRITE_ERR;
    }
    memset(buf, 0x5a, u32BufferLen);

    sema_init(&WRITE_COMPLETE[acm], 0);
#if 0
    if((WRITE_COMPLETE[acm] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
    {
        printk("  Error in creating WRITE_COMPLETE semaphore  \n");
        goto ACM_ASYNC_BASE_WRITE_ERR;
    }
#endif
    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_WRITE_CB, pCB) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d, line:%d\n", acm, s32Fd, __LINE__);
        goto ACM_ASYNC_BASE_WRITE_ERR;
    }

    stRWInfo.pBuffer = buf;
    stRWInfo.u32Size = u32TestLen;
    ret = udi_ioctl(s32Fd, ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM:%d write error\n", acm);
        s32Status = -1;
        goto ACM_ASYNC_BASE_WRITE_ERR;
    }

    down(&WRITE_COMPLETE[acm]);

ACM_ASYNC_BASE_WRITE_ERR:

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    *s32pStatus = s32Status;
    up(&TASK_WRITE_COMPLETE[acm]);
}

static int taskReadWrite_func(void* arg)
{
	acm_rw_block_t* taskarg= (acm_rw_block_t*)arg;

	printk("taskReadWrite_func: s32Fd %d, u32TestLen %d, acm %d, bRead:%d, pRCB %x, pWCB %x, s32pRStatus %x, s32pWStatus %x\n",
		taskarg->s32Fd, taskarg->u32TestLen,taskarg->acm,taskarg->bRead,(BSP_U32)taskarg->pRCB,(BSP_U32)taskarg->pWCB,(BSP_U32)taskarg->s32pRStatus,(BSP_U32)taskarg->s32pWStatus);

	if(taskarg->bRead)
	{
		if(taskarg->pRCB)
		{
			taskReadAsync(taskarg->s32Fd, taskarg->u32TestLen,taskarg->acm,taskarg->pRCB,taskarg->s32pRStatus);
		}
		else
		{
			taskRead(taskarg->s32Fd, taskarg->u32TestLen,taskarg->acm,taskarg->s32pRStatus);
		}
	}
	else
	{
		if(taskarg->pWCB)
		{
			taskWriteAsync(taskarg->s32Fd, taskarg->u32TestLen,taskarg->acm,taskarg->pWCB,taskarg->s32pWStatus);
		}
		else
		{
			taskWrite(taskarg->s32Fd, taskarg->u32TestLen,taskarg->acm,taskarg->s32pWStatus);
		}
	}
    return 0;
}

BSP_S32 BSP_ACM_TEST_038(VOID)
{
    BSP_S32 s32Status = 0;
    //BSP_S32 taskIdRead[BSP_ACM_MAX_NUM] = {0};
    //BSP_S32 taskIdWrite[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskReadStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskWriteStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];
    acm_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM*2];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_READ_COMPLETE[i], 0);
#if 0
        if((TASK_READ_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_WRITE_COMPLETE[i], 0);
#if 0
	if((TASK_WRITE_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);

        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        taskarg_r[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
            printk("can not alloc struct jtest_rw_block_t\n");
            return -1;
        }

        taskarg_r[i]->s32Fd = s32Fd[i];
        taskarg_r[i]->u32TestLen = 2048;
        taskarg_r[i]->acm   = (BSP_U32)i;
        taskarg_r[i]->pRCB   = NULL;
        taskarg_r[i]->pWCB   = NULL;
        taskarg_r[i]->s32pRStatus = &taskReadStatus[i];
        taskarg_r[i]->s32pWStatus = &taskWriteStatus[i];
        taskarg_r[i]->bRead = TRUE;
        kthread_run(taskReadWrite_func, (void*)taskarg_r[i],"taskRead");

        taskarg_w[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
            printk("can not alloc struct jtest_rw_block_t\n");
            return -1;
        }
        memcpy(taskarg_w[i], taskarg_r[i], sizeof(acm_rw_block_t));
        taskarg_w[i]->bRead = FALSE;
        kthread_run(taskReadWrite_func, (void*)taskarg_w[i],"taskWrite");
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&TASK_READ_COMPLETE[i]);
        down(&TASK_WRITE_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if((taskReadStatus[i] == -1) || (taskWriteStatus[i] == -1))
        {
            s32Status = -1;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    	TG_ACM_FREE(taskarg_w[i]);
        TG_ACM_FREE(taskarg_r[i]);
    }

    RUN_BASE_TEST(" 多任务 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_038_READ_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    //BSP_S32 taskIdRead[BSP_ACM_MAX_NUM] = {0};
    //BSP_S32 taskIdWrite[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskReadStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskWriteStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM];
    acm_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_READ_COMPLETE[i], 0);
#if 0
        if((TASK_READ_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_WRITE_COMPLETE[i], 0);
#if 0
	if((TASK_WRITE_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        taskarg_r[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

         taskarg_r[i]->s32Fd = s32Fd[i];
         taskarg_r[i]->u32TestLen = 2048;
         taskarg_r[i]->acm   = (BSP_U32)i;
         taskarg_r[i]->pRCB   = sg_AcmReadBaseCB[i];
         taskarg_r[i]->pWCB   = NULL;
         taskarg_r[i]->s32pRStatus = &taskReadStatus[i];
         taskarg_r[i]->s32pWStatus = &taskWriteStatus[i];
         taskarg_r[i]->bRead = TRUE;

         kthread_run(taskReadWrite_func,taskarg_r[i],"taskRead");

        taskarg_w[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }
         memcpy(taskarg_w[i], taskarg_r[i], sizeof(acm_rw_block_t));
         taskarg_w[i]->bRead = FALSE;
         kthread_run(taskReadWrite_func,taskarg_w[i],"taskWrite");
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&TASK_READ_COMPLETE[i]);
        down(&TASK_WRITE_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if((taskReadStatus[i] == -1) || (taskWriteStatus[i] == -1))
        {
            s32Status = -1;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
        TG_ACM_FREE(taskarg_r[i]);
        TG_ACM_FREE(taskarg_w[i]);
    }

    RUN_BASE_TEST(" 多任务 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_038_WRITE_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    //BSP_S32 taskIdRead[BSP_ACM_MAX_NUM] = {0};
    //BSP_S32 taskIdWrite[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskReadStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskWriteStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM];
    acm_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_READ_COMPLETE[i], 0);
#if 0
        if((TASK_READ_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_WRITE_COMPLETE[i], 0);
#if 0
	if((TASK_WRITE_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        taskarg_r[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

         taskarg_r[i]->s32Fd = s32Fd[i];
         taskarg_r[i]->u32TestLen = 2048;
         taskarg_r[i]->acm   = (BSP_U32)i;
         taskarg_r[i]->pRCB   = NULL;
         taskarg_r[i]->pWCB   = sg_AcmWriteBaseCB[i];
         taskarg_r[i]->s32pRStatus = &taskReadStatus[i];
         taskarg_r[i]->s32pWStatus = &taskWriteStatus[i];
         taskarg_r[i]->bRead = TRUE;

         kthread_run(taskReadWrite_func,taskarg_r[i],"taskRead");

         taskarg_w[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }
        memcpy(taskarg_w[i], taskarg_r[i], sizeof(acm_rw_block_t));
        taskarg_w[i]->bRead = FALSE;
        kthread_run(taskReadWrite_func,taskarg_w[i],"taskWrite");
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&TASK_READ_COMPLETE[i]);
        down(&TASK_WRITE_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if((taskReadStatus[i] == -1) || (taskWriteStatus[i] == -1))
        {
            s32Status = -1;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
        TG_ACM_FREE(taskarg_r[i]);
        TG_ACM_FREE(taskarg_w[i]);
    }

    RUN_BASE_TEST(" 多任务 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_038_ASYNC(VOID)
{
    BSP_S32 s32Status = 0;
    //BSP_S32 taskIdRead[BSP_ACM_MAX_NUM] = {0};
    //BSP_S32 taskIdWrite[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskReadStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskWriteStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM];
    acm_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_READ_COMPLETE[i], 0);
#if 0
        if((TASK_READ_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_WRITE_COMPLETE[i], 0);
#if 0
	if((TASK_WRITE_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        taskarg_r[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

         taskarg_r[i]->s32Fd = s32Fd[i];
         taskarg_r[i]->u32TestLen = 2048;
         taskarg_r[i]->acm   = (BSP_U32)i;
         taskarg_r[i]->pRCB   = sg_AcmReadBaseCB[i];
         taskarg_r[i]->pWCB   = sg_AcmWriteBaseCB[i];
         taskarg_r[i]->s32pRStatus = &taskReadStatus[i];
         taskarg_r[i]->s32pWStatus = &taskWriteStatus[i];
         taskarg_r[i]->bRead = TRUE;
         kthread_run(taskReadWrite_func,taskarg_r[i],"taskRead");

         taskarg_w[i] = (acm_rw_block_t *)TG_ACM_MALLOC(sizeof(acm_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }
         memcpy(taskarg_w[i], taskarg_r[i], sizeof(acm_rw_block_t));
         taskarg_w[i]->bRead = FALSE;
         kthread_run(taskReadWrite_func,taskarg_w[i],"taskWrite");
     }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&TASK_READ_COMPLETE[i]);
        down(&TASK_WRITE_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if((taskReadStatus[i] == -1) || (taskWriteStatus[i] == -1))
        {
            s32Status = -1;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
        TG_ACM_FREE(taskarg_r[i]);
        TG_ACM_FREE(taskarg_w[i]);
    }

    printk("udi_close dev\n");
    RUN_BASE_TEST(" 多任务 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_039(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U32 u32TestLen = 1000;
    BSP_U8* writebuf = NULL;
    BSP_U8* readbuf = NULL;
    BSP_U8* pWriteBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U8* pReadBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32WriteBufferLen = ((u32TestLen + 32)&(~0x1F));  /* buffer 长度要cacheline 对齐 */
    BSP_U32 u32ReadBufferLen = ((u32TestLen + 32)&(~0x1F));  /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_WRITE  buffer size:%d, read len:%d\n", u32WriteBufferLen, u32TestLen);
    printk("BLOCK_READ   buffer size:%d, read len:%d\n", u32ReadBufferLen, u32TestLen);

    writebuf = TG_ACM_MALLOC((u32WriteBufferLen*BSP_ACM_SIO_NUM),1);

    if (NULL == writebuf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(writebuf, 0x5a, (u32WriteBufferLen*BSP_ACM_SIO_NUM));

    readbuf = TG_ACM_MALLOC(u32ReadBufferLen*BSP_ACM_SIO_NUM,1);

    if (NULL == readbuf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(readbuf, 0, u32ReadBufferLen*BSP_ACM_SIO_NUM);

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pWriteBuf[i] = writebuf + i * u32WriteBufferLen;
        pReadBuf[i] = readbuf + i * u32ReadBufferLen;

        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_write(s32Fd[i], pWriteBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pReadBuf[i], u32TestLen);
        printk("len = %d \n", ret);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pReadBuf[i], u32TestLen, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pReadBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    if (NULL != writebuf)
    {
        TG_ACM_FREE(writebuf);
    }
    if (NULL != readbuf)
    {
        TG_ACM_FREE(readbuf);
    }

    return s32Status;
}

BSP_S32 BSP_ACM_TEST_040(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U32 u32TestLen = 1000;
    BSP_U8* writebuf = NULL;
    BSP_U8* readbuf = NULL;
    BSP_U8* pWriteBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U8* pReadBuf[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32WriteBufferLen = ((u32TestLen + 32)&(~0x1F));  /* buffer 长度要cacheline 对齐 */
    BSP_U32 u32ReadBufferLen = ((u32TestLen + 32)&(~0x1F));  /* buffer 长度要cacheline 对齐 */
    BSP_U32 u32Times = 10,j = 0;
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_WRITE  buffer size:%d, read len:%d\n", u32WriteBufferLen, u32TestLen);
    printk("BLOCK_READ   buffer size:%d, read len:%d\n", u32ReadBufferLen, u32TestLen);

    writebuf = TG_ACM_MALLOC((u32WriteBufferLen*BSP_ACM_SIO_NUM),1);

    if (NULL == writebuf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }

    readbuf = TG_ACM_MALLOC( u32ReadBufferLen*BSP_ACM_SIO_NUM,1);

    if (NULL == readbuf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	    stOpenParam.devid = sg_AcmDevTbl[i];
	    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pWriteBuf[i] = writebuf + i * u32WriteBufferLen;
        pReadBuf[i] = readbuf + i * u32ReadBufferLen;

        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

for(j = 0;j < u32Times; j++)
{
    memset(writebuf, 0x5a, (u32WriteBufferLen*BSP_ACM_SIO_NUM));
    memset(readbuf, 0, u32ReadBufferLen*BSP_ACM_SIO_NUM);

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_write(s32Fd[i], pWriteBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pReadBuf[i], u32TestLen);
        printk("len = %d \n", ret);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }

        /* 比较数据 */
        if (BSP_OK == BSP_ACM_CMP_MEM_WITH_VALUE(pReadBuf[i], u32TestLen, 0x5a, __LINE__))
        {
            printk("acm test, acm:%d read OK, size:%d, data:0x%x, \n", i, ret, *pReadBuf[i]);
        }
        else
        {
            s32Status = BSP_ERROR;
        }
    }
    printk("*****************************\n");
    mdelay(500);
}

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    if (NULL != writebuf)
    {
        TG_ACM_FREE(writebuf);
    }
    if (NULL != readbuf)
    {
        TG_ACM_FREE(readbuf);
    }

    return s32Status;
}

BSP_S32 BSP_ACM_TEST_044(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 1;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC( u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        printk("len = %d \n", ret);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_045(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 1024;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM, 1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	    stOpenParam.devid = sg_AcmDevTbl[i];
	    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        printk("len = %d \n", ret);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_046(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 2048;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        printk("len = %d \n", ret);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        u32TestLen = ret;
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        u32TestLen = 4096;
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_047(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret,j;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 2048;
    BSP_U32 u32Times = 10;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }
    for(j = 0; j < u32Times; j++)
    {
        for (i = 0; i < BSP_ACM_SIO_NUM; i++)
        {
            ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
            if (ret < 0)
            {
                printk("ACM:%d read error\n", i);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
            ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
            if (ret != u32TestLen)
            {
                printk("ACM:%d write error\n", i);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_048(VOID)
{
#define TEST_BAUND_TIMES 4
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret,j;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 1;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    BSP_S32 s32Baud[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32CheckBaud[TEST_BAUND_TIMES] = {110,57600,115200,256000};
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC( u32BufferLen*BSP_ACM_SIO_NUM,4);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }
    for(j = 0; j < TEST_BAUND_TIMES; j++)
    {
        for (i = 0; i < BSP_ACM_SIO_NUM; i++)
        {
            ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
            if (ret < 0)
            {
                printk("ACM:%d read error\n", i);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
            /*
            ret = udi_ioctl(s32Fd[i], SIO_BAUD_GET, &s32Baud[i]);
            if (ret != BSP_OK)
            {
                printk("udi_ioctl SIO_BAUD_GET error,ret = %x\n",ret);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
            */
            if(s32Baud[i] != s32CheckBaud[j])
            {
                printk("pc baund != board baund,  pc: %d, board : %d\n",s32CheckBaud[j],s32Baud[i]);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_049(VOID)
{
#define TEST_BAUND_TIMES 4
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret,j;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 1;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};
    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }
    for(j = 0; j < TEST_BAUND_TIMES; j++)
    {
        for (i = 0; i < BSP_ACM_SIO_NUM; i++)
        {
            /*
            ret = udi_ioctl(s32Fd[i], SIO_BAUD_SET, s32Baud[j]);
            if (ret != BSP_OK)
            {
                printk("udi_ioctl SIO_BAUD_GET error,ret = %x\n",ret);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
            */
            ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
            if (ret != u32TestLen)
            {
                printk("ACM:%d write error\n", i);
                s32Status = -1;
                goto ACM_BLOCK_BASE_ERR;
            }
            mdelay(10);
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_050(VOID)
{
#define TEST_BAUND_TIMES 4
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 1;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_051(VOID)
{
#define TEST_BAUND_TIMES 4
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 1;
    //BSP_U32 u32Times = 10;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM, 1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_053(VOID)
{
#define TEST_BAUND_TIMES 4
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 10;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC( u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_write(s32Fd[i], pUsedBuf[i], u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_054(VOID)
{
#define TEST_BAUND_TIMES 4
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;
    BSP_U8* buf = NULL;
    BSP_U8* pUsedBuf[BSP_ACM_MAX_NUM] = {0};
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 10;
    //BSP_U32 u32Times = 10;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    UDI_OPEN_PARAM stOpenParam = {0};

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen*BSP_ACM_SIO_NUM,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen*BSP_ACM_SIO_NUM);

    i = 0;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        pUsedBuf[i] = buf + i * u32BufferLen;
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        ret = udi_read(s32Fd[i], pUsedBuf[i], u32TestLen);
        printk("len = %d \n", ret);
        if (ret <0 )
        {
            printk("ACM:%d read error\n", i);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        if(pUsedBuf[i][0] != 'A')
        {
            printk("ACM:%d read error,pUsedBuf[i][0] = %c\n", i,pUsedBuf[i][0]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    return s32Status;
}

struct semaphore   TASK_COMPLETE[BSP_ACM_MAX_NUM];

BSP_VOID TaskReadAndWrite(BSP_S32 s32Fd, BSP_U32 acm, BSP_S32* s32pStatus)
{
    BSP_S32 s32Status = 0;
    BSP_S32 ret,j;
    BSP_U8* buf = NULL;
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32TestLen = 2048;
    BSP_U32 u32Times = 10;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen);

    for(j = 0; j < u32Times; j++)
    {
        ret = udi_read(s32Fd, buf, u32TestLen);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", acm);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        u32TestLen = ret;
        ret = udi_write(s32Fd, buf, u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", acm);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        u32TestLen = 2048;
    }

ACM_BLOCK_BASE_ERR:
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    *s32pStatus = s32Status;

    up(&TASK_COMPLETE[acm]);
}

typedef struct {
	BSP_S32 s32Fd;
	BSP_U32 acm;
	BSP_S32* s32pStatus;
} acm_rw2_block_t;

static int taskReadAndWrite_func(void* arg)
{
	acm_rw2_block_t* taskarg= (acm_rw2_block_t*)arg;

	printk("taskReadWrite_func: s32Fd %d, acm %d,s32pStatus %x\n",
		taskarg->s32Fd, taskarg->acm,(BSP_U32)taskarg->s32pStatus);

	TaskReadAndWrite(taskarg->s32Fd,taskarg->acm,taskarg->s32pStatus);
    return 0;
}
BSP_S32 BSP_ACM_TEST_056(VOID)
{
    BSP_S32 s32Status = {0};
    //BSP_S32 taskId[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw2_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&TASK_COMPLETE[i], 0);
#if 0
        if((TASK_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating TASK_COMPLETE semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw2_block_t *)TG_ACM_MALLOC(sizeof(acm_rw2_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->s32pStatus = &taskStatus[i];

	 kthread_run(taskReadAndWrite_func,taskarg[i],"taskId");
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&TASK_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if(taskStatus[i] == -1)
        {
            s32Status = -1;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	    TG_ACM_FREE(taskarg[i]);
    }

    RUN_BASE_TEST(" 多任务回写 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_058(VOID)
{
    return BSP_ACM_TEST_056();
}

typedef struct {
	BSP_S32 s32Fd;
	BSP_U32 acm;
	BSP_U32 u32TestLen;
	BSP_S32* s32pStatus;
} acm_rw3_block_t;

int TaskWrite59(void * arg)
{
    acm_rw3_block_t* taskarg = (acm_rw3_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 acm = taskarg->acm;
    BSP_U32 u32TestLen = taskarg->u32TestLen;
    BSP_S32* s32pStatus = taskarg->s32pStatus;
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    //ACM_READ_BUFF_INFO stBuffInfo = {0};
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    BSP_U32 i = 0;
    BSP_U32 u32Loop = 10;

    printk("BLOCK_WRITEbuffer size:%d, write len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);

    if (NULL == buf)
    {
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0x5a, u32BufferLen);
    for(i = 0; i < u32Loop; i++)
    {
        ret = udi_write(s32Fd, buf, u32TestLen);
        if (ret < 0)
        {
            printk("ACM:write error\n");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        printk("write ok i = %d\n",i);
        mdelay(100);
    }

ACM_BLOCK_BASE_ERR:

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    *s32pStatus = s32Status;

    up(&TASK_COMPLETE[acm]);
    return 0;
}


BSP_S32 BSP_ACM_TEST_059(VOID)
{
    BSP_S32 s32Status = {0};
    //BSP_S32 taskId[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 taskStatus[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_U32 u32TestLen = 100;
    BSP_S32 i;
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw3_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
     sema_init(&TASK_COMPLETE[i], 0);
#if 0
        if((TASK_COMPLETE[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating TASK_COMPLETE semaphore  ");
            s32Status = -1;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw3_block_t *)TG_ACM_MALLOC(sizeof(acm_rw3_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->u32TestLen = u32TestLen;
	 taskarg[i]->s32pStatus = &taskStatus[i];

	 kthread_run(TaskWrite59,taskarg[i],"taskId");
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&TASK_COMPLETE[i]);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if(taskStatus[i] == -1)
        {
            s32Status = -1;
        }
    }

ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	    TG_ACM_FREE(taskarg[i]);
    }

    RUN_BASE_TEST(" 多任务回写 ", s32Status);
}

VOID BSP_ACM_TEST_REALLOC_BASE_READ_CB(VOID)
{
    printk("<read CB> realloc read buff test, acm:%d read done\n", 1);
}

BSP_S32 BSP_ACM_TEST_REALLOC_READBUFF_BASE(BSP_U32 u32BuffNum, BSP_U32 u32BuffSize)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd = 0;
    BSP_S32 ret;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    ACM_READ_BUFF_INFO stBuffInfo = {0};
    UDI_OPEN_PARAM stOpenParam = {0};

    stOpenParam.devid = sg_AcmDevTbl[0];
    s32Fd = udi_open(&stOpenParam);

    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_READ_CB, BSP_ACM_TEST_REALLOC_BASE_READ_CB) != BSP_OK)
    {
        printk("udi_ioctl ACM error:%d, line:%d\n", s32Fd, __LINE__);
        goto ACM_ERR_ASYNC_BASE;
    }

    /* PC会先发一段数据 */
    while (udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo) < 0)
    {
        //printk("111----- wait for data come -----\n");
        mdelay(100);
    }

    /* 归还buffer */
    ret = udi_ioctl(s32Fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM test error\n");
        s32Status = -1;
        goto ACM_ERR_ASYNC_BASE;
    }
    printk("ACM_IOCTL_RETURN_BUFF\n");

    /* PC还会继续发送数据,此时进行 realloc read buffer */
    stBuffInfo.u32BuffNum = u32BuffNum;
    stBuffInfo.u32BuffSize = u32BuffSize;
    ret = udi_ioctl(s32Fd, ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo);
    if (ret < 0)
    {
        printk("ACM test error\n");
        s32Status = -1;
        goto ACM_ERR_ASYNC_BASE;
    }
    printk("ACM_IOCTL_RELLOC_READ_BUFF\n");

    /* PC会先发一段数据 */
    while (udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo) < 0)
    {
        //printk("222----- wait for data come -----\n");
        msleep(100);
    }
    /* 归还buffer */
    ret = udi_ioctl(s32Fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM test error\n");
        s32Status = -1;
        goto ACM_ERR_ASYNC_BASE;
    }

    /* 恢复原来的buffer配置 */
    stBuffInfo.u32BuffNum = 8;
    stBuffInfo.u32BuffSize = 2048;
    ret = udi_ioctl(s32Fd, ACM_IOCTL_RELLOC_READ_BUFF, &stBuffInfo);
    if (ret < 0)
    {
        printk("ACM test error\n");
        s32Status = -1;
        goto ACM_ERR_ASYNC_BASE;
    }
    printk("ACM_IOCTL_RELLOC_READ_BUFF  Old\n");

ACM_ERR_ASYNC_BASE:
    if (s32Fd > 0)
    {
        udi_close(s32Fd);
    }

    return s32Status;
}

BSP_S32 BSP_ACM_TEST_060(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_REALLOC_READBUFF_BASE(13, 512);
    RUN_BASE_TEST(" 重分配读取buffer (size:512 * num:13) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_061(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_REALLOC_READBUFF_BASE(4, 4096);
    RUN_BASE_TEST(" 重分配读取buffer (size:4096 * num:4) ", s32Status);
}

BSP_S32 BSP_ACM_TEST_062(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_TEST_REALLOC_READBUFF_BASE(2, 4096);
    RUN_BASE_TEST(" 重分配读取buffer (size:4096 * num:2) ", s32Status);
}

struct semaphore  LongTimeWriteComplete[BSP_ACM_MAX_NUM];

typedef struct {
	BSP_S32 s32Fd;
	BSP_U32 u32TestLen;
	BSP_U32 acm;
} acm_rw4_block_t;

BSP_S32 ACM_LONG_TIME_WRITE_Task(void* arg)
{
    acm_rw4_block_t* taskarg = (acm_rw4_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 u32TestLen = taskarg->u32TestLen;
    BSP_U32 ACM = taskarg->acm;
    BSP_U8* buf = NULL;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */
    BSP_S32 s32Times = 10000;
    BSP_S32 ret;
    struct timeval start, end;
    //BSP_U32 u32TimeProf = 0;

    buf = TG_ACM_MALLOC((u32BufferLen*BSP_ACM_SIO_NUM),1);
    if (NULL == buf)
    {
        printk("malloc buffer error\n");
        return -1;
    }
    memset(buf, 0x5a, u32BufferLen);

    do_gettimeofday(&start);
    while(s32Times)
    {
        ret = udi_write(s32Fd, buf, u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:write error\n");
            break;
        }
        s32Times--;
    }
    do_gettimeofday(&end);
    printk("ACM:%d, us=%d, \n",ACM, (int)((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec)));
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }

    up(&LongTimeWriteComplete[ACM]);
    return 0;
}

BSP_S32 BSP_ACM_TEST_WRITE(BSP_U32 u32TestLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    //BSP_S32 taskId[BSP_ACM_MAX_NUM]= {0};
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw4_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    	sema_init(&LongTimeWriteComplete[i], 0);
#if 0
        if((LongTimeWriteComplete[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw4_block_t *)TG_ACM_MALLOC(sizeof(acm_rw4_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->u32TestLen = u32TestLen;

	 kthread_run(ACM_LONG_TIME_WRITE_Task,taskarg[i],"taskIdWrite");
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&LongTimeWriteComplete[i]);
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	 TG_ACM_FREE(taskarg[i]);
    }
    return BSP_ERROR;
}

BSP_S32 BSP_ACM_TEST_065(VOID)
{
    BSP_U32 u32TestLen = 64;
    return BSP_ACM_TEST_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_066(VOID)
{
    BSP_U32 u32TestLen = 128;
    return BSP_ACM_TEST_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_067(VOID)
{
    BSP_U32 u32TestLen = 512;
    return BSP_ACM_TEST_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_068(VOID)
{
    BSP_U32 u32TestLen = 1024;
    return BSP_ACM_TEST_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_069(VOID)
{
    BSP_U32 u32TestLen = 2048;
    return BSP_ACM_TEST_WRITE(u32TestLen);
}

struct semaphore  LongTimeReadComplete[BSP_ACM_MAX_NUM];

int ACM_LONG_TIME_READ_Task(void * arg)
{
    acm_rw4_block_t* taskarg = (acm_rw4_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 u32TestLen = taskarg->u32TestLen;
    BSP_U32 ACM = taskarg->acm;
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    BSP_S32 s32Times = 10000;
    struct timeval start, end;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);

    if (NULL == buf)
    {
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen);

    ret = udi_read(s32Fd, buf, u32TestLen);
    if (ret < 0)
    {
        printk("ACM:read error\n");
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }

    do_gettimeofday(&start);
    while(s32Times)
    {
        ret = udi_read(s32Fd, buf, u32TestLen);
        if (ret < 0)
        {
            printk("ACM:read error\n");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        s32Times--;
    }
    do_gettimeofday(&end);
    printk("ACM:%d, us = %d\n",ACM, (int)((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec)));

ACM_BLOCK_BASE_ERR:

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }

    up(&LongTimeReadComplete[ACM]);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_READ(BSP_U32 u32TestLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    //BSP_S32 taskId[BSP_ACM_MAX_NUM]= {0};
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw4_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
       sema_init(&LongTimeReadComplete[i], 0);
#if 0
        if((LongTimeReadComplete[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw4_block_t *)TG_ACM_MALLOC(sizeof(acm_rw4_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->u32TestLen = u32TestLen;

	 kthread_run(ACM_LONG_TIME_READ_Task,taskarg[i],"taskIdRead");
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&LongTimeReadComplete[i]);
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	 TG_ACM_FREE(taskarg[i]);
    }
    return BSP_ERROR;
}

BSP_S32 BSP_ACM_TEST_070(VOID)
{
    BSP_U32 u32TestLen = 64;
    return BSP_ACM_TEST_READ(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_071(VOID)
{
    BSP_U32 u32TestLen = 128;
    return BSP_ACM_TEST_READ(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_072(VOID)
{
    BSP_U32 u32TestLen = 512;
    return BSP_ACM_TEST_READ(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_073(VOID)
{
    BSP_U32 u32TestLen = 1024;
    return BSP_ACM_TEST_READ(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_074(VOID)
{
    BSP_U32 u32TestLen = 2048;
    return BSP_ACM_TEST_READ(u32TestLen);
}

struct semaphore  LongTimeReadWrtieComplete[BSP_ACM_MAX_NUM];
BSP_S32 ACM_LONG_TIME_READ_WRITE_Task(void* arg)
{
    acm_rw4_block_t* taskarg = (acm_rw4_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 u32TestLen = taskarg->u32TestLen;
    BSP_U32 ACM = taskarg->acm;
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    BSP_S32 s32Times = 10000;
    struct timeval start, end;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("BLOCK_READ buffer size:%d, read len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);
    if (NULL == buf)
    {
        goto ACM_BLOCK_BASE_ERR;
    }
    memset(buf, 0, u32BufferLen);

    ret = udi_read(s32Fd, buf, u32TestLen);
    if (ret < 0)
    {
        printk("ACM:%d read error\n", ACM);
        s32Status = -1;
        goto ACM_BLOCK_BASE_ERR;
    }

    do_gettimeofday(&start);
    while(s32Times)
    {
        ret = udi_read(s32Fd, buf, u32TestLen);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", ACM);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        mdelay(10);
        ret = udi_write(s32Fd, buf, u32TestLen);
        if (ret != u32TestLen)
        {
            printk("ACM:%d write error\n", ACM);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
        s32Times--;
    }
    do_gettimeofday(&end);
    printk("ACM:%d, us = %d\n",ACM, (int)((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec)));

ACM_BLOCK_BASE_ERR:
    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    up(&LongTimeReadWrtieComplete[ACM]);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_READ_WRITE(BSP_U32 u32TestLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    //BSP_S32 taskId[BSP_ACM_MAX_NUM]= {0};
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw4_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sema_init(&LongTimeReadWrtieComplete[i], 0);
#if 0
		if((LongTimeReadWrtieComplete[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw4_block_t *)TG_ACM_MALLOC(sizeof(acm_rw4_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->u32TestLen = u32TestLen;

	 kthread_run(ACM_LONG_TIME_READ_WRITE_Task,taskarg[i],"taskIdReadWrite");
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&LongTimeReadWrtieComplete[i]);
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	 TG_ACM_FREE(taskarg[i]);
    }
    return BSP_ERROR;
}

BSP_S32 BSP_ACM_TEST_075(VOID)
{
    BSP_U32 u32TestLen = 64;
    return BSP_ACM_TEST_READ_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_076(VOID)
{
    BSP_U32 u32TestLen = 128;
    return BSP_ACM_TEST_READ_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_077(VOID)
{
    BSP_U32 u32TestLen = 512;
    return BSP_ACM_TEST_READ_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_078(VOID)
{
    BSP_U32 u32TestLen = 1023;
    return BSP_ACM_TEST_READ_WRITE(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_079(VOID)
{
    BSP_U32 u32TestLen = 2048;
    return BSP_ACM_TEST_READ_WRITE(u32TestLen);
}


BSP_U32 g_AcmTestReadAsyncTimes[6] = {0};
struct semaphore  LongTimeReadAsyncComplete[6];

VOID BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_1(VOID)
{
    g_AcmTestReadAsyncTimes[0]++;
    up(&LongTimeReadAsyncComplete[0]);
}

VOID BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_2(VOID)
{
    g_AcmTestReadAsyncTimes[1]++;
    up(&LongTimeReadAsyncComplete[1]);
}

VOID BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_3(VOID)
{
    g_AcmTestReadAsyncTimes[2]++;
    up(&LongTimeReadAsyncComplete[2]);
}

VOID BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_4(VOID)
{
    g_AcmTestReadAsyncTimes[3]++;
    up(&LongTimeReadAsyncComplete[3]);
}

VOID BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_5(VOID)
{
    g_AcmTestReadAsyncTimes[4]++;
    up(&LongTimeReadAsyncComplete[4]);
}

VOID BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_6(VOID)
{
    g_AcmTestReadAsyncTimes[5]++;
    up(&LongTimeReadAsyncComplete[5]);
}

ACM_READ_DONE_CB_T sg_AcmLongReadBaseCB[6] = {
    BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_1,
    BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_2,
    BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_3,
    BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_4,
    BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_5,
    BSP_ACM_LONG_TIME_READ_ASYNC_DONE_CB_6,
};

int ACM_LONG_TIME_READ_ASYNC_Task(void* arg)
{
    acm_rw4_block_t* taskarg = (acm_rw4_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 u32TestLen = taskarg->u32TestLen;
    BSP_U32 ACM = taskarg->acm;
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    //BSP_S32 s32Times = 10000;
    struct timeval start, end;

    printk("ASYNC_READ read len:%d\n", u32TestLen);

    sema_init(&LongTimeReadAsyncComplete[ACM], 0);
#if 0
    if((LongTimeReadAsyncComplete[ACM] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
    {
        printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
        s32Status = -1;
        goto ACM_ASYNC_BASE_LONG_TIME_READ_ERR;
    }
#endif
    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_READ_CB, sg_AcmLongReadBaseCB[ACM]) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d, line:%d\n", ACM, s32Fd, __LINE__);
        goto ACM_ASYNC_BASE_LONG_TIME_READ_ERR;
    }

    down(&LongTimeReadAsyncComplete[ACM]);
    ret = udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM:%d read error\n", ACM);
        s32Status = -1;
        goto ACM_ASYNC_BASE_LONG_TIME_READ_ERR;
    }

    ret = udi_ioctl(s32Fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo);
    if (ret < 0)
    {
        printk("ACM:%d read error\n", ACM);
        s32Status = -1;
        goto ACM_ASYNC_BASE_LONG_TIME_READ_ERR;
    }

    do_gettimeofday(&start);

    while(g_AcmTestReadAsyncTimes[ACM] != 10001)
    {
        down(&LongTimeReadAsyncComplete[ACM]);
        ret = udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", ACM);
            s32Status = -1;
            goto ACM_ASYNC_BASE_LONG_TIME_READ_ERR;
        }

        ret = udi_ioctl(s32Fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d read error\n", ACM);
            s32Status = -1;
            goto ACM_ASYNC_BASE_LONG_TIME_READ_ERR;
        }
        //printk("wait\n");
    }
    do_gettimeofday(&end);
    printk("Acm:%d  us=%d\n", ACM, (int)((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec)));

ACM_ASYNC_BASE_LONG_TIME_READ_ERR:
    up(&LongTimeReadComplete[ACM]);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_READ_ASYNC(BSP_U32 u32TestLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    //BSP_S32 taskId[BSP_ACM_MAX_NUM]= {0};
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw4_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	    stOpenParam.devid = sg_AcmDevTbl[i];
	    s32Fd[i] = udi_open(&stOpenParam);
        g_AcmTestReadAsyncTimes[i] = 0;
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    sema_init(&LongTimeReadComplete[i], 0);
#if 0
        if((LongTimeReadComplete[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw4_block_t *)TG_ACM_MALLOC(sizeof(acm_rw4_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->u32TestLen = u32TestLen;

	 kthread_run(ACM_LONG_TIME_READ_ASYNC_Task,taskarg[i],"taskIdReadAsync");
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&LongTimeReadComplete[i]);
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	 TG_ACM_FREE(taskarg[i]);
    }
    return BSP_ERROR;
}

BSP_S32 BSP_ACM_TEST_080(VOID)
{
    BSP_U32 u32TestLen = 64;
    return BSP_ACM_TEST_READ_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_081(VOID)
{
    BSP_U32 u32TestLen = 128;
    return BSP_ACM_TEST_READ_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_082(VOID)
{
    BSP_U32 u32TestLen = 512;
    return BSP_ACM_TEST_READ_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_083(VOID)
{
    BSP_U32 u32TestLen = 1024;
    return BSP_ACM_TEST_READ_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_084(VOID)
{
    BSP_U32 u32TestLen = 2048;
    return BSP_ACM_TEST_READ_ASYNC(u32TestLen);
}


struct semaphore  LongTimeWriteAsyncComplete[6];

VOID BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_1(char* pDonebuff, int size)
{
    up(&LongTimeWriteAsyncComplete[0]);
}

VOID BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_2(char* pDonebuff, int size)
{
    up(&LongTimeWriteAsyncComplete[1]);
}

VOID BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_3(char* pDonebuff, int size)
{
    up(&LongTimeWriteAsyncComplete[2]);
}

VOID BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_4(char* pDonebuff, int size)
{
    up(&LongTimeWriteAsyncComplete[3]);
}

VOID BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_5(char* pDonebuff, int size)
{
    up(&LongTimeWriteAsyncComplete[4]);
}

VOID BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_6(char* pDonebuff, int size)
{
    up(&LongTimeWriteAsyncComplete[5]);
}

ACM_WRITE_DONE_CB_T sg_AcmLongWriteBaseCB[6] = {
    BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_1,
    BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_2,
    BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_3,
    BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_4,
    BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_5,
    BSP_ACM_LONG_TIME_WRITE_ASYNC_DONE_CB_6,
};

static int ACM_LONG_TIME_WRITE_ASYNC_Task(void* arg)
{
    acm_rw4_block_t* taskarg = (acm_rw4_block_t*)arg;
    BSP_S32 s32Fd = taskarg->s32Fd;
    BSP_U32 u32TestLen = taskarg->u32TestLen;
    BSP_U32 ACM = taskarg->acm;
    BSP_S32 s32Status = 0;
    BSP_S32 ret;
    BSP_U8* buf = NULL;
    ACM_WR_ASYNC_INFO stRWInfo = {0};
    BSP_S32 s32Times = 10000;
    struct timeval start, end;
    BSP_U32 u32BufferLen = ((u32TestLen + 32)&(~0x1F)); /* buffer 长度要cacheline 对齐 */

    printk("ASYNC_WRITE buffer size:%d, write len:%d\n", u32BufferLen, u32TestLen);

    buf = TG_ACM_MALLOC(u32BufferLen,1);

    if (NULL == buf)
    {
        s32Status = -1;
        goto ACM_ASYNC_BASE_LONG_TIME_WRITE_ERR;
    }
    memset(buf, 0x5a, u32BufferLen);

   sema_init(&LongTimeWriteAsyncComplete[ACM], 0);
#if 0
    if((LongTimeWriteAsyncComplete[ACM] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
    {
        printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
        s32Status = -1;
        goto ACM_ASYNC_BASE_LONG_TIME_WRITE_ERR;
    }
#endif
    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_WRITE_CB, sg_AcmLongWriteBaseCB[ACM]) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d, line:%d\n", ACM, s32Fd, __LINE__);
        goto ACM_ASYNC_BASE_LONG_TIME_WRITE_ERR;
    }

    stRWInfo.pBuffer = buf;
    stRWInfo.u32Size = u32TestLen;

    do_gettimeofday(&start);

    while(s32Times)
    {
        ret = udi_ioctl(s32Fd, ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret < 0)
        {
            printk("ACM:%d write error\n", ACM);
            s32Status = -1;
            goto ACM_ASYNC_BASE_LONG_TIME_WRITE_ERR;
        }
        down(&LongTimeWriteAsyncComplete[ACM]);
        s32Times--;
    }
    do_gettimeofday(&end);
    printk("ACM:%d,us = %d\n",ACM, (int)((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec)));

ACM_ASYNC_BASE_LONG_TIME_WRITE_ERR:

    if (NULL != buf)
    {
        TG_ACM_FREE(buf);
    }
    up(&LongTimeWriteComplete[ACM]);
    return 0;
}

BSP_S32 BSP_ACM_TEST_WRITE_ASYNC(BSP_U32 u32TestLen)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i;
    //BSP_S32 taskId[BSP_ACM_MAX_NUM]= {0};
    UDI_OPEN_PARAM stOpenParam = {0};
    acm_rw4_block_t* taskarg[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	    stOpenParam.devid = sg_AcmDevTbl[i];
	    s32Fd[i] = udi_open(&stOpenParam);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  == BSP_ERROR)
        {
            printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd[i]);
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    sema_init(&LongTimeWriteComplete[i], 0);
#if 0
		if((LongTimeWriteComplete[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)) == NULL)
        {
            printk("  Error in creating ASYNC_WRITE_COMPLETE0 semaphore  ");
            s32Status = -1;
            goto ACM_BLOCK_BASE_ERR;
        }
#endif
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
	taskarg[i] = (acm_rw4_block_t *)TG_ACM_MALLOC(sizeof(acm_rw4_block_t),1);
	if(NULL==taskarg[i])
	{
		printk("can not alloc struct jtest_rw_block_t\n");
		return -1;
	}

	 taskarg[i]->s32Fd = s32Fd[i];
	 taskarg[i]->acm   = (BSP_U32)i;
	 taskarg[i]->u32TestLen = u32TestLen;

	 kthread_run(ACM_LONG_TIME_WRITE_ASYNC_Task,taskarg[i],"taskIdWriteAsync");
    }

    for(i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        down(&LongTimeWriteComplete[i]);
    }


ACM_BLOCK_BASE_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
	 TG_ACM_FREE(taskarg[i]);
    }
    return BSP_ERROR;
}

BSP_S32 BSP_ACM_TEST_085(VOID)
{
    BSP_U32 u32TestLen = 64;
    return BSP_ACM_TEST_WRITE_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_086(VOID)
{
    BSP_U32 u32TestLen = 128;
    return BSP_ACM_TEST_WRITE_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_087(VOID)
{
    BSP_U32 u32TestLen = 512;
    return BSP_ACM_TEST_WRITE_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_088(VOID)
{
    BSP_U32 u32TestLen = 1024;
    return BSP_ACM_TEST_WRITE_ASYNC(u32TestLen);
}

BSP_S32 BSP_ACM_TEST_089(VOID)
{
    BSP_U32 u32TestLen = 2048;
    return BSP_ACM_TEST_WRITE_ASYNC(u32TestLen);
}


#if 0
BSP_S32 BSP_ACM_MODEM_FLOW_BASE_TEST(BSP_S32 u32FlowTimeout)
{
    BSP_S32 s32Status = 0;
    MODEM_MSC_STRU stModemMsc = {0};
    int fd;
    BSP_U32 u32ReadData;
    UDI_OPEN_PARAM stOpenParam = {0};

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_FLOW_RET;
    }

    /* 先向PC端发数表示测试开始 */
    if (udi_write(fd, (char*)&u32ReadData, sizeof(u32ReadData)) <= 0)
    {
        printk("write data fail\n");
        s32Status = -1;
        goto ACM_MODEM_FLOW_RET;
    }

    printk("first, udi_open modem flow control\n");
    stModemMsc.OP_Cts = 1;
    stModemMsc.ucCts = 0;
    s32Status = udi_ioctl(fd, ACM_MODEM_IOCTL_MSC_WRITE_CMD, &stModemMsc);

    mdelay(u32FlowTimeout*10);

    printk("second, udi_close modem flow control, and the data will come\n");
    stModemMsc.OP_Cts = 1;
    stModemMsc.ucCts = 1;
    s32Status = udi_ioctl(fd, ACM_MODEM_IOCTL_MSC_WRITE_CMD, &stModemMsc);

    /* 在 2.8S 后打开,让PC端丢数,否则,可以读到数据 */
    if (u32FlowTimeout < 280)
    {
        printk("third, beg read data in fifo\n");
        if (udi_read(fd, (char*)&u32ReadData, sizeof(u32ReadData)) <= 0)
        {
            printk("read data fail\n");
            s32Status = -1;
            goto ACM_MODEM_FLOW_RET;
        }
    }

ACM_MODEM_FLOW_RET:
    udi_close(fd);
    return s32Status;
}
#endif

BSP_S32 BSP_ACM_MODEM_WRITE_BASE_TEST(BSP_U32 len, BSP_U8 value, BSP_U32 times)
{
    BSP_S32 s32Status = 0;
    int fd;
    int i;
    char* data;
    UDI_OPEN_PARAM stOpenParam = {0};

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_WRITE_RET;
    }

    for (i = 0; i < times; i++)
    {
        struct sk_buff *new_skb;
        new_skb = dev_alloc_skb(len);
        data = skb_put(new_skb, len);
        memset(data, value,len);
        if (udi_write(fd, (char*)new_skb, len) < 0)
        {
            printk("write data fail\n");
            s32Status = -1;
            goto ACM_MODEM_WRITE_RET;
        }
        msleep(10);
    }

ACM_MODEM_WRITE_RET:
    udi_close(fd);
    return s32Status;
}


BSP_S32 BSP_ACM_TEST_090(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_WRITE_BASE_TEST(100, 0x90, 1);
    RUN_BASE_TEST(" Modem 写 1 次 100 字节 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_091(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_WRITE_BASE_TEST(1024, 0x91, 1);
    RUN_BASE_TEST(" Modem 写 1 次 1024 字节 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_092(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_WRITE_BASE_TEST(200, 0x92, 100);
    RUN_BASE_TEST(" Modem 写 100 次 200 字节 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_093(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_WRITE_BASE_TEST(1024, 0x93, 100);
    RUN_BASE_TEST(" Modem 写 100 次 1024 字节 ", s32Status);
}

static BSP_U32 sg_modem_read_test_fd;
static BSP_U8 sg_modem_read_test_value;
static BSP_S32 sg_modem_read_test_times;
static BSP_U32 sg_modem_read_check;

void ACM_MODEM_READ_DONE_CB(void)
{
    ACM_WR_ASYNC_INFO wr_info;
    int ret;
    struct sk_buff *ret_skb;

    ret = udi_ioctl(sg_modem_read_test_fd, ACM_IOCTL_GET_RD_BUFF, &wr_info);
    if (ret)
    {
        if (sg_modem_read_check)
            printk("modem test fail value:0x%x, ret:%d\n", sg_modem_read_test_value, ret);
        return;
    }
    ret_skb = (struct sk_buff *)wr_info.pBuffer;
    if (sg_modem_read_check)
        BSP_ACM_CMP_MEM_WITH_VALUE(ret_skb->data, ret_skb->len, sg_modem_read_test_value, __LINE__);
    udi_ioctl(sg_modem_read_test_fd, ACM_IOCTL_RETURN_BUFF, &wr_info);
    if (sg_modem_read_test_times > 0)
        sg_modem_read_test_times--;
}

BSP_S32 BSP_ACM_MODEM_READ_BASE_TEST(BSP_U32 len, BSP_U8 value, BSP_U32 times, BSP_U32 is_check)
{
    BSP_S32 s32Status = 0;
    int fd;
    UDI_OPEN_PARAM stOpenParam = {0};

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_READ_RET;
    }
    sg_modem_read_test_fd = fd;
    sg_modem_read_test_value = value;
    sg_modem_read_test_times = times;
    sg_modem_read_check = is_check;

    s32Status = udi_ioctl(fd, ACM_IOCTL_SET_READ_CB, ACM_MODEM_READ_DONE_CB);
    if (s32Status)
    {
        s32Status = -1;
        goto ACM_MODEM_READ_RET;
    }

    /* wait for read cb complete */
    while(sg_modem_read_test_times > 0)
    {
        msleep(10);
    }

ACM_MODEM_READ_RET:
    udi_close(fd);
    sg_modem_read_test_times = 0;
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_094(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_READ_BASE_TEST(100, 0x94, 1, 1);
    RUN_BASE_TEST(" Modem 读 1 次 100 字节 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_095(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_READ_BASE_TEST(1024, 0x95, 1, 1);
    RUN_BASE_TEST(" Modem 读 1 次 1024 字节 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_096(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_READ_BASE_TEST(100, 0x96, 100, 1);
    RUN_BASE_TEST(" Modem 读 100 次 100 字节 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_097(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_READ_BASE_TEST(1024, 0x97, 100, 1);
    RUN_BASE_TEST(" Modem 读 100 次 1024 字节 ", s32Status);
}

static BSP_U8 sg_modem_sig_test_sig_on;
static BSP_U32 sg_modem_sig_test_start;


void ACM_MODEM_TEST_MSC_READ_CB(MODEM_MSC_STRU* pModemMsc)
{
    if (pModemMsc->OP_Dtr == SIGNALCH)
    {
        sg_modem_sig_test_start = 0;
        printk("OP_Dtr check OK\n");
    }
    else
    {
        printk("OP_Dtr check FAIL\n");
    }

    if (pModemMsc->ucDtr == sg_modem_sig_test_sig_on)
    {
        sg_modem_sig_test_start = 0;
        printk("ucDtr check OK\n");
    }
    else
    {
        printk("ucDtr check FAIL\n");
    }
}

BSP_S32 BSP_ACM_MODEM_SIG_BASE_TEST(BSP_U8 sig_on)
{
    BSP_S32 s32Status = 0;
    int fd;
    UDI_OPEN_PARAM stOpenParam = {0};

    sg_modem_sig_test_sig_on = sig_on;
    sg_modem_sig_test_start = 1;

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_READ_RET;
    }

    s32Status = udi_ioctl(fd, ACM_MODEM_IOCTL_SET_MSC_READ_CB, ACM_MODEM_TEST_MSC_READ_CB);
    if (s32Status)
    {
        s32Status = -1;
        goto ACM_MODEM_READ_RET;
    }

    /* wait for sig read cb complete */
    while(sg_modem_sig_test_start > 0)
    {
        msleep(10);
    }

ACM_MODEM_READ_RET:
    udi_close(fd);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_098(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_SIG_BASE_TEST(0);
    RUN_BASE_TEST(" Modem 关 DTR 测试 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_099(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_SIG_BASE_TEST(1);
    RUN_BASE_TEST(" Modem 开 DTR 测试 ", s32Status);
}

BSP_S32 BSP_ACM_MODEM_FLOW_BASE_TEST(BSP_U8 recv_on, BSP_U8 stat_on)
{
    BSP_S32 s32Status = 0;
    int fd;
    UDI_OPEN_PARAM stOpenParam = {0};
    MODEM_MSC_STRU modem_msc = {0};

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_READ_RET;
    }

    modem_msc.OP_Cts = SIGNALCH;
    if (recv_on)
    {
        modem_msc.ucCts = SIGNALCH;
    }
    else
    {
        modem_msc.ucCts = SIGNALNOCH;
    }

    modem_msc.OP_Dcd = SIGNALCH;
    if (stat_on)
    {
        modem_msc.ucDcd = HIGHLEVEL;
    }

    s32Status = udi_ioctl(fd, ACM_MODEM_IOCTL_MSC_WRITE_CMD, &modem_msc);
    if (s32Status)
    {
        s32Status = -1;
        goto ACM_MODEM_READ_RET;
    }

ACM_MODEM_READ_RET:
    udi_close(fd);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_100(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_FLOW_BASE_TEST(0, 0);
    RUN_BASE_TEST(" Modem FLOW 测试 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_101(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_FLOW_BASE_TEST(1, 1);
    RUN_BASE_TEST(" Modem FLOW 测试 ", s32Status);
}

/* long time test for modem */
BSP_S32 BSP_ACM_MODEM_WRITE_SPEED_BASE_TEST(BSP_U32 len, BSP_U32 times)
{
    BSP_S32 s32Status = 0;
    int fd;
    int i;
    char* data;
    UDI_OPEN_PARAM stOpenParam = {0};
    struct timeval start, end;
    BSP_U32 total_byte = 0;
    BSP_U32 total_ms = 0;
    ACM_WR_ASYNC_INFO wr_info;

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_WRITE_RET;
    }

    do_gettimeofday(&start);
    for (i = 0; i < times; i++)
    {
        struct sk_buff *new_skb;
        new_skb = dev_alloc_skb(len);
        data = skb_put(new_skb, len);
        wr_info.pBuffer = (char*)new_skb;
        //memset(data, value,len);
        //if (udi_write(fd, (char*)new_skb, len) < 0)
        if (udi_ioctl(fd, ACM_IOCTL_WRITE_ASYNC, (VOID*)&wr_info) < 0)
        {
            msleep(0);
            dev_kfree_skb(new_skb);
            continue;
        }
        total_byte += len;
    }
    do_gettimeofday(&end);
    total_ms = (BSP_U32)((end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
    printk("\ntotal byte:%d, total_ms:%d\n", total_byte, total_ms);
    if (total_ms > 0)
        printk("modem: %d KB/S \n", (total_byte / total_ms));

ACM_MODEM_WRITE_RET:
    udi_close(fd);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_102(int times)
{
    int i = 0;

    while (times > 0) {
        for (i = 1; i < 1514; i++) {
            BSP_ACM_MODEM_WRITE_SPEED_BASE_TEST(i, 10000);
        }
        times--;
    }
    printk("BSP_ACM_TEST_102 OK\n");
    return 0;
}

BSP_S32 BSP_ACM_TEST_103(int times)
{
    BSP_ACM_MODEM_READ_BASE_TEST(1024, 0x0, times, 0);
    printk("BSP_ACM_TEST_103 OK\n");
    return 0;
}


#if 0
typedef enum tagACM_MODEM_SIG_TYPE
{
    ACM_MODEM_SIG_DSR,
    ACM_MODEM_SIG_DCD,
    ACM_MODEM_SIG_RING,
    ACM_MODE_SIG_MAX
}ACM_MODEM_SIG_TYPE;

BSP_S32 BSP_ACM_MODEM_CHANGE_BASE_TEST(ACM_MODEM_SIG_TYPE sig)
{
    BSP_S32 s32Status = 0;
    MODEM_MSC_STRU stModemMsc = {0};
    int fd;
    UDI_OPEN_PARAM stOpenParam = {0};

    stOpenParam.devid = UDI_ACM_MODEM_ID;
    fd = udi_open(&stOpenParam);
    if (fd <= 0)
    {
        s32Status = -1;
        goto ACM_MODEM_FLOW_RET;
    }

    printk("notify the dsr sig change\n");
    switch(sig)
    {
    case ACM_MODEM_SIG_DSR:
        stModemMsc.OP_Dsr = 1;
        stModemMsc.ucDsr = 1;
        break;
    case ACM_MODEM_SIG_DCD:
        stModemMsc.OP_Dcd = 1;
        stModemMsc.ucDcd = 1;
        break;
    case ACM_MODEM_SIG_RING:
        stModemMsc.OP_Ri = 1;
        stModemMsc.ucRi = 1;
        break;
    default:
        break;
    }

    s32Status = udi_ioctl(fd, ACM_MODEM_IOCTL_MSC_WRITE_CMD, &stModemMsc);

ACM_MODEM_FLOW_RET:
    udi_close(fd);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_092(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_CHANGE_BASE_TEST(ACM_MODEM_SIG_DSR);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_093(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_CHANGE_BASE_TEST(ACM_MODEM_SIG_DCD);
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_094(VOID)
{
    BSP_S32 s32Status;
    s32Status = BSP_ACM_MODEM_CHANGE_BASE_TEST(ACM_MODEM_SIG_RING);
    return s32Status;
}
#endif


/* select 接口暂不支持, 先保留测试代码 */
#if 0
BSP_S32 BSP_ACM_TEST_009(VOID)
{
#define ACM_ST_09_LEN 200
    fd_set rfds;
    int fd  = 0;
    int ret = 0;
    int waittime = 15;
    char buf[1024] = {0};
    struct timeval tv;
    int retval;
    BSP_S32 s32Status = 0;

    fd = udi_open("/tyCo/2", O_RDWR, 0);
    if (fd < 0)
    {
        printk("udi_open tyCo dev fail\n", 0, 0, 0, 0, 0, 0);
        return;
    }

    if (udi_ioctl(fd, FIOSETOPTIONS, OPT_ECHO) != 0)
    {
        printk("udi_ioctl OPT_ECHO error\n");
    }

    /* Watch /tyCo/2 dev(fd) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    /* Wait up to n seconds. */
    tv.tv_sec  = waittime;
    tv.tv_usec = 0;

    printk("beg select fd:%d\n", fd);
    retval = select(fd + 1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
    {
        printk("select err.\n");
        s32Status= -1;
    }
    else if (retval)
    {
        printk("Data is available now.\n");
        ret = read(fd, buf, ACM_ST_09_LEN);
        printk("### read/write end, want:%d, read/write:%d, first byte:0x%x ###\n", ACM_ST_09_LEN, ret, buf[0]);
    }
    else
    {
        printk("No data within %d seconds.\n", waittime);
    }
    udi_close(fd);

    RUN_BASE_TEST(" 测试select接口 ", s32Status);
}
#endif

BSP_U32 sg_u32LongTimeTaskState = 0;

/*
长时间读写测试任务
*/

static int ACM_LONG_TIME_RW_BLOCK_Task(void* arg)
{
    jtest_rw_block_t* taskarg = (void*)arg;
	BSP_S32 s32Fd = taskarg->s32Fd;
	BSP_U32 u32Cnt = taskarg->u32Cnt;
	BSP_U32 u32AcmNum = taskarg->u32AcmNum;
	BSP_U32 u32MaxSize = taskarg->u32MaxSize;
	BSP_BOOL bCacheAlign = taskarg->bCacheAlign;
	BSP_BOOL bRead = taskarg->bRead;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_S32 ret;
    //BSP_U32 u32TickStart, u32TickEnd;
    BSP_U32 u32Length;
    BSP_U32 u32Left = u32Cnt;
    //BSP_S32 s32Rand;

    if (s32Fd <= 0)
    {
        printk("udi_open / udi_close ACM:%d error:%d\n", u32AcmNum, s32Fd);
        goto ACM_LONG_TIME_RW_ERR;
    }

    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC(u32MaxSize,1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        bufFree = buf = TG_ACM_MALLOC(u32MaxSize+1,1);
    }

    if (NULL == buf)
    {
        goto ACM_LONG_TIME_RW_ERR;
    }

    /* 填充初始值 */
    if (bRead)
    {
        memset(buf, 0, u32MaxSize);
    }
    else
    {
        memset(buf, 0x40+u32AcmNum, u32MaxSize);
    }

    printk("ACM order :%d beg %s block test, test buf:0x%x\n", u32AcmNum, (bRead)?("read"):("write"), (BSP_U32)buf);

    while(1)
    {
        //u32Length = ((s32Rand = acm_rand(1, u32MaxSize)) > u32Left) ? (u32Left) : (s32Rand);
        u32Length = acm_rand(1, u32MaxSize);
        if ((BSP_S32)u32Length > (BSP_S32)u32MaxSize)
        {
            //printk("rand test error:%d\n", u32Length);
            u32Length = u32MaxSize;
        }
        if (bRead)
        {
            ret = ReadAll(s32Fd, buf, u32Length);
            if (ret < 0)
            {
                printk("ACM:%d read error, fd:%d, left:%d\n", u32AcmNum, s32Fd, u32Left);
                goto ACM_LONG_TIME_RW_ERR;
            }
        }
        else
        {
            ret = WriteAll(s32Fd, buf, u32Length);
            if (ret < 0)
            {
                printk("ACM:%d write error, fd:%d, left:%d\n", u32AcmNum, s32Fd, u32Left);
                goto ACM_LONG_TIME_RW_ERR;
            }
            mdelay(10);
        }
        //u32Left -= u32Length;
    }

    //printk("Acm:%d %s long time test OK\n", u32AcmNum, (bRead)?("read"):("write"));

ACM_LONG_TIME_RW_ERR:
    sg_u32LongTimeTaskState &= ~(1 << u32AcmNum);

    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return 0;
}

BSP_S32 BSP_ACM_LONG_TIME_BLOCK_BASE_TEST(BSP_U32 u32Cnt, BSP_U32 u32TaskPri)
{
    BSP_S32 s32Status = 0;
    BSP_S32 i;
    BSP_U8 strName[64] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    UDI_OPEN_PARAM stOpenParam = {0};
    jtest_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM];
    jtest_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
    stOpenParam.devid = sg_AcmDevTbl[i];
    s32Fd[i] = udi_open(&stOpenParam);
    }

    sg_u32LongTimeTaskState = 0;

    /* 创建3个任务发送数据 */
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sg_u32LongTimeTaskState |= (1 << i);
        sprintf(strName, "tACM_W:%d", i);
        taskarg_w[i] = (jtest_rw_block_t *)TG_ACM_MALLOC(sizeof(jtest_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

        taskarg_w[i]->s32Fd = s32Fd[i];
        taskarg_w[i]->u32Cnt = u32Cnt;
        taskarg_w[i]->u32AcmNum = i;
        taskarg_w[i]->u32MaxSize = 2048;
        taskarg_w[i]->bCacheAlign = FALSE;
        taskarg_w[i]->bRead = FALSE;

        kthread_run(ACM_LONG_TIME_RW_BLOCK_Task,taskarg_r[i],strName);
    }

    /* 创建3个任务接收数据 */
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sg_u32LongTimeTaskState |= (1 << (i+BSP_ACM_SIO_NUM));
        sprintf(strName, "tACM_R:%d", i);
        taskarg_r[i] = (jtest_rw_block_t *)TG_ACM_MALLOC(sizeof(jtest_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
            printk("can not alloc struct jtest_rw_block_t\n");
            return -1;
        }
        taskarg_r[i]->s32Fd = s32Fd[i];
        taskarg_r[i]->u32Cnt = u32Cnt;
        taskarg_r[i]->u32AcmNum = (i+BSP_ACM_SIO_NUM);
        taskarg_r[i]->u32MaxSize = 2048;
        taskarg_r[i]->bCacheAlign = FALSE;
        taskarg_r[i]->bRead = TRUE;

        kthread_run(ACM_LONG_TIME_RW_BLOCK_Task,taskarg_r[i],strName);
        /*taskSpawn (strName, u32TaskPri+1, 0, 4096, (FUNCPTR)ACM_LONG_TIME_RW_BLOCK_Task,
                   s32Fd[i], u32Cnt, (i+BSP_ACM_SIO_NUM), 2048, FALSE, TRUE,0,0,0,0);*/
    }

    while(sg_u32LongTimeTaskState)
    {
        mdelay(500);
    }

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        udi_close(s32Fd[i]);
        TG_ACM_FREE(taskarg_r[i]);
        TG_ACM_FREE(taskarg_w[i]);
    }

    return s32Status;
}

/* 只发送ACM Bulk数据 */
BSP_S32 BSP_ACM_TEST_0200(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_BLOCK_BASE_TEST(10000, 130);
    RUN_BASE_TEST(" 长时间同步传输, 只发送ACM Bulk数据 ", s32Status);
}

/*
    ACM Bulk数据 + ctrl 数据

    注意:

    !!!! 请先关闭宏: ACM_CAP_SHELL_REDIRECT !!!!!
*/
BSP_S32 BSP_ACM_TEST_0210(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_BLOCK_BASE_TEST(10000, 130);
    RUN_BASE_TEST(" 长时间同步传输, ACM Bulk数据 + ctrl 数据 ", s32Status);
}

/*
    ACM 异步读写操作
*/
static struct semaphore sg_AcmLongTimeEvt[6];

static VOID ACM_LONG_TIME_R_ASYNC_CB_1(VOID)
{
    up(&sg_AcmLongTimeEvt[0]);
}

static VOID ACM_LONG_TIME_R_ASYNC_CB_2(VOID)
{
    up(&sg_AcmLongTimeEvt[1]);
}

static VOID ACM_LONG_TIME_R_ASYNC_CB_3(VOID)
{
    up(&sg_AcmLongTimeEvt[2]);
}

static VOID ACM_LONG_TIME_R_ASYNC_CB_4(VOID)
{
    up(&sg_AcmLongTimeEvt[3]);
}

static VOID ACM_LONG_TIME_R_ASYNC_CB_5(VOID)
{
    up(&sg_AcmLongTimeEvt[4]);
}

static VOID ACM_LONG_TIME_R_ASYNC_CB_6(VOID)
{
    up(&sg_AcmLongTimeEvt[5]);
}

ACM_READ_DONE_CB_T sg_AcmLongReadAsyncCB[6] = {
    ACM_LONG_TIME_R_ASYNC_CB_1,
    ACM_LONG_TIME_R_ASYNC_CB_2,
    ACM_LONG_TIME_R_ASYNC_CB_3,
    ACM_LONG_TIME_R_ASYNC_CB_4,
    ACM_LONG_TIME_R_ASYNC_CB_5,
    ACM_LONG_TIME_R_ASYNC_CB_6,
};

static VOID ACM_LONG_TIME_W_ASYNC_CB(char* pDonebuff, int size)
{
    return;
}

static int ACM_LONG_TIME_RW_ASYNC_Task(void* arg)
{
    jtest_rw_block_t* taskarg = (void*)arg;
	BSP_S32 s32Fd = taskarg->s32Fd;
	BSP_U32 u32Cnt = taskarg->u32Cnt;
	BSP_U32 u32AcmNum = taskarg->u32AcmNum;
	BSP_U32 u32MaxSize = taskarg->u32MaxSize;
	BSP_BOOL bCacheAlign = taskarg->bCacheAlign;
	BSP_BOOL bRead = taskarg->bRead;
    BSP_U8* buf = NULL;
    BSP_U8* bufFree = NULL;
    BSP_S32 ret;
    //BSP_U32 u32TickStart, u32TickEnd;
    BSP_U32 u32Length;
    BSP_U32 u32Left = u32Cnt;
    //BSP_S32 s32Rand;
    ACM_WR_ASYNC_INFO stRWInfo = {0};

    if (s32Fd <= 0)
    {
        printk("udi_open / udi_close ACM:%d error:%d\n", u32AcmNum, s32Fd);
        goto ACM_LONG_TIME_ASYNC_RW_ERR;
    }

    if (bCacheAlign)
    {
        bufFree = TG_ACM_MALLOC(u32MaxSize,1);
        buf = TG_ACM_GET_LIGN_ADDR(bufFree);
    }
    else
    {
        bufFree = buf = TG_ACM_MALLOC(u32MaxSize+1,1);
    }
    if (NULL == buf)
    {
        goto ACM_LONG_TIME_ASYNC_RW_ERR;
    }

    /* 填充初始值 */
    if (bRead)
    {
        memset(buf, 0, u32MaxSize);
        if (u32AcmNum > BSP_ACM_SIO_NUM)
        {
            printk("u32AcmNum error:%d\n", u32AcmNum);
            goto ACM_LONG_TIME_ASYNC_RW_ERR;
        }
    }
    else
    {
        memset(buf, 0x50+u32AcmNum, u32MaxSize);
    }

    printk("ACM order :%d beg %s async test, test buf:0x%x\n", u32AcmNum, (bRead)?("read"):("write"), (BSP_U32)buf);

    while(1)
    {
        if (bRead)
        {
            down(&sg_AcmLongTimeEvt[u32AcmNum]);
            while((ret = udi_ioctl(s32Fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo)) == 0)
            {
                u32Length = (u32MaxSize > stRWInfo.u32Size) ? (stRWInfo.u32Size) : (u32MaxSize);
                memcpy(buf, stRWInfo.pBuffer, u32Length);
                udi_ioctl(s32Fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo);
            }
        }
        else
        {
            stRWInfo.pBuffer = buf;
            //u32Length = (u32MaxSize > u32Left) ? (u32Left) : (u32MaxSize);
            u32Length = u32MaxSize;
            stRWInfo.u32Size = u32Length;

            ret = udi_ioctl(s32Fd, ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
            if (ret < 0)
            {
                if (ret == 0x90140003)
                {
                    mdelay(10);
                    continue;
                }
                printk("ACM:%d async write error, fd:%d, left:%d, ret:0x%x\n", u32AcmNum, s32Fd, u32Left, ret);
                goto ACM_LONG_TIME_ASYNC_RW_ERR;
            }
            mdelay(10);
        }
        //u32Left -= u32Length;
    }

    //printk("Acm:%d async %s long time test OK\n", u32AcmNum, (bRead)?("read"):("write"));

ACM_LONG_TIME_ASYNC_RW_ERR:
    sg_u32LongTimeTaskState &= ~(1 << u32AcmNum);

    if (NULL != bufFree)
    {
        TG_ACM_FREE(bufFree);
    }
    return 0;
}

BSP_S32 BSP_ACM_LONG_TIME_ASYNC_BASE_TEST(BSP_U32 u32Cnt, BSP_U32 u32TaskPri)
{
    BSP_S32 s32Status = 0;
    BSP_S32 i;
    BSP_U8 strName[64] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    jtest_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM];
    jtest_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];
    UDI_OPEN_PARAM stOpenParam = {0};

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    	sema_init(&sg_AcmLongTimeEvt[i], 0);
        //sg_AcmLongTimeEvt[i] = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

        if (s32Fd[i] < 0)
        {
            goto LONG_TIME_ASYNC_BASE_RET;
        }
    }

    sg_u32LongTimeTaskState = 0;

    /* 创建3个任务接收数据 */
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sg_u32LongTimeTaskState |= (1 << i);
        sprintf(strName, "tACM_R:%d", i);

        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_READ_CB, sg_AcmLongReadAsyncCB[i]) != BSP_OK)
        {
            printk("ACM_IOCTL_SET_READ_CB fail\n");
            goto LONG_TIME_ASYNC_BASE_RET;
        }
        taskarg_r[i] = (jtest_rw_block_t *)TG_ACM_MALLOC(sizeof(jtest_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

        taskarg_r[i]->s32Fd = s32Fd[i];
        taskarg_r[i]->u32Cnt = u32Cnt;
        taskarg_r[i]->u32AcmNum = i;
        taskarg_r[i]->u32MaxSize = 2048;
        taskarg_r[i]->bCacheAlign = FALSE;
        taskarg_r[i]->bRead = TRUE;

        kthread_run(ACM_LONG_TIME_RW_ASYNC_Task,taskarg_r[i],strName);
    }

    /* 创建3个任务发送数据 */
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sg_u32LongTimeTaskState |= (1 << (i+BSP_ACM_SIO_NUM));
        sprintf(strName, "tACM_W:%d", i);

        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_WRITE_CB, ACM_LONG_TIME_W_ASYNC_CB) != BSP_OK)
        {
            printk("ACM_IOCTL_SET_WRITE_CB fail\n");
            goto LONG_TIME_ASYNC_BASE_RET;
        }
        taskarg_w[i] = (jtest_rw_block_t *)TG_ACM_MALLOC(sizeof(jtest_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

        taskarg_w[i]->s32Fd = s32Fd[i];
        taskarg_w[i]->u32Cnt = u32Cnt;
        taskarg_w[i]->u32AcmNum = (i+BSP_ACM_SIO_NUM);
        taskarg_w[i]->u32MaxSize = 2048;
        taskarg_w[i]->bCacheAlign = FALSE;
        taskarg_w[i]->bRead = FALSE;

        kthread_run(ACM_LONG_TIME_RW_ASYNC_Task,taskarg_w[i],strName);
       /* taskSpawn (strName, u32TaskPri+1, 0, 4096, (FUNCPTR)ACM_LONG_TIME_RW_ASYNC_Task,
                   s32Fd[i], u32Cnt, (i+BSP_ACM_SIO_NUM), 2048, FALSE);*/
    }

    while(sg_u32LongTimeTaskState)
    {
        mdelay(500);
    }

LONG_TIME_ASYNC_BASE_RET:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i] != 0)
        {
            udi_close(s32Fd[i]);
        }
        TG_ACM_FREE(taskarg_w[i]);
        TG_ACM_FREE(taskarg_r[i]);
    }
    printk("long time test end\n");
    return s32Status;
}

BSP_S32 BSP_ACM_TEST_0220(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_ASYNC_BASE_TEST(10000, 130);
    RUN_BASE_TEST(" 长时间异步传输, 只发送ACM Bulk数据 ", s32Status);
}

/*
    ACM异步同步测试
*/
BSP_S32 BSP_ACM_LONG_TIME_MIX_BASE_TEST_1(BSP_U32 u32Cnt, BSP_U32 u32TaskPri)
{
    //BSP_S32 s32Status = 0;
    BSP_S32 i;
    BSP_U8 strName[64] = {0};
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    jtest_rw_block_t* taskarg_r[CONFIG_FD_CDC_ACM];
    jtest_rw_block_t* taskarg_w[CONFIG_FD_CDC_ACM];
    UDI_OPEN_PARAM stOpenParam = {0};

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        stOpenParam.devid = sg_AcmDevTbl[i];
        s32Fd[i] = udi_open(&stOpenParam);
    	sema_init(&sg_AcmLongTimeEvt[i], 0);
        //sg_AcmLongTimeEvt[i] = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

        if (s32Fd[i] < 0)
        {
            goto LONG_TIME_MIX_BASE_1_RET;
        }
    }

    sg_u32LongTimeTaskState = 0;

    /* 创建3个任务接收数据 */
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sg_u32LongTimeTaskState |= (1 << i);
        sprintf(strName, "tACM_R:%d", i);

        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_READ_CB, sg_AcmLongReadAsyncCB[i]) != BSP_OK)
        {
            printk("ACM_IOCTL_SET_READ_CB fail\n");
            goto LONG_TIME_MIX_BASE_1_RET;
        }
        taskarg_r[i] = (jtest_rw_block_t *)TG_ACM_MALLOC(sizeof(jtest_rw_block_t),1);
        if(NULL==taskarg_r[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

         taskarg_r[i]->s32Fd = s32Fd[i];
         taskarg_r[i]->u32Cnt = u32Cnt;
         taskarg_r[i]->u32AcmNum = i;
         taskarg_r[i]->u32MaxSize = 2048;
         taskarg_r[i]->bCacheAlign = FALSE;
         taskarg_r[i]->bRead = TRUE;

        if (i & 0x1)
        {
            kthread_run(ACM_LONG_TIME_RW_ASYNC_Task,taskarg_r[i],strName);
        }
        else
        {
            kthread_run(ACM_LONG_TIME_RW_BLOCK_Task,taskarg_r[i],strName);
        }
	/*
        if (i & 0x1)
        {
            taskSpawn (strName, u32TaskPri, 0, 4096, (FUNCPTR)ACM_LONG_TIME_RW_ASYNC_Task,
                   s32Fd[i], u32Cnt, i, 2048, TRUE);
        }
        else
        {
            taskSpawn (strName, u32TaskPri, 0, 4096, (FUNCPTR)ACM_LONG_TIME_RW_BLOCK_Task,
                   s32Fd[i], u32Cnt, i, 2048, FALSE, TRUE,0,0,0,0);
        }
        */
    }

    /* 创建3个任务发送数据 */
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        sg_u32LongTimeTaskState |= (1 << (i+BSP_ACM_SIO_NUM));
        sprintf(strName, "tACM_W:%d", i);
        taskarg_w[i] = (jtest_rw_block_t *)TG_ACM_MALLOC(sizeof(jtest_rw_block_t),1);
        if(NULL==taskarg_w[i])
        {
        	printk("can not alloc struct jtest_rw_block_t\n");
        	return -1;
        }

        taskarg_w[i]->s32Fd = s32Fd[i];
        taskarg_w[i]->u32Cnt = u32Cnt;
        taskarg_w[i]->u32AcmNum = i;
        taskarg_w[i]->u32MaxSize = 2048;
        taskarg_w[i]->bCacheAlign = FALSE;
        taskarg_w[i]->bRead = FALSE;

        if (i & 0x1)
        {
            if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_WRITE_CB, ACM_LONG_TIME_W_ASYNC_CB) != BSP_OK)
            {
                printk("ACM_IOCTL_SET_WRITE_CB fail\n");
                goto LONG_TIME_MIX_BASE_1_RET;
            }
            kthread_run(ACM_LONG_TIME_RW_ASYNC_Task,taskarg_w[i],strName);
        }
        else
        {
           kthread_run(ACM_LONG_TIME_RW_BLOCK_Task,taskarg_w[i],strName);
        }
	/*
        if (i & 0x1)
        {
            taskSpawn (strName, u32TaskPri+1, 0, 4096, (FUNCPTR)ACM_LONG_TIME_RW_ASYNC_Task,
                   s32Fd[i], u32Cnt, (i+BSP_ACM_SIO_NUM), 2048, FALSE);
        }
        else
        {
            taskSpawn (strName, u32TaskPri, 0, 4096, (FUNCPTR)ACM_LONG_TIME_RW_BLOCK_Task,
                   s32Fd[i], u32Cnt, (i+BSP_ACM_SIO_NUM), 2048, FALSE, FALSE,0,0,0,0);
        }*/
    }

    while(sg_u32LongTimeTaskState)
    {
        mdelay(500);
    }

LONG_TIME_MIX_BASE_1_RET:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i] != 0)
        {
            udi_close(s32Fd[i]);
        }
        TG_ACM_FREE(taskarg_r[i]);
        TG_ACM_FREE(taskarg_w[i]);
    }
    printk("long time test end\n");
    return 0;
}

BSP_S32 BSP_ACM_TEST_0230(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_MIX_BASE_TEST_1(10000, 130);
    RUN_BASE_TEST(" 长时间异步/同步传输, 只发送ACM Bulk数据 ", s32Status);
}

/*
    ACM 阻塞操作的长时间循环测试
*/
BSP_S32 BSP_ACM_TEST_INFINIT_01(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_BLOCK_BASE_TEST((BSP_U32)(-1), 130);
    RUN_BASE_TEST(" 长时间传输, ACM Bulk数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_INFINIT_02(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_BLOCK_BASE_TEST((BSP_U32)(-1), 130);
    RUN_BASE_TEST(" 长时间传输, ACM Bulk数据 + ctrl 数据 ", s32Status);
}

BSP_S32 BSP_ACM_TEST_INFINIT_03(VOID)
{
    BSP_S32 s32Status = 0;
    s32Status = BSP_ACM_LONG_TIME_MIX_BASE_TEST_1((BSP_U32)(-1), 130);
    RUN_BASE_TEST(" 长时间异步/同步传输, 只发送ACM Bulk数据 ", s32Status);
}

#if 0
VOID ACM_ST_10_Task(BSP_U32 u32Cnt, BSP_U32 u32AcmNum)
{
#define ACM_ST_10_LEN 2048
#define ACM_ST_10_STR_LEN 16

    BSP_U8* pStr = "This is acm st\r\n"; /* 长度必须是 16 */
    BSP_U8* buf = NULL, *cur = NULL;
    BSP_U32 i = 0;
    BSP_U32 u32SendLen;
    BSP_S32 ret;
    BSP_S32 s32Fd;
    BSP_U32 u32Total = 0;
    BSP_U32 u32TickStart, u32TickEnd;
    BSP_U8 pBuffer[64];
    BSP_U32 u32WtCnt = 256;

    switch (u32AcmNum)
    {
    case 0:
        s32Fd = udi_open(ACM_1, O_RDWR, 0);
        break;
    case 1:
        s32Fd = udi_open(ACM_2, O_RDWR, 0);
        break;
    case 2:
        s32Fd = udi_open(ACM_3, O_RDWR, 0);
        break;
    default:
        printk("u32AcmNum is:%d, error, must be [0~2]\n", u32AcmNum);
        return;
    }
    if (s32Fd <= 0)
    {
        printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd);
        goto ACM_010_TSK_ERR;
    }

    buf = memalign(32, ACM_ST_10_LEN);
    if (NULL == buf)
    {
        goto ACM_010_TSK_ERR;
    }
    /* 填充字符串 */
    cur = buf;
    for (i = 0; i < ACM_ST_10_LEN; )
    {
        memcpy(cur, pStr, ACM_ST_10_STR_LEN);
        i += ACM_ST_10_STR_LEN;
        cur += ACM_ST_10_STR_LEN;
    }

    printk("loop cnt:%d\n", u32Cnt);

    u32TickStart = tickGet();

    while(u32Cnt--)
    {
        ret = read(s32Fd, pBuffer, 1);
        if (ret < 0)
        {
            printk("read fail, ret:0x%x\n", ret);
            goto ACM_010_TSK_ERR;
        }
        u32WtCnt = 256;

        /* 开始循环发送 */
        while (u32WtCnt--)
        {
            u32SendLen = acm_rand(ACM_ST_10_STR_LEN, ACM_ST_10_LEN);
            ret = WriteAll(s32Fd, buf, u32SendLen);
            if (ret < 0)
            {
                printk("ACM:%d write error, fd:%d\n", u32AcmNum, s32Fd);
                goto ACM_010_TSK_ERR;
            }
            u32Total += u32SendLen;
        }
    }

    u32TickEnd = tickGet();
    printk("TickStart:%d, TickEnd:%d\n", u32TickStart, u32TickEnd);
    printk("ACM%d: %d B/S \n", u32AcmNum, u32Total/(u32TickEnd - u32TickStart) * 100);

ACM_010_TSK_ERR:
    if (s32Fd > 0)
    {
        udi_close(s32Fd);
    }
    if (NULL != buf)
    {
        free(buf);
    }
    return;
}

BSP_S32 BSP_ACM_TEST_010(VOID)
{
#define ACM_ST_10_ST_NUM 0x7fffffff

    BSP_S32 s32Status = 0;
    BSP_S32 i;

    /* 创建3个任务发送数据 */
    //for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    i = 2;
    {
        //ACM_ST_10_Task(1, i, s32Fd[i]);
        taskSpawn ("ACM_TEST_010", 130, 0, 4096, (FUNCPTR)ACM_ST_10_Task,
                   ACM_ST_10_ST_NUM,i,0,0,0);
    }

ACM_010_ERR:

	RUN_BASE_TEST(" 大数据量传输(16 ~ 2048) ", s32Status);
}

VOID BSP_ACM_TEST_011_BASE(int cnt, int len)
{
#define ACM_ST_11_STR_LEN 16
    BSP_U8* pStr = "This is acm st\r\n"; /* 长度必须是 16 */
    BSP_U8* buf = NULL, *cur = NULL;
    BSP_U32 i = 0;
    BSP_U32 u32SendLen;
    BSP_S32 ret;
    BSP_S32 s32Fd;
    BSP_U32 u32Cnt = cnt;
    BSP_U32 u32TickStart, u32TickEnd;

    s32Fd = udi_open(ACM_1, O_RDWR, 0);
    if (s32Fd <= 0)
    {
        printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd);
        goto ACM_011_TSK_ERR;
    }

    buf = memalign(32, len);
    if (NULL == buf)
    {
        goto ACM_011_TSK_ERR;
    }
    /* 填充字符串 */
    cur = buf;
    for (i = 0; i < len; )
    {
        memcpy(cur, pStr, ACM_ST_11_STR_LEN);
        i += ACM_ST_11_STR_LEN;
        cur += ACM_ST_11_STR_LEN;
    }

    printk("loop cnt:%d\n", u32Cnt);

    u32TickStart = tickGet();
    /* 开始循环发送 */
    while (u32Cnt--)
    {
        ret = WriteAll(s32Fd, buf, len);
        if (ret < 0)
        {
            printk("ACM:%d write error, fd:%d\n", 0, s32Fd);
            goto ACM_011_TSK_ERR;
        }
    }
    u32TickEnd = tickGet();
    printk("TickStart:%d, TickEnd:%d\n", u32TickStart, u32TickEnd);
    printk("ACM: %d B/S \n", (len*cnt)/(u32TickEnd - u32TickStart) * 100);

ACM_011_TSK_ERR:
    if (s32Fd > 0)
    {
        udi_close(s32Fd);
    }
    if (NULL != buf)
    {
        free(buf);
    }
    return;
}

VOID BSP_ACM_TEST_011_01()
{
    BSP_ACM_TEST_011_BASE(1000, 192);
}

VOID BSP_ACM_TEST_011_02()
{
    BSP_ACM_TEST_011_BASE(1000, 512);
}

VOID BSP_ACM_TEST_011_03()
{
    BSP_ACM_TEST_011_BASE(1000, 1024);
}

VOID BSP_ACM_TEST_011_04()
{
    BSP_ACM_TEST_011_BASE(1000, 2047);
}

VOID BSP_ACM_TEST_011_05()
{
    BSP_ACM_TEST_011_BASE(1000, 4095);
}

VOID BSP_ACM_TEST_011_06()
{
    BSP_ACM_TEST_011_BASE(1000, 40960);
}

#define ACM_ST_INC_POS(pos, max) ((pos)=(++(pos))%(max))
#define ACM_ST_CHECK_POS(pos, max, last) (((pos +1)%(max)) == (last))

BSP_U32 sg_ST_012_u32WritePos = 0;
BSP_U32 sg_ST_012_u32WriteDonePos = 0;
BSP_S32 sg_ST_012_s32WriteDoneCnt = 0;
SEM_ID  sg_ST_012_sem = NULL;

VOID ACM_TEST_012_WRITE_DONE_CB(char* pDoneBuff, int s32DoneSize)
{
    sg_ST_012_s32WriteDoneCnt--;
    if (sg_ST_012_s32WriteDoneCnt <= 0)
    {
        semGive(sg_ST_012_sem);
    }
    return;
}

VOID BSP_ACM_TEST_012_BASE(int cnt, int bufnum, int buflen)
{
#define ACM_ST_12_STR_LEN 16
    BSP_U8* pStr = "This is acm st\r\n"; /* 长度必须是 16 */
    BSP_U8* buf = NULL, *cur = NULL;
    BSP_U32 i = 0;
    BSP_U32 u32SendLen;
    BSP_S32 ret;
    BSP_S32 s32Fd;
    BSP_U32 u32Cnt = cnt;
    BSP_U32 u32BufCnt = 0;
    BSP_U32 u32TickStart, u32TickEnd;
    ACM_WR_ASYNC_INFO stRWInfo;

    sg_ST_012_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    s32Fd = udi_open(ACM_1, O_RDWR, 0);
    if (s32Fd <= 0)
    {
        printk("udi_open / udi_close ACM:%d error:%d\n", i, s32Fd);
        goto ACM_012_TSK_ERR;
    }

    if (udi_ioctl(s32Fd, ACM_IOCTL_SET_WRITE_CB, ACM_TEST_012_WRITE_DONE_CB) != BSP_OK)
    {
        printk("udi_ioctl ACM:%d error:%d\n", i, s32Fd);
        goto ACM_012_TSK_ERR;
    }

    buf = memalign(32, bufnum * buflen);
    if (NULL == buf)
    {
        goto ACM_012_TSK_ERR;
    }
    /* 填充字符串 */
    cur = buf;
    for (i = 0; i < bufnum * buflen; )
    {
        memcpy(cur, pStr, ACM_ST_12_STR_LEN);
        i += ACM_ST_12_STR_LEN;
        cur += ACM_ST_12_STR_LEN;
    }

    printk("loop cnt:%d\n", u32Cnt);

    sg_ST_012_s32WriteDoneCnt = u32Cnt;

    u32TickStart = tickGet();
    cur = buf;

    /* 开始循环发送 */
    while (u32Cnt != 0)
    {
        stRWInfo.pBuffer = buf + sg_ST_012_u32WritePos*buflen;
        stRWInfo.u32Size = buflen;
        ret = udi_ioctl(s32Fd, ACM_IOCTL_WRITE_ASYNC, &stRWInfo);
        if (ret < 0)
        {
            continue;
            //goto ACM_012_TSK_ERR;
        }
        u32Cnt--;
        ACM_ST_INC_POS(sg_ST_012_u32WritePos, bufnum);
    }

    if (OK != semTake (sg_ST_012_sem, WAIT_FOREVER))
    {
        printk("wate for done error\n");
    }

    u32TickEnd = tickGet();
    printk("TickStart:%d, TickEnd:%d\n", u32TickStart, u32TickEnd);
    printk("ACM ASYNC Write: %d B/S \n", (buflen*cnt)/(u32TickEnd - u32TickStart) * 100);

ACM_012_TSK_ERR:
    if (sg_ST_012_sem != NULL)
    {
        semDelete(sg_ST_012_sem);
    }
    if (s32Fd > 0)
    {
        udi_close(s32Fd);
    }
    if (NULL != buf)
    {
        free(buf);
    }
    return;
}

VOID BSP_ACM_TEST_012_01()
{
    BSP_ACM_TEST_012_BASE(1000, 32, 1024);
}

VOID BSP_ACM_TEST_012_02()
{
    BSP_ACM_TEST_012_BASE(1000, 32, 2048);
}

VOID BSP_ACM_TEST_012_03()
{
    BSP_ACM_TEST_012_BASE(1000, 32, 4096);
}

VOID BSP_ACM_TEST_012_04()
{
    BSP_ACM_TEST_012_BASE(100, 10, 40960);
}

BSP_S32 g_s32SuspendCnt = 0;

VOID BSP_ACM_TEST_013_CB(ACM_EVT_E enEvt)
{
    if (ACM_EVT_DEV_SUSPEND == enEvt)
    {
        printk("[ACM_TEST_013_CB] ACM Suspend\n", 0);
        g_s32SuspendCnt++;
    }
    else if (ACM_EVT_DEV_READY == enEvt)
    {
        printk("[ACM_TEST_013_CB] ACM Resume\n", 0);
    }
    else
    {
        printk("[ACM_TEST_013_CB] ACM Event Error\n", 0);
    }
}

BSP_S32 BSP_ACM_TEST_013(VOID)
{
    BSP_S32 s32Status = 0;
    BSP_S32 s32Fd[BSP_ACM_MAX_NUM] = {0};
    BSP_S32 i, ret;

    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (0 == i)
        {
            s32Fd[i] = udi_open(ACM_1, O_RDWR, 0);
        }
        else if (1 == i)
        {
            s32Fd[i] = udi_open(ACM_2, O_RDWR, 0);
        }
        else if (2 == i)
        {
            s32Fd[i] = udi_open(ACM_3, O_RDWR, 0);
        }

        if (udi_ioctl(s32Fd[i], ACM_IOCTL_SET_EVT_CB, BSP_ACM_TEST_013_CB) != BSP_OK)
        {
            printk("udi_ioctl ACM:%d error:%d, line:%d\n", i, s32Fd, __LINE__);
            goto ACM_013_ERR;
        }
    }

    while(g_s32SuspendCnt < 10)
    {
        taskDelay(10);
    }

ACM_013_ERR:
    for (i = 0; i < BSP_ACM_SIO_NUM; i++)
    {
        if (s32Fd[i]  != BSP_ERROR)
        {
            udi_close(s32Fd[i]);
        }
    }

    RUN_BASE_TEST(" USB 事件通知 ", s32Status);
}

#endif

/*
出错异常测试
*/


/*
    ====== For ST FrameWork =======
*/

#if 0
TEST_CASE_STRU acm_test_case[] = {
    ADD_TEST_CASE(BSP_ACM_TEST_000, " 打开关闭虚拟串口设备 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_001, " 读小于512字节的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_002, " 读等于512字节的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_003, " 读大于于512字节(但非512整数倍)的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_004, " 读大于于512字节(是512整数倍)的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_005, " 向主机写入小于512字节(非512整数倍)的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_006, " 向主机写入等于512字节的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_007, " 向主机写入大于512字节(但非512整数倍)的数据 ", AUTO_RUN),
    ADD_TEST_CASE(BSP_ACM_TEST_008, " 向主机写入大于512字节(且为512整数倍)的数据 ", AUTO_RUN),
    //ADD_TEST_CASE(BSP_ACM_TEST_009, " 测试select接口 ", AUTO_RUN),

	ADD_TEST_CASE_END()
};

BSP_S32 bsp_acm_test(CASE_RUN_TYPE runType)
{
	return executeModuleTest("CDC_ACM", acm_test_case, runType);
}
#endif

#endif

#endif /*INCLUDE_SYSTEM_TEST*/

#endif /* DRV_BUILD_SEPARATE */

