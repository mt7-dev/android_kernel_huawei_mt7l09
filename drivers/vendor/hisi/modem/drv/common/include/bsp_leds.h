#include <bsp_icc.h>

extern unsigned int led_debug_level;    /* just for debug */

/* led ccore and acore icc channel */
#define LED_ICC_CHN_ID                          ((ICC_CHN_IFC << 16) | IFC_RECV_FUNC_LED)
#define LED_TIME_BASE_UNIT                      (100)         /* 三色灯工作的基本时间单位 ms */

/* some value */
#define LED_OK      0
#define LED_ERROR   -1
#define LED_VALUE_INVALID   DR_VALUE_INVALIED

/* for debug */
#define LED_DEBUG_INFO      0x00000001
#define LED_DEBUG_WARNING   0x00000002
#define LED_DEBUG_ERROR     0x00000004
#define LED_DEBUG_ALWAYS    0xFFFFFFF0
#define LED_DEBUG_ALL       0xFFFFFFFF

#define LED_DEBUG_LVL            (LED_DEBUG_ALWAYS | LED_DEBUG_ERROR)
#define LED_DEBUG_LEVEL(LEVEL)   (LED_DEBUG_##LEVEL)

/* trace */
#define LED_TRACE(loglevel, msg, ...)    \
{                                   \
    /*lint -save -e506 -e413 -e774 -e778 -e831 -e944*/                                            \
    if((loglevel & LED_DEBUG_LVL) || (loglevel & led_debug_level))    \
    {                                        \
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_LED, msg, ##__VA_ARGS__);                    \
    }                                       \
    /*lint -restore */                                                  \
}

int bsp_led_init(void);

