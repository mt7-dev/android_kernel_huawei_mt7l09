#ifndef __BSP_PCIE_H__
#define __BSP_PCIE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PCIE_CFG_VALID  (0xA5A55A5A)

/******* work mode **********/
/* ep mode */
#ifndef PCIE_WORK_MODE_EP
#define PCIE_WORK_MODE_EP   (0x0)
#endif

/* legacy ep mode */
#ifndef PCIE_WORK_MODE_LEP
#define PCIE_WORK_MODE_LEP  (0x1)
#endif

/* rc mode */
#ifndef PCIE_WORK_MODE_RC
#define PCIE_WORK_MODE_RC   (0x4)
#endif

struct pcie_cfg
{
    unsigned valid;
    unsigned work_mode;
};


#ifdef __cplusplus
}
#endif

#endif

