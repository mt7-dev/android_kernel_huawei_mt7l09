

#ifndef __OSL_BIO_H
#define __OSL_BIO_H

static __inline__ void DWB(void) /* drain write buffer */
{
    asm volatile ( "DMB;");
}

static __inline__ void nop(void) /* drain write buffer */
{
    asm volatile ( "NOP;");
}

static __inline__ void writel(unsigned val, unsigned addr)
{
    DWB();
    (*(volatile unsigned *) (addr)) = (val);
    DWB();
}

static __inline__ void writeb(unsigned val, unsigned addr)
{
    DWB();
    (*(volatile unsigned char *) (addr)) = (val);
    DWB();
}

static __inline__ unsigned readl(unsigned addr)
{
    DWB();
    return (*(volatile unsigned *) (addr));
    DWB();
}

#endif	/* __OSL_BIO_H */
