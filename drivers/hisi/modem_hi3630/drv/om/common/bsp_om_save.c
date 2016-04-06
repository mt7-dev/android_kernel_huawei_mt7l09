
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "bsp_memmap.h"
#include "bsp_om_api.h"
#include "bsp_om_save.h"
#include "bsp_dump_def.h"
#include "bsp_dump_drv.h"

struct linux_dirent {
	unsigned long	d_ino;
	unsigned long	d_off;
	unsigned short	d_reclen;
	char		    d_name[1];
};

typedef struct {
    char *s;        /* The user's output buffer */
    int nmax;       /* The length of the user's output buffer */
    int length;     /* The current length of the string in s[] */
} print_data_t;

static char g_OmLogBuffer[256] = {0x00};

#ifndef BSP_CONFIG_HI3630
char g_ResetLogBuffer[256] = {0x00};
extern dump_global_info_t  *g_dump_global_info;
#endif

void om_trace(char *buffer, char *fmt,...)
{
    /*lint -save -e40 -e522 */
    va_list arglist;
    /*lint -restore +e40 +e522 */
	va_start(arglist, fmt);
	vsnprintf(buffer, 256, fmt, arglist); /* [false alarm]:屏蔽Fortify错误 */
	va_end(arglist);

    return;
}

int om_create_dir(char *path)
{
    int fd;

    /* 如果文件夹不存在，创建新文件夹*/
    fd = sys_access(path, 0); //F_OK, 检查文件是否存在
    if(0 != fd)
    {
        fd  = sys_mkdir(path, 0660);
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
        fd = sys_open(path, O_RDONLY| O_DIRECTORY, 0);
    }

    return fd;
}

int om_close_dir(int fd)
{
    sys_close(fd);

    return BSP_OK;
}

int om_clear_old_file(int fd, char * header)
{
    int ret = BSP_OK;
    int i;
    int index;
    int head_len;
    int read_bytes;
    char *buf = BSP_NULL;
    struct linux_dirent *dir;
    char filename[OM_DUMP_FILE_MAX_NUM][OM_DUMP_FILE_NAME_LENGTH] = {{0},{0}};
    char temp[OM_DUMP_FILE_NAME_LENGTH];

    buf = kmalloc(1024, GFP_KERNEL);
    if(BSP_NULL == buf)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_OM, "om_clear_old_file: Alloc mem error!");
        return BSP_ERROR;
    }

    read_bytes = sys_getdents(fd, (struct linux_dirent *)buf, 1024);
    if(-1 == read_bytes)
    {
        /* 读取文件夹错误 */
        om_error("<om_clear_old_file>, dents error!\n");
        ret = BSP_ERROR;
        goto out;
    }

    if(0 == read_bytes)
    {
        /* 文件夹是空的，直接返回OK */
        ret = BSP_OK;
        goto out;
    }

    /*轮询文件夹*/
    head_len = strlen(header);
    for(i=0; i<read_bytes; )
    {
        dir = (struct linux_dirent *)(buf + i);
        i += (int)dir->d_reclen;

        /* 删除旧的和错误的文件 */
        if(0 == strncmp ((char *)dir->d_name, header, head_len))
        {
            strncpy(temp, OM_ROOT_PATH, OM_DUMP_FILE_NAME_LENGTH-1);
            strncat(temp, dir->d_name, OM_DUMP_FILE_NAME_LENGTH-strlen(OM_ROOT_PATH)-1);

            index = simple_strtol(dir->d_name + head_len, NULL, 0);
            // 如果索引号超过最大值，或者有重复，直接删除文件
            if((index >= OM_DUMP_FILE_MAX_NUM - 1) || (0 != filename[index][0]))
            {
                sys_unlink(temp);
            }
            else
            {
                strncpy(filename[index], temp, OM_DUMP_FILE_NAME_LENGTH -1);
            }
        }
    }

    /* 文件重命名 */
    for(i=OM_DUMP_FILE_MAX_NUM-2; i>=0; i--)
    {
        if(filename[i][0])
        {
            snprintf(temp, sizeof(temp), "%s%s%02d.bin", OM_ROOT_PATH, OM_DUMP_HEAD, i+1);
            /* coverity[check_return] */
            (void)sys_rename(filename[i], temp);
        }
    }

out:
    if(buf)
        kfree(buf);

    return ret;
}


bool bsp_om_fs_check(void)
{
    int fd;
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = sys_access(OM_ROOT_PATH, 0); //F_OK, 检查文件是否存在
    if(0 != fd)
    {
        /*bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_OM, "[BSP_OM]:bsp_om_fs_check file system is invalid\n");*/
        set_fs(old_fs);
        return false;
    }

    om_fetal("om fs is ready\n");
    set_fs(old_fs);

    return true;
}


int bsp_om_save_loop_file(char * dirName, char *fileHeader, void * address, u32 length)
{
    int ret = BSP_OK;
    int fd;
    int bytes;
    mm_segment_t old_fs;
    char newFileName[OM_DUMP_FILE_NAME_LENGTH] = {0};

    /* 进入目录 */
    if (NULL == dirName || NULL == fileHeader)
    {
        om_error("<bsp_om_save_loop_file>, file name NULL!\n");
        return BSP_ERROR;
    }
    if ((strlen((const char*)dirName) + strlen((const char*)fileHeader)) >= OM_DUMP_FILE_NAME_LENGTH)
    {
        om_error("<bsp_om_save_loop_file>, file name too long!\n");
        return BSP_ERROR;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = om_open_dir(dirName);
    if(fd < 0)
    {
        om_error("<bsp_om_save_loop_file>, open om dir failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    ret = om_clear_old_file(fd, fileHeader);
    if(BSP_OK != ret)
    {
        om_error("<bsp_om_save_loop_file>, clear old file failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    om_close_dir(fd);

    //新文件名:0
    memset(newFileName, 0, sizeof(newFileName));
    snprintf(newFileName, sizeof(newFileName), "%s%s%02d.bin", dirName, fileHeader, 0);
    fd = sys_creat(newFileName, 0755);
    if(fd < 0)
    {
        om_error("<bsp_om_save_loop_file>, creat file failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    bytes = sys_write(fd, address, length);

    if(bytes != length)
    {
        om_error("<bsp_om_save_loop_file>, write data to file failed! ret = %d\n", bytes);
        ret = BSP_ERROR;
        (void)sys_close(fd);
        goto out;
    }

     ret = sys_close(fd);
    if(0 != ret)
    {
        om_error("<bsp_om_save_loop_file>, close file failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    ret = BSP_OK;

out:
    set_fs(old_fs);
    return ret;
}

int bsp_om_save_file(char *filename, void * address, u32 length, u32 mode)
{
    int ret = BSP_OK;
    int fd;
    int bytes;
    mm_segment_t old_fs;
    umode_t flag;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    if(DUMP_SAVE_FILE_MODE_CREATE == mode)
    {
        flag = O_CREAT|O_WRONLY|O_TRUNC;
    }
    else
    {
        flag = O_CREAT|O_RDWR|O_APPEND;
    }
    fd = sys_open(filename, flag, 0755);
    if(fd < 0)
    {
        om_error("<bsp_om_save_file>, open failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    bytes = sys_write(fd, address, length);
    if(bytes != length)
    {
        om_error("<bsp_om_save_file>, write data failed! ret = %d\n", bytes);
        ret = BSP_ERROR;
        (void)sys_close(fd);
        goto out;
    }

    ret = sys_close(fd);
    if(0 != ret)
    {
        om_error("<bsp_om_save_file>, close file failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    ret = BSP_OK;

out:
    set_fs(old_fs);
    return ret;
}

int bsp_om_append_file(char *filename, void * address, u32 length, u32 max_size)
{
    int ret = BSP_OK;
    int fd;
    int bytes;
    int len;
    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    ret = om_create_dir(OM_ROOT_PATH);
    if(BSP_OK != ret)
    {
        om_error("<bsp_om_append_file>, create dir failed! ret = %d\n", ret);
        goto out;
    }

    /* open file */
    ret = sys_access(filename, 0);
    if(BSP_OK != ret)
    {
        /*create file */
        fd = sys_open(filename, O_CREAT|O_RDWR, 0755);
        if(fd < 0)
        {
            om_error("<bsp_om_append_file>, open failed while mode is create, ret = %d\n", fd);
            goto out;
        }
    }
    else
    {
        fd = sys_open(filename, O_APPEND|O_RDWR, 0755);
        if(fd < 0)
        {
            om_error("<bsp_om_append_file>, open failed while mode is append, ret = %d\n", fd);
            goto out;
        }
    }

    len = sys_lseek(fd, 0, SEEK_END);
    if(ERROR == len)
    {
        om_error("<bsp_om_append_file>, seek failed! ret = %d\n", len);
        (void)sys_close(fd);
        goto out;
    }

    if (len >= max_size)
    {
        sys_close(fd);
        ret = sys_unlink(filename);
        if (OK != ret)
        {
            om_error("<bsp_om_append_file>, remove failed! ret = %d\n", ret);
	        goto out;
        }

        /*重新建立reset文件*/
        fd = sys_open(filename, O_CREAT|O_RDWR, 0755);
        if(fd < 0)
        {
            om_error("<bsp_om_append_file>, create failed! ret = %d\n", fd);
            goto out;
        }
    }

    bytes = sys_write(fd, address, length);
    if(bytes != length)
    {
        om_error("<bsp_om_append_file>, write data failed! ret = %d\n", bytes);
        ret = BSP_ERROR;
        (void)sys_close(fd);
        goto out;
    }

    ret = sys_close(fd);
    if(0 != ret)
    {
        om_error("<bsp_om_append_file>, close failed! ret = %d\n", ret);
        ret = BSP_ERROR;
        goto out;
    }

    ret = BSP_OK;

out:
    set_fs(old_fs);
    return ret;
}

#ifndef BSP_CONFIG_HI3630
void bsp_om_record_resetlog(void)
{
    s8 *reason;
    s8 *core;
    dump_base_info_t *base_info;

#define NORMAL_EXCH         "Software"
#define ARM_EXCH            "ARM"
#define PMU_EXCH            "PMU"
#define WDT_EXCH            "WDT"
#define CRASH_EXCH          "CRASH"
#define UNDEF_EXCH          "UNDEF"
#define NORMAL_POWER_ON     "NORMAL POWER ON"
#define NORMAL_RESET        "NORMAL RESET"
#define UNKNOW              "UNKNOW"

#define CCORE_REBOOT        "C CORE "
#define ACORE_REBOOT        "A CORE "
#define MCORE_REBOOT        "M CORE "
#define UNKNOW_CORE_REBOOT  "CORE UNKNOW "

    if((DUMP_START_EXCH == g_dump_global_info->internal_info.app_internal.start_flag)
        || (DUMP_START_EXCH == g_dump_global_info->internal_info.comm_internal.start_flag)
        || (DUMP_START_EXCH == g_dump_global_info->internal_info.m3_internal.start_flag)
        || (DUMP_START_REBOOT == g_dump_global_info->internal_info.app_internal.start_flag)
        || (DUMP_START_REBOOT == g_dump_global_info->internal_info.comm_internal.start_flag)
        || (DUMP_START_REBOOT == g_dump_global_info->internal_info.m3_internal.start_flag))
    {
        /* 先检查ACORE */
        if(DUMP_REASON_NORMAL == g_dump_global_info->reboot_reason)
        {
            reason = NORMAL_EXCH;
        }
        else if(DUMP_REASON_WDT == g_dump_global_info->reboot_reason)
        {
            reason = WDT_EXCH;
        }
        else if(DUMP_REASON_ARM == g_dump_global_info->reboot_reason)
        {
            reason = ARM_EXCH;
        }
        else if(DUMP_REASON_PMU== g_dump_global_info->reboot_reason)
        {
            reason = PMU_EXCH;
        }
        else if(DUMP_REASON_REBOOT == g_dump_global_info->reboot_reason)
        {
            reason = NORMAL_RESET;
        }
        else if(DUMP_REASON_UNDEF == g_dump_global_info->reboot_reason)
        {
            reason = UNDEF_EXCH;
        }
        else
        {
            reason = UNKNOW;
        }

        if(DUMP_CPU_COMM == g_dump_global_info->reboot_cpu)
        {
            core = CCORE_REBOOT;
            base_info = (dump_base_info_t*)(DUMP_COMM_SECTION_ADDR + DUMP_MAP_SIZE);
        }
        else if(DUMP_CPU_APP == g_dump_global_info->reboot_cpu)
        {
            core = ACORE_REBOOT;
            base_info = (dump_base_info_t*)DUMP_BASE_INFO_ADDR;
        }
        else if(DUMP_CPU_M3 == g_dump_global_info->reboot_cpu)
        {
             core = MCORE_REBOOT;
            base_info = (dump_base_info_t*)(DUMP_M3_SECTION_ADDR + DUMP_MAP_SIZE);
        }
        else
        {
            core = UNKNOW_CORE_REBOOT;
            base_info = NULL;
        }
    }
    else if((DUMP_START_CRASH == g_dump_global_info->internal_info.app_internal.start_flag)
        && (DUMP_START_CRASH == g_dump_global_info->internal_info.comm_internal.start_flag))
    {
        reason = CRASH_EXCH;
        core = UNKNOW_CORE_REBOOT;
        base_info = NULL;
    }
    else
    {
        // 正常上电。
        reason = NORMAL_POWER_ON;
        core = NULL;
        base_info = NULL;
    }

    if(NULL == core)
    {
        (void)om_trace((char*)g_ResetLogBuffer, "system reboot reason: %s %s\n",reason, "");
        om_fetal("system reboot reason: %s %s\r\n",reason, "");
    }
    else
    {
        if(NULL == base_info)
        {

            (void)om_trace((char*)g_ResetLogBuffer,"system reboot reason: %s %s\r\n", reason, core);
            om_fetal("system reboot reason: %s %s\r\n", reason, core);
        }
        else
        {

            (void)om_trace((char*)g_ResetLogBuffer,
                            "system reboot reason: %s %s, tick: %d, systemError para: ModId=0x%x, Arg1=%d, Arg2=%d \r\n",
                            reason, core,
                            g_dump_global_info->reboot_time,
                            base_info->modId,
                            base_info->arg1,
                            base_info->arg2);
            om_fetal("system reboot reason: %s %s, tick: %d, systemError para: ModId=0x%x, Arg1=%d, Arg2=%d \r\n",
                            reason, core,
                            g_dump_global_info->reboot_time,
                            base_info->modId,
                            base_info->arg1,
                            base_info->arg2);
        }
    }

    return;
}

void bsp_om_save_resetlog(void)
{
    om_fetal("save reset log:%s\n", g_ResetLogBuffer);
    bsp_om_append_file(OM_RESET_LOG, (void*)g_ResetLogBuffer, strlen(g_ResetLogBuffer), OM_RESET_LOG_MAX);

    return;
}
#endif

void print2file(char* filename, char *fmt,...)
{
    /*lint -save -e40 -e522 */
    va_list arglist;
    /*lint -restore +e40 +e522 */
	va_start(arglist, fmt);
	vsnprintf(g_OmLogBuffer, 256, fmt, arglist); /* [false alarm]:屏蔽Fortify错误 */
	va_end(arglist);

    bsp_om_append_file(filename, (void*)g_OmLogBuffer, strlen(g_OmLogBuffer), OM_PRINT_LOG_MAX);

    return;
}

