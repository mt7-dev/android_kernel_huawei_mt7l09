


#ifdef __KERNEL__
#include <linux/clk.h>
#include <linux/module.h>
#include <linux/kernel.h>
struct clk *clk_get(struct device *dev, const char *id)
{
    return NULL;
}
#elif defined(__VXWORKS__) /* __VXWORKS__ */
#include <bsp_clk.h>

int  IS_ERR(struct clk *c)
{
    return 0;
}
 struct clk *clk_get(char *dev, const char *id)
{
    return NULL;
}

#endif  /*__KERNEL__*/
int clk_status(struct clk *c)
{
    return 0;
}

void clk_put(struct clk *clk)
{
    return;
}

long clk_round_rate(struct clk *clk, unsigned long rate)
{
    return 0;
}

int clk_enable(struct clk * c)
{
    return 0;
}
void clk_disable(struct clk * c)
{
}
unsigned long clk_get_rate(struct clk * c)
{
    return 48000000;
}
int clk_set_rate(struct clk * c, unsigned long rate)
{
    return 0;
}
int clk_set_parent(struct clk * c, struct clk * parent)
{
    return 0;
}
struct clk *clk_get_parent(struct clk *c)
{
    return NULL;
}


#ifdef __KERNEL__
EXPORT_SYMBOL(clk_get);
EXPORT_SYMBOL(clk_put);
EXPORT_SYMBOL(clk_enable);
EXPORT_SYMBOL(clk_disable);
EXPORT_SYMBOL(clk_get_rate);
EXPORT_SYMBOL(clk_set_rate);
EXPORT_SYMBOL(clk_set_parent);
EXPORT_SYMBOL(clk_get_parent);
#endif  /*__KERNEL__*/

