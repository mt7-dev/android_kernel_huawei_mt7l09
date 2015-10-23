#ifndef __SYSCTRL_H__
#define __SYSCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif


#define SC_REGBASE_ADR    0x80000000


#define GPIO_LED 

#define SC_CTRL     	0x0000        
//#define SC_RESERVED 	0x0004        
#define SC_ITMCTRL  	0x0008        
#define SC_IMSTAT   	0x000C        
#define SC_XTALCTRL 	0x0010        
#define SC_PLLCTRL  	0x0014        
//#define SC_RESERVED   	0x0018        
#define SC_PERCTRL0 	0x001C        
#define SC_PERCTRL1 	0x0020        
#define SC_PEREN0   	0x0024        
#define SC_PERDIS0  	0x0028        
#define SC_PERCLKEN0	0x002C        
#define SC_PERSTAT0 	0x0030        
#define SC_PERCTRL2 	0x0034        
#define SC_PERCTRL3 	0x0038        
#define SC_PERCTRL4 	0x003C        
#define SC_PERCTRL5 	0x0040        
#define SC_PERCTRL6 	0x0044        
#define SC_PERCTRL7 	0x0048        
#define SC_PERCTRL8 	0x004C        
#define SC_PERCTRL9 	0x0050        
#define SC_PERCTRL10	0x0054        
#define SC_PERCTRL11	0x0058        
#define SC_PERCTRL12	0x005C        
#define SC_PERCTRL13	0x0060        
#define SC_PERCTRL14	0x0064        
#define SC_PERCTRL15	0x0068        
#define SC_PERCTRL16	0x006C        
#define SC_PERCTRL17	0x0070        
#define SC_PERCTRL18	0x0074        
#define SC_PERCTRL19	0x0078        
#define SC_PERCTRL20	0x007C        
#define SC_PERCTRL21	0x0080        
#define SC_PERCTRL22	0x0084        
#define SC_PERCTRL23	0x0088        
#define SC_PERCTRL24	0x008C        
#define SC_PERCTRL25	0x0090        
#define SC_PERCTRL26	0x0094        
#define SC_PERCTRL27	0x0098        
#define SC_PERCTRL28	0x009C        
#define SC_PERCTRL29	0x00A0        
#define SC_PERCTRL30	0x00A4        
#define SC_PERCTRL31	0x00A8        
#define SC_PERCTRL32	0x00AC        
#define SC_PERCTRL33	0x00B0        
#define SC_PERCTRL34	0x00B4        
#define SC_PERCTRL35	0x00B8        
#define SC_PERCTRL36	0x00BC        
#define SC_PERCTRL37	0x00C0        
#define SC_PERCTRL38	0x00C4        
#define SC_PERCTRL39	0x00C8        
#define SC_PERCTRL40	0x00CC        
#define SC_PERCTRL41	0x00D0        
#define SC_PERCTRL42	0x00D4        
#define SC_PERCTRL43	0x00D8        
#define SC_PERCTRL44	0x00DC        
#define SC_PERCTRL45	0x00E0        
#define SC_PERCTRL46	0x00E4        
#define SC_PERCTRL47	0x00E8        
#define SC_PERCTRL48	0x00EC        
#define SC_PERCTRL49	0x00F0        
#define SC_PERCTRL50	0x00F4        
#define SC_PERCTRL51	0x00F8        
#define SC_PERCTRL52	0x00FC        
#define SC_PERCTRL53	0x0100        
#define SC_PERCTRL54	0x0104        
#define SC_PERCTRL55	0x0108        
#define SC_PERCTRL56	0x010C        
#define SC_PERCTRL57	0x0110      
#define SC_PERCTRL58	0x0114                
#define SC_PERCTRL59	0x0118                
#define SC_PERCTRL60	0x011C                
#define SC_PERCTRL61	0x0120
#define SC_PERCTRL62	0x0124
#define SC_PERCTRL63	0x0128
#define SC_PERCTRL64	0x012C
#define SC_PERCTRL65	0x0130        
#define SC_PEREN1   	0x0134        
#define SC_PERDIS1  	0x0138        
#define SC_PERCLKEN1	0x013C        
#define SC_PERSTAT1 	0x0140  


/* system restart types */
#define	BOOT_NORMAL		0x00	 /* normal reboot with countdown */
#define BOOT_NO_AUTOBOOT	0x01	/* no autoboot if set */
#define BOOT_CLEAR		0x02	/* clear memory if set */
#define BOOT_QUICK_AUTOBOOT	0x04	/* fast autoboot if set */

/* for backward compatibility */
#define BOOT_WARM_AUTOBOOT		BOOT_NORMAL
/*#define BOOT_WARM_NO_AUTOBOOT		BOOT_NO_AUTOBOOT*/
#define BOOT_WARM_QUICK_AUTOBOOT	BOOT_QUICK_AUTOBOOT
#define BOOT_COLD			BOOT_CLEAR
#define BOOT_WARM_NO_AUTOBOOT 0x02

/* Make sure excArmLib.h also has same settings */

/* bits in the PSR */
#define	V_BIT	(1<<28)
#define	C_BIT	(1<<29)
#define	Z_BIT	(1<<30)
#define	N_BIT	(1<<31)
#define I_BIT   (1<<7)
#define F_BIT   (1<<6)
#define	T_BIT	(1<<5)


/* mode bits */

#define MODE_SYSTEM32	0x1F 
#define MODE_UNDEF32	0x1B
#define	MODE_ABORT32	0x17
#define MODE_SVC32	0x13
#define MODE_IRQ32      0x12
#define MODE_FIQ32      0x11
#define MODE_USER32	0x10


/* masks for getting bits from PSR */

#define MASK_MODE	0x0000003F 
#define	MASK_32MODE	0x0000001F
#define	MASK_SUBMODE	0x0000000F
#define MASK_INT	0x000000C0
#define	MASK_CC		0xF0000000

/* MMU Control Register bit allocations */

#define MMUCR_M_ENABLE	 (1<<0)  /* MMU enable */
#define MMUCR_A_ENABLE	 (1<<1)  /* Address alignment fault enable */
#define MMUCR_C_ENABLE	 (1<<2)  /* (data) cache enable */
#define MMUCR_W_ENABLE	 (1<<3)  /* write buffer enable */
#define MMUCR_PROG32	 (1<<4)  /* PROG32 */
#define MMUCR_DATA32	 (1<<5)  /* DATA32 */
#define MMUCR_L_ENABLE	 (1<<6)  /* Late abort on earlier CPUs */
#define MMUCR_BIGEND	 (1<<7)  /* Big-endian (=1), little-endian (=0) */
#define MMUCR_SYSTEM	 (1<<8)  /* System bit, modifies MMU protections */
#define MMUCR_ROM	 (1<<9)  /* ROM bit, modifies MMU protections */
#define MMUCR_F		 (1<<10) /* Should Be Zero */
#define MMUCR_Z_ENABLE	 (1<<11) /* Branch prediction enable on 810 */
#define MMUCR_I_ENABLE	 (1<<12) /* Instruction cache enable */
#define MMUCR_V_ENABLE	 (1<<13) /* Exception vectors remap to 0xFFFF0000 */
#define MMUCR_ALTVECT    MMUCR_V_ENABLE /* alternate vector select */
#define MMUCR_RR_ENABLE	 (1<<14) /* Round robin cache replacement enable */
#define MMUCR_ROUND_ROBIN MMUCR_RR_ENABLE  /* round-robin placement */
#define MMUCR_DISABLE_TBIT   (1<<15) /* disable TBIT */
#define MMUCR_ENABLE_DTCM    (1<<16) /* Enable Data TCM */
#define MMUCR_ENABLE_ITCM    (1<<18) /* Enable Instruction TCM */
#define MMUCR_UNALIGNED_ENABLE (1<<22) /* Enable unaligned access */
#define MMUCR_EXTENDED_PAGE (1<<23)  /* Use extended PTE format */
#define MMUCR_VECTORED_INTERRUPT (1<<24) /* Enable VIC Interface */


#ifdef __cplusplus
}
#endif

#endif
