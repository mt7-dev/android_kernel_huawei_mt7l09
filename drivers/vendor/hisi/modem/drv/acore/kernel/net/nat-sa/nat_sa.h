/**
 * nat_sa.h - nat_sa.c header file
 *
 * Copyright (C) 2012-2013 HiSilicon Technologies Co.,LTD.- http://www.hisilicon.com
 *
 * Authors: hisi-bsp4
 *
 *
 */

#ifndef __SOFTNAT_H__
#define __SOFTNAT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/kref.h>
#include <linux/compiler.h>

#define	ETHER_ADDR_LEN		6
#define SA_ACTION_TAG		(1<<0)
#define SA_ACTION_UNTAG	(1<<1)
#if 0
#define SA_ACTION_SNAT		(1<<2)
#define SA_ACTION_DNAT		(1<<3)
#endif
#define SA_ACTION_SNAT		0
#define SA_ACTION_DNAT		1

#define LIMIT_PKTNUM	5

#define NSA_OK	1
#define NSA_FAIL 0

#define PRINT_ENTRY_ENABLE 1
#define PRINT_ENTRY_DISABLE 0

/* message levels */
#define NAT_SA_ERROR_VAL	0x0001
#define NAT_SA_TRACE_VAL	0x0002
#define NAT_SA_INFO_VAL		0x0004
#define NAT_SA_DBG_VAL		0x0008

extern int nat_sa_msg_level;


/* *
 * NAT_SA_DEBUG and NAT_SA_REL_TRACE to control the print msg
 * in debug mode, NAT_SA_DEBUG is defined;
 * in release mode, NAT_SA_REL_TRACE is defined;
 * if desired, NAT_SA_REL_TRACE may be not defined;
 */
#if defined(CONFIG_NAT_SA_DEBUG) || defined(CONFIG_NAT_SA_REL_TRACE)
#define NAT_SA_INFO(args)		do {if (nat_sa_msg_level & NAT_SA_INFO_VAL) printk args;} while (0)
#define NAT_SA_ERROR(args)		do {if (nat_sa_msg_level & NAT_SA_ERROR_VAL) printk args;} while (0)
#define NAT_SA_TRACE(args)		do {if (nat_sa_msg_level & NAT_SA_TRACE_VAL) printk args;} while (0)
#else

#define NAT_SA_INFO(args)
#define NAT_SA_ERROR(args)
#define NAT_SA_TRACE(args)
#endif /* defined NAT_SA_TRACE  or NAT_SA_DEBUG*/


#if defined(CONFIG_NAT_SA_DEBUG)
#define NAT_SA_DBG(args)		do {if (nat_sa_msg_level & NAT_SA_DBG_VAL) printk args;} while (0)
#else
#define NAT_SA_DBG(args)
#endif /* defined NAT_SA_DEBUG */


#if defined(CONFIG_NAT_SA_DEBUG)
inline void  NS_printbyte(char *head, unsigned int len)
{
	int i;
	NAT_SA_DBG(("\n"));
	
	for(i = 0; i < len; i++)
	{
		NAT_SA_DBG(("%02x ", head[i])); 			
	}

	NAT_SA_DBG(("\n")); 			
}	
	

#else
inline void  NS_printbyte(char *head, unsigned int len)
{}
#endif




#define NAT_SA_PRE_ROUTING 0
#define NAT_SA_POST_ROUTING 4

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned long long uint64;

struct sa_stat{
	uint32 hit_num;
	uint32 lost_num;
};


enum nat_sa_manip_type
{
	NAT_SA_MANIP_SRC,
	NAT_SA_MANIP_DST
};


/*
 *Structure of a 48-bit Ethernet address
 */
struct ether_addr {
	uint8 octet[ETHER_ADDR_LEN];
} ;

struct sa_nat{
	uint32 ip;
	uint16 port;
}sa_nat;

struct sa_conn_tuple{
	uint32 sip,dip;
	uint16 sp,dp;
	uint8 proto;
}sa_conn_tuple_t;

struct sa_fw_entry{
	struct hlist_node   sa_node;              /*poionter to next entry*/
	struct kref user_cnt;			/* entry ref count */	
	struct sa_conn_tuple tuple;				/*tuple to track the stream*/
//	uint32              hits;				/*num frames matching sa entry,just for statistic*/

	uint32             age;				/*jiffies used to expire the entrty*/
	uint32				action;				/* nat operation type */
	struct sa_nat       nat;				/*manip data for SNAT,DNAT*/
	struct net_device	*output_dev;
	struct ether_addr   dhost;				/*destination MAC address*/
	struct ether_addr   shost;				/*source MAC address*/
};

struct sa_htable{
	struct hlist_head sa_head;
	uint32 hlist_count;
	rwlock_t	sa_rwlock;
};

struct sa_pending_queue {
	struct sk_buff_head sa_pending_txqueue;
	struct net_device *sa_pending_netdev;
	//int netdev_flag;
};

/* two bridge net-devices, e.g:br0 wan0 */
#define MAX_NET_DEVICE 2
/* br0 named DOWN_NET_DEVICE, wan0 UP_NET_DEVICE */
#define UP_NET_DEVICE 0
#define DOWN_NET_DEVICE 1

struct sa_ctl{
	unsigned long flags;
	struct sa_htable *sa_table;	
	struct sa_pending_queue sa_pqueue[MAX_NET_DEVICE*2];
    struct sa_stat stat;
}sa_ctl_type;


#define NAT_SA_ENABLE 1
#define NAT_SA_DISABLE 0

#define NAT_SA_MASK_ENABLE 0x00000001


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SOFTNAT_H__ */
