#ifndef __EMMC_ERRCODE_H__
#define __EMMC_ERRCODE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Error Codes */
#define ERRNOERROR                          0
#define ERRCARDNOTCONN                      1
#define ERRCMDNOTSUPP                       2
#define ERRINVALIDCARDNUM                   3
//#define ERRRESPTIMEOUT                      4
#define ERRCARDNOTFOUND                     5
#define ERRCMDRETRIESOVER                   6
#define ERRCMDINPROGRESS                    7
#define ERRNOTSUPPORTED                     8
#define ERRRESPRECEP                        9
#define ERRENUMERATE                        10
#define ERRHARDWARE                         11
#define ERRNOMEMORY                         12
#define ERRFSMSTATE                         14
#define ERRADDRESSRANGE                     15
#define ERRADDRESSMISALIGN                  16
#define ERRBLOCKLEN                         17
#define ERRERASESEQERR                      18
#define ERRERASEPARAM                       19
#define ERRPROT                             20
#define ERRCARDLOCKED                       21
#define ERRCRC                              22
#define ERRILLEGALCOMMAND                   23
#define ERRECCFAILED                        24
#define ERRCCERR                            25
#define ERRUNKNOWN                          26
#define ERRUNDERRUN                         27
#define ERROVERRUN                          28
#define ERRCSDOVERWRITE                     29
#define ERRERASERESET                       30
#define ERRDATATIMEOUT                      31
#define ERRUNDERWRITE                       32
#define ERROVERREAD                         33
#define ERRENDBITERR                        34
#define ERRDCRC                             35
#define ERRSTARTBIT                         36
#define ERRTIMEROUT                         37
#define ERRCARDNOTREADY                     38
#define ERRBADFUNC                          39
#define ERRPARAM                            40
#define ERRNOTFOUND                         41
#define ERRWRTPRT                           42
#define ERRHLE                              43
#define ERRWRITE                            44
#define ERRBOOTLEN                          45
#define ERRRETRYTIMEOUT                     46
#define ERRDATAUNCONSISTENT                 47
#define ERRPMUOP                            48
#define ERRCOMERR                           50

#define ERRIDMACFBE                         51
#define ERRIDMACDU                          52
#define ERRIDMACCBE                         53
#define ERRIDMACPOLLD                       54
#define ERRIDMACMISC                        61

#define ERRRESPTIMEOUT                      0x100
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of emmcErrcode.h */
