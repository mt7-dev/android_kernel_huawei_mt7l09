

#ifndef __PMU_H__
#define __PMU_H__

#include <balongv7r2/types.h>
#include <boot/boot.h>
#include <bsp_pmu.h>

#define  pmic_print_error(fmt, ...)    (cprintf("[pmu]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#define  pmic_print_info(fmt, ...) \
do {                               \
       /* pmic_print_error(fmt, ##__VA_ARGS__);*/\
} while (0)

/*函数声明, 放在bsp_pmu.h中*/
#endif