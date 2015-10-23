/* 
 *  Hisilicon K3 SOC camera driver source file 
 * 
 *  CopyRight (C) Huawei Technology Co., Ltd. 
 * 
 * Author:	  h00145353 
 * Email:	  alan.hefeng@huawei.com
 * Date:	  2013-11-26
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


#include <linux/err.h>
#include <linux/hisi_ion.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include "../../../staging/android/ion/ion_priv.h"

struct ion_device *idev;
struct ion_mapper *hw_user_mapper;
int num_heaps;
struct ion_heap **heaps;

struct ion_client*
hisi_ion_client_create(char const* name)
{
    return ion_client_create(idev, name);
}

static int 
hw_ion_probe(
        struct platform_device* pdev)
{
	struct ion_platform_data* pdata = pdev->dev.platform_data;
	int err;
	int i;

	num_heaps = pdata->nr;

	heaps = kzalloc(sizeof(struct ion_heap *) * pdata->nr, GFP_KERNEL);

	idev = ion_device_create(NULL);
	if (IS_ERR_OR_NULL(idev)) {
		kfree(heaps);
		return PTR_ERR(idev);
	}

	/* create the heaps as specified in the board file */
	for (i = 0; i < num_heaps; i++) {
		struct ion_platform_heap *heap_data = &pdata->heaps[i];

		heaps[i] = ion_heap_create(heap_data);
		if (IS_ERR_OR_NULL(heaps[i])) {
			err = PTR_ERR(heaps[i]);
			goto err;
		}
		ion_device_add_heap(idev, heaps[i]);
	}
	platform_set_drvdata(pdev, idev);
	return 0;
err:
	for (i = 0; i < num_heaps; i++) {
		if (heaps[i])
			ion_heap_destroy(heaps[i]);
	}
	kfree(heaps);
	return err;
}

static int 
hw_ion_remove(
        struct platform_device* pdev)
{
	struct ion_device* idev = platform_get_drvdata(pdev);
	int i;

	ion_device_destroy(idev);
	for (i = 0; i < num_heaps; i++)
		ion_heap_destroy(heaps[i]);
	kfree(heaps);
	return 0;
}

static struct platform_driver 
ion_driver = 
{
	.probe = hw_ion_probe,
	.remove = hw_ion_remove,
	.driver = 
    { 
        .name = "huawei,ion" 
    }
};

static int __init 
ion_init(void)
{
	return platform_driver_register(&ion_driver);
}

static void __exit 
ion_exit(void)
{
	platform_driver_unregister(&ion_driver);
}

module_init(ion_init);
module_exit(ion_exit);

