
#include <symbol.h>
/*lint --e{18, 532, 546} */
extern int help();
extern int d();
extern int m();
extern int system_error();
extern void pm_set_debug(unsigned int type, unsigned int flag);
extern int pm_print_debug();
extern void bsp_wdt_print_debug(void);
extern int bsp_wdt_stop(void);
extern int pm_wakeup_acore();
extern int pm_wakeup_ccore();
extern void pm_appa9_wsrc_set(unsigned int lvl);
extern void pm_mdma9_wsrc_set(unsigned int lvl);
extern void pm_mcu_wsrc_set(unsigned int lvl);
extern void pm_appa9_wsrc_unset(unsigned int lvl);
extern void pm_mdma9_wsrc_unset(unsigned int lvl);
extern void pm_mcu_wsrc_unset(unsigned int lvl);
extern void cpufreq_print_debug();
extern void dump_wdt_hook(signed int core);
extern void ios_list_echo(void);
extern void bsp_dump_bus_error_status(void);
struct symbol kallsyms[] = {
	{"help", (unsigned long)&help},
	{"d", (unsigned long)&d},
	{"m", (unsigned long)&m},
	{"system_error", (unsigned long)&system_error},
	{"pm_set_debug", (unsigned long)&pm_set_debug},
	{"pm_print_debug", (unsigned long)&pm_print_debug},
	{"pm_wakeup_acore", (unsigned long)&pm_wakeup_acore},
	{"pm_wakeup_ccore", (unsigned long)&pm_wakeup_ccore},
	{"pm_appa9_wsrc_set", (unsigned long)&pm_appa9_wsrc_set},
	{"pm_mdma9_wsrc_set", (unsigned long)&pm_mdma9_wsrc_set},
	{"pm_mcu_wsrc_set", (unsigned long)&pm_mcu_wsrc_set},
	{"pm_appa9_wsrc_unset", (unsigned long)&pm_appa9_wsrc_unset},
	{"pm_mdma9_wsrc_unset", (unsigned long)&pm_mdma9_wsrc_unset},
	{"pm_mcu_wsrc_unset", (unsigned long)&pm_mcu_wsrc_unset},
	{"cpufreq_print_debug", (unsigned long)&cpufreq_print_debug},
    {"dump_wdt_hook", (unsigned long)&dump_wdt_hook},
    {"ios_list_echo", (unsigned long)&ios_list_echo},
    {"bsp_wdt_print_debug", (unsigned long)&bsp_wdt_print_debug},
    {"bsp_wdt_stop", (unsigned long)&bsp_wdt_stop},    
    {"bsp_dump_bus_error_status", (unsigned long)&bsp_dump_bus_error_status},
};

unsigned kallsyms_count = sizeof(kallsyms)/sizeof(kallsyms[0]);
