

#ifndef __BALONG_CPUFREQ_H__
#define __BALONG_CPUFREQ_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <product_config.h>
#include <osl_types.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <osl_list.h>
#include <bsp_memmap.h>
#include <soc_cpufreq.h>
#include <bsp_icc.h>
#include <bsp_ipc.h>
#include <bsp_om.h>
#include <drv_dpm.h>
#include <bsp_cpufreq.h>


#define  cpufreq_err(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MUDU_CPUFREQ, "[cpufreq]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  cpufreq_info(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MUDU_CPUFREQ, "[cpufreq]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  cpufreq_debug(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MUDU_CPUFREQ, "[cpufreq]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))



#ifndef BSP_OK
#define BSP_OK              (0)
#endif

#ifndef BSP_ERROR
#define BSP_ERROR           (-1)
#endif

#ifndef BSP_TRUE
#define BSP_TRUE            (1)
#endif

#ifndef BSP_FALSE
#define BSP_FALSE           (0)
#endif

#ifndef BSP_NULL
#define BSP_NULL            (void*)0
#endif




/********************枚举 结构体 start**********************/
#ifndef __INCvxWindh
typedef int*        SEM_ID;
typedef void* 		MSG_Q_ID;
#ifndef __INCsemLibCommonh
#ifndef	_ASMLANGUAGE
typedef enum		    /* SEM_B_STATE */       // wangwei 09-10
    {
      SEM_EMPTY,			/* 0: semaphore not available */
      SEM_FULL			/* 1: semaphore available */
    } SEM_B_STATE;       // wangwei 09-10
#endif /*_ASMLANGUAGE*/
#endif /*__INCsemLibCommonh*/
#endif /*__INCvxWindh*/


typedef struct
{
    u32 usProfileUpLimit;
    u32 usProfileDownLimit;
} DFS_PROFILELIMIT_STRU;

typedef struct
{
    u32                ulDFSFunEnableFlag;
    u32                ulTimerLen;
    u32                usProfileUpTime;
    u32                usProfileDownTime;
    u32                ulProfileNum;
    DFS_PROFILELIMIT_STRU astThresHold[DC_RESV];
} DFS_CONFIGINFO_STRU;


typedef struct
{
    u32 enCurProfile;
    u32 ulCurSysUpTime;
    u32 ulCurSysDownTime;
    u32 ulStartTime;
    u32 aulVoteMap[DC_RESV];
} DFS_CONTROL_STRU;



struct cpufreq_frequency_table {
	unsigned int	index;     /* any */
	unsigned int	frequency; /* kHz - doesn't need to be in ascending
				    * order */
};

/***************************for v9r1 start***********************************/
#ifndef s32_t
#define s32_t   int
#endif
#ifndef u32_t
#define u32_t   unsigned int
#endif
#ifndef PWRCTRL_DFS_CMD_TIMEOUT
#define PWRCTRL_DFS_CMD_TIMEOUT    (50)
#endif
#ifndef MCA_UDATA_REQ_NAME_SIZE
#define MCA_UDATA_REQ_NAME_SIZE              (32)
#endif

#ifndef POLICY_LEN
#define POLICY_LEN (24)
#endif

#ifndef MAX_PROFILE_NUM
#define MAX_PROFILE_NUM (6)
#endif

#ifndef POLICY_MAX_TOTAL_SIZE
#define POLICY_MAX_TOTAL_SIZE (MAX_PROFILE_NUM * POLICY_LEN)
#endif

#ifndef BYTES_PER_UINT32
#define BYTES_PER_UINT32 (4)
#endif
#ifndef MCA_UDATA_REQ_SIZE
#define MCA_UDATA_REQ_SIZE              (512)
#endif
#ifndef MCA_UDATA_RSP_SIZE
#define MCA_UDATA_RSP_SIZE              (32)
#endif

#ifndef DDR_PROFILE_LEN
#define DDR_PROFILE_LEN (22)
#endif
#ifndef DDR_PROFILE_NUM
#define DDR_PROFILE_NUM (6)
#endif
#ifndef DDR_PROFILE_SIZE
#define DDR_PROFILE_SIZE (DDR_PROFILE_LEN * DDR_PROFILE_NUM * BYTES_PER_UINT32)
#endif

#ifndef PROFILE_MAX_TOTAL_SIZE
#define PROFILE_MAX_TOTAL_SIZE 			DDR_PROFILE_SIZE
#endif
#ifndef DFS_TASK_NAME_LENGTH
#define DFS_TASK_NAME_LENGTH       (16)
#endif

/*
 *  DFS Device QoS
 */
#define DFS_QOS_ID_ACPU_MINFREQ     (DFS_QOS_ID_ACPU_MINFREQ_E)
#define DFS_QOS_ID_ACPU_MAXFREQ     (DFS_QOS_ID_ACPU_MAXFREQ_E)
#define DFS_QOS_ID_CCPU_MINFREQ     (DFS_QOS_ID_CCPU_MINFREQ_E)
#define DFS_QOS_ID_CCPU_MAXFREQ     (DFS_QOS_ID_CCPU_MAXFREQ_E)
#define DFS_QOS_ID_DDR_MINFREQ      (DFS_QOS_ID_DDR_MINFREQ_E)
#define DFS_QOS_ID_DDR_MAXFREQ      (DFS_QOS_ID_DDR_MAXFREQ_E)
#define DFS_QOS_ID_GPU_MINFREQ      (DFS_QOS_ID_GPU_MINFREQ_E)
#define DFS_QOS_ID_GPU_MAXFREQ      (DFS_QOS_ID_GPU_MAXFREQ_E)
#define DFS_QOS_ID_BUS_MINFREQ      (DFS_QOS_ID_BUS_MINFREQ_E)
#define DFS_QOS_ID_BUS_MAXFREQ      (DFS_QOS_ID_BUS_MAXFREQ_E)

#define BALONG_DDRFREQUENCY_MAX			(6)
#define BALONG_DDRFREQUENCY_MIN				(0)

enum DIFF_MODE_VOTE{
	DIFF_MODE_OF_LT = 0,
	DIFF_MODE_OF_GU,
	END_OF_DIFF_MODE
};
#define CPUFREQ_ARGV_KHZ2MHZ						(1000)	
struct DFS_Qos_V7V9 {
	int request_id;   						/*请求的模块id号*/
	int request_flag;						/*请求是否有效1:有效*/
	int request_type;						/*请求的类型，DDR OR CPU?*/
	int request_profile;					/*请求对应值可能是profile或频率值*/
	struct list_head   entry;
};


union mca_udata_req
{
    char data[MCA_UDATA_REQ_SIZE];

    /* Declare MCA command udata structure. */

    struct {char name[MCA_UDATA_REQ_NAME_SIZE];} dfs_devid_req;

    struct {char name[MCA_UDATA_REQ_NAME_SIZE];} dfs_polid_req;

    struct {char name[MCA_UDATA_REQ_NAME_SIZE];} dfs_qosid_req;

    struct {s32_t dev_id;} dfs_enable_req;

    struct {s32_t dev_id;} dfs_disable_req;

    struct {s32_t dev_id;} dfs_state_req;

    struct {s32_t dev_id; u32_t prof_id;} dfs_lock_req;

    struct {s32_t dev_id;} dfs_unlock_req;

    struct {s32_t dev_id;} dfs_pfnum_req;

    struct {s32_t dev_id;} dfs_current_req;

    struct {s32_t dev_id; u32_t prof_id;} dfs_target_req;

    struct {s32_t policy_id; u32_t prof_id;} dfs_rpfpli_req;

    struct {s32_t policy_id; u32_t prof_id; s32_t pli_buf[POLICY_LEN / BYTES_PER_UINT32];} dfs_wpfpli_req;

	struct {s32_t policy_id; s32_t pli_buf[POLICY_MAX_TOTAL_SIZE / BYTES_PER_UINT32];} dfs_setpli_req;

	struct {s32_t qos_id; u32_t req_value;} dfs_rqqos_req;

    struct {s32_t qos_id; s32_t req_id;} dfs_rlqos_req;

    struct {s32_t qos_id; s32_t req_id; u32_t req_value;} dfs_udqos_req;

	struct {s32_t dev_id; u32_t prof_buf[PROFILE_MAX_TOTAL_SIZE / BYTES_PER_UINT32];} dfs_set_prof_req;
};


union mca_udata_rsp
{
    char data[MCA_UDATA_RSP_SIZE];

    /* Declare MCA command udata structure. */
    /*假命令用于表示返回失败的情况*/
    /*struct {char name[MCA_UDATA_REQ_SIZE];} dfs_devid_req;*/
    struct {s32_t ret;} dfs_default_rsp;

    struct {s32_t ret; s32_t id;} dfs_devid_rsp;

    struct {s32_t ret; s32_t id;} dfs_polid_rsp;

    struct {s32_t ret; s32_t id;} dfs_qosid_rsp;

    struct {s32_t ret;} dfs_enable_rsp;

    struct {s32_t ret;} dfs_disable_rsp;

    struct {s32_t ret; u32_t state;} dfs_state_rsp;

    struct {s32_t ret;} dfs_lock_rsp;

    struct {s32_t ret;} dfs_unlock_rsp;

    struct {s32_t ret; u32_t prof_num;} dfs_pfnum_rsp;

    struct {s32_t ret; u32_t prof_id;} dfs_current_rsp;

    struct {s32_t ret;} dfs_target_rsp;

    struct {s32_t ret; s32_t pli_buf[POLICY_LEN / BYTES_PER_UINT32];} dfs_rpfpli_rsp;

    struct {s32_t ret;} dfs_wpfpli_rsp;

    struct {s32_t ret;} dfs_setpli_rsp;
    struct {s32_t ret; s32_t req_id;} dfs_rqqos_rsp;

    struct {s32_t ret; s32_t req_id;} dfs_rlqos_rsp;

    struct {s32_t ret;} dfs_udqos_rsp;
    struct {s32_t ret;} dfs_set_prof_rsp;
};

typedef enum
{
    MCA_CMD_RSP = 0,            /* MCA Reserved for response.   */

    /* Declare MCA command ID. */
    MCA_CMD_DFS_ENABLE,         /* Enable DFS device.           */
    MCA_CMD_DFS_DISABLE,        /* Disnable DFS device.         */
    MCA_CMD_DFS_STATE,          /* Get DFS device state.        */
    MCA_CMD_DFS_LOCK,           /* Lock DFS profile.            */
    MCA_CMD_DFS_UNLOCK,         /* Unlock DFS profile.          */
    MCA_CMD_DFS_PFNUM,          /* Get DFS profile number.      */
    MCA_CMD_DFS_CURRENT,        /* Get current DFS profile.     */
    MCA_CMD_DFS_TARGET,         /* Set DFS profile.             */
    MCA_CMD_DFS_RPFPLI,         /* Get DFS policy profile.      */
    MCA_CMD_DFS_WPFPLI,         /* Write DFS policy profile.    */
    MCA_CMD_DFS_SETPLI,         /* Set All DFS policy profile.    */
    MCA_CMD_DFS_RQQOS,          /* DFS QoS request.             */
    MCA_CMD_DFS_RLQOS,          /* DFS QoS release.             */
    MCA_CMD_DFS_UDQOS,          /* DFS QoS update.              */
    MCA_CMD_DFS_SETPROFILE,         /* Set All DFS profile.    */
    MCA_CMD_END                 /* NERVER REACH.                */
} MCA_CMD_ID;
/***************************for v9r1 end***********************************/

/********************枚举 结构体 end**********************/

#define  DFS_TRUE (1L)

#ifndef SEM_Q_PRIORITY
#define SEM_Q_PRIORITY  (0)
#endif

#ifndef SEM_EMPTY
#define SEM_EMPTY 0
#endif
#define GET_PROFILE_CB_TIMEOUT 				(100)

#ifndef CPUFREQ_ENTRY_INVALID
#define CPUFREQ_ENTRY_INVALID (~0)
#endif

#ifndef CPUFREQ_TABLE_END
#define CPUFREQ_TABLE_END     (~1)
#endif
#define  DFS_NO_WAIT (0)
#define  DFS_WAIT_FOREVER (-1)


void cpufreq_lock_mcore_freq(bool lock_flag);
int cpufreq_v9r1_get_ccore_profile(int request_freq);
int cpufreq_v9r1_get_ddr_profile(int request_freq);
int cpufreq_v9r1_get_bus_profile(int request_freq);
int cpufreq_v9r1_get_profile(int request_id, int request_freq);
void calccpu_flag(int flag, int flag1);
void test_for_adjust_cpuload(int busytime, int idletime);
void test_for_pmuevent(void);
void test_for_cpufreq_longtime(void);
void test_for_pmu_task(void);
//void cpufreq_print_debug(void);
void test_for_v9r1_interface(int inter_id, int request_id, int request_freq,int req_id);
void test_send_msg(unsigned int msg_type, unsigned int source, unsigned int content, unsigned int profile);
void cpufreq_exc_change_limit(u32 sample, u32 up_cpuloadlimit, u32 down_cpuloadlimit, u32 down_times, u32 up_times);
/****************************宏*end*********************************/

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif
