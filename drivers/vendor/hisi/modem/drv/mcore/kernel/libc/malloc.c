#include <libc.h>
#include <osl_common.h>

void printk(const char *fmt, ...);

extern unsigned int __alloc_start__;
extern unsigned int __alloc_end__;

static unsigned int __alloc_start = (unsigned) &__alloc_start__;
static unsigned int __alloc_end = (unsigned) &__alloc_end__;

void *malloc(unsigned int sz)
{
	void *ptr;

	if(__alloc_start == 0)
	{
		printk("malloc __alloc_start error\n");
	}
	ptr = (void*) __alloc_start;
	__alloc_start = (__alloc_start + sz + 31) & (~31);
	if(__alloc_start > __alloc_end)
	{
		printk("malloc error no memory\n");
	}

    return ptr;
}

void free(void * addr)
{
	addr = addr;
}


