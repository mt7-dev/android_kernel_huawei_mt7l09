
# What are targets ?
$(warning The targets are $(MAKECMDGOALS).)

# guess the Top dir
BALONG_TOPDIR ?= $(patsubst %/build,%,$(CURDIR))
$(warning BALONG_TOPDIR is "$(BALONG_TOPDIR)")

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

export OUT_ANDROID := $(OBB_ANDROID_DIR)/out/target/product/balongv7r2/obj

# Used to find valid targets.
TOP_MOUDULES_DIR := $(filter-out system,$(shell ls $(BALONG_TOPDIR)))
MOUDULES_DIR := $(filter-out system,$(shell ls $(BALONG_TOPDIR)/modem))
MOUDULES_FULL_DIR := $(addprefix $(BALONG_TOPDIR)/modem/,$(MOUDULES_DIR))

MERGE_TOOL_PATH         := $(BALONG_TOPDIR)/build/tools/utility/postlink/merge/merge_dsp.py
PACKHEAD_TOOL_PATH      := $(BALONG_TOPDIR)/build/tools/utility/postlink/packHead/packHead.py

MOUDULES :=
# group 1, null

# group 2, distcc not suppored
MOUDULES += $(if $(findstring phy,$(MOUDULES_DIR)), gudsp, )

ifeq ($(CFG_HIFI_INCLUDE),YES)
MOUDULES += $(if $(findstring med,$(MOUDULES_DIR)), guhifi, )
endif

MOUDULES += $(if $(findstring drv,$(MOUDULES_DIR)), drv, )
MOUDULES += $(if $(findstring ps,$(MOUDULES_DIR)), gups, )
MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), gumsp, )
MOUDULES += $(if $(findstring taf,$(MOUDULES_DIR)), taf, )

ifneq ($(CFG_RAT_MODE),RAT_GU)
ifneq ($(lphy),false)
MOUDULES += $(if $(findstring phy,$(MOUDULES_DIR)), lphy, )
endif
#MOUDULES += $(if $(findstring tools,$(MOUDULES_DIR)), hso, )
MOUDULES += $(if $(findstring config,$(TOP_MOUDULES_DIR)), nv, )
# group 3
MOUDULES += $(if $(findstring ps,$(MOUDULES_DIR)), lps, )
MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), lmsp, )
endif

ifeq ($(win32),true)
MOUDULES += $(if $(findstring tools/source/lt/hso,$(MOUDULES_DIR)), hso, )
endif

ifeq ($(strip $(OBB_SEPARATE)),true)
MOUDULES := drv
endif

ifneq ($(ROOT_WIND_PATH_W),"C:/WindRiver")
LOCAL_MACHINE_IP_ADDR := $(shell ifconfig eth0|grep 'inet addr')
LOCAL_MACHINE_IP_ADDR := $(word 3,$(subst :, ,$(LOCAL_MACHINE_IP_ADDR)))
ifeq ($(LOCAL_MACHINE_IP_ADDR),)
$(warning "Cannot transfer package")
LOCAL_MACHINE_IP_ADDR :="WRONG IP CONFIG"
endif
endif


# tag file 
MANIFEST_PATH := $(OBB_ANDROID_DIR)/.repo/manifests
TAG_FILE_PATH  := $(MANIFEST_PATH)/$(OBB_TAG_NAME).xml
IS_TAGFILE_EXIST := $(filter $(OBB_TAG_NAME).xml, $(shell ls $(MANIFEST_PATH)))

define generate_product_tag_id
$(shell cd $(MANIFEST_PATH) && git log -1 --pretty=oneline)
endef

CFG_PRODUCT_TAG_ID =$(call generate_product_tag_id)

VERSION_H_FILE_PATH :=$(BALONG_TOPDIR)/drv/common/include/bsp_version.h

# This is the default target.  It must be the first declared target.
.PHONY: balong
DEFAULT_GOAL := balong
$(DEFAULT_GOAL):

# componnets
.PHONY: prebuild comm acore lps gups lmsp gumsp lphy gudsp drv hso taf os_ccore mcore nv nvim guhifi_voice
prebuild :
	$(Q)mkdir -p $(OBB_PRODUCT_DELIVERY_DIR)/log $(OBB_PRODUCT_DELIVERY_DIR)/lib \
				 $(OBB_PRODUCT_DELIVERY_DIR)/obj $(OBB_PRODUCT_DELIVERY_DIR)/image \
				 $(OBB_PRODUCT_DELIVERY_DIR)/os
ifneq ($(OBB_TAG_NAME),)
ifneq ($(OBB_TAGM_CONTENT),)
ifneq ($(IS_TAGFILE_EXIST), )
$(error "$(OBB_TAG_NAME)".xml has already been exist . Please input another tag name)
else
	cd $(MANIFEST_PATH);repo manifest -o $(OBB_TAG_NAME).xml;\
	git add $(OBB_TAG_NAME).xml;git commit -m $(OBB_TAGM_CONTENT)
endif
endif
endif

# comm
#pragma runlocal
comm : prebuild
#	$(MAKE) -C $(BALONG_TOPDIR)/build/libs/os

ifneq ($(OBB_TAGM_CONTENT),) 
	@echo $(CFG_PRODUCT_TAG_ID)
	$(Q)sed -i 's,#define CFG_PRODUCT_TAG_ID,#define CFG_PRODUCT_TAG_ID1,g' $(VERSION_H_FILE_PATH) && \
	$(Q)sed -i '/CFG_PRODUCT_TAG_ID1/a #define CFG_PRODUCT_TAG_ID "$(CFG_PRODUCT_TAG_ID)"' $(VERSION_H_FILE_PATH) && \
	$(Q)sed -i '/#define CFG_PRODUCT_TAG_ID1/d' $(VERSION_H_FILE_PATH)
endif

nv : comm
	$(MAKE) -C $(BALONG_TOPDIR)/config/nvim/build

os_ccore:comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/build/libs/os/

# ccore
# lte
lphy : comm
	$(MAKE) -C $(BALONG_TOPDIR)/modem/phy/build/lt
lps : comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/ps/build/tl/ 
lmsp : comm
	$(MAKE)  $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/oam/build/lt/ 
drv : os_ccore
	$(MAKE) $(OBB_AKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/drv/build/ccore/

ifeq ($(win32),true)
hso : nvim
	$(MAKE) -f $(OBB_SCRIPTS_DIR)/make_ccore_sync.mk hso
else
hso : prebuild
	$(MAKE) -C $(BALONG_TOPDIR)/tools/source/lt/hso/Build_LTE/
endif

# gu
gups : comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/ps/build/gu/ 
gumsp : comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/oam/build/gu/ 
gudsp : comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/phy/build/gu/
	
guhifi guhifi_voice: comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/med/build/hi6930/ $@
	
taf   : comm
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/modem/taf/build/gu/

$(warning The moudles are $(MOUDULES).)

# acore 
acore : prebuild
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $(maction)

# acore separate compile: acore-drv/acore-lmsp/acore-lps/acore-gumsp/acore-gups
acore-%: prebuild
	@echo do[$@]
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $*

# android independent image
%image %.img : prebuild
	@echo do[$@]
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $@

%.mk %akefile :
	@echo nothing to be done for [$@]

# Last-Resort Default Rules
%:: prebuild
	$(warning do[$@])
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $@

#mcore
mcore : prebuild
	$(MAKE) -C $(BALONG_TOPDIR)/modem/drv/build/mcore/ $(maction)

# make libs pkg transfer...
.PHONY:  libs acore_libs ccore_libs mcore_libs pkg trans_file ccore

ccore : os_ccore $(MOUDULES) 
	@echo do $@
	
ccore_libs : ccore
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR) -f make_libs.mk 
	
acore_libs : acore
#	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR) -f make_libs_acore.mk

mcore_libs : mcore
	@echo nothing to be done for [$@]

ifeq ($(strip $(OBB_CM3)),true)
libs_depends := $(if $(findstring drv,$(MOUDULES_DIR)), mcore_libs, )
libs_depends += $(if $(findstring drv,$(MOUDULES_DIR)), acore_libs, )
else
libs_depends := $(if $(findstring drv,$(MOUDULES_DIR)), acore_libs, )
endif
libs_depends += $(if $(findstring ps,$(MOUDULES_DIR)), acore_libs, )
libs_depends += $(if $(findstring oam,$(MOUDULES_DIR)), acore_libs, )
libs_depends := $(sort ccore_libs $(libs_depends))
ifneq ($(strip $(OBB_CORE_NAME)),all)
libs_depends := $(OBB_CORE_NAME)_libs
endif

libs : $(libs_depends)
	@echo do [$@]
	
pkg : libs 
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR) -f make_pkg.mk
ifeq ($(CFG_HIFI_INCLUDE),YES)
	$(Q)-python $(MERGE_TOOL_PATH) $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin $(OBB_PRODUCT_DELIVERY_DIR)/lib/guhifi_mcore.bin $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy_hifi.bin.forpack
	$(Q)-python $(PACKHEAD_TOOL_PATH) DSP $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy_hifi.bin.forpack $(OBB_PRODUCT_DELIVERY_DIR)/image/lphy.bin 0x0
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy_hifi.bin.forpack
else
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
endif
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/nv.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/ucos.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-cp -f  $(BALONG_TOPDIR)/tools/source/lt_equip/netstone/bin/HiTdsCBT.exe $(OBB_PRODUCT_DELIVERY_DIR)/image

ifeq ($(nolink),true)
trans_file : $(MAKECMDGOALS)
	$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
ifeq ($(CFG_HIFI_INCLUDE),YES)
	$(Q)-python $(MERGE_TOOL_PATH) $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin $(OBB_PRODUCT_DELIVERY_DIR)/lib/guhifi_mcore.bin $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin.forpack
	$(Q)-python $(PACKHEAD_TOOL_PATH) DSP $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin.forpack $(OBB_PRODUCT_DELIVERY_DIR)/image/lphy.bin 0x0
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin.forpack
else
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
endif
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/nv.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-mv -f  $(OBB_PRODUCT_DELIVERY_DIR)/lib/ucos.bin $(OBB_PRODUCT_DELIVERY_DIR)/image/
	$(Q)-cp -f  $(BALONG_TOPDIR)/modem/drv/common/scripts/burn_$(CFG_PLATFORM).bat $(OBB_PRODUCT_DELIVERY_DIR)/image/burn.bat
	$(Q)-cp -f  $(BALONG_TOPDIR)/modem/drv/common/scripts/burn_sec_$(CFG_PLATFORM).bat $(OBB_PRODUCT_DELIVERY_DIR)/image/burn_sec.bat
	$(Q)-cp -f  $(BALONG_TOPDIR)/tools/source/lt_equip/netstone/bin/HiTdsCBT.exe $(OBB_PRODUCT_DELIVERY_DIR)/img
ifneq ($(hs_sh),true)
	$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
	$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) \
    $(BALONG_TOPDIR)/build/tools/build_agent/build_agent $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME)  $(target_ci)
endif

else
trans_file : |pkg
ifneq ($(hs_sh),true)
	$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
	$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) \
    $(BALONG_TOPDIR)/build/tools/build_agent/build_agent $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME)  $(target_ci)
endif

trans:
ifneq ($(hs_sh),true)
	$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
	$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) \
    $(BALONG_TOPDIR)/build/tools/build_agent/build_agent $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME)  $(target_ci)
endif
endif
# this is the goal.
balong : trans_file
	@echo balong is Ready.

# maction
maction : acore mcore lmsp drv
	@echo maction=$(maction) is Ready.

# clean is so.....................................................
CLEAN_MOUDULES :=
# group 1, null

# group 2, distcc not suppored
CLEAN_MOUDULES += $(if $(findstring phy,$(MOUDULES_DIR)), clean-gudsp, )
CLEAN_MOUDULES += $(if $(findstring phy,$(MOUDULES_DIR)), clean-lphy, )
ifeq ($(win32),true)
MOUDULES += $(if $(findstring tools/source/lt/hso,$(MOUDULES_DIR)), hso, )
endif
# group 3
CLEAN_MOUDULES += $(if $(findstring drv,$(MOUDULES_DIR)), clean-drv , )
CLEAN_MOUDULES += $(if $(findstring ps,$(MOUDULES_DIR)), clean-gups, )
CLEAN_MOUDULES += $(if $(findstring ps,$(MOUDULES_DIR)), clean-lps, )
CLEAN_MOUDULES += $(if $(findstring config,$(MOUDULES_DIR)), clean-nv clean-os-ccore, )
CLEAN_MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), clean-lmsp , )
CLEAN_MOUDULES += $(if $(findstring oam,$(MOUDULES_DIR)), clean-gumsp, )
CLEAN_MOUDULES += $(if $(findstring taf,$(MOUDULES_DIR)), clean-taf, )

ifeq ($(strip $(OBB_SEPARATE)),true)
CLEAN_MOUDULES := $(if $(findstring drv,$(MOUDULES_DIR)), clean-drv, )
#CLEAN_MOUDULES += $(if $(findstring config,$(MOUDULES_DIR)), clean-nv clean-os-ccore, )
endif
$(warning The CLEAN_MOUDULES are $(CLEAN_MOUDULES).)

.PHONY:clean clean-comm clean-lps clean-gups clean-lmsp clean-gumsp clean-lphy clean-gudsp clean-guhifi clean-drv clean-mcore clean-hso \
		clean-acore clean-acore-libs clean-ccore-libs clean-pkg clean-everything

clean-pkg:
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR) -f make_pkg.mk clean

clean-ccore-libs: clean-pkg
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR) -f make_libs_ccore.mk clean
clean-acore-libs: clean-pkg
	$(MAKE)  $(OBB_JOBS)  -C $(OBB_SCRIPTS_DIR) -f make_libs_acore.mk clean

clean-os-ccore:
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/build/libs/os clean
	
clean-gudsp:clean-pkg
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/phy/build/gu/ clean
clean-hso: clean-pkg
	$(MAKE) -f $(OBB_SCRIPTS_DIR)/make_ccore_sync.mk clean-win32

clean-drv:clean-ccore-libs  clean-os-ccore clean-mcore
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/drv/build/ccore/ clean
clean-lmsp:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/oam/build/lt/ clean
	
clean-lps:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/ps/build/tl/ clean
	
clean-lphy:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/phy/build/lt clean

clean-guhifi:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/med/build/hi6930/ clean
	
clean-nas clean-gas clean-was clean-ttf clean-ascomm clean-pscomm clean-gups:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/ps/build/gu/ MODULE=$@
	
clean-taf:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/taf/build/gu/ clean
clean-gumsp:clean-ccore-libs
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/modem/oam/build/gu/ clean
	
clean-acore:
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk clean
	
clean-ccore:
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/ccore/ clean
	
clean-mcore:
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/mcore/ clean

clean-comm:
#	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/config/nvim/build/ clean
	
clean-nv:
	$(MAKE) $(OBB_JOBS) -C $(BALONG_TOPDIR)/config/nvim/build/ clean

clean: $(CLEAN_MOUDULES) clean-comm clean-acore clean-mcore clean-ccore
	@echo "Clean the targets: $(CLEAN_MOUDULES)."

distclean: clean clean-acore
	$(Q)-rm -rf  $(OBB_PRODUCT_DELIVERY_DIR)/lib \
				  $(OBB_PRODUCT_DELIVERY_DIR)/obj $(OBB_PRODUCT_DELIVERY_DIR)/img \
				  $(OBB_PRODUCT_DELIVERY_DIR)/os $(OBB_PRODUCT_DELIVERY_DIR)/image
	$(Q)-rm -rf  $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD 
	$(Q)-rm -f $(OBB_PRODUCT_DELIVERY_DIR)/*.txt			  
#	$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)
	@echo "Entire build directory removed."
	 
.PHONY: help
help:
	@echo make product=hi6930cs_p531_fpga 
	@echo make product=hi6930cs_p531_fpga clean

.PHONY:cmd
cmd:
	$(MAKE)  $(OBB_JOBS)  -C $(BALONG_TOPDIR)/$(mdir) -f $(mfile) $(goals)
.PHONY: showcommands
showcommands:
	@echo >/dev/null
	
# Used to force goals to build.  Only use for conditionally defined goals.
.PHONY: FORCE
FORCE:;
