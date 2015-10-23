#include <soc_onchiprom.h>
#include "bio.h"

#define STACK_TOP 0x1000FFD0

#define COPYRIGHT "Copyright 2008-2020 HUAWEI TECHNOLOGIES CO., LTD."

/*
 * sc and crg register define
 */
#define HI_SC_REGBASE_ADDR 0x90000000

#define MDDRC_REGBASE_ADR  0x90020000

#define HI_CRG_CTRL14_OFFSET 0x38

/*
 * UART module register define
 */
#define UART0_BASE (HI_SC_REGBASE_ADDR+0x7000)
#define UART_RBR   0x0
#define UART_THR   0x0
#define UART_DLL   0x0
#define UART_IER   0x4
#define UART_DLH   0x4
#define UART_IIR   0x8
#define UART_FCR   0x8
#define UART_LCR   0xC
#define UART_MCR   0x10
#define UART_LSR   0x14
#define UART_TXDEPTH 0x2C
#define UART_DEBUG 0x28
#define UART_FAR   0x70
#define UART_TFR   0x74
#define UART_RFW   0x78
#define UART_USR   0x7C
#define UART_TFL   0x80
#define UART_RFL   0x84
#define UART_HTX   0xA4
#define UART_DMASA 0xA8
#define UART_CPR   0xF4

int main(void);
void nmi_handler(void);
void hardfault_handler(void);

void mddrc_init();
void print_info(const void *pucBuffer);

#define AUTO_ENUM_FLAG_VALUE 0x82822828

#ifdef RAMINIT_CHIP_P531
#define VECTOR_NUM 45
#else
#define VECTOR_NUM 144
#endif


struct head_format
{
	unsigned int vectors[VECTOR_NUM];	/* interrupt vectors */
	unsigned int length;				/* image length */
	unsigned char rootca[260];			/* reserved for ROOT CA */
	unsigned char version[32];			/* reserved for version */
	unsigned char copyright[sizeof(COPYRIGHT)];			/* copyright, will be used for image check */
};

__attribute__ ((section("header"))) struct head_format head =
{
	.vectors =
	{
		STACK_TOP,
		(unsigned int)main,
		(unsigned int)nmi_handler,
		(unsigned int)hardfault_handler,
	},
	.length = 0,
	.rootca = {0},
	.version = {0},
	.copyright = COPYRIGHT,
};

int main()
{
    mddrc_init();

	/* set auto enum flag */
	writel(AUTO_ENUM_FLAG_VALUE, OCR_AUTO_ENUM_FLAG_ADDR);

    print_info("\r\nraminit ok");
    return 0;
}

void print_info(const void *pucBuffer)
{
    volatile unsigned int ulStrLen = 0x00;
    volatile unsigned char *pucTmp = (unsigned char *)pucBuffer;
    volatile unsigned int u32Loop;

    while('\0' != *pucTmp++ )
    {
        ulStrLen++;
    }
    pucTmp = (unsigned char *)pucBuffer;
    while(ulStrLen--)
    {
        /*check tx fifo is empty*/
        u32Loop = 10000;
        while(0x00 == (readl(UART0_BASE+UART_USR) & 0x04))
        {
            /* there is nothing we can do when timeout */
            if(0 == --u32Loop)
            {
                return;
            }
        }
        writel(*pucTmp, UART0_BASE + UART_THR);
        pucTmp++;
    }
}


void nmi_handler(void)
{
    /* coverity[no_escape] */
    while(1);
}

void hardfault_handler(void)
{
    /* coverity[no_escape] */
    while(1);
}