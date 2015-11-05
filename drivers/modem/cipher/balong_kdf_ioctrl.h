#ifndef _KDF_IOCTRL_H_
#define _KDF_IOCTRL_H_

#include <linux/ioctl.h>
#include "osl_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define KDFDEV_IOC_MAGIC 'K'  /*设备魔数*/
#define KDFDEV_MAJOR 0        /*预设主设备号*/
#define KDFDEV_NR_DEVS 1      /*设备个数*/
#define KDFDEV_MEM_SIZE (512) /*接受用户控件数据缓冲区[in byte]*/


/*命令*/
#define KDF_IOC_GETSTATUS (_IOR(KDFDEV_IOC_MAGIC, 1, int))
#define KDF_IOC_GETHASH   (_IOR(KDFDEV_IOC_MAGIC, 2, int))
#define KDF_IOC_SETDATA   (_IOW(KDFDEV_IOC_MAGIC, 3, int))
#define KDF_IOC_SETKEY    (_IOW(KDFDEV_IOC_MAGIC, 4, int))

/*设备工作空间*/
struct kdf_dev_mem
{
	u8 * buf;
	u32  size;
};

#ifdef __cplusplus
}
#endif


#endif
