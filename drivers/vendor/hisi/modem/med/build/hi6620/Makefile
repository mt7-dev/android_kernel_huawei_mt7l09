
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).)

# guess the top dir
BALONG_TOPDIR ?= $(patsubst %/modem/med/build/hi6620/hifi,%,$(CURDIR))

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
.PHONY:all mcore acore bootrom bootloader
all: mcore acore bootrom bootloader
	@echo do [$@]
	@echo "DSP_CODE_GU compile successfully."
mcore : 
	$(MAKE)  $(OBB_JOBS) -f makefile_codec.mk
bootrom :
	@echo do [$@]
acore : 
	@echo do [$@]
bootloader : 
	@echo do [$@]

# clean
.PHONY: clean
clean:
	$(MAKE)  $(OBB_JOBS)  -f makefile_codec.mk $@
	$(Q)echo "COMM_CODE clean complete."
