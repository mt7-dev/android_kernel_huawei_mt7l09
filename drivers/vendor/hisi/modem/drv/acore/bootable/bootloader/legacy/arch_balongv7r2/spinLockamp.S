#define _ASMLANGUAGE
	.text
	.global spinLockAmpGive
	.global spinLockAmpTakeOneTime
	.global spinLockAmpTake
spinLockAmpGive:	
	DSB
	MOV r1,#0
	STR r1,[r0,#0]
	DSB
	MOV pc,lr

spinLockAmpTakeOneTime:
	MOV r1,#1
	DSB
		
	LDREX r2,[r0]
	STREX r3,r1,[r0]
	ORR r0,r2,r3
	MOV pc,lr
	
spinLockAmpTake:
	MOV r1,#1
	DSB
        AmpTake_again:
		LDREX r2,[r0]
		STREX r3,r1,[r0]
		TEQ r3,#0
		BNE AmpTake_again
		TEQ r2,#0
		BNE AmpTake_again
		ORR r0,r2,r3 
		MOV pc,lr
