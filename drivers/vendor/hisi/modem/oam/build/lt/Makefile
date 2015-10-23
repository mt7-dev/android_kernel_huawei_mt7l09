
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).)

# guess the Top dir
BALONG_TOPDIR ?= $(patsubst %oam,%,$(CURDIR))

# Make sure that there are no spaces in the absolute path; the build system can't deal with them.
ifneq ($(words $(BALONG_TOPDIR)),1)
$(warning ************************************************************)
$(warning You are building in a directory whose absolute path contains a space character)
$(warning "$(BALONG_TOPDIR)")
$(warning Please move your source tree to a path that does not contain any spaces.)
$(warning ************************************************************)
$(error Directory names containing spaces not supported)
endif

# include the define at the top
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

# process
.PHONY:all ccore ccore1
all: ccore ccore1
	@echo do [$@]
	
ccore :
	$(MAKE)   -f makefile_ccore.mk INSTANCE_ID=INSTANCE_0
ccore1 :
ifeq ($(CFG_FEATURE_MULTI_MODEM) ,FEATURE_ON)
	$(MAKE)  -f makefile_ccore.mk INSTANCE_ID=INSTANCE_1
endif


# clean
.PHONY: clean
clean:
ifeq ($(OBB_CORE_NAME),all)
	$(MAKE)   -f makefile_ccore.mk $@ INSTANCE_ID=INSTANCE_0
	#$(MAKE) -f makefile_acore.mk $@
else
	$(MAKE)   -f makefile_$(OBB_CORE_NAME).mk $@ INSTANCE_ID=INSTANCE_0
endif

ifeq ($(CFG_FEATURE_MULTI_MODEM) ,FEATURE_ON)
	$(MAKE)   -f makefile_ccore.mk $@ INSTANCE_ID=INSTANCE_1
endif
	
	@echo "MSP_CODE clean complete."
