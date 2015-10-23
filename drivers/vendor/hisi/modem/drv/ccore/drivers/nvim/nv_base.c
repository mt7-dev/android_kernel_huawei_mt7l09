

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -save -e537*/
#include <drv_sync.h>
#include <bsp_nvim.h>
#include <bsp_hardtimer.h>
#include "nv_comm.h"
/*lint -restore +e537*/

u32 nv_readEx(u32 modem_id,u32 itemid,u32 offset,u8* pdata,u32 datalen)
{
    u32 ret = NV_ERROR;
    struct nv_file_list_info_stru file_info = {0};
    struct nv_ref_data_info_stru  ref_info  = {0};

    nv_debug(NV_FUN_READ_EX,0,itemid,modem_id,datalen);

    if((NULL == pdata)||(0 == datalen))
    {
        nv_debug(NV_FUN_READ_EX,1,itemid,0,0);
        ret = BSP_ERR_NV_INVALID_PARAM;
        goto nv_readEx_err;
    }

    /*lint -save -e826*/
    ret = nv_search_byid(itemid,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
    /*lint -restore +e826*/
    if(ret)
    {
        nv_debug(NV_FUN_READ_EX,2,itemid,ret,0);
        return BSP_ERR_NV_NO_THIS_ID;
    }

    if((offset + datalen) > ref_info.nv_len)
    {
        ret = BSP_ERR_NV_ITEM_LEN_ERR;
        nv_debug(NV_FUN_READ_EX,4,offset,datalen,ref_info.nv_len);
        goto nv_readEx_err;
    }
    if(modem_id <= ref_info.modem_num)
    {
        ref_info.nv_off += (modem_id - NV_USIMM_CARD_1)*ref_info.nv_len;
    }
    else
    {
#if (FEATURE_OFF == FEATURE_MULTI_MODEM)
        ret = BSP_ERR_NV_INVALID_PARAM;
        nv_debug(NV_FUN_READ_EX,5,ret,itemid,modem_id);
        goto nv_readEx_err;
#endif
    }

    ret = nv_read_from_mem(pdata,datalen,file_info.file_id,ref_info.nv_off+offset);
    if(ret)
    {
        nv_debug(NV_FUN_READ_EX,6,offset,datalen,ref_info.nv_len);
        goto nv_readEx_err;
    }
    /*lint -save -e578 -e530*/
    nv_debug_trace(pdata,datalen);
    /*lint -restore +e578 +e530*/

    return NV_OK;
nv_readEx_err:
    printf("\n[%s]:[0x%x]:[%d]\n",__FUNCTION__,itemid,modem_id);
    nv_help(NV_FUN_READ_EX);
    return ret;
}
u32 nv_writeEx(u32 modem_id,u32 itemid,u32 offset,u8* pdata,u32 datalen)
{
    u32 ret = NV_ERROR;
    struct nv_file_list_info_stru file_info = {0};
    struct nv_ref_data_info_stru  ref_info  = {0};

    nv_debug(NV_FUN_WRITE_EX,0,itemid,modem_id,datalen);

    if((NULL == pdata)||(0 == datalen))
    {
        nv_debug(NV_FUN_WRITE_EX,1,itemid,datalen,0);
        ret = BSP_ERR_NV_INVALID_PARAM;
        goto nv_writeEx_err;
    }

    ret = nv_search_byid(itemid,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
    if(ret)
    {

        nv_debug(NV_FUN_WRITE_EX,2,itemid,datalen,ret);
        return BSP_ERR_NV_NO_THIS_ID;
    }

    /*lint -save -e578 -e530*/
    nv_debug_trace(pdata, datalen);
    /*lint -restore +e578 +e530*/

    if((datalen + offset) >ref_info.nv_len)
    {
        ret = BSP_ERR_NV_ITEM_LEN_ERR;
        nv_debug(NV_FUN_WRITE_EX,3,itemid,datalen,ref_info.nv_len);
        goto nv_writeEx_err;
    }
    if(modem_id <= ref_info.modem_num)
    {
        ref_info.nv_off += (modem_id - NV_USIMM_CARD_1)*ref_info.nv_len;
    }
    else
    {

#if (FEATURE_OFF == FEATURE_MULTI_MODEM)
        ret = BSP_ERR_NV_INVALID_PARAM;
        nv_debug(NV_FUN_WRITE_EX,4,itemid,ret,modem_id);
        goto nv_writeEx_err;
#endif
    }
    ret = nv_write_to_mem(pdata,datalen,file_info.file_id,ref_info.nv_off+offset);
    if(ret)
    {
        nv_debug(NV_FUN_WRITE_EX,5,itemid,datalen,0);
        goto nv_writeEx_err;
    }

    ret = nv_write_to_file(&ref_info);
    if(ret)
    {
        nv_debug(NV_FUN_WRITE_EX,6,itemid,datalen,ret);
        goto nv_writeEx_err;
    }
    nv_AddListNode(itemid);
    return NV_OK;
nv_writeEx_err:
    printf("\n[%s]:[0x%x]:[%d]\n",__FUNCTION__,itemid,modem_id);
    nv_help(NV_FUN_WRITE_EX);
    return ret;
}

u32 bsp_nvm_get_nv_num(void)
{
    /*lint -save -e826*/
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    /*lint -restore +e826*/
    return ctrl_info->ref_count;
}

u32 bsp_nvm_get_nvidlist(NV_LIST_INFO_STRU*  nvlist)
{
    u32 i;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    if(NULL == nvlist)
    {
        return NV_ERROR;
    }

    for(i = 0;i<ctrl_info->ref_count;i++)
    {
        nvlist[i].usNvId       = ref_info[i].itemid;
        nvlist[i].ucNvModemNum = ref_info[i].modem_num;
    }
    return NV_OK;
}

u32 bsp_nvm_get_len(u32 itemid,u32* len)
{
    u32 ret  = NV_ERROR;
    struct nv_ref_data_info_stru ref_info = {0};
    struct nv_file_list_info_stru file_info = {0};

    nv_debug(NV_API_GETLEN,0,itemid,0,0);

    if(NULL == len)
    {
        nv_debug(NV_API_GETLEN,1,itemid,0,0);
        return BSP_ERR_NV_INVALID_PARAM;
    }

    /*check init state*/
    if(false == nv_read_right(itemid))
    {
        nv_debug(NV_API_GETLEN,3,itemid,0,0);
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }
    ret = nv_search_byid(itemid,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info, &file_info);
    if(NV_OK == ret)
    {
        *len = ref_info.nv_len;
        return NV_OK;
    }
    return ret;
}
u32 bsp_nvm_authgetlen(u32 itemid,u32* len)
{
    return bsp_nvm_get_len(itemid,len);
}

u32 bsp_nvm_dcread_direct(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return nv_readEx(modem_id,itemid,0,pdata,datalen);
}

u32 bsp_nvm_dcread(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    /*check init state*/
    if(false == nv_read_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_readEx(modem_id,itemid,0,pdata,datalen);
}
u32 bsp_nvm_auth_dcread(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return bsp_nvm_dcread(modem_id,itemid,pdata,datalen);
}

u32 bsp_nvm_dcreadpart(u32 modem_id,u32 itemid,u32 offset,u8* pdata,u32 datalen)
{
    /*check init state*/
    if(false == nv_read_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_readEx(modem_id,itemid,offset,pdata,datalen);
}
u32 bsp_nvm_dcwrite(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    if(false == nv_write_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_writeEx(modem_id,itemid,0,pdata,datalen);
}

u32 bsp_nvm_auth_dcwrite(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return bsp_nvm_dcwrite(modem_id,itemid,pdata,datalen);
}

u32 bsp_nvm_dcwritepart(u32 modem_id,u32 itemid, u32 offset,u8* pdata,u32 datalen)
{
    if(false == nv_write_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_writeEx(modem_id,itemid,offset,pdata,datalen);
}

u32 bsp_nvm_dcwrite_direct(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return nv_writeEx(modem_id,itemid,0,pdata,datalen);
}

u32 bsp_nvm_flushEx(u32 off,u32 len,u32 itemid)
{
    u32 ret = NV_ERROR;
    struct nv_icc_stru icc_req = {0};
    struct nv_icc_stru icc_cnf = {0};
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_debug(NV_API_FLUSH,0,0,0,0);

    if(NV_INIT_OK != ddr_info->ccore_init_state)
    {
        nv_debug(NV_API_FLUSH,1,ddr_info->ccore_init_state,0,0);
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    icc_req.msg_type = NV_ICC_REQ;
    icc_req.data_len = len;
    icc_req.data_off = off;
    icc_req.ret      = 93;   /*use to test ,no meaning*/
    icc_req.itemid   = itemid;
    icc_req.slice    = bsp_get_slice_value();

    ret = nv_icc_send((u8*)&icc_req,sizeof(icc_req));
    if(ret)
    {
        nv_debug(NV_API_FLUSH,2,0,ret,0);
        return ret;
    }
    /*lint -save -e534*/
    osl_sem_downtimeout(&g_nv_ctrl.cc_sem,NV_MAX_WAIT_TICK);
    /*lint -restore +e534*/

    memcpy(&icc_cnf,g_nv_ctrl.nv_icc_buf,sizeof(icc_cnf));
    if(icc_cnf.msg_type != NV_ICC_CNF)
    {

        nv_debug(NV_API_FLUSH,3,0,0,icc_cnf.msg_type);
        printf("\n[%s]\n",__FUNCTION__);
        nv_help(NV_API_FLUSH);
        return BSP_ERR_NV_INVALID_PARAM;
    }
    return icc_cnf.ret;
}
u32 bsp_nvm_flush(void)
{

    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    return bsp_nvm_flushEx(0,ddr_info->file_len,NV_ERROR);
}


u32 bsp_nvm_flushSys(u32 itemid)
{
    u32 ret = NV_ERROR;
    struct nv_icc_stru icc_req = {0};
    struct nv_icc_stru icc_cnf = {0};
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_debug(NV_API_FLUSH,0,0,0,0);

    if(NV_INIT_OK != ddr_info->ccore_init_state)
    {
        nv_debug(NV_API_FLUSH,1,ddr_info->ccore_init_state,0,0);
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    icc_req.msg_type = NV_ICC_REQ_SYS;
    icc_req.data_len = ddr_info->file_len;
    icc_req.data_off = 0;
    icc_req.ret      = 93;   /*use to test ,no meaning*/
    icc_req.itemid   = itemid;
    icc_req.slice    = bsp_get_slice_value();

    ret = nv_icc_send((u8*)&icc_req,sizeof(icc_req));
    if(ret)
    {
        nv_debug(NV_API_FLUSH,2,0,ret,0);
        return ret;
    }
    /*lint -save -e534*/
    osl_sem_downtimeout(&g_nv_ctrl.cc_sem,NV_MAX_WAIT_TICK);
    /*lint -restore +e534*/

    memcpy(&icc_cnf,g_nv_ctrl.nv_icc_buf,sizeof(icc_cnf));
    if(icc_cnf.msg_type != NV_ICC_CNF)
    {
        nv_debug(NV_API_FLUSH,3,0,0,icc_cnf.msg_type);
        printf("\n[%s]\n",__FUNCTION__);
        nv_help(NV_API_FLUSH);
        return BSP_ERR_NV_INVALID_PARAM;
    }
    return icc_cnf.ret;
}
u32 bsp_nvm_init(void)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

#ifdef BSP_CONFIG_HI3630
    nv_printf("waiting for ap modem nv init ok .......\n");
    BSP_SYNC_Wait(SYNC_MODULE_NV,0);
#endif

    nv_debug(NV_FUN_NVM_INIT,0,0,0,0);
    if(ddr_info->ccore_init_state < NV_BOOT_INIT_OK)
    {
        nv_printf("[%s]:pre init fail,break here!\n",__FUNCTION__);
        nv_debug(NV_FUN_NVM_INIT,1,0,0,0);
        goto nv_init_fail;
    }
    g_nv_ctrl.shared_addr = NV_GLOBAL_INFO_ADDR;
    spin_lock_init(&g_nv_ctrl.spinlock);

    ret = nv_icc_chan_init();
    if(ret)
    {
        nv_debug(NV_FUN_NVM_INIT,2,ret,0,0);
        goto nv_init_fail;
    }
    osl_sem_init(1,&g_nv_ctrl.rw_sem);
    osl_sem_init(0,&g_nv_ctrl.cc_sem);

    ret = bsp_nvm_read(NV_ID_MSP_FLASH_LESS_MID_THRED,(u8*)&g_nv_ctrl.mid_prio,sizeof(u32));
    if(ret)
    {
        g_nv_ctrl.mid_prio = 20;
        nv_printf("read 0x%x error : 0x%x,use default count\n",NV_ID_MSP_FLASH_LESS_MID_THRED,ret);
    }
    ddr_info->ccore_init_state = NV_INIT_OK;
    nv_printf("nv init ok !\n");

    INIT_LIST_HEAD(&g_nv_ctrl.stList);
    return NV_OK;
nv_init_fail:
    ddr_info->ccore_init_state = NV_INIT_FAIL;
    nv_printf("\n[%s]\n",__FUNCTION__);
    nv_help(NV_FUN_NVM_INIT);
    return ret;
}



u32 nvm_read_rand(u32 nvid)
{
    u32 ret;
    u8* tempdata;
    u32 i= 0;
    struct nv_ref_data_info_stru ref_info = {0};
    struct nv_file_list_info_stru file_info = {0};

    ret = nv_search_byid(nvid, (u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
    if(NV_OK != ret)
    {
        return ret;
    }
    if(ref_info.nv_len == 0)
    {
        return NV_ERROR;
    }

    printf("[0x%x]:len 0x%x,off 0x%x,file id %d\n",nvid,ref_info.nv_len,ref_info.nv_off,ref_info.file_id);

    /*lint -save -e516*/
    tempdata = (u8*)nv_malloc(ref_info.nv_len);
    /*lint -restore +e516*/
    if(NULL == tempdata)
    {
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    ret = bsp_nvm_read(nvid,tempdata,ref_info.nv_len);
    if(NV_OK != ret)
    {
        nv_free(tempdata);
        return BSP_ERR_NV_READ_DATA_FAIL;
    }

    for(i=0;i<ref_info.nv_len;i++)
    {
        if((i%32) == 0)
        {
            printf("\n");
        }
        printf("%02x ",(u8)(*(tempdata+i)));
    }

    printf("\n\n");
    nv_free(tempdata);
    return 0;
}


u32 nvm_read_randex(u32 nvid,u32 modem_id)
{
	u32 ret;    u8* tempdata;    u32 i= 0;
	struct nv_ref_data_info_stru ref_info = {0};
	struct nv_file_list_info_stru file_info = {0};

	ret = nv_search_byid(nvid, (u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
	if(NV_OK != ret)
	{
		return ret;
	}
	if(ref_info.nv_len == 0)
	{
		return NV_ERROR;
	}

	printf("[0x%x]:len 0x%x,off 0x%x,file id %d\n",nvid,ref_info.nv_len,ref_info.nv_off,ref_info.file_id);
	printf("[0x%x]:dsda 0x%x\n",nvid,ref_info.modem_num);


	/*lint -save -e516*/
	tempdata = (u8*)nv_malloc(ref_info.nv_len +1);
	/*lint -restore +e516*/
	if(NULL == tempdata)
	{
		return BSP_ERR_NV_MALLOC_FAIL;
	}
	ret = bsp_nvm_dcread(modem_id,nvid,tempdata,ref_info.nv_len);
	if(NV_OK != ret)
	{
		nv_free(tempdata);
		return BSP_ERR_NV_READ_DATA_FAIL;
	}

	for(i=0;i<ref_info.nv_len;i++)
	{
		if((i%32) == 0)
		{
			printf("\n");
		}
		printf("%02x ",(u8)(*(tempdata+i)));
	}

	printf("\n\n");
	nv_free(tempdata);

	return 0;

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




