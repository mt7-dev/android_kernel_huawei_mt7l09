/**
 * nat_sa.c - nat soft speed acceleration mechanism
 *
 * Copyright (C) 2012-2013 HiSilicon Technologies Co.,LTD.- http://www.hisilicon.com
 *
 * Authors: hisi-bsp4
 *
 *
 */
 
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/if.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <linux/rwlock_types.h>
#include <linux/rwlock.h>
#include <linux/ip.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/if_ether.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/kref.h>

#include <net/checksum.h>
#include <net/ip.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/route.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack.h>


//#include <net/wireless/bcm43239/include/linuxver.h>

#include "nat_sa.h"

MODULE_LICENSE("GPL");
MODULE_ALIAS("nat_soft_accelaration");


int nat_sa_msg_level = NAT_SA_ERROR_VAL;


extern NatEntryUpdate g_pNatEntryUpdate;
extern NatEntryDelete g_pNatEntryDelete;

/* Translate a packet to the target according to manip type.
   Return true if succeeded. */
int (*nat_manip_pkt)(struct sk_buff *skb,
		 unsigned int iphdroff,
		 struct sa_fw_entry *manip_entry);

struct sa_htable *g_HTBySource;
unsigned int nat_sa_tablesize = 0;
struct kmem_cache *sa_htable_slab;
struct kmem_cache *sa_entry_slab;

struct tasklet_struct g_sa_task;
struct sa_ctl g_sactl;

static unsigned int g_timeout=10;
static unsigned int g_loop_limit=50;


typedef int (*sa_match_func_type)(struct sk_buff *skb);
extern sa_match_func_type sa_match_func;
extern int sa_enable;

#if 0
unsigned int ack_num[4];
unsigned int tx_num[4];
#endif

char sa_info[]= "version compiled at " __DATE__" "__TIME__ "\n";

void print_sa_entry()
{
	struct sa_htable *htbysource;
	int i,entry_idx;
	struct sa_fw_entry *sa_entry;
	struct hlist_node *hnode;
	
	htbysource = g_HTBySource;

	if(NULL == htbysource) {
		printk(KERN_ERR "ERROR:sa entry hash table in NULL\n");
		return;
	}

	printk(KERN_ALERT "sa entries in hash list as below:\n");
	printk(KERN_ALERT "  seq proto    sip:sport    dip:dport S/DNAT    natip:natport\n");

	for(i = 0; i < nat_sa_tablesize; i++)
	{
		read_lock(&(htbysource->sa_rwlock));
		if (htbysource->hlist_count != 0) {	
			entry_idx = 0;
		/* roll over this hash list to print */
			hlist_for_each_entry(sa_entry,hnode,&(htbysource->sa_head), sa_node) {
				entry_idx ++;

				printk(KERN_ALERT "  #%d.%d [%d]:%pI4:%u -->> %pI4:%u %s %pI4:%u\n",
                    i,entry_idx,sa_entry->tuple.proto,
					&(sa_entry->tuple.sip), ntohs(sa_entry->tuple.sp), 
					&(sa_entry->tuple.dip), ntohs(sa_entry->tuple.dp),
					(sa_entry->action==1?"D":"S"),
					&(sa_entry->nat.ip), ntohs(sa_entry->nat.port));
			}
		}
		read_unlock(&(htbysource->sa_rwlock));							
		
		/* roll over for the next hash list */
		htbysource = htbysource + 1;		
	}	

	
}
void nat_sa_set_timeout(unsigned int timeout)
{
	if(0 == timeout) {
		printk(KERN_ALERT "current timeout value is %d\n",g_timeout);
		return;
	}
	g_timeout = timeout;
	return;
}
void nat_sa_set_looplimit(unsigned int limit)
{
	if(0 == limit) {
		printk(KERN_ALERT "current looplimit value is %d\n",g_loop_limit);
		return;
	}
	g_loop_limit = limit;
	return;
}

void print_sa_stat(void)
{
    struct sa_stat *stat = &g_sactl.stat;
	printk(KERN_EMERG "total: hit_num = %d,lost_num = %d\n", stat->hit_num, stat->lost_num);
	return;
}

#if 0
void print_acknum_func(void)
{
	int i = 0;
	for(i = 0; i<4; i++){
		printk("ack_num[%d] = %d\n", i, ack_num[i]);
		printk("tx_num[%d] = %d\n", i, tx_num[i]);
		printk("drop_pkt = %d\n", ack_num[i] - tx_num[i]);
		printk("---------------------\n");
	}
	return;

}
#endif

void nat_sa_set_level(unsigned int level)
{
	nat_sa_msg_level = level;
	return;
}
void nat_sa_set_flags(unsigned int flag)
{
	g_sactl.flags = flag;
	sa_enable = flag;
	return;
}

static int nat_sa_device_event(struct notifier_block *this, unsigned long event,
			     void *ptr)
{
	struct net_device *dev = ptr;
	int netdev_num;
	int queue_num;
	/* filter name array should be defined as order, as the queue DOWN_NET_DEVICE is 1, UP is 0 */
	static char *filter_out_netdevice_name[MAX_NET_DEVICE]={"wan0","br0"};

	NAT_SA_INFO(("%s: 100: event = %ld received from dev = %s \n", __FUNCTION__, event, dev->name));

	switch (event) {
	case NETDEV_REGISTER:
		for(netdev_num = 0; netdev_num < MAX_NET_DEVICE; netdev_num++) {
			if (g_sactl.sa_pqueue[netdev_num<<1].sa_pending_netdev == NULL) {
				if (0 == strcmp(dev->name, filter_out_netdevice_name[netdev_num])) {
					queue_num = netdev_num<<1;
					/* initialize the corresponding two queues */
					g_sactl.sa_pqueue[queue_num].sa_pending_netdev = dev;
					skb_queue_head_init(&(g_sactl.sa_pqueue[queue_num].sa_pending_txqueue));
					g_sactl.sa_pqueue[queue_num+1].sa_pending_netdev = dev;
					skb_queue_head_init(&(g_sactl.sa_pqueue[queue_num+1].sa_pending_txqueue));
					
					dev_hold(g_sactl.sa_pqueue[queue_num].sa_pending_netdev);
					printk(KERN_NOTICE "new net_device %s registered, expected! \n",dev->name);
					return NOTIFY_OK;
				}						
			}	
			else
				continue;			
		}
		
		printk(KERN_INFO "new net_device %s registered, not expected! \n",dev->name);
		break;
	
	case NETDEV_UNREGISTER:
		for (netdev_num = 0; netdev_num < MAX_NET_DEVICE; netdev_num++) {
			if(dev == g_sactl.sa_pqueue[netdev_num<<1].sa_pending_netdev){
				queue_num = netdev_num<<1;
				/* clear the corresponding two queues */
				skb_queue_purge(&(g_sactl.sa_pqueue[queue_num].sa_pending_txqueue));			
				g_sactl.sa_pqueue[queue_num].sa_pending_netdev = NULL;
				skb_queue_purge(&(g_sactl.sa_pqueue[queue_num+1].sa_pending_txqueue));			
				g_sactl.sa_pqueue[queue_num+1].sa_pending_netdev = NULL;
				
				dev_put(g_sactl.sa_pqueue[queue_num].sa_pending_netdev);
				printk(KERN_NOTICE "net_device %s unregistered, queue cleared! \n",dev->name);
				return NOTIFY_OK;
			}
		}
		
		printk(KERN_INFO "net_device %s unregistered, not expected! \n",dev->name);
		break;

	default:
		NAT_SA_INFO(("%s: 100: unconcerned event = %d received from dev = %s \n", __FUNCTION__, event, dev->name));
		break;
	}	

	return NOTIFY_DONE;
}

void release_sa_entry (struct kref *kref)
{
	struct sa_fw_entry *sa_entry=container_of(kref,struct sa_fw_entry,user_cnt);

	NAT_SA_INFO(("%s: 1000: free the sa_entry_slab \n", __FUNCTION__));
	kmem_cache_free(sa_entry_slab, sa_entry);	
}

static inline void put_sa_entry(struct sa_fw_entry *sa_entry)
{
	kref_put(&(sa_entry->user_cnt),release_sa_entry);
}

static inline void get_sa_entry(struct sa_fw_entry *sa_entry)
{
	kref_get(&(sa_entry->user_cnt));
}

/*get hash table key*/
static inline unsigned int nat_sa_hash_conntrack(uint32 saddr,uint32 daddr,uint32 proto, uint16 sport, uint16 dport)
{
	unsigned int hindex;
    unsigned mask = nat_sa_tablesize - 1;

	hindex = saddr ^ daddr ^ (proto  + sport + dport);
	
	return hindex & mask;
}

/* add one entry to the hash table */
void htable_sa_add(struct sa_fw_entry *sa_entry)
{
	unsigned int hindex;
	unsigned long flags;
	struct sa_htable * hash_table;
	
	NAT_SA_DBG(("%s: 100: enter\n", __FUNCTION__));
	// if support the hash, calculate the index for this entry here

	#if 0
	NAT_SA_TRACE(("%s:200 entry in htable [%d]: %pI4:%u -->> %pI4:%u \n",
			__FUNCTION__, sa_entry->tuple.proto, &(sa_entry->tuple.sip), ntohs(sa_entry->tuple.sp), 
			&(sa_entry->tuple.dip), ntohs(sa_entry->tuple.dp)));
	#endif
	
	hindex = nat_sa_hash_conntrack(sa_entry->tuple.sip, sa_entry->tuple.dip, 
										sa_entry->tuple.proto, sa_entry->tuple.sp, sa_entry->tuple.dp);
	NAT_SA_TRACE(("%s: 150: preparing to add hindex = %d\n", __FUNCTION__, hindex));

    hash_table = g_HTBySource + hindex;
    
    /* check whether the entry is duplicate */
    read_lock_irqsave(&(hash_table->sa_rwlock),flags);
	if (hash_table->hlist_count != 0) {
        struct sa_fw_entry *entry;
        struct hlist_node *hnode;

        hlist_for_each_entry(entry,hnode,&(hash_table->sa_head), sa_node) {
			if(sa_entry->tuple.sip == entry->tuple.sip && sa_entry->tuple.dip == entry->tuple.dip \
        		&& sa_entry->tuple.proto == entry->tuple.proto && sa_entry->tuple.sp == entry->tuple.sp \
        		&& sa_entry->tuple.dp == entry->tuple.dp) {
        		read_unlock_irqrestore(&(hash_table->sa_rwlock),flags);
                /* free the entry because of duplicate entry */
                kmem_cache_free(sa_entry_slab, sa_entry);
                NAT_SA_DBG(("%s: ===duplicated entry===\n",__FUNCTION__));
                return;
			}
        }
    }	
    read_unlock_irqrestore(&(hash_table->sa_rwlock),flags);
    
    write_lock_irqsave(&(hash_table->sa_rwlock),flags);
	hash_table->hlist_count++;
	hlist_add_head(&(sa_entry->sa_node), &(hash_table->sa_head));
	write_unlock_irqrestore(&(hash_table->sa_rwlock),flags);
	
	NAT_SA_DBG(("%s:200:add the entry node to hash table\n", __FUNCTION__));	
	return;
}

void htable_sa_delete(uint32 saddr,uint32 daddr,uint32 proto, uint16 sport, uint16 dport)
{
	unsigned int hindex;
	unsigned long flags;
	struct sa_htable * hash_table;
	struct sa_fw_entry *sa_entry;
	struct hlist_node *hnode;

	NAT_SA_DBG(("%s: 300: enter\n", __FUNCTION__));
	
	//struct hlist_node   sa_node; 
	/* just get the first hash table*/
	
	hindex = nat_sa_hash_conntrack(saddr, daddr, proto, sport, dport);
	NAT_SA_TRACE(("%s: 350: deleting hindex = %d\n", __FUNCTION__, hindex));
	hash_table = g_HTBySource + hindex;
	
	write_lock_irqsave(&(hash_table->sa_rwlock),flags);
	/* search the table for the node entry */
	hlist_for_each_entry(sa_entry,hnode,&(hash_table->sa_head), sa_node) {
		/* compare to locate the node*/
		if(sa_entry->tuple.sip == saddr && sa_entry->tuple.dip == daddr \
		&& sa_entry->tuple.proto == proto && sa_entry->tuple.sp == sport \
		&& sa_entry->tuple.dp == dport)
			break;
		else
			continue;
	}
	//read_unlock_irqrestore(&(hash_table->sa_rwlock),flags);
	if(NULL==hnode){
		NAT_SA_ERROR(("%s: 400: no match entry found\n", __FUNCTION__));		
		write_unlock_irqrestore(&(hash_table->sa_rwlock),flags);
		return;
	}

	//write_lock_irqsave(&(hash_table->sa_rwlock),flags);
	/* delete the node entry from the hash list */
	hlist_del(&(sa_entry->sa_node));
	hash_table->hlist_count--;
	write_unlock_irqrestore(&(hash_table->sa_rwlock),flags);
	
	NAT_SA_DBG(("%s: 500: delete the list from the htable\n", __FUNCTION__));	
	
	NAT_SA_INFO(("%s: 600: free the sa_entry_slab \n", __FUNCTION__));
	put_sa_entry(sa_entry);

	return;
}


/*This contains the information to distiguish a connection*/

/* add the entry according to the nf_conntrack infermation*/
void sa_entry_add(struct sk_buff *skb, uint32 hooknum,
								struct nf_conn *ct,enum ip_conntrack_info ci,
								struct nf_conntrack_tuple *manip)
{
	struct sa_fw_entry *sa_entry;
	struct hh_cache *hh;
	struct ethhdr *eth;
	struct iphdr *iph;
	struct tcphdr *tcph;
	uint32 daddr;
	struct rtable *rt;
	struct nf_conn_help *help;
    int i;
	
	NAT_SA_DBG(("%s: 100: enter\n", __FUNCTION__));
	
	if((skb == NULL) || (ct == NULL)|| (manip == NULL)){
		NAT_SA_INFO(("%s:155: skb,ct or manip is NULL\n", __FUNCTION__)); 
		return;
	}
		
	/* We only add cache entires for non-helper connections and at
	 * pre or post routing hooks.
	 */
	help = nfct_help(ct);
	if ((help && help->helper) || 
		((hooknum != NAT_SA_PRE_ROUTING) && (hooknum != NAT_SA_POST_ROUTING))){
		NAT_SA_TRACE(("%s:200: the help and hooknum is error\n", __FUNCTION__));
		return;
	}

	/* Add ipc entries for connections in established state only */

	if ((ci != IP_CT_ESTABLISHED) && (ci != (IP_CT_ESTABLISHED+IP_CT_IS_REPLY))){
		NAT_SA_TRACE(("%s:300: not established when add entries!\n", __FUNCTION__));
		return;
	}

    /* currently JUST nat sa the ftp data packets for the application which has alg helpers */
    if (ct->master) {
        help = nfct_help(ct->master);
        NAT_SA_INFO(("%s:===alg data connection occured===\n", __FUNCTION__));
        if (strncmp(help->helper->name, "ftp", 3)) {
            NAT_SA_INFO(("%s:===non ftp data connection ignored===\n", __FUNCTION__));
            return;
        }
    }    
	
	//skb data shoud be ip header  check
	iph = (struct iphdr *)(skb->data);

	if (iph->version != 4){
		NAT_SA_ERROR(("%s:400: ip version is error!\n", __FUNCTION__));
		return;
	}

	tcph = ((struct tcphdr *)(((__u8 *)iph) + (iph->ihl << 2)));
	
	if(iph->protocol == IPPROTO_TCP){	
		if(ct->proto.tcp.state >=TCP_CONNTRACK_FIN_WAIT &&
		    ct->proto.tcp.state <= TCP_CONNTRACK_TIME_WAIT)
		    return;
	}
	else if(iph->protocol !=IPPROTO_UDP)
	{
		NAT_SA_ERROR(("protocol error not tcp or udp\n"));
		return;
	}
	
	if(iph->ihl != 5){
		NAT_SA_ERROR(("%s:500: ihl is not 5!\n", __FUNCTION__));
		return;
	}		
	
	/* Do route lookup for alias address if we are doing DNAT in this
	 * direction.
	 */
	daddr = iph->daddr;
	if ((HOOK2MANIP(hooknum) == NAT_SA_MANIP_DST)){
		//NAT_SA_INFO(("%s:700: iph->daddr = %pI4\n", __FUNCTION__,&(daddr)));
		daddr = manip->dst.u3.ip; 
	}
	NAT_SA_DBG(("%s:700: iph->daddr = %pI4\n", __FUNCTION__,&(daddr)));
	
	/* Find the destination interface */
	
	if (skb_dst(skb) == NULL) 
	{
		//NAT_SA_INFO(("%s:800: check the dest addr's routing info\n", __FUNCTION__));
		ip_route_input(skb, daddr, iph->saddr, iph->tos, skb->dev);
	}

	/* Ensure the packet belongs to a forwarding connection and it is
	 * destined to an unicast address.
	 */
	rt = skb_rtable(skb);

	if ((rt == NULL) || (skb->sk != NULL) ||
	    (rt->rt_type != RTN_UNICAST)){
	     NAT_SA_ERROR(("%s:900: rt error!\n", __FUNCTION__));
		 	 
		 return;
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
    if ((rt->dst._neighbour == NULL) ||
	    ((rt->dst._neighbour->nud_state &
	     (NUD_PERMANENT|NUD_REACHABLE|NUD_STALE|NUD_DELAY|NUD_PROBE)) == 0)) {
	     NAT_SA_ERROR(("%s:950: rt error!\n", __FUNCTION__));
         return;
    }
#else
    if ((rt->u.dst.neighbour == NULL) ||
	    ((rt->u.dst.neighbour->nud_state &
	     (NUD_PERMANENT|NUD_REACHABLE|NUD_STALE|NUD_DELAY|NUD_PROBE)) == 0)) {
	     NAT_SA_ERROR(("%s:950: rt error!\n", __FUNCTION__));
         return;
    }
#endif
	
	NAT_SA_DBG(("%s:920: skb sock = %p\n", __FUNCTION__, skb->sk));

	#if 0
	// malloc the node element
	sa_entry = kzalloc(sizeof(struct sa_fw_entry),GFP_ATOMIC);
		
	if(NULL == sa_entry) {
		NAT_SA_ERROR(("%s:1000: memeory alloc failed when allocating the node entry!\n", __FUNCTION__));
		return ;
	}
	#endif
	
	//use lookaside cache to malloc mempool

	sa_entry = kmem_cache_alloc(sa_entry_slab, GFP_ATOMIC);
	if (NULL == sa_entry){
		NAT_SA_ERROR(("%s: 940: malloc the sa_entry error!\n", __FUNCTION__));
		return;
	}
	
	//NAT_SA_INFO(("%s:1020: sa_entry = %p added\n", __FUNCTION__, sa_entry));

	kref_init(&(sa_entry->user_cnt));

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
	sa_entry->output_dev = rt->dst.dev;
#else
    sa_entry->output_dev = rt->u.dst.dev;
#endif
	if (sa_entry->output_dev)
		NAT_SA_INFO(("%s:1030: output_dev name: %s\n", __FUNCTION__, sa_entry->output_dev->name));
	else {
		/* should do something here */
		printk(KERN_ALERT "ERROR: null output net dev occured!!!\n");
	}

			
	// initial the node		
	INIT_HLIST_NODE(&(sa_entry->sa_node));// init the hlist node

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
	hh = &(rt->dst._neighbour->hh);
#elif (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
    hh = rt->dst._neighbour->hh;
#else
    hh = rt->u.dst.hh;
#endif
	if (hh != NULL) {
		
		NAT_SA_DBG(("%s:1050: hh_data(2+DMac+SMac+proto):\n", __FUNCTION__));

		{
			for(i = 0; i < 16; i++)
			{
	
					NAT_SA_DBG(("%02x ", *((char *)hh->hh_data + i)));				
			}

		}
		NAT_SA_DBG(("\n"));

		eth = (struct ethhdr *)(((unsigned char *)hh->hh_data) + 2);
		memcpy(sa_entry->dhost.octet, eth->h_dest, ETH_ALEN);
		memcpy(sa_entry->shost.octet, eth->h_source, ETH_ALEN);
	} 
	else 
	{
	
	NAT_SA_DBG(("%s:1080: rt->u.dst.neighbour->ha\n", __FUNCTION__));
		{
			for(i = 0; i < 20; i++)
			{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
					NAT_SA_DBG(("%02x ", *((char *)rt->dst._neighbour->ha + i)));
#else
                    NAT_SA_DBG(("%02x ", *((char *)rt->u.dst.neighbour->ha + i)));
#endif
			}

		}
	
		NAT_SA_DBG(("\n"));
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
		memcpy(sa_entry->dhost.octet, rt->dst._neighbour->ha, ETH_ALEN);
        memcpy(sa_entry->shost.octet, rt->dst.dev->dev_addr, ETH_ALEN);
#else
        memcpy(sa_entry->dhost.octet, rt->u.dst.neighbour->ha, ETH_ALEN);
        memcpy(sa_entry->shost.octet, rt->u.dst.dev->dev_addr, ETH_ALEN);
#endif
	}

	

	/* Add ct entry for this direction */
	sa_entry->tuple.sip = iph->saddr;
	sa_entry->tuple.dip = iph->daddr;
	sa_entry->tuple.proto = iph->protocol;
	sa_entry->tuple.sp = tcph->source;
	sa_entry->tuple.dp = tcph->dest;


	/* Update the manip ip and port */
	if (HOOK2MANIP(hooknum) == NAT_SA_MANIP_SRC) {
		sa_entry->nat.ip = manip->src.u3.ip;
		sa_entry->nat.port = manip->src.u.tcp.port;
		//sa_entry->action |= SA_ACTION_SNAT;
		sa_entry->action = SA_ACTION_SNAT;

	} else {
		sa_entry->nat.ip = manip->dst.u3.ip;
		sa_entry->nat.port = manip->dst.u.tcp.port;
		//sa_entry->action |= SA_ACTION_DNAT;
		sa_entry->action = SA_ACTION_DNAT;
	}

	//usb trace instead of printk
	
		NAT_SA_INFO(("%s:1200 Adding ipc entry for protocol [%d]: %pI4:%u -->> %pI4:%u\n", __FUNCTION__,
				sa_entry->tuple.proto, 
				&(sa_entry->tuple.sip), ntohs(sa_entry->tuple.sp), 
				&(sa_entry->tuple.dip), ntohs(sa_entry->tuple.dp)));

		NAT_SA_INFO(("sa %02x:%02x:%02x:%02x:%02x:%02x\n",
				sa_entry->shost.octet[0], sa_entry->shost.octet[1],
				sa_entry->shost.octet[2], sa_entry->shost.octet[3],
				sa_entry->shost.octet[4], sa_entry->shost.octet[5]));
		NAT_SA_INFO(("da %02x:%02x:%02x:%02x:%02x:%02x\n",
				sa_entry->dhost.octet[0], sa_entry->dhost.octet[1],
				sa_entry->dhost.octet[2], sa_entry->dhost.octet[3],
				sa_entry->dhost.octet[4], sa_entry->dhost.octet[5]));
				NAT_SA_INFO(("hooknum:[%d] action %d\n", hooknum, sa_entry->action));
	
	NAT_SA_INFO(("manip_ip: %pI4 manip_port %u\n",
			&(sa_entry->nat.ip), ntohs(sa_entry->nat.port)));

	/* the jiffies*/
	sa_entry->age = jiffies;
	//NAT_SA_INFO(("%s:1400: sa_entry->age=%u\n", __FUNCTION__,sa_entry->age));
	
	htable_sa_add(sa_entry);

}
	

/* del the entry */
void sa_entry_del(struct nf_conn *ct)
{
	struct nf_conntrack_tuple *orig, *repl;
	NAT_SA_DBG(("%s:100: enter\n", __FUNCTION__));

	orig = &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;

	if ((orig->dst.protonum != IPPROTO_TCP) && (orig->dst.protonum != IPPROTO_UDP))
		return ;

	repl = &ct->tuplehash[IP_CT_DIR_REPLY].tuple;

	/* If there are no packets over this connection for timeout period
	 * delete the entries.
	 */
	htable_sa_delete(orig->src.u3.ip,orig->dst.u3.ip,orig->dst.protonum, \
					orig->src.u.tcp.port, orig->dst.u.tcp.port);	

	NAT_SA_INFO(("%s:200 Deleting the orig tuple [%d]: %pI4:%u -->> %pI4:%u\n", __FUNCTION__,
			orig->dst.protonum, 
			&(orig->src.u3.ip), ntohs(orig->src.u.tcp.port), 
			&(orig->dst.u3.ip), ntohs(orig->dst.u.tcp.port)));
	
	htable_sa_delete(repl->src.u3.ip, repl->dst.u3.ip, repl->dst.protonum, \
					repl->src.u.tcp.port, repl->dst.u.tcp.port);

	NAT_SA_INFO(("%s:300 Deleting the repl tuple [%d]: %pI4:%u -->> %pI4:%u\n", __FUNCTION__,
			repl->dst.protonum, 
			&(repl->src.u3.ip), ntohs(repl->src.u.tcp.port), 
			&(repl->dst.u3.ip), ntohs(repl->dst.u.tcp.port)));
	return ;
}



/* search the table to match the skb */
/* return 1 if match, or return 0 */
/* if match, manipulate the packet, add the packet to the g_sactl.sa_skb_head*/
int sa_table_match(struct sk_buff *skb)
{
	unsigned int hindex;
	unsigned int ismatched=0;
	struct sa_htable * hash_table;
	struct sa_fw_entry *sa_entry;
	struct hlist_node *hnode;
	struct iphdr *iph;
	struct tcphdr *tcph;
	struct sk_buff *skb_dup;
		
	int ret;
	unsigned int pri_index;

	NAT_SA_DBG(("%s: 200: enter\n", __FUNCTION__));
  	
	/* reply on the caller to check whether enabled */

	if(!skb){
		NAT_SA_ERROR(("%s:300: sk_buff is NULL\n", __FUNCTION__));
		return 0;
	}

	NS_printbyte(skb->data - 14, 34);


	/* set the right ip header positiont */
	// to do
	//iphdroff = skb->mac_len;
	//NAT_SA_INFO(("%s: 200: skb->mac_len = %d\n", __FUNCTION__,skb->mac_len));

	//fragment deny
	
	iph = (struct iphdr *)(skb->data);

	if(iph->ttl <= 1)
	{
		return 0;		
	}

	
	if (iph->version != 4 ||
	    (iph->protocol != IPPROTO_TCP && iph->protocol != IPPROTO_UDP))
		return 0;

	if(iph->ihl != 5){
		NAT_SA_ERROR(("%s:560: ihl is not 5!\n", __FUNCTION__));
		return 0;
	}
	
	tcph = ((struct tcphdr *)(((__u8 *)iph) + (iph->ihl << 2)));


	if(iph->protocol == IPPROTO_TCP){
		if (tcph->rst) {
			NAT_SA_INFO(("====do not deal tcp rst pkt!===iph id = %d,iph length=%d \n",\
				ntohs(iph->id),ntohs(iph->tot_len)));
			return 0;
		}
		if (tcph->syn) {
			NAT_SA_INFO(("====do not deal tcp syn pkt!===iph id = %d,iph length=%d\n",\
				ntohs(iph->id),ntohs(iph->tot_len)));
			return 0;
		}
	}
	if (iph->frag_off & htons(IP_MF | IP_OFFSET)) {
		NAT_SA_INFO(("%s:566: do not deal ip fragment !===iph id = %d,iph length=%d \n",\
			__FUNCTION__,ntohs(iph->id), ntohs(iph->tot_len)));
	
		return 0;
	}

	/* count the hash index */
	
	hindex = nat_sa_hash_conntrack(iph->saddr, iph->daddr, iph->protocol,tcph->source,tcph->dest);
	NAT_SA_TRACE(("%s: 570: hindex = %d\n", __FUNCTION__, hindex));
	hash_table = g_HTBySource + hindex;
	//NAT_SA_INFO(("%s: 570: g_HTBySource=0x%x\n", __FUNCTION__,hash_table));
	/* iterate the hash list */
	read_lock(&(hash_table->sa_rwlock));

	/* check whether the hash list is empty */
	if(hlist_empty(&(hash_table->sa_head))) {
		read_unlock(&(hash_table->sa_rwlock));
		NAT_SA_INFO(("%s:580: hash list is empty!\n", __FUNCTION__));
		return 0;
	}
	
	hlist_for_each_entry(sa_entry,hnode,&hash_table->sa_head,sa_node){
		if(sa_entry->tuple.sip == iph->saddr && sa_entry->tuple.dip == iph->daddr \
		&& sa_entry->tuple.proto == iph->protocol && sa_entry->tuple.sp == tcph->source \
		&& sa_entry->tuple.dp == tcph->dest){
					ismatched=1;
					get_sa_entry(sa_entry);
					break;
		}
	}	
	read_unlock(&(hash_table->sa_rwlock));
	
	/*check the squeue num of the tcph*/
	
	if(0 == ismatched){
		g_sactl.stat.lost_num++;
		NAT_SA_TRACE(("%s:590: no entry matched!\n", __FUNCTION__));
		return 0;
	}

	g_sactl.stat.hit_num++;
	NAT_SA_TRACE(("%s:591:sa_entry = %p hit, action = %d\n", __FUNCTION__, sa_entry, sa_entry->action));

	/*subtract age from jiffies,check the result,if it is bigger than 15 HZ,put the skb to its pool 
	or update the jiffies in sa_entry_add*/
	if(likely(time_before(jiffies, (sa_entry->age+g_timeout*HZ)))){
		NAT_SA_DBG(("%s:600:put entry to skb pool\n", __FUNCTION__));	

		skb_dup = skb;
		/* return 1 to bypass the tcp/ip stack */
		ret = 1;
	}
	else {
		NAT_SA_TRACE(("%s:800:refesh the sa_entry age every 10s\n", __FUNCTION__));
		sa_entry->age = jiffies;
		/* clone the skb to send from the softnat */
		#if 1
		skb_dup = pskb_copy(skb, GFP_ATOMIC);
		if (NULL == skb_dup) {
			printk(KERN_ALERT "====skb copy error\n");
			return 0;
		}	
		#endif
		
		
		NAT_SA_DBG(("===cloned skb, iph id = %d,len=%d, from %pI4:%u->%pI4:%u \n",\
		ntohs(iph->id),ntohs(iph->tot_len),&(iph->saddr),ntohs(tcph->source),\
					&(iph->daddr),ntohs(tcph->dest)));

		/* return 0 to make the original skb traverse the tcp/ip stack */
		ret = 0;		
	}
	*((struct sa_fw_entry **)(skb_dup->cb)) = sa_entry;
	//memcpy(&skb_dup->cb[0], &sa_entry, 4);			
		
	//NAT_SA_INFO(("%s:700:enter skb_queue_tail\n", __FUNCTION__));

	pri_index = (sa_entry->action)<<1;

	if (skb_dup->len > 128)
		pri_index++;

	/* place the skb in four queues 
	 * 00 up-short; 01 up-long; 10 down-short; 11 down-long	 
	 */
	skb_queue_tail(&(g_sactl.sa_pqueue[pri_index].sa_pending_txqueue),skb_dup);
	tasklet_schedule(&g_sa_task);

	return ret;
}
EXPORT_SYMBOL_GPL(sa_table_match);


static int
tcp_manip_pkt(struct sk_buff *skb,
	      unsigned int iphdroff,
	      struct sa_fw_entry *manip_entry)
{
	struct iphdr *iph;
	struct tcphdr *hdr;
	enum nat_sa_manip_type maniptype;
	uint32 oldip;
	uint32 newip;
	uint16 oldport;
	uint16 newport;
	
	//unsigned int hdroff;
	unsigned int hdroff = iphdroff + 20;
	
	int hdrsize = 8; /* TCP connection tracking guarantees this much */

	NAT_SA_DBG(("%s: 100: enter\n", __FUNCTION__));
	#if 0
	if (!skb_make_writable(skb, hdroff + hdrsize))
	return 0;
	#endif
	/* this could be a inner header returned in icmp packet; in such
	   cases we cannot update the checksum field since it is outside of
	   the 8 bytes of transport layer headers we are guaranteed */
	if (skb->len >= hdroff + sizeof(struct tcphdr))
		hdrsize = sizeof(struct tcphdr);



	//NAT_SA_INFO(("%s: 200: get iph and tcph\n", __FUNCTION__));
	iph = (struct iphdr *)(skb->data + iphdroff);
	
	hdr = (struct tcphdr *)(skb->data + hdroff);

	maniptype = manip_entry->action;
	
	if (maniptype == NAT_SA_MANIP_SRC) {
		/* Get rid of src ip and src pt */
		//iph->saddr = manip_entry->nat.sip;
		oldip = iph->saddr;
		newip = manip_entry->nat.ip;
		oldport = hdr->source;
		hdr->source = manip_entry->nat.port;
		newport = hdr->source;
				
	} else {
		/* Get rid of dst ip and dst pt */
		//iph->daddr = manip_entry->nat.dip;
		oldip = iph->daddr;
		newip = manip_entry->nat.ip;
		oldport = hdr->dest;
		hdr->dest = manip_entry->nat.port;
		newport = hdr->dest;
	}

	#if 0
	if (hdrsize < sizeof(*hdr)){
		return 1;
	}
	#endif
	inet_proto_csum_replace4(&hdr->check, skb, oldip, newip, 1);
	inet_proto_csum_replace2(&hdr->check, skb, oldport, newport, 0);
	return 1;
}

static int
udp_manip_pkt(struct sk_buff *skb,
	      unsigned int iphdroff,
	      struct sa_fw_entry *manip_entry)
{
	
	struct iphdr *iph = (struct iphdr *)(skb->data + iphdroff);
	struct udphdr *hdr;
	//unsigned int hdroff = iphdroff + iph->ihl*4; //no ip option will be handled
	unsigned int hdroff = iphdroff + 20;
	enum nat_sa_manip_type maniptype;
	
	NAT_SA_DBG(("%s: 100: enter\n", __FUNCTION__));
	#if 0
	if (!skb_make_writable(skb, hdroff + sizeof(*hdr)))
		return 0;
	#endif

	//NAT_SA_INFO(("%s: 200: get iph and udph\n", __FUNCTION__));
	iph = (struct iphdr *)(skb->data + iphdroff);
	hdr = (struct udphdr *)(skb->data + hdroff);

		
	maniptype = manip_entry->action;
	
	if (maniptype == NAT_SA_MANIP_SRC) {
		/* Get rid of src ip and src pt */
		//iph->saddr = manip_entry->nat.ip;
		hdr->source = manip_entry->nat.port;
				
	} else {
		/* Get rid of dst ip and dst pt */
		//iph->daddr = manip_entry->nat.ip;
		hdr->dest = manip_entry->nat.port;
	}

	hdr->check = 0;
	
	return 1;
}

static int sa_manip_pkt(struct sk_buff *skb,
	  unsigned int iphdroff,
	  struct sa_fw_entry *manip_entry)
{	
	struct iphdr *iph;
	struct ethhdr *out_eth;
	enum nat_sa_manip_type maniptype;
	
	NAT_SA_DBG(("%s: 100: enter\n", __FUNCTION__));

	if (!skb_make_writable(skb, iphdroff + sizeof(*iph)))
	{
		return 0;
	}
	
	iph = (struct iphdr *)(skb->data + iphdroff);

	//already checked
	#if 0
	if(iph->ttl <= 1)
	{
		return 0;		
	}
	#endif
	
	ip_decrease_ttl(iph);


	switch(iph->protocol){
		case IPPROTO_TCP:
			nat_manip_pkt = tcp_manip_pkt;
			NAT_SA_DBG(("%s: 200: tcp protocol\n", __FUNCTION__));
			break;
		case IPPROTO_UDP:
			nat_manip_pkt = udp_manip_pkt;
			NAT_SA_DBG(("%s: 300: udp protocol\n", __FUNCTION__));
			break;
		default:
			NAT_SA_DBG(("%s: 400: protocol not support\n", __FUNCTION__));
			return 0;
		}

	NS_printbyte(skb->data - 14, 34);
	
	#if 0
	{
		
		int i;
		char *ctmp1 = skb->data - 14;		
		NAT_SA_DBG(("%s: 450: skb head\n", __FUNCTION__));
		
		for(i = 0; i < 34; i++)
		{
	
				NAT_SA_DBG(("%02x ", ctmp1[i]));				
		}
	
		NAT_SA_DBG(("\n"));				
	}	
	#endif

	if (nat_manip_pkt(skb, iphdroff, manip_entry) == 0){
		NAT_SA_ERROR(("%s: 500: nat_manip error!\n", __FUNCTION__));
		return 0;
		}
	#if 0
	if (maniptype == IP_NAT_MANIP_SRC) {
		iph->saddr = ((struct sa_fw_entry *)(skb->cb[0]))->tuple.sip;
	} else {
		iph->daddr = ((struct sa_fw_entry *)(skb->cb[0]))->tuple.dip;
	}
	#endif

	maniptype = manip_entry->action;

	if (maniptype == NAT_SA_MANIP_SRC) {
		NAT_SA_DBG(("%s: 600:nat_sa_manip_src checksum !\n", __FUNCTION__));
		csum_replace4(&iph->check, iph->saddr, manip_entry->nat.ip);
		iph->saddr = manip_entry->nat.ip;
	} else {
		NAT_SA_DBG(("%s: 700:nat_sa_manip_drc checksum !\n", __FUNCTION__));
		csum_replace4(&iph->check, iph->daddr, manip_entry->nat.ip);		
		iph->daddr = manip_entry->nat.ip;
	}


	skb_push(skb, 14);
	out_eth = eth_hdr(skb);
	out_eth->h_proto = htons(0x0800);
	skb->dev = manip_entry->output_dev;

	/* if(maniptype == NAT_SA_MANIP_DST) */
	{
		/* no need to fill the ether header for upstream */
		memcpy(out_eth->h_dest,manip_entry->dhost.octet, ETH_ALEN);
		memcpy(out_eth->h_source,manip_entry->shost.octet, ETH_ALEN);
	}
	
	NS_printbyte(skb->data, 42);
		
	return 1;	
}

/* packet forwarding and manipulation func */
/**/
void sa_task_func(unsigned long data)
{	
	struct sk_buff *skb;
	int ret;
	struct sa_fw_entry *manip_entry;
	int queue_num;
	struct sa_pending_queue *cur_pq;
	int pkt_num = 300; /* dispose 300 packets at most once a time */
	static const unsigned pri_seq[MAX_NET_DEVICE*2] = {3,0,1,2};

	int loop_num;           /*limit the loop num of each queue*/

	
	NAT_SA_DBG(("%s: 100: enter\n", __FUNCTION__));
	
	for (queue_num=0; queue_num<(MAX_NET_DEVICE*2); queue_num++) {

		loop_num = 0;
				
		/* dispose the queue according to the priority seq */
		cur_pq = &g_sactl.sa_pqueue[(pri_seq[queue_num])];

		/* cur_pq couldn't be NULL */
		BUG_ON(!cur_pq);
		
		while (!netif_queue_stopped(cur_pq->sa_pending_netdev)) {
			/* deal with the corresponding pending queue */
			skb = skb_dequeue(&(cur_pq->sa_pending_txqueue));
			
			/* no more skb in the current pengding queue */
			if (NULL == skb)
				break;
			
			NAT_SA_DBG(("%s: 200:skb queue = %d ,queue length = %d \n", __FUNCTION__,
							pri_seq[queue_num],cur_pq->sa_pending_txqueue.qlen));

			NS_printbyte(skb->data - 14, 34);
			
			manip_entry = *((struct sa_fw_entry **)(skb->cb));

			NAT_SA_DBG(("%s: 300:enter nat \n", __FUNCTION__));
			
			ret = sa_manip_pkt(skb,0,manip_entry);

			put_sa_entry(manip_entry);

			if(ret == NSA_FAIL)	{
				//NAT_SA_ERROR(("%s: 400:nat failed!!! \n", __FUNCTION__));
				printk("%s: 400:nat failed!!! \n", __FUNCTION__);
				kfree_skb(skb);
				continue;
			}

			/* transmit this skb */
			skb->dev->netdev_ops->ndo_start_xmit(skb, skb->dev);

			#if 0
			tx_num[(pri_seq[queue_num])]++;
			#endif

				
			if(loop_num++ > g_loop_limit){
				NAT_SA_DBG(("%s: 500:next queue is ready to transmite \n", __FUNCTION__));
				break;
			}
			
			
			/* time is used up */
			if (pkt_num-- <0) {
				NAT_SA_INFO(( "%s: 600:time is used up \n", __FUNCTION__));
				return ;
			}				
		}	
	}	
}

static struct notifier_block nat_sa_notifier = {
	.notifier_call = nat_sa_device_event,
};


/* module initialization */
int __init nat_sa_init(void)
{
 	unsigned int i;
	struct sa_htable *htbysource;
	int netdev_num;
	int ret;

	printk(KERN_ALERT "NAT_SA_MODULE:%s\n",sa_info);
	
	/* initialize the statistics*/
	memset((void *)&g_sactl.stat, 0 , sizeof(struct sa_stat));
	
	// set the default hash table size, should de modified by mmi such as sys, proc, ioctl
	/* the nat_sa_tablesize should be 2^n */
	if( 0 == nat_sa_tablesize )
		nat_sa_tablesize=256;
	
	// one vmalloc for nat entry hash table
	g_HTBySource = (struct sa_htable *)kmalloc (sizeof(struct sa_htable) * nat_sa_tablesize,GFP_KERNEL);

    if(g_HTBySource == NULL)
    {
        goto nat_sa_init_fail1;
    }

	
	sa_entry_slab = kmem_cache_create("sa_entry",
					     sizeof(struct sa_fw_entry),
					     0, SLAB_HWCACHE_ALIGN, NULL);
	if (sa_entry_slab == NULL){
		NAT_SA_ERROR(("%s: 230: creat the sa_entry_slab error!\n", __FUNCTION__));
		goto nat_sa_init_fail2;
	}

	NAT_SA_INFO(("%s: 400: init the htable!\n", __FUNCTION__));

	htbysource = g_HTBySource;
	NAT_SA_DBG(("%s: 500: g_HTBySource=0x%x\n", __FUNCTION__,g_HTBySource));
	
	for(i = 0; i < nat_sa_tablesize; i++)
	{
		INIT_HLIST_HEAD(&htbysource->sa_head);
		rwlock_init(&(htbysource->sa_rwlock));
		htbysource->hlist_count=0;
		htbysource = htbysource + 1;
		
	}	
	NAT_SA_INFO(("%s: 600:init the htable succeed!\n", __FUNCTION__));
	
	/* initial the pending queue's netdevice porinter */
	for( netdev_num=0; netdev_num < MAX_NET_DEVICE; netdev_num++ ) {
		g_sactl.sa_pqueue[netdev_num].sa_pending_netdev = NULL;
	}
	

	/* inital the task */
	NAT_SA_INFO(("%s: 700:init the tasklet!\n", __FUNCTION__));
	tasklet_init(&(g_sa_task),sa_task_func,(unsigned long)(&g_sactl));
    
	ret = register_netdevice_notifier(&nat_sa_notifier);

    if(ret)
    {goto nat_sa_init_fail3;}
    
	// set the nat entry update func
	g_pNatEntryUpdate = sa_entry_add;
	g_pNatEntryDelete = sa_entry_del;

	// set the global var to use in the func @ dev.c
	sa_match_func = sa_table_match;

	g_sactl.sa_table = g_HTBySource;
	
	g_sactl.flags = NAT_SA_DISABLE;

	sa_enable = g_sactl.flags;
		
	return 0;

nat_sa_init_fail3:

    kmem_cache_destroy(sa_entry_slab);

nat_sa_init_fail2:

    kfree(g_HTBySource);

nat_sa_init_fail1:
    
	return -1;


	
}

int __exit nat_sa_exit(void)
{
	unsigned int i = 0;
	struct hlist_head *htable_head;
	struct sa_htable *htbysource;
	NAT_SA_INFO(("%s: 100: enter\n", __FUNCTION__));

	sa_enable = NAT_SA_DISABLE;
	g_sactl.flags =  NAT_SA_DISABLE;	
	// unset the nat entry update func

	sa_match_func = NULL;
	g_pNatEntryUpdate = NULL;
	g_pNatEntryDelete = NULL;	

	//NAT_SA_INFO(("%s: 200: free the hash table entries  \n", __FUNCTION__));

	htbysource = g_HTBySource;
		
	for(i = 0; i < nat_sa_tablesize; i++){
		//NAT_SA_INFO(("%s: 700: enter for loop,i=%d\n", __FUNCTION__,i));
		write_lock(&htbysource->sa_rwlock);
		htable_head = &htbysource->sa_head;
		NAT_SA_INFO(("%s: 200: point to htable_node, htable_node=0x%x\n", __FUNCTION__,(unsigned long)htable_head));
		while(!hlist_empty(htable_head)){
			NAT_SA_INFO(("%s: 300: enter while htable_node\n", __FUNCTION__));
			hlist_del(htable_head->first);
			NAT_SA_INFO(("%s: 400: end delete the htable node\n", __FUNCTION__));
		}
	//list_del(&g_sactl.sa_table->sa_head);
		write_unlock(&htbysource->sa_rwlock);
		htbysource = htbysource + 1;
	}
	
	NAT_SA_INFO(("%s: 500: free the sa_htable_slab \n", __FUNCTION__));

	kfree(g_HTBySource);
	
	NAT_SA_INFO(("%s: 700: destory the sa_entry_slab \n", __FUNCTION__));
	if (sa_entry_slab != NULL) {
		kmem_cache_destroy(sa_entry_slab);
		sa_entry_slab = NULL;
	}
	
	//NAT_SA_INFO(("%s: 600: free the sk_buff \n", __FUNCTION__));

	/*free the pending queue*/
	for (i = 0; i < (MAX_NET_DEVICE*2); i++) {
		skb_queue_purge(&g_sactl.sa_pqueue[i].sa_pending_txqueue);
	}
	
	unregister_netdevice_notifier(&nat_sa_notifier);
	NAT_SA_INFO(("%s: 800: free the hash table and all the entries succeed!\n", __FUNCTION__));

	return 0;
}


module_init(nat_sa_init);
module_exit(nat_sa_exit);



#if 0
#ifdef CONFIG_SYSCTL
struct ctl_path nat_soft_sp_sysctl_path[] = {
	{ .procname = "XXX", },
	{ .procname = "XXX", },
	{ }
};
EXPORT_SYMBOL(nat_soft_sp_sysctl_path);
#endif /* CONFIG_SYSCTL */
#endif


