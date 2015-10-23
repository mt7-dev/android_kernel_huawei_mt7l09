

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "bsp_nvim.h"
#include "nv_comm.h"




u32 nv_init_test(void)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    if(ddr_info->ccore_init_state == NV_INIT_OK)
    {
        return NV_OK;
    }

    ret = bsp_nvm_init();

    return ret;

}

u32 nv_read_part_test(u32 nvid,u32 off,u32 len)
{
    u32 ret;
    u8* tempdata;
    u32 i= 0;

    tempdata = (u8*)nv_malloc(len +1);
    if(NULL == tempdata)
    {
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    ret = bsp_nvm_readpart(nvid,off,tempdata,len);
    if(NV_OK != ret)
    {
        nv_free(tempdata);
        return BSP_ERR_NV_READ_DATA_FAIL;
    }

    for(i=0;i<len;i++)
    {
        if((i%16) == 0)
        {
            printf("\n");
        }
        printf("%2.2x ",(u8)(*(tempdata+i)));
    }

    printf("\n");
    return 0;
}

u32 nv_read_rand_test(u32 nvid)
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

    tempdata = (u8*)nv_malloc(ref_info.nv_len +1);
    if(NULL == tempdata)
    {
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    ret = bsp_nvm_read(nvid,tempdata,ref_info.nv_len);
    if(NV_OK != ret)
    {
        return BSP_ERR_NV_READ_DATA_FAIL;
    }

    for(i=0;i<ref_info.nv_len;i++)
    {
        if((i%16) == 0)
        {
            printf("\n");
        }
        printf("%2.2x ",(u8)(*(tempdata+i)));
    }

    printf("\n");
    return 0;
}


/*get len test*/
u32 nv_get_len_test01(void)
{
    u32 ret;
    u32 len = 0;
    u32 itemid = 0xd007;

    ret = bsp_nvm_get_len(itemid,&len);
    return ret;
}

u32 nv_get_len_test02(void)
{
    u32 ret= NV_ERROR;
    u32 len = 0;
    u32 itemid = 0xffff;

    ret = bsp_nvm_get_len(itemid,&len);
    if(ret == BSP_ERR_NV_NO_THIS_ID)
    {
        return NV_OK;
    }
    return ret;
}

/*read nv test*/


/*不存在的nv*/
u32 nv_read_test_01(void)
{
    u32 itemid = 0xffff;
    u32 ret;

    ret = nv_read_rand_test(itemid);

    if(ret == BSP_ERR_NV_NO_THIS_ID)
    {
        return NV_OK;
    }

    return ret;
}

/*传入长度偏移错误测试01*/
u32 nv_read_test_02(void)
{
    u32 itemid = 0xd007;
    u32 ret = NV_ERROR;

    u32 datalen  = 5;
    u32 off = 0;

    ret = nv_read_part_test(itemid,off,datalen);
    if(ret == BSP_ERR_NV_READ_DATA_FAIL)
    {
        return NV_OK;
    }

    return NV_ERROR;
}
/*传入长度偏移错误测试02*/
u32 nv_read_test_03(void)
{
    u32 itemid = 0xd007;
    u32 ret = NV_ERROR;

    u32 datalen  = 1;
    u32 off = 4;

    ret = nv_read_part_test(itemid,off,datalen);
    if(ret == BSP_ERR_NV_READ_DATA_FAIL)
    {
        return NV_OK;
    }

    return NV_ERROR;
}

u32 nv_read_test_04(void)
{
    u32 itemid = 0xd007;
    u32 ret = NV_ERROR;

    u32 datalen  = 4;
    u32 off = 0;

    ret = nv_read_part_test(itemid,off,datalen);

    return ret;
}

u32 nv_read_test_05(void)
{
    u32 itemid = 0xd007;
    u32 ret = NV_ERROR;

    u32 datalen  = 4;
    u32 off = 0;

    ret = nv_read_rand_test(itemid);

    return ret;
}

u32 nv_read_test_06(void)
{
    u32 itemid = 0xd007;
    u32 ret = NV_ERROR;
    u8* pdata = NULL;

    ret = bsp_nvm_read(itemid,pdata,sizeof(u32));
    if(ret == BSP_ERR_NV_INVALID_PARAM)
    {
        return NV_OK;
    }
    return ret;
}

u32 nv_read_test_07(void)
{
    u32 itemid = 0xd007;
    u32 ret = NV_ERROR;
    u32 pdata = 0;

    ret = bsp_nvm_read(itemid,&pdata,1);
    if(ret == BSP_ERR_NV_ITEM_LEN_ERR)
    {
        return NV_OK;
    }
    return ret;
}


u32 nv_write_test01(void)
{
    u32 ret = NV_OK;
    u16 data_len = 0;
    u32 i = 0;
    u32 read_data = 0;
    u32 write_data = 0;

    for(i = 0;i<100;i++)
    {
         printf("\n****************第%d次读写开始**************************\n",i);
         write_data = i+60;
         ret = bsp_nvm_write(0xd007,&write_data,sizeof(u32));
         if(NV_OK != ret)
         {
             printf("****write error*****i = %d\n",i);
             return ret;
         }
         ret = bsp_nvm_read(0xd007,&read_data,sizeof(u32));
         if(ret)
         {
             printf("****read error*****i = %d\n",i);
             return ret;
         }
         if(read_data != write_data)
         {
            return NV_ERROR;
         }
    }
    return NV_OK;
}


u32 nv_write_test02(void)
{
    u32 ret = NV_OK;
    u16 data_len = 0;
    u32 i = 0;
    u32 read_data = 0;
    u32 write_data = 0;

    for(i = 0;i<100;i++)
    {
         printf("\n****************第%d次读写开始**************************\n",i);
         write_data = i+60;
         ret = bsp_nvm_write(0xd007,&write_data,sizeof(u32));
         if(NV_OK != ret)
         {
             printf("****write error*****i = %d\n",i);
             return ret;
         }
         ret = bsp_nvm_read(0xd007,&read_data,sizeof(u32));
         if(ret)
         {
             printf("****read error*****i = %d\n",i);
             return ret;
         }
    }
    return NV_OK;
}

u32 nv_write_test03(void)
{
    u32 ret = NV_ERROR;
    u32 itemid = 0xd007;

    u8* pdata = NULL;

    ret = bsp_nvm_write(itemid,pdata,sizeof(u32));
    if(ret == BSP_ERR_NV_INVALID_PARAM)
    {
        return NV_OK;
    }
    return ret;
}

u32 nv_write_test04(void)
{
    u32 ret = NV_ERROR;
    u32 itemid = 0xffff;

    u32 datalen  = 0;

    ret = bsp_nvm_write(itemid,&datalen,sizeof(u32));
    if(ret == BSP_ERR_NV_NO_THIS_ID)
    {
        return NV_OK;
    }
    return ret;
}

u32 nv_write_test05(void)
{
    u32 ret = NV_ERROR;
    u32 itemid = 0xd007;
    u32 data = 56;

    ret = bsp_nvm_write(itemid,&data,sizeof(u16));
    if(ret == BSP_ERR_NV_ITEM_LEN_ERR)
    {
        return NV_OK;
    }
    return ret;
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
