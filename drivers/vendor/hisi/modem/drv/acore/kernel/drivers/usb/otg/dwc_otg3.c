/*lint -save -e7 -e21 -e34 -e63 -e124 -e578 -e537 -e573 -e52  -e831 -e413 -e132 -e101 -e2 -e565 
-e84 -e438 -e516 -e527 -e530 -e533  -e550 -e43  -e62 -e64 -e648 -e616 -e30 -e529 -e665 -e123 -e19 
-e732 -e539 -e322 -e401 -e713 -e737 -e718 -e746 -e752 -e830 -e958*/
/*lint --e{438}*/
/*lint --e{529}*/
/*lint --e{550}*/
/*lint --e{533}*/
/*lint --e{752}*/
/*lint --e{830}*/
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/usb/otg.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include <linux/version.h>

#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/usb/gadget.h>
#include <linux/gpio.h>

#include <asm/mach/irq.h>
#include "bsp_memmap.h"
#include "hi_syssc_interface.h"
#include "hi_syscrg_interface.h"
#include "bsp_usb.h"
#include "usb_vendor.h"

#define VERSION "2.10a"

#define otg_dbg     USB_DBG_OTG
#define otg_vdbg    USB_DBG_OTG 
#define otg_err(d, fmt, args...)  dev_err((d)->dev, "%s(): ERROR: " fmt , __func__, ## args)
#define otg_warn(d, fmt, args...) dev_warn((d)->dev, "%s(): WARN: " fmt , __func__, ## args)
#define otg_info(d, fmt, args...) dev_info((d)->dev, "%s(): INFO: " fmt , __func__, ## args)

#ifdef DEBUG
#define otg_write(o, reg, val)	do {					\
		otg_dbg(o, "OTG_WRITE: reg=0x%05x, val=0x%08x\n", reg, val); \
		writel(val, ((void *)((o)->otg.phy->io_priv)) + reg);	\
	} while (0)

#define otg_read(o, reg) ({						\
		u32 __r = readl(((void *)((o)->otg.phy->io_priv)) + reg);	\
		otg_dbg(o, "OTG_READ: reg=0x%05x, val=0x%08x\n", reg, __r); \
		__r;							\
	})
#else
#define otg_write(o, reg, val)	writel(val, ((void *)((o)->otg.phy->io_priv)) + reg)
#define otg_read(o, reg)	readl(((void *)((o)->otg.phy->io_priv)) + reg)
#endif

#define GUSB2PHYCFG0				0xc200
#define GUSB2PHYCFG_SUS_PHY                     0x40

#define GUSB3PIPECTL0                           0xc2c0
#define GUSB3PIPECTL_SUS_EN                     0x20000

#define GHWPARAMS6				0xc158
#define GHWPARAMS6_SRP_SUPPORT_ENABLED		0x0400
#define GHWPARAMS6_HNP_SUPPORT_ENABLED		0x0800
#define GHWPARAMS6_ADP_SUPPORT_ENABLED		0x1000

#define GCTL 0xc110
#define GCTL_PRT_CAP_DIR 0x3000
#define GCTL_PRT_CAP_DIR_SHIFT 12
#define GCTL_PRT_CAP_DIR_HOST 1
#define GCTL_PRT_CAP_DIR_DEV 2
#define GCTL_PRT_CAP_DIR_OTG 3
#define GCTL_GBL_HIBERNATION_EN 0x2

#define OCFG					0xcc00
#define OCFG_SRP_CAP				0x01
#define OCFG_SRP_CAP_SHIFT			0
#define OCFG_HNP_CAP				0x02
#define OCFG_HNP_CAP_SHIFT			1
#define OCFG_OTG_VERSION			0x04
#define OCFG_OTG_VERSION_SHIFT		2
#define OCFG_OTG_PRTPWRCUTOFF		0x20
#define OCFG_OTG_PRTPWRCUTOFF_SHIFT	5

#define OCTL					0xcc04
#define OCTL_HST_SET_HNP_EN			0x01
#define OCTL_HST_SET_HNP_EN_SHIFT		0
#define OCTL_DEV_SET_HNP_EN			0x02
#define OCTL_DEV_SET_HNP_EN_SHIFT		1
#define OCTL_TERM_SEL_DL_PULSE			0x04
#define OCTL_TERM_SEL_DL_PULSE_SHIFT		2
#define OCTL_SES_REQ				0x08
#define OCTL_SES_REQ_SHIFT			3
#define OCTL_HNP_REQ				0x10
#define OCTL_HNP_REQ_SHIFT			4
#define OCTL_PRT_PWR_CTL			0x20
#define OCTL_PRT_PWR_CTL_SHIFT			5
#define OCTL_PERI_MODE				0x40
#define OCTL_PERI_MODE_SHIFT			6

#define OEVT					0xcc08
#define OEVT_ERR				0x00000001
#define OEVT_ERR_SHIFT				0
#define OEVT_SES_REQ_SCS			0x00000002
#define OEVT_SES_REQ_SCS_SHIFT			1
#define OEVT_HST_NEG_SCS			0x00000004
#define OEVT_HST_NEG_SCS_SHIFT			2
#define OEVT_B_SES_VLD_EVT			0x00000008
#define OEVT_B_SES_VLD_EVT_SHIFT		3
#define OEVT_B_DEV_VBUS_CHNG_EVNT		0x00000100
#define OEVT_B_DEV_VBUS_CHNG_EVNT_SHIFT		8
#define OEVT_B_DEV_SES_VLD_DET_EVNT		0x00000200
#define OEVT_B_DEV_SES_VLD_DET_EVNT_SHIFT	9
#define OEVT_B_DEV_HNP_CHNG_EVNT		0x00000400
#define OEVT_B_DEV_HNP_CHNG_EVNT_SHIFT		10
#define OEVT_B_DEV_B_HOST_END_EVNT		0x00000800
#define OEVT_B_DEV_B_HOST_END_EVNT_SHIFT	11
#define OEVT_A_DEV_SESS_END_DET_EVNT		0x00010000
#define OEVT_A_DEV_SESS_END_DET_EVNT_SHIFT	16
#define OEVT_A_DEV_SRP_DET_EVNT			0x00020000
#define OEVT_A_DEV_SRP_DET_EVNT_SHIFT		17
#define OEVT_A_DEV_HNP_CHNG_EVNT		0x00040000
#define OEVT_A_DEV_HNP_CHNG_EVNT_SHIFT		18
#define OEVT_A_DEV_HOST_EVNT			0x00080000
#define OEVT_A_DEV_HOST_EVNT_SHIFT		19
#define OEVT_A_DEV_B_DEV_HOST_END_EVNT		0x00100000
#define OEVT_A_DEV_B_DEV_HOST_END_EVNT_SHIFT	20
#define OEVT_HOST_ROLE_REQ_INIT_EVNT            0x00400000
#define OEVT_HOST_ROLE_REQ_INIT_EVNT_SHIFT      22
#define OEVT_HOST_ROLE_REQ_CONFIRM_EVNT         0x00800000
#define OEVT_HOST_ROLE_REQ_CONFIRM_EVNT_SHIFT   23
#define OEVT_CONN_ID_STS_CHNG_EVNT		0x01000000
#define OEVT_CONN_ID_STS_CHNG_EVNT_SHIFT	24
#define OEVT_DEV_MOD_EVNT			0x80000000
#define OEVT_DEV_MOD_EVNT_SHIFT			31

#define OEVTEN					0xcc0c

#define OEVT_ALL (OEVT_CONN_ID_STS_CHNG_EVNT | \
	        OEVT_HOST_ROLE_REQ_INIT_EVNT | \
	        OEVT_HOST_ROLE_REQ_CONFIRM_EVNT | \
		OEVT_A_DEV_B_DEV_HOST_END_EVNT | \
		OEVT_A_DEV_HOST_EVNT | \
		OEVT_A_DEV_HNP_CHNG_EVNT | \
		OEVT_A_DEV_SRP_DET_EVNT | \
		OEVT_A_DEV_SESS_END_DET_EVNT | \
		OEVT_B_DEV_B_HOST_END_EVNT | \
		OEVT_B_DEV_HNP_CHNG_EVNT | \
		OEVT_B_DEV_SES_VLD_DET_EVNT | \
		OEVT_B_DEV_VBUS_CHNG_EVNT)

#define OSTS					0xcc10
#define OSTS_CONN_ID_STS			0x0001
#define OSTS_CONN_ID_STS_SHIFT			0
#define OSTS_A_SES_VLD				0x0002
#define OSTS_A_SES_VLD_SHIFT			1
#define OSTS_B_SES_VLD				0x0004
#define OSTS_B_SES_VLD_SHIFT			2
#define OSTS_XHCI_PRT_PWR			0x0008
#define OSTS_XHCI_PRT_PWR_SHIFT			3
#define OSTS_PERIP_MODE				0x0010
#define OSTS_PERIP_MODE_SHIFT			4
#define OSTS_OTG_STATES				0x0f00
#define OSTS_OTG_STATE_SHIFT			8

#define ADPCFG					0xcc20
#define ADPCFG_PRB_DSCHGS			0x0c000000
#define ADPCFG_PRB_DSCHG_SHIFT			26
#define ADPCFG_PRB_DELTAS			0x30000000
#define ADPCFG_PRB_DELTA_SHIFT			28
#define ADPCFG_PRB_PERS				0xc0000000
#define ADPCFG_PRB_PER_SHIFT			30

#define ADPCTL					0xcc24
#define ADPCTL_WB				0x01000000
#define ADPCTL_WB_SHIFT				24
#define ADPCTL_ADP_RES				0x02000000
#define ADPCTL_ADP_RES_SHIFT			25
#define ADPCTL_ADP_EN				0x04000000
#define ADPCTL_ADP_EN_SHIFT			26
#define ADPCTL_ENA_SNS				0x08000000
#define ADPCTL_ENA_SNS_SHIFT			27
#define ADPCTL_ENA_PRB				0x10000000
#define ADPCTL_ENA_PRB_SHIFT			28

#define ADPEVT					0xcc28
#define ADPEVT_RTIM_EVNTS			0x000007ff
#define ADPEVT_RTIM_EVNT_SHIFT			0
#define ADPEVT_ADP_RST_CMPLT_EVNT		0x02000000
#define ADPEVT_ADP_RST_CMPLT_EVNT_SHIFT		25
#define ADPEVT_ADP_TMOUT_EVNT			0x04000000
#define ADPEVT_ADP_TMOUT_EVNT_SHIFT		26
#define ADPEVT_ADP_SNS_EVNT			0x08000000
#define ADPEVT_ADP_SNS_EVNT_SHIFT		27
#define ADPEVT_ADP_PRB_EVNT			0x10000000
#define ADPEVT_ADP_PRB_EVNT_SHIFT		28

#define ADPEVTEN				0xcc2c
#define ADPEVTEN_ACC_DONE_EN			0x01000000
#define ADPEVTEN_ACC_DONE_EN_SHIFT		24
#define ADPEVTEN_ADP_RST_CMPLT_EVNT_EN		0x02000000
#define ADPEVTEN_ADP_RST_CMPLT_EVNT_EN_SHIFT	25
#define ADPEVTEN_ADP_TMOUT_EVNT_EN		0x04000000
#define ADPEVTEN_ADP_TMOUT_EVNT_EN_SHIFT	26
#define ADPEVTEN_ADP_SNS_EVNT_EN		0x08000000
#define ADPEVTEN_ADP_SNS_EVNT_EN_SHIFT		27
#define ADPEVTEN_ADP_PRB_EVNT_EN		0x10000000
#define ADPEVTEN_ADP_PRB_EVNT_EN_SHIFT		28

/** The states for the OTG driver */
typedef enum dwc_otg_state {
	DWC_STATE_INVALID = -1,

	/** The initial state, check the connector id status and determine what mode
	 * (A-device or B-device) to operate in. */
	DWC_STATE_INIT = 0,

	/* A-Host states */
	DWC_STATE_A_PROBE,
	DWC_STATE_A_HOST,
	DWC_STATE_A_HNP_INIT,

	/* A-Peripheral states */
	DWC_STATE_A_PERIPHERAL,

	/* B-Peripheral states */
	DWC_STATE_B_SENSE,
	DWC_STATE_B_PROBE,
	DWC_STATE_B_PERIPHERAL,
	DWC_STATE_B_HNP_INIT,

	/* B-Host states */
	DWC_STATE_B_HOST,

	/* RSP */
	DWC_STATE_B_RSP_INIT,

	/* Exit */
	DWC_STATE_EXIT,
	DWC_STATE_TERMINATED,
} otg_state_t;

static struct {

	int hibernate;

} otg_params = {

	.hibernate = 0,

};

module_param_named(hibernate, otg_params.hibernate, int, S_IRUGO);
MODULE_PARM_DESC(hibernate, "Enable hibernation (0=no, 1=yes)");

/** The main structure to keep track of OTG driver state. */
struct dwc_otg2 {
	/** OTG transceiver */
	struct usb_otg	otg;
	struct device		*dev;

    int main_wakeup_needed;
    struct task_struct *main_thread;
    wait_queue_head_t main_wq;
    wait_queue_head_t exit_wq;

	spinlock_t lock;

    u32 thread_cancel;

	/* Events */
	u32 otg_events;
	u32 adp_events;

	u32 user_events;
	/** User initiated SRP.
	 *
	 * Valid in B-device during sensing/probing. Initiates SRP signalling
	 * across the bus.
	 *
	 * Also valid as an A-device during probing. This causes the A-device to
	 * apply V-bus manually and check for a device. Can be used if the
	 * device does not support SRP and the host does not support ADP. */
#define USER_SRP_EVENT 0x1
	/** User initiated HNP (only valid in B-peripheral) */
#define USER_HNP_EVENT 0x2
	/** User has ended the session (only valid in B-peripheral) */
#define USER_END_SESSION 0x4
	/** User initiated VBUS. This will cause the A-device to turn on the
	 * VBUS and see if a device will connect (only valid in A-device during
	 * sensing/probing) */
#define USER_VBUS_ON 0x8

#define USER_EXIT_EVENT 0x10

	/** User has initiated RSP */
#define USER_RSP_EVENT 0x10
	/** Host release event */
#define PCD_RECEIVED_HOST_RELEASE_EVENT 0x20
	/** Initial SRP */
#define INITIAL_SRP 0x40

	/* States */
	otg_state_t prev;
	otg_state_t state;
};

static struct platform_device *pd;
static struct mutex lock;
static struct resource res[] = {
	[0] = {
		.start = INT_LVL_USB3_OTG,
		.end = INT_LVL_USB3_OTG,
		.flags = IORESOURCE_IRQ
	}
};

extern struct blocking_notifier_head usb_balong_notifier_list;
static struct notifier_block gs_otg3_nb;
#ifdef CONFIG_USB_OTG_USBID_BYGPIO
int dwc_otg2_usb_id = 1;
#endif

#ifdef CONFIG_USB_OTG_USBID_BYGPIO
void otg_set_usbid(int usb_id)
{
    dwc_otg2_usb_id = usb_id;
}

int otg_get_usbid(void)
{
    return dwc_otg2_usb_id;
}
#endif

static void print_debug_regs(struct dwc_otg2 *otg)
{
	//u32 usbcmd = otg_read(otg, 0x0000);
	//u32 usbsts = otg_read(otg, 0x0004);
	//u32 portsc = otg_read(otg, 0x400);
	u32 gctl = otg_read(otg, 0xc110);
	u32 gsts = otg_read(otg, 0xc118);
	u32 gdbgltssm = otg_read(otg, 0xc164);
	u32 gusb2phycfg0 = otg_read(otg, 0xc200);
	u32 gusb3pipectl0 = otg_read(otg, 0xc2c0);
	u32 dcfg = otg_read(otg, 0xc700);
	u32 dctl = otg_read(otg, 0xc704);
	u32 dsts = otg_read(otg, 0xc70c);
	u32 ocfg = otg_read(otg, OCFG);
	u32 octl = otg_read(otg, OCTL);
	u32 oevt = otg_read(otg, OEVT);
	u32 oevten = otg_read(otg, OEVTEN);
	u32 osts = otg_read(otg, OSTS);

	//printk(KERN_ERR "usbcmd = %08x\n", usbcmd);
	//printk(KERN_ERR "usbsts = %08x\n", usbsts);
	//printk(KERN_ERR "portsc = %08x\n", portsc);
	printk(KERN_ERR "gctl = %08x\n", gctl);
	printk(KERN_ERR "gsts = %08x\n", gsts);
	printk(KERN_ERR "gdbgltssm = %08x\n", gdbgltssm);
	printk(KERN_ERR "gusb2phycfg0 = %08x\n", gusb2phycfg0);
	printk(KERN_ERR "gusb3pipectl0 = %08x\n", gusb3pipectl0);
	printk(KERN_ERR "dcfg = %08x\n", dcfg);
	printk(KERN_ERR "dctl = %08x\n", dctl);
	printk(KERN_ERR "dsts = %08x\n", dsts);
	printk(KERN_ERR "ocfg = %08x\n", ocfg);
	printk(KERN_ERR "octl = %08x\n", octl);
	printk(KERN_ERR "oevt = %08x\n", oevt);
	printk(KERN_ERR "oevten = %08x\n", oevten);
	printk(KERN_ERR "osts = %08x\n", osts);
}

static int adp_capable(struct dwc_otg2 *otg)
{
	u32 hwparams = otg_read(otg, GHWPARAMS6);

	if (hwparams & GHWPARAMS6_ADP_SUPPORT_ENABLED)
		return 1;
	return 0;
}

static int hnp_capable(struct dwc_otg2 *otg)
{
	u32 hwparams = otg_read(otg, GHWPARAMS6);

	if (hwparams & GHWPARAMS6_HNP_SUPPORT_ENABLED)
		return 1;
	return 0;
}

static int srp_capable(struct dwc_otg2 *otg)
{
	u32 hwparams = otg_read(otg, GHWPARAMS6);

	if (hwparams & GHWPARAMS6_SRP_SUPPORT_ENABLED)
		return 1;
	return 0;
}

/* Caller must hold otg->lock */
static void wakeup_main_thread(struct dwc_otg2 *otg)
{
	if (!otg->main_thread)
		return;

	otg_dbg(otg, "\n");
	/* Tell the main thread that something has happened */
	otg->main_wakeup_needed = 1;
	wake_up_interruptible(&otg->main_wq);
}

static int sleep_main_thread_timeout(struct dwc_otg2 *otg, int msecs)
{
	signed long jiffies;
	int rc = msecs;

	if (otg->state == DWC_STATE_EXIT) {
		otg_dbg(otg, "Main thread exiting\n");
		rc = -EINTR;
		goto done;
	}

	if (signal_pending(current)) {
		otg_dbg(otg, "Main thread signal pending\n");
		rc = -EINTR;
		goto done;
	}
	if (otg->main_wakeup_needed) {
		otg_dbg(otg, "Main thread wakeup needed\n");
		rc = msecs;
		goto done;
	}

	jiffies = msecs_to_jiffies(msecs);
	rc = wait_event_freezable_timeout(otg->main_wq,
					otg->main_wakeup_needed,
					jiffies);

	if (otg->state == DWC_STATE_EXIT) {
		otg_dbg(otg, "Main thread exiting\n");
		rc = -EINTR;
		goto done;
	}

	if (rc > 0)
		rc = jiffies_to_msecs(rc);

done:
	otg->main_wakeup_needed = 0;
	return rc;
}

#define sleep_main_thread_until_condition_timeout(otg, condition, msecs) ({ \
		int __timeout = msecs;				\
		while (!(condition)) {				\
			otg_dbg(otg, "  ... sleeping for %d\n", __timeout); \
			__timeout = sleep_main_thread_timeout(otg, __timeout); \
			if (__timeout <= 0) {			\
				break;				\
			}					\
		}						\
		__timeout;					\
	})

#define sleep_main_thread_until_condition(otg, condition) ({	\
		int __rc = 0;					\
		do {						\
			__rc = sleep_main_thread_until_condition_timeout(otg, condition, 50000); \
		} while (__rc == 0);				\
		__rc;						\
	})

static int sleep_main_thread(struct dwc_otg2 *otg)
{
	int rc = 0;

	do {
		rc = sleep_main_thread_timeout(otg, 5000);
	} while (rc == 0);

	return rc;
}

static void get_events(struct dwc_otg2 *otg, u32 *otg_events, u32 *adp_events, u32 *user_events)
{
	unsigned long flags;

	spin_lock_irqsave(&otg->lock, flags);
	if (otg_events)
		*otg_events = otg->otg_events;

	if (adp_events)
		*adp_events = otg->adp_events;

	if (user_events)
		*user_events = otg->user_events;
	spin_unlock_irqrestore(&otg->lock, flags);
}

static void get_and_clear_events(struct dwc_otg2 *otg,
				u32 *otg_events, u32 *adp_events, u32 *user_events)
{
	unsigned long flags;

	spin_lock_irqsave(&otg->lock, flags);

	if (otg_events)
		*otg_events = otg->otg_events;

	if (adp_events)
		*adp_events = otg->adp_events;

	if (user_events)
		*user_events = otg->user_events;

	otg->otg_events = 0;
	otg->adp_events = 0;
	otg->user_events = 0;

	spin_unlock_irqrestore(&otg->lock, flags);
}

static int check_event(struct dwc_otg2 *otg, u32 otg_mask, u32 adp_mask, u32 user_mask)
{
	u32 otg_events = 0;
	u32 adp_events = 0;
	u32 user_events = 0;

	get_events(otg, &otg_events, &adp_events, &user_events);
	if ((otg_events & otg_mask) || (adp_events & adp_mask) || (user_events & user_mask)) {
		otg_dbg(otg, "Event occurred: "
			"otg_events=%x, otg_mask=%x, "
			"adp_events=%x, adp_mask=%x"
			"user_events=%x, user_mask=%x",
			otg_events, otg_mask, adp_events, adp_mask, user_events, user_mask);
		return 1;
	}

	return 0;
}

static int sleep_until_event(struct dwc_otg2 *otg,
			u32 otg_mask, u32 adp_mask, u32 user_mask,
			u32 *otg_events, u32 *adp_events, u32 *user_events,
			int timeout)
{
	int rc = 0;
	u32 oevten = otg_mask;
	u32 adpevten = adp_mask;

	/* Enable the events */
	if (oevten)
		otg_write(otg, OEVTEN, oevten);
	if (adp_capable(otg) && adpevten)
		otg_write(otg, ADPEVTEN, adpevten);

	/* Wait until it occurs, or timeout, or interrupt. */
	if (timeout) {
		otg_dbg(otg, "Waiting for event (timeout=%d)...\n", timeout);
		rc = sleep_main_thread_until_condition_timeout(otg,
			check_event(otg, otg_mask, adp_mask, user_mask), timeout);
	} else {
		otg_dbg(otg, "Waiting for event (no timeout)...\n");
		rc = sleep_main_thread_until_condition(otg,
				check_event(otg, otg_mask, adp_mask, user_mask));
	}

	/* Disable the events */
	otg_write(otg, OEVTEN, 0);
	otg_write(otg, ADPEVTEN, 0);

	otg_dbg(otg, "Woke up rc=%d\n", rc);
	if (rc < 0)
		goto done;

	get_and_clear_events(otg, otg_events, adp_events, user_events);

done:
	return rc;
}

static void set_capabilities(struct dwc_otg2 *otg)
{
	u32 ocfg = 0;

	otg_dbg(otg, "\n");
	if (srp_capable(otg))
		ocfg |= OCFG_SRP_CAP;

	if (hnp_capable(otg))
		ocfg |= OCFG_HNP_CAP;

	otg_write(otg, OCFG, ocfg);
}

static int handshake(struct dwc_otg2 *otg, u32 reg, u32 mask, u32 done, u32 msec)
{
	u32 result;

	otg_dbg(otg, "reg=%08x, mask=%08x, value=%08x\n", reg, mask, done);
	do {
		result = otg_read(otg, reg);
		if ((result & mask) == done)
			return 1;

		mdelay(1);
		msec -= 1;
	} while (msec > 0);

	return 0;
}

#ifdef CONFIG_USB_OTG
extern int hcd_otg_reset_port(struct usb_hcd *hcd);
extern int hcd_otg_start_host(struct usb_hcd *hcd);
extern int hcd_otg_stop_host(struct usb_hcd *hcd);
extern int hcd_otg_host_release(struct usb_hcd *hcd);
#else
int hcd_otg_reset_port(struct usb_hcd *hcd)
{
    return 0;
}
int hcd_otg_start_host(struct usb_hcd *hcd)
{
    return 0;
}
int hcd_otg_stop_host(struct usb_hcd *hcd)
{
    return 0;
}
int hcd_otg_host_release(struct usb_hcd *hcd)
{
    return 0;
}
#endif

static int reset_port(struct dwc_otg2 *otg)
{
	struct usb_hcd *hcd = NULL;

	if (otg->otg.host) {
		hcd = container_of(otg->otg.host, struct usb_hcd, self);
		return hcd_otg_reset_port(hcd);
	}
	return -ENODEV;
}

#define PERI_MODE_PERIPHERAL	1
#define PERI_MODE_HOST		0

static int set_peri_mode(struct dwc_otg2 *otg, int mode)
{
	u32 octl = 0;

	/* Set peri_mode */
	octl = otg_read(otg, OCTL);
	if (mode)
		octl |= OCTL_PERI_MODE;
	else
		octl &= (~OCTL_PERI_MODE);

	otg_write(otg, OCTL, octl);

/* TODO Workaround for OTG 3.0 core. Reverse when fixed. */
#if 0
	if (mode)
		return handshake(otg, OSTS, OSTS_PERIP_MODE, OSTS_PERIP_MODE, 100);
	else
		return handshake(otg, OSTS, OSTS_PERIP_MODE, 0, 100);
#else
	mdelay(20);
	return 1;
#endif
}

/*
static void set_sus_phy(struct dwc_otg2 *otg, int bit)
{
	u32 data = 0;

	data = otg_read(otg, GUSB2PHYCFG0);
	if (bit) {
		data |= GUSB2PHYCFG_SUS_PHY;
	} else {
		data &= ~GUSB2PHYCFG_SUS_PHY;
	}
	otg_write(otg, GUSB2PHYCFG0, data);

#if 0
	data = otg_read(otg, GUSB3PIPECTL0);
	if (bit) {
		data |= GUSB3PIPECTL_SUS_EN;
	}
	else {
		data &= ~GUSB3PIPECTL_SUS_EN;
	}
	otg_write(otg, GUSB3PIPECTL0, data);
#endif
}
*/

#ifdef CONFIG_USB_OTG_VBUS_BYCHARGER
extern int chg_set_usb_otg(void);
extern int chg_clr_usb_otg(void);
#endif
static int start_host(struct dwc_otg2 *otg)
{
	int ret = 0;
	u32 octl = 0;
	u32 osts = 0;
	int flg;
	struct usb_hcd *hcd = NULL;

	otg_dbg(otg, "\n");

	if (!otg->otg.host)
		return -ENODEV;

	if (!set_peri_mode(otg, PERI_MODE_HOST))
		otg_err(otg, "Failed to start host.");

	/* Start host driver */
	hcd = container_of(otg->otg.host, struct usb_hcd, self);
	hcd_otg_start_host(hcd);

	set_capabilities(otg);

	/* Power the port only for A-host */
	if (otg->state == DWC_STATE_A_HOST) {

		/* Spin osts xhciPrtPwr bit until it becomes 1 */
		osts = otg_read(otg, OSTS);
		flg = handshake(otg, OSTS,
				OSTS_XHCI_PRT_PWR,
				OSTS_XHCI_PRT_PWR,
				1000);
		if (flg) {
			otg_dbg(otg, "Port is powered by xhci-hcd\n");

#ifdef CONFIG_USB_DWC3_VBUS_DISCONNECT
            /* vbusvldext and vbusvldextsel */
            hi_syssc_usb_vbusvldext(1);

            /* override_en and override_value*/
            hi_syssc_usb_override(1);
#endif

#ifdef CONFIG_USB_OTG_VBUS_BYCHARGER
			chg_set_usb_otg();
#endif

			/* Set port power control bit */
			octl = otg_read(otg, OCTL);
			octl |= OCTL_PRT_PWR_CTL;
			otg_write(otg, OCTL, octl);

			octl = otg_read(otg, OCFG);
			octl |= OCFG_OTG_PRTPWRCUTOFF;
			otg_write(otg, OCFG, octl);
		} else {
			otg_dbg(otg, "Port is not powered by xhci-hcd\n");
		}
	}
	return ret;
}

static int stop_host(struct dwc_otg2 *otg)
{
	int ret = -1;
	u32 value = 0;
	struct usb_hcd *hcd = NULL;

	otg_dbg(otg, "\n");

	if (otg->otg.host) {
		hcd = container_of(otg->otg.host, struct usb_hcd, self);
		ret = hcd_otg_stop_host(hcd);

#ifdef CONFIG_USB_DWC3_VBUS_DISCONNECT
        /* vbusvldext and vbusvldextsel */
        hi_syssc_usb_vbusvldext(0);

        /* override_en and override_value*/
        hi_syssc_usb_override(0);
#endif

#ifdef CONFIG_USB_OTG_VBUS_BYCHARGER
		chg_clr_usb_otg();
#endif

		/* Set port power control bit */
		value = otg_read(otg, OCTL);
		value &= ~OCTL_PRT_PWR_CTL;
		otg_write(otg, OCTL, value);
	}

	return ret;
}

/* Sends the host release set feature request */
static void host_release(struct dwc_otg2 *otg)
{
	struct usb_hcd *hcd = NULL;

	otg_dbg(otg, "\n");

	if (otg->otg.host) {
		hcd = container_of(otg->otg.host, struct usb_hcd, self);
		hcd_otg_host_release(hcd);
	}
}

extern int pcd_otg_start_peripheral(struct usb_gadget *gadget);
extern int pcd_otg_stop_peripheral(struct usb_gadget *gadget);

static void start_peripheral(struct dwc_otg2 *otg)
{
	struct usb_gadget *gadget;

	otg_dbg(otg, "\n");

	gadget = otg->otg.gadget;
	if (!gadget)
		return;

	if (!set_peri_mode(otg, PERI_MODE_PERIPHERAL))
		otg_err(otg, "Failed to start peripheral.");

#ifdef CONFIG_USB_DWC3_VBUS_DISCONNECT
    /* vbusvldext and vbusvldextsel */
    hi_syssc_usb_vbusvldext(1);

    /* override_en and override_value*/
    hi_syssc_usb_override(1);
#endif

	pcd_otg_start_peripheral(gadget);
}

static void stop_peripheral(struct dwc_otg2 *otg)
{
	struct usb_gadget *gadget = otg->otg.gadget;

	if (!gadget)
		return;

	otg_dbg(otg, "\n");

#ifdef CONFIG_USB_DWC3_VBUS_DISCONNECT
    /* vbusvldext and vbusvldextsel */
    hi_syssc_usb_vbusvldext(0);

    /* override_en and override_value*/
    hi_syssc_usb_override(0);
#endif
    
	pcd_otg_stop_peripheral(gadget);
}

static void send_hrr_init(struct dwc_otg2 *otg)
{
	struct usb_gadget *gadget = otg->otg.gadget;
	if (!gadget)
		return;

	otg_dbg(otg, "\n");
	gadget->ops->send_hrr(gadget, 1);
}

static void send_hrr_confirm(struct dwc_otg2 *otg)
{
	struct usb_gadget *gadget = otg->otg.gadget;
	if (!gadget)
		return;

	otg_dbg(otg, "\n");
	gadget->ops->send_hrr(gadget, 0);
}

static void set_b_host(struct dwc_otg2 *otg, int val)
{
	otg->otg.host->is_b_host = val;
}

static void reset_adp(struct dwc_otg2 *otg)
{
	int rc;
	u32 events = 0;
	u32 adpcfg = 0;

	otg_dbg(otg, "\n");

	/* Reset ADP related registers */
	otg_write(otg, ADPCFG, 0);
	otg_write(otg, ADPCTL, 0);
	otg_write(otg, ADPEVTEN, 0);

	if (!adp_capable(otg))
		return;

	/* Reset ADP controller */
	otg_write(otg, ADPEVTEN, ADPEVTEN_ADP_RST_CMPLT_EVNT_EN);
	otg_write(otg, ADPCTL, ADPCTL_ADP_RES);

	rc = sleep_until_event(otg,
			0, ADPEVT_ADP_RST_CMPLT_EVNT, 0,
			NULL, &events, NULL, 500);
	if (!(events & ADPEVTEN_ADP_RST_CMPLT_EVNT_EN)) {
		otg_err(otg, "ADP Reset Failed!!\n");
		goto done;
	}

	otg_dbg(otg, "ADP Reset Success\n");

	/* TODO this value should set the period to 2.275s but I'm seeing just 0.56s period. */
	adpcfg = (0x2 << ADPCFG_PRB_PER_SHIFT);
	otg_write(otg, ADPCFG, adpcfg);

	rc = handshake(otg, ADPCTL, ADPCTL_WB, 0, 100);
	if (!rc) {
		otg_err(otg, "Could not write ADPCFG values\n");
		goto done;
	}

done:
	otg_dbg(otg, "ADPCFG initialization done!\n");
	otg_write(otg, ADPEVTEN, 0);
}

static int init_b_device(struct dwc_otg2 *otg)
{
	otg_dbg(otg, "\n");
	set_capabilities(otg);

	if (!set_peri_mode(otg, PERI_MODE_PERIPHERAL))
		otg_err(otg, "Failed to start peripheral.");

	/* otg->user_events |= INITIAL_SRP; */
	return DWC_STATE_B_PERIPHERAL;
}

static int init_a_device(struct dwc_otg2 *otg)
{
	otg_write(otg, OCFG, 0);
	otg_write(otg, OCTL, 0);

	return DWC_STATE_A_HOST;
}

static otg_state_t do_connector_id_status(struct dwc_otg2 *otg)
{
	otg_state_t state;
	u32 osts = 0;

	otg_dbg(otg, "\n");

	reset_adp(otg);

	otg_write(otg, OCFG, 0);
	otg_write(otg, OEVTEN, 0);
	otg_write(otg, OEVT, 0xffffffff);
	otg_write(otg, OEVTEN, OEVT_CONN_ID_STS_CHNG_EVNT);
	otg_write(otg, OCTL, OCTL_PERI_MODE);
	mdelay(60);

	osts = otg_read(otg, OSTS);
	if (!(osts & OSTS_CONN_ID_STS)) {
		otg_dbg(otg, "Connector ID is A\n");
		state = init_a_device(otg);
	} else {
		otg_dbg(otg, "Connector ID is B\n");
		state = init_b_device(otg);
	}

	/** TODO: This is a workaround for latest hibernation-enabled bitfiles
	 * which have problems before initializing SRP. */
	mdelay(50);
	return state;
}

static void reset_hw(struct dwc_otg2 *otg)
{
	u32 gctl = 0;
	otg_dbg(otg, "\n");
	otg_write(otg, OEVTEN, 0);
	otg_write(otg, OCTL, 0);
	gctl = otg_read(otg, GCTL);
	gctl |= GCTL_PRT_CAP_DIR_OTG << GCTL_PRT_CAP_DIR_SHIFT;
	if (otg_params.hibernate)
		gctl |= GCTL_GBL_HIBERNATION_EN;
	otg_write(otg, GCTL, gctl);
}

static void start_adp_probe(struct dwc_otg2 *otg)
{
	u32 adpevten = ADPEVTEN_ADP_PRB_EVNT_EN |
		ADPEVTEN_ADP_TMOUT_EVNT_EN;

	if (!adp_capable(otg))
		return;

	otg_write(otg, ADPEVTEN, adpevten);
	otg_write(otg, ADPCTL, ADPCTL_ENA_PRB | ADPCTL_ADP_EN);
	handshake(otg, ADPCTL, ADPCTL_WB, 0, 100);
}

static void stop_adp_probe(struct dwc_otg2 *otg)
{
	if (!adp_capable(otg))
		return;

	otg_write(otg, ADPEVTEN, 0);
	otg_write(otg, ADPCTL, 0);
	handshake(otg, ADPCTL, ADPCTL_WB, 0, 100);
}

static void start_adp_sense(struct dwc_otg2 *otg)
{
	u32 adpevten = ADPEVTEN_ADP_SNS_EVNT_EN;

	if (!adp_capable(otg))
		return;

	otg_write(otg, ADPEVTEN, adpevten);
	otg_write(otg, ADPCTL, ADPCTL_ENA_SNS | ADPCTL_ADP_EN);
	handshake(otg, ADPCTL, ADPCTL_WB, 0, 100);
}

static void stop_adp_sense(struct dwc_otg2 *otg)
{
	if (!adp_capable(otg))
		return;

	otg_write(otg, ADPEVTEN, 0);
	otg_write(otg, ADPCTL, 0);
	handshake(otg, ADPCTL, ADPCTL_WB, 0, 100);
}

#define ADP_RAMP_TIME_CHANGE_THRESHOLD	5
#define ADP_PROBE_TIMEOUT		5000
#define SRP_TIMEOUT			6000

static void start_srp(struct dwc_otg2 *otg)
{
	u32 octl = 0;

	octl = otg_read(otg, OCTL);
	octl |= OCTL_SES_REQ;
	otg_write(otg, OCTL, octl);
}

static void start_b_hnp(struct dwc_otg2 *otg)
{
	u32 octl = 0;

	octl = otg_read(otg, OCTL);
	octl |= OCTL_HNP_REQ | OCTL_DEV_SET_HNP_EN;
	otg_write(otg, OCTL, octl);
}

static void stop_b_hnp(struct dwc_otg2 *otg)
{
	u32 octl = 0;

	octl = otg_read(otg, OCTL);
	octl &= ~(OCTL_HNP_REQ | OCTL_DEV_SET_HNP_EN);
	otg_write(otg, OCTL, octl);
}

static void start_a_hnp(struct dwc_otg2 *otg)
{
	u32 octl = 0;

	octl = otg_read(otg, OCTL);
	octl |= OCTL_HST_SET_HNP_EN;
	otg_write(otg, OCTL, octl);
}

static void stop_a_hnp(struct dwc_otg2 *otg)
{
	u32 octl = 0;

	octl = otg_read(otg, OCTL);
	octl &= ~(OCTL_HST_SET_HNP_EN);
	otg_write(otg, OCTL, octl);
}

#define RSP_CONFIRM_TIMEOUT 200

static otg_state_t do_a_host_rsp_confirm(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 otg_events = 0;

	otg_dbg(otg, "");
	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_A_DEV_SESS_END_DET_EVNT |
		OEVT_HOST_ROLE_REQ_CONFIRM_EVNT;

	host_release(otg);
	rc = RSP_CONFIRM_TIMEOUT;
again:

	rc = sleep_until_event(otg,
			otg_mask, 0, 0,
			&otg_events, NULL, NULL, rc);
	if (rc < 0)
		return DWC_STATE_EXIT;

	/* Higher priority first */
	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;

	} else if (otg_events & OEVT_A_DEV_SESS_END_DET_EVNT) {
		otg_dbg(otg, "OEVT_A_DEV_SESS_END_DET_EVNT\n");
		return DWC_STATE_A_PROBE;

	} else if (otg_events & OEVT_HOST_ROLE_REQ_CONFIRM_EVNT) {
		u32 octl;
		otg_dbg(otg, "OEVT_HOST_ROLE_REQ_CONFIRM_EVNT\n");
		octl = otg_read(otg, OCTL);
		otg_write(otg, OCTL, octl | OCTL_HST_SET_HNP_EN);
		return DWC_STATE_A_PERIPHERAL;

	} else if (rc == 0) {
		otg_dbg(otg, "RSP confirm timed out\n");
		return DWC_STATE_A_HOST;
	} else {
		goto again;
	}
}

static otg_state_t do_a_host(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_events = 0;
	u32 user_events = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;

	set_b_host(otg, 0);
	start_host(otg);

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_A_DEV_SESS_END_DET_EVNT |
		OEVT_HOST_ROLE_REQ_INIT_EVNT;

	user_mask = USER_HNP_EVENT | USER_EXIT_EVENT;

again:
	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, 0);
	if (rc < 0)
		return DWC_STATE_EXIT;

    if(user_events & USER_EXIT_EVENT){
	    otg_dbg(otg, "USER_EXIT_EVENT\n");
        return DWC_STATE_EXIT;
	}

	/* Higher priority first */
	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;

	} else if (otg_events & OEVT_A_DEV_SESS_END_DET_EVNT) {
		otg_dbg(otg, "OEVT_A_DEV_SESS_END_DET_EVNT\n");
		return DWC_STATE_A_PROBE;

	} else if (otg_events & OEVT_HOST_ROLE_REQ_INIT_EVNT) {
		otg_state_t next;
		otg_dbg(otg, "OEVT_HOST_ROLE_REQ_INIT_EVNT\n");

		/* TODO check initiate or confirm */
		next = do_a_host_rsp_confirm(otg);
		if (next == DWC_STATE_A_HOST)
			goto again;

		return next;

	} else if (user_events & USER_HNP_EVENT) {
		otg_dbg(otg, "USER_HNP_EVENT\n");
		return DWC_STATE_A_HNP_INIT;
	}

	/* Invalid state */
	return DWC_STATE_INVALID;
}

static otg_state_t do_a_hnp_init(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_events = 0;
	u32 otg_mask = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
			OEVT_A_DEV_HNP_CHNG_EVNT;

	start_a_hnp(otg);
	rc = 5000;
again:
	rc = sleep_until_event(otg,
			otg_mask, 0, 0,
			&otg_events, NULL, NULL, rc);
	stop_a_hnp(otg);
	if (rc < 0)
		return DWC_STATE_EXIT;

	/* Higher priority first */
	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_A_DEV_HNP_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_A_DEV_HNP_CHNG_EVNT\n");
		if (otg_events & OEVT_HST_NEG_SCS) {
			otg_dbg(otg, "A-HNP Success\n");
			return DWC_STATE_A_PERIPHERAL;
		} else {
			otg_dbg(otg, "A-HNP Failed\n");
			return DWC_STATE_A_PROBE;
		}
	} else if (rc == 0) {
		otg_dbg(otg, "A-HNP Failed (Timed out)\n");
		return DWC_STATE_A_PROBE;
	} else {
		goto again;
	}
}

static otg_state_t do_a_probe(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 adp_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 adp_events = 0;
	u32 user_events = 0;
	int count = 0;
	int previous_ramp_time = 0x7ff;
	int ramp_time = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
			OEVT_A_DEV_SRP_DET_EVNT;
	adp_mask = ADPEVT_ADP_PRB_EVNT | ADPEVT_ADP_TMOUT_EVNT;
	user_mask = USER_SRP_EVENT;

again2:
	start_adp_probe(otg);

again:
	rc = sleep_until_event(otg,
			otg_mask, adp_mask, user_mask,
			&otg_events, &adp_events, &user_events,
			ADP_PROBE_TIMEOUT);

	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_A_DEV_SRP_DET_EVNT) {
		otg_dbg(otg, "OEVT_A_DEV_SRP_DET_EVNT\n");
		return DWC_STATE_A_HOST;
	} else if (adp_events & ADPEVT_ADP_PRB_EVNT) {
		ramp_time = (adp_events & ADPEVT_RTIM_EVNTS) >> ADPEVT_RTIM_EVNT_SHIFT;
		otg_dbg(otg, "ADP ramp_time=%d, prev=%d, count=%d",
			ramp_time, previous_ramp_time, count);
		if (count < 3)
			previous_ramp_time = ramp_time;
		count ++;

		if (abs(ramp_time - previous_ramp_time) > ADP_RAMP_TIME_CHANGE_THRESHOLD) {
			otg_dbg(otg, "ADP ramp time changed\n");
			return DWC_STATE_INIT;
		} else {
			otg_dbg(otg, "ADP ramp time not changed\n");
			previous_ramp_time = ramp_time;
			goto again;
		}
	} else if (adp_events & ADPEVT_ADP_TMOUT_EVNT) {
		count = 0;
		goto again;
	} else if (user_events & USER_SRP_EVENT) {
		otg_dbg(otg, "User initiated VBUS\n");
		return DWC_STATE_A_HOST;
	} else if (rc == 0) {
		/* Don't receive any probe event within 3 seconds */
		if (adp_capable(otg))
			otg_err(otg, "Probe interrupt not received within 3 seconds, trying to restart probing.\n");

		goto again2;
	}

	return DWC_STATE_INVALID;
}

#define RSP_WRST_TIMEOUT 2000
#define RSP_ACK_TIMEOUT 2000

static int do_a_peripheral_rsp_confirm(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 user_events = 0;
	u32 octl = 0;

	otg_dbg(otg, "");
	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_A_DEV_B_DEV_HOST_END_EVNT;

	user_mask = USER_END_SESSION;

	/* Send the host role request (confirm) DN */
	mdelay(5);
	send_hrr_confirm(otg);

	/* Set HNP/RSP Enable */
	octl = otg_read(otg, OCTL);
	octl |= OCTL_HST_SET_HNP_EN;
	otg_write(otg, OCTL, octl);

	rc = RSP_WRST_TIMEOUT;
again:

	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, rc);

	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_A_DEV_B_DEV_HOST_END_EVNT) {
		otg_dbg(otg, "OEVT_A_DEV_B_DEV_HOST_END_EVNT\n");
		return DWC_STATE_A_HOST;
	} else if (rc == 0) {
		otg_dbg(otg, "RSP timed out\n");
		return DWC_STATE_A_PERIPHERAL;
	} else {
		goto again;
	}
}

static int do_a_peripheral_rsp_init(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 user_events = 0;

	otg_dbg(otg, "");
	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT;

	user_mask = PCD_RECEIVED_HOST_RELEASE_EVENT;

	/* Send the host role request (init) DN */
	send_hrr_init(otg);
	rc = RSP_ACK_TIMEOUT;
again:

	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, rc);
	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (rc == 0) {
		otg_dbg(otg, "RSP timed out\n");
		return DWC_STATE_A_PERIPHERAL;
	} else if (user_events & PCD_RECEIVED_HOST_RELEASE_EVENT) {
		otg_dbg(otg, "PCD_RECEIVED_HOST_RELEASE_EVENT\n");
		return do_a_peripheral_rsp_confirm(otg);
	} else {
		goto again;
	}
}

static int do_a_peripheral(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 events = 0;
	u32 user_events = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_A_DEV_B_DEV_HOST_END_EVNT;

	user_mask = USER_RSP_EVENT;

again:
	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&events, NULL, &user_events, 0);
	if (rc < 0)
		return DWC_STATE_EXIT;

	if (events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (events & OEVT_A_DEV_B_DEV_HOST_END_EVNT) {
		otg_dbg(otg, "OEVT_A_DEV_B_DEV_HOST_END_EVNT\n");
		return DWC_STATE_A_HOST;
	} else if (user_events & USER_RSP_EVENT) {
		/* Initiate reverse role swap */
		otg_state_t next;
		otg_dbg(otg, "USER_RSP_EVENT\n");
		next = do_a_peripheral_rsp_init(otg);
		if (next == DWC_STATE_A_PERIPHERAL)
			goto again;

		return next;
	}

	return DWC_STATE_INVALID;
}

static int do_b_peripheral_rsp_confirm(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 user_events = 0;
	u32 octl = 0;

	otg_dbg(otg, "");
	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT |
		OEVT_B_DEV_HNP_CHNG_EVNT;

	user_mask = USER_END_SESSION;

	/* Send the host role request (confirm) DN */
	mdelay(5);
	send_hrr_confirm(otg);

	/* Set HNP/RSP Enable */
	octl = otg_read(otg, OCTL);
	octl |= OCTL_DEV_SET_HNP_EN;
	otg_write(otg, OCTL, octl);

	rc = RSP_WRST_TIMEOUT;
again:

	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, rc);

	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_B_DEV_VBUS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_VBUS_CHNG_EVNT\n");
		return DWC_STATE_B_SENSE;
	} else if (otg_events & OEVT_B_DEV_HNP_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_HNP_CHNG_EVNT\n");
		return DWC_STATE_B_HOST;
	} else if (rc == 0) {
		otg_dbg(otg, "RSP timed out\n");
		return DWC_STATE_B_PERIPHERAL;
	} else {
		goto again;
	}
}

static int do_b_peripheral_rsp_init(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 user_events = 0;

	otg_dbg(otg, "");
	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT;

	user_mask = USER_END_SESSION | PCD_RECEIVED_HOST_RELEASE_EVENT;

	/* Send the host role request (init) DN */
	send_hrr_init(otg);
	rc = RSP_ACK_TIMEOUT;
again:

	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, rc);
	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_B_DEV_VBUS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_VBUS_CHNG_EVNT\n");
		return DWC_STATE_B_SENSE;
	} else if (user_events & USER_END_SESSION) {
		otg_dbg(otg, "USER_END_SESSION\n");
		return DWC_STATE_B_SENSE;
	} else if (rc == 0) {
		otg_dbg(otg, "RSP timed out\n");
		return DWC_STATE_B_PERIPHERAL;
	} else if (user_events & PCD_RECEIVED_HOST_RELEASE_EVENT) {
		otg_dbg(otg, "PCD_RECEIVED_HOST_RELEASE_EVENT\n");
		return do_b_peripheral_rsp_confirm(otg);
	} else {
		goto again;
	}
}

static int do_b_srp_init(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 events = 0;
	u32 otg_mask = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_SES_VLD_DET_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT;

	otg_write(otg, OEVTEN, otg_mask);
	start_srp(otg);

	if (!otg_params.hibernate)
		rc = SRP_TIMEOUT;
	else
		rc = 500;

again:
	rc = sleep_until_event(otg,
			otg_mask, 0, 0,
			&events, NULL, NULL, rc);
	if (rc < 0)
		return DWC_STATE_INVALID;

	if (events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (events & OEVT_B_DEV_SES_VLD_DET_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_SES_VLD_DET_EVNT\n");
		return DWC_STATE_B_PERIPHERAL;
	} else if (rc == 0) {
		otg_dbg(otg, "SRP Timeout (rc=%d)\n", rc);

		if (otg_params.hibernate)
			return DWC_STATE_B_PERIPHERAL;

		return DWC_STATE_B_SENSE;
	} else {
		goto again;
	}
}

static int do_b_peripheral(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 user_events = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT;

	user_mask = USER_HNP_EVENT | USER_RSP_EVENT | USER_END_SESSION | INITIAL_SRP | USER_EXIT_EVENT;

again:
	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, 0);
	if (rc < 0)
		return DWC_STATE_EXIT;

    if(user_events & USER_EXIT_EVENT){
		otg_dbg(otg, "USER_EXIT_EVENT\n");
	    return DWC_STATE_EXIT;
	}

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_B_DEV_VBUS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_VBUS_CHNG_EVNT\n");

		/* TODO Until ADP supported in hibernate */
		if (otg_params.hibernate)
			goto again;

		if (!(otg_events & OEVT_B_SES_VLD_EVT))
			return DWC_STATE_B_SENSE;
		else
			goto again;

	} else if (user_events & USER_HNP_EVENT) {
		otg_dbg(otg, "USER_HNP_EVENT\n");
		/* TODO make it a substate here like RSP */
		return DWC_STATE_B_HNP_INIT;
	} else if (user_events & USER_END_SESSION) {
		otg_dbg(otg, "USER_END_SESSION\n");
		return DWC_STATE_B_SENSE;
	} else if (user_events & USER_RSP_EVENT) {
		/* Initiate role swap */
		otg_state_t next;
		otg_dbg(otg, "USER_RSP_EVENT\n");
		next = do_b_peripheral_rsp_init(otg);
		if (next == DWC_STATE_B_PERIPHERAL)
			goto again;

		return next;
	} else if (user_events & INITIAL_SRP) {
		otg_state_t next;
		if (otg_params.hibernate)
			goto again;

		otg_dbg(otg, "INITIAL_SRP\n");
		next = do_b_srp_init(otg);
		if (next == DWC_STATE_B_PERIPHERAL)
			goto again;

		return next;
	}

	return DWC_STATE_INVALID;
}

static otg_state_t do_b_host_rsp_confirm(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 otg_events = 0;

	otg_dbg(otg, "");
	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_HOST_ROLE_REQ_CONFIRM_EVNT;

	host_release(otg);
	rc = RSP_CONFIRM_TIMEOUT;
again:

	rc = sleep_until_event(otg,
			otg_mask, 0, 0,
			&otg_events, NULL, NULL, rc);
	if (rc < 0)
		return DWC_STATE_EXIT;

	/* Higher priority first */
	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;

	} else if (otg_events & OEVT_HOST_ROLE_REQ_CONFIRM_EVNT) {
		u32 octl;
		otg_dbg(otg, "OEVT_HOST_ROLE_REQ_CONFIRM_EVNT\n");
		octl = otg_read(otg, OCTL);
		otg_write(otg, OCTL, octl | OCTL_DEV_SET_HNP_EN);
		return DWC_STATE_B_PERIPHERAL;

	} else if (rc == 0) {
		otg_dbg(otg, "RSP confirm timed out\n");
		return DWC_STATE_B_HOST;
	} else {
		goto again;
	}
}

static int do_b_host(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_events = 0;
	u32 user_events = 0;
	u32 otg_mask = 0;
	u32 user_mask = 0;

	set_b_host(otg, 1);
	start_host(otg);
	reset_port(otg);

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_B_HOST_END_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT |
		OEVT_HOST_ROLE_REQ_INIT_EVNT;

again:
	rc = sleep_until_event(otg,
			otg_mask, 0, user_mask,
			&otg_events, NULL, &user_events, 0);
	if (rc < 0)
		return DWC_STATE_EXIT;

	/* Higher priority first */
	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (otg_events & OEVT_B_DEV_B_HOST_END_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_B_HOST_END_EVNT\n");
		return DWC_STATE_B_PERIPHERAL;
	} else if (otg_events & OEVT_B_DEV_VBUS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_VBUS_CHNG_EVNT\n");
		if (otg_events & OEVT_B_SES_VLD_EVT) {
			otg_dbg(otg, "Session valid\n");
			goto again;
		} else {
			otg_dbg(otg, "Session not valid\n");
			return DWC_STATE_B_SENSE;
		}
	} else if (otg_events & OEVT_HOST_ROLE_REQ_INIT_EVNT) {
		otg_state_t next;
		otg_dbg(otg, "OEVT_HOST_ROLE_REQ_INIT_EVNT\n");

		/* TODO check initiate or confirm */
		next = do_b_host_rsp_confirm(otg);
		if (next == DWC_STATE_B_HOST)
			goto again;

		return next;
	} else if (user_events & USER_RSP_EVENT) {
		/* Initiate reverse role swap */
	}

	/* Invalid state */
	return DWC_STATE_INVALID;
}

static int dwc_otg2_start_srp(struct usb_otg *x);

static int do_b_sense(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 adp_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 adp_events = 0;
	u32 user_events = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_SES_VLD_DET_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT;

	adp_mask = ADPEVT_ADP_SNS_EVNT;
	user_mask = USER_SRP_EVENT;
again:
	start_adp_sense(otg);
	rc = sleep_until_event(otg,
			otg_mask, adp_mask, user_mask,
			&otg_events, &adp_events, &user_events,
			ADP_PROBE_TIMEOUT);

	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if ((otg_events & OEVT_B_DEV_VBUS_CHNG_EVNT) ||
		(otg_events & OEVT_B_DEV_SES_VLD_DET_EVNT)) {
		otg_dbg(otg, "OEVT_B_DEV_VBUS_CHNG_EVNT\n");
		if (otg_events & OEVT_B_SES_VLD_EVT) {
			otg_dbg(otg, "Session valid\n");
			return DWC_STATE_B_PERIPHERAL;
		} else {
			otg_dbg(otg, "Session not valid\n");
			goto again;
		}
	} else if (user_events & USER_SRP_EVENT) {
		otg_dbg(otg, "USER_SRP_EVENT\n");
		dwc_otg2_start_srp(&otg->otg);
		return DWC_STATE_B_PERIPHERAL;
	} else if (adp_events & ADPEVT_ADP_SNS_EVNT) {
		/* Keep sensing until we don't sense host */
		otg_dbg(otg, "ADPEVT_ADP_SNS_EVNT\n");
		goto again;
	} else if (rc == 0) {
		/* Sense timeout */
		return DWC_STATE_B_PROBE;
	}
	return DWC_STATE_INVALID;
}

static int do_b_probe(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 otg_mask = 0;
	u32 adp_mask = 0;
	u32 user_mask = 0;
	u32 otg_events = 0;
	u32 adp_events = 0;
	u32 user_events = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_SES_VLD_DET_EVNT |
		OEVT_B_DEV_VBUS_CHNG_EVNT;

	user_mask = USER_SRP_EVENT;

	/* TODO add probing/timeout */
again:
	rc = sleep_until_event(otg,
			otg_mask, adp_mask, user_mask,
			&otg_events, &adp_events, &user_events, 0);

	if (rc < 0)
		return DWC_STATE_EXIT;

	if (otg_events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if ((otg_events & OEVT_B_DEV_VBUS_CHNG_EVNT) ||
		(otg_events & OEVT_B_DEV_SES_VLD_DET_EVNT)) {
		otg_dbg(otg, "OEVT_B_DEV_VBUS_CHNG_EVNT\n");
		if (otg_events & OEVT_B_SES_VLD_EVT) {
			otg_dbg(otg, "Session valid\n");
			return DWC_STATE_B_PERIPHERAL;
		} else {
			otg_dbg(otg, "Session not valid\n");
			goto again;
		}
	}
	else if (user_events & USER_SRP_EVENT) {
		otg_dbg(otg, "USER_SRP_EVENT\n");
		dwc_otg2_start_srp(&otg->otg);
		return DWC_STATE_B_PERIPHERAL;
	}

	return DWC_STATE_INVALID;
}

#define HNP_TIMEOUT	4000

static int do_b_hnp_init(struct dwc_otg2 *otg)
{
	int rc = 0;
	u32 events = 0;
	u32 otg_mask = 0;

	otg_mask = OEVT_CONN_ID_STS_CHNG_EVNT |
		OEVT_B_DEV_HNP_CHNG_EVNT;

	start_b_hnp(otg);
	rc = HNP_TIMEOUT;
again:
	rc = sleep_until_event(otg,
			otg_mask, 0, 0,
			&events, NULL, NULL, rc);
	stop_b_hnp(otg);

	if (rc < 0)
		return DWC_STATE_INVALID;

	if (events & OEVT_CONN_ID_STS_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_CONN_ID_STS_CHNG_EVNT\n");
		return DWC_STATE_INIT;
	} else if (events & OEVT_B_DEV_HNP_CHNG_EVNT) {
		otg_dbg(otg, "OEVT_B_DEV_HNP_CHNG_EVNT\n");
		if (events & OEVT_HST_NEG_SCS) {
			otg_dbg(otg, "B-HNP Success\n");
			return DWC_STATE_B_HOST;
		} else {
			otg_err(otg, "B-HNP Failed\n");
			return DWC_STATE_B_PERIPHERAL;
		}
	} else if (rc == 0) {
		/* Timeout */
		otg_err(otg, "HNP timed out!\n");
		return DWC_STATE_B_PERIPHERAL;
	} else {
		goto again;
	}
}

int otg_main_thread(void *data)
{
	struct dwc_otg2 *otg = (struct dwc_otg2 *)data;
#ifdef DEBUG
	u32 snpsid = otg_read(otg, 0xc120);
	otg_dbg(otg, "io_priv=%p\n", otg->otg.phy->io_priv);
	otg_dbg(otg, "c120: %x\n", snpsid);
#endif

	/* Allow the thread to be killed by a signal, but set the signal mask
	 * to block everything but INT, TERM, KILL, and USR1. */
	allow_signal(SIGINT);
	allow_signal(SIGTERM);
	allow_signal(SIGKILL);
	allow_signal(SIGUSR1);

	/* Allow the thread to be frozen */
	set_freezable();
	reset_hw(otg);

	otg_dbg(otg, "Thread running\n");
	while (otg->state != DWC_STATE_TERMINATED) {
		int next = DWC_STATE_INIT;
		otg_dbg(otg, "\n\n\nMain thread entering state\n");

        if(otg->thread_cancel){
            otg_dbg(otg, "Abort DWC_STATE_INIT\n");
            otg->state = DWC_STATE_EXIT;
        }

		switch (otg->state) {
		case DWC_STATE_INIT:
			otg_dbg(otg, "DWC_STATE_INIT\n");
			next = do_connector_id_status(otg);
			break;

		case DWC_STATE_A_PROBE:
			otg_dbg(otg, "DWC_STATE_A_PROBE\n");
			next = do_a_probe(otg);
			stop_adp_probe(otg);
			break;

		case DWC_STATE_A_HOST:
			otg_dbg(otg, "DWC_STATE_A_HOST\n");
			next = do_a_host(otg);

			/* Don't stop the host here if we are going into
			 * A_HNP_INIT.  We need to delay that until later.  It
			 * will be stopped when coming out of A_HNP_INIT
			 * state. */
			if (next != DWC_STATE_A_HNP_INIT)
				stop_host(otg);
			break;

		case DWC_STATE_A_HNP_INIT:
			otg_dbg(otg, "DWC_STATE_A_HNP_INIT\n");
			next = do_a_hnp_init(otg);

			/* Stop the host. */
			stop_host(otg);
			break;

		case DWC_STATE_A_PERIPHERAL:
			otg_dbg(otg, "DWC_STATE_A_PERIPHERAL\n");
			start_peripheral(otg);
			next = do_a_peripheral(otg);
			stop_peripheral(otg);
			break;

		case DWC_STATE_B_PERIPHERAL:
			otg_dbg(otg, "DWC_STATE_B_PERIPHERAL\n");
			start_peripheral(otg);
			next = do_b_peripheral(otg);

			/* TODO HNP/RSP make a sub-state inside B-peripheral */
			/* Continue b-peripheral if we're going into HNP or RSP
			 * init state, otherwise stop b-peripheral if we go into
			 * any other state. */
			if ((next == DWC_STATE_B_HNP_INIT) ||
			    (next == DWC_STATE_B_RSP_INIT))
				otg_dbg(otg, "Continuing b_peripheral for role swap\n");
			else
				stop_peripheral(otg);			
			break;

		case DWC_STATE_B_HNP_INIT:
			otg_dbg(otg, "DWC_STATE_B_HNP_INIT\n");
			next = do_b_hnp_init(otg);

			/* If hnp succeeds, or we go into any other state other
			 * than b-peripheral, stop the b-peripheral */
			if (next != DWC_STATE_B_PERIPHERAL)
				stop_peripheral(otg);
			break;

		case DWC_STATE_B_SENSE:
			otg_dbg(otg, "DWC_STATE_B_SENSE\n");
			next = do_b_sense(otg);
			stop_adp_sense(otg);
			break;

		case DWC_STATE_B_PROBE:
			otg_dbg(otg, "DWC_STATE_B_PROBE\n");
			next = do_b_probe(otg);
			stop_adp_probe(otg);
			break;

		case DWC_STATE_B_HOST:
			otg_dbg(otg, "DWC_STATE_B_HOST\n");
			next = do_b_host(otg);
			stop_host(otg);
			break;

		case DWC_STATE_EXIT:
			otg_dbg(otg, "DWC_STATE_EXIT\n");
			reset_hw(otg);
			next = DWC_STATE_TERMINATED;
            otg->thread_cancel = 0;
            wake_up_interruptible(&otg->exit_wq);
			break;

		case DWC_STATE_INVALID:
			otg_dbg(otg, "DWC_STATE_INVALID!!!\n");
		default:
			otg_dbg(otg, "Unknown State %d, sleeping...\n", otg->state);
			sleep_main_thread(otg);
			break;
		}

		otg->prev = otg->state;
		otg->state = next;
	}

	otg->main_thread = NULL;
	otg_dbg(otg, "OTG main thread exiting....\n");

	return 0;
}

static void start_main_thread(struct dwc_otg2 *otg)
{
	mutex_lock(&lock);
	if (!otg->main_thread && otg->otg.gadget && otg->otg.host) {
        otg_dbg(otg, "Starting OTG main thread\n");
        if(otg->thread_cancel){
            otg_dbg(otg, "Cancel OTG main thread\n");
            otg->thread_cancel = 0;
            wake_up_interruptible(&otg->exit_wq);
            mutex_unlock(&lock);
            return ;
        }

		otg->main_thread = kthread_create(otg_main_thread, otg, "otg");
		wake_up_process(otg->main_thread);
	}
	mutex_unlock(&lock);
}

static void stop_main_thread(struct dwc_otg2 *otg)
{
	mutex_lock(&lock);
	if (otg->main_thread) {
		otg_dbg(otg, "Stopping OTG main thread\n");
		otg->state = DWC_STATE_EXIT;
		wakeup_main_thread(otg);
	}
	mutex_unlock(&lock);
}

void dwc_otg2_register(void)
{
	mutex_lock(&lock);
	if (!pd) {
		pd = platform_device_register_simple("dwc_otg2", -1, res, 
				ARRAY_SIZE(res));
		if (!pd) {
			printk(KERN_ERR "Unable to register dwc_otg2 transceiver\n");
		}
	}
	mutex_unlock(&lock);
}

void dwc_otg2_unregister(void)
{
	mutex_lock(&lock);
	if (pd) {
		platform_device_unregister(pd);
		pd = NULL;
	}
	mutex_unlock(&lock);
}

static inline struct dwc_otg2 *xceiv_to_dwc_otg2(struct usb_otg *x)
{
	return container_of(x, struct dwc_otg2, otg);
}

static int dwc_otg2_set_power(struct usb_phy *x, unsigned mA)
{
	return 0;
}

static int dwc_otg2_set_suspend(struct usb_phy *x, int suspend)
{
	return 0;
}

/* @debug */
void print_otg_regs(void)
{
	struct usb_phy *transceiver = usb_get_transceiver();
	struct dwc_otg2 *otg = xceiv_to_dwc_otg2(transceiver->otg);

	print_debug_regs(otg);
	usb_put_transceiver(transceiver);
}

static irqreturn_t dwc_otg_irq(int irq, void *x)
{
	unsigned long flags;
	struct dwc_otg2 *otg = xceiv_to_dwc_otg2((struct usb_otg *)x);
	u32 oevten = 0;
	u32 oevt = 0;
	u32 osts = 0;
	u32 octl = 0;
	u32 ocfg = 0;
	u32 adpcfg = 0;
	u32 adpctl = 0;
	u32 adpevt = 0;
	u32 adpevten = 0;
	u32 otg_mask = OEVT_ALL;
	u32 adp_mask = ADPEVT_ADP_PRB_EVNT |
		ADPEVT_ADP_SNS_EVNT |
		ADPEVT_ADP_TMOUT_EVNT |
		ADPEVT_ADP_RST_CMPLT_EVNT;

	oevt = otg_read(otg, OEVT);
	osts = otg_read(otg, OSTS);
	octl = otg_read(otg, OCTL);
	ocfg = otg_read(otg, OCFG);
	oevten = otg_read(otg, OEVTEN);

	adpcfg = otg_read(otg, ADPCFG);
	adpctl = otg_read(otg, ADPCTL);
	adpevt = otg_read(otg, ADPEVT);
	adpevten = otg_read(otg, ADPEVTEN);

	/* Clear handled events */
	otg_write(otg, OEVT, oevt);
	otg_write(otg, ADPEVT, adpevt);

	otg_dbg(otg, "\n");
	otg_dbg(otg, "    oevt = %08x\n", oevt);
	otg_dbg(otg, "    osts = %08x\n", osts);
	otg_dbg(otg, "    octl = %08x\n", octl);
	otg_dbg(otg, "    ocfg = %08x\n", ocfg);
	otg_dbg(otg, "  oevten = %08x\n", oevten);
	otg_dbg(otg, "  adpcfg = %08x\n", adpcfg);
	otg_dbg(otg, "  adpctl = %08x\n", adpctl);
	otg_dbg(otg, "  adpevt = %08x\n", adpevt);
	otg_dbg(otg, "adpevten = %08x\n", adpevten);

	otg_dbg(otg, "oevt[DeviceMode] = %s\n", oevt & OEVT_DEV_MOD_EVNT ? "Device" : "Host");

	if (oevt & OEVT_CONN_ID_STS_CHNG_EVNT)
		otg_dbg(otg, "Connector ID Status Change Event\n");
	if (oevt & OEVT_HOST_ROLE_REQ_INIT_EVNT)
		otg_dbg(otg, "Host Role Request Init Notification Event\n");
	if (oevt & OEVT_HOST_ROLE_REQ_CONFIRM_EVNT)
		otg_dbg(otg, "Host Role Request Confirm Notification Event\n");
	if (oevt & OEVT_A_DEV_B_DEV_HOST_END_EVNT)
		otg_dbg(otg, "A-Device B-Host End Event\n");
	if (oevt & OEVT_A_DEV_HOST_EVNT)
		otg_dbg(otg, "A-Device Host Event\n");
	if (oevt & OEVT_A_DEV_HNP_CHNG_EVNT)
		otg_dbg(otg, "A-Device HNP Change Event\n");
	if (oevt & OEVT_A_DEV_SRP_DET_EVNT)
		otg_dbg(otg, "A-Device SRP Detect Event\n");
	if (oevt & OEVT_A_DEV_SESS_END_DET_EVNT)
		otg_dbg(otg, "A-Device Session End Detected Event\n");
	if (oevt & OEVT_B_DEV_B_HOST_END_EVNT)
		otg_dbg(otg, "B-Device B-Host End Event\n");
	if (oevt & OEVT_B_DEV_HNP_CHNG_EVNT)
		otg_dbg(otg, "B-Device HNP Change Event\n");
	if (oevt & OEVT_B_DEV_SES_VLD_DET_EVNT)
		otg_dbg(otg, "B-Device Session Valid Detect Event\n");
	if (oevt & OEVT_B_DEV_VBUS_CHNG_EVNT)
		otg_dbg(otg, "B-Device VBUS Change Event\n");

	if (adpevt & adp_mask) {
		if (adpevt & ADPEVT_ADP_PRB_EVNT) {
			u32 ramp_time = (adpevt & ADPEVT_RTIM_EVNTS) >> ADPEVT_RTIM_EVNT_SHIFT;
			otg_dbg(otg, "ADP Probe Event (ramp_time=%x)\n", ramp_time);
		}
		if (adpevt & ADPEVT_ADP_SNS_EVNT)
			otg_dbg(otg, "ADP Sense Event\n");
		if (adpevt & ADPEVT_ADP_TMOUT_EVNT)
			otg_dbg(otg, "ADP Timeout Event\n");
		if (adpevt & ADPEVT_ADP_RST_CMPLT_EVNT)
			otg_dbg(otg, "ADP Reset Complete Event\n");
	}

	if ((oevt & otg_mask) || (adpevt & adp_mask)) {
		/* Pass event to main thread */
		spin_lock_irqsave(&otg->lock, flags);
		otg->otg_events |= oevt;
		otg->adp_events |= adpevt;
		wakeup_main_thread(otg);
		spin_unlock_irqrestore(&otg->lock, flags);
	}

	return IRQ_HANDLED;
}

static int dwc_otg2_set_peripheral(struct usb_otg *x,
		struct usb_gadget *gadget)
{
	struct dwc_otg2 *otg;

	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!gadget) {
		otg->otg.gadget = NULL;
		stop_main_thread(otg);
		return -ENODEV;
	}

	otg->otg.gadget = gadget;
	otg->otg.phy->state = OTG_STATE_B_IDLE;
	start_main_thread(otg);
	return 0;
}

static int dwc_otg2_set_host(struct usb_otg *x, struct usb_bus *host)
{
	struct dwc_otg2 *otg;

	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!host) {
		otg->otg.host = NULL;
		stop_main_thread(otg);
		return -ENODEV;
	}

	otg->otg.host = host;
	start_main_thread(otg);
	return 0;
}

static int dwc_otg2_start_srp(struct usb_otg *x)
{
	unsigned long flags;
	struct dwc_otg2 *otg;

	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!otg->otg.host || !otg->otg.gadget)
		return -ENODEV;

	spin_lock_irqsave(&otg->lock, flags);
	otg->user_events |= USER_SRP_EVENT;
	wakeup_main_thread(otg);
	spin_unlock_irqrestore(&otg->lock, flags);
	return 0;
}

static int dwc_otg2_start_hnp(struct usb_otg *x)
{
	unsigned long flags;
	struct dwc_otg2 *otg;

	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!otg->otg.host || !otg->otg.gadget)
		return -ENODEV;

	spin_lock_irqsave(&otg->lock, flags);
	otg->user_events |= USER_HNP_EVENT;
	wakeup_main_thread(otg);
	spin_unlock_irqrestore(&otg->lock, flags);
	return 0;
}

static int dwc_otg2_end_session(struct usb_otg *x)
{
	unsigned long flags;
	struct dwc_otg2 *otg;

	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!otg->otg.host || !otg->otg.gadget)
		return -ENODEV;

	spin_lock_irqsave(&otg->lock, flags);
	otg->user_events |= USER_END_SESSION;
	wakeup_main_thread(otg);
	spin_unlock_irqrestore(&otg->lock, flags);
	return 0;
}

static int dwc_otg2_start_rsp(struct usb_otg *x)
{
	unsigned long flags;
	struct dwc_otg2 *otg;
	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!otg->otg.host || !otg->otg.gadget)
		return -ENODEV;

	spin_lock_irqsave(&otg->lock, flags);
	otg->user_events |= USER_RSP_EVENT;
	wakeup_main_thread(otg);
	spin_unlock_irqrestore(&otg->lock, flags);
	return 0;
}

static int dwc_otg2_received_host_release(struct usb_otg *x)
{
	unsigned long flags;
	struct dwc_otg2 *otg;
	if (!x)
		return -ENODEV;

	otg = xceiv_to_dwc_otg2(x);
	otg_dbg(otg, "\n");

	if (!otg->otg.host || !otg->otg.gadget)
		return -ENODEV;

	spin_lock_irqsave(&otg->lock, flags);
	otg->user_events |= PCD_RECEIVED_HOST_RELEASE_EVENT;
	wakeup_main_thread(otg);
	spin_unlock_irqrestore(&otg->lock, flags);
	return 0;
}

static int dwc_otg2_notifier_cb(struct notifier_block *nb,
            unsigned long event, void *priv)
{
	struct dwc_otg2 *otg;
    struct usb_phy  *phy;
    unsigned long flags;
    int rc = 0;

    phy = usb_get_transceiver();
    if(!phy){
        return -ENOENT;
    }

    otg = xceiv_to_dwc_otg2(phy->otg);
    if(!otg){
        usb_put_transceiver(phy);
        return -ENOENT;
    }

    otg_dbg(otg, "notifier event %lu.\n",event);

    switch(event)
    {
        case USB_BALONG_DEVICE_REMOVE:
        case USB_BALONG_PERIP_REMOVE:
        	/* Pass event to main thread */
            spin_lock_irqsave(&otg->lock, flags);
            otg->thread_cancel = 1;
            if(otg->main_thread){
                otg->user_events |= USER_EXIT_EVENT;
            	wakeup_main_thread(otg);
            }
            spin_unlock_irqrestore(&otg->lock, flags);

            rc = wait_event_interruptible_timeout(
                otg->exit_wq,
				0 == otg->thread_cancel,
				(2*HZ));
            if(0 >= rc){
                otg_err(otg,"wait otg terminate failed,rc=%d\n",rc);
            }
            break;
        default:
            break;
    }
    
    usb_put_transceiver(phy);
    return 0;
}
static int __devinit dwc_otg2_probe(struct platform_device *pdev)
{
	struct dwc_otg2 *otg;
    struct usb_phy  *phy;
	int err;
	int irq;

	otg = kzalloc(sizeof *otg, GFP_KERNEL);
	if (!otg)
		return -ENOMEM;

	phy = kzalloc(sizeof *phy, GFP_KERNEL);
   	if (!phy) {
    	err = -ENOMEM;
    	goto exit0;
	}

	otg->dev		= &pdev->dev;
	otg->otg.phy    = phy;
	otg->otg.set_host       = dwc_otg2_set_host;
	otg->otg.set_peripheral = dwc_otg2_set_peripheral;
	otg->otg.start_srp      = dwc_otg2_start_srp;
	otg->otg.start_hnp      = dwc_otg2_start_hnp;
	otg->otg.end_session    = dwc_otg2_end_session;
	otg->otg.start_rsp      = dwc_otg2_start_rsp;
	otg->otg.host_release   = dwc_otg2_received_host_release;
	otg->otg.irq            = dwc_otg_irq;
    
	phy->dev		= otg->dev;
	phy->label	= "dwc_otg2";
	phy->state	= OTG_STATE_UNDEFINED;
	phy->otg 		= &otg->otg;
	phy->set_power      = dwc_otg2_set_power;
	phy->set_suspend    = dwc_otg2_set_suspend;

	otg->state = DWC_STATE_INIT;
	spin_lock_init(&otg->lock);
	init_waitqueue_head(&otg->main_wq);

	otg_dbg(otg, "Version: %s\n", VERSION);
	err = usb_set_transceiver(otg->otg.phy);
	if (err) {
		dev_err(&pdev->dev, "can't register transceiver, err: %d\n",
			err);
		goto exit1;
	}

	irq = platform_get_irq(pdev, 0);
	err = request_irq(irq, dwc_otg_irq, IRQF_SHARED, 
		"dwc_otg2", (void *)&otg->otg);
	if (err) {
		dev_err(&pdev->dev, "failed to request irq #%d --> %d\n",
				irq, err);
		goto exit2;
	}

	platform_set_drvdata(pdev, otg);

#ifdef CONFIG_USB_OTG_USBID_BYGPIO
    hi_syssc_usb_iddig(otg_get_usbid());
#endif

    gs_otg3_nb.priority = USB_NOTIF_PRIO_HAL;
    gs_otg3_nb.notifier_call = dwc_otg2_notifier_cb;
    bsp_usb_register_notify(&gs_otg3_nb);
    init_waitqueue_head(&otg->exit_wq);

	return 0;

exit2:
	usb_set_transceiver(NULL);
exit1:
    kfree(phy);
exit0:
	kfree(otg);

	return err;
}

static int __devexit dwc_otg2_remove(struct platform_device *pdev)
{
	struct dwc_otg2 *otg = platform_get_drvdata(pdev);
	int irq;

	otg_dbg(otg, "\n");

    bsp_usb_unregister_notify(&gs_otg3_nb);
	irq = platform_get_irq(pdev, 0);
	free_irq(irq, (void *)&otg->otg);
	usb_set_transceiver(NULL);
	platform_set_drvdata(pdev, NULL);
	kfree(otg);
	return 0;
}

static struct platform_driver dwc_otg2_xceiv_driver = {
	.probe		= dwc_otg2_probe,
	.remove		= dwc_otg2_remove,
	.driver	= {
		.name	= "dwc_otg2",
		.owner	= THIS_MODULE,
	},
};

extern void usb3_sysctrl_init(void);
extern int bc_charger_type_identify(void);
extern void usb3_bc_clk_disable(void);
extern void usb3_sysctrl_exit(void);
/*static int __init dwc_otg_init(void)*/
int dwc_otg_init(void)
{
	int retval;

	usb3_sysctrl_init();

    /* if the version support charger, identify the charger type
     * we must detect charger type before usb core init
     */
    if (bsp_usb_is_support_charger()) {
        bsp_usb_set_charger_type(bc_charger_type_identify());
    }

    /* clk off the bc controller, when we never use it */
    usb3_bc_clk_disable();

	mutex_init(&lock);

	dwc_otg2_register();
	
	retval = platform_driver_register(&dwc_otg2_xceiv_driver);
	if (retval)
		return retval;

	return 0;
}
/*module_init(dwc_otg_init);*/

/*static void __exit dwc_otg_exit(void)*/
void dwc_otg_exit(void)
{
	dwc_otg2_unregister();
	platform_driver_unregister(&dwc_otg2_xceiv_driver);
	mutex_destroy(&lock);
    usb3_sysctrl_exit();
}
/*module_exit(dwc_otg_exit);*/

MODULE_AUTHOR("Synopsys, Inc");
MODULE_DESCRIPTION("Synopsys DWC USB 3.0 with OTG 2.0/3.0");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(VERSION);

/** @mainpage DesignWare Cores OTG 3.0 driver for USB 3.0.

    This driver manages the OTG core and the OTG, host, and peripheral states as
    specified in the DWC OTG 3.0 databook.  It is in charge of handling
    start/stop of the appropriate driver based on the connector ID, as well SRP,
    HNP, and ADP states and protocols.

    @section changes Changes to USB Core

    The otg driver uses the OTG framework in the kernel.  The host and device
    driver know to register with this framework based on a configuration
    parameter: CONFIG_USB_DWC_OTG.  This parameter specifies that the system
    contains a Synopsys Dual-Role (XHCI/UDC) OTG controller.  With this
    information both the PCD and the HCD will register with the OTG transceiver
    driver.  When both of these are registered, the transceiver driver starts a
    thread which manages the OTG state.

    Due to this registration scheme, you must ensure that the dwc_otg3 driver is
    loaded prior to the xhci and pcd drivers.

    Changes to the existing host stack are kept to a minimum.  These include
    changes for HNP polling, XHCI port reset (for HNP), and the addition of OTG
    2.0 and 3.0 descriptors.

    @section hnppoll HNP Polling

    The usb core is also enhanced to send HNP polling requests.  This is a new
    mechanism in OTG 2.0 that allows the host to send the
    SET_FEATURE(hnp_enable) only when the device wants to change roles.  This is
    enabled based on OTG being configure (CONFIG_USB_OTG) and by examining a new
    field in the OTG descripters to determine that the device supports OTG 2.0 or
    3.0 (bcdOTG).

    @section sysfs Sysfs Attributes

    Two new sysfs attributes have been introduced in the PCD:

    hnp - Initiates HNP request.  Write to this parameter to role-swap. This
    only works in high speed.

    srp - Initiates SRP request.  Write to this to request a session from the
    host.

    end - Write as a B-peripheral to end a session.

    rsp - Initiates RSP request.  Write to this as an A or B-peripheral to
    request host mode. This only works in superspeed.

 */
/*lint -restore*/
