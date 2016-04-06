#ifndef __LINUX_NETLINK_H
#define __LINUX_NETLINK_H

#include <linux/socket.h> /* for __kernel_sa_family_t */
#include <linux/types.h>

#define NETLINK_ROUTE		0	/* Routing/device hook				*/
#define NETLINK_UNUSED		1	/* Unused number				*/
#define NETLINK_USERSOCK	2	/* Reserved for user mode socket protocols 	*/
#define NETLINK_FIREWALL	3	/* Firewalling hook				*/
#define NETLINK_SOCK_DIAG	4	/* socket monitoring				*/
#define NETLINK_NFLOG		5	/* netfilter/iptables ULOG */
#define NETLINK_XFRM		6	/* ipsec */
#define NETLINK_SELINUX		7	/* SELinux event notifications */
#define NETLINK_ISCSI		8	/* Open-iSCSI */
#define NETLINK_AUDIT		9	/* auditing */
#define NETLINK_FIB_LOOKUP	10	
#define NETLINK_CONNECTOR	11
#define NETLINK_NETFILTER	12	/* netfilter subsystem */
#define NETLINK_IP6_FW		13
#define NETLINK_DNRTMSG		14	/* DECnet routing messages */
#define NETLINK_KOBJECT_UEVENT	15	/* Kernel messages to userspace */
#define NETLINK_GENERIC		16
/* leave room for NETLINK_DM (DM Events) */
#define NETLINK_SCSITRANSPORT	18	/* SCSI Transports */
#define NETLINK_ECRYPTFS	19
#define NETLINK_RDMA		20
#define NETLINK_CRYPTO		21	/* Crypto layer */

#define NETLINK_HW_LOGCAT   29
#define NETLINK_DEVICE_EVENT	30

#define NETLINK_USER_WIFI_EVENT	 31

#define NETLINK_INET_DIAG	NETLINK_SOCK_DIAG

#define MAX_LINKS 32		

struct sockaddr_nl {
	__kernel_sa_family_t	nl_family;	/* AF_NETLINK	*/
	unsigned short	nl_pad;		/* zero		*/
	__u32		nl_pid;		/* port ID	*/
       	__u32		nl_groups;	/* multicast groups mask */
};

struct nlmsghdr {
	__u32		nlmsg_len;	/* Length of message including header */
	__u16		nlmsg_type;	/* Message content */
	__u16		nlmsg_flags;	/* Additional flags */
	__u32		nlmsg_seq;	/* Sequence number */
	__u32		nlmsg_pid;	/* Sending process port ID */
};

/* Flags values */

#define NLM_F_REQUEST		1	/* It is request message. 	*/
#define NLM_F_MULTI		2	/* Multipart message, terminated by NLMSG_DONE */
#define NLM_F_ACK		4	/* Reply with ack, with zero or error code */
#define NLM_F_ECHO		8	/* Echo this request 		*/
#define NLM_F_DUMP_INTR		16	/* Dump was inconsistent due to sequence change */

/* Modifiers to GET request */
#define NLM_F_ROOT	0x100	/* specify tree	root	*/
#define NLM_F_MATCH	0x200	/* return all matching	*/
#define NLM_F_ATOMIC	0x400	/* atomic GET		*/
#define NLM_F_DUMP	(NLM_F_ROOT|NLM_F_MATCH)

/* Modifiers to NEW request */
#define NLM_F_REPLACE	0x100	/* Override existing		*/
#define NLM_F_EXCL	0x200	/* Do not touch, if it exists	*/
#define NLM_F_CREATE	0x400	/* Create, if it does not exist	*/
#define NLM_F_APPEND	0x800	/* Add to end of list		*/

/*
   4.4BSD ADD		NLM_F_CREATE|NLM_F_EXCL
   4.4BSD CHANGE	NLM_F_REPLACE

   True CHANGE		NLM_F_CREATE|NLM_F_REPLACE
   Append		NLM_F_CREATE
   Check		NLM_F_EXCL
 */

#define NLMSG_ALIGNTO	4U
#define NLMSG_ALIGN(len) ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_HDRLEN	 ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) ((len)+NLMSG_ALIGN(NLMSG_HDRLEN))
#define NLMSG_SPACE(len) NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)  ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)	 ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
				  (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_NOOP		0x1	/* Nothing.		*/
#define NLMSG_ERROR		0x2	/* Error		*/
#define NLMSG_DONE		0x3	/* End of a dump	*/
#define NLMSG_OVERRUN		0x4	/* Data lost		*/

#define NLMSG_MIN_TYPE		0x10	/* < 0x10: reserved control messages */

struct nlmsgerr {
	int		error;
	struct nlmsghdr msg;
};

#define NETLINK_ADD_MEMBERSHIP	1
#define NETLINK_DROP_MEMBERSHIP	2
#define NETLINK_PKTINFO		3
#define NETLINK_BROADCAST_ERROR	4
#define NETLINK_NO_ENOBUFS	5

struct nl_pktinfo {
	__u32	group;
};

#define NET_MAJOR 36		/* Major 36 is reserved for networking 						*/

enum {
	NETLINK_UNCONNECTED = 0,
	NETLINK_CONNECTED,
};

/*
 *  <------- NLA_HDRLEN ------> <-- NLA_ALIGN(payload)-->
 * +---------------------+- - -+- - - - - - - - - -+- - -+
 * |        Header       | Pad |     Payload       | Pad |
 * |   (struct nlattr)   | ing |                   | ing |
 * +---------------------+- - -+- - - - - - - - - -+- - -+
 *  <-------------- nlattr->nla_len -------------->
 */

struct nlattr {
	__u16           nla_len;
	__u16           nla_type;
};

/*
 * nla_type (16 bits)
 * +---+---+-------------------------------+
 * | N | O | Attribute Type                |
 * +---+---+-------------------------------+
 * N := Carries nested attributes
 * O := Payload stored in network byte order
 *
 * Note: The N and O flag are mutually exclusive.
 */
#define NLA_F_NESTED		(1 << 15)
#define NLA_F_NET_BYTEORDER	(1 << 14)
#define NLA_TYPE_MASK		~(NLA_F_NESTED | NLA_F_NET_BYTEORDER)

#define NLA_ALIGNTO		4
#define NLA_ALIGN(len)		(((len) + NLA_ALIGNTO - 1) & ~(NLA_ALIGNTO - 1))
#define NLA_HDRLEN		((int) NLA_ALIGN(sizeof(struct nlattr)))

#ifdef __KERNEL__

#include <linux/capability.h>
#include <linux/skbuff.h>

struct net;

static inline struct nlmsghdr *nlmsg_hdr(const struct sk_buff *skb)
{
	return (struct nlmsghdr *)skb->data;
}

struct netlink_skb_parms {
	struct ucred		creds;		/* Skb credentials	*/
	__u32			pid;
	__u32			dst_group;
};

#define NETLINK_CB(skb)		(*(struct netlink_skb_parms*)&((skb)->cb))
#define NETLINK_CREDS(skb)	(&NETLINK_CB((skb)).creds)


extern void netlink_table_grab(void);
extern void netlink_table_ungrab(void);

extern struct sock *netlink_kernel_create(struct net *net,
					  int unit,unsigned int groups,
					  void (*input)(struct sk_buff *skb),
					  struct mutex *cb_mutex,
					  struct module *module);
extern void netlink_kernel_release(struct sock *sk);
extern int __netlink_change_ngroups(struct sock *sk, unsigned int groups);
extern int netlink_change_ngroups(struct sock *sk, unsigned int groups);
extern void __netlink_clear_multicast_users(struct sock *sk, unsigned int group);
extern void netlink_clear_multicast_users(struct sock *sk, unsigned int group);
extern void netlink_ack(struct sk_buff *in_skb, struct nlmsghdr *nlh, int err);
extern int netlink_has_listeners(struct sock *sk, unsigned int group);
extern int netlink_unicast(struct sock *ssk, struct sk_buff *skb, __u32 pid, int nonblock);
extern int netlink_broadcast(struct sock *ssk, struct sk_buff *skb, __u32 pid,
			     __u32 group, gfp_t allocation);
extern int netlink_broadcast_filtered(struct sock *ssk, struct sk_buff *skb,
	__u32 pid, __u32 group, gfp_t allocation,
	int (*filter)(struct sock *dsk, struct sk_buff *skb, void *data),
	void *filter_data);
extern int netlink_set_err(struct sock *ssk, __u32 pid, __u32 group, int code);
extern int netlink_register_notifier(struct notifier_block *nb);
extern int netlink_unregister_notifier(struct notifier_block *nb);

/* finegrained unicast helpers: */
struct sock *netlink_getsockbyfilp(struct file *filp);
int netlink_attachskb(struct sock *sk, struct sk_buff *skb,
		      long *timeo, struct sock *ssk);
void netlink_detachskb(struct sock *sk, struct sk_buff *skb);
int netlink_sendskb(struct sock *sk, struct sk_buff *skb);

/*
 *	skb should fit one page. This choice is good for headerless malloc.
 *	But we should limit to 8K so that userspace does not have to
 *	use enormous buffer sizes on recvmsg() calls just to avoid
 *	MSG_TRUNC when PAGE_SIZE is very large.
 */
#if PAGE_SIZE < 8192UL
#define NLMSG_GOODSIZE	SKB_WITH_OVERHEAD(PAGE_SIZE)
#else
#define NLMSG_GOODSIZE	SKB_WITH_OVERHEAD(8192UL)
#endif

#define NLMSG_DEFAULT_SIZE (NLMSG_GOODSIZE - NLMSG_HDRLEN)


struct netlink_callback {
	struct sk_buff		*skb;
	const struct nlmsghdr	*nlh;
	int			(*dump)(struct sk_buff * skb,
					struct netlink_callback *cb);
	int			(*done)(struct netlink_callback *cb);
	void			*data;
	u16			family;
	u16			min_dump_alloc;
	unsigned int		prev_seq, seq;
	long			args[6];
};

struct netlink_notify {
	struct net *net;
	int pid;
	int protocol;
};

struct nlmsghdr *
__nlmsg_put(struct sk_buff *skb, u32 pid, u32 seq, int type, int len, int flags);

#define NLMSG_NEW(skb, pid, seq, type, len, flags) \
({	if (unlikely(skb_tailroom(skb) < (int)NLMSG_SPACE(len))) \
		goto nlmsg_failure; \
	__nlmsg_put(skb, pid, seq, type, len, flags); })

#define NLMSG_PUT(skb, pid, seq, type, len) \
	NLMSG_NEW(skb, pid, seq, type, len, 0)

struct netlink_dump_control {
	int (*dump)(struct sk_buff *skb, struct netlink_callback *);
	int (*done)(struct netlink_callback*);
	void *data;
	u16 min_dump_alloc;
};

extern int netlink_dump_start(struct sock *ssk, struct sk_buff *skb,
			      const struct nlmsghdr *nlh,
			      struct netlink_dump_control *control);


#define NL_NONROOT_RECV 0x1
#define NL_NONROOT_SEND 0x2
extern void netlink_set_nonroot(int protocol, unsigned flag);


typedef enum _DEVICE_ID
{
	DEVICE_ID_NULL_ID = 0,	  /*NULL_ID, as initialize value.*/ 
	DEVICE_ID_USB = 1,	// USB device = 1
	DEVICE_ID_KEY,		// Keyboard device = 2 
	DEVICE_ID_BATTERY,	// Battery device = 3
	DEVICE_ID_CHARGER,	// Charger device = 4
	DEVICE_ID_SD,		// SD card device =  5
	DEVICE_ID_PROCESS,	// Used for monitored process = 6
	DEVICE_ID_SCREEN,	// Screen device = 7
	DEVICE_ID_WLAN,	// WLAN device = 8
	DEVICE_ID_OM,	// OM module = 9
	DEVICE_ID_TEMP,	// Temperature = 10
	DEVICE_ID_WAN,	// WAN module = 11
	DEVICE_ID_SIM,	//SIM device = 12
	DEVICE_ID_UPDATEONLINE,  //Used for update online ;Add fansaihua
	DEVICE_ID_MAX
} DEVICE_ID;

typedef enum _USB_EVENT
{
	USB_ATTACH = 0,
	USB_REMOVE,
	USB_ENABLE,
	USB_DISABLE,
	USB_INIT,
	USB_HALT,
	USB_RESET,
	USB_SUSPEND,
	USB_RESUME,
	USB_EVENT_MAX
} USB_EVENT;

typedef enum _KEY_EVENT
{
	KEY_WAKEUP = 0,
	KEY_WLAN =1,
	KEY_WPS =2,
	KEY_FACTORY_RESTORE =3,
	KEY_POWER_OFF =4,
	KEY_UPDATE =5,
	KEY_SSID =6,
	KEY_WPS_PROMPT =7,
/*      FOR  SBM                         */
    KEY_POWER_SHORT =8,
    KEY_WPS_SHORT =9,
    KEY_RESET_SHORT =10,
    KEY_WPS_POWER =11,
    KEY_WPS_POWER_UP =12,
    KEY_POWER_ONE_SECOND =13,
    KEY_POWER_ONE_SECOND_UP =14,
	KEY_SHORT_POWER_OFF =17,
	KEY_SHORT_POWER_ON =18,
	KEY_EVENT_MAX
} KEY_EVENT;

typedef enum _BAT_EVENT
{
    BAT_CHARGING_ON_START = 0,/*在开机模式，开始充电**/
    BAT_CHARGING_OFF_START,	/*在关机模式，开始充电*/
    BAT_LOW_POWER, 	/*电池电量低时，上报此事件*/
    BAT_CHARGING_UP_STOP, 	/*开机停止充电时，上报此事件*/
    BAT_CHARGING_DOWN_STOP,	/*关机停止充电时，上报此事件*/
    BAT_EVENT_MAX  /*如果事件值不小于此值，则为非法事件*/
}BATTERY_EVENT;

typedef enum _CHARGER_EVENT
{
	CHARGER_ATTATCH = 0,
	CHARGER_REMOVE,
	CHARGER_EVENT_MAX
} CHARGER_EVENT;

typedef enum _SD_EVENT
{
	SD_ATTATCH = 0,
	SD_REMOVE,
	SD_EVENT_MAX
} SD_EVENT;

typedef enum _PROCESS_EVENT
{
	PROCESS_ADD_MONITORED = 0,
	PROCESS_RM_MONITORED,
	PROCESS_REPORT_EXIT,
	PROCESS_EVENT_MAX
} PROCESS_EVENT;

/*EVENT ID*/
typedef enum _UPDATEONLINE_EVENT
{
    /*APP_GET_UPDATE_ID ,             app ???ó  id */
    /*KERNEL_REPORT_UPDATE_ID,        kernel ???′  id */
    APP_REBOOT_REQUEST,             // app reboot ???ó
	UPDATEONLINE_EVENT_MAX
} UPDATEONLINE_EVENT;

// Define the device event structure (driver => APP)
typedef struct _DEVICE_EVENT
{
	DEVICE_ID device_id;
	int event_code;
	int len;
	char data[0];
} DEVICE_EVENT;

// Define the device event handler (APP => driver)
typedef int (*device_event_dispatch)(void *data, int len);

// Export interface to other module
extern int device_event_report(void *data, int len);
extern int device_event_handler_register(DEVICE_ID id, device_event_dispatch dispatcher);


#endif /* __KERNEL__ */

#endif	/* __LINUX_NETLINK_H */
