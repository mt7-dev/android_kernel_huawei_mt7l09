/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/videodev2.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <asm/io.h>

#include "../platform/sensor_commom.h"
#include "../io/hw_isp_io.h"
#include "hw_csi.h"

/*csi register macro definition*/
#define CSI_INT_EN			0x00000000
#define CSI_INT_DIS			0xffffffff
#define CSI_REG_N_LANES     		(0x04)
#define CSI_REG_PHY_SHUTDOWNZ   	(0x08)
#define CSI_REG_DPHY_RSTZ		(0x0c)
#define CSI_REG_RESETN			(0x10)
#define CSI_REG_PHY_STATE		(0x14)
#define CSI_REG_MASK1			(0x28)
#define CSI_REG_MASK2          		(0x2c)

#define PHY_TST_CONTRL0			(0x30)
#define PHY_TST_CONTRL1			(0x34)

#define CSI_REG_ERR1			(0x20)
#define CSI_REG_ERR2			(0x24)
#define CSI_PERI_CTRL19                 (0x50)
#define CSI_PERI_CTRL24                 (0x64)
#define PCTRL_BASE			(0xE8A09000)
#define DPHY_DSI_CSI_2_CSI_P_1	(1 << 19)
#define DPHY_DSI_CSI_2_CSI_P_2	(1 << 22)
#define DPHY_CSI2_2_CSI_S	(1 << 23)
#define DPHY_MUX_SEL		(1 << 12)

#define PHY_STATE_STOPSTATE0      	(1 << 4)
#define PHY_STATE_STOPSTATE1      	(1 << 5)
#define PHY_STATE_RXCLKACTIVEHS    	(1 << 8)
#define PHY_STATE_STOPSTATECLK     	(1 << 10)

#define PHY_SELECT_PRIME_SENSOR         (1 << 19 | 1 << 22)
#define PHY_SELECT_SECOND_SENSOR        (1 << 23)

#define CSI_INIT_DELAY			10

struct hw_csi_pad hw_csi_pad;

#if 0
static irqreturn_t hisi_csi0_isr(int irq, void *dev_id)
{
	u32 irq_status1;
	u32 irq_status2;

	u32 reg_offset = hw_csi_pad.info.csi_base_offset[CSI_INDEX_0];

	irq_status1 = HW_CSI_GETREG32(reg_offset + CSI_REG_ERR1);
	irq_status2 = HW_CSI_GETREG32(reg_offset + CSI_REG_ERR2);

	if ((irq_status1 | irq_status2) != 0)
		cam_err("%s ERR1[%#x], ERR2[%#x]\n", __func__, irq_status1, irq_status2);

	/* automatic clear all interrupts */

	return IRQ_HANDLED;
}

static irqreturn_t hisi_csi1_isr(int irq, void *dev_id)
{
	u32 irq_status1;
	u32 irq_status2;

	u32 reg_offset = hw_csi_pad.info.csi_base_offset[CSI_INDEX_1];

	irq_status1 = HW_CSI_GETREG32(reg_offset + CSI_REG_ERR1);
	irq_status2 = HW_CSI_GETREG32(reg_offset + CSI_REG_ERR2);

	if ((irq_status1 | irq_status2) != 0)
		cam_err("%s ERR1[%#x], ERR2[%#x]\n", __func__, irq_status1, irq_status2);

	/* automatic clear all interrupts */

	return IRQ_HANDLED;
}
#endif

int hw_csi_enable(csi_index_t csi_index, int csi_lane, int csi_mipi_clk)
{
	/*CSI PHY enbale*/
	u32 phy_state1;
	u32 reg_offset;
	u32 err1 ;
	u32 err2 ;
	int ret = 0;

	cam_notice("%s enter:csi_index=%d,csi_lane=%d,csi_mipi_clk=%d.\n", __func__,
		csi_index, csi_lane, csi_mipi_clk);
	if (csi_index == 2) {
		csi_index = 1;
	}

	if(hw_csi_pad.csi_inited[csi_index]) {
		cam_info("csi  phy has already been enabled.");
		return ret;
	}

	reg_offset = hw_csi_pad.info.csi_base_offset[csi_index];
	cam_info("%s reg_offset = 0x%x.\n", __func__, reg_offset);

	if (!hw_is_fpga_board()) {//fixme
		u32 pctrl_reg = 0;
		cam_debug("enter %s, excute enable csi clock", __func__);
		ret = clk_prepare_enable(hw_csi_pad.info.phyclk[csi_index]);
		if (0 != ret) {
			cam_err("%s, clock enable fail", __func__);
		}

		if (!hw_csi_pad.info.dsi_csi_used) {
			/* config csi to sensor */
			cam_notice("%s Config CSI to Sensor.", __func__);
			pctrl_reg = (u32)ioremap_nocache(PCTRL_BASE, 0x8000);
			writel_relaxed(readl_relaxed((const volatile void *)(pctrl_reg + CSI_PERI_CTRL19)) | DPHY_CSI2_2_CSI_S,
				(volatile void *)(pctrl_reg + CSI_PERI_CTRL19));
			iounmap((void*)pctrl_reg);
		} else {
			/* config csi to sensor */
			cam_notice("%s Config CSI to Sensor.", __func__);
			pctrl_reg = (u32)ioremap_nocache(PCTRL_BASE, 0x8000);
			writel_relaxed(readl_relaxed((const volatile void *)(pctrl_reg + CSI_PERI_CTRL24)) & ~DPHY_MUX_SEL,
				(volatile void *)(pctrl_reg + CSI_PERI_CTRL24));
			//writel_relaxed(readl_relaxed(prti_cfg_reg + REG_PCTRL24) & ~DPHY_MUX_SEL, prti_cfg_reg + REG_PCTRL24);
			iounmap((void*)pctrl_reg);
		}

	}
#ifdef DEBUG_CSI_IRQ
	cam_info("%s request csi0 & csi1 irq.\n", __func__);
	ret = request_irq(hw_csi_pad.info.csi_irq[CSI_INDEX_0], hisi_csi0_isr, 0, "csi0_irq", 0);
	if (ret < 0) {
		cam_err("fail to request irq [%d]", hw_csi_pad.info.csi_irq[CSI_INDEX_0]);
		return -EAGAIN;
	}

	ret = request_irq(hw_csi_pad.info.csi_irq[CSI_INDEX_1], hisi_csi1_isr, 0, "csi1_irq", 0);
	if (ret < 0) {
		cam_err("fail to request irq [%d]", hw_csi_pad.info.csi_irq[CSI_INDEX_1]);
		free_irq(hw_csi_pad.info.csi_irq[CSI_INDEX_0], 0);
		return -EAGAIN;
	}
#endif

	HW_CSI_SETREG32(reg_offset + CSI_REG_PHY_SHUTDOWNZ, 0);
	HW_CSI_SETREG32(reg_offset + CSI_REG_PHY_SHUTDOWNZ, 1);

	HW_CSI_SETREG32(reg_offset + CSI_REG_N_LANES, csi_lane);

	HW_CSI_SETREG32(reg_offset + CSI_REG_DPHY_RSTZ, 0);
	HW_CSI_SETREG32(reg_offset + CSI_REG_DPHY_RSTZ, 1);

	HW_CSI_SETREG32(reg_offset + CSI_REG_RESETN, 0);
	HW_CSI_SETREG32(reg_offset + CSI_REG_RESETN, 1);

	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL0, 1);
	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL0, 0);

	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL1, 0x10044);
	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL0, 2);
	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL0, 0);

	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL1, csi_mipi_clk);
	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL0, 2);
	HW_CSI_SETREG32(reg_offset + PHY_TST_CONTRL0, 0);


	/*
	   csi-2 Controller programming - Read the PHY status register (PHY_STATE) to confirm that all data
	   and clock lanes of the D-PHY are in Stop State (that is, ready to receive data).
	 */

	/*
	   Configure the MIPI Camera Sensor - Access Camera Sensor using CCI interface to initialize and
	   configure the Camera Sensor to start transmitting a clock on the D-PHY clock lane.
	 */

	/*
	   csi-2 Controller programming - Read the PHY status register (PHY_STATE) to confirm that the
	   D-PHY is receiving a clock on the D-PHY clock lane.
	 */

	phy_state1 = HW_CSI_GETREG32(reg_offset + CSI_REG_PHY_STATE);

	err1 = HW_CSI_GETREG32(reg_offset + CSI_REG_ERR1);

	err2 = HW_CSI_GETREG32(reg_offset + CSI_REG_ERR2);

	cam_debug("Not all data and clock lanes of the D-PHY are in Stop State,"
		"err1 : %#x, err2 : %#x .\n", err1,  err2);

	if (0 == (phy_state1 & PHY_STATE_STOPSTATE0)
	&& 0 == (phy_state1 & PHY_STATE_STOPSTATECLK)) {
		cam_info("Not all data and clock lanes of the D-PHY are in Stop State,"
			"phy_state1 : %#x .\n", phy_state1);
	}

	if (0 == (phy_state1 & PHY_STATE_RXCLKACTIVEHS)) {
		cam_info("D-PHY was not receive a clock \n");
	}

	HW_CSI_SETREG32(reg_offset + CSI_REG_MASK1, CSI_INT_EN);
	HW_CSI_SETREG32(reg_offset + CSI_REG_MASK2, CSI_INT_EN);

	phy_state1 = HW_CSI_GETREG32(reg_offset + CSI_REG_PHY_STATE);
	cam_debug("Not all data and clock lanes of the D-PHY are in Stop State,"
		"phy_state1 : %#x .\n", phy_state1);
	hw_csi_pad.csi_inited[csi_index] = true;

	return ret;
}

int hw_csi_disable(csi_index_t csi_index)
{
	u32 reg_offset;

	cam_info("%s enter:csi_index=%d.\n", __func__, csi_index);
	if (csi_index == 2) {
		csi_index = 1;
	}

	if(!hw_csi_pad.csi_inited[csi_index]) {
		cam_debug("csi is disabled now");
		return 0;
	}

	reg_offset = hw_csi_pad.info.csi_base_offset[csi_index];

	HW_CSI_SETREG32(reg_offset + CSI_REG_RESETN, 0);
	HW_CSI_SETREG32(reg_offset + CSI_REG_PHY_SHUTDOWNZ, 0);

	if (!hw_is_fpga_board()) {
		clk_disable_unprepare(hw_csi_pad.info.phyclk[csi_index]);
	}
#ifdef DEBUG_CSI_IRQ
	cam_info("%s free csi0 & csi1 irq.\n", __func__);
	free_irq(hw_csi_pad.info.csi_irq[CSI_INDEX_0], 0);
	free_irq(hw_csi_pad.info.csi_irq[CSI_INDEX_1], 0);
#endif
	hw_csi_pad.csi_inited[csi_index] = false;
	return 0;
}

int hw_csi_config(csi_index_t csi_index, int csi_lane, int csi_mipi_clk)
{
	cam_debug("%s enter.\n", __func__);
	/*TO DO ...*/
	return 0;
}

int hw_csi_platform_probe(struct platform_device *pdev)
{
	struct device_node *of_node = pdev->dev.of_node;
	uint32_t base_array[CSI_INDEX_CNT] = {0};
	uint32_t count = 0;
	int ret;
	int i;

	cam_debug("%s enter.\n", __func__);
	/*NOTE: should assign all the csi phy hardware info here*/

	/* property(hisi,csi-base) = <csi0_base_offset, csi1_base_offset>,
	 * so count is 2 .
	 */
	count = 2;
	if (of_node) {
		ret = of_property_read_u32_array(of_node, "huawei,csi-base-offset",
			base_array, count);
		if (ret < 0) {
			cam_err("%s failed line %d\n", __func__, __LINE__);
			return ret;
		}
	} else {
		cam_err("%s csi of_node is NULL.\n", __func__);
		return -ENXIO;
	}

	for (i=0; i<count; i++) {
		cam_debug("%s csi base[%d] = 0x%x.\n", __func__, i, base_array[i]);
		hw_csi_pad.info.csi_base_offset[i] = base_array[i];
	}

	ret = of_property_read_u32(of_node, "huawei,csi0-irq",
		&hw_csi_pad.info.csi_irq[CSI_INDEX_0]);
	if (ret < 0) {
		cam_err("%s failed line %d\n", __func__, __LINE__);
		return ret;
	}

	ret = of_property_read_u32(of_node, "huawei,csi1-irq",
		&hw_csi_pad.info.csi_irq[CSI_INDEX_1]);
	if (ret < 0) {
		cam_err("%s failed line %d\n", __func__, __LINE__);
		return ret;
	}

	cam_notice("%s csi0-irq = %d, csi1-irq = %d.\n", __func__,
		hw_csi_pad.info.csi_irq[CSI_INDEX_0], hw_csi_pad.info.csi_irq[CSI_INDEX_1]);

	ret = of_property_read_u32(of_node, "huawei,dsi_csi_used",
		&hw_csi_pad.info.dsi_csi_used);
	if (ret < 0) {
		hw_csi_pad.info.dsi_csi_used = 0;
	}
	cam_notice("%s dsi_csi_used=%d\n", __func__, hw_csi_pad.info.dsi_csi_used);

	/* get csi clock */
	hw_csi_pad.info.phyclk[CSI_INDEX_0]= devm_clk_get(&pdev->dev, "clk_dphy1");
	if (IS_ERR_OR_NULL(hw_csi_pad.info.phyclk[CSI_INDEX_0])) {
		cam_err("failed to get dphy1 clock");
		ret = -ENXIO;
	}


	if (hw_csi_pad.info.dsi_csi_used) {
		hw_csi_pad.info.phyclk[CSI_INDEX_1]= devm_clk_get(&pdev->dev, "clk_dphy2");
		if (IS_ERR_OR_NULL(hw_csi_pad.info.phyclk[CSI_INDEX_1])) {
			cam_err("failed to get dphy2 clock");
			ret = -ENXIO;
		}
	} else {
		hw_csi_pad.info.phyclk[CSI_INDEX_1]= devm_clk_get(&pdev->dev, "clk_dphy3");
		if (IS_ERR_OR_NULL(hw_csi_pad.info.phyclk[CSI_INDEX_1])) {
			cam_err("failed to get dphy3 clock");
			ret = -ENXIO;
		}
	}



	return ret;
}

struct hw_csi_pad hw_csi_pad = {
	.csi_inited = {false, false},
	.hw_csi_enable = hw_csi_enable,
	.hw_csi_disable = hw_csi_disable,
	.hw_csi_config = hw_csi_config,
};

static const struct of_device_id hisi_csi_dt_match[] = {
	{.compatible = "huawei,csi-phy"},
	{}
};
MODULE_DEVICE_TABLE(of, hisi_csi_dt_match);
static struct platform_driver csi_platform_driver = {
	.driver = {
		.name = "csi-phy",
		.owner = THIS_MODULE,
		.of_match_table = hisi_csi_dt_match,
	},
};

static int32_t csi_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;

	match = of_match_device(hisi_csi_dt_match, &pdev->dev);
	cam_notice("%s compatible=%s.\n", __func__, match->compatible);
	rc = hw_csi_platform_probe(pdev);
	return rc;
}

static int __init csi_init_module(void)
{
	int rc = 0;
	cam_debug("%s:%d\n", __func__, __LINE__);

	rc = platform_driver_probe(&csi_platform_driver,
		csi_platform_probe);
	if (rc < 0) {
		cam_err("%s platform_driver_probe error(%d).\n", __func__, rc);
	}
	return rc;
}

static void __exit csi_exit_module(void)
{
	return platform_driver_unregister(&csi_platform_driver);
}

module_init(csi_init_module);
module_exit(csi_exit_module);
MODULE_DESCRIPTION("hisi_csi");
MODULE_LICENSE("GPL v2");

