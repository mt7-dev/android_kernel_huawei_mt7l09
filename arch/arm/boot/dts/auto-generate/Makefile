#Copyright Huawei Technologies Co., Ltd. 1998-2011. All rights reserved.
#This file is Auto Generated 

dtb-y += BELLA_ULG_V3_config.dtb
dtb-y += BELLA_TLGULG_V3_config.dtb
dtb-y += BELLA_CLG_V3_config.dtb
dtb-y += fpga_config.dtb
dtb-y += emulator_config.dtb
dtb-y += udp_config.dtb
dtb-y += JAZZ_CLG_V1_config.dtb
dtb-y += JAZZ_ULCA_V1_config.dtb
dtb-y += JAZZ_ULG_V3_config.dtb
dtb-y += JAZZ_ULCA_VN1_config.dtb
dtb-y += JAZZ_TLG_VN2_config.dtb
dtb-y += JAZZ_ULG_V1_config.dtb
dtb-y += JAZZ_TLG_VN1_config.dtb
dtb-y += JAZZ_ULCA_V3_config.dtb
dtb-y += JAZZ_TLG_V3_config.dtb
dtb-y += JAZZ_CLG_VN1_config.dtb
dtb-y += JAZZ_SGLTEU_V1_config.dtb
dtb-y += JAZZ_TLG_V1_config.dtb
dtb-y += JAZZ_ULG_VN1_config.dtb
dtb-y += JAZZ_CLG_V3_config.dtb
dtb-y += MOGOLIA_ULG_V3_config.dtb
dtb-y += MOGOLIA_ULG_V1_config.dtb
dtb-y += MOGOLIA_TL_V1_config.dtb
dtb-y += MOGOLIA_ULG_V3_noNFC_config.dtb
dtb-y += MOGOLIA_ULCA_V1_noNFC_config.dtb
dtb-y += MOGOLIA_TL_CMCC_CERTIFICATION_noNFC_config.dtb
dtb-y += MOGOLIA_TL_2GFreq_config.dtb
dtb-y += MOGOLIA_ULCA_GENERAL_V3_noNFC_config.dtb
dtb-y += MOGOLIA_TL_V1_TRIAL_config.dtb
dtb-y += MOGOLIA_TL_V3_config.dtb
dtb-y += MOGOLIA_TL_V3_noNFC_config.dtb
dtb-y += PINE_ULG_V1_config.dtb
dtb-y += PINE_TLGULG_V3_EMMC64G_config.dtb
dtb-y += PINE_TLG_V1_config.dtb
dtb-y += PINE_TLGULG_V1_config.dtb

targets += dtbs
targets += $(dtb-y)

# *.dtb used to be generated in the directory above. Clean out the
# old build results so people don't accidentally use them.
dtbs: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb

#end of file
