#include <intLib.h>
#include <logLib.h>
#include <vxWorks.h>
#include <msgQLib.h>

#include <bsp_hardtimer.h>
#include <osl_thread.h>
//#include <osl_barrier.h>


#define isb() __asm__ __volatile__ ("isb" : : : "memory")

#define ARMV7_PMNC_E		(1 << 0) /* Enable all counters */
#define	ARMV7_PMNC_MASK		0x3f	 /* Mask for writable bits */
#define	ARMV7_IDX_CYCLE_COUNTER	0
#define	ARMV7_IDX_COUNTER0	1
#define	ARMV7_EVTYPE_MASK	0xc00000ff	/* Mask for writable bits */
#define	ARMV7_EVTYPE_EVENT	0xff		/* Mask for EVENT bits */
#define	ARMV7_MAX_COUNTERS	32
#define	ARMV7_COUNTER_MASK	(ARMV7_MAX_COUNTERS - 1)
#define	ARMV7_PMNC_N_SHIFT	11	 /* Number of counters supported */
#define	ARMV7_PMNC_N_MASK	0x1f
#define	ARMV7_IDX_COUNTER_LAST	(ARMV7_IDX_CYCLE_COUNTER + cpu_pmu_num_events - 1)
#define ARMV7_PMNC_P		(1 << 1) /* Reset all counters */
#define ARMV7_PMNC_C		(1 << 2) /* Cycle counter reset */
#define	ARMV7_IDX_TO_COUNTER(x)	\
	(((x) - ARMV7_IDX_COUNTER0) & ARMV7_COUNTER_MASK)
#define	ARMV7_FLAG_MASK		0xffffffff	/* Mask for writable bits */
#define	ARMV7_OVERFLOWED_MASK	ARMV7_FLAG_MASK
#define BIT(nr)			(1UL << (nr))



#ifndef EINVAL
#define EINVAL   1
#endif



/**
 * struct hw_perf_event - performance event hardware details:
 */
struct hw_perf_event {
		unsigned long	config_base;
		unsigned long	event_base;
		int		idx;
		int		last_cpu;
};
int cpu_pmu_num_events = 0;

int armv7_pmnc_counter_valid(int idx)
{
	return idx >= ARMV7_IDX_CYCLE_COUNTER && idx <= ARMV7_IDX_COUNTER_LAST;
}



u32 armv7_pmnc_read(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	return val;
}
void armv7_pmnc_write(u32 val)
{
	val &= ARMV7_PMNC_MASK;
	isb();
	asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}
int armv7_pmnc_disable_counter(int idx)
{
	u32 counter;

	if (!armv7_pmnc_counter_valid(idx)) {
		printf("CPU  disabling wrong PMNC counter \n");
		return -EINVAL;
	}

	counter = ARMV7_IDX_TO_COUNTER(idx);
	asm volatile("mcr p15, 0, %0, c9, c12, 2" : : "r" (BIT(counter)));
	return idx;
}
 u32  armv7_read_num_pmnc_events(void)
{
	u32 nb_cnt;

	/* Read the nb of CNTx counters supported from PMNC */
	nb_cnt = (armv7_pmnc_read() >> ARMV7_PMNC_N_SHIFT) & ARMV7_PMNC_N_MASK;

	/* Add the CPU cycles counter and return */
	return nb_cnt + 1;
}
int armv7_pmnc_select_counter(int idx)
{
	u32 counter;

	if (!armv7_pmnc_counter_valid(idx)) {
		printf("CPU enabling wrong PMNC counter IRQ enable \n");
		return -EINVAL;
	}

	counter = ARMV7_IDX_TO_COUNTER(idx);
	asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r" (counter));
	isb();

	return idx;
}
void armv7_pmnc_write_evtsel(int idx, u32 val)
{
	if (armv7_pmnc_select_counter(idx) == idx) {
		val &= ARMV7_EVTYPE_MASK;
		asm volatile("mcr p15, 0, %0, c9, c13, 1" : : "r" (val));
	}
}

int armv7_pmnc_enable_intens(int idx)
{
	u32 counter;

	if (!armv7_pmnc_counter_valid(idx)) {
		printf("CPU enabling wrong PMNC counter IRQ enable \n");
		return -EINVAL;
	}

	counter = ARMV7_IDX_TO_COUNTER(idx);
	asm volatile("mcr p15, 0, %0, c9, c14, 1" : : "r" (BIT(counter)));
	return idx;
}
int armv7_pmnc_enable_counter(int idx)
{
	u32 counter;

	if (!armv7_pmnc_counter_valid(idx)) {
		printf("CPU enabling wrong PMNC counter IRQ enable \n");
		return -EINVAL;
	}

	counter = ARMV7_IDX_TO_COUNTER(idx);
	asm volatile("mcr p15, 0, %0, c9, c12, 1" : : "r" (BIT(counter)));
	return idx;
}

void armv7pmu_enable_event(struct hw_perf_event *hwc, int idx)
{
	/*
	 * Disable counter
	 */
	armv7_pmnc_disable_counter(idx);

	/*
	 * Set event (if destined for PMNx counters)
	 * We only need to set the event for the cycle counter if we
	 * have the ability to perform event filtering.
	 */
	if (idx != ARMV7_IDX_CYCLE_COUNTER)
		armv7_pmnc_write_evtsel(idx, hwc->config_base);

	/*
	 * Enable interrupt for this counter
	 */
	armv7_pmnc_enable_intens(idx);

	/*
	 * Enable counter
	 */
	armv7_pmnc_enable_counter(idx);


}

void armv7pmu_start(void)
{
	/* Enable all counters */
	armv7_pmnc_write(armv7_pmnc_read() | ARMV7_PMNC_E);
}

void armv7_pmnc_dump_regs(void)
{
	u32 val;
	unsigned int cnt;

	printf("PMNC registers dump:\n");

	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r" (val));
	printf("PMNC  =0x%08x\n", val);

	asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r" (val));
	printf("CNTENS=0x%08x\n", val);

	asm volatile("mrc p15, 0, %0, c9, c14, 1" : "=r" (val));
	printf("INTENS=0x%08x\n", val);

	asm volatile("mrc p15, 0, %0, c9, c12, 3" : "=r" (val));
	printf("FLAGS =0x%08x\n", val);

	asm volatile("mrc p15, 0, %0, c9, c12, 5" : "=r" (val));
	printf("SELECT=0x%08x\n", val);

	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r" (val));
	printf("CCNT  =0x%08x\n", val);

	for (cnt = (unsigned int)ARMV7_IDX_COUNTER0; cnt <= (unsigned int)ARMV7_IDX_COUNTER_LAST; cnt++) {
		armv7_pmnc_select_counter(cnt);
		asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r" (val));
		printf("CNT[%d] count =0x%08x\n",
			ARMV7_IDX_TO_COUNTER(cnt), val);
		asm volatile("mrc p15, 0, %0, c9, c13, 1" : "=r" (val));
		printf("CNT[%d] evtsel=0x%08x\n",
			ARMV7_IDX_TO_COUNTER(cnt), val);
	}
}
int armv7_pmnc_disable_intens(int idx)
{
	u32 counter;

	if (!armv7_pmnc_counter_valid(idx)) {
		printf("CPU disabling wrong PMNC counter IRQ enable \n");
		return -EINVAL;
	}

	counter = ARMV7_IDX_TO_COUNTER(idx);
	asm volatile("mcr p15, 0, %0, c9, c14, 2" : : "r" (BIT(counter)));
	isb();
	/* Clear the overflow flag in case an interrupt is pending. */
	asm volatile("mcr p15, 0, %0, c9, c12, 3" : : "r" (BIT(counter)));
	isb();

	return idx;
}
u32 armv7_pmnc_getreset_flags(void)
{
	u32 val;

	/* Read */
	asm volatile("mrc p15, 0, %0, c9, c12, 3" : "=r" (val));

	/* Write to clear flags */
	val &= ARMV7_FLAG_MASK;
	asm volatile("mcr p15, 0, %0, c9, c12, 3" : : "r" (val));

	return val;
}
int armv7_pmnc_has_overflowed(u32 pmnc)
{
	return pmnc & ARMV7_OVERFLOWED_MASK;
}
static void armv7pmu_disable_event(struct hw_perf_event *hwc, int idx)
{
	/*
	 * Disable counter
	 */
	armv7_pmnc_disable_counter(idx);

	/*
	 * Disable interrupt for this counter
	 */
	armv7_pmnc_disable_intens(idx);

}
void armv7pmu_init()
{
	cpu_pmu_num_events = armv7_read_num_pmnc_events();
}
void armv7pmu_reset()
{
	u32 idx, nb_cnt = cpu_pmu_num_events;

	/* The counter and interrupt enable registers are unknown at reset. */
	for (idx = ARMV7_IDX_CYCLE_COUNTER; idx < nb_cnt; ++idx)
		armv7pmu_disable_event((void *)0, idx);

	/* Initialize & Reset PMNC: C and P bits */
	armv7_pmnc_write(ARMV7_PMNC_P | ARMV7_PMNC_C);
}

int enable_event(long config_base, int idx)
{
	struct hw_perf_event hwc;
	hwc.config_base = config_base;
	armv7pmu_enable_event(&hwc, idx);
	return 0;
}
#define EVENT_COUNT_OVERFLOW    (0xFFFFFFFF)

int g_aver_count = 1;
int idle_time_pmu = 500;
void set_averg_count(int aver_count, int idletime_10ms)
{
	g_aver_count = aver_count;
	idle_time_pmu = idletime_10ms;
}

u32 last_event[10] = {0,0,0,0,0,0,0,0,0,0};
u32 event[10] = {0,0,0,0,0,0,0,0,0,0};
u32 event_flow[10] = {0,0,0,0,0,0,0,0,0,0};

void pmu_analysis_for_overflow(u32 pmnc)
{
	if (pmnc & 0x1)
	{
		event_flow[1] +=  1;
	}
	if (pmnc & 0x2)
	{
		event_flow[2] +=  1;
	}
	if (pmnc & 0x4)
	{
		event_flow[3] +=  1;
	}
	if (pmnc & 0x8)
	{
		event_flow[4] +=  1;
	}
	if (pmnc & 0x10)
	{
		event_flow[5] +=  1;
	}
	if (pmnc & 0x20)
	{
		event_flow[6] +=  1;
	}
}
void init_array(u32 *arr)
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		arr[i] = 0;
	}
}
int g_count_enable_flag = 0;
void pmu_analysis_dump_info(void)
{
	int cnt = 0;
	u32 val;
	u32 pmnc;
	g_count_enable_flag = 0;
	pmnc = armv7_pmnc_getreset_flags();
	if (armv7_pmnc_has_overflowed(pmnc))
	{
		pmu_analysis_for_overflow(pmnc);
	}

	for (cnt = ARMV7_IDX_COUNTER0; cnt <= ARMV7_IDX_COUNTER_LAST; cnt++) {
		armv7_pmnc_select_counter(cnt);
		printf("CNT[%d] ",ARMV7_IDX_TO_COUNTER(cnt));
		printf(" aver_count =%u", event[cnt]);
		printf(" flow_count =%d\n", event_flow[cnt]);
		asm volatile("mrc p15, 0, %0, c9, c13, 1" : "=r" (val));
		printf("CNT[%d] evtsel=0x%08x\n",
			ARMV7_IDX_TO_COUNTER(cnt), val);
	}
}


void pmu_start_count(void)
{
	int cnt = 0;
	u32 val;
	if (0 == g_count_enable_flag)
	{
		return;
	}
	init_array(last_event);
	for (cnt = ARMV7_IDX_COUNTER0; cnt <= ARMV7_IDX_COUNTER_LAST; cnt++)
	{
			armv7_pmnc_select_counter(cnt);
			asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r" (val));
			last_event[cnt] = val;
	}
}

void pmu_calc_counter(void)
{
	int cnt = 0;
	u32 val;
	u32 pmnc;
	if (0 == g_count_enable_flag)
	{
		return;
	}

	pmnc = armv7_pmnc_getreset_flags();
	if (armv7_pmnc_has_overflowed(pmnc))
	{
		pmu_analysis_for_overflow(pmnc);
	}
	for (cnt = ARMV7_IDX_COUNTER0; cnt <= ARMV7_IDX_COUNTER_LAST; cnt++) {
		armv7_pmnc_select_counter(cnt);
		asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r" (val));
		//printf(">>>val:%d, lastevent:%d\n", val, last_event[cnt]);
		event[cnt] += ((val >= last_event[cnt]) ? (val - last_event[cnt]) : ((EVENT_COUNT_OVERFLOW - last_event[cnt]) + val));
	}
}

void pmu_start_counter(int event1, int event2, int event3, int event4, int event5, int event6)
{
	unsigned long flags = 0;
	local_irq_save(flags);
	armv7pmu_init();
	armv7pmu_reset();
	init_array(last_event);
	init_array(event);
	init_array(event_flow);
	enable_event(event1, 1);
	enable_event(event2, 2);
	enable_event(event3, 3);
	enable_event(event4, 4);
	enable_event(event5, 5);
	enable_event(event6, 6);
	armv7pmu_start();
	pmu_start_count();
	
	g_count_enable_flag = 1;
	local_irq_restore(flags);
}
extern void perf_analysis_stop_test_task(void);
void pmu_analysis_task()
{
	//int cnt = 0;
	//u32 val;
	if (0 ==g_aver_count)
	{
		return;
	}
	printf("pmu counter start\n");
	pmu_start_count();
#if 0
	for (cnt = ARMV7_IDX_COUNTER0; cnt <= ARMV7_IDX_COUNTER_LAST; cnt++)
	{
			armv7_pmnc_select_counter(cnt);
			asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r" (val));
			last_event[cnt] = val;
	}
#endif

	taskDelay(idle_time_pmu);
	pmu_calc_counter();
#if 0
	/*输出统计周期内的计数*/
	for (cnt = ARMV7_IDX_COUNTER0; cnt <= ARMV7_IDX_COUNTER_LAST; cnt++) {
		armv7_pmnc_select_counter(cnt);
		asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r" (val));
		event[cnt] += ((val > last_event[cnt]) ? (val - last_event[cnt]) : ((EVENT_COUNT_OVERFLOW - last_event[cnt]) + val));
	}
#endif
	pmu_analysis_dump_info();
	perf_analysis_stop_test_task();
}


int g_pmu_event1 = 138;
int g_pmu_event2 = 139;

void pmu_set_config(int event_num, int pmu_event1, int pmu_event2, int check_time)
{
	armv7pmu_init();
	armv7pmu_reset();
	switch (event_num)
	{
		case 0:
			enable_event( 3, 1);
			enable_event( 4, 2);
			enable_event( 6, 3);
			enable_event( 7, 4);
			enable_event( 12, 5);
			enable_event( 13, 6);

			break;
		case 1:
			enable_event(102 , 1);
			enable_event( 103, 2);
			enable_event( 112, 3);
			enable_event( 113, 4);
			enable_event( 114, 5);
			enable_event( 138, 6);
			break;
		case 2:
	 		g_pmu_event1 = pmu_event1;
	 		g_pmu_event2 = pmu_event2;
			enable_event( g_pmu_event1, 1);
			enable_event( g_pmu_event2, 2);
			break;
		default:
			printf("error\n");
			break;
	}
	idle_time_pmu = check_time;
}

void pmu_analysis_start(void)
{
	init_array(last_event);
	init_array(event);
	init_array(event_flow);
	armv7pmu_start();
	taskSpawn("calc_pmu_test", 100, 0, 4096, (FUNCPTR)pmu_analysis_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
