

#ifndef __DIAG_FS_OP_H__
#define __DIAG_FS_OP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "diag_cmd_id.h"


#if (VOS_OS_VER == VOS_LINUX)
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/personality.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/rcupdate.h>
#include <linux/hrtimer.h>
#include <linux/statfs.h>
#include <linux/stat.h>
#elif (VOS_OS_VER == VOS_RTOSCK)
#include "FileSysInterface.h"
#endif
#include "diag_cmd_param_plugin.h"
#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define DIAG_FILE_NULL (-1)
#define DIAG_FS_ERROR (-1)
#define MSP_DF_DIR_BUF_LEN (1024*5)

#if (VOS_OS_VER == VOS_LINUX)
#define DIAG_FS_OPEN(filename,flags,mode)       sys_open(filename,flags,mode)
#define DIAG_FS_OPENDIR(dirname,flags,mode)     sys_open(dirname,flags,mode)
#define DIAG_FS_CLOSE(fd)                       sys_close(fd)
#define DIAG_FS_CLOSEDIR(dir_handle)            sys_close(dir_handle)
#define DIAG_FS_READ(fd,buffer,count)           sys_read(fd,buffer,count)
#define DIAG_FS_GETDENTS(fd, buf, count)        sys_getdents(fd, buf, count)
#define DIAG_FS_WRITE(fd,buffer,count)          sys_write(fd,buffer,count)
#define DIAG_FS_MKDIR(dirname,mode)             sys_mkdir(dirname,mode)
#define DIAG_FS_RMDIR(dirname)                  sys_rmdir(dirname )
#define DIAG_FS_RMFILE(filename)                sys_unlink(filename)
#define DIAG_FS_LSEEK(fd,offset,origin)         sys_lseek(fd,offset,origin)
#define DIAG_FS_STAT(path,buf)                  vfs_stat(path,buf)
#define DIAG_FS_SYNC() 							sys_sync()
#define DIAG_FS_FILE_SYNC(fd) 					sys_fsync(fd)
#elif (VOS_OS_VER == VOS_RTOSCK)
#define DIAG_FS_OPEN(filename,flags,mode)       DRV_FILE_OPEN(filename,flags)
#define DIAG_FS_OPENDIR(dirname,flags,mode)     DRV_FILE_OPENDIR(dirname)
#define DIAG_FS_CLOSE(fd)                       DRV_FILE_CLOSE(fd)
#define DIAG_FS_CLOSEDIR(dir_handle)            DRV_FILE_CLOSEDIR(dir_handle)
#define DIAG_FS_READ(fd,buffer,count)           DRV_FILE_READ(buffer,1,count,fd)
#define DIAG_FS_READDIR(dir_handle)             DRV_FILE_READDIR(dir_handle)
#define DIAG_FS_WRITE(fd,buffer,count)          DRV_FILE_WRITE(buffer,1,count,fd)
#define DIAG_FS_MKDIR(dirname,mode)             DRV_FILE_MKDIR(dirname)
#define DIAG_FS_RMDIR(dirname)                  DRV_FILE_RMDIR(dirname)
#define DIAG_FS_RMFILE(filename)                DRV_FILE_RMFILE(filename)
#define DIAG_FS_LSEEK(fd,offset,origin)         DRV_FILE_LSEEK(fd,offset,origin)
#define DIAG_FS_FTELL(fp)                       DRV_FILE_TELL(fp)
#define DIAG_FS_STAT(path,pStat)                DRV_FILE_STAT(path,pStat)
#define DIAG_FS_SYNC()     
#define DIAG_FS_FILE_SYNC(fd)    
#else
#define DIAG_FS_OPEN(filename,flags,mode)       fopen(filename,mode)
#define DIAG_FS_OPENDIR(dirname,flags,mode)     opendir(dirname)
#define DIAG_FS_CLOSE(fd)                       fclose(fd)
#define DIAG_FS_CLOSEDIR(dir_handle)            closedir(dir_handle)
#define DIAG_FS_READ(fd,buffer,count)           fread(buffer,1,count,fd)
#define DIAG_FS_READDIR(dir_handle)             readdir(dir_handle)
#define DIAG_FS_WRITE(fd,buffer,count)          fwrite(buffer,1,count,fd)
#define DIAG_FS_MKDIR(dirname,mode)             mkdir(dirname)
#define DIAG_FS_RMDIR(dirname)                  rmdir(dirname)
#define DIAG_FS_RMFILE(filename)                unlink(filename)
#define DIAG_FS_LSEEK(fd,offset,origin)         fseek(fd,offset,origin)
#define DIAG_FS_FTELL(fp)                       ftell(fp)
#define DIAG_FS_STAT(path,pStat)                stat(path,pStat)
#define DIAG_FS_SYNC() 	
#define DIAG_FS_FILE_SYNC(fd) 					
#endif

#if (VOS_OS_VER == VOS_LINUX)
#define DIAG_IF_DIR             0040000          /* directory */
#else
#define DIAG_IF_DIR             0x4000          /* directory */
#endif

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*保存操作文件的信息*/
typedef struct
{
    VOS_INT     lFile;
    VOS_UINT32  ulFileType;
    HTIMER      hTimer;
}DIAG_FILE_INFO_STRU;

#if (VOS_OS_VER == VOS_LINUX)
typedef struct
{
	VOS_UINT32	d_ino;
	VOS_UINT32	d_off;
	VOS_UINT16	d_reclen;
	VOS_CHAR	d_name[1];
}DIAG_DIRENT_STRU;

/*typedef struct
{
	VOS_UINT16 st_dev;
	VOS_UINT16 st_ino;
	VOS_UINT16 st_mode;
	VOS_UINT16 st_nlink;
	VOS_UINT16 st_uid;
	VOS_UINT16 st_gid;
	VOS_UINT16 st_rdev;
	VOS_UINT32  st_size;
	VOS_UINT32  st_atime;
	VOS_UINT32  st_mtime;
	VOS_UINT32  st_ctime;
}DIAG_STAT_STRU;*/
typedef struct kstat DIAG_STAT_STRU;

#else
typedef struct
{
	VOS_UINT32          d_ino;                      	  /* file serial number */
    VOS_CHAR            d_name [MSP_DF_DIR_MAX_LEN];  /* file name, null-terminated */
}DIAG_DIRENT_STRU;

typedef struct
{
    VOS_UINT16      	st_dev;
    VOS_CHAR            padding[2];
    VOS_INT16        	st_ino;
   	VOS_INT16        	st_mode;
    VOS_UINT16     		st_nlink;
    VOS_UINT16      	st_uid;
    VOS_UINT16      	st_gid;
    VOS_UINT16      	st_rdev;
    VOS_INT16        	st_size;                      /* file size*/
    VOS_UINT32          st_atime;                     /*Time of last access.*/
    VOS_UINT32          st_mtime;                     /*Time of last data modification.*/
    VOS_UINT32          st_ctime;                     /*Time of last status create */
}DIAG_STAT_STRU;

#endif
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/

extern VOS_UINT32 diag_FsProcEntry (VOS_UINT8* pstReq , VOS_UINT32 ulCmdId);


extern VOS_VOID diag_FsClose(VOS_VOID);


/*****************************************************************************
  9 OTHERS
*****************************************************************************/



VOS_UINT32 diag_FsGetDirInfo(VOS_CHAR *pDirName, VOS_UINT32 *pulTotalNum);
VOS_UINT32 diag_FsGetItemInfo(VOS_CHAR *pDirectory,DIAG_DIR_FILE_INFO_STRU *pstDirFileInfo);
VOS_INT32  diag_FsOpen(const VOS_CHAR *pcFileName, VOS_INT lFlag);
VOS_UINT32 diag_FsQueryProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
VOS_UINT32 diag_FsExportProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
VOS_UINT32 diag_FsDeleteProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
VOS_UINT32 diag_FsOpenProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
VOS_UINT32 diag_FsImportProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);
VOS_UINT32 diag_FsSpaceProc(VOS_UINT8* pstReq,VOS_UINT32 ulCmdId);


#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of diag_fs_op.h */
