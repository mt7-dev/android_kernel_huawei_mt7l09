
#include "drv_clk.h"
#if 0

#include "drv_clk.h"
extern struct clk_lookup hi6930_clk_lookup[];
extern struct crg_tag reg_message[];

unsigned int DRV_PWRCTRL_PLL_ENABLE (mode md_id, module_id mdu_id)
{
        int ret = 0;
        if((md_id < PWC_COMM_MODE_WCDMA) ||(md_id >= PWC_COMM_MODE_BUTT))
        {
            logMsg("BSP_PWRCTRL_PllEnable enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
            return 1;
        }
        if((mdu_id < PWC_COMM_MODULE_PA)||( mdu_id >= PWC_COMM_MODULE_BUTT))
        {
            logMsg("BSP_PWRCTRL_PllEnable enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
            return 1;
        }
        return 0;/*此处用于打桩*/

        unsigned int clk_id = md_id + mdu_id;
        char *clk_name = hi6930_clk_lookup[clk_id].con_id;
        struct clk *clk = clk_get(NULL, clk_name);
        if(!clk)
            return 1;
        ret = clk_enable(clk);
        if(ret){
            return 1;
         }
         return 0;
}

unsigned int DRV_PWRCTRL_PLL_DISABLE (mode md_id, module_id mdu_id)
{
        if((md_id < PWC_COMM_MODE_WCDMA) ||(md_id >= PWC_COMM_MODE_BUTT))
        {
            logMsg("BSP_PWRCTRL_PllEnable enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
            return 1;
        }
        if((mdu_id < PWC_COMM_MODULE_PA)||( mdu_id >= PWC_COMM_MODULE_BUTT))
        {
            logMsg("BSP_PWRCTRL_PllEnable enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
            return 1;
        }
        return 0;/*此处用于打桩*/

        unsigned int clk_id = md_id + mdu_id;
        char *clk_name = hi6930_clk_lookup[clk_id].con_id;
        struct clk *clk = clk_get(NULL, clk_name);
        if(!clk)
            return 1;
        clk_disable(clk);
        return 0;
}
unsigned int DRV_PWRCTRL_PllStatusGet (mode md_id, module_id mdu_id)
{
        if((md_id < PWC_COMM_MODE_WCDMA) ||(md_id >= PWC_COMM_MODE_BUTT))
        {
            logMsg("BSP_PWRCTRL_PllEnable enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
            return 1;
        }
        if((mdu_id < PWC_COMM_MODULE_PA)||( mdu_id >= PWC_COMM_MODULE_BUTT))
        {
            logMsg("BSP_PWRCTRL_PllEnable enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
            return 1;
        }
        return 0;/*此处用于打桩*/

        unsigned int clk_id = md_id + mdu_id;
        char *clk_name = hi6930_clk_lookup[clk_id].con_id;
        struct clk *clk = clk_get(NULL, clk_name);
        if (reg_message[clk->clkid].enable_flag){
                int ret = clk->ops->isenable(clk);
                if(ret){
                        return 1;
                 }
        }
        return 0;
}
#endif
