include $(BALONG_TOPDIR)/build/scripts/make_base.mk

# config
#*******************************************************************************
SYNC_SERVER_HOST		:= $(OBB_USERNAME)@$(OBB_SERVER_ADDR)
MY_SSH					:= ssh $(SYNC_SERVER_HOST)
MY_SYNC					:= rsync -e ssh -rt -v
local_dir_to_sync		=$(addprefix /cygdrive/,$(subst :,,$(1)))

SERVER_SOURCE_ROOT		:= $(OBB_SERVER_SOURCE_ROOT)
OBC_SHELL_FUNC_NAME			:= libs 
LOCAL_ROOT				?=$(OBB_PRODUCT_DELIVERY_DIR)/lib
SERVER_ROOT				?=$(OBB_SERVER_SOURCE_ROOT)/ANDROID/local_bin/$(OBB_PRODUCT_NAME)/bin
LOCAL_SYNC_DIR			:=$(call local_dir_to_sync,$(LOCAL_ROOT)/)

src_files				:=LPS.o LMSP.o LMSPNV.o GUPS.o GUMSP.o system.zip mkyaffs2image
include_src_files		:=$(addprefix --include=,$(src_files))
full_src_files			:=$(addprefix $(LOCAL_ROOT)/,$(src_files))
dst_files				:=system.img

# Shell cmd args
# $1=function $2=product_name $3=verbose $4=DIST_CC_ENABLED $5=CHIP_BB_6920ES $6=BOARD_ASIC  $7=ALLY/SEPARATE
SHELL_CMD_ARGS	:= $(OBC_SHELL_FUNC_NAME) \
				   $(OBB_PRODUCT_NAME) \
				   $(if $(OBB_VERBOSE),VERBOSE_ENABLED,VERBOSE_DISABLED) \
				   $(if $(OBB_DISTCC),DIST_CC_ENABLED,DIST_CC_DISABLED) \
				   CHIP_BB_$(CFG_CHIP_BB) \
				   BOARD_$(CFG_BOARD) \
				   $(if $(OBB_SEPARATE),SEPARATE,ALLY) \
				   $(CFG_GU_FEATURE_CONFIG_PATH) \
				   $(CFG_OS_LINUX_PRODUCT_NAME) \
				   $(CFG_OS_ANDROID_PRODUCT_NAME) \
				   $(CFG_FEATURE_WIFI_ON) 

# rules
#*******************************************************************************
.PHONY:all make_system_img
all : make_system_img
	@echo do [$@]

make_system_img:$(LOCAL_ROOT)/system.img

$(LOCAL_ROOT)/$(dst_files) : $(full_src_files)
	@echo [$(OBC_SHELL_FUNC_NAME)] $@
	$(Q)$(MY_SSH) "mkdir -p $(SERVER_ROOT)" 
	$(Q)$(MY_SYNC) -m --chmod=ugo=rwX  $(include_src_files) --filter="- *" $(LOCAL_SYNC_DIR)  $(SYNC_SERVER_HOST):$(SERVER_ROOT)/
	$(Q)$(MY_SSH) " $(SERVER_SOURCE_ROOT)/TARGET/scripts/build_balong_acore_entry.sh $(SERVER_SOURCE_ROOT) $(SHELL_CMD_ARGS) "
	$(Q)$(MY_SYNC) -p  $(SYNC_SERVER_HOST):$(SERVER_ROOT)/$(dst_files)  $(LOCAL_SYNC_DIR)/

.PHONY: clean
clean:
	$(Q)-rm -f $(V) $(LOCAL_SYNC_DIR)/$(dst_files) 