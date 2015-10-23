/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: drv_rfile.h(come from v9r1 FileSysInterface.h)                  */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-07                                                             */
/*                                                                           */
/* Description: Announcement of File System Interface.                       */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-09-18                                                       */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/* 2. Date: 2013-04-02                                                       */
/*    Modification: modify for v7r2                                          */
/*                                                                           */
/*****************************************************************************/
#ifndef __DRV_RFILE_H__
#define __DRV_RFILE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
 Balong协议栈封装的宏定义
*****************************************************************************/


#define DRV_S_IFDIR             0x4000          /* directory */

#define DRV_NAME_MAX            255             /* max length of the file name*/

#define DRV_TIME                unsigned long   /* type for file time fields */

#define DRV_ERROR               (-1)            /* return status values */

#define DRV_FILE_NULL            0              /* empty file*/

#define DRV_O_RDONLY            0               /* +1 == FREAD */
#define DRV_O_WRONLY            1               /* +1 == FWRITE */
#define DRV_O_RDWR              2               /* +1 == FREAD|FWRITE */
#define DRV_O_CREAT             0x0200          /* open with file create */
#define DRV_O_APPEND            0x0008          /* append (writes guaranteed at the end) */
#define DRV_O_TRUNC             0x0400          /* open with truncation */
#define DRV_O_EXCL              0x0800          /* error on open if file exists */
#define DRV_O_NONBLOCK          0x4000          /* non blocking I/O (POSIX style) */
#define DRV_O_NOCTTY            0x8000          /* don't assign a ctty on this open */

#define DRV_SEEK_SET            0               /* absolute offset, was L_SET */
#define DRV_SEEK_CUR            1               /* relative to current offset, was L_INCR */
#define DRV_SEEK_END            2               /* relative to end of file, was L_XTND */

/*****************************************************************************
 Balong协议栈封装的结构体定义
*****************************************************************************/

#ifdef __KERNEL__
#ifndef FILE
typedef int FILE;
#endif
#endif
typedef int                 DRV_STATUS;
typedef unsigned int        DRV_SIZE_T;
typedef long                DRV_OFF_T;
typedef	unsigned long       DRV_INO_T;
/* dirent */
typedef struct
{
    DRV_INO_T           d_ino;                      /* file serial number */
    char                d_name [DRV_NAME_MAX + 1];  /* file name, null-terminated */
}DRV_DIRENT_STRU;

/* Directory descriptor */
typedef struct
{
    int                 dd_fd;                      /* file descriptor for open directory */
    int                 dd_cookie;                  /* filesys-specific marker within dir */
    int                 dd_eof;                     /* readdir EOF flag */
    DRV_DIRENT_STRU     dd_dirent;                  /* obtained directory entry */
}DRV_DIR_STRU;

/* 由于从vxwork6.2换到 vxwork6.8 ，stat结构有所变化 */
typedef struct
{
    unsigned long       st_dev;     /* Device ID number */
    unsigned long       st_ino;     /* File serial number */
    int                 st_mode;    /* Mode of file */
    unsigned long       st_nlink;   /* Number of hard links to file */
    unsigned short      st_uid;     /* User ID of file */
    unsigned short      st_gid;     /* Group ID of file */
    unsigned long       st_rdev;    /* Device ID if special file */
    signed long long    st_size;    /* File size in bytes */
    DRV_TIME            st_atime;   /* Time of last access */
    DRV_TIME            st_mtime;   /* Time of last modification */
    DRV_TIME            st_ctime;   /* Time of last status change */
    long                st_blksize; /* File system block size */
    unsigned long       st_blocks;  /* Number of blocks containing file */
    unsigned char       st_attrib;  /* DOSFS only - file attributes */
    int                 st_reserved1;  /* reserved for future use */
    int                 st_reserved2;  /* reserved for future use */
    int                 st_reserved3;  /* reserved for future use */
    int                 st_reserved4;  /* reserved for future use */
}DRV_STAT_STRU;

/*****************************************************************************
 Balong协议栈封装的函数声明
*****************************************************************************/


/*文件系统接口*/
extern void *BSP_fopen(const char *path, const char *mode);
extern int BSP_fclose(void *fp);
extern int BSP_fread(void *ptr, unsigned int size, unsigned int number, void *stream);
extern int BSP_fwrite(void *ptr, unsigned int size, unsigned int number, void *stream);
extern int BSP_fseek(void *stream, long offset, int whence);
extern long BSP_ftell(void *stream);
extern int BSP_remove(const char *pathname);
extern int BSP_mkdir(const char *dirName);
extern int BSP_rmdir(const char *path);
extern DRV_DIR_STRU* BSP_opendir(const char *dirName);
extern DRV_DIRENT_STRU* BSP_readdir(DRV_DIR_STRU *dirp);
extern int BSP_closedir(DRV_DIR_STRU *dirp);
extern int BSP_stat(const char *path, DRV_STAT_STRU*buf);
extern unsigned long BSP_errnoGet(void);
extern int BSP_access(const char *path, int amode);
extern int BSP_xcopy(const char *source,const char *dest);
extern int BSP_xdelete(const char *source);
extern int BSP_rename(const char *oldname, const char *newname);
extern int BSP_mass_read(const char *partition, unsigned int offset,unsigned int size,void* buf);
extern int BSP_mass_write(const char *partition, unsigned int offset,unsigned int size,void* buf);


#define DRV_FILE_OPEN(name,flags)                 BSP_fopen(name,flags)
#define DRV_FILE_CLOSE(fp)                        BSP_fclose(fp)
#define DRV_FILE_READ(buff,size,number,fp)        BSP_fread(buff,size,number,fp)
#define DRV_FILE_WRITE(buff,size,number,fp)       BSP_fwrite(buff,size,number,fp)
#define DRV_FILE_LSEEK(fp,offset,whence)          BSP_fseek(fp,offset,whence)
#define DRV_FILE_TELL(fp)                         BSP_ftell(fp)
#define DRV_FILE_RMFILE(fileName)                 BSP_remove(fileName)
#define DRV_FILE_MKDIR(dirName)                   BSP_mkdir(dirName)
#define DRV_FILE_RMDIR(path)                      BSP_rmdir(path)
#define DRV_FILE_OPENDIR(dirName)                 BSP_opendir(dirName)
#define DRV_FILE_READDIR(pDir)                    BSP_readdir(pDir)
#define DRV_FILE_CLOSEDIR(pDir)                   BSP_closedir(pDir)
#define DRV_FILE_STAT(name,pStat)                 BSP_stat(name,pStat)
#define DRV_FILE_ERRNOGET()                       BSP_errnoGet()
#define DRV_FILE_ACCESS(name,flags)               BSP_access(name,flags)
#define DRV_FILE_XCOPY(src, dest)                 BSP_xcopy(src, dest)
#define DRV_FILE_XDELETE(name)                    BSP_xdelete(name)
#define DRV_FILE_RENAME(oldname,newname)          BSP_rename(oldname,newname)

#pragma pack(0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


