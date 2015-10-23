#ifndef __PINTRL_BALONG_H__
#define __PINTRL_BALONG_H__

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    MODEM_PIN_INIT = 0,
    MODEM_PIN_NORMAL,
    MODEM_PIN_POWERDOWN,
    MODEM_PIN_BUTT,
};

enum
{
    MODEM_IO_MUX = 0,
    MODEM_IO_CFG,
    MODEM_GPIO_INPUT,
    MODEM_GPIO_OUTPUT,
    MODEM_IO_BUTT,
};

struct pintrl_stru{
	unsigned int ugpiog;
	unsigned int ugpio_bit;
	unsigned int uiomg_off;
	unsigned int iomg_val;
	unsigned int uiocg_off;
	unsigned int iocg_val;
	unsigned int gpio_dir;
	unsigned int gpio_val;
};

#ifdef CONFIG_MODEM_PINTRL
void modem_pintrl_config(int mode);

void bsp_set_pintrl(unsigned int type, unsigned int gpio, unsigned int value);

int bsp_get_pintrl(unsigned int type, unsigned int gpio, unsigned int *value);

int is_in_modem_pintrl(unsigned int gpio);

#else

static inline void modem_pintrl_config(int mode)
{
    return ;
}


static inline void bsp_set_pintrl(unsigned int type, unsigned int gpio, unsigned int value)
{
    return ;
}

static inline int bsp_get_pintrl(unsigned int type, unsigned int gpio, unsigned int *value)
{
    return -1;
}

static inline int is_in_modem_pintrl(unsigned int gpio)
{
    return -1;
}

#endif

#ifdef __cplusplus
}
#endif

#endif

