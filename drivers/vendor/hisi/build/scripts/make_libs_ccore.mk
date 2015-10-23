
include $(BALONG_TOPDIR)/build/scripts/make_base.mk
#*********************************************** 
# obc define
#*********************************************** 
OBC_LOCAL_CORE_NAME		:=ccore
OBC_LOCAL_MOUDLE_NAME	:=balong_modem

ifeq ($(strip $(OBB_SEPARATE)),true)
CFG_CCORE_COMPONENTS := bsp_ccore.o os_ccore.o
endif

OBC_LOCAL_LIB_DIR		:=
ifeq ($(CFG_FEATURE_MULTI_MODEM) ,FEATURE_ON)
CFG_CCORE_COMPONENTS +=$(CFG_CCORE_MODEMUNION_COMPONENTS)
endif

ifeq ($(CFG_FEATURE_IMS) ,FEATURE_ON)
ifneq ($(OBB_SEPARATE), true)
CFG_CCORE_COMPONENTS +=hw_sdk.o
endif
endif


OBC_LOCAL_LIB_FILE		:=$(addprefix $(OBB_PRODUCT_DELIVERY_DIR)/lib/,$(CFG_CCORE_COMPONENTS))

OBC_LD_SCRIPT_FILE		:=$(BALONG_TOPDIR)/build/scripts/ld_vxworks6.8_link.ld

OBC_LD_RAM_LOW_ADDR		:=$(CFG_MCORE_TEXT_START_ADDR)
OBC_LD_VX_START_ADDR	:=$(OBC_LD_RAM_LOW_ADDR)

OBC_LD_DEFINES			:=-D_VX_DATA_ALIGN=0x1 
OBC_LOCAL_CLEAN_DIRS    :=
OBC_LOCAL_CLEAN_FILES	:= 

#include rules. must be droped at the bottom
include $(BALONG_TOPDIR)/build/scripts/ld_vxworks6.8_rules.mk
