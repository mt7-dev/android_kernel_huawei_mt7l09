
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).)

# guess the Top dir
BALONG_TOPDIR ?= $(patsubst %build/scripts,%,$(CURDIR))
SC_DIR		  :=$(BALONG_TOPDIR)/build/tools/utility/postlink/makeFirmware

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
.PHONY:all ccore ccore1_rename $(maction)
all: ccore
	@echo do [$@]
	
ccore : ccore1_rename
ifeq ($(CFG_DUMMY_ENABLE),YES)
	$(Q)cp -f $(SC_DIR)/dummy  $(OBB_PRODUCT_DELIVERY_DIR)/lib/dummy
endif
ifeq ($(CFG_FEATURE_IMS) ,FEATURE_ON)
	$(Q)cp -f $(BALONG_TOPDIR)/build/libs/IMS/hw_sdk.o $(OBB_PRODUCT_DELIVERY_DIR)/lib
endif
	$(MAKE)  $(OBB_JOBS)  -f make_libs_ccore.mk
	
ccore1_rename :
ifeq ($(CFG_FEATURE_MULTI_MODEM) ,FEATURE_ON)
	$(MAKE)  -f make_ccore1_rename.mk
endif

# clean
.PHONY: clean
clean:
ifeq ($(OBB_CORE_NAME),all)
	$(MAKE)  $(OBB_JOBS)  -f make_libs_ccore.mk $@
	
else
	$(MAKE)  $(OBB_JOBS)  -f make_libs_$(OBB_CORE_NAME).mk $@
endif
	$(Q)echo "make libs clean complete."

.PHONY : $(maction)
$(maction) :
	@echo do $(maction)