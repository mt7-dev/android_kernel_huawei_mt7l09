
#ifndef __REGULATOR_BALONG_H__
#define __REGULATOR_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <hi_pwrctrl_interface.h>
#include <bsp_memmap.h>
#include <bsp_om.h>
#include <bsp_regulator.h>

//32
#define  regu_pr_err(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_REGULATOR, "[regulator]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  regu_pr_info(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_REGULATOR, "[regulator]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  regu_pr_debug(fmt,...)		(bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_REGULATOR, "[regulator]: <%s> <%d>"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))

struct regulator_id_ops {
	int (*list_voltage) (int regulator_id, unsigned selector);
	int (*set_voltage) (int regulator_id, int min_uV, int max_uV, unsigned *selector);
	int (*get_voltage) (int regulator_id);
	int (*is_enabled) (int regulator_id);
	int (*enable) (int regulator_id);
	int (*disable) (int regulator_id);
	int (*set_mode) (int regulator_id, int mode);
	unsigned int (*get_mode) (int regulator_id);
	int (*set_current) (int regulator_id, int min_uA, int max_uA, unsigned *selector);
	int (*get_current) (int regulator_id);
};
struct regulators_type {
	int base_num;
	int regulator_type;
	struct regulator_id_ops *ops;
};

struct regulator_mtcmos_ops {
	void (*mtcmos_enable) (void);
	void (*mtcmos_disable) (void);
	unsigned int  (*mtcmos_is_enable)(void);
};

#ifdef __cplusplus
}
#endif

#endif/*__REGULATOR_BALONG_H__*/