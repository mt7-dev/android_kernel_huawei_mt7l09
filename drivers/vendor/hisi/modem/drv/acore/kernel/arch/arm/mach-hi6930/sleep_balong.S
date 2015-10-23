#include <linux/linkage.h>
#include <asm/assembler.h>
#include "sleep.h"

_asm_function_called_from_c(cpu_enter_idle)

	@save current mode r0~r12, r14, so we can use them
	STMFD  sp!, {r0-r12, r14}
	dsb					@ WFI may enter a low-power mode
	wfi

	LDMFD    sp!, {r0-r12, r14}

	/** go back to the call point  **/
	MOV  pc, lr
	NOP
	NOP
	NOP

      .ltorg

