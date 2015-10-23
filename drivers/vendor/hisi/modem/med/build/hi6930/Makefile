
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).)

# guess the top dir
BALONG_TOPDIR ?= $(patsubst %/modem/med/build/hi6930,%,$(CURDIR))

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
.PHONY:all mcore mcore_libs guhifi
all guhifi: mcore_libs

mcore_libs: mcore
	$(MAKE)  -f makefile_mcore_libs.mk

mcore : 
	$(MAKE)  -f makefile_mcore.mk

.DEFAULT:
	$(MAKE)  -f makefile_mcore.mk $@
	
# clean
.PHONY: clean
clean:
	$(MAKE)  -f makefile_mcore_libs.mk clean
	$(MAKE)  -f makefile_mcore.mk clean

