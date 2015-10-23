#ifndef __STMMAC_DEBUG__
#define __STMMAC_DEBUG__

#include <linux/kernel.h>
#include <linux/skbuff.h>

/* gmac msg level */
#define GMAC_LEVEL_ERR	            BIT(0)
#define GMAC_LEVEL_WARNING          BIT(1)
#define GMAC_LEVEL_TRACE	        BIT(2)
#define GMAC_LEVEL_DBG	            BIT(3)
#define GMAC_LEVEL_INFO	            BIT(4)
#define GMAC_LEVEL_BUG	            BIT(5)
#define GMAC_LEVEL_CHIP_DBG         BIT(6)
#define GMAC_LEVEL_CHIP_INFO        BIT(7)
#define GMAC_LEVEL_SKB	            BIT(8)
#define GMAC_LEVEL_BYTE	            BIT(9)
#define GMAC_DWMAC_LIB_DBG	        BIT(10)
#define GMAC_LEVEL_RX_DBG	        BIT(11)
#define GMAC_LEVEL_TX_DBG	        BIT(12)

extern unsigned int gmac_msg_level;

#define GMAC_ERR(args)			do {if (gmac_msg_level & GMAC_LEVEL_ERR) printk args;} while (0)
#define GMAC_WARNING(args)			do {if (gmac_msg_level & GMAC_LEVEL_WARNING) printk args;} while (0)
#define GMAC_BUG(args,condition)          \
    do {\
        if (condition)\
        {\
            printk args;\
        }\
\
        if (gmac_msg_level & GNET_LEVEL_BUG)\
        {\
            BUG_ON(condition);\
        }\
    } while (0)

#define CONFIG_GMAC_DEBUG
#ifdef CONFIG_GMAC_DEBUG
#define GMAC_TRACE(args)		    do {if (gmac_msg_level & GMAC_LEVEL_TRACE) printk args;} while (0)
#define GMAC_DBG(args)			    do {if (gmac_msg_level & GMAC_LEVEL_DBG) printk args;} while (0)
#define GMAC_INFO(args)		        do {if (gmac_msg_level & GMAC_LEVEL_INFO) printk args;} while (0)
#define GMAC_CHIP_DBG(args)		    do {if (gmac_msg_level & GMAC_LEVEL_CHIP_DBG) printk args;} while (0)
#define GMAC_CHIP_INFO(args)		do {if (gmac_msg_level & GMAC_LEVEL_CHIP_INFO) printk args;} while (0)
#define DWMAC_LIB_DBG(args)         do {if (gmac_msg_level & GMAC_DWMAC_LIB_DBG) printk args;} while (0)
#define GMAC_RX_DBG(args)           do {if (gmac_msg_level & GMAC_LEVEL_RX_DBG) printk args;} while (0)
#define GMAC_TX_DBG(args)           do {if (gmac_msg_level & GMAC_LEVEL_TX_DBG) printk args;} while (0)


static inline void print_bytes(struct stmmac_priv *priv, char *data, u32 len)
{
    if(gmac_msg_level & GMAC_LEVEL_BYTE)
    {
        u32 cnt = 0;

        printk("------pkt start------\n");
        for(cnt=0; cnt<len; cnt++)
        {
            printk("%02x ",data[cnt]);
        }
        printk("------pkt end  ------\n");
    }
}

static void print_pkt(unsigned char *buf, int len)
{
    if(gmac_msg_level & GMAC_LEVEL_SKB) {
    	int j;
    	printk("len = %d byte, buf addr: 0x%p", len, buf);
    	for (j = 0; j < len; j++) {
        	if ((j % 16) == 0)
    	    	printk("\n %03x:", j);
    	    printk(" %02x", buf[j]);
        }
        printk("\n");
    }
}

#else   /* CONFIG_GMAC_DEBUG */
#define GMAC_TRACE(args)
#define GMAC_DBG(args)
#define GMAC_INFO(args)
#define GMAC_CHIP_DBG(args)
#define GMAC_CHIP_INFO(args)
#define DWMAC_LIB_DBG(args)
#define GMAC_RX_DBG(args)
#define GMAC_TX_DBG(args)

static inline void print_bytes(struct eth_dev *dev, char *data, u32 len)
{   return ;    }
static inline void print_skb(struct eth_dev *dev, struct sk_buff *skb)
{   return ;    }
static inline int gnet_rx_isdrop(struct sk_buff *skb)
{   return 0;   }
#endif  /* CONFIG_GMAC_DEBUG */

#endif /* __STMMAC_DEBUG__ */
