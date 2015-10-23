/*
 * PWM driver data - see drivers/pwm/pwm_balong.c
 */
#ifndef __PWM_BALONG_H
#define __PWM_BALONG_H
#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

struct pwm_device {
    struct list_head     node;
    struct platform_device  *pdev;

    const char  *label;
    struct clk  *clk;
    int     clk_enabled;
    void __iomem    *mmio_base;

    unsigned int         period_ns;
    unsigned int         duty_ns;

    unsigned int        running;
    unsigned int        pwm_id;
    unsigned int        use_count;
};
#define pwm_dbg(_pwm, msg...) dev_dbg(&(_pwm)->pdev->dev, msg)
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif
