#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

MODULES_DIR := $(shell ls $(BALONG_TOPDIR)/modem/med)

MODULES:= guhifi_custom
MODULES += $(if $(findstring hi6930,$(MODULES_DIR)),guhifi_common, )
MODULES += $(if $(findstring voice,$(MODULES_DIR)), guhifi_voice, )
	
# process
.PHONY: all guhifi_custom guhifi_common guhifi_voice

all: $(MODULES)

guhifi_voice:
	$(MAKE)  -f makefile_guhifi_voice.mk 
	
guhifi_custom:
	$(MAKE)  -f makefile_guhifi_custom.mk 
guhifi_common:
	$(MAKE)  -f makefile_guhifi_common.mk 

CLEAN_MOUDULES:= clean-guhifi_custom
CLEAN_MOUDULES += $(if $(findstring hi6930,$(MODULES_DIR)),clean-guhifi_common, )
CLEAN_MOUDULES += $(if $(findstring voice,$(MODULES_DIR)),clean-guhifi_voice, )


.PHONY: clean clean-hifi_custom clean-guhifi_common clean-guhifi_voice

clean-guhifi_custom:
	$(MAKE)  -f makefile_guhifi_custom.mk clean
	
clean-guhifi_common:
	$(MAKE)  -f makefile_guhifi_common.mk clean	
	
clean-guhifi_voice:
	$(MAKE)  -f makefile_guhifi_voice.mk clean	
	
clean:$(CLEAN_MOUDULES)
