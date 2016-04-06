#ifndef __HOSTGSM
#define __HOSTGSM

#include "hr_typedefs.h"

/*_________________________________________________________________________
 |                                                                         |
 |                           Function Prototypes                           |
 |_________________________________________________________________________|
*/

void   fillBitAlloc(int iVoicing, int iR0, int *piVqIndeces,
                           int iSoftInterp, int *piLags,
                           int *piCodeWrdsA, int *piCodeWrdsB,
                           int *piGsp0s, Shortword swVadFlag,
                           Shortword swSP, Shortword *pswBAlloc);



#endif
