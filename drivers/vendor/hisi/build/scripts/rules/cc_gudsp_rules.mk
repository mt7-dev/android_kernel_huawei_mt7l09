################################################################################
# Copyright (C) 2012 Huawei Tech. Co., Ltd.
# Makefile for Appcore
# History : 
################################################################################
# include comm define
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

# input parameters, Generic directory names from top makefile (input)
################################################################################
OBC_LOCAL_CORE_NAME          ?=
OBC_LOCAL_MOUDLE_NAME        ?=

OBC_LOCAL_SRC_FILE           ?= 
OBC_LOCAL_SRC_DIR            ?=
OBC_LOCAL_INC_DIR            ?=

OBC_USER_CC_FLAGS            ?=
OBC_USER_AS_FLAGS            ?=
OBC_USER_AR_FLAGS            ?=
OBC_USER_LD_FLAGS            ?=

OBC_LOCAL_BUILD_TYPE         ?=$(OBB_BUILD_TYPE)
OBC_LINK_SCRIPT              ?=


#*************************************************************************
# Compiler-Specific options (arm)
#*************************************************************************
VX_CPU_FAMILY   ?= arm
CPU             ?= ARMARCH7
TOOL_FAMILY     ?= gnu
TOOL            ?= gnu


#*************************************************************************
# OS-Specific LIB Path
#*************************************************************************
OS_LIB_PATH             :=$(BALONG_TOPDIR)/build/libs/os/$(OBC_LOCAL_CORE_NAME)/BALONGV7R1.LIB.$(OBB_BUILD_TYPE)
OS_CPU_LIB_PATH         :=$(OS_LIB_PATH)/$(VX_CPU_FAMILY)/$(CPU)
OS_LIB_TAG_PATH         :=$(OS_LIB_PATH)/tags/$(VX_CPU_FAMILY)/$(CPU)

#*************************************************************************
# Compiler-Specific options
#*************************************************************************
VSB_DIR         ?= $(OS_LIB_PATH)
VSB_CONFIG_FILE := $(VSB_DIR)/h/config/vsbConfig.h

#*************************************************************************
# Compilter-Specific flags
#*************************************************************************
# CC & AS
CC = $(DISTCC) $(SOURCEANALYZER) $(CCACHE) ccarm
AS = $(SOURCEANALYZER) $(CCACHE) ccarm
LIBAR           ?= ararm

COMPILER_CFLAGS := -c -w -t7 -msoft-float -fdollars-in-identifiers \
			   -Os -fno-builtin -fvolatile -fsigned-char -falign-arrays -fno-feature-proxy -Wall -fno-zero-initialized-in-bss

#make dependency;chenchaofei
# ifeq ($(COMPILER_CFG_MAKE_DEP),YES)
# COMPILER_CFLAGS     += -Xmake-dependency=0x04
# endif


CFLAGS := -g -c -O2 $(USER_CCFLAGS) $(PRODUCT_CFG_CFLAGS) $(SUBSYS_CFG_CFLAGS) $(COMPILER_CFLAGS)

# LD & AR
LD       :=$(SOURCEANALYZER) ldarm
LD_FLAGS  := -r -o 

AR     :=$(SOURCEANALYZER) ldarm 
AR_FLAGS := -r -o 

SIZE =sizearm
OBJ_DUMP = objdumparm.exe -D 


#*************************************************************************
# OS System Include files
#*************************************************************************
# SYS_OS_INC_DIR := $(WIND_HOME)/vxworks-6.8/target/h \
            # $(WIND_HOME)/vxworks-6.8/target/h/types \
            # $(WIND_HOME)/vxworks-6.8/target/h/wrn/coreip \
			# $(WIND_HOME)/vxworks-6.8/target/config/all \
            # $(WIND_HOME)/components/ip_net2-6.8/vxcompat/include

# OS_LIB_TAG_PATH         :=$(OS_LIB_PATH)/tags/$(VX_CPU_FAMILY)/$(CPU)

#*************************************************************************
# Compiler-Specific Configuration
#*************************************************************************
LOCAL_CC := $(CC) $(CFLAGS)

#  π”√C±‡“Î∆˜±‡“Îª„±‡¥˙¬Î 
LOCAL_AS = $(CC) $(CFLAGS)

#****************************************************************************
# Direcotry configuration
#****************************************************************************
LOCAL_OBJ_PATH		:= $(OBB_PRODUCT_DELIVERY_DIR)/obj/$(OBC_LOCAL_MOUDLE_NAME)
LOCAL_LIBRARY		:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).o
LOCAL_LIBRARY_RT	:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)_RT.o
LOCAL_IMAGE_FILE	:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)

# INC
OBC_LOCAL_INC_DIR	+= $(OBB_PRODUCT_CONFIG_DIR) $(OBB_CHIPSET_DIR) $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
						$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu
FULL_SRC_INC_PATH	:= ${foreach lib_inc_path, ${OBC_LOCAL_INC_DIR}, -I${lib_inc_path}}

# CC configuration 	
C_DIR_FILES			:= $(foreach dir ,$(OBC_LOCAL_SRC_DIR) ,$(wildcard $(dir)/*.c $(dir)/*.C))
FULL_C_SRCS			:= $(filter %.c,$(OBC_LOCAL_SRC_FILE)) $(subst .C,.c, $(C_DIR_FILES)) 
FULL_C_OBJS			:= $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(FULL_C_SRCS))
FULL_C_DEPENDS		:= $(subst .o,.d,$(FULL_C_OBJS))

# AS configuration 
AS_DIR_FILES		:= $(foreach dir,$(OBC_LOCAL_SRC_DIR),$(wildcard $(dir)/*.s $(dir)/*.S))
FULL_AS_SRCS		:= $(filter %.s,$(OBC_LOCAL_SRC_FILE)) $(subst .S,.s, $(AS_DIR_FILES))
FULL_AS_OBJS		:= $(patsubst $(BALONG_TOPDIR)/%.s,$(LOCAL_OBJ_PATH)/%.o, $(filter $(BALONG_TOPDIR)/%.s,$(FULL_AS_SRCS)))

# AS for crt0.s, god!
FULL_AS_OBJS_SYS	:= $(patsubst %.s,%.o,$(filter-out $(BALONG_TOPDIR)/%.s,$(FULL_AS_SRCS)))

OBJ_C_DIRS          := $(sort $(dir $(FULL_C_OBJS)))
OBJ_C_DIRS          := $(patsubst %/,%,$(OBJ_C_DIRS))
OBJ_AS_DIRS         := $(sort $(dir $(FULL_AS_OBJS)))
OBJ_AS_DIRS         := $(patsubst %/,%,$(OBJ_AS_DIRS))
OBJ_AS_SYS_DIRS     := $(sort $(dir $(FULL_AS_OBJS_SYS)))
OBJ_AS_SYS_DIRS     := $(patsubst %/,%,$(OBJ_AS_SYS_DIRS))

CODE_SIZE_FILE			:= $(LOCAL_LIBRARY).code.size
OBJ_LIST_FILE			:= $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME).obj.list

# AR config
filter_objs = $(patsubst $(BALONG_TOPDIR)/%.o,$(LOCAL_OBJ_PATH)/%.o, $(patsubst %.C,%.o,$(patsubst %.c,%.o,$(patsubst %.S,%.o,$(patsubst %.s,%.o,$(1))))))

LIB_comm :=$(LOCAL_OBJ_PATH)/comm.a
OBJS_comm :=$(call filter_objs,$(src_comm))
LIB_GG :=$(LOCAL_OBJ_PATH)/gg.a
OBJS_GG :=$(call filter_objs,$(src_gg))
LIB_WCDMA :=$(LOCAL_OBJ_PATH)/wcdma.a 
OBJS_WCDMA :=$(call filter_objs,$(src_wcdma))
LIB_WCDMA_UPA :=$(LOCAL_OBJ_PATH)/wcdma_upa.a 
OBJS_WCDMA_UPA :=$(call filter_objs,$(src_wcdma_upa))
LIB_GG_POWERDOWN :=$(LOCAL_OBJ_PATH)/gg_powerdown.a 
OBJS_GG_POWERDOWN :=$(call filter_objs,$(src_gg_powerdown))
LIB_PHY_HAL :=$(LOCAL_OBJ_PATH)/src_phy_hal.a 
OBJS_PHY_HAL :=$(call filter_objs,$(src_phy_hal))
# LIB_ZOS :=$(LOCAL_OBJ_PATH)/zos.a
# OBJS_ZOS :=$(call filter_objs,$(src_zos))

# OS_LIBS := $(ZSP_HOME_U)/zspg2/lib/libm.a\
          # $(ZSP_HOME_U)/zspg2/lib/libc.a 
		  
AR_LIBS :=	
ifeq ($(INSTANCE_ID), INSTANCE_0)
AR_LIBS := $(LIB_GG) $(LIB_WCDMA) $(LIB_WCDMA_UPA) $(LIB_PHY_HAL)
endif

ifeq ($(INSTANCE_ID), INSTANCE_1)
AR_LIBS := $(LIB_GG) $(LIB_PHY_HAL)
endif

AR_LIBS_RT := $(LIB_comm)
# LD
TARGET_LINK_SCRIPT :=$(LOCAL_OBJ_PATH)/$(notdir $(OBC_LINK_SCRIPT))
################################################################################
# Build Targets	
################################################################################
.PHONY: all do_pre_build_before do_pre_build do_build_before do_build do_lib_before do_lib do_post_build_before do_post_build 
		
all: do_pre_build_before do_pre_build do_build_before do_build do_lib_before do_lib do_post_build_before do_post_build 
	@echo - do [$@]

### Rules order
do_post_build : do_post_build_before
do_post_build_before : do_lib
do_lib : do_lib_before
do_lib_before : do_build
do_build : do_build_before
do_build_before : do_pre_build
do_pre_build : do_pre_build_before
#do_pre_build_before :

#
# Pre build
#
do_pre_build:
	@echo -  do [$@]
	@mkdir -p $(LOCAL_OBJ_PATH)
ifneq ($(strip $(OBJ_C_DIRS)),)
	$(Q)mkdir -p $(OBJ_C_DIRS)
endif
ifneq ($(strip $(OBJ_AS_DIRS)),)
	$(Q)mkdir -p $(OBJ_AS_DIRS)
endif
ifneq ($(strip $(OBJ_AS_SYS_DIRS)),)
	$(Q)mkdir -p $(OBJ_AS_SYS_DIRS)
endif
#
# build CC & AS ...
#
do_build : $(FULL_C_OBJS) $(FULL_AS_OBJS) $(FULL_AS_OBJS_SYS)
	@echo do [$@]

ifneq ($(strip $(FULL_C_OBJS)),)
ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_C_DEPENDS)
endif

$(FULL_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c | do_build_before
	@echo [CC] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
#	$(Q)$(CC) $(CC_FLAGS) $(FULL_SRC_INC_PATH) -c $< -o $@
ifeq ($(DISABLE_VXWORKS_DEPEND),true)
	$(Q)$(LOCAL_CC) $(FULL_SRC_INC_PATH) -c $< -o $@ 
else
	$(Q)$(LOCAL_CC) -MD -MF $(basename $@).d $(FULL_SRC_INC_PATH) -c $< -o $@ 
	$(Q)sed -e 's,\([A-Za-z]:\)\([^ /\\]\),\1/\2,g' -e 's,\([^ ]\)\\\([^\r\n]\),\1/\2,g' -i $(basename $@).d
endif
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif
endif
ifneq ($(strip $(FULL_AS_OBJS)),)
$(FULL_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.s | do_build_before
	@echo [AS] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
#	$(Q)$(AS) $(AS_FLAGS) $(FULL_SRC_INC_PATH) -c $< -o $@
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif
endif
# AS for crt0.s, god!
ifneq ($(strip $(FULL_AS_OBJS_SYS)),)
$(FULL_AS_OBJS_SYS): %.o :%.s   | do_build_before
	@echo [AS] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
#	$(Q)$(AS) $(AS_FLAGS) $(FULL_SRC_INC_PATH) -c $< -o $@
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif
endif
#
# Lib AR or LD...
#
do_lib : $(LOCAL_LIBRARY)
	@echo do [$@]
	
$(LIB_comm):$(OBJS_comm) | do_lib_before
	@echo [AR] $@
	$(Q)$(AR) $(AR_FLAGS) $@ $^
$(LIB_GG):$(OBJS_GG) | do_lib_before
	@echo [AR] $@
	$(Q)$(AR) $(AR_FLAGS) $@ $^
$(LIB_GG_POWERDOWN):$(OBJS_GG_POWERDOWN) | do_lib_before
	@echo [AR] $@
	$(Q)$(AR) $(AR_FLAGS) $@ $^
$(LIB_PHY_HAL):$(OBJS_PHY_HAL) | do_lib_before
	@echo [AR] $@
	$(Q)$(AR) $(AR_FLAGS) $@ $^
ifeq ($(INSTANCE_ID), INSTANCE_0)
$(LIB_WCDMA):$(OBJS_WCDMA) | do_lib_before
	@echo [AR] $@
	$(Q)$(AR) $(AR_FLAGS) $@ $^
$(LIB_WCDMA_UPA):$(OBJS_WCDMA_UPA) | do_lib_before
	@echo [AR] $@
	$(Q)$(AR) $(AR_FLAGS) $@ $^
endif
# $(LIB_ZOS):$(OBJS_ZOS) | do_lib_before
	# @echo [AR] $@
	# $(Q)$(AR) $(AR_FLAGS) $@ $^
	
# zdmake do not recognize the | do_lib_before
$(LOCAL_LIBRARY): $(AR_LIBS) $(AR_LIBS_RT) $(FULL_C_OBJS) $(FULL_AS_OBJS) $(FULL_AS_OBJS_SYS) | do_lib_before
	$(Q)echo [LD] $@
	# $(Q)cp -f $(OBC_LINK_SCRIPT) $(TARGET_LINK_SCRIPT)
	# $(Q)chmod 777 $(TARGET_LINK_SCRIPT)
	# $(Q)sed "s,result/,$(LOCAL_OBJ_PATH)/,g" -i $(TARGET_LINK_SCRIPT)
	# $(Q)$(LD) $(LD_FLAGS) -T $(TARGET_LINK_SCRIPT) -Map $(LOCAL_IMAGE_FILE).map  -o $@ $(OBJS_comm)   --start-group $(AR_LIBS) $(OS_LIBS) --end-group
ifneq ($(INSTANCE_ID), INSTANCE_COMM)
	$(Q)$(LD) $(LD_FLAGS) $(LOCAL_LIBRARY) $(AR_LIBS)
	objcopyarm --rename-section .data=.phy_data  $(LOCAL_LIBRARY)  $(LOCAL_LIBRARY)
	objcopyarm --rename-section .bss=.phy_bssdata  $(LOCAL_LIBRARY)  $(LOCAL_LIBRARY)
endif
	$(Q)$(LD) $(LD_FLAGS) $(LOCAL_LIBRARY_RT) $(AR_LIBS_RT)
	objcopyarm --rename-section .data=.phy_data  $(LOCAL_LIBRARY_RT)  $(LOCAL_LIBRARY_RT)
	objcopyarm --rename-section .bss=.phy_bssdata  $(LOCAL_LIBRARY_RT)  $(LOCAL_LIBRARY_RT)
	
#
# image file BIN & NM
#
# do_post_build : $(LOCAL_IMAGE_FILE).bin 
	# @echo - do [$@]

# $(LOCAL_IMAGE_FILE).bin : $(LOCAL_LIBRARY) | do_post_build_before
	# @echo [BIN] $@
	# $(Q)cd $(dir $(LOCAL_IMAGE_FILE)) && \
	# $(OBB_SCRIPTS_DIR)/elfreader.exe $(notdir $(LOCAL_IMAGE_FILE).out) $(notdir $(LOCAL_IMAGE_FILE).bin)  bin && \
	# cd  $(CURDIR)
	# echo $(OBC_LOCAL_MOUDLE_NAME)  Code Size Info: >> "$(CODE_SIZE_FILE)"
	# -$(SIZE) -t $(LOCAL_LIBRARY)               >> "$(CODE_SIZE_FILE)"



################################################################################
# Clean up 
################################################################################
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -rf $(LOCAL_OBJ_PATH)
	$(Q)-rm -f $(OBJ_LIST_FILE) $(CODE_SIZE_FILE) $(LOCAL_LIBRARY)  $(LOCAL_LIBRARY_RT)
	$(Q)-rm -f $(OBC_LOCAL_CLEAN_FILES) $(LOCAL_IMAGE_FILE).bin $(LOCAL_IMAGE_FILE).map $(LOCAL_IMAGE_FILE).out \
				$(TARGET_LINK_SCRIPT) $(AR_LIBS) 
ifneq ($(OBC_LOCAL_CLEAN_DIRS),)
	$(Q)-rm -rf $(OBC_LOCAL_CLEAN_DIRS)
endif


