/*************************************************************************
*   版权所有(C) 1987-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  ncm_balong.c
*
*   作    者 :  wangzhongshun
*
*   描    述 :  ncm应用层文件
*
*   修改记录 :  2013年2月16日  v1.00  wangzhongshun  创建

*************************************************************************/
#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */
/*lint -save -e537 */
#include <linux/types.h>
#include <linux/export.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>

#include "drv_ncm.h"
#include "ncm_balong.h"
#include "bsp_usb.h"
#include "usb_vendor.h"


#ifdef CONFIG_ETH_BYPASS_MODE

#define FNAME __FUNCTION__

static bool ncm_vendor_inited = false;
bool ncm_bypass_udi = false;    /* just for test */
static ncm_app_ctx_t ncm_app_ctx_set[GNET_MAX_NUM] = {{0}};
static NCM_PKT_INFO_S ncm_pkt_info_cur[GNET_MAX_NUM] = {{0}};  /* ugly requirement */
static NCM_PKT_INFO_S ncm_pkt_info_last[GNET_MAX_NUM] = {{0}}; /* ugly requirement */


/* just for mbim */
const u8 ncm_eth_hdr_arp[ETH_HLEN] = {0x58,0x2C,0x80,0x13,0x92,0x08,0x58,0x2C,0x80,0x13,0x92,0x63,0x08,0x06};
const u8 ncm_eth_hdr_ipv4[ETH_HLEN] = {0x58,0x2C,0x80,0x13,0x92,0x08,0x58,0x2C,0x80,0x13,0x92,0x63,0x08,0x00};
const u8 ncm_eth_hdr_ipv6[ETH_HLEN] = {0x58,0x2C,0x80,0x13,0x92,0x08,0x58,0x2C,0x80,0x13,0x92,0x63,0x86,0xdd};

extern UDI_HANDLE BSP_UDI_NCM_DataChnToHandle(u32 u32Chn);
extern int ncm_add_response (void *f_ncm,u8 *buf, u32 length);
extern int ncm_vendor_post_skb(void *ncm_priv, struct sk_buff *skb);

void ncm_mbim_send_at_func(void *ncm_cxt, unsigned char *data, unsigned int length);
int ncm_mbim_send_encap_rsp(void *ncm_cxt, unsigned char *data, unsigned int length);

/*****************************************************************************
* 函 数 名  : ncm_get_id
*
* 功能描述  : 获取一个可用的ncm app ctx
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : >=0 ncm app ctx下标
*             -ENODEV
*
* 其它说明  : 无
*
*****************************************************************************/
static inline int ncm_get_idle(void)
{
    u32 i;/*lint !e578*/

    for (i = 0; i < GNET_USED_NUM; i++)
    {
        if (false == ncm_app_ctx_set[i].bUsed)
        {
            ncm_app_ctx_set[i].bUsed = true;
            ncm_app_ctx_set[i].ncm_id = i;
            return (int)i;
        }
    }

    return -ENODEV;
}

/*****************************************************************************
* 函 数 名  : ncm_set_idle
*
* 功能描述  : recycle a ncm app ctxt
*
* 输入参数  : ncm_id :ncm id
* 输出参数  : 无
*
* 返 回 值  : 0  :success
*             <0 :error
*
* 修改记录 : 
*
*****************************************************************************/
static inline int ncm_set_idle(u32 dev_id)
{
    if(dev_id >= GNET_USED_NUM)
    {
        return -EINVAL;
    }

    ncm_app_ctx_set[dev_id].bUsed = false;
    ncm_app_ctx_set[dev_id].ncm_id = 0;
    
    return 0;
}

/*****************************************************************************
* 函 数 名  : ncm_get_ctx
*
* 功能描述  : 根据设备ID获取上下文
*
* 输入参数  : u32 dev_id    :设备ID
* 输出参数  : 无
*
* 返 回 值  : 设备ID上下文
*
* 其它说明  : 无
*
*****************************************************************************/
static inline ncm_app_ctx_t *ncm_get_ctx(u32 dev_id)
{
    return (&ncm_app_ctx_set[dev_id]);
}

/*****************************************************************************
* 函 数 名  : ncm_get_id
*
* 功能描述  : 根据上下文结构获取设备ID
*
* 输入参数  : BSP_U32 u32NcmDevId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
static inline s32 ncm_get_id(ncm_app_ctx_t *app_ctx)
{
    u32 i;/*lint !e578*/

    for (i = 0; i < GNET_USED_NUM; i++)
    {
        if (app_ctx == &ncm_app_ctx_set[i])
        {
            return i;
        }
    }

    return -ENODEV;
}

/*****************************************************************************
* 函 数 名  : ncm_at_resp_process
*
* 功能描述  : AT命令响应接口，同步接口
*
* 输入参数  : ncm_app_ctx_t *pNcmCtx
*             void *response  
*             u32 size     
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
s32 ncm_vendor_at_resp_process(ncm_app_ctx_t *ctx, void *response, u32 size)
{
    s32 ret = 0;
    
    ncm_vendor_ctx_t *vendor_ctx = ctx->vendor_ctx;
    
    if (vendor_ctx->is_mbim_mode)
    {
#ifdef CONFIG_NCM_MBIM_SUPPORT
        ret = mbim_srvc_send_at_rsp(response, size);
#endif
    }
    else
    {
        ret = ncm_vendor_add_response(vendor_ctx->ncm_priv, response, size);
    }

    return ret;
}

/*****************************************************************************
* 函 数 名  : ncm_vendor_encap_cmd
*
* 功能描述  : 扩展命令处理，目前只处理AT命令
*
* 输入参数  : u8 *command     
*             u32 size          
*             void *ctx  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
s32 ncm_vendor_at_processs(void *ctx, u8 *command, u32 size)
{
    ncm_app_ctx_t *app_ctx = (ncm_app_ctx_t *)ctx;

    if(!app_ctx)
        return -ENOMEM;

    if (app_ctx->chnl_open[NCM_DEV_CTRL_TYPE]
        && app_ctx->cbs.ndis_at_recv_cb)
    {
        app_ctx->debug_info.u32NcmAtCbNum++;
        app_ctx->cbs.ndis_at_recv_cb(command, size);
    }
    
    return 0;
}

s32 ncm_vendor_encap_cmd(ncm_app_ctx_t *app_ctx, u8 *command, u32 size)
{
    s32 ret=0;
    if (app_ctx->vendor_ctx->is_mbim_mode)
    {
#ifdef CONFIG_NCM_MBIM_SUPPORT
        ret = ncm_mbim_send_encap_cmd(app_ctx, command, size);
#endif
    }
    else
    {
        ret = ncm_vendor_at_processs(app_ctx, command, size);
    }

    return ret;
}

/*****************************************************************************
* 函 数 名  : ncm_vendor_open
*
* 功能描述  : 打开ncm通道
*
* 输入参数  : NCM_DEV_TYPE_E dev_type  通道类型
*             
* 输出参数  : u32 *dev_id              设备id
*
* 返 回 值  : -EINVAL
*             -ENODEV
*             OK
*
* 修改记录  : 2013年2月17日   wangzhongshun  creat
*****************************************************************************/
s32 ncm_vendor_open(NCM_DEV_TYPE_E dev_type, u32 dev_id)
{
    if(!ncm_vendor_inited)
    {
        printk(KERN_ERR "%s line %d:module not init.\n",
            __FUNCTION__,__LINE__);
        return -ENODEV;
    }

    if((dev_id >= GNET_USED_NUM) 
        || ((NCM_DEV_DATA_TYPE != dev_type) && (NCM_DEV_CTRL_TYPE != dev_type)))
    {
        return -EINVAL;
    }


    if (false == ncm_app_ctx_set[dev_id].chnl_open[dev_type]) {

            ncm_app_ctx_set[dev_id].chnl_open[dev_type] = true;
            return 0;
    }
  
    return -ENODEV;
}

/*****************************************************************************
* 函 数 名  : ncm_vendor_close
*
* 功能描述  : 关闭ncm设备通道
*
* 输入参数  : NCM_DEV_TYPE_E enDevType  设备类型
*             u32 u32NcmDevId  NCM设备ID
* 输出参数  : NA
*
* 返 回 值  : -EINVAL   : 参数非法
*             OK        : 成功
*
* 其它      : NA
*****************************************************************************/
s32 ncm_vendor_close(NCM_DEV_TYPE_E dev_type, u32 dev_id)
{
    if(!ncm_vendor_inited)
        return -ENODEV;

    if (dev_id >= GNET_USED_NUM)
    {
        printk(KERN_ERR "%s: dev_id = %d, dev_type = %d\n", 
            FNAME, (s32)dev_id, (s32)dev_type);
        return -ENODEV;
    }
    
    if (dev_type != NCM_DEV_DATA_TYPE && dev_type != NCM_DEV_CTRL_TYPE)
    {
        printk(KERN_ERR "%s: dev_id = %d, dev_type = %d\n", 
            FNAME, (s32)dev_id, (s32)dev_type);
        return -EINVAL;
    }

    ncm_app_ctx_set[dev_id].chnl_open[dev_type] = false;

    return 0;
}

/*****************************************************************************
* 函 数 名  : ncm_write
*
* 功能描述  : 发送数据
*
* 输入参数  : u32 dev_id        : 设备ID
* 输出参数  : void *pkt_encap   : 包封装首地址
*
* 返 回 值  : 0
*             -EINVAL
*             -ENODEV
*
* 其它      : NA
*****************************************************************************/
s32 ncm_vendor_write(u32 dev_id, void *pkt_encap, void *net_priv)
{
    ncm_app_ctx_t *app_ctx = NULL;
    ncm_vendor_ctx_t *vendor_ctx = NULL;
    
    if(!ncm_vendor_inited)
    {
        printk("%s line %d:ncm vendor not init.\n",__FUNCTION__,__LINE__);
        return -ENODEV;
    }
    
    if(dev_id >= GNET_USED_NUM || !pkt_encap)
    {
        printk("%s line %d:invalid para(dev_id=%d,pkt_encap=%d).\n",
            __FUNCTION__,__LINE__,dev_id,(u32)pkt_encap);
        return -EINVAL;
    }

    app_ctx = ncm_get_ctx(dev_id);
    vendor_ctx = app_ctx->vendor_ctx;

    if(!app_ctx->chnl_open[NCM_DEV_DATA_TYPE] || !app_ctx->bDataConnect)
    {
            vendor_ctx->stat_tx_nochl++;

        return -ENODEV;
    }

    return ncm_vendor_post_skb(app_ctx->vendor_ctx->ncm_priv,(struct sk_buff *)pkt_encap);
}

/*****************************************************************************
* 函 数 名  : ncm_ioctl
*
* 功能描述  : 配置ncm设备属性
*
* 输入参数  : dev_id
*             cmd
*             param
* 输出参数  : NA
*
* 返 回 值  : -ENODEV
*
* 其它      : NA
*****************************************************************************/
s32 ncm_vendor_ioctl(u32 dev_id, NCM_IOCTL_CMD_TYPE_E cmd, void *param)
{    
    ncm_app_ctx_t *ctx = NULL;
    s32 ret = 0; 

    if(!ncm_vendor_inited)
    {
        printk(KERN_ERR "%s line %d:module not init.\n",
            __FUNCTION__,__LINE__);
        return -ENODEV;
    }

    if (dev_id >= GNET_USED_NUM)
    {
        printk(KERN_ERR "%s line %d:dev_id %d overflow.\n",
            __FUNCTION__,__LINE__,dev_id);
        return -EINVAL;
    }

    ctx = ncm_get_ctx(dev_id);
    if(!ctx->chnl_open[NCM_DEV_DATA_TYPE] && !ctx->chnl_open[NCM_DEV_CTRL_TYPE])
    {
        printk(KERN_ERR "%s line %d:both channel not opened.\n",
            __FUNCTION__,__LINE__);
        return -ENODEV;
    }

    /* at_process/at_response divert to ctrl channel, other's divert to data channel. */
    if (!ctx->chnl_open[NCM_DEV_DATA_TYPE]
        && (cmd != NCM_IOCTL_REG_AT_PROCESS_FUNC)
        && (cmd != NCM_IOCTL_AT_RESPONSE))
    {
        return -EIO;
    }
    
    if (!ctx->chnl_open[NCM_DEV_CTRL_TYPE]
        && (NCM_IOCTL_REG_AT_PROCESS_FUNC == cmd
            || NCM_IOCTL_AT_RESPONSE == cmd))
    {
        return -EIO;
    }
    
    switch (cmd)
    {
        case NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF:   /* notify speed to pc */
        {
            NCM_IOCTL_CONNECTION_SPEED_S *speed = (NCM_IOCTL_CONNECTION_SPEED_S *)param;

            if (true != ctx->bEnable || !param)
            {
                return -EIO;
            }
            
            /* PC端网卡显示速率为上下行最大值，如果上下行速率均小于1k，
               或者大于480M，则PC端速率会显示不准确*/
            if (((speed->u32UpBitRate < NCM_MIN_RATE) 
                && (speed->u32DownBitRate < NCM_MIN_RATE))
                || (speed->u32UpBitRate > NCM_MAX_RATE)
                || (speed->u32DownBitRate > NCM_MAX_RATE))
            {
                return -EIO;
            }
            
            /* notify the host the connection speed */
            ret = ncm_vendor_notify(ctx->vendor_ctx->ncm_priv, cmd, param);
            break;
        }

        case NCM_IOCTL_NETWORK_CONNECTION_NOTIF:
        {
            if (true != ctx->bEnable || !param)
            {
                return -EIO;
            }
            if (((*(BSP_U32 *)param) != NCM_IOCTL_CONNECTION_LINKDOWN) && ((*(BSP_U32 *)param) != NCM_IOCTL_CONNECTION_LINKUP))
            {
                return -EIO;
            }
            ctx->bDataConnect = *(BSP_U32 *)param;
                        
            /* notify the host the connection state */
            ret = ncm_vendor_notify(ctx->vendor_ctx->ncm_priv, cmd, param);

            break;
        }

        case NCM_IOCTL_FREE_BUFF:
        {
            if (!param)
            {
                return -EIO;
            }
            break;
        }
                
        case NCM_IOCTL_SET_ACCUMULATION_TIME:
        {
            if (true != ctx->bEnable)
            {
                return -EIO;
            }

            ctx->u32AccumulationTime = *(u32 *)param;
            break;
        }
                
        case NCM_IOCTL_SET_PKT_ENCAP_INFO:
        {
            ctx->pkt_encap_info[0] = (u32)(((NCM_PKT_ENCAP_INFO_S *)param)->s32BufOft);
            ctx->pkt_encap_info[1] = (u32)(((NCM_PKT_ENCAP_INFO_S *)param)->s32LenOft);
            ctx->pkt_encap_info[2] = (u32)(((NCM_PKT_ENCAP_INFO_S *)param)->s32NextOft);
            break;
        }

        case NCM_IOCTL_REG_UPLINK_RX_FUNC:
        {
            ctx->cbs.rx_cb = (USBUpLinkRxFunc)param;/*lint !e611*/
            break;
        }
        
        case NCM_IOCTL_REG_FREE_PKT_FUNC:
        {
            ctx->cbs.pkt_encap_free_cb = (USBFreePktEncap)param;/*lint !e611*/
            break;
        }
        
        case NCM_IOCTL_GET_USED_MAX_BUFF_NUM:
        {
            *(u32*)param = (u32)(NCM_RCV_BUFFER_NUM - 100);
            break;
        }
        
        case NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM:
        {
            *(u32*)param = MIN_NCM_TX;
            break;
        }
        
        case NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT:
        {            
            *(u32*)param = NCM_TX_TIMEOUT;
            break;
        }
        
        case NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE:
        {            
            *(u32*)param = NCM_MAX_TX_TRANSFER_SIZE;
            break;
        }
        
        case NCM_IOCTL_SET_TX_MIN_NUM:
        {            
            break;
        }
        
        case NCM_IOCTL_SET_TX_TIMEOUT:
        {
            if (true != ctx->bEnable)
            {
                return -EIO;
            }

            break;
        }
        
        case NCM_IOCTL_SET_TX_MAX_SIZE:
        {
            break;
        }
        
        case NCM_IOCTL_GET_RX_BUF_SIZE:
        {
            *(u32 *)param = NCM_MAX_RX_TRANSFER_SIZE;
            break;
        }
        
        case NCM_IOCTL_FLOW_CTRL_NOTIF:
        {    
            if (true != ctx->bEnable)
            {
                return -EIO;
            }
            
            if (*(u32 *)param > (u32)NCM_IOCTL_FLOW_CTRL_DISABLE)
            {

                return -EINVAL;
            }
            
            ctx->u32FlowCtrlStus = *(u32 *)param;
            break;
        }    
        
        case NCM_IOCTL_REG_AT_PROCESS_FUNC:
        {
            ctx->cbs.ndis_at_recv_cb = (USBNdisAtRecvFunc)param;/*lint !e611*/
            break;
        }
        
        case NCM_IOCTL_AT_RESPONSE:
        {
            if((NULL == ((NCM_AT_RSP_S *)param)->pu8AtAnswer)
                || (0 == ((NCM_AT_RSP_S *)param)->u32Length))
            {
                return -EINVAL;
            }

            ret = ncm_vendor_at_resp_process(ctx,
              ((NCM_AT_RSP_S *)param)->pu8AtAnswer,
              ((NCM_AT_RSP_S *)param)->u32Length);
            if(ret)
            {
                return -EIO;
            }
            break;
        }

        case NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC:
        {
            ctx->cbs.ndis_stat_change_cb = (USBNdisStusChgFunc)param;/*lint !e611*/
            break;
        }

        case NCM_IOCTL_SET_PKT_STATICS:
        { 
            memcpy(&(ctx->pkt_stats), param, sizeof(NCM_IOCTL_PKT_STATISTICS_S));
            break;
        }
        
        case NCM_IOCTL_GET_FLOWCTRL_STATUS:
        {
            *(u32 *)param = ctx->u32FlowCtrlStus;
            break;
        }

        case NCM_IOCTL_GET_CUR_TX_MIN_NUM:
        {
            *(u32 *)param = ctx->u32TxCurMinNum;
            break;
        }
        
        case NCM_IOCTL_GET_CUR_TX_TIMEOUT:
        {
            *(u32 *)param = ctx->u32TxCurTimeout;
            break;
        }
        
        case NCM_IOCTL_IPV6_DNS_NOTIF:
        {
            if (true != ctx->bEnable)
            {
                return -EIO;
            }

            if((NULL  == ((NCM_AT_IPV6_DNS_NTF_S *)param)->pu8Ipv6DnsNtf)||
                (0   == ((NCM_AT_IPV6_DNS_NTF_S *)param)->u32Length))
            {
                printk(KERN_ERR "parm error pu8Ipv6DnsNtf = 0x%x,length:%d\n",
                    (u32)(((NCM_AT_IPV6_DNS_NTF_S *)param)->pu8Ipv6DnsNtf),
                      ((NCM_AT_IPV6_DNS_NTF_S *)param)->u32Length);
                return -EINVAL;
            }

            return -EIO;
         }
         
        case NCM_IOCTL_SET_IPV6_DNS:
        {
            u8 * pu8Ipv6DnsInfo = ((NCM_IPV6DNS_S *)param)->pu8Ipv6DnsInfo;
            u32 u32Len = ((NCM_IPV6DNS_S *)param)->u32Length;
            if (true != ctx->bEnable)
            {
                printk(KERN_ERR "%s: the NCM device is disable\n",FNAME);
                return -EIO;        
            }
            
            if((NULL  == pu8Ipv6DnsInfo)
                || (BSP_NCM_IPV6_DNS_LEN != u32Len))
            {
                printk(KERN_ERR "parm error pu8Ipv6DnsInfo = 0x%x,at.length:%d\n", (u32)pu8Ipv6DnsInfo, u32Len);
                return -EINVAL;
            }
            
            memcpy(ctx->au8Ipv6Dns, pu8Ipv6DnsInfo, BSP_NCM_IPV6_DNS_LEN);
            ctx->u32Ipv6DnsLen = u32Len;            
            break;
        }
        
        case NCM_IOCTL_CLEAR_IPV6_DNS:
        {
            (void)memset(ctx->au8Ipv6Dns, 0x0, BSP_NCM_IPV6_DNS_LEN);
            ctx->u32Ipv6DnsLen = 0;
            break;
        }
        
        case NCM_IOCTL_GET_NCM_STATUS:
        {
            *(u32 *)param = (u32)ctx->bEnable;
            break;
        }
        
        case NCM_IOCTL_SET_RX_MIN_NUM:
        {
            break;
        }
        
        case NCM_IOCTL_SET_RX_TIMEOUT:
        {
            break;
        }
        
        default:
        {
            printk(KERN_ERR "%s: cmd [%d]param[0x%x] is invalid!\n", FNAME, (u32)cmd,(u32)param);
            return -EINVAL;
        }
              
    }

    return ret;
}

/*****************************************************************************
* 函 数 名  : ncm_vendor_init
*
* 功能描述  : ncm app初始化
*
* 输入参数  :       
*             void *app_ctx     
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
s32 ncm_vendor_init(ncm_vendor_ctx_t *vendor_ctx)
{
    ncm_app_ctx_t *app_ctx = NULL;
    s32 dev_id = 0;

    if(!vendor_ctx)
    {
        return -ENODEV;
    }

    if (false == ncm_vendor_inited)
    {        
        (void)memset((void *)ncm_app_ctx_set, 0x0, sizeof(ncm_app_ctx_set));
        ncm_vendor_inited = true;
    }

    dev_id = ncm_get_idle();
    if (dev_id < 0)
    {
        return -ENODEV;
    }
    
    app_ctx = ncm_get_ctx(dev_id);
    app_ctx->bEnable = true;
    app_ctx->u32AccumulationTime = NCM_PACKETS_ACCUMULATION_TIME;
    app_ctx->vendor_ctx = vendor_ctx;
    app_ctx->vendor_ctx->vendor_priv = app_ctx;
    if(!app_ctx->vendor_ctx)
    {
        printk("%s line %d:null app_ctx->vendor_ctx\n",__FUNCTION__,__LINE__);
    }

    return 0;
}

/*****************************************************************************
* 函 数 名  : ncm_vendor_uninit
*
* 功能描述  : ncm app去初始化
*
* 输入参数  :       
*             void *app_ctx     
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
s32 ncm_vendor_uninit(ncm_vendor_ctx_t *vendor_ctx)
{
    ncm_app_ctx_t *app_ctx = (ncm_app_ctx_t *)vendor_ctx->vendor_priv;

    if(!app_ctx || app_ctx->ncm_id >= GNET_USED_NUM)
    {
        return -EINVAL;
    }

    ncm_set_idle(app_ctx->ncm_id);
    memset(app_ctx, 0, sizeof(*app_ctx));

    ncm_vendor_inited = false;

    return 0;
}

int ncm_vendor_submit_skb(ncm_app_ctx_t *app_ctx, struct sk_buff *skb)
{
    UDI_HANDLE handle;
    s32 dev_id = 0;
    u32 pkt_len = 0;
#ifdef CONFIG_NCM_MBIM_SUPPORT
    u8 hdr = 0;
    uint8_t *pucDestPtr = NULL;
#endif

    if (!app_ctx || !app_ctx->bDataConnect || !app_ctx->cbs.rx_cb || !skb)
    {
        return -ENOMEM;
    }
    
    dev_id = ncm_get_id(app_ctx);
    if(0 > dev_id)
    {
        return -ENODEV;
    }

    if(!ncm_bypass_udi)
    {
        handle = BSP_UDI_NCM_DataChnToHandle((u32)dev_id);
    }
    else
    {
        handle = (u32)dev_id;
    }
        

#ifdef CONFIG_NCM_MBIM_SUPPORT
    pkt_len = app_ctx->vendor_ctx->is_mbim_mode?
        skb->len + ETH_HLEN:skb->len;
#else
    pkt_len = skb->len;
#endif

    if (pkt_len < ETH_HLEN || pkt_len > ETH_FRAME_LEN)
    {
        app_ctx->debug_info.u32RcvTooLongNum++;
        app_ctx->debug_info.u32RcvErrPktLen = pkt_len;
        return -EINVAL;
    }

#ifdef CONFIG_NCM_MBIM_SUPPORT
    if (app_ctx->vendor_ctx->is_mbim_mode)
    {
        hdr = *((u8 *)skb->data);
        if(0x40 == hdr&0xF0)
        {
            memcpy(pucDestPtr, ncm_eth_hdr_ipv4, ETH_HLEN);
        }
        else if(0x60 == hdr&0xF0)
        {
            memcpy(pucDestPtr, ncm_eth_hdr_ipv6, ETH_HLEN);
        }
        else
        {
            memcpy(pucDestPtr, ncm_eth_hdr_arp, ETH_HLEN);
        }

        pucDestPtr += ETH_HLEN;
        pkt_len -= ETH_HLEN;    
    }

    memcpy(pucDestPtr, skb->data, pkt_len);                
    app_ctx->debug_info.u32PsRcvPktNum++;
            
    app_ctx->cbs.rx_cb(handle, pstIMM_ZC);
#else
    app_ctx->cbs.rx_cb(handle, skb);
#endif

    return 0;
}

void ncm_mbim_send_at_func(void *ncm_cxt, 
  unsigned char *data, unsigned int length)
{
    (void)ncm_vendor_at_processs(ncm_cxt, data, length);
}

int ncm_mbim_send_encap_rsp(void *ncm_cxt, 
  unsigned char *data, unsigned int length)
{
    ncm_add_response(ncm_cxt, data, length);
    
    return 0;
}  

s32 ncm_mbim_send_encap_cmd(void *app_ctx, unsigned char *data, 
  unsigned int size)
{
#ifdef CONFIG_NCM_MBIM_SUPPORT
    return mbim_srvc_send_encap_cmd(data, size);
#else
    return 0;
#endif
}

void ncm_mbim_send_encap_rsp_done(int status)
{
#ifdef CONFIG_NCM_MBIM_SUPPORT
    mbim_srvc_send_rsp_cmd_done(status);
#else
    return;
#endif
}

int ncm_bind_func(void *ctx)
{
    ncm_app_ctx_t *ncm_ctx = (ncm_app_ctx_t *)ctx;
    int ret = 0;

    if (ncm_ctx && ncm_ctx->vendor_ctx && ncm_ctx->vendor_ctx->is_mbim_mode)
    {
#ifdef CONFIG_NCM_MBIM_SUPPORT
        ret = mbim_start(ncm_ctx->vendor_ctx->ncm_priv, ncm_mbim_send_at_func, ncm_mbim_send_encap_rsp);
        if (ret)
        {
            printk(KERN_ERR"huwei_ncm_app_init, start mbim failed. ret=%u\n", ret);
        }
#endif
    }

    return ret;
}

void ncm_unbind_func(void *app_ctx)
{
    ncm_app_ctx_t *ncm_ctx = (ncm_app_ctx_t *)app_ctx;
    
    if (!ncm_ctx)
    {
        return;
    }
    
    (void)ncm_vendor_close(NCM_DEV_DATA_TYPE, ncm_ctx->ncm_id);
    (void)ncm_vendor_close(NCM_DEV_CTRL_TYPE, ncm_ctx->ncm_id);
}

s32 NCM_debugPktGet(u32 dev_id, NCM_PKT_INFO_S *pkt_info)
{
    if(dev_id >= GNET_USED_NUM)
    {
        return -EINVAL;
    }
    
    (void)memset((void *)&ncm_pkt_info_cur[dev_id], 0, sizeof(NCM_PKT_INFO_S));

    ncm_pkt_info_cur[dev_id].u32NcmSendPktNum = ncm_app_ctx_set[dev_id].debug_info.u32NcmSendPktNum;
    ncm_pkt_info_cur[dev_id].u32PsRcvPktNum = ncm_app_ctx_set[dev_id].debug_info.u32PsRcvPktNum;
    ncm_pkt_info_cur[dev_id].u32RcvUnusedNum = ncm_app_ctx_set[dev_id].debug_info.u32RcvUnusedNum;

    ncm_pkt_info_cur[dev_id].u32NcmSendPktNum -= ncm_pkt_info_last[dev_id].u32NcmSendPktNum;
    ncm_pkt_info_cur[dev_id].u32PsRcvPktNum -= ncm_pkt_info_last[dev_id].u32PsRcvPktNum;
    ncm_pkt_info_cur[dev_id].u32RcvUnusedNum -= ncm_pkt_info_last[dev_id].u32RcvUnusedNum;

    *pkt_info = ncm_pkt_info_cur[dev_id];
    
    return 0;
}
EXPORT_SYMBOL(NCM_debugPktGet);

s32 NCM_debugPktClear(u32 dev_id)
{
    if(dev_id >= GNET_USED_NUM)
    {
        return -EINVAL;
    }
    
    ncm_pkt_info_last[dev_id].u32NcmSendPktNum += ncm_pkt_info_cur[dev_id].u32NcmSendPktNum;
    ncm_pkt_info_last[dev_id].u32PsRcvPktNum += ncm_pkt_info_cur[dev_id].u32PsRcvPktNum;
    ncm_pkt_info_last[dev_id].u32RcvUnusedNum += ncm_pkt_info_cur[dev_id].u32RcvUnusedNum;
    
    (void)memset((void *)&ncm_pkt_info_cur[dev_id], 0, sizeof(NCM_PKT_INFO_S));

    return 0;
}
EXPORT_SYMBOL(NCM_debugPktClear);

void ncm_vendor_help(void)
{    
    (void) printk("\r |*************************************|\n");    
    (void) printk("\r ncm_debuginfo_show : 查看各种包统计信息\n");    
    (void) printk("\r ncm_debugcb_show   : 查看回调函数信息\n");
    (void) printk("\r ncm_debugctx_Show  : 查看vendor上下文信息\n");
    (void) printk("\r ncm_vendorctx_show : 查看上下文信息\n");    
    (void) printk("\r |*************************************|\n");    
}

void ncm_debuginfo_show(void)
{
    u32 i;/*lint !e578*/
    
    for (i = 0; i < GNET_MAX_NUM;i++)
    {        
        (void)  printk("\r The NCM[%d]adpter info                :\n",(s32)i);
        (void)  printk("\r The NCM[%d]:收包函数进入次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32RcvInNum);
        (void)  printk("\r The NCM[%d]:收包字节数                : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32RcvInSize);
        (void)  printk("\r The NCM[%d]:收包错误包计数            : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32RcvTooLongNum);
        (void)  printk("\r The NCM[%d]:收包错误包包长            : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32RcvErrPktLen);
        (void)  printk("\r The NCM[%d]:收包分配skb失败次数       : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32AllocSkbFailNum);
        (void)  printk("\r The NCM[%d]:收包函数进入异常退出次数  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32RcvInErrNum);
        (void)  printk("\r The NCM[%d]:收包送到PS的包个数        : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32PsRcvPktNum);
        (void)  printk("\r The NCM[%d]:收包回调函数空而丢弃包个数: %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32RcvUnusedNum);
        (void)  printk("\r The NCM[%d]:发包函数进入次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendInNum);    
        (void)  printk("\r The NCM[%d]:发包个数                  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendPktNum);      
        (void)  printk("\r The NCM[%d]:发包全部无效次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendNoPktNum);
        (void)  printk("\r The NCM[%d]:发包函数成功次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendOkNum);
        (void)  printk("\r The NCM[%d]:发包申请不到内存退出次数  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendNoMemNum);
        (void)  printk("\r The NCM[%d]:发包调用fd层write失败次数 : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendFailNum);
        (void)  printk("\r The NCM[%d]:发包通道没有打开退出次数  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendDevNotOpenNum);
        (void)  printk("\r The NCM[%d]:发包设备没有使能退出次数  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendDevNotEnableNum);
        (void)  printk("\r The NCM[%d]:发包网卡没有连接退出次数  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendNetNotConnectNum);
        (void)  printk("\r The NCM[%d]:发包包长超过1514退出次数  : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendPktTooLargeNum);
        (void)  printk("\r The NCM[%d]:发包函数数据包无效次数    : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendPktInvalidNum);
        (void)  printk("\r The NCM[%d]:发包完成回调次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendDoneNum);
        (void)  printk("\r The NCM[%d]:发包完成回调失败次数      : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmSendDoneFailNum);
        (void)  printk("\r The NCM[%d]:AT reply进入次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmAtInNum);
        (void)  printk("\r The NCM[%d]:AT reply成功次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmAtInOKNum);
        (void)  printk("\r The NCM[%d]:AT reply失败次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmAtInFailNum);
        (void)  printk("\r The NCM[%d]:AT reply超时次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmAtInTimeOutNum);
        (void)  printk("\r The NCM[%d]:AT reply完成次数          : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmAtDoneNum);
        (void)  printk("\r The NCM[%d]:AT request回调处理次数    : %d\n",(s32)i,(s32)ncm_app_ctx_set[i].debug_info.u32NcmAtCbNum);
    }    
}

void ncm_debugcb_show(void)
{
    u32 i;/*lint !e578*/
    
    for (i = 0; i < GNET_MAX_NUM; i++)
    {       
        (void)  printk("\r The NCM[%d] callback func info            :\n",(s32)i);
        (void)  printk("\r The NCM[%d]:上行收包回调函数              :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].cbs.rx_cb);
        (void)  printk("\r The NCM[%d]:下行buffer释放回调函数        :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].cbs.pkt_encap_free_cb);
        (void)  printk("\r The NCM[%d]:AT命令处理回调                :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].cbs.ndis_at_recv_cb);
        (void)  printk("\r The NCM[%d]:USB网卡状态变化处理回调函数   :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].cbs.ndis_stat_change_cb);
    }
}

void ncm_debugctx_Show(void)
{
    u32 i;/*lint !e578*/
    for (i = 0; i < GNET_MAX_NUM; i++)
    {       
        (void)  printk("\r The NCM[%d] ctx info                      :\n",(s32)i);
        (void)  printk("\r The NCM[%d]:设备ID号                      :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].ncm_id);
        (void)  printk("\r The NCM[%d]:设备是否被初始化              :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].bUsed);
        (void)  printk("\r The NCM[%d]:设备数据通道是否被打开        :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].chnl_open[NCM_DEV_DATA_TYPE]);
        (void)  printk("\r The NCM[%d]:设备控制通道是否被打开        :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].chnl_open[NCM_DEV_CTRL_TYPE]);      
        (void)  printk("\r The NCM[%d]:设备是否使能                  :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].bEnable);
        (void)  printk("\r The NCM[%d]:网卡是否连接                  :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].bDataConnect); 
        (void)  printk("\r The NCM[%d]:AT发送返回值                  :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].s32atResponseRet);
        (void)  printk("\r The NCM[%d]:回调函数结构                  :0x%x\n",(s32)i,(s32)&ncm_app_ctx_set[i].cbs);
        (void)  printk("\r The NCM[%d]:PS配置的包封装结构            :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].pkt_encap_info);      
        (void)  printk("\r The NCM[%d]:流控状态                      :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].u32FlowCtrlStus);        
        (void)  printk("\r The NCM[%d]:当前发包阈值                  :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].u32TxCurMinNum);      
        (void)  printk("\r The NCM[%d]:当前发包超时时间              :0x%x\n",(s32)i,(s32)ncm_app_ctx_set[i].u32TxCurTimeout);        
        (void)  printk("\r     The NCM[%d]:PS配置的包封装结构buf偏移量   :0x%x\n",
            (s32)i,(s32)ncm_app_ctx_set[i].pkt_encap_info[0]); 
        (void)  printk("\r     The NCM[%d]:PS配置的包封装结构len偏移量   :0x%x\n",
            (s32)i,(s32)ncm_app_ctx_set[i].pkt_encap_info[1]); 
        (void)  printk("\r     The NCM[%d]:PS配置的包封装结构next偏移量  :0x%x\n",
            (s32)i,(s32)ncm_app_ctx_set[i].pkt_encap_info[2]); 
        (void)  printk("\r The NCM[%d]:debug信息结构                  :0x%x\n",(s32)i,(s32)&ncm_app_ctx_set[i].debug_info);
        (void)  printk("\r The NCM[%d]:PS配置的收发包统计             :0x%x\n",(s32)i,(s32)&ncm_app_ctx_set[i].pkt_stats); 
    }
}

void ncm_vendorctx_show(void)
{
    u32 i;/*lint !e578*/
    ncm_app_ctx_t *app_ctx;
    ncm_vendor_ctx_t *vendor_ctx;

    for (i = 0; i < GNET_MAX_NUM; i++)
    {
        app_ctx = &ncm_app_ctx_set[i];
        vendor_ctx = app_ctx->vendor_ctx;

        if(!vendor_ctx)
        {
            continue;
        }
        
        printk("|-+ncm[%d] vendor ctx show  :\n",i);
        printk("| |--is_mbim_mode           :%u\n",vendor_ctx->is_mbim_mode);
        printk("| |--connect                :%u\n",vendor_ctx->connect);
        printk("| |--tx_task_run            :%u\n",vendor_ctx->tx_task_run);
        printk("| |--u32UpBitRate           :%u\n",vendor_ctx->speed.u32UpBitRate);
        printk("| |--u32DownBitRate         :%u\n",vendor_ctx->speed.u32DownBitRate);
        printk("| |--stat_rx_total          :%lu\n",vendor_ctx->stat_rx_total);
        printk("| |--stat_rx_einval         :%lu\n",vendor_ctx->stat_rx_einval);
        printk("| |--stat_tx_total          :%lu\n",vendor_ctx->stat_tx_total);
        printk("| |--stat_tx_cancel         :%lu\n",vendor_ctx->stat_tx_cancel);
        printk("| |--stat_tx_xmit           :%lu\n",vendor_ctx->stat_tx_xmit);
        printk("| |--stat_tx_xmit_fail      :%lu\n",vendor_ctx->stat_tx_xmit_fail);
        printk("| |--stat_tx_post           :%lu\n",vendor_ctx->stat_tx_post);
        printk("| |--stat_tx_drop           :%lu\n",vendor_ctx->stat_tx_drop);
        printk("| |--stat_tx_nochl          :%lu\n",vendor_ctx->stat_tx_nochl);
        printk("| |--stat_tx_nodev          :%lu\n",vendor_ctx->stat_tx_nodev);
        printk("| |--stat_notify_timeout    :%lu\n",vendor_ctx->stat_notify_timeout);
    }
}
#else
s32 ncm_vendor_open(NCM_DEV_TYPE_E dev_type, u32 dev_id)
{
    return 0;
}

s32 ncm_vendor_close(NCM_DEV_TYPE_E dev_type, u32 dev_id)
{
    return 0;
}

s32 ncm_vendor_write(u32 dev_id, void *pkt_encap, void *net_priv)
{
    return 0;
}

s32 ncm_vendor_ioctl(u32 dev_id, NCM_IOCTL_CMD_TYPE_E cmd, void * param)
{
    return 0;
}
#endif  /* CONFIG_ETH_BYPASS_MODE */
/*lint -restore*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

