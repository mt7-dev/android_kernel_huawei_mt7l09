    .text
        
    .type   AMRWB_Residu, @function
    .align   4
    .global AMRWB_Residu

AMRWB_Residu: # 0x4
    entry   a1, 80
    addi    a11, a2, 44
    l16si   a12, a2, 32
    l16si   a14, a2, 28
    l16si   a13, a2, 30
    addi    a10, a2, 42
    ae_lp16f.i  aep6, a2, 4
    ae_lp16f.i  aep7, a2, 6
    ae_lp16f.i  aep1, a2, 10
    ae_lp16f.i  aep2, a2, 12
    addi    a3, a1, 32
    addi    a9, a1, 32
    ae_lp16f.i  aep0, a2, 8
    movi.n  a15, 4
    addi    a7, a2, 34
    mov.n   a8, a5
    ae_lp16f.i  aep3, a2, 14
    ae_sp16f.l.i    aep3, a1, 14
    addi    a5, a2, 32
    ae_lp16f.i  aep5, a7, -16
    wur.ae_sar  a15
    ae_sp16f.l.i    aep0, a9, 10
    ae_sp16f.l.i    aep0, a3, -12
    l16si   a15, a2, 0
    ae_sp16f.l.i    aep2, a9, 6
    ae_sp16f.l.i    aep2, a3, -16
    addi    a7, a2, 40
    ae_sp16f.l.i    aep1, a9, 8
    ae_sp16f.l.i    aep1, a3, -14
    ae_sp16f.l.i    aep7, a9, 12
    ae_sp16f.l.i    aep7, a3, -10
    ae_sp16f.l.i    aep6, a9, 14
    ae_sp16f.l.i    aep6, a3, -8
    ae_sp16f.l.i    aep5, a1, 10
    ae_sp16f.l.i    aep5, a9, 0
    ae_lp16f.i  aep4, a5, -16
    ae_sp16f.l.i    aep3, a9, 4
    addi    a5, a2, 38
    ae_sp16f.l.i    aep4, a9, 2
    ae_sp16f.l.i    aep4, a1, 12
    ae_lp16f.i  aep3, a10, -16
    ae_lp16f.i  aep5, a2, 2
    ae_sp16f.l.i    aep5, a3, -6
    l16si   a10, a2, 26
    ae_sp16f.l.i    aep3, a1, 2
    ae_cvtp24a16x2.ll   aep6, a13, a14
    ae_cvtp24a16x2.ll   aep7, a12, a13
    ae_lp16f.i  aep4, a11, -16
    ae_lp16f.i  aep1, a5, -16
    ae_lp16f.i  aep2, a7, -16
    srai    a5, a6, 1
    ae_sp16f.l.i    aep2, a1, 4
    ae_sp16f.l.i    aep1, a1, 6
    addi    a7, a4, -36
    ae_sp16f.l.i    aep4, a1, 0
    l16si   a12, a2, 22
    l16si   a11, a2, 24
    l16si   a14, a2, 2
    ae_cvtp24a16x2.ll   aep3, a14, a15
    l16si   a13, a2, 20
    ae_cvtp24a16x2.ll   aep5, a10, a11
    addi    a3, a2, 36
    ae_lp16f.i  aep0, a3, -16
    ae_cvtp24a16x2.ll   aep4, a12, a13
    addi    a3, a8, -2
    ae_sp16f.l.i    aep0, a1, 8
    ae_lp16x2f.iu   aep0, a7, 4
    nop
    nop
    loopgtz a5, .Residu_0x182
    { ae_lp16x2f.i  aep1, a7, 4; ae_mulzaafp24s.hh.ll   aeq0, aep0, aep7 }

.Residu_0xd8:
    { ae_lp16x2f.i  aep0, a1, 0; ae_mulfs32p16s.lh  aeq1, aep0, aep7 }

.Residu_0xe0:
    { ae_lp16x2f.i  aep2, a7, 8; ae_mulaafp24s.hh.ll    aeq1, aep1, aep6 }

.Residu_0xe8:
    { ae_lp16x2f.i  aep1, a1, 4; ae_mulaafp24s.hh.ll    aeq0, aep1, aep0 }
    { ae_lp16x2f.i  aep1, a7, 12; ae_mulaafp24s.hh.ll   aeq0, aep2, aep1 }

.Residu_0xf8:
    { ae_lp16x2f.i  aep0, a1, 8; ae_mulaafp24s.hh.ll    aeq1, aep2, aep5 }
    { ae_lp16x2f.i  aep2, a9, 0; ae_mulaafp24s.hh.ll    aeq1, aep1, aep4 }

 .Residu_0x108:
    { ae_lp16x2f.i  aep1, a7, 16; ae_mulaafp24s.hh.ll   aeq0, aep1, aep0 }

.Residu_0x110:
    { ae_lp16x2f.i  aep0, a1, 12; ae_mulaafp24s.hh.ll   aeq1, aep1, aep2 }

.Residu_0x118:
    { ae_lp16x2f.i  aep1, a9, 4; ae_mulaafp24s.hh.ll    aeq0, aep1, aep0 }

.Residu_0x120:
    ae_lp16x2f.i    aep0, a7, 20

.Residu_0x123:
    { ae_lp16x2f.i  aep1, a1, 16; ae_mulaafp24s.hh.ll   aeq1, aep0, aep1 }
    { ae_lp16x2f.i  aep1, a9, 8; ae_mulaafp24s.hh.ll    aeq0, aep0, aep1 }

.Residu_0x133:
    ae_lp16x2f.i    aep0, a7, 24

.Residu_0x136:
    { ae_lp16x2f.i  aep1, a1, 20; ae_mulaafp24s.hh.ll   aeq1, aep0, aep1 }
    { ae_lp16x2f.iu aep1, a7, 28; ae_mulaafp24s.hh.ll   aeq0, aep0, aep1 }

.Residu_0x146:
    ae_lp16x2f.i    aep0, a9, 12

.Residu_0x149:
    { ae_lp16x2f.i  aep0, a1, 24; ae_mulaafp24s.hh.ll   aeq1, aep1, aep0 }
    { ae_lp16x2f.i  aep1, a7, 4; ae_mulaafp24s.hh.ll    aeq0, aep1, aep0 }

.Residu_0x159:
    { ae_lp16x2f.iu aep0, a7, -24; ae_mulaafp24s.hh.ll  aeq1, aep1, aep3 }
    { nop; ae_mulafs56p24s.hl   aeq0, aep1, aep3 }
    ae_sllssq56s    aeq1, aeq1
    ae_sllssq56s    aeq0, aeq0
    { nop; ae_roundsp16q48asym  aep1, aeq0 }

.Residu_0x177:
    { ae_sp16f.l.i  aep1, a3, 2; ae_roundsp16q48asym    aep1, aeq1 }

.Residu_0x17f:
    ae_sp16f.l.iu   aep1, a3, 4

.Residu_0x182:
    retw.n
