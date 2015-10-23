
#include <linux/kernel.h>
#include <linux/etherdevice.h>
#include "stmmac.h"
#include "stmmac_debug.h"

#define GMAC_CONTROL		0x00000000	/* Configuration */
#define GMAC_CONTROL_LM		0x00001000 /* Loop-back mode */

/*gmac netdev, add by wangweichao*/
extern struct stmmac_priv *g_priv_dev;

#define LOOPBACK_TEST_OK 0
#define LOOPBACK_TEST_ERROR -1

#define LOOPBACK_RX_OK 0
#define LOOPBACK_RX_LIMITED -1          /**there is not enough receive packets*/
#define LOOPBACK_RX_ERROR -2           /**there is not enough receive packets*/

#define LOOPBACK_TX_OK 0
#define LOOPBACK_TX_ERROR -1
#define LOOPBACK_MATCH_ERROR -2

#define TEST_PACKET_LEN 76

struct sk_buff * g_tx_skb= NULL;
int g_rx_result;
static int packet[TEST_PACKET_LEN]  = {

                        0x00,0x00,0x5e,0x00,0x01,0x69,0x00,0xe0,0x4c,0x97,
                        0xa7,0x6a,0x08,0x00,0x45,0x00,0x00,0x3e,0xdd,0x3a,
                        0x00,0x00,0x40,0x11,0x44,0x6c,0x0a,0x91,0x3d,0xae,
                        0xda,0x19,0x36,0xb0,0x22,0x68,0x1f,0x77,0x00,0x2a,
                        0x38,0xa1,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x16,0x0f,0xa0,0x00,0x00,0x00,0x10,
                        0x71,0xef,0x7f,0xea,0x3f,0x6c,0x46,0x6a,0x96,0x7c,
                        0xb4,0x6b,0xaf,0xaa,0xa9,0x04,
                        };



int local_receive_skb(int frame_len,struct sk_buff *skb)
{

    int rx_pkt[100];
    int ret=0;
    int i=0;

    ret = LOOPBACK_RX_LIMITED;

    if(TEST_PACKET_LEN==frame_len)
    {
        memcpy(rx_pkt,(uint8_t *)skb->data,frame_len);

        for(i;i<TEST_PACKET_LEN;i++)
        {
            if(rx_pkt[i]!=packet[i])
            {
                break;
            }
        }
        if(TEST_PACKET_LEN!=i)
        {
            GMAC_ERR(("%s:match failed.\n",__FUNCTION__));
            return LOOPBACK_RX_ERROR;
        }
        GMAC_DBG(("%s:match success.\n",__FUNCTION__));
        ret = LOOPBACK_RX_OK;
    }

    g_rx_result = ret;
    return ret;
}
EXPORT_SYMBOL(local_receive_skb);


/*************************************************
  Function:      receive_packet
  Description:    接收数据，
  Input:
  Output:       接收数据长度
  Return:
  Others:
*************************************************/
int stmmac_receive_packet()
{
    int ret=0;
    struct sk_buff *skb;
    struct stmmac_priv *priv;
    int limit = 64;   /*this value may be need modified*/

    priv = g_priv_dev;
    ret = stmmac_rx(priv, limit);

    return ret;
}


int stmmac_send_packet()
{
    int ret=0;

    struct sk_buff *skb;
    struct stmmac_priv *priv;
    struct net_device *ndev;

    priv = g_priv_dev;
    ndev = priv->dev;

    skb = netdev_alloc_skb_ip_align(priv, 1528);
    if(NULL == skb)
    {
        GMAC_ERR(("%s:no packet to send,return\n",__FUNCTION__));
        return LOOPBACK_TX_ERROR;
    }

    memcpy(skb->data,packet,76);
    skb->len = 76;

    /*send the packet*/
    ret = stmmac_xmit(skb,priv);
    if(NETDEV_TX_OK != ret) {
        GMAC_ERR(("%s:xmit error\n",__FUNCTION__));
        return LOOPBACK_TX_ERROR;
    }

    g_tx_skb = skb;

    return ret;

}

void stop_test(void)
{
    if(g_tx_skb){
        dev_kfree_skb_any(g_tx_skb);
        g_tx_skb = NULL;
    }

    if (g_priv_dev){
        g_priv_dev = 0;
    }

    if(g_rx_result){
        g_rx_result = 0;
    }
}

int start_test()
{

    int ret=0;

    ret = stmmac_send_packet();
    if(ret){
        GMAC_ERR(("%s:send packet error:ret=%d\n",__FUNCTION__,ret));
        return LOOPBACK_TX_ERROR;
    }

    msleep(100);

#if 0
    ret = stmmac_receive_packet();
    if(!ret){
        GMAC_ERR(("%s:recived count is zero! len=%d\n",__FUNCTION__,ret));
        return LOOPBACK_RX_ERROR;
    }
#endif

    if(g_rx_result){

        GMAC_ERR(("%s:tx and rx do not match!\n",__FUNCTION__));
        return LOOPBACK_MATCH_ERROR;
    }

    return ret;
}


int disable_lookback(void __iomem *ioaddr)
{

	u32 value = readl(ioaddr + GMAC_CONTROL);
	value &= ~GMAC_CONTROL_LM;
	writel(value, ioaddr + GMAC_CONTROL);

}

int enable_lookback(void __iomem *ioaddr)
{
	u32 value = readl(ioaddr + GMAC_CONTROL);
	value |= GMAC_CONTROL_LM;
	writel(value, ioaddr + GMAC_CONTROL);
}

int start_loopback_test()
{
    struct stmmac_priv *priv;
    int ret = 0;

    GMAC_DBG(("%s:loopback test begain! \n",__FUNCTION__));
    priv = g_priv_dev;

    /*set internal loopback mode*/
    enable_lookback(priv->ioaddr);
    ret = start_test();
    if (ret) {
        GMAC_ERR(("%s:loopback test failed! ret = %d\n",__FUNCTION__,ret));
        return ret;
    }
    GMAC_DBG(("%s:loopback test sucess! \n",__FUNCTION__));
    return LOOPBACK_TEST_OK;
}

int stop_loopback_test()
{
    struct stmmac_priv *priv;

    priv = g_priv_dev;

    /*set internal loopback mode*/
    disable_lookback(priv->ioaddr);
    stop_test();
    GMAC_DBG(("%s:stop loopback test sucess! \n",__FUNCTION__));
    return 0;
}


