



#ifdef __cplusplus
extern "C" 
{
#endif

#ifndef CONFIG_SEC_CHECK

int bsp_sec_check(unsigned int image_addr, unsigned int image_length)
{
    return 0;
}

#endif

#ifdef __cplusplus
}
#endif
