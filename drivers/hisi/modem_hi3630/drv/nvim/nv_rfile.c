

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "bsp_rfile.h"
#include "nv_file.h"
#include "nv_com.h"

FILE* nv_rfile_open(s8* path,s8* mode)
{
    return bsp_open(path,mode);
}

s32 nv_rile_read(u8* ptr,u32 size,u32 count,FILE* fp)
{

}

s32 nv_rfile_write(u8* ptr,u32 size,u32 count,FILE* fp)
{
}

s32 nv_rfile_seek(FILE* fp,s32 offset,s32 whence)
{

}

s32 nv_rfile_close(FILE* fp)
{
}

s32 nv_rfile_remove(s8* path)
{
}

s32 nv_rfile_ftell(FILE* fp)
{
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

