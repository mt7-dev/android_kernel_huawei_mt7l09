

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <bsp_clk.h>

/************************************************************************
*打印时钟ID和可用的调试函数
************************************************************************/
void debug_clock(void);

/************************************************************************
*可用的调试函数
************************************************************************/
void debug_clk_enable(unsigned int num);
void debug_clk_disable(unsigned int num);
void debug_clk_status(unsigned int num);
void debug_clk_set_rate(unsigned int num, unsigned long rate);
void debug_clk_get_rate(unsigned int num);
void debug_clk_set_parent(unsigned int num, unsigned int parent_num);

/************************************************************************
*时钟OPS
************************************************************************/
int hi6930_clk_enable (struct clk *clk);
void hi6930_clk_disable (struct clk *clk);
int hi6930_clk_round_rate(struct clk *clk, unsigned long rate);
int hi6930_clk_set_rate(struct clk *clk, unsigned long rate);
int hi6930_clk_set_rate_n(struct clk *clk, unsigned long rate);
int hi6930_clk_set_parent(struct clk *clk, struct clk *parent);
int hi6930_clk_isenable(struct clk *clk);
/************************************************************************
*时钟模块自动化测试函数
************************************************************************/
#ifdef __KERNEL__
int clk_get_test_case0(void);
int clk_enable_test_case0(void);
int clk_disable_test_case0(void);
int clk_get_parent_test_case0(void);
int clk_get_rate_test_case0(void);
int clk_set_parent_test_case0(void);
int clk_set_rate_test_case0(void);
#else
int clk_get_test_case1(void);
int clk_enable_test_case1(void);
int clk_disable_test_case1(void);
int clk_get_parent_test_case1(void);
int clk_get_rate_test_case1(void);
int clk_set_parent_test_case1(void);
int clk_set_rate_test_case1(void);
#endif

#endif

