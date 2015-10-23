#ifndef _DDM_PHASE_H_
#define _DDM_PHASE_H_

#define DDM_OK      0
#define DDM_ERROR   -1

int ddm_phase_boot_init(void);

int ddm_phase_boot_score(const char * phaseName, unsigned int param);

#endif

/*end of this file*/
