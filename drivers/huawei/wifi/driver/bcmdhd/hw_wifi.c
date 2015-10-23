#include <linux/module.h>
#include <linux/init.h>
#include <wlioctl.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <hw_wifi.h>
#include "dhd_dbg.h"


/* Customized Locale table : OPTIONAL feature */
const struct cntry_locales_custom hw_translate_custom_table[] = {
/* Table should be filled out based on custom platform regulatory requirement */
	/* Table should be filled out based
	on custom platform regulatory requirement */
	{"",   "XZ", 11},  /* Universal if Country code is unknown or empty */
	{"IR", "CN", 0},  // add by huawei
	{"CU", "CN", 0},  // add by huawei
	{"KP", "CN", 0},   // add by huawei
	{"PK", "CN", 0},  // add by huawei
	{"AE", "AE", 1},
	{"AR", "AR", 1},
	{"AT", "AT", 1},
	{"AU", "AU", 2},
	{"BE", "BE", 1},
	{"BG", "BG", 1},
	{"BN", "BN", 1},
	{"CA", "CA", 2},
	{"CH", "CH", 1},
	{"CY", "CY", 1},
	{"CZ", "CZ", 1},
	{"DE", "DE", 3},
	{"DK", "DK", 1},
	{"EE", "EE", 1},
	{"ES", "ES", 1},
	{"FI", "FI", 1},
	{"FR", "FR", 1},
	{"GB", "GB", 1},
	{"GR", "GR", 1},
	{"HR", "HR", 1},
	{"HU", "HU", 1},
	{"IE", "IE", 1},
	{"IS", "IS", 1},
	{"IT", "IT", 1},
	{"JP", "JP", 5},
	{"KR", "KR", 24},
	{"KW", "KW", 1},
	{"LI", "LI", 1},
	{"LT", "LT", 1},
	{"LU", "LU", 1},
	{"LV", "LV", 1},
	{"MA", "MA", 1},
	{"MT", "MT", 1},
	{"MX", "MX", 1},
	{"NL", "NL", 1},
	{"NO", "NO", 1},
	{"PL", "PL", 1},
	{"PT", "PT", 1},
	{"PY", "PY", 1},
	{"RO", "RO", 1},
	{"RU", "RU", 5},
	{"SE", "SE", 1},
	{"SG", "SG", 4},
	{"SI", "SI", 1},
	{"SK", "SK", 1},
	{"TR", "TR", 7},
	{"TW", "TW", 2},
	{"US", "US", 46},
};

#define HW_BCN_TIMEOUT 10

/* Customized Locale convertor
*  input : ISO 3166-1 country abbreviation
*  output: customized cspec
*/
void get_customized_country_code_for_hw(char *country_iso_code, wl_country_t *cspec)
{
	int size, i;

	printk(KERN_ERR "enter : %s.\n", __FUNCTION__);
	size = ARRAYSIZE(hw_translate_custom_table);

	if (cspec == 0)
		 return;

	if (size == 0)
		 return;

	printk(KERN_ERR "input country code: %s.\n", country_iso_code);
	for (i = 0; i < size; i++) {
		if (strcmp(country_iso_code, hw_translate_custom_table[i].iso_abbrev) == 0) {
			memcpy(cspec->ccode, hw_translate_custom_table[i].custom_locale, WLC_CNTRY_BUF_SZ);
			memcpy(cspec->country_abbrev, hw_translate_custom_table[i].custom_locale, WLC_CNTRY_BUF_SZ);
			cspec->rev = hw_translate_custom_table[i].custom_locale_rev;
			printk(KERN_ERR "output country code: %s, ver: %d.\n", cspec->ccode, cspec->rev);
			return;
		}
	}
	
	return;
}


uint hw_get_bcn_timeout(void)
{
	return HW_BCN_TIMEOUT;
}

static struct dsm_dev dsm_wifi = {
    .name = "dsm_wifi",
    .fops = NULL,
    .buff_size = 1024,
};

struct dsm_client *wifi_dsm_client = NULL;

void hw_register_wifi_dsm_client(void) {
    if(NULL == wifi_dsm_client) {
        wifi_dsm_client = dsm_register_client(&dsm_wifi);
    }
}

void hw_wifi_dsm_client_notify(const char* content) {
    if(wifi_dsm_client && !dsm_client_ocuppy(wifi_dsm_client)) {
        if(content) {
            dsm_client_record(wifi_dsm_client, "%s\n", content);
        }
        dsm_client_notify(wifi_dsm_client, DSM_WIFI_ERROR_NO);
        printk(KERN_INFO "wifi dsm_client_notify success\n");
    } else {
        printk(KERN_INFO "wifi dsm_client_notify failed\n");
    }
}

extern volatile bool cfg80211_suspend;
static volatile int g_need_enable_intr = 0;
extern dhd_pub_t *hw_get_dhd_pub(struct net_device *dev);

static int wait_for_cfg80211_resume(void) {
	int wait_count = 10;
	while(wait_count > 0) {
		msleep(20);
		if(!cfg80211_suspend) {
			HW_PRINT((WIFI_TAG"%s %d\n", __func__, wait_count));
			return wait_count;
		}
		wait_count--;
	}
	return 0;
}

/**
 * This function used in dhd_dpc_thread(dhd_linux.c)
 * when sdio host is in suspend state, skip this action in dpc thread
 * @return: 1 skip, 0 dont skip
 */
int hw_skip_dpc_in_suspend(void) {
	if((!cfg80211_suspend) || (wait_for_cfg80211_resume() > 0)) {
		g_need_enable_intr = 0;
		return 0;
	} else {
		g_need_enable_intr = 1;
		HW_PRINT((WIFI_TAG"%s skip, cfg80211 is in suspend state\n", __func__));
		return 1;
	}
}

/**
 * This function used in wl_cfg80211_resume(wl_cfg80211.c)
 * when dpc thread is interrupted by suspend(see hw_skip_dpc_in_suspend), resched dpc again
 */
void hw_resched_dpc_ifneed(struct net_device *ndev) {
	dhd_pub_t *pub = NULL;
	if(g_need_enable_intr) {
		g_need_enable_intr = 0;
		pub = hw_get_dhd_pub(ndev);
		if(pub && pub->up) {
			HW_PRINT((WIFI_TAG"%s, resched dpc\n", __func__));
			dhd_sched_dpc(pub);
		}
	}
}

