#include "srs_typedefs.h"
#include "srs_trmediahd_def.h"


static const SRSInt32 truEq32k[] = {
      2,      //iwl of this section
      SRS_FXP32(+2.089127153158188E-001, 2),      //B0
      SRS_FXP32(-4.156465530395508E-001, 2),      //B1
      SRS_FXP32(+2.067466378211975E-001, 2),      //B2
      SRS_FXP32(+1.997323870658875E+000, 2),      //A1
      SRS_FXP32(-9.973854422569275E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP32(+9.898679256439209E-001, 2),      //B0
      SRS_FXP32(-1.952648043632507E+000, 2),      //B1
      SRS_FXP32(+9.642866253852844E-001, 2),      //B2
      SRS_FXP32(+1.767128348350525E+000, 2),      //A1
      SRS_FXP32(-7.684917449951172E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP32(+8.293326497077942E-001, 2),      //B0
      SRS_FXP32(-1.527297258377075E+000, 2),      //B1
      SRS_FXP32(+7.278861403465271E-001, 2),      //B2
      SRS_FXP32(+1.929988384246826E+000, 2),      //A1
      SRS_FXP32(-9.677990674972534E-001, 2),      //A2

      1,      //iwl of this section
      SRS_FXP32(+5.537297129631043E-001, 1),      //B0
      SRS_FXP32(-5.545395016670227E-001, 1),      //B1
      SRS_FXP32(+4.444150924682617E-001, 1),      //B2
      SRS_FXP32(+5.303322076797485E-001, 1),      //A1
      SRS_FXP32(+4.542721807956696E-002, 1),      //A2

      4,      //gain iwl
      SRS_FXP32(+5.255512237548828E+000, 4)      //gain
};

//Crossover freq: 650Hz

static const SRSInt16 xoverlpf32k[] = {
/*Xover Filter: LPF: cutoff = Fc-Fc/(2*order-1)*/

        /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+2.685546875000000E-3, 2),
        /* Coefficient B1 */                        SRS_FXP16(+5.432128906250000E-3, 2),
        /* Coefficient B2 */                        SRS_FXP16(+2.685546875000000E-3, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.805908203125000E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-8.167724609375000E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+2.197265625000000E-3, 2),
        /* Coefficient B1 */                        SRS_FXP16(+4.394531250000000E-3, 2),
        /* Coefficient B2 */                        SRS_FXP16(+2.197265625000000E-3, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.908325195312500E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.197998046875000E-1, 2),

        /* Gain iwl*/                                                                2,
        /* Gain */                                  SRS_FXP16(+1.300065855982868E+0, 2)


 };

static const SRSInt16 xoverhpf32k[] = {
/*Xover Filter: HPF: cutoff = Fc+Fc/(2*order-1)*/

        /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+4.267578125000000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-8.535156250000000E-1, 2),
        /* Coefficient B2 */                        SRS_FXP16(+4.267578125000000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.744506835937500E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-7.632446289062500E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+6.771240234375000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.354309082031250E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+6.771240234375000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.874511718750000E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-8.945922851562500E-1, 2),

        /* Gain iwl*/                                                                3,
        /* Gain */                                  SRS_FXP16(+2.859513700278974E+0, 3)



 };


static const SRSInt16  hpfptb32k[] = {
/*SOS cascaded IIR by SRS Filter Design Tool Ver1.0.0.0*/
//Cutof: 200Hz
        /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+4.358520507812500E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-8.717041015625000E-1, 2),
        /* Coefficient B2 */                        SRS_FXP16(+4.358520507812500E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.925415039062500E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.269409179687500E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+8.138427734375000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.627685546875000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+8.138427734375000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.944458007812500E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.459838867187500E-1, 2),

        /* Section 2 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+7.111206054687500E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.422241210937500E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+7.111206054687500E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.978332519531250E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.798583984375000E-1, 2),

        /* Gain iwl*/                                                                3,
        /* Gain */                                  SRS_FXP16(+3.674820277781754E+0, 3)


 };

static const SRSInt32 truEq44k[] = {
      2,      //iwl of this section
      SRS_FXP32(+2.087072730064392E-001, 2),      //B0
      SRS_FXP32(-4.158353209495544E-001, 2),      //B1
      SRS_FXP32(+2.071348130702972E-001, 2),      //B2
      SRS_FXP32(+1.998069643974304E+000, 2),      //A1
      SRS_FXP32(-9.981021285057068E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP32(+1.007293343544006E+000, 2),      //B0
      SRS_FXP32(-1.994820833206177E+000, 2),      //B1
      SRS_FXP32(+9.883375167846680E-001, 2),      //B2
      SRS_FXP32(+1.825764656066895E+000, 2),      //A1
      SRS_FXP32(-8.265060782432556E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP32(+8.245672583580017E-001, 2),      //B0
      SRS_FXP32(-1.558743715286255E+000, 2),      //B1
      SRS_FXP32(+7.501321434974670E-001, 2),      //B2
      SRS_FXP32(+1.956504344940186E+000, 2),      //A1
      SRS_FXP32(-9.765315651893616E-001, 2),      //A2

      1,      //iwl of this section
      SRS_FXP32(+6.108590364456177E-001, 1),      //B0
      SRS_FXP32(-8.566578030586243E-001, 1),      //B1
      SRS_FXP32(+5.210751891136169E-001, 1),      //B2
      SRS_FXP32(+8.440923094749451E-001, 1),      //A1
      SRS_FXP32(-1.153309419751167E-001, 1),      //A2

      4,      //gain iwl
      SRS_FXP32(+5.438996791839600E+000, 4)      //gain
};

//Crossover freq: 650Hz

static const SRSInt16 xoverlpf44k[] = {
/*Xover Filter: LPF: cutoff = Fc-Fc/(2*order-1)*/

        /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+1.464843750000000E-3, 2),
        /* Coefficient B1 */                        SRS_FXP16(+2.929687500000000E-3, 2),
        /* Coefficient B2 */                        SRS_FXP16(+1.464843750000000E-3, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.857604980468750E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-8.634643554687500E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+1.159667968750000E-3, 2),
        /* Coefficient B1 */                        SRS_FXP16(+2.380371093750000E-3, 2),
        /* Coefficient B2 */                        SRS_FXP16(+1.159667968750000E-3, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.934997558593750E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.411010742187500E-1, 2),

        /* Gain iwl*/                                                                2,
        /* Gain */                                  SRS_FXP16(+1.299618357078202E+0, 2)

 };

static const SRSInt16 xoverhpf44k[] = {
/*Xover Filter: HPF: cutoff = Fc+Fc/(2*order-1)*/

        /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+4.289550781250000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-8.578491210937500E-1, 2),
        /* Coefficient B2 */                        SRS_FXP16(+4.289550781250000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.811950683593750E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-8.221435546875000E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+6.881103515625000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.376220703125000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+6.881103515625000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.911560058593750E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.223022460937500E-1, 2),

        /* Gain iwl*/                                                                3,
        /* Gain */                                  SRS_FXP16(+2.950247401625332E+0, 3)


 };


static const SRSInt16  hpfptb44k[] = {
/*SOS cascaded IIR by SRS Filter Design Tool Ver1.0.0.0*/
//Cutof: 200Hz
         /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+9.818115234375000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.963623046875000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+9.818115234375000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.945617675781250E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.464111328125000E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+9.818115234375000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.963623046875000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+9.818115234375000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.959716796875000E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.605102539062500E-1, 2),

        /* Section 2 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+9.818115234375000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.963623046875000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+9.818115234375000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.984558105468750E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.853515625000000E-1, 2),

        /* Gain iwl*/                                                                2,
        /* Gain */                                  SRS_FXP16(+1.000000000000000E+0, 2)


 };


static const SRSInt32 truEq48k[] = {
      2,      //iwl of this section
      SRS_FXP32(+2.086426168680191E-001, 2),      //B0
      SRS_FXP32(-4.158348143100739E-001, 2),      //B1
      SRS_FXP32(+2.071979045867920E-001, 2),      //B2
      SRS_FXP32(+1.998228788375855E+000, 2),      //A1
      SRS_FXP32(-9.982562065124512E-001, 2),      //A2

      3,      //iwl of this section
      SRS_FXP32(+1.011117696762085E+000, 3),      //B0
      SRS_FXP32(-2.004053592681885E+000, 3),      //B1
      SRS_FXP32(+9.936226010322571E-001, 3),      //B2
      SRS_FXP32(+1.838841438293457E+000, 3),      //A1
      SRS_FXP32(-8.394717574119568E-001, 3),      //A2

      2,      //iwl of this section
      SRS_FXP32(+8.233727812767029E-001, 2),      //B0
      SRS_FXP32(-1.564704656600952E+000, 2),      //B1
      SRS_FXP32(+7.548336386680603E-001, 2),      //B2
      SRS_FXP32(+1.961492300033569E+000, 2),      //A1
      SRS_FXP32(-9.784179329872131E-001, 2),      //A2

      1,      //iwl of this section
      SRS_FXP32(+6.279438138008118E-001, 1),      //B0
      SRS_FXP32(-9.287059307098389E-001, 1),      //B1
      SRS_FXP32(+5.426644086837769E-001, 1),      //B2
      SRS_FXP32(+9.179646372795105E-001, 1),      //A1
      SRS_FXP32(-1.570691168308258E-001, 1),      //A2

      4,      //gain iwl
      SRS_FXP32(+5.463378906250000E+000, 4)      //gain
};

//Crossover freq: 650Hz

static const SRSInt16 xoverlpf48k[] = {
/*Xover Filter: LPF: cutoff = Fc-Fc/(2*order-1)*/

         /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+1.281738281250000E-3, 2),
        /* Coefficient B1 */                        SRS_FXP16(+2.563476562500000E-3, 2),
        /* Coefficient B2 */                        SRS_FXP16(+1.281738281250000E-3, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.868896484375000E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-8.738403320312500E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+1.281738281250000E-3, 2),
        /* Coefficient B1 */                        SRS_FXP16(+2.563476562500000E-3, 2),
        /* Coefficient B2 */                        SRS_FXP16(+1.281738281250000E-3, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.940551757812500E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.457397460937500E-1, 2),

        /* Gain iwl*/                                                                2,
        /* Gain */                                  SRS_FXP16(+1.000000000000000E+0, 2)


 };

static const SRSInt16 xoverhpf48k[] = {
/*Xover Filter: HPF: cutoff = Fc+Fc/(2*order-1)*/

       /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+4.294433593750000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-8.588867187500000E-1, 2),
        /* Coefficient B2 */                        SRS_FXP16(+4.294433593750000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.826721191406250E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-8.353881835937500E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+6.904296875000000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.380920410156250E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+6.904296875000000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.919250488281250E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.283447265625000E-1, 2),

        /* Gain iwl*/                                                                3,
        /* Gain */                                  SRS_FXP16(+2.969892316624282E+0, 3)


 };

static const SRSInt16  hpfptb48k[] = {
/*SOS cascaded IIR by SRS Filter Design Tool Ver1.0.0.0*/
//Cutoff: 200Hz
        /* Section 0 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+9.832763671875000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.966552734375000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+9.832763671875000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.950012207031250E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.506835937500000E-1, 2),

        /* Section 1 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+9.832763671875000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.966552734375000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+9.832763671875000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.962951660156250E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.636230468750000E-1, 2),

        /* Section 2 */
        /* iwl */                                                                    2,
        /* Coefficient B0 */                        SRS_FXP16(+9.832763671875000E-1, 2),
        /* Coefficient B1 */                        SRS_FXP16(-1.966552734375000E+0, 2),
        /* Coefficient B2 */                        SRS_FXP16(+9.832763671875000E-1, 2),
        /* Coefficient A1 */                        SRS_FXP16(+1.985839843750000E+0, 2),
        /* Coefficient A2 */                        SRS_FXP16(-9.865112304687500E-1, 2),

        /* Gain iwl*/                                                                2,
        /* Gain */                                  SRS_FXP16(+1.000000000000000E+0, 2)


 };

//2postfix stand for headset;1postfix stand for speaker;
extern SRSInt16 Untitled_HPF1[];
extern SRSInt16 Untitled_HPF2[];
extern SRSInt32 Untitled_PEQ1[];
extern SRSInt32 Untitled_PEQ2[];
extern SRSTrMediaHDHpfOrder HPF_Order;

SRSTrMediaHDFilterConfig  GetFilterNonflatPeqCfg(int sampleRate, unsigned int device)
{
    SRSTrMediaHDFilterConfig cfg;


    cfg.FilterOrders.LPeqBands = cfg.FilterOrders.RPeqBands = 4;

    //if device is headset(1 XA_DTS_DEV_HEADSET xa_dts_pp.h)
    if (1 == device)
    {
        switch(sampleRate){
            case 32:
                cfg.XoverLpfCoefs = xoverlpf32k;
                cfg.XoverHpfCoefs = xoverhpf32k;
                cfg.FilterOrders.HpfOrder = (sizeof(hpfptb32k)/sizeof(SRSFloat32)-1)/5*2;
                cfg.HpfCoefs = hpfptb32k;
                cfg.LeftTruEqCoefs = truEq32k;
                cfg.RightTruEqCoefs = truEq32k;
                break;
            case 44:
                cfg.XoverLpfCoefs = xoverlpf44k;
                cfg.XoverHpfCoefs = xoverhpf44k;
                cfg.FilterOrders.HpfOrder = (sizeof(hpfptb44k)/sizeof(SRSFloat32)-1)/5*2;
                cfg.HpfCoefs = hpfptb44k;
                cfg.LeftTruEqCoefs = truEq44k;
                cfg.RightTruEqCoefs = truEq44k;
                break;
            case 48:
                cfg.XoverLpfCoefs = xoverlpf48k;
                cfg.XoverHpfCoefs = xoverhpf48k;
                cfg.FilterOrders.HpfOrder = HPF_Order.HPF2_order;
                cfg.HpfCoefs = Untitled_HPF2;
                cfg.LeftTruEqCoefs = (const SRSInt32 *)Untitled_PEQ2;
                cfg.RightTruEqCoefs = (const SRSInt32 *)Untitled_PEQ2;
                break;
        }
    }
    else//the device will be speaker in other situation
    {
        switch(sampleRate){
            case 32:
                cfg.XoverLpfCoefs = xoverlpf32k;
                cfg.XoverHpfCoefs = xoverhpf32k;
                cfg.FilterOrders.HpfOrder = (sizeof(hpfptb32k)/sizeof(SRSFloat32)-1)/5*2;
                cfg.HpfCoefs = hpfptb32k;
                cfg.LeftTruEqCoefs = truEq32k;
                cfg.RightTruEqCoefs = truEq32k;
                break;
            case 44:
                cfg.XoverLpfCoefs = xoverlpf44k;
                cfg.XoverHpfCoefs = xoverhpf44k;
                cfg.FilterOrders.HpfOrder = (sizeof(hpfptb44k)/sizeof(SRSFloat32)-1)/5*2;
                cfg.HpfCoefs = hpfptb44k;
                cfg.LeftTruEqCoefs = truEq44k;
                cfg.RightTruEqCoefs = truEq44k;
                break;
            case 48:
                cfg.XoverLpfCoefs = xoverlpf48k;
                cfg.XoverHpfCoefs = xoverhpf48k;
                cfg.FilterOrders.HpfOrder = HPF_Order.HPF1_order;
                cfg.HpfCoefs = Untitled_HPF1;
                cfg.LeftTruEqCoefs = (const SRSInt32 *)Untitled_PEQ1;
                cfg.RightTruEqCoefs = (const SRSInt32 *)Untitled_PEQ1;
                break;
        }
    }

    return cfg;
}

