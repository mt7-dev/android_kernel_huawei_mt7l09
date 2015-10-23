#ifndef __SPINLOCKAMP_H
#define __SPINLOCKAMP_H

extern void raw_spin_lock(unsigned int *);
extern void raw_spin_unlock(unsigned int *);
extern int raw_spin_trylock(unsigned int *);


#endif
