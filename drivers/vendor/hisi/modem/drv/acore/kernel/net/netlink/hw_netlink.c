/* 
 *
 *  Netlink adapter layer in kernel.
 *  Device drviers report event to user space through API of this layer.
 *
 */
 


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <net/sock.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

struct sock *device_event_sock;
u32 us_pid = (unsigned int)(-1);

struct delayed_work device_event_send_work;
struct workqueue_struct *netlink_wq = NULL; //事件上报用工作队列
#define DELAY_FOR_NL_SEND msecs_to_jiffies(1)

// Device event buffer, to avoid blocking by socket
struct list_head device_event_queue;
spinlock_t queue_lock;

struct device_event_node {
    struct list_head list;
    int len;
    char data[0];
}device_event_node;

/* Limit the event buffer queue length,
 * Otherwise, this queue may consuming out all of memory
 */
int event_queue_len = 0;
#define MAX_EVENT_QUEUE_LEN	(300)

/* Device callback
 * Used to dispatch message from userspace
 */
device_event_dispatch device_event_handler[DEVICE_ID_MAX] = {0};

/**
 *
 * Register call back
 *
 */
int device_event_handler_register(DEVICE_ID id, device_event_dispatch dispatcher)
{
	if (((int)id < 0) || (id >= DEVICE_ID_MAX) || (NULL == dispatcher)) {
		printk(KERN_ERR "device_event_handler_register: register info is not correct\n");
		return -1;
	}

	device_event_handler[id] = dispatcher;

	return 0;
}

EXPORT_SYMBOL_GPL(device_event_handler_register);

/**
 *
 * Construct a buffer node
 *
 */
static struct device_event_node *device_event_node_alloc(void * data, int len)
{
	struct device_event_node *node;
	unsigned long flags;

	spin_lock_irqsave(&queue_lock, flags);
	if (event_queue_len >= MAX_EVENT_QUEUE_LEN)
	{
		spin_unlock_irqrestore(&queue_lock, flags);
		return NULL;
	}
	spin_unlock_irqrestore(&queue_lock, flags);
    
	node = (struct device_event_node *)kmalloc((len + sizeof(struct device_event_node)), GFP_ATOMIC);
	if (NULL == node) {
		printk(KERN_ERR "device_event_node_alloc: kmalloc failed\n");
		return NULL;
	}

	node->len = len;
	memcpy(node->data, data, (unsigned int)len);

	spin_lock_irqsave(&queue_lock, flags);
	event_queue_len++;
	spin_unlock_irqrestore(&queue_lock, flags);

	return node;
}

/**
 *
 * Destruct a buffer node
 *
 */
static inline void device_event_node_free(struct device_event_node *node)
{

    event_queue_len--;

    kfree(node);
}

/**
 *
 * Put a device event into event buffer
 *
 */
static inline void device_event_enqueue(struct device_event_node *node)
{
	unsigned long flags;

	spin_lock_irqsave(&queue_lock, flags);
	list_add_tail(&node->list, &device_event_queue);
	spin_unlock_irqrestore(&queue_lock, flags);
}

/**
 *
 * Get a device event from event buffer
 * Notice: list should have at least one entry!
 *
 */
static inline struct device_event_node *device_event_dequeue(void)
{
    struct device_event_node *node = NULL;
    unsigned long flags;

    spin_lock_irqsave(&queue_lock, flags);
    node = list_first_entry(&device_event_queue, struct device_event_node, list);
    if (NULL == node)
    {
        spin_unlock_irqrestore(&queue_lock, flags);    
        return NULL;
    }

    list_del(&node->list);
    spin_unlock_irqrestore(&queue_lock, flags);

    return node;
}

/**
 *
 * Check whether the device event buffer queue is empty
 *
 */
static inline bool device_event_queue_empty(void)
{
	unsigned long flags;
	bool ret;

	spin_lock_irqsave(&queue_lock, flags);
	ret = list_empty(&device_event_queue);
	spin_unlock_irqrestore(&queue_lock, flags);

	return ret;
}

/**
 *
 * Process the message sent from user space 
 *
 */
static void device_event_input(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	DEVICE_EVENT *msg_body;
	device_event_dispatch dispatcher;

	printk("device_event_input entry\n");

	skb = skb_get(__skb);
	if (skb->len < NLMSG_SPACE(0))
	{
		return;
	}

	nlh = nlmsg_hdr(skb);

	if ((unsigned int)-1 == us_pid) {
		// Store the user space daemon pid
		us_pid = nlh->nlmsg_pid;

		// Send device events, reported before daemon start
		/*将工作任务加入到工作队列中*/
		queue_delayed_work(netlink_wq, &device_event_send_work, DELAY_FOR_NL_SEND);
	}

	// Dispatch message to other device driver ...
	msg_body = NLMSG_DATA(nlh);

	if ((msg_body->device_id < 0) || (msg_body->device_id >= DEVICE_ID_MAX)) {
		printk(KERN_ERR "device_event_input: device id is NOT correct\n");
		return;
	}

	dispatcher = device_event_handler[msg_body->device_id];
	if (likely(NULL != dispatcher))
		dispatcher(msg_body, msg_body->len + sizeof(DEVICE_EVENT));
	else
		printk(KERN_ERR "device_event_input: event handler is NOT registered\n");

	return;
}

/**
 *
 * Send message to user space 
 *
 */
static void device_event_send(struct work_struct *w)
{
	struct device_event_node *node = NULL;
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int size;
	int ret;

	while (!device_event_queue_empty())
	{
		printk("device_event_send: queue is NOT empty\n");

		// Get one entry from event buffer queue
		node = device_event_dequeue();
		if (NULL == node)
			break;

		/*
		 * node->len: size of (node->data), is event message length
		 * size: size of (skb) 
		 * size = sizeof(*nlh) + align pad + node->len + aligh pad
		 * 
		 * alloc skb here
		 * But, NOT release skb here
		 * 
		 */
		size = NLMSG_SPACE(node->len);
		skb = alloc_skb((unsigned int)size, GFP_ATOMIC);
		if (NULL == skb)
		{
			printk(KERN_ALERT "device_event_send: alloc skb failed\n");
			return;
		}

		// Use "size - sizeof(*nlh)" here (incluing align pads)
		nlh = nlmsg_put(skb, 0, 0, 0, size - sizeof(*nlh), 0);

		NETLINK_CB(skb).pid = 0;
		NETLINK_CB(skb).dst_group = 0;
		memcpy (NLMSG_DATA(nlh), node->data, (unsigned int)node->len);

		// Release this event entry
		device_event_node_free(node);

		// Send message to user space
		ret = netlink_unicast(device_event_sock, skb, us_pid, 0);
		if (ret < 0) {
			printk(KERN_ERR "device_event_send: msg send failed\n");
			kfree_skb(skb);
			return;
		}
    	
		printk(KERN_ALERT "device_event_send: msg send over\n");	
	}
}

/**
 *
 * Device driver use this interface to report an event 
 * NOTICE: This interface may be called in interrupt or in process context
 *
 */
int device_event_report(void *data, int len) 
{
	struct device_event_node *node = NULL;

	if (unlikely(NULL == device_event_sock)) {
		printk(KERN_ERR "device_event_report: sock is NULL\n");
		return -1;
	}

	node = device_event_node_alloc(data, len);
	if (NULL == node) {
		printk(KERN_ERR "device_event_report: alloc node failed\n");
        	return  -1;
	}

	device_event_enqueue(node);

	/*
	 * If device driver report event before daemon was started,
	 * Only put event into buffer queue.
	 */
	if (likely(-1 != us_pid))
	{
	    /*将工作任务加入到工作队列中*/
		queue_delayed_work(netlink_wq, &device_event_send_work, DELAY_FOR_NL_SEND);
	}
	return 0;		
}

EXPORT_SYMBOL_GPL(device_event_report);

// Testing code, to be deleted ...
#if 0

static int test_handler(void *data, int len)
{
	DEVICE_EVENT *event = (DEVICE_EVENT *)data;

	printk("*** test_handler: device_id = %d, event_code = %d, event_len = %d, data = %s\n", event->device_id, event->event_code, event->len, event->data);

	// Loop-back for APP test
	device_event_report(data, len);

	return 0;
}

#endif

static int __devinit device_event_init(void)
{

	/*创建netlink用工作队列*/
    netlink_wq = create_workqueue("NET_LINK_WQ");
    if( NULL == netlink_wq )
    {  
        printk(KERN_ALERT "kernel create NET_LINK_WQ workqueue failed.\n");
        return -EIO;
    }	

	INIT_LIST_HEAD(&device_event_queue);
	INIT_DELAYED_WORK(&device_event_send_work, device_event_send);
	spin_lock_init(&queue_lock);
	
	// Create a socket
	device_event_sock = netlink_kernel_create(&init_net, NETLINK_DEVICE_EVENT,
                                             32, device_event_input, NULL, THIS_MODULE);

	if (!device_event_sock)
	{
		printk(KERN_ALERT "kernel create netlink failed.\n");
		return -EIO;
	}

	//Testing Code. To be deleted ...
#if 0
	{
		int ret, size, i;
		DEVICE_EVENT *event;

		size = 100 + sizeof(DEVICE_EVENT);
		event = (DEVICE_EVENT *)kmalloc(size, GFP_ATOMIC);
		if (NULL == event)
			return 0;

		for (i = 0; i < 3; i++)
		{
			event->device_id = 123;
			event->event_code = 456;
			sprintf(event->data, "Haishen: Hello world(%d)\n", i);
			event->len = strlen(event->data) + 1;
		
			ret = device_event_report(event, size);
			if (-1 == ret) {
				printk(KERN_ERR "device_event_init: can't add event\n");
			}
		}

		device_event_handler_register(3, test_handler);
	}
#endif

    return 0;
}

static void __devexit device_event_exit(void)
{
	cancel_delayed_work_sync(&device_event_send_work);
	
	/*清空工作队列并释放资源*/
	flush_workqueue(netlink_wq);
	destroy_workqueue(netlink_wq);
	netlink_wq = NULL;
	
	netlink_kernel_release(device_event_sock);
}

subsys_initcall(device_event_init);
module_exit(device_event_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("haishen.zhang");
MODULE_DESCRIPTION("Netlink module for kernel<->userspace messge.");


