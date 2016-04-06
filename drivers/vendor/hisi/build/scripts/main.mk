
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

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/phy/gu),)
MOUDULES += gudsp
endif

ifeq ($(CFG_HIFI_INCLUDE),YES)

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/med),)
MOUDULES += guhifi
endif
endif


ifneq ($(wildcard $(BALONG_TOPDIR)/modem/drv),)
MOUDULES += drv
endif

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps),)
MOUDULES += gups
endif

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/oam),)
MOUDULES += gumsp
endif

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/taf),)
MOUDULES += taf
endif
ifneq ($(CFG_RAT_MODE),RAT_GU)
ifneq ($(lphy),false)
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/phy/lt),)
MOUDULES += lphy
endif
endif
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/tools),)
#MOUDULES += hso
endif

ifneq ($(wildcard $(BALONG_TOPDIR)/config),)
MOUDULES += nv
endif
# group 3

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/ps),)
MOUDULES += lps
endif

ifneq ($(wildcard $(BALONG_TOPDIR)/modem/oam),)
MOUDULES += lmsp
endif

endif

ifeq ($(win32),true)
ifneq ($(wildcard $(BALONG_TOPDIR)/modem/tools/source/lt/hso),)
MOUDULES += hso
endif
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
MANIFEST_PATH := $(BALONG_TOPDIR)/../../.repo/manifests
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
	$(MAKE)  -C $(BALONG_TOPDIR)/build/libs/os/

# ccore
# lte
lphy : comm
	$(MAKE) -C $(BALONG_TOPDIR)/modem/phy/build/lt
lps : comm
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/ps/build/tl/ 
lmsp : comm
	$(MAKE)   -C $(BALONG_TOPDIR)/modem/oam/build/lt/ 
drv : os_ccore
	$(MAKE) $(OBB_AKE)  -C $(BALONG_TOPDIR)/modem/drv/build/ccore/

ifeq ($(win32),true)
hso : nvim
	$(MAKE) -f $(OBB_SCRIPTS_DIR)/make_ccore_sync.mk hso
else
hso : prebuild
	$(MAKE) -C $(BALONG_TOPDIR)/tools/source/lt/hso/Build_LTE/
endif

# gu
gups : comm
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/ps/build/gu/ 
gumsp : comm
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/oam/build/gu/ 
gudsp : comm
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/phy/build/gu/
	
guhifi guhifi_voice: comm
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/med/build/hi6930/ $@
	
taf   : comm
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/taf/build/gu/

$(warning The moudles are $(MOUDULES).)


ifneq ($(strip $(OBB_CORE_NAME)),all)
libs_kirin920L := $(OBB_CORE_NAME)_libs
else
libs_kirin920L := acore ccore_libs
endif


pkg_kirin920L : $(libs_kirin920L)
	$(MAKE)   -C $(OBB_SCRIPTS_DIR) -f make_pkg_920L.mk
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

# acore 
acore : prebuild
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $(maction)  1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION)

# acore separate compile: acore-drv/acore-lmsp/acore-lps/acore-gumsp/acore-gups
acore-%: prebuild
	@echo do[$@]
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $*

# android independent image
%image %.img : prebuild
	@echo do[$@]
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $@ 1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION)

lpm3libs :
	@echo do[$@]
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk $@ 1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION)

# mcu
mcu : prebuild
ifneq ($(wildcard $(BALONG_TOPDIR)/confidential/mcu),)
	$(MAKE) -C $(BALONG_TOPDIR)/confidential/mcu/build 1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION) 
endif 	
hipse : prebuild
ifneq ($(wildcard $(BALONG_TOPDIR)/confidential/hipse),)
	$(MAKE) -C $(BALONG_TOPDIR)/confidential/hipse 1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION) 
	cp -f $(OBB_PRODUCT_DELIVERY_DIR)/obj/hibench/hibench.bin $(OBB_PRODUCT_DELIVERY_DIR)/lib/
	cp -f $(OBB_PRODUCT_DELIVERY_DIR)/obj/hibench/hibench.elf $(OBB_PRODUCT_DELIVERY_DIR)/lib/
endif 	


hipse_hi3xxx : prebuild
ifneq ($(wildcard $(BALONG_TOPDIR)/confidential/hipse_hi3xxx/makefile),)
	$(MAKE) -C $(BALONG_TOPDIR)/confidential/hipse_hi3xxx 1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION) 
	cp -f $(OBB_PRODUCT_DELIVERY_DIR)/obj/hibench_hi3xxx/hibench.bin $(OBB_PRODUCT_DELIVERY_DIR)/lib/
	cp -f $(OBB_PRODUCT_DELIVERY_DIR)/obj/hibench_hi3xxx/hibench.elf $(OBB_PRODUCT_DELIVERY_DIR)/lib/
else
	-cp -rf  $(BALONG_TOPDIR)/../../device/hisi/kirinlibs/$(OBB_PRODUCT_NAME)/hibench.bin  $(OBB_PRODUCT_DELIVERY_DIR)/lib/
	-cp -rf  $(BALONG_TOPDIR)/../../device/hisi/kirinlibs/$(OBB_PRODUCT_NAME)/hibench.elf  $(OBB_PRODUCT_DELIVERY_DIR)/lib/
endif 	
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
	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_libs.mk $(maction)  1>$(OBB_PRODUCT_DELIVERY_DIR)/log/obuild_$(@).log 2>&1 $(SERIAL_OPTION)
	
acore_libs : acore
#	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_libs_acore.mk

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
	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_pkg.mk
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
	$(Q)-rm -rf $(PWD)/delivery/$(OBB_PRODUCT_NAME).tar.gz
	$(Q)-cd $(OBB_PRODUCT_DELIVERY_DIR) && tar -zcvf $(PWD)/delivery/$(OBB_PRODUCT_NAME).tar.gz log lib image usb_dump_scripts
	$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file_new.py $(OBB_PRODUCT_NAME) $(BALONG_TOPDIR) $(BALONG_TOPDIR)/build/scripts/hibuild_transfer.py
	#$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file_new.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) $(BALONG_TOPDIR)/build/scripts/hibuild_transfer.py $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME) $(target_ci)
	#$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
	#$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) \
    #$(BALONG_TOPDIR)/build/tools/build_agent/build_agent $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME)  $(target_ci)
endif

else
trans_file : |pkg
ifneq ($(hs_sh),true)
	$(Q)-rm -rf $(PWD)/delivery/$(OBB_PRODUCT_NAME).tar.gz
	$(Q)-cd $(OBB_PRODUCT_DELIVERY_DIR) && tar -zcvf $(PWD)/delivery/$(OBB_PRODUCT_NAME).tar.gz log lib image usb_dump_scripts
	$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file_new.py $(OBB_PRODUCT_NAME) $(BALONG_TOPDIR) $(BALONG_TOPDIR)/build/scripts/hibuild_transfer.py
	#$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file_new.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) $(BALONG_TOPDIR)/build/scripts/hibuild_transfer.py $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME) $(target_ci)
	#$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
	#$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) \
    #$(BALONG_TOPDIR)/build/tools/build_agent/build_agent $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME)  $(target_ci)
endif

trans:
ifneq ($(hs_sh),true)
	$(Q)-rm -rf $(PWD)/delivery/$(OBB_PRODUCT_NAME).tar.gz
	$(Q)-cd $(OBB_PRODUCT_DELIVERY_DIR) && tar -zcvf $(PWD)/delivery/$(OBB_PRODUCT_NAME).tar.gz log lib image usb_dump_scripts
	$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file_new.py $(OBB_PRODUCT_NAME) $(BALONG_TOPDIR) $(BALONG_TOPDIR)/build/scripts/hibuild_transfer.py
	#$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file_new.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) $(BALONG_TOPDIR)/build/scripts/hibuild_transfer.py $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME) $(target_ci)
	#$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/UPLOAD/
	#$(Q) python $(BALONG_TOPDIR)/build/scripts/transfer_file.py $(CFG_PRODUCT_VERSION_STR) $(OBB_PRODUCT_NAME)  $(LOCAL_MACHINE_IP_ADDR) $(BALONG_TOPDIR) \
    #$(BALONG_TOPDIR)/build/tools/build_agent/build_agent $(OBB_PRODUCT_DELIVERY_DIR) $(OBB_TRANS_MATCH) $(RELEASE_PACKAGE_NAME)  $(target_ci)
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
	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_pkg.mk clean
clean-pkg_kirin920L:
	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_pkg_920L.mk clean

clean-ccore-libs: clean-pkg
	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_libs_ccore.mk clean
clean-acore-libs: clean-pkg
	$(MAKE)    -C $(OBB_SCRIPTS_DIR) -f make_libs_acore.mk clean

clean-os-ccore:
	$(MAKE)  -C $(BALONG_TOPDIR)/build/libs/os clean
	
clean-gudsp:clean-pkg
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/phy/build/gu/ clean
clean-hso: clean-pkg
	$(MAKE) -f $(OBB_SCRIPTS_DIR)/make_ccore_sync.mk clean-win32

clean-drv:clean-ccore-libs  clean-os-ccore clean-mcore
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/drv/build/ccore/ clean
clean-lmsp:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/oam/build/lt/ clean
	
clean-lps:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/ps/build/tl/ clean
	
clean-lphy:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/phy/build/lt clean

clean-guhifi:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/med/build/hi6930/ clean
	
clean-nas clean-gas clean-was clean-ttf clean-ascomm clean-pscomm clean-gups:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/ps/build/gu/ MODULE=$@
	
clean-taf:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/taf/build/gu/ clean
clean-gumsp:clean-ccore-libs
	$(MAKE)    -C $(BALONG_TOPDIR)/modem/oam/build/gu/ clean
	
clean-acore:
	$(MAKE)  -C $(OBB_SCRIPTS_DIR)/ -f main_acore.mk clean
	
clean-ccore:
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/ccore/ clean
	
clean-mcore:
	$(MAKE)  -C $(BALONG_TOPDIR)/modem/drv/build/mcore/ clean

clean-comm:
#	$(MAKE)    -C $(BALONG_TOPDIR)/config/nvim/build/ clean
	
clean-nv:
	$(MAKE)  -C $(BALONG_TOPDIR)/config/nvim/build/ clean

ifeq ($(CFG_OS_ANDROID_USE_K3V3_KERNEL),YES)
clean:  clean-acore  clean-pkg_kirin920L
else
ifeq ($(OBB_PRODUCT_NAME),hi6210sft)
clean: 
	@echo "Clean the targets: $(CLEAN_MOUDULES)."
else
clean: $(CLEAN_MOUDULES) clean-comm clean-acore clean-mcore clean-ccore
endif
endif
	@echo "Clean the targets: $(CLEAN_MOUDULES)."

distclean: clean clean-acore
	$(Q)-rm -rf  $(OBB_PRODUCT_DELIVERY_DIR)/lib \
				  $(OBB_PRODUCT_DELIVERY_DIR)/obj $(OBB_PRODUCT_DELIVERY_DIR)/img \
				  $(OBB_PRODUCT_DELIVERY_DIR)/os $(OBB_PRODUCT_DELIVERY_DIR)/image
	$(Q)-rm -rf  $(OBB_PRODUCT_DELIVERY_DIR)/../$(OBB_PRODUCT_NAME).tar.gz
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
	$(MAKE)    -C $(BALONG_TOPDIR)/$(mdir) -f $(mfile) $(goals)
.PHONY: showcommands
showcommands:
	@echo >/dev/null
	
# Used to force goals to build.  Only use for conditionally defined goals.
.PHONY: FORCE
FORCE:;
