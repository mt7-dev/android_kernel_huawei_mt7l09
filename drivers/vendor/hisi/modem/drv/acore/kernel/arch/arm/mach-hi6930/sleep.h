
#ifndef __BALONG_SLEEP_H
#define __BALONG_SLEEP_H
#include <linux/linkage.h>
#include <linux/init.h>
#include <asm/pgtable.h>

#define _asm_function_called_from_c(a) \
    .globl a ;\
    .code  32 ;\
    .balign 4 ;\
a:


#endif

