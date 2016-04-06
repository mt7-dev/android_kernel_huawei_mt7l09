	.text
		
	# Program Unit: AMRWB_Convolve
	.type	AMRWB_Convolve, @function
	.align	 4
	.global	AMRWB_Convolve
AMRWB_Convolve:	# 0x78
    entry	a1, 32
    movi.n	a8, 0
    movi.n	a7, 4
    or	a9, a2, a2
    ae_lp16x2f.iu	aep2, a9, 0
    or	a2, a3, a3
    { ae_lp16x2f.iu	aep3, a2, 0; ae_zeroq56	aeq3 }
    { movi	a12, -124; ae_mulfs32p16s.hh	aeq1, aep2, aep3 }
    { addi	a10, a4, -2; ae_mulzaafp24s.hl.lh	aeq0, aep2, aep3 }
    { movi	a3, 0; ae_roundsp16q48asym	aep1, aeq1 }
    { ae_sp16f.l.iu	aep1, a10, 2; ae_roundsp16q48asym	aep0, aeq0 }
    { ae_sp16f.l.iu	aep0, a10, 2; ae_movq56	aeq0, aeq3 }

.Convolve_0x40:
    ae_lp16x2f.xu	aep1, a2, a7
    ae_lp16x2f.xu	aep0, a9, a8
    { addi	a11, a3, 1; ae_mulzaafp24s.hl.lh	aeq1, aep0, aep1 }
    nop
    nop.n
    nop.n
    loopgtz	a11, .Convolve_0x70

.Convolve_0x58:
    { ae_lp16x2f.iu	aep1, a2, -4; ae_mulafs32p16s.hh	aeq0, aep0, aep1 }
    { ae_lp16x2f.iu	aep0, a9, 4; ae_mulafs32p16s.ll	aeq0, aep0, aep1 }

.Convolve_0x68:
    { nop; ae_mulaafp24s.hl.lh	aeq1, aep0, aep1 }

.Convolve_0x70:
    { or	a3, a11, a11; ae_mulafs32p16s.hh	aeq0, aep0, aep1 }
    addi.n	a7, a7, 4
    { addi	a8, a8, -4; ae_roundsp16q48asym	aep5, aeq0 }
    { ae_sp16f.l.iu	aep5, a10, 2; ae_roundsp16q48asym	aep4, aeq1 }
    ae_sp16f.l.iu	aep4, a10, 2
    { bne	a8, a12, .Convolve_0x40; ae_movq56	aeq0, aeq3 }
    retw.n
