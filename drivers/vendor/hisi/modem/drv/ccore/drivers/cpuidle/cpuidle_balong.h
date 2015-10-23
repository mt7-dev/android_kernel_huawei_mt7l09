
#ifndef __CPUIDLE_BALONG_H__
#define __CPUIDLE_BALONG_H__
#ifdef __cplusplus
extern "C" {
#endif
void cpuidle_set_deepsleep_ops(int (*go_deepsleep)(void));
void cpuidle_idle_management (void);
extern void cpu_enter_idle(void);

#ifdef __cplusplus
}
#endif

#endif



