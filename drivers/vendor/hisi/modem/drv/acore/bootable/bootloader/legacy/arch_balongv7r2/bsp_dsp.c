/*lint --e{537} */
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>
#include <bsp_dsp.h>
#include <ios.h>
#include <boot/boot.h>
#include <balongv7r2/types.h>
#include <balongv7r2/version.h>

#include <product_config.h>

#ifdef __cplusplus
extern "C" {
#endif

int bsp_dsp_is_hifi_exist(void)
{
    int ret = 0;
    DRV_MODULE_SUPPORT_STRU   stSupportNv = {0};

    ret = bsp_nvm_read(NV_ID_DRV_MODULE_SUPPORT, (u8*)&stSupportNv, sizeof(DRV_MODULE_SUPPORT_STRU));
    if (ret)
        ret = 0;
    else
        ret = (int)stSupportNv.hifi;

    return ret;
}

void bsp_dsp_init()
{
    u32 board_type;

    board_type = bsp_version_get_board_chip_type();
    if (board_type != HW_VER_PRODUCT_UDP)
        return;

    if (bsp_dsp_is_hifi_exist())
    {
        pcm_io_mux();
        cprintf("pcm io init ok\n");
    }

    return;
}


#ifdef __cplusplus
}
#endif

