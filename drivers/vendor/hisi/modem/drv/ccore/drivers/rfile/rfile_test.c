

#include "rfile_balong.h"
#include "osl_thread.h"
#include "drv_rfile.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __KERNEL__

#define rfile_print_info        printk

#else /* __VXWORKS__ */

#define rfile_print_info        printf

#endif /* end of __KERNEL__ */

#define RFILE_WR_DATA_LEN           8888
#define RFILE_RD_DATA_LEN           8880

#define DUAL_THREAD_WR_DATA_LEN     1000

s32 rfile_test_003_tmp1 = 0;
s32 rfile_test_003_tmp2 = 0;
struct task_struct *rfile_test_003_taskid1;
struct task_struct *rfile_test_003_taskid2;

s32 rfile_test_013_tmp1 = 0;
s32 rfile_test_013_tmp2 = 0;
struct task_struct *rfile_test_013_taskid1;
struct task_struct *rfile_test_013_taskid2;

s32 g_pfile;

/* 内部接口测试函数 */
/*************************************************************************
 函 数 名   : rfile_test_001
 功能描述   : 新建目录-->新建文件-->写入内容-->读取内容-->关闭文件-->获取文件状态
*************************************************************************/
s32 rfile_test_001()
{
    s32 ret, i;
    s8 *dirpar = "/rfiletest001";
    s8 *filepath = "/rfiletest001/rfile001.txt";
    s8 *wrbuf = "1234567890abcdefghijklmnopqrstuvwxyz";
    s8 *rfilewrtmp = NULL;
    s8 *rfilerdtmp = NULL;
    s8 rdbuf[64] = {0};
    s32 pfile;
    struct rfile_stat_stru buf = {0};

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : mkdir-->access-->fopen-->fseek-->fwrite-->fwrite-->close-->access\n");
    rfile_print_info("open-->ftell-->fseek-->ftell-->fread-->fread-->fclose-->stat.\n\n");

    rfilewrtmp = Rfile_Malloc(RFILE_WR_DATA_LEN);
    memset(rfilewrtmp, 'x', RFILE_WR_DATA_LEN);

    rfilerdtmp = Rfile_Malloc(RFILE_WR_DATA_LEN);
    memset(rfilerdtmp, 0, RFILE_WR_DATA_LEN);

    ret = bsp_mkdir(dirpar, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_001_fail_1;
    }

    /* 判断文件是否存在 */
    ret = bsp_access(filepath, 0);
    if(BSP_OK == ret)
    {
        rfile_print_info("!!!!!!!! bsp_access failed.\n");

        goto rfile_test_001_fail_1;
    }

    pfile = bsp_open(filepath, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");

        goto rfile_test_001_fail_1;
    }

    ret = bsp_lseek(pfile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_write(pfile, wrbuf, strlen(wrbuf));
    if(ret != strlen(wrbuf))
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_write(pfile, rfilewrtmp, RFILE_WR_DATA_LEN);
    if(ret != RFILE_WR_DATA_LEN)
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_close(pfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_001_fail_1;
    }

    /* 判断文件是否存在 */
    ret = bsp_access(filepath, 0);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_access failed.\n");

        goto rfile_test_001_fail_1;
    }

    pfile = bsp_open(filepath, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");

        goto rfile_test_001_fail_1;
    }

    ret = bsp_tell(pfile);
    if(ret < 0)
    {
        rfile_print_info("!!!!!!!! BSP_ftell 1 failed 0x%x.\n", ret);
    }

    ret = bsp_lseek(pfile, 0, SEEK_SET);
    if(ret < 0)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 2 failed .\n");
    }

    ret = bsp_tell(pfile);
    if(ret < 0)
    {
        rfile_print_info("!!!!!!!! BSP_ftell 2 failed 0x%x.\n", ret);
    }

    ret = bsp_read(pfile, rdbuf, strlen(wrbuf));
    if(ret != strlen(wrbuf))
    {
        rfile_print_info("!!!!!!!! BSP_fread 1 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_read(pfile, rfilerdtmp, RFILE_WR_DATA_LEN);
    if(ret != RFILE_WR_DATA_LEN)
    {
        rfile_print_info("!!!!!!!! BSP_fread 2 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_close(pfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_001_fail_1;
    }

    for(i = 0; i < strlen(wrbuf); i++)
    {
        if(rdbuf[i] != wrbuf[i])
        {
            rfile_print_info("!!!!!!!! read compare 1 failed .\n");
            
            goto rfile_test_001_fail_1;
        }
    }

    for(i = 0; i < RFILE_WR_DATA_LEN; i++)
    {
        if(rfilerdtmp[i] != rfilewrtmp[i])
        {
            rfile_print_info("!!!!!!!! read compare 2 failed .\n");
            
            goto rfile_test_001_fail_1;
        }
    }

    ret = bsp_stat(filepath, &buf);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_stat 1 failed.\n");

        goto rfile_test_001_fail_1;
    }
    else
    {
        rfile_print_info("BSP_stat : buf.mode %d, buf.mtime.tv_nsec %d, buf.blksize %d\n",
            buf.mode, buf.mtime.tv_nsec, buf.blksize);
    }

    ret = bsp_stat(dirpar, &buf);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_stat 2 failed.\n");

        goto rfile_test_001_fail_1;
    }
    else
    {
        rfile_print_info("BSP_stat : buf.mode %d, buf.mtime.tv_nsec %d, buf.blksize %d\n",
            buf.mode, buf.mtime.tv_nsec, buf.blksize);
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_001_fail_2:
    bsp_close(pfile);

rfile_test_001_fail_1:
    
    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_001_01
 功能描述   : 删除文件-->删除目录
*************************************************************************/
s32 rfile_test_001_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest001";
    s8 *filepath = "/rfiletest001/rfile001.txt";

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : remove-->rmdir.\n\n");

    ret = bsp_remove(filepath);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_remove failed.\n");

        goto rfile_test_001_01_fail_1;
    }

    ret = bsp_rmdir(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_rmdir failed.\n");

        goto rfile_test_001_01_fail_1;
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_001_01_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}



/*************************************************************************
 函 数 名   : rfile_test_002
 功能描述   : 新建目录-->新建目录-->打开目录-->读目录-->关闭目录
*************************************************************************/
s32 rfile_test_002()
{
    s32 ret;
    s8 *dirpar = "/rfiletest002";
    s8 *dirsub1 = "/rfiletest002/subdir1";
    s8 *dirsub2 = "/rfiletest002/subdir2";
    s8 *dirfile = "/rfiletest002/subfile";
    s32 dir;
    s8 data[1024] = {0};
    s32 i;
    RFILE_DIRENT_STRU *pstDirent;
    s32 pfile;

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : mkdir-->mksubdir1-->mksubdir2-->opendir-->readdir-->closedir.\n\n");

    ret = bsp_mkdir(dirpar, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_mkdir(dirsub1, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_mkdir(dirsub2, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_002_fail_1;
    }

    pfile = bsp_open(dirfile, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_close(pfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_002_fail_1;
    }

    dir = bsp_opendir(dirpar);
    if(dir < 0)
    {
        rfile_print_info("!!!!!!!! BSP_opendir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_readdir(dir, data, 1024);
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_readdir failed.\n");
    }
    else
    {
        for(i=0; i<ret; )
        {
            pstDirent = (RFILE_DIRENT_STRU*)(data + i);
            i += pstDirent->d_reclen;

            rfile_print_info("pst->d_name=%s!\n",pstDirent->d_name);
        }

    }

    ret = bsp_closedir(dir);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_closedir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = BSP_xcopy(dirpar, "/rfile_xcopy");
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_xcopy failed.\n");

        goto rfile_test_002_fail_1;
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_002_fail_2:
    ret = bsp_closedir(dir);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_closedir failed.\n");
    }

rfile_test_002_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_002_01
 功能描述   : 删除目录-->删除目录
*************************************************************************/
s32 rfile_test_002_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest002";
    s8 *dirsub1 = "/rfiletest002/subdir1";
    s8 *dirsub2 = "/rfiletest002/subdir2";
    s8 *dirfile = "/rfiletest002/subfile";

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : rmsubdir1-->rmsubdir2-->rmdir.\n\n");

    ret = bsp_remove(dirfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_remove failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    ret = bsp_rmdir(dirsub1);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    ret = bsp_rmdir(dirsub2);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    ret = bsp_rmdir(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_002_01_fail_1;
    }
    
    ret = BSP_xdelete("/rfile_xcopy");
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_xdelete failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_002_01_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}


s32 rfile_test_003_task1(void* obj)
{
    s32 ret;
    s8 *wrbuf = "12";
    s32 i;
    
    for(i = 0; i < DUAL_THREAD_WR_DATA_LEN; i++)
    {
        ret = bsp_write(g_pfile, wrbuf, strlen(wrbuf));
        if(ret <= 0)
        {
            rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

            rfile_test_003_tmp1 = 1;
            return 0;
        }
    }

    rfile_test_003_tmp1 = 2;

    return 0;
}

s32 rfile_test_003_task2(void* obj)
{
    s32 ret;
    s8 *wrbuf = "34";
    s32 i;

    for(i = 0; i < DUAL_THREAD_WR_DATA_LEN; i++)
    {
        ret = bsp_write(g_pfile, wrbuf, strlen(wrbuf));
        if(ret <= 0)
        {
            rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");
        
            rfile_test_003_tmp2 = 1;
            return 0;
        }
    }

    rfile_test_003_tmp2 = 2;

    return 0;
}

/*************************************************************************
 函 数 名   : rfile_test_003
 功能描述   : 多线程同时读写文件测试
*************************************************************************/
s32 rfile_test_003()
{
    s32 ret;
    s8 *dirpar = "/rfiletest003";
    s8 *filepath = "/rfiletest003/rfile001.txt";

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);

    ret = bsp_mkdir(dirpar, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_003_fail_1;
    }

    g_pfile = bsp_open(filepath, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(g_pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");

        goto rfile_test_003_fail_1;
    }

    ret = bsp_lseek(g_pfile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_003_fail_2;
    }
    
    rfile_test_003_tmp1 = 0;
    rfile_test_003_tmp2 = 0;

    rfile_test_003_taskid1 = kthread_run(rfile_test_003_task1, BSP_NULL, "rfile_test_003_task1");
    rfile_test_003_taskid2 = kthread_run(rfile_test_003_task2, BSP_NULL, "rfile_test_003_task2");

    while((0 == rfile_test_003_tmp1) || (0 == rfile_test_003_tmp2))
    {
        RFILE_SLEEP(1000);
    }

    if((1 == rfile_test_003_tmp1) || (1 == rfile_test_003_tmp2))
    {
        goto rfile_test_003_fail_2;
    }
    
    bsp_close(g_pfile);
    
    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_003_fail_2:

    bsp_close(g_pfile);
    
rfile_test_003_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_003_01
 功能描述   : 多线程同时读写文件中止测试
*************************************************************************/
s32 rfile_test_003_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest003";
    s8 *filepath = "/rfiletest003/rfile001.txt";
    s8 *rdbuf = BSP_NULL;
    s32 len;

    len = 4*DUAL_THREAD_WR_DATA_LEN + 10;

    rdbuf = Rfile_Malloc(len);
    memset(rdbuf, 0, len);

    g_pfile = bsp_open(filepath, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(g_pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");

        goto rfile_test_003_01_fail_1;
    }

    ret = bsp_lseek(g_pfile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_003_01_fail_2;
    }

    ret = bsp_read(g_pfile, rdbuf, (len-1));
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_fread 1 failed .\n");
    }

//    rfile_print_info("!!!!%s!!!!\n", rdbuf);

    Rfile_Free(rdbuf);

    /* 如果读出来的长度与两个线程写入的长度不相等，则用例失败 */
    if(ret != 4*DUAL_THREAD_WR_DATA_LEN)
    {
        goto rfile_test_003_01_fail_2;
    }

    bsp_close(g_pfile);

    ret = bsp_remove(filepath);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_remove failed.\n");

        goto rfile_test_003_01_fail_1;
    }

    ret = bsp_rmdir(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_rmdir failed.\n");

        goto rfile_test_003_01_fail_1;
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_003_01_fail_2:

    bsp_close(g_pfile);

rfile_test_003_01_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_004
 功能描述   : 压力测试
*************************************************************************/
s32 rfile_test_004(s32 x)
{
    s32 ret, i;

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);

    for(i = 0; i < x; i++)
    {
        ret = rfile_test_001();
        if(ret)
        {
            goto rfile_test_004_fail_1;
        }
        
        ret = rfile_test_001_01();
        if(ret)
        {
            goto rfile_test_004_fail_1;
        }
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_004_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return ret;
}

/*************************************************************************
 函 数 名   : rfile_test_005
 功能描述   : xcopy测试
*************************************************************************/
s32 rfile_test_005()
{
    s32 ret;
    s8 *dirpar = "/rfiletest005";
    s8 *dirsub1 = "/rfiletest005/subdir1";
    s8 *dirsub2 = "/rfiletest005/subdir2";
    s8 *dirfile = "/rfiletest005/subfile";
    s8 *dirsub1sub = "/rfiletest005/subdir1/sub";
    s8 *dirdest = "/rfiletest005dest";
    s8 *destfile = "/rfiletest005dest/subfile";
    s32 dir;
    s8 data[1024] = {0};
    s32 i;
    RFILE_DIRENT_STRU *pstDirent;
    s32 pfile;
    s8 *rfilewrtmp = NULL;
    s8 *rfilerdtmp = NULL;

    rfilewrtmp = Rfile_Malloc(RFILE_WR_DATA_LEN);
    memset(rfilewrtmp, 'x', RFILE_WR_DATA_LEN);

    rfilerdtmp = Rfile_Malloc(RFILE_WR_DATA_LEN);
    memset(rfilerdtmp, 0, RFILE_WR_DATA_LEN);

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : mkdir-->mksubdir1-->mksubdir2-->creat file-->mksubdir1sub-->xcopy.\n\n");

    ret = bsp_mkdir(dirpar, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir src failed.\n");
        return -1;
    }

    ret = bsp_mkdir(dirdest, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir dest failed.\n");
        return -1;
    }

    ret = bsp_mkdir(dirsub1, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");
        return -1;
    }

    ret = bsp_mkdir(dirsub2, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");
        return -1;
    }

    pfile = bsp_open(dirfile, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");
        return -1;
    }

    ret = bsp_write(pfile, rfilewrtmp, RFILE_WR_DATA_LEN);
    if(ret != RFILE_WR_DATA_LEN)
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");
        return -1;
    }

    ret = bsp_close(pfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");
        return -1;
    }

    ret = bsp_mkdir(dirsub1sub, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");
        return -1;
    }

    ret = BSP_xcopy(dirpar, dirdest);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_xcopy failed.\n");
        return -1;
    }

    pfile = bsp_open(destfile, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");
        return -1;
    }
    
    ret = bsp_lseek(pfile, 0, SEEK_SET);
    if(ret < 0)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 2 failed .\n");
        return -1;
    }
    
    ret = bsp_read(pfile, rfilerdtmp, RFILE_WR_DATA_LEN);
    if(ret != RFILE_WR_DATA_LEN)
    {
        rfile_print_info("!!!!!!!! BSP_fread 2 failed .\n");
        return -1;
    }
    
    ret = bsp_close(pfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");
        return -1;
    }
    
    for(i = 0; i < RFILE_WR_DATA_LEN; i++)
    {
        if(rfilerdtmp[i] != rfilewrtmp[i])
        {
            rfile_print_info("!!!!!!!! read compare 2 failed .\n");

            return -1;
        }
    }

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;
}


/*************************************************************************
 函 数 名   : rfile_test_005_01
 功能描述   : xdelete测试
*************************************************************************/
s32 rfile_test_005_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest005";
    s8 *dirdest = "/rfiletest005dest";

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : xdelete-->xdelete.\n\n");

    ret = BSP_xdelete(dirdest);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_xdelete dirdest failed.\n");
        return -1;
    }
    
    ret = BSP_xdelete(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_xdelete dirpar failed.\n");
        return -1;
    }

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;
}

/*************************************************************************
 函 数 名   : rfile_test_005
 功能描述   : bsp_mass_read 测试
*************************************************************************/
s32 rfile_test_006(s8 *test)
{
    s32 i;
    u8 *tmp;

    tmp = Rfile_Malloc(1024*2);
    
    if(0 != BSP_mass_read("logo", 0, 1024*2, tmp))
    {
        printf("rfile_test_006 BSP_mass_read 1 failed.\n");
        return -1;
    }
    else
    {
        for(i = 0; i< 1024*2; i++)
        {
            printf("0x%02x ", tmp[i]);
        }
        printf("\n");
    }

    memset(tmp, 0xa5, 1024*2);
    if(0 != BSP_mass_write("logo", 0, 1024*2, tmp))
    {
        printf("rfile_test_006 failed.\n");
        return -1;
    }
    else
    {
        for(i = 0; i< 1024*2; i++)
        {
            printf("0x%02x ", tmp[i]);
        }
        printf("\n");
    }
    
    memset(tmp, 0, 1024*2);

    for(i = 0; i< 1024*2; i++)
    {
        printf("0x%02x ", tmp[i]);
    }
    printf("\n");

    if(0 != BSP_mass_read("logo", 0, 1024*2, tmp))
    {
        printf("rfile_test_006 BSP_mass_read 1 failed.\n");
        return -1;
    }
    else
    {
        for(i = 0; i< 1024*2; i++)
        {
            printf("0x%02x ", tmp[i]);
        }
        printf("\n");
    }

    Rfile_Free(tmp);

    return 0;
}

/* 外部接口测试函数 */
/*************************************************************************
 函 数 名   : rfile_test_001
 功能描述   : 新建目录-->新建文件-->写入内容-->读取内容-->关闭文件-->获取文件状态
*************************************************************************/
s32 rfile_test_011()
{
    s32 ret;
    s8 *dirpar = "/rfiletest001";
    s8 *filepath = "/rfiletest001/rfile001.txt";
    s8 *wrbuf = "1234567890abcdefghijklmnopqrstuvwxyz";
    s8 *rfilewrtmp = NULL;
    s8 *rfilerdtmp = NULL;
    s8 rdbuf[64] = {0};
    FILE *pFile = 0;
    DRV_STAT_STRU buf = {0};

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : mkdir-->fopen-->fseek-->fwrite-->fwrite-->");
    rfile_print_info("ftell-->fseek-->ftell-->fread-->fread-->fclose-->stat.\n\n");

    rfilewrtmp = Rfile_Malloc(RFILE_WR_DATA_LEN);
    memset(rfilewrtmp, 'x', RFILE_WR_DATA_LEN);

    rfilerdtmp = Rfile_Malloc(RFILE_WR_DATA_LEN);
    memset(rfilerdtmp, 0, RFILE_WR_DATA_LEN);

    ret = DRV_FILE_MKDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_011_fail_1;
    }

    pFile = DRV_FILE_OPEN(filepath, "a+");
    if(pFile == 0)
    {
        rfile_print_info("!!!!!!!! BSP_fopen failed.\n");

        goto rfile_test_011_fail_1;
    }

    rfile_print_info("pFile %d.\n", pFile);

    ret = DRV_FILE_LSEEK(pFile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_011_fail_2;
    }

    ret = DRV_FILE_WRITE(wrbuf, 1, strlen(wrbuf), pFile);
    if(ret != strlen(wrbuf))
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

        goto rfile_test_011_fail_2;
    }

    ret = DRV_FILE_WRITE(rfilewrtmp, 1, RFILE_WR_DATA_LEN, pFile);
    if(ret != RFILE_WR_DATA_LEN)
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");

        goto rfile_test_011_fail_2;
    }

    rfile_print_info("BSP_fwrite %d bytes.\n", ret);

    for(ret = 0; ret < 28; ret++)
    {
        rfile_print_info("-0x%x-", rfilewrtmp[ret]);
    }

    rfile_print_info(".\n");

    for(ret = RFILE_RD_DATA_LEN; ret < RFILE_WR_DATA_LEN; ret++)
    {
        rfile_print_info("-0x%x-", rfilewrtmp[ret]);
    }

    rfile_print_info(".\n");

    ret = DRV_FILE_TELL(pFile);
    if(ret != (strlen(wrbuf)+RFILE_WR_DATA_LEN))
    {
        rfile_print_info("!!!!!!!! BSP_ftell 1 failed 0x%x.\n", ret);
        goto rfile_test_011_fail_2;
    }
    else
    {
        rfile_print_info("!!!!!!!! BSP_ftell 1 success 0x%x .\n", ret);
    }

    ret = DRV_FILE_CLOSE(pFile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_011_fail_1;
    }

    pFile = (s32)DRV_FILE_OPEN(filepath, "a+");
    if(pFile == 0)
    {
        rfile_print_info("!!!!!!!! BSP_fopen failed.\n");

        goto rfile_test_011_fail_1;
    }

    ret = DRV_FILE_LSEEK(pFile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 2 failed .\n");
        goto rfile_test_011_fail_2;
    }

    ret = DRV_FILE_TELL(pFile);
    if(ret != 0)
    {
        rfile_print_info("!!!!!!!! BSP_ftell 2 failed 0x%x.\n", ret);
        goto rfile_test_011_fail_2;
    }
    else
    {
        rfile_print_info("!!!!!!!! BSP_ftell 2 success 0x%x .\n", ret);
    }

    ret = (s32)DRV_FILE_READ(rdbuf, 1, strlen(wrbuf), pFile);
    if(ret != strlen(wrbuf))
    {
        rfile_print_info("!!!!!!!! BSP_fread 1 failed .\n");

        goto rfile_test_011_fail_2;
    }

    ret = (s32)DRV_FILE_READ(rfilerdtmp, 2, RFILE_WR_DATA_LEN, pFile);
    if(ret != (RFILE_WR_DATA_LEN/2))
    {
        rfile_print_info("!!!!!!!! BSP_fread 2 failed .\n");

        goto rfile_test_011_fail_2;
    }

    rfile_print_info("BSP_fread %d bytes.\n", ret);

    for(ret = 0; ret < 28; ret++)
    {
        rfile_print_info(" 0x%x ", rfilerdtmp[ret]);
    }

    rfile_print_info(".\n");

    for(ret = RFILE_RD_DATA_LEN; ret < RFILE_WR_DATA_LEN; ret++)
    {
        rfile_print_info(" 0x%x ", rfilerdtmp[ret]);
    }

    rfile_print_info(".\n");

    ret = DRV_FILE_CLOSE(pFile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_011_fail_1;
    }

    ret = DRV_FILE_STAT(filepath, &buf);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_stat failed.\n");

        goto rfile_test_011_fail_1;
    }
    else
    {
        rfile_print_info("BSP_stat : st_atime %d, st_mtime %d, st_ctime %d, buf.blksize %d\n",
            buf.st_atime, buf.st_mtime, buf.st_ctime, buf.st_blksize);
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_011_fail_2:
    DRV_FILE_CLOSE(pFile);

rfile_test_011_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_001_01
 功能描述   : 删除文件-->删除目录
*************************************************************************/
s32 rfile_test_011_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest001";
    s8 *filepath = "/rfiletest001/rfile001.txt";

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : remove-->rmdir.\n\n");

    ret = DRV_FILE_RMFILE(filepath);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_remove failed.\n");

        goto rfile_test_011_01_fail_1;
    }

    ret = DRV_FILE_RMDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_rmdir failed.\n");

        goto rfile_test_011_01_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_011_01_fail_1:

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}



/*************************************************************************
 函 数 名   : rfile_test_002
 功能描述   : 新建目录-->新建目录-->打开目录-->读目录-->关闭目录
*************************************************************************/
s32 rfile_test_012()
{
    s32 ret;
    s8 *dirpar = "/rfiletest002";
    s8 *dirsub1 = "/rfiletest002/subdir1";
    s8 *dirsub2 = "/rfiletest002/subdir2";
    s32 dir;
    s8 data[1024] = {0};
    s32 i;
    RFILE_DIRENT_STRU *pstDirent;
    DRV_DIRENT_STRU* pdirtmp;

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : mkdir-->mksubdir1-->mksubdir2-->opendir-->readdir-->closedir.\n\n");

    ret = DRV_FILE_MKDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_012_fail_1;
    }

    ret = DRV_FILE_MKDIR(dirsub1);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_012_fail_1;
    }

    ret = DRV_FILE_MKDIR(dirsub2);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_012_fail_1;
    }

    dir = DRV_FILE_OPENDIR(dirpar);
    if(dir < 0)
    {
        rfile_print_info("!!!!!!!! BSP_opendir failed.\n");

        goto rfile_test_012_fail_1;
    }

    pdirtmp = DRV_FILE_READDIR(dir);
    if(pdirtmp == BSP_NULL)
    {
        rfile_print_info("!!!!!!!! BSP_readdir 1 failed.\n");
        goto rfile_test_012_fail_2;
    }
    else
    {
        rfile_print_info("pst->d_name=%s!\n",pdirtmp->d_name);
    }

    pdirtmp = DRV_FILE_READDIR(dir);
    if(pdirtmp == BSP_NULL)
    {
        rfile_print_info("!!!!!!!! BSP_readdir 2 failed.\n");
        goto rfile_test_012_fail_2;
    }
    else
    {
        rfile_print_info("pst->d_name=%s!\n",pdirtmp->d_name);
    }

    pdirtmp = DRV_FILE_READDIR(dir);
    if(pdirtmp == BSP_NULL)
    {
        rfile_print_info("!!!!!!!! BSP_readdir 3 failed.\n");
        goto rfile_test_012_fail_2;
    }
    else
    {
        rfile_print_info("pst->d_name=%s!\n",pdirtmp->d_name);
    }

    pdirtmp = DRV_FILE_READDIR(dir);
    if(pdirtmp == BSP_NULL)
    {
        rfile_print_info("!!!!!!!! BSP_readdir 4 failed.\n");
        goto rfile_test_012_fail_2;
    }
    else
    {
        rfile_print_info("pst->d_name=%s!\n",pdirtmp->d_name);
    }

    pdirtmp = DRV_FILE_READDIR(dir);
    if(pdirtmp == BSP_NULL)
    {
        rfile_print_info("!!!!!!!! BSP_readdir 5 failed.\n");
    }
    else
    {
        rfile_print_info("pst->d_name=%s!\n",pdirtmp->d_name);
    }

    ret = DRV_FILE_CLOSEDIR(dir);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_closedir failed.\n");

        goto rfile_test_012_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_012_fail_2:
    ret = DRV_FILE_CLOSEDIR(dir);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_closedir failed.\n");
    }

rfile_test_012_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_002_01
 功能描述   : 删除目录-->删除目录
*************************************************************************/
s32 rfile_test_012_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest002";
    s8 *dirsub1 = "/rfiletest002/subdir1";
    s8 *dirsub2 = "/rfiletest002/subdir2";

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : rmsubdir1-->rmsubdir2-->rmdir.\n\n");

    ret = DRV_FILE_RMDIR(dirsub1);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_012_01_fail_1;
    }

    ret = DRV_FILE_RMDIR(dirsub2);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_012_01_fail_1;
    }

    ret = DRV_FILE_RMDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_012_01_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_012_01_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}


#define DUAL_THREAD_WR_DATA_SIZE_1301    6*1024
#define DUAL_THREAD_WR_DATA_TIME_1301    25
#define DUAL_THREAD_WR_DATA_SIZE_1302    2
#define DUAL_THREAD_WR_DATA_TIME_1302    100


s32 rfile_test_013_task1(void* obj)
{
    s32 ret;
    s8 *wrbuf;
    s32 i;

    wrbuf = malloc(DUAL_THREAD_WR_DATA_SIZE_1301+1);
    if(NULL == wrbuf)
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 1 malloc failed .\n");

        rfile_test_013_tmp1 = 1;
        return 0;
    }

    memset(wrbuf, 0x30, DUAL_THREAD_WR_DATA_SIZE_1301);
    wrbuf[DUAL_THREAD_WR_DATA_SIZE_1301] = 0;
    
    for(i = 0; i < DUAL_THREAD_WR_DATA_TIME_1301; i++)
    {
        ret = DRV_FILE_WRITE(wrbuf, DUAL_THREAD_WR_DATA_SIZE_1301, 1, g_pfile);
        if(ret <= 0)
        {
            rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

            rfile_test_013_tmp1 = 1;
            return 0;
        }
    }

    rfile_test_013_tmp1 = 2;

    return 0;
}


s32 rfile_test_013_task2(void* obj)
{
    s32 ret;
    s8 *wrbuf = "34";
    s32 i;

    for(i = 0; i < DUAL_THREAD_WR_DATA_TIME_1302; i++)
    {
        ret = DRV_FILE_WRITE(wrbuf, DUAL_THREAD_WR_DATA_SIZE_1302, 1, g_pfile);
        if(ret <= 0)
        {
            rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");
        
            rfile_test_013_tmp2 = 1;
            return 0;
        }
    }

    rfile_test_013_tmp2 = 2;

    return 0;
}

/*************************************************************************
 函 数 名   : rfile_test_003
 功能描述   : 多线程同时读写文件测试
*************************************************************************/
s32 rfile_test_013()
{
    s32 ret;
    s8 *dirpar = "/rfiletest003";
    s8 *filepath = "/rfiletest003/rfile001.txt";

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);

    ret = DRV_FILE_MKDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_013_fail_1;
    }

    g_pfile = DRV_FILE_OPEN(filepath, "a+");
    if(g_pfile == 0)
    {
        rfile_print_info("!!!!!!!! BSP_fopen failed.\n");

        goto rfile_test_013_fail_1;
    }

    ret = DRV_FILE_LSEEK(g_pfile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_013_fail_2;
    }

    rfile_test_013_tmp1 = 0;
    rfile_test_013_tmp2 = 0;

    rfile_test_013_taskid1 = kthread_run(rfile_test_013_task1, BSP_NULL, "rfile_test_013_task1");
    rfile_test_013_taskid2 = kthread_run(rfile_test_013_task2, BSP_NULL, "rfile_test_013_task2");

    while((0 == rfile_test_003_tmp1) || (0 == rfile_test_003_tmp2))
    {
        RFILE_SLEEP(1000);
    }

    if((1 == rfile_test_003_tmp1) || (1 == rfile_test_003_tmp2))
    {
        goto rfile_test_013_fail_2;
    }
    
    DRV_FILE_CLOSE(g_pfile);
    
    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_013_fail_2:

    DRV_FILE_CLOSE(g_pfile);
    
rfile_test_013_fail_1:

    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_003_01
 功能描述   : 多线程同时读写文件中止测试
*************************************************************************/
s32 rfile_test_013_01()
{
    s32 ret;
    s8 *dirpar = "/rfiletest003";
    s8 *filepath = "/rfiletest003/rfile001.txt";
    long len, size;

    g_pfile = DRV_FILE_OPEN(filepath, "a+");
    if(g_pfile < 0)
    {
        rfile_print_info("!!!!!!!! DRV_FILE_OPEN failed.\n");

        goto rfile_test_013_01_fail_1;
    }

    ret = DRV_FILE_LSEEK(g_pfile, 0, SEEK_END);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_013_01_fail_2;
    }

    len = DRV_FILE_TELL(g_pfile);

    size = (DUAL_THREAD_WR_DATA_SIZE_1301 * DUAL_THREAD_WR_DATA_TIME_1301)
        + (DUAL_THREAD_WR_DATA_SIZE_1302 * DUAL_THREAD_WR_DATA_TIME_1302);
    
    /* 如果读出来的长度与两个线程写入的长度不相等，则用例失败 */
    if(len != size)
    {
        rfile_print_info("!!!!!!!! failed len %d != size %d.\n", len, size);
        
        goto rfile_test_013_01_fail_2;
    }

    DRV_FILE_CLOSE(g_pfile);

    ret = DRV_FILE_RMFILE(filepath);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_remove failed.\n");

        goto rfile_test_013_01_fail_1;
    }

    ret = DRV_FILE_RMDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_rmdir failed.\n");

        goto rfile_test_013_01_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_013_01_fail_2:

    DRV_FILE_CLOSE(g_pfile);

rfile_test_013_01_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}


s32 rfile_test_017()
{
    s32 ret;
    s8 *dirpar = "/rfiletest017";
    s8 *fileold = "/rfiletest017/rfile001.txt";
    s8 *filenew = "/rfiletest017/rfile001_new.txt";
    s8 *wrbuf = "1234567890abcdefghijklmnopqrstuvwxyz";
    FILE *pFile = 0;

    rfile_print_info("====================================== [%s] start =====================================\n\n", 
        __FUNCTION__);
    rfile_print_info("test for : mkdir-->fopen-->close-->rename\n");

    ret = DRV_FILE_MKDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_fail_1;
    }

    pFile = DRV_FILE_OPEN(fileold, "a+");
    if(pFile == 0)
    {
        rfile_print_info("!!!!!!!! BSP_fopen failed.\n");

        goto rfile_test_fail_1;
    }

    rfile_print_info("pFile %d.\n", pFile);

    ret = DRV_FILE_LSEEK(pFile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_fail_2;
    }

    ret = DRV_FILE_WRITE(wrbuf, 1, strlen(wrbuf), pFile);
    if(ret != strlen(wrbuf))
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

        goto rfile_test_fail_2;
    }

    ret = DRV_FILE_CLOSE(pFile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! DRV_FILE_CLOSE failed.\n");

        goto rfile_test_fail_1;
    }

    ret = DRV_FILE_RENAME(fileold, filenew);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! DRV_FILE_RENAME failed.\n");

        goto rfile_test_fail_1;
    }

    rfile_print_info("====================================== [%s] success ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return 0;

rfile_test_fail_2:
    ret = DRV_FILE_CLOSE(pFile);

rfile_test_fail_1:
    
    rfile_print_info("====================================== [%s] failure ===================================\n", 
        __FUNCTION__);

    rfile_print_info("====================================== [%s] end =======================================\n", 
        __FUNCTION__);

    return -1;
}

/* 创建新文件，写入20个字节的内容，修改其中的10到15字节 */
s32 rfile_test_018()
{
    s32 ret;
    s8 *dirpar = "/rfiletest018";
    s8 *filepath = "/rfiletest018/rfile018.txt";
    s8 *rfilewrtmp = NULL;
    s8 *rfilerdtmp = NULL;
    s8 rdbuf[64] = {0};
    FILE *pFile = 0;
    DRV_STAT_STRU buf = {0};

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : mkdir-->fopen-->fseek-->fwrite-->fclose-->fopen-->fseek-->fwrite-->fclose.\n\n");

    ret = DRV_FILE_MKDIR(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_018_fail_1;
    }

    pFile = DRV_FILE_OPEN(filepath, "a+");
    if(pFile == 0)
    {
        rfile_print_info("!!!!!!!! BSP_fopen failed.\n");

        goto rfile_test_018_fail_1;
    }

    rfile_print_info("pFile %d.\n", pFile);

    ret = DRV_FILE_LSEEK(pFile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_018_fail_2;
    }

    ret = DRV_FILE_WRITE("01234567890123456789", 1, strlen("01234567890123456789"), pFile);
    if(ret != strlen("01234567890123456789"))
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

        goto rfile_test_018_fail_2;
    }

    rfile_print_info("BSP_fwrite %d bytes.\n", ret);

    ret = DRV_FILE_CLOSE(pFile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_018_fail_1;
    }

    pFile = (s32)DRV_FILE_OPEN(filepath, "a+");
    if(pFile == 0)
    {
        rfile_print_info("!!!!!!!! BSP_fopen failed.\n");

        goto rfile_test_018_fail_1;
    }

    ret = DRV_FILE_LSEEK(pFile, 10, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 2 failed .\n");
        goto rfile_test_018_fail_2;
    }

    ret = DRV_FILE_WRITE("abcde", 1, strlen("abcde"), pFile);
    if(ret != strlen("abcde"))
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");

        goto rfile_test_018_fail_2;
    }

    ret = DRV_FILE_CLOSE(pFile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_018_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_018_fail_2:
    DRV_FILE_CLOSE(pFile);

rfile_test_018_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


