

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include "nv_comm.h"
/*lint -restore +e537*/

/*lint -save -e530 -e438*/
static const u32 nv_mid_droit[NV_MID_PRI_LEVEL_NUM] = {1,1,1,1,1,1};

struct nv_global_ctrl_info_stru  g_nv_ctrl;


/*
 * get nv read right,check the nv init state or upgrade state to read nv,
 * A core may read nv data after kernel init ,C core read nv data must behine the phase of
 *       acore kernel init or acore init ok
 */
bool nv_read_right(u32 itemid)
{
    struct nv_global_ddr_info_stru* ddr_info= (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    if(   (NV_BOOT_INIT_OK > ddr_info->mcore_init_state)
        ||(NV_KERNEL_INIT_DOING == ddr_info->acore_init_state))
    {
        return false;
    }
    return true;
}

#ifndef BSP_CONFIG_HI3630
bool nv_write_right(u32 itemid)
{
    struct nv_global_ddr_info_stru* ddr_info= (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    if(   (NV_INIT_OK != ddr_info->mcore_init_state)
        ||(NV_KERNEL_INIT_DOING == ddr_info->acore_init_state))
    {
        return false;
    }
    return true;
}
#endif

/*
 * search nv info by nv id
 * &pdata:  data start ddr
 * output: ref_info,file_info
 */
u32 nv_search_byid(u32 itemid,u8* pdata,struct nv_ref_data_info_stru* ref_info,struct nv_file_list_info_stru* file_info)
{
    u32 low;
    u32 high;
    u32 mid;
    u32 offset;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)pdata;

    high = ctrl_info->ref_count;
    low  = 1;

    while(low <= high)
    {
        mid = (low+high)/2;

        offset =NV_GLOBAL_CTRL_INFO_SIZE +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num + (mid -1)*NV_REF_LIST_ITEM_SIZE;
        memcpy((u8*)ref_info,(u8*)ctrl_info+offset,NV_REF_LIST_ITEM_SIZE);

        if(itemid < ref_info->itemid)
        {
            high = mid -1;
        }
        else if(itemid > ref_info->itemid)
        {
            low = mid +1;
        }
        else
        {
            offset = NV_GLOBAL_CTRL_INFO_SIZE + NV_GLOBAL_FILE_ELEMENT_SIZE*(ref_info->file_id -1);
            memcpy((u8*)file_info,(u8*)ctrl_info+offset,NV_GLOBAL_FILE_ELEMENT_SIZE);
            return NV_OK;
        }
    }
    return BSP_ERR_NV_NO_THIS_ID;

}

#ifndef BSP_CONFIG_HI3630
/*
 * copy user buff to global ddr,used to write nv data to ddr
 * &file_id :file id
 * &offset:  offset of global file ddr
 */
u32 nv_write_to_mem(u8* pdata,u32 size,u32 file_id,u32 offset)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    unsigned long nvflag;
    if(offset > ddr_info->file_info[file_id-1].size)
    {
        return BSP_ERR_NV_FILE_ERROR;
    }

    /*lint -save -e732*/
    nv_spin_lock(nvflag, IPC_SEM_NV);
    memcpy((u8*)(NV_GLOBAL_CTRL_INFO_ADDR + ddr_info->file_info[file_id-1].offset + offset),pdata,size);
    nv_spin_unlock(nvflag, IPC_SEM_NV);
    /*lint -restore*/

    return NV_OK;
}
#endif

/*
 * copy global ddr to user buff,used to read nv data from ddr
 * &file_id : file id
 * &offset:  offset of the file
 */
u32 nv_read_from_mem(u8* pdata,u32 size,u32 file_id,u32 offset)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    if(offset > ddr_info->file_info[file_id-1].size)
    {
        return BSP_ERR_NV_FILE_ERROR;
    }

    memcpy(pdata,(u8*)(NV_GLOBAL_CTRL_INFO_ADDR +ddr_info->file_info[file_id-1].offset + offset),size);

    return NV_OK;
}


/*
 *  nv read data from global ddr of icc through the chanel id
 */
u32 nv_icc_read(u32 chanid, u32 len)
{
    u32 ret = NV_ERROR;

    memset(g_nv_ctrl.nv_icc_buf,0,NV_ICC_BUF_LEN);
    ret = (u32)bsp_icc_read(chanid,g_nv_ctrl.nv_icc_buf,len);
    if(len != ret)
    {
        return BSP_ERR_NV_ICC_CHAN_ERR;
    }
    return NV_OK;
}

/*
 *  m3 callback of icc msg.only accept cnf message
 */
s32 nv_icc_msg_proc(u32 chanid ,u32 len,void* pdata)
{
    return NV_OK;
}

/*
 *  nv use this inter to send data through the icc channel
 */
u32 nv_icc_send(u8* pdata,u32 len)
{
    u32  ret = NV_ERROR;
    u32 chanid = 0;
    chanid = ICC_CHN_MCORE_ACORE << 16 | NV_RECV_FUNC_AM;

    ret = (u32)bsp_icc_send(ICC_CPU_APP,chanid,pdata,len);
    if(len != ret)
    {
        return BSP_ERR_NV_ICC_CHAN_ERR;
    }
    return NV_OK;
}

/*
 *  init icc channel used by nv module
 */
u32 nv_icc_chan_init(void)
{
    u32 chanid = 0;
    chanid = ICC_CHN_MCORE_ACORE << 16 | NV_RECV_FUNC_AM;

    return (u32)bsp_icc_event_register(chanid,nv_icc_msg_proc,NULL,NULL,NULL);
}

#ifndef BSP_CONFIG_HI3630
/*
 *  write data to file/flash/rfile,base the nv priority,inner packing write to ddr
 *  &pdata:    user buff
 *  &offset:   offset of nv in ddr
 *  &len :     data length
 */
u32 nv_write_to_file(struct nv_ref_data_info_stru* ref_info)
{
    u32 ret = NV_OK;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    struct nv_ctrl_file_info_stru*ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    u32 off;
    u32 temp_prio = 0;
    unsigned long nvflag;

    if(NV_HIGH_PRIORITY == ref_info->priority)
    {
        off = ddr_info->file_info[ref_info->file_id-1].offset+ref_info->nv_off;
        ret = bsp_nvm_flushEx(off,ref_info->nv_len,ref_info->itemid);
    }
    else if(NV_LOW_PRIORITY == ref_info->priority)
    {
        return ret;
    }
    else
    {
        /*lint -save -e732*/
        nv_spin_lock(nvflag, IPC_SEM_NV);

        ddr_info->priority[ref_info->priority - NV_MID_PRIORITY1] +=\
            nv_mid_droit[ref_info->priority - NV_MID_PRIORITY1];
        temp_prio = ddr_info->priority[ref_info->priority - NV_MID_PRIORITY1];

        nv_spin_unlock(nvflag, IPC_SEM_NV);
        /*lint -restore*/
    }
    if(temp_prio >= g_nv_ctrl.mid_prio)
    {
        off = ddr_info->file_info[ref_info->file_id-1].offset+ref_info->nv_off;
        ret = bsp_nvm_flushEx(ctrl_info->ctrl_size,(ddr_info->file_len-ctrl_info->ctrl_size),ref_info->itemid);
    }

    ret |= bsp_nvm_flushSys(ref_info->itemid);
    return ret;
}
#endif
/*lint -restore*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




