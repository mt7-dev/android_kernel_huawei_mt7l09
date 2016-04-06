
#include <linux/huawei/usb/hisi_usb.h>

enum hisi_charger_type fsa9685_get_charger_type(void);
int fcp_read_switch_status (void);
int fcp_read_adapter_status(void);
void switch_dump_register(void);
/*fcp detect */
#define FCP_ADAPTER_DETECT_FAIL 1
#define FCP_ADAPTER_DETECT_SUCC 0
#define FCP_ADAPTER_DETECT_OTHER -1
/*fcp adapter status */
#define FCP_ADAPTER_STATUS 0x28
#define FCP_ADAPTER_OVLT 0x04
#define FCP_ADAPTER_OCURRENT 0x02
#define FCP_ADAPTER_OTEMP 0x01
