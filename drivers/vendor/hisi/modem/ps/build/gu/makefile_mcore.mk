
#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

MODULES_DIR := $(filter-out nas build comm inc,$(shell ls $(BALONG_TOPDIR)/modem/ps))

MOUDULES:= pscomm nas 
MOUDULES += $(if $(findstring as,$(MODULES_DIR)),gas was ttf ascomm, )
	
# process
.PHONY: gups nas gas was ttf pscomm ascomm
gups: $(MOUDULES)

nas:
	$(MAKE)  -f MODEM_CORE/makefile_nas.mk INSTANCE_ID=$(INSTANCE_ID)
	$(MAKE)  -f MODEM_CORE/makefile_rabm.mk INSTANCE_ID=$(INSTANCE_ID)
gas:
	$(MAKE)  -f MODEM_CORE/makefile_gas.mk	INSTANCE_ID=$(INSTANCE_ID)
was:
	$(MAKE)  -f MODEM_CORE/makefile_was.mk	INSTANCE_ID=$(INSTANCE_ID)
ttf:
	$(MAKE)  -f MODEM_CORE/makefile_ttf.mk INSTANCE_ID=$(INSTANCE_ID)
ascomm:
	$(MAKE)  -f MODEM_CORE/makefile_ascomm.mk INSTANCE_ID=$(INSTANCE_ID)
pscomm:
	$(MAKE)  -f MODEM_CORE/makefile_pscomm.mk INSTANCE_ID=$(INSTANCE_ID)

CLEAN_MOUDULES:= clean-nas clean-pscomm
CLEAN_MOUDULES += $(if $(findstring as,$(MODULES_DIR)),clean-gas clean-was clean-ttf clean-ascomm, )

.PHONY: clean-gups clean-nas clean-gas clean-was clean-ttf clean-pscomm clean-ascomm

clean-nas:
	$(MAKE)  -f MODEM_CORE/makefile_nas.mk clean INSTANCE_ID=$(INSTANCE_ID)
	$(MAKE)  -f MODEM_CORE/makefile_rabm.mk clean INSTANCE_ID=$(INSTANCE_ID)
clean-gas:
	$(MAKE)  -f MODEM_CORE/makefile_gas.mk	clean INSTANCE_ID=$(INSTANCE_ID)
clean-was:
	$(MAKE)  -f MODEM_CORE/makefile_was.mk	clean INSTANCE_ID=$(INSTANCE_ID)
clean-ttf:
	$(MAKE)  -f MODEM_CORE/makefile_ttf.mk clean INSTANCE_ID=$(INSTANCE_ID)
clean-ascomm:
	$(MAKE)  -f MODEM_CORE/makefile_ascomm.mk clean INSTANCE_ID=$(INSTANCE_ID)
clean-pscomm:
	$(MAKE)  -f MODEM_CORE/makefile_pscomm.mk clean INSTANCE_ID=$(INSTANCE_ID)

clean-gups: $(CLEAN_MOUDULES)
