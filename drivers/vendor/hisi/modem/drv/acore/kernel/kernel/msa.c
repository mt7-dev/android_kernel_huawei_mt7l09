/*
 * Microstate accounting.
 * Try to account for various states much more accurately than
 * the normal code does.
 *
 * Copyright (c) Peter Chubb 2005
 *  UNSW and National ICT Australia
 * Copyright (c) 2010 MontaVista Software, LLC
 *  Corey Minyard <minyard@mvista.com>, <minyard@acm.org>, <source@mvista.com>
 * This code is released under the Gnu Public Licence, version 2.
 */


#include <linux/types.h>
#include <linux/errno.h>
#include <linux/linkage.h>
#include <linux/msa.h>
#ifdef CONFIG_MICROSTATE_ACCT
#include <linux/irq.h>
#include <linux/hardirq.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/acct.h>
#include <linux/tsacct_kern.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/bug.h>
#include <linux/syscalls.h>

#include <asm/uaccess.h>
#include <linux/interrupt.h>

/*
 * Track time spend in interrupt handlers.
 */
struct msa_irq {
    msa_time_t times;
    msa_time_t last_entered;
};

/*
 * Time spent in interrupt handlers
 */
#if 0
static DEFINE_PER_CPU(struct msa_irq[NR_IRQS + 1], msa_irq);
#endif

struct msa_irq msa_softirq[NR_SOFTIRQS];
msa_time_t msa_cputime[CPUTIME_TYPE];

char *softirq_name[NR_SOFTIRQS] = {
    "HI", "TIMER", "NET_TX", "NET_RX", "BLOCK", "BLOCK_IOPOLL",
    "TASKLET", "SCHED", "HRTIMER",  "RCU"
};

char *cputime_name[CPUTIME_TYPE] = {
    "SYS", "USER", "IDLE"
};

void msa_getcpu_idle(msa_time_t *total,msa_time_t *idle)
{
    msa_time_t now;

    MSA_NOW(now);

    *total  = SLIENCE_MAX - now;
    *idle   = msa_cputime[2];
}

EXPORT_SYMBOL(msa_getcpu_idle);

void msa_cputime_account(struct task_struct *p, struct task_struct *idle)
{
    struct microstates *msp = &p->microstates;
    msa_time_t now, delta;

    MSA_NOW(now);

    delta = MOD_SUB(msp->last_change, now, SLIENCE_MAX);

    if(msp->cur_state == MSA_ONCPU_SYS)
    {
        msa_cputime[0] += delta;
    }

    if(p == idle)
    {
        if(msp->cur_state == MSA_ONCPU_SYS)
        {
            msa_cputime[2] = msp->timers[MSA_ONCPU_SYS] + delta;
        }
    }
}

/**
 * msa_switch: Update microstate timers when switching from one task to
 * another.
 *
 * @prev, @next:  The prev task is coming off the processor;
 *                the new task is about to run on the processor.
 *
 * Update the times in both prev and next.  It may be necessary to infer the
 * next state for each task.
 *
 */
void msa_switch(struct task_struct *prev, struct task_struct *next)
{
    struct microstates *prev_msp = &prev->microstates;
    struct microstates *next_msp = &next->microstates;
    msa_time_t now;
    enum msa_thread_state next_state;
    unsigned long flags;

    local_irq_save(flags);

    MSA_NOW(now);

    next_msp->timers[next_msp->cur_state] += MOD_SUB(next_msp->last_change, now, SLIENCE_MAX);
    prev_msp->timers[prev_msp->cur_state] += MOD_SUB(prev_msp->last_change, now, SLIENCE_MAX);

    /*
     * Update states, state is sort of a bitmask, except that
     * TASK_RUNNING is 0.
     */
    if (prev->state == TASK_RUNNING)
        next_state = MSA_ONRUNQUEUE;
    else if (prev->state & TASK_INTERRUPTIBLE)
        next_state = MSA_INTERRUPTIBLE_SLEEP;
    else if (prev->state & TASK_UNINTERRUPTIBLE)
        next_state = MSA_UNINTERRUPTIBLE_SLEEP;
    else if (prev->state & (TASK_STOPPED | TASK_TRACED))
        next_state = MSA_STOPPED;
    else if (prev->state & (TASK_DEAD | EXIT_DEAD | EXIT_ZOMBIE))
        next_state = MSA_ZOMBIE;
    else {
        printk(KERN_WARNING "msa: Setting UNKNOWN state from %ld\n",
               prev->state);
        WARN_ON(1);
        next_state = MSA_UNKNOWN;
    }

    /* special states */
    switch (prev_msp->next_state) {
    case MSA_PAGING_SLEEP:
    case MSA_FUTEX_SLEEP:
    case MSA_POLL_SLEEP:
        if (prev->state & TASK_INTERRUPTIBLE ||
            prev->state & TASK_UNINTERRUPTIBLE)
            next_state = prev_msp->next_state;
    default:
        break;
    }

    prev_msp->next_state = prev_msp->cur_state;
    prev_msp->cur_state = next_state;
    prev_msp->last_change = now;

    next_msp->last_change = now;

    WARN_ON(next_msp->next_state == MSA_UNKNOWN);
    next_msp->cur_state = next_msp->next_state;
    if (next_msp->cur_state != MSA_ONCPU_USER)
        next_msp->cur_state = MSA_ONCPU_SYS;
    next_msp->next_state = MSA_UNKNOWN;

    local_irq_restore(flags);
}

/**
 * msa_init:  Initialise the struct microstates in a new task
 * @p: pointer to the struct task_struct to be initialised
 *
 * This function is called from copy_process().
 * It initialises the microstate timers to zero, and sets the
 * current state to MSA_UNINTERRUPTIBLE_SLEEP.
 */
void msa_init(struct task_struct *p)
{
    struct microstates *msp = &p->microstates;

    memset(msp, 0, sizeof *msp);
    MSA_NOW(msp->last_change);
    msp->cur_state = MSA_UNINTERRUPTIBLE_SLEEP;
    msp->next_state = MSA_ONCPU_SYS;
}

/**
 * __msa_set_timer: Helper function to update microstate times.
 * &msp:  Pointer to the struct microstates to update
 * next_state: the state being changed to.
 *
 * The time spent in the current state is updated, and the time of
 * last state change set to MSA_NOW().  Then the current state is updated
 * to next_state.
 */
static inline msa_time_t __msa_set_timer(struct microstates *msp, int next_state)
{
    unsigned long flags;
    msa_time_t now, delta;

    local_irq_save(flags);
    MSA_NOW(now);
    delta = MOD_SUB(msp->last_change, now, SLIENCE_MAX);
    msp->timers[msp->cur_state] += delta;
    msp->last_change = now;
    msp->cur_state = next_state;
    local_irq_restore(flags);

    return delta;
}

#if 0
/**
 * __msa_set_timer_onswitch: same as __msa_set_timer, but it do timing
 * stuff only when state changes.
 */
static inline msa_time_t __msa_set_timer_onswitch(struct microstates *msp,
                          int next_state)
{
    unsigned long flags;
    msa_time_t now, delta;

    local_irq_save(flags);
    delta = 0;
    if (msp->cur_state == next_state)
        goto out;
    MSA_NOW(now);
    delta = now - msp->last_change;
    msp->timers[msp->cur_state] += delta;
    msp->last_change = now;
    msp->cur_state = next_state;
out:
    local_irq_restore(flags);
    return delta;
}
#endif
/**
 * msa_set_timer:  Time stamp an explicit state change.
 * @p: pointer to the task that has just changed state.
 * @next_state: the state being changed to.
 *
 * This function is called, e.g., from __activate_task(), when an
 * immediate state change happens.
 */
void msa_set_timer(struct task_struct *p, int next_state)
{
    struct microstates *msp = &current->microstates;
    __msa_set_timer(msp, MSA_ONCPU_SYS);
}

#if 0
/*
 * Helper routines, to be called from assembly language stubs
 */

/**
 * msa_kernel: change state to MSA_ONCPU_SYS.
 *
 * Should be called upon every entry in the kernel from user space.
 */
asmlinkage void msa_kernel(void)
{
    struct microstates *msp = &current->microstates;
    __msa_set_timer(msp, MSA_ONCPU_SYS);
}

/**
 * msa_user: change state out of MSA_ONCPU_SYS
 *
 * Called when about to leave the kernel.
 */
asmlinkage void msa_user(void)
{
    struct microstates *msp = &current->microstates;
    __msa_set_timer(msp, MSA_ONCPU_USER);
}
#endif

void msa_start_softirq(int irq_id, struct task_struct *p)
{
    msa_time_t now , delta;
    struct microstates *msp = &p->microstates;

    MSA_NOW(now);
    msa_softirq[irq_id].last_entered = now;

    delta = MOD_SUB(msp->last_change, now, SLIENCE_MAX);
    msp->timers[msp->cur_state] += delta;

    if(msp->cur_state == MSA_ONCPU_SYS)
    {
        msa_cputime[0] += delta;
    }
}

void msa_softirq_exit(int irq_id , struct task_struct *p)
{
    msa_time_t now, delta;
    struct microstates *msp = &p->microstates;

    MSA_NOW(now);
    delta = MOD_SUB(msa_softirq[irq_id].last_entered, now, SLIENCE_MAX);
    msa_softirq[irq_id].times += delta;

    msp->last_change = now;
}

#if 0
/**
 * msa_start_irq: mark the start of an interrupt handler.
 * @irq: irq number being handled.
 *
 * Update the current task state to MSA_INTERRUPTED, and start
 * accumulating time to the interrupt handler for irq.
 *
 * Note that the irq_id does not have to be the actual irq, just some way
 * to uniquely identify the interrupt source that is less than NR_IRQ.
 * x86 uses the vector, for instance, since the IRQ numbers don't map
 * to all the relevant interrupt sources.
 */
void msa_start_irq(int irq_id)
{
    struct task_struct *p = current;
    struct microstates *msp = &p->microstates;
    msa_time_t now;
    int nested;

    BUG_ON(irq_id > NR_IRQS);

    /* we're in an interrupt handler... no possibility of preemption */
    MSA_NOW(now);

    nested = hardirq_count() - HARDIRQ_OFFSET;
    BUG_ON(nested < 0);

    __get_cpu_var(msa_irq)[irq_id].last_entered = now;

    if (!nested) {
        msa_time_t delta = now - msp->last_change;
        msp->timers[msp->cur_state] += delta;
        msp->last_change = now;
        if (msp->cur_state == MSA_ONCPU_USER
                || msp->cur_state == MSA_ONCPU_SYS) {
            msp->next_state = msp->cur_state;
            msp->cur_state = MSA_INTERRUPTED;
        }
    }
}

/**
 * msa_continue_irq: While remaining in MSA_INTERRUPTED state, switch
 * to a new IRQ.
 *
 * @oldirq: the irq that was just serviced
 * @newirq: the irq that is about to be serviced.
 *
 * Architectures such as IA64 can handle more than one interrupt
 * without allowing the interrupted process to continue.  This function
 * is called when switching to a new interrupt.
 */
void msa_continue_irq(int oldirq_id, int newirq_id)
{
    msa_time_t now;
    struct msa_irq *mip;

    BUG_ON(oldirq_id > NR_IRQS);
    BUG_ON(newirq_id > NR_IRQS);

    MSA_NOW(now);
    /* we're in an interrupt handler... no possibility of preemption */
    BUG_ON(!in_interrupt());
    mip = __get_cpu_var(msa_irq);

    mip[oldirq_id].times +=  now - mip[oldirq_id].last_entered;
    mip[newirq_id].last_entered = now;
}

/**
 * msa_finish_irq: end processing for an interrupt.
 * @irq: the interrupt that was just serviced.
 *
 * Update the time spent handling irq, then update the current task's
 * state to MSA_ONCPU_USER or MSA_ONCPU_SYS.
 *
 * This MUST be called instead of irq_exit() whenever msa_start_irq()
 * was called for a given irq.  irq_exit() is implied by this function.
 *
 * See the notes in msa_start_irq() for info about irq_id
 */
void msa_irq_exit(int irq_id, int is_going_to_user)
{
    struct task_struct *p = current;
    struct microstates *msp = &p->microstates;
    msa_time_t now, delta;
    struct msa_irq *mip;
    int nested;

    BUG_ON(irq_id > NR_IRQS);

    mip = get_cpu_var(msa_irq);
    nested = hardirq_count() - HARDIRQ_OFFSET;
    BUG_ON(nested < 0);

    MSA_NOW(now);
    delta = now - mip[irq_id].last_entered;
    mip[irq_id].times += delta;

    irq_exit();

    if (!nested) {
        msa_time_t before = now;
        MSA_NOW(now);
        delta = now - before;
        msp->timers[msp->cur_state] += now - msp->last_change;
        msp->last_change = now;
        if (is_going_to_user)
            msp->cur_state = MSA_ONCPU_USER;
        else
            msp->cur_state = MSA_ONCPU_SYS;
    }

    put_cpu_var(msa_irq);
}
#endif
/**
 * msa_update_parent:  Accumulate child times into parent, after zombie is over.
 * @parent: pointer to parent task
 * @this: pointer to task that is now a zombie
 *
 * Called from release_task(). (Note: it may be better to call this
 * from wait_zombie())
 */
void msa_update_parent(struct task_struct *parent, struct task_struct *this)
{
    enum msa_thread_state s;
    msa_time_t *pmsp = parent->microstates.child_timers;
    struct microstates *msp = &this->microstates;
    msa_time_t *msc = msp->timers;
    msa_time_t *msgc = msp->child_timers;

    /*
     * State could be MSA_ZOMBIE (if parent is interested)
     * or something else (if the parent isn't interested)
     */
    __msa_set_timer(msp, msp->cur_state);

    for (s = 0; s < MSA_NR_STATES; s++)
        *pmsp++ += *msc++ + *msgc++;
}

/**
 * sys_msa: get microstate data for self or waited-for children.
 * @ntimers: the number of timers requested
 * @which: which set of timers is wanted.
 * @timers: pointer in user space to an array of timers.
 *
 * 'which' can take the values
 *   MSA_THREAD: return times for current thread only
 *   MSA_SELF:  return times for current process,
 *      summing over all live threads
 *   MSA_CHILDREN: return sum of times for all dead children.
 *   MSA_GET_NOW: return the current msa timer value in the first value.
 *
 * The timers are ordered so that the most interesting ones are first.
 * Thus a user program can ask for only the  few most interesting ones
 * if it wishes.  Also, we can add more in the kernel as we need
 * to without invalidating user code.
 */
SYSCALL_DEFINE3(msa, int, ntimers, int, which, msa_time_t __user *, timers)
{
    msa_time_t *tp;
    int i;
    struct microstates *msp = &current->microstates;
    struct microstates out;

    WARN_ON(current->microstates.cur_state != MSA_ONCPU_SYS);

    if (ntimers <= 0 || ntimers > MSA_NR_STATES)
        return -EINVAL;

    switch (which) {
    case MSA_SELF:
    case MSA_THREAD:
        preempt_disable();
        __msa_set_timer(msp, MSA_ONCPU_SYS);

        if (which == MSA_SELF) {
            struct task_struct *task;
            struct task_struct *leader_task;
            msa_time_t *tp1;

            memset(out.timers, 0, sizeof(out.timers));
            read_lock(&tasklist_lock);
            leader_task = task = current->group_leader;
            do {
                tp = task->microstates.timers;
                tp1 = out.timers;
                for (i = 0; i < ntimers; i++)
                    *tp1++ += *tp++;
            } while ((task = next_thread(task)) != leader_task);
            read_unlock(&tasklist_lock);
            tp = out.timers;
        } else
            tp = msp->timers;
        preempt_enable();
        break;

    case MSA_CHILDREN:
        tp =  msp->child_timers;
        break;

    case MSA_GET_NOW:
        ntimers = 1;
        tp = out.timers;
        MSA_NOW(*tp);
        break;

    default:
        return -EINVAL;
    }

    for (i = 0; i < ntimers; i++) {
        __u64 x = MSA_TO_NSEC(*tp++);
        if (copy_to_user(timers++, &x, sizeof x))
            return -EFAULT;
    }

    return 0;
}

#ifdef CONFIG_PROC_FS

/*
 * Display the total number of nanoseconds since boot spent
 * in handling each interrupt on each processor.
 */
#if 0
static void *msa_irq_time_seq_start(struct seq_file *f, loff_t *pos)
{
    return (*pos <= NR_IRQS) ? pos : NULL;
}

static void *msa_irq_time_seq_next(struct seq_file *f, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos > NR_IRQS)
        return NULL;
    return pos;
}

static void msa_irq_time_seq_stop(struct seq_file *f, void *v)
{
    /* Nothing to do */
}

static int msa_irq_time_seq_show(struct seq_file *f, void *v)
{
    int i = *(loff_t *) v, cpu;

    if (i == 0) {
        msa_time_t now;
        char cpuname[10];
        MSA_NOW(now);
        seq_printf(f, "Now: %15llu\n", now);
        seq_printf(f, "     ");
        for_each_present_cpu(cpu) {
            sprintf(cpuname, "CPU%d", cpu);
            seq_printf(f, " %15s", cpuname);
        }
        seq_putc(f, '\n');
    }

    if (i < NR_IRQS) {
        int all_zeroes = 0;
        for_each_present_cpu(cpu)
            if (!(all_zeroes = !per_cpu(msa_irq, cpu)[i].times))
                break;
        if (all_zeroes)
            return 0;
        seq_printf(f, "%3d: ", i);
        for_each_present_cpu(cpu) {
            msa_time_t x = MSA_TO_NSEC(per_cpu(msa_irq, cpu)[i].times);
            seq_printf(f, " %15llu", (unsigned long long)x);
        }
        seq_putc(f, '\n');
    }

    return 0;
}

static struct seq_operations msa_irq_time_seq_ops = {
    .start  = msa_irq_time_seq_start,
    .next   = msa_irq_time_seq_next,
    .stop   = msa_irq_time_seq_stop,
    .show   = msa_irq_time_seq_show,
};

static int msa_irq_time_open(struct inode *inode, struct file *filp)
{
    return seq_open(filp, &msa_irq_time_seq_ops);
}

static struct file_operations proc_msa_irq_time_ops = {
    .open       = msa_irq_time_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
};

static int __init proc_msa_irq_time_iinit(void)
{
    struct proc_dir_entry *entry;
    entry = create_proc_entry("msa_irq_time", 0, NULL);
    if (entry)
        entry->proc_fops = &proc_msa_irq_time_ops;
    return 0;
}
#endif

static void *msa_softirq_time_seq_start(struct seq_file *f, loff_t *pos)
{
    return (*pos <= NR_SOFTIRQS) ? pos : NULL;
}

static void *msa_softirq_time_seq_next(struct seq_file *f, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos > NR_SOFTIRQS)
        return NULL;
    return pos;
}

static void msa_softirq_time_seq_stop(struct seq_file *f, void *v)
{
    /* Nothing to do */
}

static int msa_softirq_time_seq_show(struct seq_file *f, void *v)
{
    int i = *(loff_t *) v, cpu;
    msa_time_t now;
    MSA_NOW(now);

    if(i == 0)
    {
        seq_printf(f, "Now: %15lu\n", now);
    }

    if(i < NR_SOFTIRQS)
    {
        seq_printf(f, "%s: ", softirq_name[i]);
        seq_printf(f, " %15lu", (unsigned long long)msa_softirq[i].times);
        seq_putc(f, '\n');
    }

    return 0;
}

static struct seq_operations msa_softirq_time_seq_ops = {
    .start  = msa_softirq_time_seq_start,
    .next   = msa_softirq_time_seq_next,
    .stop   = msa_softirq_time_seq_stop,
    .show   = msa_softirq_time_seq_show,
};

static int msa_softirq_time_open(struct inode *inode, struct file *filp)
{
    return seq_open(filp, &msa_softirq_time_seq_ops);
}

static struct file_operations proc_msa_softirq_time_ops = {
    .open       = msa_softirq_time_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
};

static int __init proc_msa_softirq_time_iinit(void)
{
    struct proc_dir_entry *entry;
    entry = create_proc_entry("msa_softirq_time", 0, NULL);
    if (entry)
        entry->proc_fops = &proc_msa_softirq_time_ops;
    return 0;
}

static void *msa_cpu_time_seq_start(struct seq_file *f, loff_t *pos)
{
    return (*pos <= CPUTIME_TYPE) ? pos : NULL;
}

static void *msa_cpu_time_seq_next(struct seq_file *f, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos > CPUTIME_TYPE)
        return NULL;
    return pos;
}

static void msa_cpu_time_seq_stop(struct seq_file *f, void *v)
{
    /* Nothing to do */
}

static int msa_cpu_time_seq_show(struct seq_file *f, void *v)
{
    int i = *(loff_t *) v, cpu;
    msa_time_t now;
    MSA_NOW(now);

    if(i == 0)
    {
        seq_printf(f, "Now: %15lu\n", now);
    }

    if(i < CPUTIME_TYPE)
    {
        seq_printf(f, "%s: ", cputime_name[i]);
        seq_printf(f, " %15lu", (unsigned long long)msa_cputime[i]);
        seq_putc(f, '\n');
    }

    return 0;
}

static struct seq_operations msa_cpu_time_seq_ops = {
    .start  = msa_cpu_time_seq_start,
    .next   = msa_cpu_time_seq_next,
    .stop   = msa_cpu_time_seq_stop,
    .show   = msa_cpu_time_seq_show,
};

static int msa_cpu_time_open(struct inode *inode, struct file *filp)
{
    return seq_open(filp, &msa_cpu_time_seq_ops);
}

static struct file_operations proc_msa_cpu_time_ops = {
    .open       = msa_cpu_time_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
};

static int __init proc_msa_cpu_time_iinit(void)
{
    struct proc_dir_entry *entry;
    entry = create_proc_entry("msa_cpu_time", 0, NULL);
    if (entry)
        entry->proc_fops = &proc_msa_cpu_time_ops;
    return 0;
}


#if 0
fs_initcall(proc_msa_irq_time_iinit);
#endif
fs_initcall(proc_msa_softirq_time_iinit);
fs_initcall(proc_msa_cpu_time_iinit);


#endif
#endif
/* CONFIG_MICROSTATE_ACCT */
