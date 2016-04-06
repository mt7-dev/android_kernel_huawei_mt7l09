#ifndef __MATHHALF
#define __MATHHALF

#include "hr_typedefs.h"
#include "codec_op_etsi.h"
#include "codec_op_netsi.h"
#include "codec_op_vec.h"



/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

#define divide_s(var1, var2)      div_s(var1, var2)

Longword L_shift_r(Longword L_var1, Shortword var2);
Shortword shift_r(Shortword var1, Shortword var2);
Shortword isSwLimit(Shortword swIn);
Shortword isLwLimit(Longword L_In);
Longword  L_mpy_ll(Longword L_var1, Longword L_var2);

#endif
