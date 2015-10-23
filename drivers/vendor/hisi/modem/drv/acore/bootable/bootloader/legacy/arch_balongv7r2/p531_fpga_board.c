#include <boot/boot.h>
#include <balongv7r2/config.h>
#include <balongv7r2/parameters.h>

void p531_axi_exp32init(void)
{
	unsigned int val = 0;
	unsigned int reg = 0;

	reg = 0x90000064;
	val = 0x0;
	writel(val, reg);

	reg = 0x90010444;
	val = readl(reg);
	val |= 0x10;
	writel(val, reg);

	reg = 0x90010448;
	val = readl(reg);
	val |= 0x10;
	writel(val, reg);

	reg = 0x9001044C;
	val = readl(reg);
	val |= 0x10;
	writel(val, reg);

	reg = 0x90010434;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0x1a;
	writel(val, reg);

	reg += 0x4;
	val = 0x1a;
	writel(val, reg);

	reg += 0x4;
	val = 0x1a;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

}

void p531_axi_exp64init(void)
{
	unsigned int val = 0;
	unsigned int reg = 0;

	reg = 0x90010470;
	val = readl(reg);
	val |= 0x10;
	writel(val, reg);

	reg = 0x90010474;
	val = readl(reg);
	val |= 0x10;
	writel(val, reg);

	reg = 0x90010478;
	val = readl(reg);
	val |= 0x10;
	writel(val, reg);

	reg = 0x9001045c;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg = 0x9001046c;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0x1a;
	writel(val, reg);

	reg += 0x4;
	val = 0x1a;
	writel(val, reg);

	reg += 0x4;
	val = 0x1a;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

	reg += 0x4;
	val = 0xa;
	writel(val, reg);

}

void p531_fpga_init(void)
{
	unsigned int val = 0;
	unsigned int reg = 0;

	p531_axi_exp32init();
	p531_axi_exp64init();

	reg = 0x9000003c;
	val = readl(reg);
	val &= ~0x10001;
	writel(val, reg);

}
