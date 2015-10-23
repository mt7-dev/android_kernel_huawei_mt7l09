#ifndef __HI6451_EXC_H
#define __HI6451_EXC_H

#define HI6451_OCP_OFFSET                   0

#define HI6451_OTP_OFFSET                   0/*over temp threshould,default is 125??*/
#define HI6451_UVP_OFFSET                   2
#define HI6451_OVP_OFFSET                   4

#define HI6451_OTMP_125_OFFSET                          0
#define HI6451_OTMP_150_OFFSET                          0
#define HI6451_VINI_UNDER_2P7_OFFSET                    2
#define HI6451_VINI_OVER_6P0_OFFSET                     4
#define HI6451_CORE_OVER_CUR_OFFSET                     0

typedef unsigned long hi6451_excflags_t;

#endif