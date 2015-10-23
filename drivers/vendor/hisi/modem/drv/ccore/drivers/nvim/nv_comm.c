

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -save -e537*/
#include <bsp_dump.h>
#include <bsp_hardtimer.h>
#include "nv_comm.h"
/*lint -restore +e537*/

/*lint -save -e826 -e958 -e438*/
static const u32 nv_mid_droit[NV_MID_PRI_LEVEL_NUM] = {1,1,1,1,1,1};

struct nv_global_ctrl_info_stru  g_nv_ctrl;
struct nv_global_debug_stru      g_nv_debug[NV_FUN_MAX_ID];

u16 g_NvSysList[] = { NV_ID_DRV_IMEI,
                      NV_ID_DRV_NV_FACTORY_INFO_I,
                      NV_ID_DRV_NV_DRV_VERSION_REPLACE_I,
                      NV_ID_DRV_SEC_BOOT_ENABLE_FLAG,
                      NV_ID_DRV_SOCP_LOG_CFG};

/*nv debug func*/
void nv_debug(u32 type,u32 reseverd1,u32 reserved2,u32 reserved3,u32 reserved4)
{
    if(0 == reseverd1)
    {
        g_nv_debug[type].callnum++;
    }
    g_nv_debug[type].reseved1 = reseverd1;
    g_nv_debug[type].reseved2 = reserved2;
    g_nv_debug[type].reseved3 = reserved3;
    g_nv_debug[type].reseved4 = reserved4;
}

void nv_modify_print_sw(u32 arg)
{
    g_nv_ctrl.debug_sw = arg;
}
void nv_modify_statis_sw(u32 arg)
{
    g_nv_ctrl.statis_sw = (bool)arg;
}

bool nv_isSysNv(u16 itemid)
{
    u32 i = 0;
    if(itemid >= NV_ID_SYS_MIN_ID && itemid <= NV_ID_SYS_MAX_ID)
        return (bool)true;

    for(i = 0;i<sizeof(g_NvSysList)/sizeof(g_NvSysList[0]);i++)
    {
        if(itemid == g_NvSysList[i])
            return (bool)true;
    }
    return (bool)false;

}
/*lint -save -e429 -e830*/
void nv_AddListNode(u32 itemid)
{
    struct list_head * me = NULL;
    struct nv_write_list_stru * cur = NULL;
    if(g_nv_ctrl.statis_sw == true)
    {
        list_for_each(me, &g_nv_ctrl.stList)
        {
            cur = list_entry(me,struct nv_write_list_stru,stList);/*存在相同的nv id*/
            if(cur->itemid == itemid){
                cur->count ++;
                cur->slice = bsp_get_slice_value() - cur->slice;
                return;
            }
        }
        cur = NULL;
        /* [false alarm]:test using */
        cur = (struct nv_write_list_stru*)nv_malloc(sizeof(struct nv_write_list_stru));/* [false alarm]:test using */
        if(!cur)
            return;
        cur->itemid = itemid;
        cur->count  = 1;
        cur->slice  = bsp_get_slice_value();
        list_add(&cur->stList, &g_nv_ctrl.stList);
    }
    /* coverity[leaked_storage] */
    return;
}
/*lint -restore*/
/*
 * get nv read right,check the nv init state or upgrade state to read nv,
 * A core may read nv data after kernel init ,C core read nv data must behine the phase of
 *       acore kernel init or acore init ok
 */
bool nv_read_right(u32 itemid)
{
    struct nv_global_ddr_info_stru* ddr_info= (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    /*[false alarm]:Value Never Read*/
    itemid = itemid;
    if(   (NV_BOOT_INIT_OK > ddr_info->ccore_init_state)
        ||(NV_KERNEL_INIT_DOING == ddr_info->acore_init_state))
    {
        return (bool)false;
    }

    return (bool)true;
}

bool nv_write_right(u32 itemid)
{
    struct nv_global_ddr_info_stru* ddr_info= (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    /* [false alarm]:Value Never Read */
    itemid = itemid;
    if(   (NV_INIT_OK != ddr_info->ccore_init_state)
        ||(NV_KERNEL_INIT_DOING == ddr_info->acore_init_state))
    {
        return (bool)false;
    }
    return (bool)true;
}


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

    nv_debug(NV_FUN_SEARCH_NV,0,itemid,high,0);

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
    printf("\n[%s]:can not find 0x%x !\n",__FUNCTION__,itemid);
    return BSP_ERR_NV_NO_THIS_ID;
}

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
        show_ddr_info();
        printf("[%s]:offset 0x%x\n",__FUNCTION__,offset);
        return BSP_ERR_NV_FILE_ERROR;
    }


    nv_spin_lock(nvflag, IPC_SEM_NV);
    memcpy((u8*)(NV_GLOBAL_CTRL_INFO_ADDR+ddr_info->file_info[file_id-1].offset + offset),pdata,size);
    nv_flush_cache((u8*)(NV_GLOBAL_CTRL_INFO_ADDR+ddr_info->file_info[file_id-1].offset + offset),size);
    nv_spin_unlock(nvflag, IPC_SEM_NV);

    return NV_OK;
}


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
        show_ddr_info();
        printf("[%s]:offset 0x%x\n",__FUNCTION__,offset);
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
        printf("[%s]:ret 0x%x,len 0x%x\n",__FUNCTION__,ret,len);
        return BSP_ERR_NV_ICC_CHAN_ERR;
    }
    return NV_OK;
}

/*
 *  ccore callback of icc msg.only accept cnf message
 */
s32 nv_icc_msg_proc(u32 chanid ,u32 len,void* pdata)
{
    u32 ret = NV_ERROR;

    pdata = pdata;
    ret = nv_icc_read(chanid,len);
    if(ret)
    {
        osl_sem_up(&g_nv_ctrl.cc_sem);
        printf("[%s] :%d 0x%x\n",__FUNCTION__,__LINE__,ret);
        return (s32)ret;
    }
    osl_sem_up(&g_nv_ctrl.cc_sem);
    return NV_OK;
}

/*
 *  nv use this inter to send data through the icc channel
 */
u32 nv_icc_send(u8* pdata,u32 len)
{
    s32  ret;
    u32 chanid = 0;
    u32 i = 0;
    chanid = ICC_CHN_NV << 16 | NV_RECV_FUNC_AC;

    /*lint -save -e578 -e530*/
    nv_debug_trace(pdata, len);
    /*lint -restore +e578 +e530*/

    for(i = 0;i<NV_ICC_SEND_COUNT;i++)
    {
        ret = bsp_icc_send(ICC_CPU_APP,chanid,pdata,len);
        /*lint -save -e650 -e737*/
        if(ICC_INVALID_NO_FIFO_SPACE == (u32)ret)/*消息队列满,则50ms之后重新发送*/
        /*lint -restore +e650 +e737*/
        {
            /*lint -save -e534*/
            nv_taskdelay(5);
            /*lint -restore +e534*/
            continue;
        }
        else if((u32)ret != len)
        {
            printf("[%s]:ret :0x%x,len 0x%x\n",__FUNCTION__,ret,len);
            return BSP_ERR_NV_ICC_CHAN_ERR;
        }
        else
        {
            return NV_OK;
        }
    }
    system_error(DRV_ERRNO_NV_ICC_FIFO_FULL,ICC_CPU_APP,chanid,(char*)pdata,len);
    return NV_ERROR;
}

/*
 *  init icc channel used by nv module
 */
u32 nv_icc_chan_init(void)
{
    u32 chanid = 0;
    chanid = ICC_CHN_NV << 16 | NV_RECV_FUNC_AC;

    /* [false alarm]:Value Never Read*/
    return (u32)bsp_icc_event_register(chanid,nv_icc_msg_proc,NULL,NULL,NULL);
}

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
    /*[false alarm]:Value Never Read*/
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
        nv_spin_lock(nvflag, IPC_SEM_NV);

        ddr_info->priority[ref_info->priority - NV_MID_PRIORITY1] +=\
            nv_mid_droit[ref_info->priority - NV_MID_PRIORITY1];
        temp_prio = ddr_info->priority[ref_info->priority - NV_MID_PRIORITY1];

        nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);
        nv_spin_unlock(nvflag, IPC_SEM_NV);
    }
    /*[false alarm]:Value Never Read*/
    if(temp_prio >= g_nv_ctrl.mid_prio)
    {
         /*[false alarm]:Value Never Read*/
        off = ddr_info->file_info[ref_info->file_id-1].offset+ref_info->nv_off;
        /* coverity[sleep] */
        ret = bsp_nvm_flushEx(0,ddr_info->file_len,ref_info->itemid);
    }

    if(true == nv_isSysNv(ref_info->itemid))
    {
        /* coverity[sleep] */
        ret = bsp_nvm_flushSys(ref_info->itemid);
    }
    return ret;
}

void show_ddr_info(void)
{
    u32 i;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_file_list_info_stru* file_info = (struct nv_file_list_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE);
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    printf("global start ddr        :0x%x\n",NV_GLOBAL_INFO_ADDR);
    printf("global ctrl file ddr    :0x%x\n",NV_GLOBAL_CTRL_INFO_ADDR);
    printf("global file list ddr    :0x%x\n",file_info);
    printf("global ref info  ddr    :0x%x\n",ref_info);
    printf("*******************ddr global ctrl************************\n");
    printf("acore init state: 0x%x\n",ddr_info->acore_init_state);
    printf("ccore init state: 0x%x\n",ddr_info->ccore_init_state);
    printf("file total len  : 0x%x\n",ddr_info->file_len);
    printf("comm file num   : 0x%x\n",ddr_info->file_num);
    printf("ddr read case  : 0x%x\n",ddr_info->ddr_read);
    printf("mem file type   : 0x%x\n",ddr_info->mem_file_type);
    for(i = 0;i<NV_MID_PRI_LEVEL_NUM;i++)
    {
        printf("mid priority %d  : 0x%x\n",i,ddr_info->priority[i]);
    }


    for(i = 0;i<ddr_info->file_num;i++)
    {
        printf("##############################\n");
        printf("** file id   0x%x\n",ddr_info->file_info[i].file_id);
        printf("** file size 0x%x\n",ddr_info->file_info[i].size);
        printf("** file off  0x%x\n",ddr_info->file_info[i].offset);
    }

    printf("*******************global ctrl file***********************\n");
    printf("ctrl file size    : 0x%x\n",ctrl_info->ctrl_size);
    printf("file num          : 0x%x\n",ctrl_info->file_num);
    printf("file list off     : 0x%x\n",ctrl_info->file_offset);
    printf("file list size    : 0x%x\n",ctrl_info->file_size);
    printf("ctrl file magic   : 0x%x\n",ctrl_info->magicnum);
    printf("modem num         : 0x%x\n",ctrl_info->modem_num);
    printf("nv count          : 0x%x\n",ctrl_info->ref_count);
    printf("nv ref data off   : 0x%x\n",ctrl_info->ref_offset);
    printf("nv ref data size  : 0x%x\n",ctrl_info->ref_size);
    printf("*******************global file list***********************\n");
    for(i = 0;i<ctrl_info->file_num;i++)
    {
        printf("file_info     : 0x%x\n",file_info);
        printf("file id       : 0x%x\n",file_info->file_id);
        printf("file name     : %s\n",file_info->file_name);
        printf("file size     : 0x%x\n",file_info->file_size);
        file_info ++;
        printf("\n");
    }
}


void show_ref_info(u32 arg1,u32 arg2)
{
    u32 i;
    u32 _max;
    u32 _min;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    _max = arg2 > ctrl_info->ref_count ? ctrl_info->ref_count:arg2;
    _min = arg1 > _max ? 0: arg1;

    _max = (arg2 ==0)?ctrl_info->ref_count: _max;

    ref_info = ref_info+_min;

    for(i = _min;i<_max;i++)
    {
        printf("第%d项 :\n",i);
        printf("nvid   :0x%-8x, file id : 0x%-8x\n",ref_info->itemid,ref_info->file_id);
        printf("nvlen  :0x%-8x, nv_off  : 0x%-8x, nv_pri 0x%-8x\n",ref_info->nv_len,ref_info->nv_off,ref_info->priority);
        printf("dsda   :0x%-8x\n",ref_info->modem_num);
        ref_info++;
    }
}

void nv_help(u32 type)
{
    u32 i = type;
    /* [false alarm]:Redundant Initialization*/
    if(type == 63/*'?'*/)
    {
        printf("1   -------  read\n");
        printf("4   -------  auth read\n");
        printf("5   -------  write\n");
        printf("6   -------  auth write\n");
        printf("8   -------  get len\n");
        printf("9   -------  auth get len\n");
        printf("10  -------  flush\n");
        printf("12  -------  backup\n");
        printf("15  -------  import\n");
        printf("16  -------  export\n");
        printf("19  -------  kernel init\n");
        printf("20  -------  remian init\n");
        printf("21  -------  nvm init\n");
        printf("22  -------  xml decode\n");
        printf("24  -------  revert\n");
        printf("25  -------  update default\n");
        return;

    }
    if(type == NV_FUN_MAX_ID)
    {
        for(i = 0;i< NV_FUN_MAX_ID;i++)
        {
            printf("************fun id %d******************\n",i);
            printf("call num             : 0x%x\n",g_nv_debug[i].callnum);
            printf("out branch (reseved1): 0x%x\n",g_nv_debug[i].reseved1);
            printf("reseved2             : 0x%x\n",g_nv_debug[i].reseved2);
            printf("reseved3             : 0x%x\n",g_nv_debug[i].reseved3);
            printf("reseved4             : 0x%x\n",g_nv_debug[i].reseved4);
            printf("***************************************\n");
        }
        return ;
    }

    i = type;
    printf("************fun id %d******************\n",i);
    printf("call num             : 0x%x\n",g_nv_debug[i].callnum);
    printf("out branch (reseved1): 0x%x\n",g_nv_debug[i].reseved1);
    printf("reseved2             : 0x%x\n",g_nv_debug[i].reseved2);
    printf("reseved3             : 0x%x\n",g_nv_debug[i].reseved3);
    printf("reseved4             : 0x%x\n",g_nv_debug[i].reseved4);
    printf("***************************************\n");
}
void nv_show_listInfo(void)
{
    struct list_head *me = NULL;
    struct nv_write_list_stru * cur = NULL;
    list_for_each(me,&g_nv_ctrl.stList)
    {
        cur = list_entry(me, struct nv_write_list_stru, stList);
        printf("NV ID :0x%4x 写入 %d 次，最近写入间隔:0x%x\n",cur->itemid,cur->count,cur->slice);
    }
}
 /*lint -restore*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




