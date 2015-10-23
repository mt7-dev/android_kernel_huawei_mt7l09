.equ INTEGRATOR_SC_BASE , 0x90000000
.equ SC_AO_CTRL0    ,(INTEGRATOR_SC_BASE+0x400)
.equ SC_AO_CTRL1    ,(INTEGRATOR_SC_BASE+0x404)

.equ PWR_CTRL0      ,(INTEGRATOR_SC_BASE+0xc00)

.equ UART0_BASE, (INTEGRATOR_SC_BASE+0x7000)
.equ REG_UARTDR          , 0x000 
.equ REG_UARTRSR_UARTECR , 0x004 
.equ REG_UARTFR          , 0x018 
.equ REG_UARTIBRD        , 0x024 
.equ REG_UARTFBRD        , 0x028 
.equ REG_UARTLCR_H       , 0x02C 
.equ REG_UARTCR          , 0x030 
.equ REG_UARTIFLS        , 0x034 
.equ REG_UARTIMSC        , 0x038 
.equ REG_UARTICR         , 0x044 
.equ REG_UARTDMACR       , 0x048 

.equ  M3_NVIC_VTOR, 0xE000ED08
.equ  M3_MPU_BASE,  0xE000ED90
.equ  NANDC_MEM_BASE, 0xA0000000

.equ CRG_CLKDIV2    ,(0x9001b000+0x104)
.equ CRG_CLKDIV3    ,(0x9001b000+0x108)
.equ CRG_DFS1_CTRL3 ,(0x9001b000+0x208)
.equ CRG_DFS5_CTRL3 ,(0x9001b000+0x238)


.equ SRAM_ADDR ,                     (0x4FE00000)
.equ SRAM_SIZE ,                     (1024*1024)
.equ STACK_TOP ,                     (SRAM_ADDR + SRAM_SIZE - 2*1024 - 4)  /* 2K的共享数据 */

/*平台宏定义*/
.equ PLATFORM_ASIC    ,  1
.equ PLATFORM_PORTING ,  2
.equ PLATFORM_EDA     ,  3

