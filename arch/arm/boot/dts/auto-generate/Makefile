dtb-y += JAZZ_SGLTEU_V1_config.dtb
dtb-y += JAZZ_TLG_VN1_config.dtb
dtb-y += JAZZ_TLG_V3_config.dtb
dtb-y += JAZZ_TLG_VN2_config.dtb
dtb-y += JAZZ_ULCA_VN1_config.dtb
dtb-y += JAZZ_ULCA_V3_config.dtb
dtb-y += JAZZ_CLG_V1_config.dtb
dtb-y += JAZZ_TLG_V1_config.dtb
dtb-y += JAZZ_ULG_VN1_config.dtb
dtb-y += JAZZ_CLG_V3_config.dtb
dtb-y += JAZZ_ULCA_V1_config.dtb
dtb-y += JAZZ_ULG_V3_config.dtb
dtb-y += JAZZ_ULG_V1_config.dtb
dtb-y += JAZZ_ULCA_JAPAN_config.dtb
dtb-y += JAZZ_CLG_VN1_config.dtb

targets += dtbs

dtbs: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb
