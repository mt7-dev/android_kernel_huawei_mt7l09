#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/pwm.h>
#include <asm/div64.h>
#include <osl_math64.h>
#include <soc_memmap.h>
#include <hi_pwm.h>
#include <mach/irqs.h>//modem
#include "pwm_balong.h"



/*
 * period_ns = 10^9 * PV/ PWM_CLK_RATE
 * duty_ns   = period_ns * (DC / PV)= 10^9 * DC / PWM_CLK_RATE
 */
int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns)
{
    unsigned long long c;
    unsigned long pv, dc;
    int disable_flag = 0;

    if (pwm == NULL || period_ns == 0 || duty_ns > period_ns)
        return -EINVAL;

    if (period_ns == pwm->period_ns &&
        duty_ns == pwm->duty_ns)
        return 0;

    c = clk_get_rate(pwm->clk);
    c = c * period_ns;
    c = div_u64(c, 1000000000);
    //do_div(c, 1000000000);
    pv = c;

    if (pv > 1024)
        return -EINVAL;

    dc = pv * duty_ns / period_ns;

    /* NOTE: the clock to PWM has to be enabled first
     * before writing to the registers
     */
    clk_enable(pwm->clk);
    if(!(pwm->pwm_id % 2))
    {
        __raw_writel(dc, pwm->mmio_base + HI_PWM_OUTA_WIDE_OFFSET);
        __raw_writel(pv, pwm->mmio_base + HI_PWM_OUTA_DIV_OFFSET);
    }
    else
    {
        __raw_writel(dc, pwm->mmio_base + HI_PWM_OUTB_WIDE_OFFSET);
        __raw_writel(pv, pwm->mmio_base + HI_PWM_OUTB_DIV_OFFSET);
    }

    /*只有两个通道都不使用的时候，才能关闭pwm时钟*/
    if(!readl(pwm->mmio_base + HI_PWM_OUTA_EN_OFFSET) && !readl(pwm->mmio_base + HI_PWM_OUTB_EN_OFFSET))
        disable_flag = 1;

    if(disable_flag)
        clk_disable(pwm->clk);

    return 0;
}
EXPORT_SYMBOL(pwm_config);
int pwm_enable(struct pwm_device *pwm)
{
    int rc = 0;

    if (!pwm->clk_enabled) {
        rc = clk_enable(pwm->clk);
        if (!rc)
            pwm->clk_enabled = 1;
    }
    if(!(pwm->pwm_id % 2))
        writel(1, pwm->mmio_base + HI_PWM_OUTA_EN_OFFSET);
    else
        writel(1, pwm->mmio_base + HI_PWM_OUTB_EN_OFFSET);

    pwm->running = 1;
    return rc;
}
EXPORT_SYMBOL(pwm_enable);

void pwm_disable(struct pwm_device *pwm)
{
    int disable_flag = 0;

    if(!(pwm->pwm_id % 2))
        writel(0, pwm->mmio_base + HI_PWM_OUTA_EN_OFFSET);
    else
        writel(0, pwm->mmio_base + HI_PWM_OUTB_EN_OFFSET);

    /*只有两个通道都不使用的时候，才能关闭pwm时钟*/
    if(!readl(pwm->mmio_base + HI_PWM_OUTA_EN_OFFSET) && !readl(pwm->mmio_base + HI_PWM_OUTB_EN_OFFSET))
        disable_flag = 1;

    if ((pwm->clk_enabled)&&(disable_flag)) {
        clk_disable(pwm->clk);
        pwm->clk_enabled = 0;
    }
    pwm->running = 0;
}
EXPORT_SYMBOL(pwm_disable);

static DEFINE_MUTEX(pwm_lock);
static LIST_HEAD(pwm_list);

struct pwm_device *pwm_request(int pwm_id,const char *label)
{
    struct pwm_device *pwm;
    int found = 0;

    mutex_lock(&pwm_lock);

    list_for_each_entry(pwm, &pwm_list, node) {
        if (pwm->pwm_id == pwm_id) {
            found = 1;
            break;
        }
    }

    if (found) {
        if (pwm->use_count == 0) {
            pwm->use_count++;
            pwm->label = label;
         } else
            pwm = ERR_PTR(-EBUSY);
    } else
        pwm = ERR_PTR(-ENOENT);

    mutex_unlock(&pwm_lock);
    return pwm;
}
EXPORT_SYMBOL(pwm_request);
void pwm_free(struct pwm_device *pwm)
{
    mutex_lock(&pwm_lock);

    if (pwm->use_count) {
        pwm->use_count--;
        pwm->label = NULL;
    } else
        pr_warning("PWM device already freed\n");

    mutex_unlock(&pwm_lock);
}
EXPORT_SYMBOL(pwm_free);

static int pwm_register(struct pwm_device *pwm)
{
    pwm->duty_ns = -1;
    pwm->period_ns = -1;

    mutex_lock(&pwm_lock);
    list_add_tail(&pwm->node, &pwm_list);
    mutex_unlock(&pwm_lock);

    return 0;
}
int balong_pwm_probe(struct platform_device *pdev)
{
    struct pwm_device *pwm0 = NULL;
    struct pwm_device *pwm1 = NULL;
    struct resource *r;
    unsigned char i;
    int ret = 0;

    pwm0 = kzalloc(sizeof(struct pwm_device), GFP_KERNEL);
    if (pwm0 == NULL) {
        dev_err(&pdev->dev, "failed to allocate memory\n");
        return -ENOMEM;
    }
    pwm1 = kzalloc(sizeof(struct pwm_device), GFP_KERNEL);
    if (pwm1 == NULL) {
        dev_err(&pdev->dev, "failed to allocate memory\n");
        return -ENOMEM;
    }

    if(pdev->id == 0)
    {
        pwm0->clk = clk_get(NULL, "pwm0_clk");
        pwm1->clk = clk_get(NULL, "pwm0_clk");
    }
    else
    {
        pwm0->clk = clk_get(NULL, "pwm1_clk");
        pwm1->clk = clk_get(NULL, "pwm1_clk");
    }

    if (IS_ERR(pwm0->clk)) {
        ret = PTR_ERR(pwm0->clk);
        goto err_free;
    }
    if (IS_ERR(pwm1->clk)) {
        ret = PTR_ERR(pwm1->clk);
        goto err_free;
    }

    pwm0->clk_enabled = 0;
    pwm0->use_count = 0;
    pwm0->pdev = pdev;

    pwm1->clk_enabled = 0;
    pwm1->use_count = 0;
    pwm1->pdev = pdev;

    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (r == NULL) {
        dev_err(&pdev->dev, "no memory resource defined\n");
        ret = -ENODEV;
        goto err_free_clk;
    }

    r = request_mem_region(r->start, resource_size(r), pdev->name);
    if (r == NULL) {
        dev_err(&pdev->dev, "failed to request memory resource\n");
        ret = -EBUSY;
        goto err_free_clk;
    }

    pwm0->mmio_base = ioremap(r->start, resource_size(r));
    if (pwm0->mmio_base == NULL) {
        dev_err(&pdev->dev, "failed to ioremap() registers\n");
        ret = -ENODEV;
        goto err_free_mem;
    }
    pwm1->mmio_base = ioremap(r->start, resource_size(r));
    if (pwm1->mmio_base == NULL) {
        dev_err(&pdev->dev, "failed to ioremap() registers\n");
        ret = -ENODEV;
        goto err_free_mem;
    }

    pwm0->pwm_id = pdev->id;
    pwm1->pwm_id = pdev->id + 1;

    ret = pwm_register(pwm0);
    if (ret) {
        dev_err(&pdev->dev, "failed to register pwm0\n");
        goto err_free_mem;
    }
    ret = pwm_register(pwm1);
    if (ret) {
        dev_err(&pdev->dev, "failed to register pwm1\n");
        goto err_free_mem;
    }

    platform_set_drvdata(pdev, pwm0);

    dev_err(&pdev->dev, "balong_pwm_probe ok! \n");
    return 0;

err_free_mem:
    release_mem_region(r->start, resource_size(r));
err_free_clk:
    clk_put(pwm0->clk);
    clk_put(pwm1->clk);
err_free:
    kfree(pwm0);
    kfree(pwm1);
    return ret;
}
int balong_pwm_remove(struct platform_device *pdev)
{
    struct pwm_device *pwm;
    struct resource *r;

    pwm = platform_get_drvdata(pdev);
    if (pwm == NULL)
        return -ENODEV;

    mutex_lock(&pwm_lock);
    list_del(&pwm->node);
    mutex_unlock(&pwm_lock);

    iounmap(pwm->mmio_base);

    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    release_mem_region(r->start, resource_size(r));

    clk_put(pwm->clk);
    kfree(pwm);
    return 0;
}
static struct resource balong_resource_pwm0[] = {
    [0] = {
        .start  = HI_PWM0_REGBASE_ADDR,
        .end    = HI_PWM0_REGBASE_ADDR + 0x30,
        .flags  = IORESOURCE_MEM,
    },
};

struct platform_device balong_device_pwm0 = {
    .name       = "balong-pwm",
    .id     = 0,
    .resource   = balong_resource_pwm0,
    .num_resources  = ARRAY_SIZE(balong_resource_pwm0),
};

static struct resource balong_resource_pwm1[] = {
    [0] = {
        .start  = HI_PWM1_REGBASE_ADDR,
        .end    = HI_PWM1_REGBASE_ADDR + 0x30,
        .flags  = IORESOURCE_MEM,
    },
};

struct platform_device balong_device_pwm1 = {
    .name       = "balong-pwm",
    .id     = 2,
    .resource   = balong_resource_pwm1,
    .num_resources  = ARRAY_SIZE(balong_resource_pwm1),
};

static struct platform_driver balong_pwm_driver = {
    .driver     = {
        .name   = "balong-pwm",
        .owner  = THIS_MODULE,
    },
    .probe      = balong_pwm_probe,
    .remove     = __devexit_p(balong_pwm_remove),
};
int balong_pwm_init(void)
{
    int ret;

    ret = platform_device_register(&balong_device_pwm0);
    if(ret)
        printk(KERN_ERR "pwm0:platform_device_register err! \n");
    ret = platform_device_register(&balong_device_pwm1);
    if(ret)
        printk(KERN_ERR "pwm1:platform_device_register err! \n");

    ret = platform_driver_register(&balong_pwm_driver);
    if (ret)
        printk(KERN_ERR "%s: failed to add pwm driver\n", __func__);

    return ret;
}
arch_initcall(balong_pwm_init);

static void __exit balong_pwm_exit(void)
{
    platform_driver_unregister(&balong_pwm_driver);
}
module_exit(balong_pwm_exit);
