/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_clk.h
*
*   作    者 :  xujingcui
*
*   描    述 :  本文件命名为"drv_clk", 给出V7R2底软和协议栈之间的CLK_API接口
*
*   修改记录 :  2013年1月18日  v1.00 xujingcui创建
*************************************************************************/

#ifndef __DRV_CLK_H__
#define __DRV_CLK_H__

#ifndef NULL
#define NULL (void *)0
#endif

struct clk {
    char                              *name;               /*clock name*/
    unsigned int	                  refcnt;              /*clock referent count*/
    struct clk	                     *parent;              /*the parent of this clock*/
    unsigned long                     rate;                /*clock rate*/
    struct clk_ops		             *ops;                 /*clock oprate function*/
    struct clksel	                 *sel_parents;         /*clocks can be this clock's parent*/
    unsigned int                     clkid;                /*clock id ,come from clk_id ,referent to struct crg_tag array*/
};

/**
 * struct clk_ops - standard clock operations
 */
struct clk_ops {
    int        (*isenable)(struct clk *clk);
    int        (*enable)(struct clk *clk);
    void      (*disable)(struct clk *clk);
    int	   (*round_rate)(struct clk *clk, unsigned long rate);
    int	   (*set_rate)(struct clk *clk, unsigned long rate);
    int	   (*set_parent)(struct clk *clk, struct clk *parent);
};


/**
 * struct clksel - select clk parent by clock register
 */
struct clksel {
	struct clk	*sel_parent;              /*clocks can be parent*/
	int	             sel_val;                    	/* the value should be set to the registers' enable bit */
	unsigned long	parent_min;              /*parent min rate*/
	unsigned long	parent_max;              /*parent max rate*/
};



#ifdef __VXWORKS__
/***********************************************************************
*功能:获取时钟
*入参:dev:NULL,  id:该时钟静态注册时的名字
*返回:时钟结构体指针
*时钟必须静态注册后，才能使用，不然获取失败
************************************************************************/
 struct clk *clk_get(char *dev, const char *id);

 /***********************************************************************
*功能:使能时钟
*入参:时钟结构体指针
*返回:成功为0，失败非0
************************************************************************/
int clk_enable(struct clk *clk);

/***********************************************************************
*功能:去使能时钟
*入参:时钟结构体指针
*返回:无
************************************************************************/
void clk_disable(struct clk *clk);

/***********************************************************************
*功能:获取时钟频率
*入参:时钟结构体指针
*返回:时钟频率
************************************************************************/
unsigned long clk_get_rate(struct clk *clk);

long clk_round_rate(struct clk *clk, unsigned long rate);

/***********************************************************************
*功能:设置时钟频率
*入参:时钟结构体指针，预设置频率值(不是分频比)
*返回:成功为0，失败非0
************************************************************************/
int clk_set_rate(struct clk *clk, unsigned long rate);

/***********************************************************************
*功能:设置时钟父级
*入参:时钟结构体指针，父级时钟结构体指针
*返回:成功为0，失败非0
************************************************************************/
int clk_set_parent(struct clk *clk, struct clk *parent);

/***********************************************************************
*功能:获取时钟父级
*入参:时钟结构体指针
*返回:父级时钟结构体指针
************************************************************************/
struct clk *clk_get_parent(struct clk *clk);
#endif

/***********************************************************************
*功能:查询时钟状态
*入参:时钟结构体指针
*返回: 1:enable  0:disable
************************************************************************/
int clk_status(struct clk *c);

#endif

