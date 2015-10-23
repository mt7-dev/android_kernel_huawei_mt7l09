//*********************************************
//
// DTS control interface for Hisilicon/Huawei
// Aegis.F @ 2013/10/09
//
//*********************************************

#include "srs_trmediahd_def.h"

SRSTrMediaHDControls DTS_speaker={
    1,      //SRSBool                   Enable;
    SRS_FXP16(1.0,1),       //SRSInt16              InputGain;
    SRS_FXP16(1.0,1),       //SRSInt16              OutputGain;
    SRS_FXP16(1.0,1),       //SRSInt16              BypassGain;
    SRS_FXP16(0.5,1),       //SRSInt16              HeadrmGain;
    SRS_TRMEDIAHD_MAINTECH_WOWHDX,  //SRS_TRMEDIAHD_MAINTECH_NONE, SRS_TRMEDIAHD_MAINTECH_WOWHDX, SRS_TRMEDIAHD_MAINTECH_CSHP

    0,  //SRSBool                   EnableGEQ;
    1,  //SRSBool                   EnableHPF;
    1,  //SRSBool                   EnableTruEQ;

    {
    SRS_FXP16(0.3,1),       //SRSInt16                  InputGain;
    SRS_CSHP_CS_DECODER,    //SRS_CSHP_PASSIVE_DECODER, SRS_CSHP_CS_DECODER
    SRS_CSD_CINEMA, //SRS_CSD_CINEMA, SRS_CSD_PRO, SRS_CSD_MUSIC, SRS_CSD_MONO, SRS_CSD_LCRS

    1,  //SRSBool                       DCEnable;    //DialogClariy Enable
    1,  //SRSBool                       DefEnable;   //Definition Enable
    SRS_FXP16(0.1,1),   //SRSInt16                  DCLevel;     //DialogClarity Level
    SRS_FXP16(0.05,1),  //SRSInt16                  DefLevel;    //Definition Level

    {
    1,      //SRSBool               Enable;
    1,      //SRSBool               SplitAnalysisEnable;  //Enable/disable the split analysis feature
    SRS_FXP16(0.1,1),   //SRSInt16          CompressorLevel;    //0.0~1.0
    SRS_FXP16(0.6,1),   //SRSInt16          Level;  //TruBass Level, 0.0~1.0
    {SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM,SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM},  //SRSTBHDSpeakerSize    SpkSize; //{AudioFilter, Analysis Filter}
    SRS_TBHD_MODE_STEREO    //SRS_TBHD_MODE_MONO, SRS_TBHD_MODE_STEREO
    },   //SRSTrMediaHDTBHDControls  TBHDControls;   //TruBass HD controls

    1,  //SRSBool                       LmtEnable;      //Limiter Enable
    SRS_FXP32(0.45,2)   //SRSInt32                  LmtMinimalGain; //Limiter minimal gain
    }, //SRSTrMediaHDCshpControls  CshpControls;

    {
    SRS_FXP16(0.2,1),   //SRSInt16                  InputGain;
    SRS_WOWHDX_STEREO_ENHANCE_NONE,    //SRS_WOWHDX_STEREO_ENHANCE_NONE, SRS_WOWHDX_STEREO_ENHANCE_SRS3D, SRS_WOWHDX_STEREO_ENHANCE_WDSRD
    0,  //SRSBool                       XoverEnable;
    1,  //SRSBool                       FocusEnable;
    1,  //SRSBool                       DefEnable;
    SRS_FXP16(0.15,1),   //SRSInt16                  FocusLevel;
    SRS_FXP16(0.1,1), //SRSInt16                  DefLevel;

    {
    1,      //SRSBool               Enable;
    1,      //SRSBool               SplitAnalysisEnable;  //Enable/disable the split analysis feature
    SRS_FXP16(0.12,1),  //SRSInt16          CompressorLevel;    //0.0~1.0
    SRS_FXP16(0.35,1),  //SRSInt16          Level;  //TruBass Level, 0.0~1.0
    {SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM,SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM},  //SRSTBHDSpeakerSize    SpkSize; //{AudioFilter, Analysis Filter}
    SRS_TBHD_MODE_MONO  //SRS_TBHD_MODE_MONO, SRS_TBHD_MODE_STEREO
    }, //SRSTrMediaHDTBHDControls  TBHDControls;
    {
    SRS_FXP16(0.0,1),   //SRSInt16          SpaceLevel;
    SRS_FXP16(0.7,1),  //SRSInt16          CenterLevel;
    0,  //SRSBool               HeadphoneEnable;
    1,  //SRSBool               HighBitRateEnable;
    SRS_SRS3D_STEREO    //SRS_SRS3D_MONO, SRS_SRS3D_SINGLE_SPEAKER, SRS_SRS3D_STEREO
    },    //SRSTrMediaHDSRS3DControls SRS3DControls;
    {
    SRS_FXP16(1.0,1),   //SRSInt16          SpeakerSeparationLevel; //Speaker separation, 0~1,
    SRS_FXP16(1.0,1),   //SRSInt16          CenterBoostGain;    //Center boost gain, 0.25~1
    0,  //SRSBool               HandsetHPFEnable;   //Enable/disable the handset device HPF
    }     //SRSTrMediaHDWideSrdControls   WdSrdControls;
    },  //SRSTrMediaHDWowhdxControls    WowhdxControls;

    {
    1,  //SRSBool               Enable;
    SRS_FXP16(0.83,3),   //SRSInt16          OutputGain;
    SRS_FXP16(1.0,1),   //SRSInt16          BypassGain;
    SRS_FXP32(18.5,6),  //SRSInt32          BoostGain;
    40  //int                   DelayLen;
    }  //SRSTrMediaHDHdLmtControls HdLmtControls;
};

SRSTrMediaHDControls DTS_headphone={
    1,      //SRSBool                   Enable;
    SRS_FXP16(1.0,1),       //SRSInt16              InputGain;
    SRS_FXP16(1.0,1),       //SRSInt16              OutputGain;
    SRS_FXP16(1.0,1),       //SRSInt16              BypassGain;
    SRS_FXP16(0.5,1),       //SRSInt16              HeadrmGain;
    SRS_TRMEDIAHD_MAINTECH_WOWHDX,  //SRS_TRMEDIAHD_MAINTECH_NONE, SRS_TRMEDIAHD_MAINTECH_WOWHDX, SRS_TRMEDIAHD_MAINTECH_CSHP

    0,  //SRSBool                   EnableGEQ;
    0,  //SRSBool                   EnableHPF;
    1,  //SRSBool                   EnableTruEQ;

    {
    SRS_FXP16(0.3,1),       //SRSInt16                  InputGain;
    SRS_CSHP_CS_DECODER,    //SRS_CSHP_PASSIVE_DECODER, SRS_CSHP_CS_DECODER
    SRS_CSD_CINEMA, //SRS_CSD_CINEMA, SRS_CSD_PRO, SRS_CSD_MUSIC, SRS_CSD_MONO, SRS_CSD_LCRS

    1,  //SRSBool                       DCEnable;    //DialogClariy Enable
    1,  //SRSBool                       DefEnable;   //Definition Enable
    SRS_FXP16(0.1,1),   //SRSInt16                  DCLevel;     //DialogClarity Level
    SRS_FXP16(0.05,1),  //SRSInt16                  DefLevel;    //Definition Level

    {
    1,      //SRSBool               Enable;
    1,      //SRSBool               SplitAnalysisEnable;  //Enable/disable the split analysis feature
    SRS_FXP16(0.1,1),   //SRSInt16          CompressorLevel;    //0.0~1.0
    SRS_FXP16(0.6,1),   //SRSInt16          Level;  //TruBass Level, 0.0~1.0
    {SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM,SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM},  //SRSTBHDSpeakerSize    SpkSize; //{AudioFilter, Analysis Filter}
    SRS_TBHD_MODE_STEREO    //SRS_TBHD_MODE_MONO, SRS_TBHD_MODE_STEREO
    },   //SRSTrMediaHDTBHDControls  TBHDControls;   //TruBass HD controls

    1,  //SRSBool                       LmtEnable;      //Limiter Enable
    SRS_FXP32(0.45,2)   //SRSInt32                  LmtMinimalGain; //Limiter minimal gain
    }, //SRSTrMediaHDCshpControls  CshpControls;

    {
    SRS_FXP16(0.17,1),  //SRSInt16                  InputGain;
    SRS_WOWHDX_STEREO_ENHANCE_NONE, //SRS_WOWHDX_STEREO_ENHANCE_NONE, SRS_WOWHDX_STEREO_ENHANCE_SRS3D, SRS_WOWHDX_STEREO_ENHANCE_WDSRD
    0,  //SRSBool                       XoverEnable;
    1,  //SRSBool                       FocusEnable;
    1,  //SRSBool                       DefEnable;
    SRS_FXP16(0.27,1),  //SRSInt16                  FocusLevel;
    SRS_FXP16(0.085,1), //SRSInt16                  DefLevel;

    {
    1,      //SRSBool               Enable;
    1,      //SRSBool               SplitAnalysisEnable;  //Enable/disable the split analysis feature
    SRS_FXP16(0.1,1),   //SRSInt16          CompressorLevel;    //0.0~1.0
    SRS_FXP16(0.6,1),   //SRSInt16          Level;  //TruBass Level, 0.0~1.0
    {SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM,SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM},  //SRSTBHDSpeakerSize    SpkSize; //{AudioFilter, Analysis Filter}
    SRS_TBHD_MODE_STEREO    //SRS_TBHD_MODE_MONO, SRS_TBHD_MODE_STEREO
    },   //SRSTrMediaHDTBHDControls  TBHDControls;
    {
    SRS_FXP16(0.7,1),   //SRSInt16          SpaceLevel;
    SRS_FXP16(0.7,1),   //SRSInt16          CenterLevel;
    0,  //SRSBool               HeadphoneEnable;
    1,  //SRSBool               HighBitRateEnable;
    SRS_SRS3D_STEREO    //SRS_SRS3D_MONO, SRS_SRS3D_SINGLE_SPEAKER, SRS_SRS3D_STEREO
    },  //SRSTrMediaHDSRS3DControls SRS3DControls;
    {
    SRS_FXP16(1.0,1),   //SRSInt16          SpeakerSeparationLevel; //Speaker separation, 0~1,
    SRS_FXP16(1.0,1),   //SRSInt16          CenterBoostGain;    //Center boost gain, 0.25~1
    0,  //SRSBool               HandsetHPFEnable;   //Enable/disable the handset device HPF
    }     //SRSTrMediaHDWideSrdControls   WdSrdControls;
    },    //SRSTrMediaHDWowhdxControls    WowhdxControls;

    {
    1,  //SRSBool               Enable;
    SRS_FXP16(1.0,3),   //SRSInt16          OutputGain;
    SRS_FXP16(1.0,1),   //SRSInt16          BypassGain;
    SRS_FXP32(9.2,6),   //SRSInt32          BoostGain;
    22  //int                   DelayLen;
    }    //SRSTrMediaHDHdLmtControls HdLmtControls;
};

//××××××××××××××××××××××× 上面的部分为控制参数 ×××××××××××××××××××××××××××××××

//*********************** 以下部分为滤波器系数 ×××××××××××××××××××××××××××××××

/**
Filter Coefficients generated by SRS Labs DesignBench 1.1.0.0
**/
SRSTrMediaHDHpfOrder HPF_Order = {
    6,
    6
};


/*High Pass Filter: @SampleRate: 48000 Hz, @Cutoff: 101 Hz, @Order: 6*/

SRSInt16 Untitled_HPF1[] = {
      2,      //iwl of this section
      SRS_FXP16(+2.667846679687500E-001, 2),      //B0
      SRS_FXP16(-5.335693359375000E-001, 2),      //B1
      SRS_FXP16(+2.667846679687500E-001, 2),      //B2
      SRS_FXP16(+1.992980957031250E+000, 2),      //A1
      SRS_FXP16(-9.931640625000000E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP16(+9.071044921875000E-001, 2),      //B0
      SRS_FXP16(-1.814208984375000E+000, 2),      //B1
      SRS_FXP16(+9.071044921875000E-001, 2),      //B2
      SRS_FXP16(+1.981323242187500E+000, 2),      //A1
      SRS_FXP16(-9.815063476562500E-001, 2),      //A2

      3,      //iwl of this section
      SRS_FXP16(+1.068481445312500E+000, 3),      //B0
      SRS_FXP16(-2.136962890625000E+000, 3),      //B1
      SRS_FXP16(+1.068481445312500E+000, 3),      //B2
      SRS_FXP16(+1.974609375000000E+000, 3),      //A1
      SRS_FXP16(-9.747314453125000E-001, 3),      //A2

      3,      //gain iwl
      SRS_FXP16(+3.769897460937500E+000, 3)      //gain
};

/*High Pass Filter: @SampleRate: 48000 Hz, @Cutoff: 20 Hz, @Order: 6*/
SRSInt16 Untitled_HPF2[] = {
      2,      //iwl of this section
      SRS_FXP16(+2.677612304687500E-001, 2),      //B0
      SRS_FXP16(-5.355224609375000E-001, 2),      //B1
      SRS_FXP16(+2.677612304687500E-001, 2),      //B2
      SRS_FXP16(+1.998657226562500E+000, 2),      //A1
      SRS_FXP16(-9.986572265625000E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP16(+9.116821289062500E-001, 2),      //B0
      SRS_FXP16(-1.823364257812500E+000, 2),      //B1
      SRS_FXP16(+9.116821289062500E-001, 2),      //B2
      SRS_FXP16(+1.996276855468750E+000, 2),      //A1
      SRS_FXP16(-9.962768554687500E-001, 2),      //A2

      3,      //iwl of this section
      SRS_FXP16(+1.072753906250000E+000, 3),      //B0
      SRS_FXP16(-2.145507812500000E+000, 3),      //B1
      SRS_FXP16(+1.072753906250000E+000, 3),      //B2
      SRS_FXP16(+1.994995117187500E+000, 3),      //A1
      SRS_FXP16(-9.949951171875000E-001, 3),      //A2

      3,      //gain iwl
      SRS_FXP16(+3.799804687500000E+000, 3)      //gain
};

/*4-band TruEQ, @SampleRate: 48000 Hz
Band 00 -- Type: Traditional, Freq: 1200.0 Hz, Gain: -4.0 dB, Q: 2.5, InvertPhase: False
Band 01 -- Type: Traditional, Freq: 9000.0 Hz, Gain: -4.0 dB, Q: 2.1, InvertPhase: False
Band 02 -- Type: Traditional, Freq: 3500.0 Hz, Gain: -5.0 dB, Q: 3.3, InvertPhase: False
Band 03 -- Type: Traditional, Freq: 6100.0 Hz, Gain: -8.0 dB, Q: 9.0, InvertPhase: False
*/
SRSInt32 Untitled_PEQ1[] = {
      2,      //iwl of this section
      SRS_FXP32(+6.823997497558594E-001, 2),      //B0
      SRS_FXP32(-1.320339441299439E+000, 2),      //B1
      SRS_FXP32(+6.543978452682495E-001, 2),      //B2
      SRS_FXP32(+1.911903858184815E+000, 2),      //A1
      SRS_FXP32(-9.357359409332275E-001, 2),      //A2

      1,      //iwl of this section
      SRS_FXP32(+8.127263784408569E-001, 1),      //B0
      SRS_FXP32(-5.230646729469299E-001, 1),      //B1
      SRS_FXP32(+5.541074872016907E-001, 1),      //B2
      SRS_FXP32(+5.887989997863770E-001, 1),      //A1
      SRS_FXP32(-5.386059284210205E-001, 1),      //A2

      2,      //iwl of this section
      SRS_FXP32(+9.415585398674011E-001, 2),      //B0
      SRS_FXP32(-1.614163279533386E+000, 2),      //B1
      SRS_FXP32(+8.582099080085754E-001, 2),      //B2
      SRS_FXP32(+1.657264113426209E+000, 2),      //A1
      SRS_FXP32(-8.478252291679382E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP32(+9.138927459716797E-001, 2),      //B0
      SRS_FXP32(-1.238765835762024E+000, 2),      //B1
      SRS_FXP32(+8.613762855529785E-001, 2),      //B2
      SRS_FXP32(+1.299051880836487E+000, 2),      //A1
      SRS_FXP32(-8.616647720336914E-001, 2),      //A2

      2,      //gain iwl
      SRS_FXP32(+1.754986166954041E+000, 2)      //gain
};

/*4-band TruEQ, @SampleRate: 48000 Hz
Band 00 -- Type: Traditional, Freq: 3500.0 Hz, Gain: -4.0 dB, Q: 2.2, InvertPhase: False
Band 01 -- Type: Traditional, Freq: 730.0 Hz, Gain: -3.0 dB, Q: 2.0, InvertPhase: False
Band 02 -- Type: Traditional, Freq: 12001.0 Hz, Gain: 5.0 dB, Q: 2.0, InvertPhase: False
Band 03 -- Type: Traditional, Freq: 21.0 Hz, Gain: -6.0 dB, Q: 5.0, InvertPhase: False
*/
SRSInt32 Untitled_PEQ2[] = {
      2,      //iwl of this section
      SRS_FXP32(+6.914188265800476E-001, 2),      //B0
      SRS_FXP32(-1.159604191780090E+000, 2),      //B1
      SRS_FXP32(+6.015229225158691E-001, 2),      //B2
      SRS_FXP32(+1.615703344345093E+000, 2),      //A1
      SRS_FXP32(-8.014856576919556E-001, 2),      //A2

      2,      //iwl of this section
      SRS_FXP32(+8.599538803100586E-001, 2),      //B0
      SRS_FXP32(-1.685220003128052E+000, 2),      //B1
      SRS_FXP32(+8.329895734786987E-001, 2),      //B2
      SRS_FXP32(+1.947070360183716E+000, 2),      //A1
      SRS_FXP32(-9.559937715530396E-001, 2),      //A2

      1,      //iwl of this section
      SRS_FXP32(+8.270444869995117E-001, 1),      //B0
      SRS_FXP32(+1.466409303247929E-004, 1),      //B1
      SRS_FXP32(+2.932096421718597E-001, 1),      //B2
      SRS_FXP32(-2.064709551632404E-004, 1),      //A1
      SRS_FXP32(-5.773211717605591E-001, 1),      //A2

      2,      //iwl of this section
      SRS_FXP32(+7.987031340599060E-001, 2),      //B0
      SRS_FXP32(-1.597107529640198E+000, 2),      //B1
      SRS_FXP32(+7.984104156494141E-001, 2),      //B2
      SRS_FXP32(+1.999261379241943E+000, 2),      //A1
      SRS_FXP32(-9.992689490318298E-001, 2),      //A2

      3,      //gain iwl
      SRS_FXP32(+2.837369680404663E+000, 3)      //gain
};

/*TruBass HD Filters: @SampleRate: 48000 Hz, @AudioFreq: 220 Hz, @AnalysisFreq: 100 Hz*/
SRSInt32 Untitled_TBHD1[] = {
      SRS_FXP32(+1.422784384340048E-002, 1),
      SRS_FXP32(+9.715443253517151E-001, 1),
      SRS_FXP32(-5.157389678061009E-003, 2),
      SRS_FXP32(+1.985404968261719E+000, 2),
      SRS_FXP32(-9.862450361251831E-001, 2),
      SRS_FXP32(-3.724975511431694E-002, 2),
      SRS_FXP32(+1.960597157478333E+000, 2),
      SRS_FXP32(-9.630892276763916E-001, 2),
      SRS_FXP32(+6.517783738672733E-003, 1),
      SRS_FXP32(+9.869644045829773E-001, 1),
      SRS_FXP32(-2.353131771087647E-003, 2),
      SRS_FXP32(+1.993549942970276E+000, 2),
      SRS_FXP32(-9.937241077423096E-001, 2),
      SRS_FXP32(-1.710546948015690E-002, 2),
      SRS_FXP32(+1.982529878616333E+000, 2),
      SRS_FXP32(-9.830501675605774E-001, 2),

      //TruBass Q Filter:
      2,      //iwl of Q filter
      SRS_FXP32(+2.668438851833344E-001, 2),      //B0
      SRS_FXP32(-5.545211434364319E-001, 2),      //B1
      SRS_FXP32(+2.895304560661316E-001, 2),      //B2
      SRS_FXP32(+1.915242791175842E+000, 2),      //A1
      SRS_FXP32(-9.216435551643372E-001, 2),      //A2

      3,      //gain iwl
      SRS_FXP32(+3.453867912292481E+000, 3)      //gain
};

/*TruBass HD Filters: @SampleRate: 48000 Hz, @AudioFreq: 70 Hz, @AnalysisFreq: 50 Hz*/
SRSInt32 Untitled_TBHD2[] = {
      SRS_FXP32(+4.571387078613043E-003, 1),
      SRS_FXP32(+9.908572435379028E-001, 1),
      SRS_FXP32(-1.648760400712490E-003, 2),
      SRS_FXP32(+1.995517253875732E+000, 2),
      SRS_FXP32(-9.956027269363403E-001, 2),
      SRS_FXP32(-1.200449839234352E-002, 2),
      SRS_FXP32(+1.987849116325378E+000, 2),
      SRS_FXP32(-9.881047606468201E-001, 2),
      SRS_FXP32(+3.269547130912542E-003, 1),
      SRS_FXP32(+9.934608936309815E-001, 1),
      SRS_FXP32(-1.178393140435219E-003, 2),
      SRS_FXP32(+1.996813535690308E+000, 2),
      SRS_FXP32(-9.968571066856384E-001, 2),
      SRS_FXP32(-8.589287288486958E-003, 2),
      SRS_FXP32(+1.991358280181885E+000, 2),
      SRS_FXP32(-9.914888739585877E-001, 2),

      //TruBass Q Filter:
      2,      //iwl of Q filter
      SRS_FXP32(+2.737270593643189E-001, 2),      //B0
      SRS_FXP32(-5.544704198837280E-001, 2),      //B1
      SRS_FXP32(+2.809304594993591E-001, 2),      //B2
      SRS_FXP32(+1.973692774772644E+000, 2),      //A1
      SRS_FXP32(-9.743588566780090E-001, 2),      //A2

      3,      //gain iwl
      SRS_FXP32(+3.559599876403809E+000, 3)      //gain
};

