#ifndef __HW_WIFI_H__
#define __HW_WIFI_H__

/************修改记录************************
版本:				日期:		解决问题:
HUAWEI-2014.001:  	0529		solve wifi panic
HUAWEI-2014.002:       0609		solve scan_done panic.
HUAWEI-2014.003:       0613             throughput optimize.
HUAWEI-2014.004:       0626             solve wdev_cleanup_work panic.
HUAWEI-2014.005:       0702             solve country code problem.
HUAWEI-2014.006:       0725             use huawei customize country code.
HUAWEI-2014.007:       0728             set bcn_timeout for beacon loss and roaming problem.
**********************************************/
#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>
#include <bcmutils.h>
#include <dngl_stats.h>
#include <dhd.h>
#include <wlioctl.h>
#include <wl_iw.h>
#include <linux/huawei/dsm_pub.h>

#define        HUAWEI_VERSION_STR ", HUAWEI-2014.007"

#define   HW_5G_CUSTOM_ROAM_TRIGGER_SETTING  -70  /* dBm default roam trigger 5 band , used by dhd_preinit_ioctls func*/

extern void get_customized_country_code_for_hw(char *country_iso_code, wl_country_t *cspec);
extern uint hw_get_bcn_timeout(void);
extern void hw_register_wifi_dsm_client(void);
extern void hw_wifi_dsm_client_notify(const char* content);
extern int hw_skip_dpc_in_suspend(void);
extern void hw_resched_dpc_ifneed(struct net_device *ndev);

#endif
