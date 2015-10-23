
/*lint --e{537} */
#include <stdio.h>
#include <osl_types.h>
#include <osl_bio.h>

#include <bsp_om.h>
#include <bsp_dpm.h>
#include <bsp_nvim.h>
#include <bsp_mipi.h>
#include <bsp_tuner.h>

#include <product_config.h>

#include <drv_usb.h>
#include <DrvNvInterface.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_TUNER

u32 g_is_tuner_inited = 0;

int bsp_tuner_resume(struct dpm_device *dev)
{
    bsp_tuner_init();

    return 0;
}

#ifdef CONFIG_CCORE_PM
static struct dpm_device g_tuner_dpm_device={
    .device_name = "tuner_dpm",
    .prepare = NULL,
    .suspend_early= NULL,
    .suspend = NULL,
    .suspend_late = NULL,
    .resume_early = NULL,
    .resume = NULL,
    .resume_late = NULL,
    .complete = bsp_tuner_resume,
};
#endif

void bsp_tuner_init(void)
{
    u32 ret = 0;
    u32 i = 0, j = 0;

    static NV_GU_RF_FEND_MIPI_INIT_CONFIG_STRU tuner_nv_init_data;

    NV_TUNER_MIPI_FRAME_CONFIG_STRU * p_tuner_mipi_frame = &tuner_nv_init_data.RFInitReg[0];

    if (!g_is_tuner_inited)
    {
        memset((void*)&tuner_nv_init_data,0,sizeof(tuner_nv_init_data));

        ret = bsp_nvm_read(NV_GU_RF_FEND_MIPI_INIT_CONFIG_ID,
                           (u8*)&tuner_nv_init_data,
                           sizeof(tuner_nv_init_data));
        if (ret)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_TUNER,
                "fail to read tuner nv, id: %d\n", NV_GU_RF_FEND_MIPI_INIT_CONFIG_ID);
             return;
        }
#ifdef CONFIG_CCORE_PM
        ret = bsp_device_pm_add(&g_tuner_dpm_device);
        if(ret)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_TUNER, "fail to add tuner dpm device\r\n");
            return;
        }
#endif
    }

    for(i = 0; i < TUNER_MAX_NUM ; i++)
    {
        if(0 == p_tuner_mipi_frame->validNum)
        {
            p_tuner_mipi_frame++;
            continue;/*第一个MIPI通道validNum有效FRAME参数为0，则不继续后续动作，转而检查第二MIPI通道*/
        }

        if(TUNER_USER_DATA_MAX_NUM < p_tuner_mipi_frame->validNum)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_TUNER, \
            "p_tuner_mipi_frame->validNum %d > TUNER_USER_DATA_MAX_NUM %d \n",
            p_tuner_mipi_frame->validNum, TUNER_USER_DATA_MAX_NUM);
            return;
        }

        for(j = 0; j< p_tuner_mipi_frame->validNum; j++)
        {
            if((0 == p_tuner_mipi_frame->cmdFrame[j])&&(0 == p_tuner_mipi_frame->dataFrame[j]))
            {
                continue;/*同一FRAME 的CMD&DATA 同时为0 则不下发*/
            }

            bsp_mipi_data_send(MIPI_WRITE,
                (p_tuner_mipi_frame->cmdFrame[j] >> 9) & 0x0F,
                (p_tuner_mipi_frame->dataFrame[j] >> 1) & 0xFF,
                0,
                p_tuner_mipi_frame->mipiChan);
        }

        p_tuner_mipi_frame++;
    }

    if (!g_is_tuner_inited)
    {
        g_is_tuner_inited = 1;
        printf("tuner init ok\n");
    }
}

#else

void bsp_tuner_init(void)
{
}

#endif

#ifdef __cplusplus
}
#endif


