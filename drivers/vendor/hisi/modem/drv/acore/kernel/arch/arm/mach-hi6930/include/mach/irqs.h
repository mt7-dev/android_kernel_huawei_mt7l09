/*
 *  arch/arm/mach-balongv7r2/include/mach/irqs.h
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ASM_ARCH_BALONG_IRQS_H
#define __ASM_ARCH_BALONG_IRQS_H

#include "soc_interrupts.h"


#define NR_IRQS			             INT_LVL_MAX

#define MAX_GIC_NR		             1

#define BALONG_USB3_IRQ		         INT_LVL_USB3

#ifndef NR_IRQS
#error "NR_IRQS not defined by the board-specific files"
#endif


#endif
