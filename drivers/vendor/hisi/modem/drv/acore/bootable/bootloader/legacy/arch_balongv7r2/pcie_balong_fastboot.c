#include <boot/boot.h>

#include <bsp_sram.h>
#include <bsp_pcie.h>

struct pcie_cmd
{
    char *cmd;
    unsigned cmd_size;
    void (*handler)(char *cmd);
    char *cmd_help;
};

static unsigned pcie_id = 0;

static struct pcie_cfg *pcie_cfg_table = (struct pcie_cfg *)SRAM_PCIE_INFO_ADDR;

static unsigned hex2unsigned(char *x)
{
    unsigned n = 0;

    if ((!memcmp((void*)x, "0x", 2)) || (!memcpy((void*)x, "0X", 2)))
        x += 2;

    while (*x) {
        switch(*x) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = (n << 4) | (*x - '0');
            break;
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
            n = (n << 4) | (*x - 'a' + 10);
            break;
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
            n = (n << 4) | (*x - 'A' + 10);
            break;
        default:
            return n;
        }
        x++;
    }

    return n;
}

void pcie_fastboot_cmd_d(char *cmd)
{
    unsigned addr = hex2unsigned(cmd);
    cprintf("0x%X: %X\n", addr, *((unsigned *)addr));
}

void pcie_fastboot_cmd_m(char *cmd)
{
    unsigned addr = 0;
    unsigned value = 0;

    addr = hex2unsigned(cmd);

    while (*cmd++ != ' ') ;
    value = hex2unsigned(cmd);

    *((unsigned *)addr) = value;

    cprintf("0x%X: %X\n", addr, *((unsigned *)addr));
}

void pcie_fastboot_cmd_id(char *cmd)
{
    char id_char = *cmd;

    pcie_id = id_char - '0';

    pcie_cfg_table[pcie_id].valid = PCIE_CFG_VALID;

    cprintf("the controller %d's config is enabled\n", pcie_id);
}

void pcie_fastboot_cmd_mode(char *cmd)
{
    if (!memcmp((void*)cmd, (void*)"rc", 2))
    {
        pcie_cfg_table[pcie_id].work_mode = PCIE_WORK_MODE_RC;
    }
    else if (!memcmp((void*)cmd, (void*)"ep", 2))
    {
        pcie_cfg_table[pcie_id].work_mode = PCIE_WORK_MODE_EP;
    }
    else
    {
        cprintf("error work mode: %s\n", cmd);
    }
}

void pcie_fastboot_cmd_help(char *cmd);
static struct pcie_cmd pcie_cmd_table[] = {
    {"m" , sizeof("m"), pcie_fastboot_cmd_m, "modify memory"},
    {"d" , sizeof("d"), pcie_fastboot_cmd_d, "display memory"},
    {"id" , sizeof("id"), pcie_fastboot_cmd_id, "choose the controller id"},
    {"mode", sizeof("mode"), pcie_fastboot_cmd_mode, "set the controller's work mode(rc or ep)"},
    {"help", sizeof("help"), pcie_fastboot_cmd_help, "show this message"},
};

void pcie_fastboot_cmd_help(char *cmd)
{
    unsigned i = 0;

    for (i = 0; i < sizeof(pcie_cmd_table)/sizeof(pcie_cmd_table[0]); i++)
    {
        cprintf("%s\n", pcie_cmd_table[i].cmd);
        cprintf("    %s\n", pcie_cmd_table[i].cmd_help);
    }
}

void pcie_fastboot_console(char *cmd)
{
    unsigned i = 0;

    for (i = 0; i < sizeof(pcie_cmd_table)/sizeof(pcie_cmd_table[0]); i++)
    {
        if ((*(cmd + pcie_cmd_table[i].cmd_size - 1) == ' ') &&
            (!memcmp((void*)cmd, (void*)pcie_cmd_table[i].cmd, pcie_cmd_table[i].cmd_size - 1)))
        {
            pcie_cmd_table[i].handler(cmd + pcie_cmd_table[i].cmd_size);
            return;
        }
    }

    pcie_fastboot_cmd_help(cmd);
}

void pcie_fastboot_init(void)
{
    memset((void*)SRAM_PCIE_INFO_ADDR, 0, 64);
}


