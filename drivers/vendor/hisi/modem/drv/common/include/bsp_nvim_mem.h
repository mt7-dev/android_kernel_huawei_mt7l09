


#ifndef __BSP_NVIM_MEM_H__
#define __BSP_NVIM_MEM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*global ddr distribution*/
/*
   ------------------------512k----------------------------
   --------------------------------------------------------
   | glb  | ctrl |file |ref  |file 1|file 2|..... |file n|
   | info | info |list |list |data  |data  |..... |data  |
   | 1k   |      |     |     |      |      |      |      |
   --------------------------------------------------------
*/



#define NV_GLOBAL_START_ADDR        (void*) ((unsigned long)SHM_MEM_NV_ADDR)
#define NV_GLOBAL_END_ADDR           (void*)((unsigned long)SHM_MEM_NV_ADDR + SHM_MEM_NV_SIZE)

/*reg global info*/
#define NV_GLOBAL_INFO_ADDR         (void*)((unsigned long)NV_GLOBAL_START_ADDR)
#define NV_GLOBAL_INFO_SIZE         0x400               /*1 K*/

#define NV_GLOBAL_CTRL_INFO_ADDR   (void*) ((unsigned long)NV_GLOBAL_INFO_ADDR+NV_GLOBAL_INFO_SIZE)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /*__BSP_NVIM_MEM_H__*/



