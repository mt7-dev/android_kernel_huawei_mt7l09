

#include <bsp_clk.h>
#include <bsp_icc.h>
#include "clock.h"

spinlock_t  clocks_spinlock;
#ifdef __VXWORKS__

#ifndef WAIT_FOREVER
#define WAIT_FOREVER	(-1)
#endif
static LIST_HEAD(clocks);
SEM_ID clocks_mutex;

int  IS_ERR(struct clk *c)
{
	if (!c)
           return 1;
      else
           return 0;
}
static int clk_strcmp(const char *cs, const char *ct)
{
	char c1, c2;
	//unsigned char c1, c2;
	while (1) {			/*lint !e716*/
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}
	return 0;
}
static struct clk_lookup *clk_find(const char *dev_id, const char *con_id)
{
	struct clk_lookup *p, *cl = NULL;
	int match, best = 0;
	list_for_each_entry(p, &clocks, node){
		match = 0;
		if (p->con_id) {
			if (!con_id || clk_strcmp(p->con_id, con_id))
				continue;
			match += 1;
		}

		if (match > best) {
			cl = p;
                   break;
		}
	}
	return cl;
}

static struct clk *clk_get_sys(const char *dev_id, const char *con_id)
{
	struct clk_lookup *cl;

	semTake(clocks_mutex, WAIT_FOREVER);
	cl = clk_find(dev_id, con_id);
	semGive(clocks_mutex);
	return cl ? cl->clk : NULL;
}
struct clk *clk_get(char *dev, const char *con_id)
{
	const char *dev_id = dev ? NULL : NULL;
      struct clk *c = NULL;

	c = clk_get_sys(dev_id, con_id);
      if (IS_ERR(c))
             clk_printf("clk_get function can't find %s,may be you should register it firstly!!\n", con_id);
      return c;
}
static void clkdev_add(struct clk_lookup *cl)
{
	semTake(clocks_mutex, WAIT_FOREVER);
	list_add_tail(&cl->node, &clocks);
	semGive(clocks_mutex);
}

#endif /*__KERNEL__*/



/*******************************************
*get clk's rate
*@c--come from clk_get();
*return:
*          the clk'rate
********************************************/
unsigned long clk_get_rate(struct clk *c)
{
    unsigned long rate = 0;

    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return 0;
    }
    rate = c->rate;
    return rate;
}


/*******************************************
*set clk's rate
*@c--come from clk_get();
*@rate --the clk rate to be
*return
*       zero        OK
*       nonzern   failure
********************************************/
int clk_set_rate(struct clk *c, unsigned long rate)
{
    int ret = 0;
    unsigned long flags = 0;
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return 0;
    }
    spin_lock_irqsave(&clocks_spinlock,flags);
    bsp_ipc_spin_lock(IPC_SEM_CLK);
    if (!c->ops || !c->ops->set_rate) {
       clk_printf("clock: %s has no ops or ops->set_rate!!!\n", c->name);
    	ret = -1;
    	goto out;
    }
    ret = c->ops->set_rate(c, rate);
out:
    bsp_ipc_spin_unlock(IPC_SEM_CLK);
    spin_unlock_irqrestore(&clocks_spinlock,flags);
    return ret;
}

/*******************************************
*round clk's rate--(it's no useful)
*@c--come from clk_get();
*@rate --the clk rate to be
*return
*       zero         OK
*       nonzern   failure
********************************************/
long clk_round_rate(struct clk *c, unsigned long rate)
{
    unsigned long ret = 0;
    unsigned long flags = 0;
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return 0;
    }
    spin_lock_irqsave(&clocks_spinlock,flags);
    bsp_ipc_spin_lock(IPC_SEM_CLK);
    if (!c->ops || !c->ops->round_rate) {
        ret = 0;
        goto out;
    }
    ret =(unsigned long) c->ops->round_rate(c, rate);
out:
    bsp_ipc_spin_unlock(IPC_SEM_CLK);
    spin_unlock_irqrestore(&clocks_spinlock,flags);
    return (long)ret;
}

/*******************************************
*disable clk
*@c--come from clk_get();
********************************************/
static void __clk_disable(struct clk *c)
{
    if (!c)
    	  return;
    if (c->refcnt == 0){
        clk_printf("%s has been disabled!!!\n",c->name);
        return;
    }

    if (c->refcnt == 1){
            if (c->ops && c->ops->disable) {
                c->ops->disable(c);
                if(c->parent)
            	     __clk_disable(c->parent);
            }
    }
    c->refcnt--;
    return;
}


void clk_disable(struct clk *c)
{
    unsigned long flags = 0;
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return;
    }
    spin_lock_irqsave(&clocks_spinlock,flags);

    __clk_disable(c);

    spin_unlock_irqrestore(&clocks_spinlock,flags);
}

/*******************************************
*enable clk
*@c--come from clk_get();
*return
*       zero        OK
*       nonzern   failure
********************************************/
static int __clk_enable(struct clk *c)
{
    int ret = 0;
    if (!c)
        return 0;
    if (c->refcnt == 0){
        if (c->parent) {
             ret = __clk_enable(c->parent); /* enable parent */
            	if (ret) {
            		return ret;
            	}
        }
        if (c->ops && c->ops->enable)
            	ret = c->ops->enable(c);
        if (ret) {
              	if (c->parent)
              		__clk_disable(c->parent);
    			return ret;
        }
    }
    c->refcnt++;
    return 0;
}

int clk_enable (struct clk *c)
{
    int ret = 0;
    unsigned long flags = 0;
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return 0;
    }
    spin_lock_irqsave(&clocks_spinlock,flags);

    ret = __clk_enable(c);

    spin_unlock_irqrestore(&clocks_spinlock,flags);
    return ret;
}


/*******************************************
*set clk'parent
*@c--come from clk_get();
*@parent---come from clk_get();
*return
*       zero        OK
*       nonzern   failure
********************************************/
int clk_set_parent(struct clk *c, struct clk *parent)
{
    int ret = 0;
    unsigned long flags = 0;
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return 0;
    }
    spin_lock_irqsave(&clocks_spinlock,flags);
    bsp_ipc_spin_lock(IPC_SEM_CLK);
    if (!c->ops || !c->ops->set_parent) {
        clk_printf("This clock has no ops or has no mulparent!!!\n");
        ret = -1;
        goto out;
    }
    ret = c->ops->set_parent(c, parent);
out:
    bsp_ipc_spin_unlock(IPC_SEM_CLK);
    spin_unlock_irqrestore(&clocks_spinlock,flags);
    return ret;
}

/*******************************************
*get clk'parent
*@c--come from clk_get();
*@parent---come from clk_get();
*return
*       the struct of clk'parent
********************************************/
struct clk *clk_get_parent(struct clk *c)
{
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return NULL;
    }
    return c->parent;
}
/*******************************************
*get clk satus
*@c--come from clk_get
*@parent---come from clk_get();
*return
*       the status of clk
*       1:enable
*       0:disable
********************************************/
int clk_status(struct clk *c)
{
    int ret = 0;
    unsigned long flags = 0;
    if (IS_ERR(c)){
        clk_printf("This clock is NULL!!!\n");
        return -1;
    }
    if (!c->ops || !c->ops->isenable) {
        clk_printf("This clock has no ops or has no isenable ops!!!\n");
        return -1;
    }
    spin_lock_irqsave(&clocks_spinlock,flags);

    ret = c->ops->isenable(c);

    spin_unlock_irqrestore(&clocks_spinlock,flags);
    return ret;
}
#ifdef __KERNEL__
EXPORT_SYMBOL(clk_enable);
EXPORT_SYMBOL(clk_disable);
EXPORT_SYMBOL(clk_round_rate);
EXPORT_SYMBOL(clk_set_rate);
EXPORT_SYMBOL(clk_get_rate);
EXPORT_SYMBOL(clk_set_parent);
EXPORT_SYMBOL(clk_get_parent);

/**
*init hi6930_clock
*初始化时钟查询结构体数组里的时钟，并使之成链表
*/
void __init hi6930_clock_init(void)
{
	int i = 0;
	struct clk_lookup *cl_lookups = &hi6930_clk_lookup[0];
	struct clk_lookup *cl = NULL;
      spin_lock_init(&clocks_spinlock);
	while((cl_lookups[i].clk != NULL) ) {
		cl = &cl_lookups[i];
		clkdev_add(cl);
		i++;
	}
        clk_printf("clk init success!\n");
}
#elif defined(__VXWORKS__) /* __VXWORKS__ */
#ifdef CONFIG_K3V3_CLK_CRG /*CONFIG_K3V3_CLK_CRG*/
static int gps_reclk_enable_and_disable_read_cb(u32 channel_id , u32 len, void* contex)
{
    int read_size = 0;
    u8 bufvalue = 0;
    if (!len)
    {
		clk_printf("C:gps_reclk_enable_and_disable_read_cb fail for zero length data\n");
        return 1;
    }
    read_size = (int)bsp_icc_read((ICC_CHN_IFC << 16 | FC_RECV_FUNC_GPSCLK), &bufvalue, len);
    if ((read_size > (int)len) || (read_size < 0))
    {
		clk_printf("C:gps_reclk_enable_and_disable_read_cb fail [0x%x]\n", read_size);
        return 1;
    }
    if(bufvalue){
        writel((unsigned int)0x1 << 27, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKEN3_OFFSET);
    }else{
        writel((unsigned int)0x1 << 27, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKDIS3_OFFSET);
    }
    return 0;
}
void gps_refclk_icc_read_cb_init(void)
{
    if(0 !=bsp_icc_event_register(ICC_CHN_IFC << 16 | FC_RECV_FUNC_GPSCLK, (read_cb_func)gps_reclk_enable_and_disable_read_cb, NULL, NULL, NULL))
    {
        clk_printf("register icc callback fail!\n");
        return;
    }
}
#endif
/*clocks init API*/
void  hi6930_clock_init(void)
{
	int i = 0;
	struct clk_lookup *cl_lookups = &hi6930_clk_lookup[0];
	struct clk_lookup *cl = NULL;
      unsigned int regvalue = 0;

     clocks_mutex = semBCreate (SEM_Q_FIFO, (SEM_B_STATE)SEM_FULL);
    spin_lock_init(&clocks_spinlock);

	while((cl_lookups[i].clk != NULL) ) {
		cl = &cl_lookups[i];
		clkdev_add(cl);
		i++;
	}
#ifdef CONFIG_P531_CLK_CRG /*CONFIG_P531_CLK_CRG*/
/*enable auto clk gate of all clocks*/
      regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + 0x454);
      regvalue |= 0x1FFFE3C;
      writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + 0x454);

      regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + 0x450);
      regvalue |= 0xDF48366B;
      writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + 0x450);

#elif defined(CONFIG_V7R2_CLK_CRG) /* CONFIG_V7R2_CLK_CRG */
/*enable auto clk gate of all clocks*/
      regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + 0x454);
      regvalue |= 0x1FFFE3C;
      writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + 0x454);

     regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + 0x450);
     regvalue |= 0xD848266B;
     writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + 0x450);

#elif defined(CONFIG_K3V3_CLK_CRG) /* CONFIG_K3V3_CLK_CRG */
      regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + 0x450);
      regvalue |= 0x9140006B;
      writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + 0x450);

     regvalue = readl(HI_SYSCTRL_BASE_ADDR_VIRT + 0x454);
      regvalue |= 0x17EAC10;
      writel(regvalue, HI_SYSCTRL_BASE_ADDR_VIRT + 0x454);
#else
#error "This branch is neither P531 nor V7R2 nor K3V3,clock module can't support"
#endif
        clk_printf("clk init success!\n");
}
#endif


