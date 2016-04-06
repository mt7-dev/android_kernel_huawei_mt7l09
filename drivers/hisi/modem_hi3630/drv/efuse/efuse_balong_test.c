#include <linux/types.h>
#include <linux/efuse/efuse_balong.h>

#define EFUSE_OK               (0)
#define EFUSE_ERROR            (-1)

int test_bsp_efuse_read_para_error(void);
int test_bsp_efuse_write_para_error(void);
int bsp_efuse_write_zero(void);
int efuse_test(void);

int test_bsp_efuse_read_para_error(void)
{
    if(EFUSE_ERROR == bsp_efuse_read(NULL, 0, 1))
    {
        return EFUSE_OK;
    }

    return EFUSE_ERROR;
}

int test_bsp_efuse_write_para_error(void)
{
    if(EFUSE_ERROR == bsp_efuse_write(NULL, 0, 1))
    {
        return EFUSE_OK;
    }

    return EFUSE_ERROR;
}

int bsp_efuse_write_zero(void)
{
    u32 value = 0;
    if(EFUSE_ERROR == bsp_efuse_write(&value, 0, 1))
    {
        return EFUSE_ERROR;
    }

    return EFUSE_OK;
}

extern void bsp_efuse_show(void);
int bsp_efuse_test(void)
{
    test_bsp_efuse_read_para_error();
    test_bsp_efuse_write_para_error();
    
	bsp_efuse_show();
    bsp_efuse_write_zero();
	
	return EFUSE_OK;
}

