/*************************************************************************/
/*                                                                       */
/*Copyright (c) 2005.12.08  CQCYIT                                       */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/
/* FILE NAME   filename asn.hec                    VERSION   3.0.0       */
/*                                                                       */
/* COMPONENT    RRC,MAC                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/* This file is used to define the constants in the RRC and MAC module   */
/* in the lcr                                                            */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*                                                                       */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*                                                                       */
/*                                                                       */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/* NAME             DATE        VERSION   REMARKS                        */
/* zhenghuanping   2005.12.08  v0.0.1    create the interface           */
/* yangliming       2005.12.08  v3.0.0    align to sytem version         */
/*************************************************************************/
#ifndef ASN_HEC
#define ASN_HEC

#define hiPDSCHidentities          1   //INTEGER ::= 64
#define hiPUSCHidentities          1   //INTEGER ::= 64
#define hiRM                       256 //INTEGER ::= 256
#define maxAC                      16  //INTEGER ::= 16
#define maxAdditionalMeas          4   //INTEGER ::= 4 20051212
#define maxASC                     8   //INTEGER ::= 8
#define maxASCmap                  7   //INTEGER ::= 7
#define maxASCpersist              6   //INTEGER ::= 6
#define maxCCTrCH                  1   //INTEGER ::= 8------->1
#define maxCellMeas                32  //INTEGER ::= 32
#define maxCellMeas_1              31  //INTEGER ::= 31
#define maxCNdomains               2   //INTEGER ::= 4
#define maxCPCHsets                1   //INTEGER ::= 16
#define maxDPCH_DLchan             8   //INTEGER ::= 8
#define maxDPDCH_UL                6   //INTEGER ::= 6
#define maxDRACclasses             1   //INTEGER ::= 8
#define maxFACHPCH                 8   //INTEGER ::= 8
#define maxFreq                    8   //INTEGER ::= 8
#define maxFreqBandsFDD            1   //INTEGER ::= 8
#define maxFreqBandsTDD            4   //INTEGER ::= 4
#define maxFreqBandsGSM            1   //INTEGER ::= 16
#define maxHProcesses              8   //INTEGER ::= 8 //20070129
#define maxHSSCCHs                 4   //INTEGER ::= 4 //20070129
#define maxInterSysMessages        4   //INTEGER ::= 4//20060306
#define maxLoCHperRLC              2   //INTEGER ::= 2
#define maxMAC_d_PDUsizes          8   //INTEGER ::= 8  //20070129
#define maxMeasEvent               8   //INTEGER ::= 8
#define maxMeasIntervals           3   //INTEGER ::= 3
#define maxMeasParEvent            2   //INTEGER ::= 2
#define maxNumCDMA2000Freqs        1   //INTEGER ::=  8
#define maxNumGSMFreqRanges        1   //INTEGER ::= 32
#define maxNumFDDFreqs             1   //INTEGER ::=  8
#define maxNumTDDFreqs             8   //INTEGER ::=  8
#define maxNoOfMeas                16  //INTEGER ::= 20051122
#define maxOtherRAT                1   //INTEGER ::= 15
#define maxOtherRAT_16             1   //INTEGER ::= 16
#define maxPage1                   8   //INTEGER ::= 8
#define maxPCPCH_APsig             1   //INTEGER ::= 16
#define maxPCPCH_APsubCh           1   //INTEGER ::= 12
#define maxPCPCH_CDsig             1   //INTEGER ::= 16
#define maxPCPCH_CDsubCh           1   //INTEGER ::= 12
#define maxPCPCH_SF                1   //INTEGER ::= 7
#define maxPCPCHs                  1   //INTEGER ::= 64
#define maxPDCPAlgoType            8   //INTEGER ::= 8
#define maxPDSCH                   1   //INTEGER ::= 8
#define maxPDSCH_TFCIgroups        1   //INTEGER ::= 256
#define maxPRACH                   16  //INTEGER ::= 16
#define maxPRACH_FPACH             8   //INTEGER ::= 8
#define maxPredefConfig            16  //INTEGER ::= 16
#define maxPUSCH                   1   //INTEGER ::= 8
#define maxRABsetup                16  //INTEGER ::= 16
#define maxQueueIDs                8   //INTEGER ::= 8 //20070129
#define maxRAT                     1   //INTEGER ::= 16
#define maxRB                      10   //INTEGER ::= 32  ----> 10 20070425 
#define maxRBallRABs               27  //INTEGER ::= 27
#define maxRBMuxOptions            4   //INTEGER ::= 8------>1//20070320
#define maxRBperRAB                3   //INTEGER ::= 8------>3
#define maxReportedGSMCells        8   //20060727 4h0 INTEGER ::= 6
#define maxRL                      1   //INTEGER ::= 8
#define maxRL_1                    1   //INTEGER ::= 7
#define maxROHC_PacketSizes_r4     16  //INTEGER ::= 16
#define maxROHC_Profile_r4         16  //INTEGER ::= 8
#define maxSat                     16  //INTEGER ::= 16
#define maxSCCPCH                  16  //INTEGER ::= 16
#define maxSIB                     32  //INTEGER ::= 32
#define maxSIB_FACH                8   //INTEGER ::= 8
#define maxSIBperMsg               16  //INTEGER ::= 16
#define maxSRBsetup                8   //INTEGER ::= 8
#define maxSystemCapability        1   //INTEGER ::= 16
#define maxTDD128Carrier           6   //INTEGER ::= 6 //20070129
#define maxTF                      24  //for mbms  //INTEGER ::= 32------->10
#define maxTF_CPCH                 1   //INTEGER ::= 16

#define MAX_SIB16_INSTANCE              16 //for SIB16

/* lidui add for ASN MEASUREMENT CONTROL begin */
#define maxNumEUTRAFreqs           8    //INTEGER ::= 8
#define maxEUTRACellPerFreq        16   //INTEGER ::= 16
#define maxGANSSSat                64   //INTEGER ::= 64
#define maxMeasCSGRange            4    //INTEGER ::= 4
#define maxSIrequest               4    //INTEGER ::= 4
#define maxCellMeasOnSecULFreq     32   //INTEGER ::= 32
#define maxMeasEventOnSecULFreq    8    //INTEGER ::= 8
#define maxMeasOccasionPattern     5    //INTEGER ::= 5
#define maxCommonHRNTI             4    //INTEGER ::= 4
#define maxEDCHs                   32   //INTEGER ::= 32
#define maxERUCCH                  256  //INTEGER ::= 256
#define maxERNTIgroup              32   //INTEGER ::= 32
#define maxERNTIperGroup           2    //INTEGER ::= 2
#define maxDedicatedCSGFreq        4    //INTEGER ::= 4
#define maxPrio                    8    //INTEGER ::= 8
#define maxNumGSMCellGroup         16   //INTEGER ::= 16
#define maxHNBNameSize             48   //INTEGER ::= 48
#define maxGANSS                   8    //INTEGER ::= 8
#define maxSgnType                 8    //INTEGER ::= 8
#define maxReportedEUTRAFreqs      4    //INTEGER ::= 4
#define maxReportedEUTRACellPerFreq 4   //INTEGER ::= 4



/* lidui add for ASN MEASUREMENT CONTROL end */


//20100521
//#if defined(SPREAD_TRUM) || defined(SDL) || defined(RUN_ON_C32)
//20100523
//#ifndef as_test
  
  //20060803
  //20100521
  //#if defined(SPREAD_TRUM) || defined(RUN_ON_C32)/////
  #ifndef TEST_RRC_ARM_TTCN
  #define maxTFC                     128   //INTEGER ::= 1024------->128
  #else
  #define maxTFC                     32
  #endif
  //#else
  //#define maxTFC                     32   //INTEGER ::= 1024------->32
  //#endif
  
/*  #else
  #define maxTFC                     16
  #endif*/
//#else
//#define maxTFC                     16   //INTEGER ::= 1024------->16
//#endif

#define maxTFCsub                  60  //INTEGER ::= 1024
#define maxTFCI_2_Combs            1   //INTEGER ::= 512
#define maxTGPS                    1   //INTEGER ::= 6

//20100523
/*#ifdef as_test//20050529
#define maxTrCH                    5   //INTEGER ::= 32------->5
#else*/
#define maxTrCH                    6   //INTEGER ::= 32------->6
//#endif

#define maxTrCHpreconf             6   //INTEGER ::= 32------->6
#define maxTS                      6   //INTEGER ::= 14
#define maxTS_1                    5   //INTEGER ::= 13
#define maxTS_LCR                  6   //INTEGER ::= 6
#define maxTS_LCR_1                5   //INTEGER ::= 5----1//20050325
#define maxURA                     8   //INTEGER ::= 8

// add for mbms 20071226
#define MAX_TDM_NUM                6

// add for hsupa 20080612
#define maxMacDFlow                8
#define maxBeta                    8
#define maxEagch                   4
#define maxEhich                   4
#define maxPrach                   2
#define maxE_DCHMACdFlow           8
#define maxNumEagch                4
#define maxNumEhich                4
#define maxRlcPduSize              32

/* lidui add for ASN DECODE begin */
#define ulbitMask_1                0x00000001
#define ulbitMask_2                0x00000003
#define ulbitMask_3                0x00000007
#define ulbitMask_4                0x0000000f
#define ulbitMask_5                0x0000001f
#define ulbitMask_6                0x0000003f
#define ulbitMask_7                0x0000007f
#define ulbitMask_8                0x000000ff
#define ulbitMask_9                0x000001ff
#define ulbitMask_10               0x000003ff
#define ulbitMask_11               0x000007ff
#define ulbitMask_12               0x00000fff
#define ulbitMask_13               0x00001fff
#define ulbitMask_14               0x00003fff
#define ulbitMask_15               0x00007fff
#define ulbitMask_16               0x0000ffff
#define ulbitMask_17               0x0001ffff
#define ulbitMask_20               0x000fffff
#define ulbitMask_22               0x003fffff
#define ulbitMask_24               0x00ffffff
#define ulbitMask_30               0x3fffffff
#define ulbitMask_32               0xffffffff

/* lidui add for ASN DECODE end */
#endif

