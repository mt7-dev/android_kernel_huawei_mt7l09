


#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <vxWorks.h>
#include <intLib.h>
#include <errnoLib.h>
#include <sioLib.h>

#include "regulator_balong.h"
#include "osl_types.h"
#include "osl_spinlock.h"

struct regulator *regulator_get_by_id(u32 id);
void regulator_print_status(int id, int type);
void regulator_status(int id, int type);
void regulator_get_current_status(void);
int  regulator_is_enabled_byid(int id, int type);
int regulator_enable_byid(int id, int type);
int regulator_disable_byid(int id, int type);
int regulator_get_voltage_byid(int id, int type);
int regulator_set_voltage_byid(int id, int min_uV, int max_uV, int type);




extern struct regulator regulators[];


struct regulator *regulator_get_by_id(u32 id)
{
	u32 i = 0;
	for (i = 0; i <= NUM_OF_REGUALTORS; i++)
	{
		if ((u32)regulators[i].id == id)
		{
			regu_pr_debug("regulators_id:%d****id---->%d,name--->%s,Total--->%d\n", regulators[i].id, id, regulators[i].name, NUM_OF_REGUALTORS);
			return &regulators[i];
		}
	}

	regu_pr_info(">>>>>>>get regulator failed ----->id:%d <<<<<<<<\n", id);
	return NULL;
}

void regulator_print_status(int id, int type)
{
	struct regulator *regulator = NULL;
	int ret = 0;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)
	{
		regu_pr_err("get regulator failed---->id is :%d\n", id);
		return;
	}
	printf("******************regulator id %d's status*******************\n\r", id);
	printf("regulator id------------>%d\n", (regulator->id & REGULATOR_TYPE_FLAG));
	printf("regulator name---------->%s\n", regulator->name);
	ret = regulator_is_enabled(regulator);
	printf("regulator status-------->%d\n", ret);
	printf("regulator use_count----->%d\n", regulator->use_count);
	ret = regulator_get_voltage(regulator);
	printf("regulator voltage------->%d\n", ret);
	printf("******************regulator status end***********************\n\r");
}

void regulator_status(int id, int type)
{
	struct regulator *regulator = NULL;
	int status = 0, voltage = 0;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)
	{
		regu_pr_info("get regulator failed---->id is :%d\n", id);
		return;
	}
	status = regulator_is_enabled(regulator);
	if (type != REGULATOR_TYPE_P531_MTCMOS && type != REGULATOR_TYPE_V7R2_MTCMOS)
	{
		voltage = regulator_get_voltage(regulator);
	}
	else
	{
		voltage = -1;
	}

	printf("id\t name\t\t use_count\t status\t voltage\t\n");
	printf("%-d\t %-s\t %-d\t\t %-d\t %-d\t\n", regulator->id, regulator->name, regulator->use_count,
											status, voltage);
}
void regulator_get_current_status(void)
{
	int id = 0;
	regu_pr_err("******************regulator status*******************\n", id);
	for(id = HI6451_BUCK1; id <= HI6451_DR3; id++)
	{
		regulator_status(id, REGULATOR_TYPE_HI6451);
	}
	for(id = HI6551_BUCK1; id <= HI6551_LVS10; id++)
	{
		regulator_status(id, REGULATOR_TYPE_HI6551);
	}
	for(id = P531_MTCMOS1; id <= P531_MTCMOS10; id++)
	{
		regulator_status(id, REGULATOR_TYPE_P531_MTCMOS);
	}
	for(id = V7R2_MTCMOS1; id <= V7R2_MTCMOS13; id++)
	{
		regulator_status(id, REGULATOR_TYPE_V7R2_MTCMOS);
	}
}


/*
	返回为0 ----未使能
	返回大于0-----已使能
*/
int  regulator_is_enabled_byid(int id, int type)
{
	struct regulator *regulator = NULL;
	int ret = -EINVAL;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)
	{
		regu_pr_err("get regulator failed---->id is :%d\n", id);
		ret = -EINVAL;
		goto out;
	}
	ret = regulator_is_enabled(regulator);

	regu_pr_debug("ret-->%d, regu_id--->%d, name--->%s\n", ret, regulator->id, regulator->name);

out:

	return ret;
}
/*
	返回0 ------使能成功
	返回非0-----使能失败
*/
int regulator_enable_byid(int id, int type)
{
	struct regulator *regulator = NULL;
	int ret = -EINVAL;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)
	{
		regu_pr_err("get regulator failed---->id is :%d\n", id);
		ret = -EINVAL;
		goto out;
	}
	ret = regulator_enable(regulator);

	regu_pr_debug("ret-->%d, regu_id--->%d, name--->%s\n", ret, regulator->id, regulator->name);

out:
	return ret;
}

int regulator_disable_byid(int id, int type)
{
	struct regulator *regulator = NULL;
	int ret = -EINVAL;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)
	{
		regu_pr_err("get regulator failed---->id is :%d\n", id);
		ret = -EINVAL;
		goto out;
	}
	ret = regulator_disable(regulator);

	regu_pr_debug("ret-->%d, regu_id--->%d, name--->%s\n", ret, regulator->id, regulator->name);
out:

	return ret;
}

/*
	获取当前电压
*/
int regulator_get_voltage_byid(int id, int type)
{
	struct regulator *regulator = NULL;
	int ret = -EINVAL;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)	{
		regu_pr_err("get regulator failed---->id is :%d\n", id);
		ret = -EINVAL;
		goto out;
	}
	ret = regulator_get_voltage(regulator);


	regu_pr_debug("ret-->%d, regu_id--->%d, name--->%s\n", ret, regulator->id, regulator->name);
out:

	return ret;
}

int regulator_set_voltage_byid(int id, int min_uV, int max_uV, int type)
{
	struct regulator *regulator = NULL;
	int ret = -EINVAL;
	id = (type << REGULATOR_SHIFT) | id;
	regulator = regulator_get_by_id(id);
	if (NULL == regulator)
	{
		regu_pr_err("get regulator failed---->id is :%d\n", id);
		ret = -EINVAL;
		goto out;
	}
	if ((min_uV > max_uV) || (min_uV < 0) || max_uV < 0)
	{
		regu_pr_err("regulator uV is error---->id is :%d,---->min_uV is : %d,---->max_uV is : %d\n", id, min_uV, max_uV);
		ret = -EINVAL;
		goto out;
	}
	ret = regulator_set_voltage(regulator, min_uV, max_uV);
	if (ret < 0)
	{
		regu_pr_err("set regulator voltage failed---->id is :%d\n", id);
	}
	regu_pr_debug("ret-->%d, regu_id--->%d, name--->%s\n", ret, regulator->id, regulator->name);

out:

	return ret;
}

#ifdef __cplusplus
}
#endif
