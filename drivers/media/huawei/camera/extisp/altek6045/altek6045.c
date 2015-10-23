/* 
 *  Hisilicon K3 SOC camera driver source file 
 * 
 *  Copyright (C) Huawei Technology Co., Ltd. 
 * 
 * Author:	  h00145353 
 * Email:	  alan.hefeng@huawei.com
 * Date:	  2013-12-05
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */


#include <linux/compiler.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/videodev2.h>
#include <media/huawei/camera.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-subdev.h>
#include <media/videobuf2-core.h>

#include "hwcam_intf.h"
#include "../hwextisp.h"
#include "hw_pmic.h"
#include "extisp.h"
#include "cam_log.h"

typedef enum {
	ISP_DVDD = 0,
	ISP_RST,
	ISP_INT,
	ISP_MAX,
}altek6045_pin_type;

enum mini_isp_power_state_t{
	MINI_ISP_POWER_OFF = 0,
	MINI_ISP_POWER_ON,
};

typedef struct _altek6045_private_data_t {
	unsigned int pin[ISP_MAX];
}altek6045_private_data_t;



enum {
	ALTEK6045_PIPE_0 = 0,
	ALTEK6045_PIPE_1 = 1,
	ALTEK6045_PIPE_DUAL = 2,
	ALTEK6045_PIPE_MAX = 3,
};

enum altek6045_pipe_test_stage {
	ALTEK6045_PIPE_UNTESTED = 0,
	ALTEK6045_PIPE_TESTING = 1,
	ALTEK6045_PIPE_TEST_CMD_ERR = 2,
	ALTEK6045_PIPE_GET_CMD_ERR = 3,
	ALTEK6045_PIPE_TEST_BAD = 4,
	ALTEK6045_PIPE_TEST_DONE = 5,
	ALTEK6045_PIPE_TEST_MAX = 6,
};


typedef struct _tag_altek6045
{
    char 				name[32];
    hwextisp_intf_t                             intf; 
    altek6045_private_data_t*		pdata;
} altek6045_t; 

#define I2A(i) container_of(i, altek6045_t, intf)

#define VOLTAGE_1P1V		1100000
#define VOLTAGE_1P8V		1800000

altek6045_private_data_t altek6045_pdata;
static altek6045_t s_altek6045;
extern struct hisi_pmic_ctrl_t ncp6925_ctrl;
int altek6045_power_on(const hwextisp_intf_t* i)
{
    int ret = 0;
    misp_init();
#if 0
    altek6045_private_data_t* pdata = NULL;
    altek6045_t* mini_isp = NULL;
	
    cam_notice("enter %s.", __func__);
    mini_isp = I2A(i);
    pdata = (altek6045_private_data_t *)mini_isp->pdata;

    /* step1 pull up gpio20 to enable dcdc 1.1v for miniisp */
    //todo...
    // for udp, gpio20 to gpio21
    gpio_direction_output(pdata->pin[ISP_DVDD], MINI_ISP_POWER_ON);

    udelay(5);

    /* step2 open pmic ldo5 1.1v for minisip */
    //todo...
    if (ncp6925_ctrl.func_tbl->pmic_seq_config) {
	ret = ncp6925_ctrl.func_tbl->pmic_seq_config(&ncp6925_ctrl, VOUT_LDO_5, VOLTAGE_1P1V, MINI_ISP_POWER_ON);
    }

    udelay(5);

    /* step3 open pmic ldo4 1.8v for minisip */
    //todo...
    if (ncp6925_ctrl.func_tbl->pmic_seq_config) {
	ret = ncp6925_ctrl.func_tbl->pmic_seq_config(&ncp6925_ctrl, VOUT_LDO_4, VOLTAGE_1P8V, MINI_ISP_POWER_ON);
    }

    msleep(2);

    /* step4 reset miniisp */
    //todo...

    /* step5 load miniisp fw */
    //todo...
#endif
    return ret;
}

int altek6045_power_off(const hwextisp_intf_t* i)
{
	return misp_exit();
#if 0
    altek6045_private_data_t* pdata = NULL;
    altek6045_t* mini_isp = NULL;

    cam_notice("enter %s.", __func__);
    mini_isp = I2A(i);
    pdata = (altek6045_private_data_t *)mini_isp->pdata;

    if (ncp6925_ctrl.func_tbl->pmic_seq_config) {
	ret = ncp6925_ctrl.func_tbl->pmic_seq_config(&ncp6925_ctrl, VOUT_LDO_4, VOLTAGE_1P8V, MINI_ISP_POWER_OFF);
    }

    udelay(5);

    if (ncp6925_ctrl.func_tbl->pmic_seq_config) {
	ret = ncp6925_ctrl.func_tbl->pmic_seq_config(&ncp6925_ctrl, VOUT_LDO_5, VOLTAGE_1P1V, MINI_ISP_POWER_OFF);
    }

    udelay(5);

    gpio_direction_output(pdata->pin[ISP_DVDD], MINI_ISP_POWER_OFF);

    msleep(2);
#endif
}

int altek6045_load_firmware(const hwextisp_intf_t* i)
{
    int rc = 0;
    cam_notice("enter %s.", __func__);

    rc = misp_load_fw();
    if (rc < 0) {
        cam_err("%s failed to load firmware for altek6045.", __func__);
        return rc;
    }
    return 0;
}

int altek6045_exec_cmd(const hwextisp_intf_t* i, hwcam_config_data_t *data)
{
	int rc = 0;
	u8 *in_buf = NULL, *out_buf = NULL;
	u32 opcode, dir_type, block_response, out_len, in_len;
	bool out_to_block;

	cam_notice("enter %s cmd=0x%x.", __func__, data->cmd);

	dir_type = data->cmd & EXTISP_CMD_DIR_FLAG_MASK;
	block_response = data->cmd & EXTISP_CMD_RESP_FLAG_MASK;
	out_len = in_len = (data->cmd & EXTISP_CMD_LEN_MASK)>>EXTISP_CMD_LEN_SHIT;
	opcode = (data->cmd & EXTISP_CMD_OPCODE_MASK)>>EXTISP_CMD_OPCODE_SHIT;

	out_buf = in_buf = data->u.buf;
	out_to_block = (EXTISP_BLOCK_RESPONSE_CMD == block_response)? true: false;

	/* allocate kernel buf: override out_buf out_len*/
	if (out_to_block) {
		out_len = data->ext_buf.user_buf_len;
		if (out_len > 4096) {
			cam_err("%s invalid ext_buf_len=%d", __func__, out_len);
			return -EINVAL;
		}
		out_buf = kmalloc(out_len, GFP_KERNEL);
		if (NULL == out_buf) {
			cam_err("%s kmalloc failed", __func__);
			return -ENOMEM;;
		}
	}

	if (EXTISP_INOUT_CMD == dir_type) {
		rc = misp_exec_bidir_cmd((u16)opcode, data->u.buf, in_len,
							out_to_block, out_buf, out_len);
	} else if (EXTISP_SET_CMD == dir_type) {
		rc = misp_exec_unidir_cmd((u16)opcode, true,
							out_to_block, in_buf, in_len);
	} else if (EXTISP_GET_CMD == dir_type) {
		rc = misp_exec_unidir_cmd((u16)opcode, false,
							out_to_block, out_buf, out_len);
	} else {
		cam_err("%s unkown cmd direction", __func__);
		rc = -EINVAL;
	}

	/* reclaimed kernel buf*/
	if (out_to_block) {
		if (copy_to_user(data->ext_buf.user_buf_ptr, out_buf, out_len)) {
			cam_err("%s copy to user failed", __func__);
			rc = -EFAULT;
		}
		kfree(out_buf);
	}

	return rc;
}

static ssize_t altel6045_powerctrl_show(struct device *dev,
	struct device_attribute *attr,char *buf)
{
        int rc=0;
        cam_info("enter %s", __func__);

        return rc;
}
static ssize_t altel6045_powerctrl_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int state = simple_strtol(buf, NULL, 10);
	cam_info("enter %s, state %d", __func__, state);

	if (state == MINI_ISP_POWER_ON)
		altek6045_power_on(&s_altek6045.intf);
	else
		altek6045_power_off(&s_altek6045.intf);

	return count;
}


static struct device_attribute altel6045_powerctrl =
    __ATTR(power_ctrl, 0664, altel6045_powerctrl_show, altel6045_powerctrl_store);

int altel6045_register_attribute(const hwextisp_intf_t* i, struct device *dev)
{
	int ret = 0;
	cam_info("enter %s", __func__);

	ret = device_create_file(dev, &altel6045_powerctrl);
	if (ret < 0) {
		cam_err("%s failed to creat power ctrl attribute.", __func__);
		goto err_create_power_ctrl;
	}
	return 0;
err_create_power_ctrl:
	device_remove_file(dev, &altel6045_powerctrl);
	return ret;
}

/*
maybe used later
static char *test_raw_buf[ALTEK6045_PIPE_MAX][33];
*/
static int   test_result[ALTEK6045_PIPE_MAX];
#define set_test_result(pipe, result) \
	do { \
		test_result[pipe] = result;\
	} while(0)

static const char *test_report[ALTEK6045_PIPE_TEST_MAX] =
{
	[ALTEK6045_PIPE_UNTESTED] = "untested",
	[ALTEK6045_PIPE_TESTING] = "still in testing",
	[ALTEK6045_PIPE_TEST_CMD_ERR] = "test cmd error",
	[ALTEK6045_PIPE_GET_CMD_ERR] = "get cmd error",
	[ALTEK6045_PIPE_TEST_BAD] = "bad",
	[ALTEK6045_PIPE_TEST_DONE] = "ok",
};


static ssize_t altel6045_test_pipe_show(struct device_driver *drv,
								char *buf);
static ssize_t altel6045_test_pipe_store(struct device_driver *drv,
								const char *buf, size_t count);
static DRIVER_ATTR(test_pipe, 0664, altel6045_test_pipe_show, altel6045_test_pipe_store);


static ssize_t altel6045_test_pipe_store(struct device_driver *drv,
												  const char *buf, size_t count)
{
	int ret = 0, test_pipe_id = -1, index = 0;
	u8 in_buf[7], out_buf[33];
	u16 opcode = 0;
	const char *pos = buf;

	cam_info("%s enter %s", __func__, buf);

	/* input:test_pipe=0 test_pipe=1 test_pipe=2 */
	if (0 == strncmp("test_pipe", pos, strlen("test_pipe"))) {
		while (*pos) {
			if (isdigit(*pos))
				break;
			else
				pos++;
		}
	}

	if (*pos == '0' ) {
		test_pipe_id = ALTEK6045_PIPE_0;
	} else if (*pos == '1') {
		test_pipe_id = ALTEK6045_PIPE_1;
	} else if (*pos == '2') {
		test_pipe_id = ALTEK6045_PIPE_DUAL;
	} else {
	//	test_pipe_id = -1;
		cam_info("%s invalid argument", __func__);
		goto err;
	}

	set_test_result(test_pipe_id, ALTEK6045_PIPE_TESTING);

	/* start test mode */
	memset(in_buf, 0, sizeof(in_buf));
	if (test_pipe_id == ALTEK6045_PIPE_DUAL) {
		in_buf[0] = in_buf[2] = 1;
		in_buf[1] = in_buf[3] = 99;
	} else {
		in_buf[test_pipe_id * 2] = 1;
		in_buf[test_pipe_id * 2 + 1] = 99;
	}
	opcode = ISPCMD_CAMERA_SET_SENSORMODE;
	ret = misp_exec_unidir_cmd(opcode, true, false, in_buf, sizeof(in_buf));
	if (ret) {
		set_test_result(test_pipe_id, ALTEK6045_PIPE_TEST_CMD_ERR);
		cam_err("%s set test mode cmd failed ret:%d", __func__, ret);
		goto err;
	}

	msleep(1000);

	/* get test mode */
	opcode = ISPCMD_GET_BULK_CHIPTEST_REPORT;
	memset(out_buf, 0, 33);
	ret = misp_exec_unidir_cmd(opcode, false, false, out_buf, sizeof(out_buf));
	if (ret) {
		set_test_result(test_pipe_id, ALTEK6045_PIPE_GET_CMD_ERR);
		cam_err("%s get test result cmd failed ret:%d", __func__, ret);
		goto err;
	}

	for (index = 0; index < 33; index++) {
		if(out_buf[index] != 1) {
			set_test_result(test_pipe_id, ALTEK6045_PIPE_TEST_BAD);
			goto err;
		}
	}
	set_test_result(test_pipe_id, ALTEK6045_PIPE_TEST_DONE);

err:
	msleep(100);
	return count;
}


static ssize_t altel6045_test_pipe_show(struct device_driver *drv,
												char *buf)
{
	char *offset = buf;
	int ret, index, stage;
	const char *report;

	cam_info("%s enter", __func__);

	for (index = ALTEK6045_PIPE_0; index < ALTEK6045_PIPE_MAX; index++) {
		stage = test_result[index];
		report = test_report[stage];
		ret = snprintf(offset, PAGE_SIZE, "[PIPE%d:%s] ", index, report);
		offset += ret;
	}
	cam_info("%s:%s",  __func__, buf);

	ret = snprintf(offset, PAGE_SIZE, "\n");
	offset += ret;
	return (offset - buf);
}


int altel6045_get_dt_data(const hwextisp_intf_t *i, struct device_node *of_node)
{
	int ret = 0;
	int index = 0;
	altek6045_private_data_t* pdata = NULL;
	altek6045_t* mini_isp = NULL;
	
	mini_isp = I2A(i);
	pdata = (altek6045_private_data_t *)mini_isp->pdata;

	ret = of_property_read_u32_array(of_node, "hisi,isp-pin",
		pdata->pin, ISP_MAX);
	if (ret < 0) {
		cam_err("%s failed line %d\n", __func__, __LINE__);
		return ret;
	} else {
		for (index = 0; index < ISP_MAX; index++) {
			cam_debug("%s pin[%d]=%d.\n", __func__, index,
				pdata->pin[index]);
		}
	}
#if 0
    ret = gpio_request(pdata->pin[ISP_DVDD], "isp-dcdc");
    if (ret < 0) {
        cam_err("%s failed to request isp-dvdd pin.", __func__);
        return ret;
    }
#endif
	return ret; 
}

char const* altel6045_get_name(const hwextisp_intf_t* i)
{
    altek6045_t* mini_isp = NULL;
    mini_isp = I2A(i);
    return mini_isp->name;    
}

static hwextisp_vtbl_t
s_vtbl_altek6045 = 
{
    .get_name = altel6045_get_name,
    .power_on = altek6045_power_on,
    .power_off = altek6045_power_off,
    .load_firmware = altek6045_load_firmware,
    .exec_cmd = altek6045_exec_cmd,
    .mini_isp_get_dt_data = altel6045_get_dt_data,
    .mini_isp_register_attribute = altel6045_register_attribute,
    //TODO ...

}; 

static altek6045_t 
s_altek6045 = 
{
    .name = "altek6045",
    .intf = { .vtbl = &s_vtbl_altek6045, }, 
    .pdata = &altek6045_pdata,
}; 

static const struct of_device_id 
s_altek6045_dt_match[] = 
{
	{
        .compatible = "huawei,altek6045", 
        .data = &s_altek6045.intf, 
    },
	{
    }, 
};

MODULE_DEVICE_TABLE(of, s_altek6045_dt_match);

static struct platform_driver 
s_altek6045_driver = 
{
	.driver = 
    {
		.name = "huawei,altek6045",
		.owner = THIS_MODULE,
		.of_match_table = s_altek6045_dt_match,
	},
};

static int32_t 
altek6045_platform_probe(
        struct platform_device* pdev)
{
	cam_notice("%s enter", __func__);
    return hwextisp_register(pdev, &s_altek6045.intf);
}

static int __init 
altek6045_init_module(void)
{
	int ret = 0;

	cam_notice("%s enter", __func__);

	ret = platform_driver_probe(&s_altek6045_driver,
			altek6045_platform_probe);

	/* NOTE: use driver attribute */
	if (ret == 0) {
		if (driver_create_file(&s_altek6045_driver.driver, &driver_attr_test_pipe))
			cam_warn("%s create driver attr failed", __func__);
	}

	return ret;
}

static void __exit 
altek6045_exit_module(void)
{
    platform_driver_unregister(&s_altek6045_driver);
}

module_init(altek6045_init_module);
module_exit(altek6045_exit_module);
MODULE_DESCRIPTION("altek6045");
MODULE_LICENSE("GPL v2");

