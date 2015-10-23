

#ifndef _WM8990_I2C_H_
#define _WM8990_I2C_H_

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#include "osl_types.h"

int wm8990_reg_read(u8 addr, u16 *data);
int wm8990_reg_write(u8 usRegAddr, u16 usRegData);
int wm8990_reg_writeD(u8 usRegAddr, u16 mask, u16 usRegData);
int wm8990_i2c_init(void);
int wm8990_i2c_deinit();

/*8990寄存器写操作顺序信息统计 */
/*#define WM8990_OPERATE_STAT */

#ifdef WM8990_OPERATE_STAT

typedef enum
{
    WM8990_OP_START = 0x0040,
    WM8990_PROBE,
    WM8990_OP_OPEN,
    WM8990_OP_CLOSE,
    WM8990_OP_PCM_DN,
    WM8990_OP_PCM_UP,
    WM8990_OP_I2S_DN,
    WM8990_OP_I2S_UP,
    WM8990_OP_SAMPLERATE,
    WM8990_OP_IN_SEL,
    WM8990_OP_IN_UNSEL,
    WM8990_OP_OUT_SEL,
    WM8990_OP_OUT_UNSEL,
    WM8990_OP_NV_RW,
    WM8990_OP_PLAY_OPEN,
    WM8990_OP_PLAY_CLOSE,
    WM8990_OP_RECORD_OPEN,
    WM8990_OP_RECORD_CLOSE,
    WM8990_OP_CB_OPEN,
    WM8990_OP_CB_CLOSE,
    WM8990_OP_VOICE_OPEN,
    WM8990_OP_VOICE_CLOSE,
    WM8990_OP_END
}WM8990_OP_FUNC;

void wm8990_func_name_record(WM8990_OP_FUNC x);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif /* _WM8990_I2C_H_ */

