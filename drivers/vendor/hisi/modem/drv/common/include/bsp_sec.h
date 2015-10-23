#ifndef __BSP_SEC_H
#define __BSP_SEC_H

#ifdef __cplusplus
extern "C" {
#endif

struct image_head
{
    char image_name[16];
    char image_version[32];

    unsigned int image_index;
    unsigned int image_length;
    unsigned int load_addr;
    unsigned int crc;
    unsigned int is_sec;
    unsigned int is_compressed;

    char reserved[128 - 16 - 32 - sizeof(unsigned int) * 6];
};

int bsp_sec_check(unsigned int image_addr, unsigned int image_length);


#ifdef __cplusplus
}
#endif

#endif