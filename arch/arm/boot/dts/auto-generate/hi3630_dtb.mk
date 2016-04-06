#Copyright Huawei Technologies Co., Ltd. 1998-2011. All rights reserved.
#This file is Auto Generated 

dtb-y += hi3630/PINE_ULG_V4_config.dtb
dtb-y += hi3630/PINE_TLGULG_OUTSEA_EMMC32G_config.dtb
dtb-y += hi3630/PINE_TLG_V4_noNFC_config.dtb
dtb-y += hi3630/PINE_TLGULG_V4_NEW_config.dtb
dtb-y += hi3630/PINE_TLG_V4_config.dtb
dtb-y += hi3630/PINE_TLGULG_JAPAN_EMMC32G_config.dtb
dtb-y += hi3630/PINE_TLGULG_V3_EMMC64G_config.dtb
dtb-y += hi3630/PINE_ULG_V4_noNFC_config.dtb
dtb-y += hi3630/PINE_TLGULG_V1_config.dtb
dtb-y += hi3630/PINE_CLG_VN1_noNFC_config.dtb
dtb-y += hi3630/PINE_TLG_V1_config.dtb
dtb-y += hi3630/PINE_ULG_V1_config.dtb
dtb-y += hi3630/PINE_TLGULG_V4_EMMC32G_config.dtb
dtb-y += hi3630/PINE_CLG_V1_noNFC_config.dtb
dtb-y += hi3630/JAZZ_SGLTEU_V1_config.dtb
dtb-y += hi3630/JAZZ_TLG_VN1_config.dtb
dtb-y += hi3630/JAZZ_TLG_V3_config.dtb
dtb-y += hi3630/JAZZ_TLG_VN2_config.dtb
dtb-y += hi3630/JAZZ_ULCA_VN1_config.dtb
dtb-y += hi3630/JAZZ_ULCA_V3_config.dtb
dtb-y += hi3630/JAZZ_CLG_V1_config.dtb
dtb-y += hi3630/JAZZ_TLG_V1_config.dtb
dtb-y += hi3630/JAZZ_ULG_VN1_config.dtb
dtb-y += hi3630/JAZZ_CLG_V3_config.dtb
dtb-y += hi3630/JAZZ_ULCA_V1_config.dtb
dtb-y += hi3630/JAZZ_ULG_V3_config.dtb
dtb-y += hi3630/JAZZ_ULG_V1_config.dtb
dtb-y += hi3630/JAZZ_ULCA_JAPAN_config.dtb
dtb-y += hi3630/JAZZ_CLG_VN1_config.dtb
dtb-y += hi3630/hi3630_udp_config.dtb
dtb-y += hi3630/MOGOLIA_ULG_V3_noNFC_config.dtb
dtb-y += hi3630/MOGOLIA_TL_V3_noNFC_config.dtb
dtb-y += hi3630/MOGOLIA_TL_V1_config.dtb
dtb-y += hi3630/MOGOLIA_TL_2GFreq_config.dtb
dtb-y += hi3630/MOGOLIA_TL_V1_TRIAL_config.dtb
dtb-y += hi3630/MOGOLIA_ULCA_V1_noNFC_config.dtb
dtb-y += hi3630/MOGOLIA_TL_V3_config.dtb
dtb-y += hi3630/MOGOLIA_ULG_V3_config.dtb
dtb-y += hi3630/MOGOLIA_ULG_V1_config.dtb
dtb-y += hi3630/MOGOLIA_ULCA_GENERAL_V3_noNFC_config.dtb
dtb-y += hi3630/MOGOLIA_TL_CMCC_CERTIFICATION_noNFC_config.dtb
dtb-y += hi3630/MOGOLIA_ULCA_GENERAL_newRF_noNFC_config.dtb

targets += hi3630_dtb
targets += $(dtb-y)

# *.dtb used to be generated in the directory above. Clean out the
# old build results so people don't accidentally use them.
hi3630_dtb: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb

#end of file
