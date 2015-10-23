/*
 * msa.h
 *   microstate accounting.  See Documentation/MicrostateAccounting for info.
 *
 * Copyright (c) Peter Chubb 2005
 *  UNSW and National ICT Australia
 * Copyright (c) 2010 MontaVista Software, LLC
 *  Corey Minyard <minyard@mvista.com>, <minyard@acm.org>, <source@mvista.com>
 */

#ifndef _LINUX_MSA_H
#define _LINUX_MSA_H

#include <linux/types.h>

typedef uint32_t msa_time_t;

/*
 * Tracked states
 */
enum msa_thread_state {
    MSA_UNKNOWN = -1,
    MSA_ONCPU_USER,
    MSA_ONCPU_SYS,
    MSA_INTERRUPTIBLE_SLEEP,
    MSA_UNINTERRUPTIBLE_SLEEP,
    MSA_ONRUNQUEUE,
    MSA_ZOMBIE,
    MSA_STOPPED,
    MSA_INTERRUPTED,
    MSA_PAGING_SLEEP,
    MSA_FUTEX_SLEEP,
    MSA_POLL_SLEEP,

    MSA_NR_STATES /* Must be last */
};

/* Values for "which" in the msa syscall */
#define MSA_THREAD  0   /* Just the current thread */
#define MSA_CHILDREN    1   /* All dead and waited-for threads */
#define MSA_SELF    2   /* All threads in current process */
#define MSA_GET_NOW 3   /* Current MSA timer in the first value */

#define SLIENCE_MAX ~0UL
#define MOD_SUB(x, y, z)    (((x) + (z) - (y)) % (z))
#define CPUTIME_TYPE 3

#ifdef __KERNEL__

#include <linux/compiler.h> /* For __user */

extern long asmlinkage sys_msa(int ntimers, int which,
                   msa_time_t __user *timers);

/* Forward definition... */
struct task_struct;

#ifdef CONFIG_MICROSTATE_ACCT

#include <asm-generic/msa.h>

/*
 * Times are tracked for the current task in timers[], and for the
 * current task's children in child_timers[] (accumulated at wait()
 * time).  One of these structures is added to every struct task_struct.
 */
struct microstates {
    enum msa_thread_state cur_state;
    enum msa_thread_state next_state;
    msa_time_t last_change; /* When the last change happened */
    msa_time_t timers[MSA_NR_STATES];
    msa_time_t child_timers[MSA_NR_STATES];
};

/* Has to be a macro because microstates is part of task_struct */
#define msa_next_state(p, s) do { (p)->microstates.next_state = s; } while(0)
void msa_switch(struct task_struct *prev, struct task_struct *next);
void msa_cputime_account(struct task_struct *p, struct task_struct *idle);
void msa_update_parent(struct task_struct *parent, struct task_struct *this);
void msa_init(struct task_struct *p);
void msa_set_timer(struct task_struct *p, int state);
#if 0
void msa_start_irq(int irq);
void msa_continue_irq(int oldirq, int newirq);
void msa_irq_exit(int irq, int is_going_to_user);
asmlinkage void msa_kernel(void);
asmlinkage void msa_user(void);
#endif
void msa_start_softirq(int irq_id, struct task_struct *p);
void msa_softirq_exit(int irq_id , struct task_struct *p);
void msa_getcpu_idle(msa_time_t *total,msa_time_t *idle);

#else /* CONFIG_MICROSTATE_ACCT */

/*
 * Dummy functions to do nothing, for when MICROSTATE_ACCT is configured off.
 */
#define msa_next_state(p, s) do { } while (0)
static inline void msa_switch(struct task_struct *prev,
                  struct task_struct *next) { }
static inline void msa_cputime_account(struct task_struct *p,
                  struct task_struct *idle) { }
static inline void msa_update_parent(struct task_struct *parent,
                     struct task_struct *this) { }

static inline void msa_init(struct task_struct *p) { }
static inline void msa_set_timer(struct task_struct *p, int state) { }
static inline void msa_getcpu_idle(msa_time_t *total,msa_time_t *idle) { }
#if 0
static inline void msa_start_irq(int irq) { }
static inline void msa_continue_irq(int oldirq, int newirq) { }
#define msa_irq_exit(irq, is_going_to_user) irq_exit()
static inline void msa_kernel(void) { }
static inline void msa_user(void) { }
#endif

#endif /* CONFIG_MICROSTATE_ACCT */
#endif /* __KERNEL__ */

#endif /* _LINUX_MSA_H */