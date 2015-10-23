#ifndef __ASM_ARM_EXEC_H
#define __ASM_ARM_EXEC_H

/* exception vector addresses */
#define EXC_OFF_RESET		0x00	/* reset */
#define EXC_OFF_UNDEF		0x04	/* undefined instruction */
#define EXC_OFF_SWI		0x08	/* software interrupt */
#define EXC_OFF_PREFETCH	0x0c	/* prefetch abort */
#define EXC_OFF_DATA		0x10	/* data abort */
#define EXC_OFF_IRQ             0x18    /* interrupt */
#define EXC_OFF_FIQ             0x1C    /* fast interrupt */

#define arch_align_stack(x) (x)

#endif /* __ASM_ARM_EXEC_H */
