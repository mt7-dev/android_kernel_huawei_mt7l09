/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2012, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * The BALONG peripherals are spread all over across 768MB of physical
 * space, which makes just having a simple IO_ADDRESS macro to slide
 * them into the right virtual location rough.  
 *
 */

#ifndef __ASM_BALONGV7R2_IOMAP_H
#define __ASM_BALONGV7R2_IOMAP_H

#include <asm/sizes.h>
#include <mach/hardware.h>
#include "bsp_memmap.h"

/* Physical base address and size of peripherals.
 * Ordered by the virtual base addresses they will be mapped at.
 *
 * If you add or remove entries here, you'll want to edit the
 * msm_io_desc array in arch/arm/mach-balongv7r2/io.c to reflect your
 * changes.
 *
 */


#define GIC_CPU_VIRT_BASE                         (HI_APP_GIC_BASE_ADDR_VIRT + 0x100)
#define GIC_DIST_VIRT_BASE                        (HI_APP_GIC_BASE_ADDR_VIRT + 0x1000)

#define HI_TIMER_14_REGBASE_VIRT_ADDR             (HI_TIMER_14_REGBASE_ADDR_VIRT)
#define HI_TIMER_15_REGBASE_VIRT_ADDR             (HI_TIMER_15_REGBASE_ADDR_VIRT)

#endif
