

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#include "nv_file.h"
#include <linux/syscalls.h>



/*
 * local file sys open
 */
FILE* nv_lfile_open(s8* path,s8* mode)
{
#ifdef __KERNEL__
      return sys_open(path,0,mode);
#else
      return fopen(path,mode);
#endif
}

/*
 * local file read
 */

s32 nv_lfile_read(u8* ptr,u32 size,u32 count,FILE* fp)
{
#ifdef __KERNEL__
      return sys_read(fp,ptr,(size*count));
#else
      return fread(ptr,size,count,fp);
#endif
}

/*
 * local file write
 */
s32 nv_lfile_write(u8* ptr,u32 size,u32 count,FILE* fp)
{
#ifdef __KERNEL__
      return sys_write(fp,ptr,(size*count));
#else
      return fwrite(ptr,size,count,fp);
#endif
}

/*
 * local file seek
 */
s32 nv_lfile_seek(FILE* fp,s32 offset,s32 whence)
{
#ifdef __KERNEL__
      return sys_lseek(fp,offset,whence);
#else
      return fseek(fp,offset,whence);
#endif
}

/*
 * local file remove
 */
s32 nv_lfile_remove(s8* path)
{
#ifdef __KERNEL__
      return sys_unlink(path);
#else
      return unlink(path);
#endif
}

/*
 * local file close
 */
s32 nv_lfile_close(FILE* fp)
{
#ifdef __KERNEL__
      return sys_close(fp);
#else
      return fclose(fp);
#endif
}

/*
 * local file ftell
 */
s32 nv_lfile_ftell(FILE* fp)
{
#ifdef __KERNEL__
    s32 ret;
    s32 seek =0;

    ret = sys_llseek(fp, 0, 0, &seek, SEEK_CUR);
    if(ret)
    {
        printf("sys_llseek error 0x%x\n",ret);
        return NV_ERROR;
    }

    return seek;

#else
      return ftell(fp);
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

