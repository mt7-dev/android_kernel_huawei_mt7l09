#include <linux/pm.h>

struct lm_device {
	struct device		dev;
	struct resource		resource;
	unsigned int		irq;
	unsigned int		id;
    void *              lm_driver_data;/* the driver data at the dev is occupied by kernel */
};

struct lm_driver {
	struct device_driver	drv;
	int			(*probe)(struct lm_device *);
	void			(*remove)(struct lm_device *);
	int			(*suspend)(struct lm_device *);
	int			(*resume)(struct lm_device *);

	const struct dev_pm_ops *pm;

};

int lm_driver_register(struct lm_driver *drv);
void lm_driver_unregister(struct lm_driver *drv);


int lm_device_register(struct lm_device *dev);
void lm_device_unregister(struct lm_device *dev);

#define lm_get_drvdata(lm)	(dwc_otg_device_t *)lm->lm_driver_data
#define lm_set_drvdata(lm,d)	lm->lm_driver_data = (void *)d
