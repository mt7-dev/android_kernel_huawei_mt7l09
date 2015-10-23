
/*lint -save -e537 -e451*/
#include <vxWorks.h>
#include <logLib.h>
#include <errnoLib.h>
#include <stdio.h>
#include <semLib.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include "bsp_om_api.h"
#include "bsp_om_log.h"
#include "bsp_om.h"
#include "bsp_om_save.h"
#include "bsp_rfile.h"
/*lint -restore*/
struct linux_dirent {
	unsigned long	d_ino;
	unsigned long	d_off;
	unsigned short	d_reclen;
	char		    d_name[2];
};

typedef struct {
    char *s;        /* The user's output buffer */
    int nmax;       /* The length of the user's output buffer */
    int length;     /* The current length of the string in s[] */
} print_data_t;

static char g_OmLogBuffer[256] = {0x00};

void om_trace(char *buffer, char *fmt,...);
int  om_create_dir(char *path);
int  om_open_dir(char *path);
int  om_close_dir(int fd);

void om_trace(char *buffer, char *fmt,...)
{
    va_list arglist = (va_list)NULL;/*lint !e40 !e522*/

	va_start(arglist, fmt);
    /*lint -save -e119*/
	vsnprintf(buffer, 256, fmt, arglist); /* [false alarm]:屏蔽Fority错误 */
    /*lint -restore*/
	va_end(arglist);

    return;
}

int om_create_dir(char *path)
{
    int fd;

    /* 如果文件夹不存在，创建新文件夹*/
    fd = bsp_access((s8*)path, 0); //F_OK, 检查文件是否存在
    if(0 != fd)
    {
        fd  = bsp_mkdir((s8*)path, 0660);
        if(fd < 0)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_OM, "create om dir failed! ret = %d\n", fd);
            return fd;
        }
    }

    return BSP_OK;
}

int om_open_dir(char *path)
{
    int fd;

    fd = om_create_dir(path);
    if(BSP_OK == fd)
    {
        fd = bsp_open((s8*)path, RFILE_RDONLY , 0);
    }

    return fd;
}

int om_close_dir(int fd)
{
    (void)bsp_close((u32)fd);

    return BSP_OK;
}


bool bsp_om_fs_check(void)
{
    int fd;

    fd = bsp_access((s8*)OM_ROOT_PATH, 0); //F_OK, 检查文件是否存在
    if(0 != fd)
    {
        /*bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_OM, "[BSP_OM]:bsp_om_fs_check file system is invalid\n");*/
        return false;
    }

    om_fetal("om fs is ready\n");


    return true;
}


int bsp_om_save_file(char *filename, void * address, u32 length, u32 mode)
{
    int ret = BSP_OK;
    int fd;
    u32 bytes;

    s32 flag;

    if(DUMP_SAVE_FILE_MODE_CREATE == mode)
    {
        flag = RFILE_CREAT|RFILE_WRONLY|RFILE_TRUNC;
    }
    else
    {
        flag = RFILE_CREAT|RFILE_RDWR|RFILE_APPEND;
    }
    fd = bsp_open((s8*)filename, flag, 0755);
    if(fd < 0)
    {
        om_error("<bsp_om_save_file>, open failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    bytes = (u32)bsp_write((u32)fd, address, length);
    if(bytes != length)
    {
        om_error("<bsp_om_save_file>, write data failed! ret = %d\n", bytes);
        ret = BSP_ERROR;
        (void)bsp_close((u32)fd);
        goto out;
    }

    ret = bsp_close((u32)fd);
    if(0 != ret)
    {
        om_error("<bsp_om_save_file>, close file failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    ret = BSP_OK;

out:

    return ret;
}

int bsp_om_append_file(char *filename, void * address, u32 length, u32 max_size)
{
    int ret = BSP_OK;
    int fd;
    u32 bytes;
    int len;


    ret = om_create_dir(OM_ROOT_PATH);
    if(BSP_OK != ret)
    {
        om_error("<bsp_om_append_file>, create dir failed! ret = %d\n", ret);
        goto out;
    }

    ret = bsp_access((s8*)filename, 0);
    if(BSP_OK != ret)
    {   
        /*create file */
        fd = bsp_open((s8*)filename, RFILE_CREAT|RFILE_RDWR, 0755);
        if(fd < 0)
        {
            om_error("<bsp_om_append_file>, open failed while mode is create, ret = %d\n", fd);
            goto out;
        }
    }
    else
    {
        fd = bsp_open((s8*)filename, RFILE_APPEND|RFILE_RDWR, 0755);
        if(fd < 0)
        {
            om_error("<bsp_om_append_file>, open failed while mode is append, ret = %d\n", fd);
            goto out;
        }

    }

    len = bsp_lseek((u32)fd, 0, SEEK_END);
    if(ERROR == len)
    {
        om_error("<bsp_om_append_file>, seek failed! ret = %d\n", len);
        goto out1;
    }

    if (len >= (int)max_size)
    {
        (void)bsp_close((u32)fd);
        ret = bsp_remove((s8*)filename);
        if (OK != ret)
        {
            om_error("<bsp_om_append_file>, remove failed! ret = %d\n", ret);
	        goto out;
        }

        /*重新建立reset文件*/
        fd = bsp_open((s8*)filename, RFILE_CREAT|RFILE_RDWR, 0755);
        if(fd < 0)
        {
            om_error("<bsp_om_append_file>, create failed! ret = %d\n", fd);
            goto out;
        }
    }

    bytes = (u32)bsp_write((u32)fd, address, length);
    if(bytes != length)
    {
        om_error("<bsp_om_append_file>, write data failed! ret = %d\n", bytes);
        ret = BSP_ERROR;
        goto out1;
    }

    (void)bsp_close((u32)fd);

    return BSP_OK;

out1:
    (void)bsp_close((u32)fd);
out:
    return ret;
}

void print2file(char* filename, char *fmt,...)
{
    va_list arglist = (va_list)NULL;/*lint !e40 !e522*/

	va_start(arglist, fmt);
    /*lint -save -e119*/
	vsnprintf(g_OmLogBuffer, 256, fmt, arglist); /* [false alarm]:屏蔽Fority错误 */
    /*lint -restore*/
	va_end(arglist);

    bsp_om_append_file(filename, (void*)g_OmLogBuffer, strlen(g_OmLogBuffer), OM_PRINT_LOG_MAX);

    return;
}


