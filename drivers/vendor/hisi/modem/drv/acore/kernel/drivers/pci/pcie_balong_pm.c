#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#include <linux/clk.h>
#include <linux/msi.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/interrupt.h>
#include <linux/syscore_ops.h>
#include <linux/regulator/consumer.h>

#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/pci_regs.h>
#include <asm/mach/pci.h>

#include <bsp_memmap.h>
#include <product_config.h>
#include <hi_pcie_interface.h>

#include "pci.h"
#include "pcie_balong.h"


bool balong_pcie_pm_is_manageable(struct pci_dev *dev)
{
    return true;
}

int balong_pcie_pm_set_state(struct pci_dev *dev, pci_power_t state)
{
    struct balong_pcie *info = NULL;

    info = (struct balong_pcie *)((struct pci_sys_data *)dev->bus->sysdata)->private_data;
    if (PCI_D0 == state)
    {
        /* assert the apps_pm_xmt_pme */
        hi_pcie_set_apps_pm_xmt_pme(info->hw_pci.domain, 1);
        udelay(1);
        hi_pcie_set_apps_pm_xmt_pme(info->hw_pci.domain, 0);

        pcie_ltssm_enable(info->hw_pci.domain);
        pcie_wait_for_linkup(info->hw_pci.domain);

        if (balong_pcie[info->hw_pci.domain].linked)
        {
            pcie_try_change_speed(info->hw_pci.domain);
        }
        else
        {
            pcie_trace("link up time out, controller: %u\n", info->hw_pci.domain);
        }
    }

    if (PCI_D3cold == state)
    {
        pcie_ltssm_disable(info->hw_pci.domain);

        /* broadcast the PME_Turn_Off Message by asserting the signal for one clock */
        hi_pcie_set_apps_pm_xmt_turnoff(info->hw_pci.domain, 1);

        /* wait for the PME_TO_Ack Message */
        while (hi_pcie_get_radm_pm_to_ack_reg(info->hw_pci.domain)) ;

        hi_pcie_set_apps_pm_xmt_turnoff(info->hw_pci.domain, 0);
    }

    return 0;
}

pci_power_t balong_pcie_pm_choose_state(struct pci_dev *dev)
{
    return PCI_D0;
}

bool balong_pcie_pm_can_wakeup(struct pci_dev *dev)
{
    return false;
}

int balong_pcie_pm_sleep_wake(struct pci_dev *dev, bool enable)
{
    return 0;
}

int balong_pcie_pm_run_wake(struct pci_dev *dev, bool enable)
{
    return 0;
}

struct pci_platform_pm_ops balong_pcie_platform_pm_ops =
{
    .is_manageable = balong_pcie_pm_is_manageable,
    .set_state = balong_pcie_pm_set_state,
    .choose_state = balong_pcie_pm_choose_state,
    .can_wakeup = balong_pcie_pm_can_wakeup,
    .sleep_wake = balong_pcie_pm_sleep_wake,
    .run_wake = balong_pcie_pm_run_wake,
};

int balong_pcie_pm_prepare(struct device *dev)
{
    int ret = 0;
    struct pci_dev *pci_dev = to_pci_dev(dev);

    ret = pci_save_state(pci_dev);

    return ret;
}

int balong_pcie_pm_suspend(struct device *dev)
{
    return 0;
}

int balong_pcie_pm_suspend_noirq(struct device *dev)
{
    return 0;
}

int balong_pcie_pm_resume_noirq(struct device *dev)
{
    struct pci_dev *pci_dev = to_pci_dev(dev);

    pci_restore_state(pci_dev);

    return 0;
}

int balong_pcie_pm_resume(struct device *dev)
{
    return 0;
}

void balong_pcie_pm_complete(struct device *dev)
{
    struct balong_pcie *info = NULL;

    info = (struct balong_pcie *)dev->platform_data;

    pcie_link_up_check(info->hw_pci.domain);
    if (balong_pcie[info->hw_pci.domain].linked)
    {
        pcie_try_change_speed(info->hw_pci.domain);
    }

    enable_irq(info->irqs.link_down_int);

    return;
}

int balong_pcie_pm_sys_suspend(void)
{
    u32 id = 0;

    for (id = 0; id < balong_pcie_num; id++)
    {
        if ((!balong_pcie[id].enabled) || (balong_pcie[id].work_mode != PCIE_WORK_MODE_RC))
            continue;

        /* broadcast the PME_Turn_Off Message by asserting the signal for one clock */
        hi_pcie_set_apps_pm_xmt_turnoff(id, 1);

        /* assert the app_ready_entr_123 */
        hi_pcie_set_app_ready_entr_l23(id, 1);

        /* wait for the PME_TO_Ack Message */
        /* while (!get_hi_sc_stat71_radm_pm_to_ack_reg()) ; */

        pcie_ltssm_disable(id);
        pcie_reset(id);
        pcie_clk_disable(id);
        pcie_power_off(id);
    }

    return 0;
}

void balong_pcie_pm_sys_resume(void)
{
    u32 id = 0;

    for (id = 0; id < balong_pcie_num; id++)
    {
        if ((!balong_pcie[id].enabled) || (balong_pcie[id].work_mode != PCIE_WORK_MODE_RC))
            continue;

        disable_irq(balong_pcie[id].irqs.link_down_int);

        /* stop to broadcast the PME_Turn_Off Message */
        hi_pcie_set_apps_pm_xmt_turnoff(id, 0);
        /* deassert the app_ready_entr_123 */
        hi_pcie_set_app_ready_entr_l23(id, 0);

        /* assert the apps_pm_xmt_pme */
        hi_pcie_set_apps_pm_xmt_pme(id, 1);
        udelay(1);
        hi_pcie_set_apps_pm_xmt_pme(id, 0);

        pcie_hardware_init(balong_pcie[id].hw_pci.domain);
    }
}

void balong_pcie_pm_sys_shutdown(void)
{
}

struct syscore_ops balong_pcie_pm_syscore_ops =
{
    .suspend = balong_pcie_pm_sys_suspend,
    .resume  = balong_pcie_pm_sys_resume,
    .shutdown= balong_pcie_pm_sys_shutdown,
};

int balong_pcie_pm_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    int ret = 0;

    register_syscore_ops(&balong_pcie_pm_syscore_ops);

    /* ret = pci_set_platform_pm(&balong_pci_platform_pm_ops); */

    dev->dev.platform_data = (void *)((struct pci_sys_data *)dev->bus->sysdata)->private_data;

    return ret;
}

static DEFINE_PCI_DEVICE_TABLE(balong_pci_table) = {
    {PCI_DEVICE_CLASS(PCI_CLASS_BRIDGE_PCI<<8, ~0)},
    {0},
};

MODULE_DEVICE_TABLE(pci, balong_pci_table);

struct dev_pm_ops balong_pcie_dev_pm_ops = {
    .prepare    = balong_pcie_pm_prepare,
    .suspend    = balong_pcie_pm_suspend,
    .suspend_noirq  = balong_pcie_pm_suspend_noirq,
    .resume_noirq   = balong_pcie_pm_resume_noirq,
    .resume     = balong_pcie_pm_resume,
    .complete   = balong_pcie_pm_complete,
};

struct pci_driver balong_pcie_pci_driver =
{
    .name   = "pcie_balong",
    .id_table   = balong_pci_table,
    .probe  = balong_pcie_pm_probe,
    .remove = NULL,
    .err_handler = NULL,
    .driver = {
        .pm = &balong_pcie_dev_pm_ops,
    },
};

static int __init balong_pcie_pm_init(void)
{
    int ret = 0;

    ret = pci_register_driver(&balong_pcie_pci_driver);

    return ret;
}


subsys_initcall_sync(balong_pcie_pm_init);


