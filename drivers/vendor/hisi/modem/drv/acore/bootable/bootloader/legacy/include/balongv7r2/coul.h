#ifndef __COUL_H__
#define __COUL_H__

typedef enum{
    BATT_CAPACITY = 0,
    BATT_VOLTAGE,
    BATT_CURRENT,
    BATT_VOLT_CALI,
    BATT_CHARGE_TIME,
    BATT_DISCHARGE_TIME,
    BATT_INFO_BUILT
}COUL_BATT_INFO;

typedef enum{
    COUL_POWER_OFF = 0,
    COUL_POWER_ON,
    COUL_POWER_BUILT
}COUL_POWER_CTRL;

typedef enum{
    COUL_REFLASH_ALL = 0,
    COUL_REFLASH_ECO,
    COUL_REFLASH_BUILT
}COUL_REFLASH_CTRL;

typedef enum{
    COUL_ECO_EXIT = 0,
    COUL_ECO_FORCE,
    COUL_ECO_BUILT
}COUL_ECO_CTRL;

typedef enum{
    COUL_CALI_OFF = 0,
    COUL_CALI_ON,
    COUL_CALI_BUILT
}COUL_CALI_CTRL;

#define COUL_CTRL_ONOFF_MASK  (1<<7)
#define COUL_CTRL_CALI_MASK   (1<<6)
#define COUL_CTRL_REFLASH_MASK (1<<3)
#define COUL_CTRL_ECO_MASK      (1<<2)
#define COUL_CTRL_FILETR_MASK        (3<<4)/*bit4,bit5*/
#define COUL_STATE_MASK     0x7 /*bit0,1,2*/

typedef void (*COUL_INT_FUNC)(void *);


#define coul_dbg(fmt, ...)    \
do {                               \
       /*cprintf*/\
} while (0)
#define coul_err(fmt, ...)  (cprintf("[coul]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#define coul_info(fmt, ...)   \
do {                               \
       /*cprintf*/\
} while (0)

#define BSP_COUL_OK (0)
#define BSP_COUL_ERR (-1)


#endif
