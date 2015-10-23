include $(BALONG_TOPDIR)/build/scripts/make_base.mk
#*********************************************** 
# obc define
#*********************************************** 
OBC_LOCAL_MOUDLE_NAME	:=guhifi_mcore

# AR config

LIB_codec_src_ajb :=$(BALONG_TOPDIR)/modem/med/build/common/hme_lib/libHME_AJB.a

# 链接顺序不能修改

CUSTOM_lIB_FILE := guhifi_custom.a
CUSTOM_lIB_FILE := $(addprefix $(OBB_PRODUCT_DELIVERY_DIR)/lib/,$(CUSTOM_lIB_FILE))
CUSTOM_lIB_DIR  := $(BALONG_TOPDIR)/modem/med/custom/hi6930/lib
CUSTOM_lIB_FILE += $(foreach dir, $(CUSTOM_lIB_DIR), $(wildcard $(dir)/*.a))

VOICE_LIB_FILE  := guhifi_voice.a
VOICE_LIB_FILE  := $(addprefix $(OBB_PRODUCT_DELIVERY_DIR)/lib/,$(VOICE_LIB_FILE))

COMMON_LIB_FILE := guhifi_common.a
COMMON_LIB_FILE := $(addprefix $(OBB_PRODUCT_DELIVERY_DIR)/lib/,$(COMMON_LIB_FILE))


OBC_LOCAL_LIB_FILE      := $(CUSTOM_lIB_FILE) $(COMMON_LIB_FILE) $(VOICE_LIB_FILE) $(LIB_codec_src_ajb)

OBC_LD_SCRIPT_DIR	    :=$(BALONG_TOPDIR)/modem/med/build/hi6930/$(CFG_HIFI_LINK_DIR_NAME)

#include rules. must be droped at the bottom
include $(BALONG_TOPDIR)/build/scripts/rules/ld_xtensa_hifi_rules.mk