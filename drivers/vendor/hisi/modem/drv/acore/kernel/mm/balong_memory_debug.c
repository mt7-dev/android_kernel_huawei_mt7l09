/*
 *  This file is used to debug balong android driver memory management and
 *  It is free. Balong android usesslub mechanism to alloc memory
 */
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <asm/uaccess.h>

#define BALONG_DEBUG_BUF_SIZE 200*1024
#define BALONG_DEBUG_PAGE_SIZE 2097152*2
#define BALONG_DEBUG_NODE_SIZE 200*1024

#if BALONG_DEBUG_BUF_SIZE

//static DECLARE_MUTEX(mr_sem); // by xiechanglong
static DEFINE_SPINLOCK(mr_lock);
static char huge[BALONG_DEBUG_BUF_SIZE];
static int offset = 0;
static int enable = 1;

static char pagehuge[BALONG_DEBUG_PAGE_SIZE];
static int lpageoffset = 0;
static int pageenable = 1;

static char nodehuge[BALONG_DEBUG_NODE_SIZE];
static int nodeoffset = 0;
static int nodeenable = 1;


void balong_switch(unsigned long receive)
{			
	int i;	
	unsigned char j;
	
	for(i = 0; i < 8; i++)
	{
		j = ((receive >> (28 - 4*i))&0xf);		
			
		if((unsigned long)j >= 0x0 && (unsigned long)j <= 0x9)
			huge[offset++] = j + 0x30;
		else if((unsigned long)j >= 0xa && (unsigned long)j <= 0xf)
			huge[offset++] = j + 0x57;

	}
	return;
}


void balong_switch_page(unsigned long receive)
{			
	int i;	
	unsigned char j;
	
	for(i = 0; i < 8; i++)
	{
		j = ((receive >> (28 - 4*i))&0xf);		
			
		if((unsigned long)j >= 0x0 && (unsigned long)j <= 0x9)
			pagehuge[lpageoffset++] = j + 0x30;
		else if((unsigned long)j >= 0xa && (unsigned long)j <= 0xf)
			pagehuge[lpageoffset++] = j + 0x57;

	}
	return;
}


void balong_switch_node(unsigned long receive)
{			
	int i;	
	unsigned char j;
	
	for(i = 0; i < 8; i++)
	{
		j = ((receive >> (28 - 4*i))&0xf);		
			
		if((unsigned long)j >= 0x0 && (unsigned long)j <= 0x9)
			nodehuge[nodeoffset++] = j + 0x30;
		else if((unsigned long)j >= 0xa && (unsigned long)j <= 0xf)
			nodehuge[nodeoffset++] = j + 0x57;

	}
	return;
}


void balong_memory_debug_reset()
{
	unsigned long flags;

	printk("offset: %u\n", offset);
	printk("lpageoffset: %u\n", lpageoffset);
	printk("nodeoffset: %u\n", nodeoffset);

	spin_lock_irqsave(&mr_lock, flags);
	offset = 0;
	memset(huge, 0, BALONG_DEBUG_BUF_SIZE);
	enable = 1;

	lpageoffset = 0;
	memset(pagehuge, 0, BALONG_DEBUG_PAGE_SIZE);
	pageenable = 1;

	nodeoffset = 0;
	memset(nodehuge, 0, BALONG_DEBUG_NODE_SIZE);
	nodeenable = 1;	
	spin_unlock_irqrestore(&mr_lock, flags);

}
EXPORT_SYMBOL(balong_memory_debug_reset);


void balong_memory_debug_malloc_trace(unsigned long this_ip, void * ret, size_t alloc_size)
{
	unsigned long flags;
	
	if(1024*1024 < alloc_size)
	{
	    dump_stack();
	}
	if(enable){
		if (offset < BALONG_DEBUG_BUF_SIZE - 28){
			
			spin_lock_irqsave(&mr_lock, flags);
			
			huge[offset++] = 'M';
			huge[offset++] = ',';
			
			balong_switch(this_ip);		
			huge[offset++] = ',';

			balong_switch((unsigned long)ret);		
			huge[offset++] = ',';
			
			balong_switch(alloc_size);
			huge[offset++] = '\n';		

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============memory_debug Huge buffer is full============\n");
            enable = 0;
        }
	}
	return;
}

EXPORT_SYMBOL_GPL(balong_memory_debug_malloc_trace);


void balong_memory_debug_free_trace(const void * x)
{
	unsigned long flags;
	if(enable){
		if (offset < BALONG_DEBUG_BUF_SIZE - 11){			

			spin_lock_irqsave(&mr_lock, flags);

			huge[offset++] = 'F';
			huge[offset++] = ',';

			balong_switch((unsigned long)x);
			huge[offset++] = '\n';

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============memory_debug Huge buffer is full============\n");
            enable = 0;
        }
	}
	return;	
}

EXPORT_SYMBOL_GPL(balong_memory_debug_free_trace);


void balong_page_debug_malloc_trace(unsigned long this_ip, void * ret, size_t alloc_size)
{
	unsigned long flags;
	if(pageenable){
		if (lpageoffset < BALONG_DEBUG_PAGE_SIZE - 28){
			
			spin_lock_irqsave(&mr_lock, flags);

			pagehuge[lpageoffset++] = 'M';
			pagehuge[lpageoffset++] = ',';

			balong_switch_page(this_ip);		
			pagehuge[lpageoffset++] = ',';

			balong_switch_page((unsigned long)ret);		
			pagehuge[lpageoffset++] = ',';

			balong_switch_page(alloc_size);
			pagehuge[lpageoffset++] = '\n';		

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============page_debug Huge buffer is full============\n");
            pageenable = 0;
        }
	}
	return;
}

EXPORT_SYMBOL_GPL(balong_page_debug_malloc_trace);


void balong_page_debug_free_trace(const void * x)
{
	unsigned long flags;
	if(pageenable){
		if (lpageoffset < BALONG_DEBUG_PAGE_SIZE - 11){			

			spin_lock_irqsave(&mr_lock, flags);

			pagehuge[lpageoffset++] = 'F';
			pagehuge[lpageoffset++] = ',';

			balong_switch_page((unsigned long)x);
			pagehuge[lpageoffset++] = '\n';

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============page_debug Huge buffer is full============\n");
            pageenable = 0;
        }
	}
	return;	
}

EXPORT_SYMBOL_GPL(balong_page_debug_free_trace);


void balong_node_debug_malloc_trace(unsigned long this_ip, void * ret, size_t alloc_size)
{
	unsigned long flags;
	if(nodeenable){
		if (nodeoffset < BALONG_DEBUG_NODE_SIZE - 28){
			
			spin_lock_irqsave(&mr_lock, flags);
			
			nodehuge[nodeoffset++] = 'M';
			nodehuge[nodeoffset++] = ',';
			
			balong_switch_node(this_ip);		
			nodehuge[nodeoffset++] = ',';

			balong_switch_node((unsigned long)ret);		
			nodehuge[nodeoffset++] = ',';
			
			balong_switch_node(alloc_size);
			nodehuge[nodeoffset++] = '\n';		

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============node_debug Huge buffer is full============\n");
            nodeenable = 0;
        }
	}
	return;
}

EXPORT_SYMBOL_GPL(balong_node_debug_malloc_trace);


void balong_node_debug_free_trace(const void * x)
{
	unsigned long flags;
	if(nodeenable){
		if (offset < BALONG_DEBUG_NODE_SIZE - 11){			

			spin_lock_irqsave(&mr_lock, flags);

			nodehuge[nodeoffset++] = 'F';
			nodehuge[nodeoffset++] = ',';

			balong_switch_node((unsigned long)x);
			nodehuge[nodeoffset++] = '\n';

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============node_debug Huge buffer is full============\n");
            nodeenable = 0;
        }
	}
	return;	
}

EXPORT_SYMBOL_GPL(balong_node_debug_free_trace);


static int balong_memory_debug_open(struct inode *inode, 
		struct file *filp)
{
	return 0;
}


static ssize_t balong_memory_debug_write(struct file *file,
		const char __user *buf,
		size_t length, loff_t *ppos)
{
	if (buf[0] == '1'){
		enable = 1;
		printk ("Debug ==> enable:%d\n", enable);
	}
	else if (buf[0] == '0'){
		enable = 0;
		//memset(huge, 0, BALONG_DEBUG_BUF_SIZE);
		printk ("Debug ==> disable:%d\n", enable);
	}
	return length;
}


static ssize_t balong_memory_debug_read(struct file *file,
		char __user *buf,
		size_t length, loff_t *ppos)
{
	unsigned long len;

	if (*ppos >= offset)
		return 0;

	if (offset - *ppos < length)
		len = offset - *ppos;
	else 
		len = length;

	copy_to_user(buf, huge + *ppos, len);

	*ppos += len;

	return len;
}


static int balong_page_debug_open(struct inode *inode, 
		struct file *filp)
{
	return 0;
}


static ssize_t balong_page_debug_write(struct file *file,
		const char __user *buf,
		size_t length, loff_t *ppos)
{
	if (buf[0] == '1'){
		pageenable = 1;
		printk ("Debug ==> enable:%d\n", pageenable);
	}
	else if (buf[0] == '0'){
		pageenable = 0;
		//memset(huge, 0, BALONG_DEBUG_BUF_SIZE);
		printk ("Debug ==> disable:%d\n", pageenable);
	}
	return length;
}


static ssize_t balong_page_debug_read(struct file *file,
		char __user *buf,
		size_t length, loff_t *ppos)
{
	unsigned long len;

	if (*ppos >= lpageoffset)
		return 0;

	if (lpageoffset - *ppos < length)
		len = lpageoffset - *ppos;
	else 
		len = length;

	copy_to_user(buf, pagehuge + *ppos, len);

	*ppos += len;

	return len;
}


static int balong_node_debug_open(struct inode *inode, 
		struct file *filp)
{
	return 0;
}


static ssize_t balong_node_debug_write(struct file *file,
		const char __user *buf,
		size_t length, loff_t *ppos)
{
	if (buf[0] == '1'){
		nodeenable = 1;
		printk ("Debug ==> enable:%d\n", nodeenable);
	}
	else if (buf[0] == '0'){
		nodeenable = 0;
		//memset(huge, 0, BALONG_DEBUG_BUF_SIZE);
		printk ("Debug ==> disable:%d\n", nodeenable);
	}
	return length;
}


static ssize_t balong_node_debug_read(struct file *file,
		char __user *buf,
		size_t length, loff_t *ppos)
{
	unsigned long len;

	if (*ppos >= nodeoffset)
		return 0;

	if (nodeoffset - *ppos < length)
		len = nodeoffset - *ppos;
	else 
		len = length;

	copy_to_user(buf, nodehuge + *ppos, len);

	*ppos += len;

	return len;
}


static const struct file_operations proc_fops = {
	.owner		= THIS_MODULE,
	.open            = balong_memory_debug_open,
	.read		= balong_memory_debug_read,
	.write		= balong_memory_debug_write,	
};


static const struct file_operations pageproc_fops = {
	.owner		= THIS_MODULE,
	.open            = balong_page_debug_open,
	.read		= balong_page_debug_read,
	.write		= balong_page_debug_write,	
};


static const struct file_operations nodeproc_fops = {
	.owner		= THIS_MODULE,
	.open            = balong_node_debug_open,
	.read		= balong_node_debug_read,
	.write		= balong_node_debug_write,	
};


static struct proc_dir_entry *p;

int __init balong_memory_debug_init(void){
		
	printk ("*************Make balong_memory_debug proc successfully.*************\n");
	p = proc_create("balong_memory_debug", 0660, NULL, &proc_fops);

	if (!p) {
		printk ("Proc file fail.\n");
		return -1;
	}

	p = proc_create("balong_page_debug", 0660, NULL, &pageproc_fops);

	if (!p) {
		printk ("Proc file fail.\n");
		return -1;
	}

	p = proc_create("balong_node_debug", 0660, NULL, &nodeproc_fops);

	if (!p) {
		printk ("Proc file fail.\n");
		return -1;
	}	

	return 0;
}

void __exit balong_memory_debug_exit(void){
	remove_proc_entry("balong_memory_debug", NULL);
	remove_proc_entry("balong_page_debug", NULL);
	remove_proc_entry("balong_node_debug", NULL);
	return ;
}

module_init(balong_memory_debug_init);
module_exit(balong_memory_debug_exit);
MODULE_LICENSE("GPL");	

#endif


