

#ifndef __BSP_CPUFREQ_H__
#define __BSP_CPUFREQ_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <osl_types.h>
#include <drv_cpufreq.h>
#include "bsp_sram.h"


#ifndef MAX_WAIT_TIMEOUT
#define  MAX_WAIT_TIMEOUT         (0xa)
#endif

#define CPUFREQ_CUR_PROFILE						(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_CUR_CPUFREQ_PROFILE)
#define CPUFREQ_MAX_PROFILE_LIMIT			(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_MAX_CPUFREQ_PROFILE)
#define CPUFREQ_MIN_PROFILE_LIMIT			(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_MIN_CPUFREQ_PROFILE)
#define CPUFREQ_DOWN_FLAG(core_type)   					(unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_CPUFREQ_DOWN_FLAG[core_type])
#define CPUFREQ_DOWN_PROFILE(core_type)  				 (unsigned int)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->SRAM_CPUFREQ_DOWN_PROFILE[core_type])

#define QUERY_PROFILE_TABLE_INIT(PROFILE, CPU_FREQ, DDR_FREQ, SBUS_FREQ, USING_FLAG) \
				[PROFILE] = {     \
							.cpu_frequency = CPU_FREQ,     \
							.ddr_frequency = DDR_FREQ,     \
							.sbus_frequency = SBUS_FREQ, \
							.profile = PROFILE,                    \
							.using_flag = USING_FLAG, }
		


enum CPUFREQ_MSG_TYPE{
	CPUFREQ_ADJUST_FREQ = 1,			/*通知M3调频*/
	CPUFREQ_DEBUG_FREQ = 2,			/*用于DEBUG  标记*/
	CPUFREQ_GET_FREQ_FROM_M = 3,		/*获取当前频率*/
	CPUFREQ_SEND_CPUFREQ_FROM_M = 4,			/*M3回复当前频率*/
	CPUFREQ_LOCK_MCORE_ACTION = 5,			/*锁定M3调频*/
    CPUFREQ_SET_DDR_LIMIT = 6,
};
/*哪个核发的消息*/
enum CPUFREQ_SOURCE{
	CPUFREQ_ACORE = 0,
	CPUFREQ_CCORE,
	CPUFREQ_MCORE
};

/*content 低4BIT 上调下调 高4BIT 哪个profile*/
struct cpufreq_msg {
	u32 msg_type;
	u32 source;
	u32 content;
	u32 profile;
};

typedef enum
{
    DFS_PROFILE_UP   = 0,
    DFS_PROFILE_UP_TARGET = 1,
    DFS_PROFILE_DOWN = 2,
    DFS_PROFILE_DOWN_TARGET = 3,
    DFS_PROFILE_DOWN_LIMIT = 4,
    DFS_PROFILE_UP_LIMIT = 5,
    DFS_PROFILE_LOCKFREQ = 0,
    DFS_PROFILE_UNLOCKFREQ = 1,
	DFS_PROFILE_NOCHANGE = 10,
} DFS_PROFILE_STATE_ENUM;


#define BALONG_CPUFREQUENCY_666 				(666)
#define BALONG_CPUFREQUENCY_640				(640)
#define BALONG_CPUFREQUENCY_610 				(610)
#define BALONG_CPUFREQUENCY_600 				(600)
#define BALONG_CPUFREQUENCY_580 				(580)
#define BALONG_CPUFREQUENCY_550 				(550)
#define BALONG_CPUFREQUENCY_520 				(520)
#define BALONG_CPUFREQUENCY_490 				(490)
#define BALONG_CPUFREQUENCY_460 				(460)
#define BALONG_CPUFREQUENCY_430 				(430)
#define BALONG_CPUFREQUENCY_400 				(400)
#define BALONG_CPUFREQUENCY_370 				(370)
#define BALONG_CPUFREQUENCY_333 				(333)
#define BALONG_CPUFREQUENCY_300				(300)
#define BALONG_CPUFREQUENCY_260 				(260)
#define BALONG_CPUFREQUENCY_222 				(222)
#define BALONG_CPUFREQUENCY_200 				(200)
#define BALONG_CPUFREQUENCY_166 				(166)
#define BALONG_CPUFREQUENCY_111 				(111)
#define BALONG_CPUFREQUENCY_100 				(100)

#define BALONG_CPUFREQ_MHZ							(1000000)

#define BALONG_DDRFREQUENCY_666				(666)
#define BALONG_DDRFREQUENCY_600				(600)
#define BALONG_DDRFREQUENCY_400				(400)
#define BALONG_DDRFREQUENCY_333				(333)
#define BALONG_DDRFREQUENCY_300				(300)
#define BALONG_DDRFREQUENCY_222				(222)
#define BALONG_DDRFREQUENCY_200				(200)
#define BALONG_DDRFREQUENCY_166				(166)
#define BALONG_DDRFREQUENCY_100				(100)

#define BALONG_SBUSFREQUENCY_200			(200)
#define BALONG_SBUSFREQUENCY_166			(166)
#define BALONG_SBUSFREQUENCY_150			(150)
#define BALONG_SBUSFREQUENCY_133			(133)
#define BALONG_SBUSFREQUENCY_111			(111)
#define BALONG_SBUSFREQUENCY_100			(100)
#define BALONG_SBUSFREQUENCY_83				(83)
#define BALONG_SBUSFREQUENCY_55				(55)
#define BALONG_SBUSFREQUENCY_50				(50)

/*CPU FREQUENCE HZ*/



#define CPU_FREQ_FREQ_NUM 4

struct cpufreq_query {
	u32 cpu_frequency;
	u32 ddr_frequency;
	u32 sbus_frequency;
	u32 profile;
	u32 using_flag;
};


#ifdef __VXWORKS__
void cpufreq_init(void);
void cpufreq_set_max_freq(void);
#endif


#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __BSP_CPUFREQ_H__ */

