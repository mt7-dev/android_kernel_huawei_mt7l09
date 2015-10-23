# ATTENTION: OBB_ is the prefix of make_base.mk

# this turns off the suffix rules built into make
.SUFFIXES:

# If a rule fails, delete $@.
.DELETE_ON_ERROR:

# trim \ of the BALONG_TOPDIR
BALONG_TOPDIR := $(subst \,/,$(BALONG_TOPDIR))

# parameters from commandline
OBB_PRODUCT_NAME    ?=$(product)
OBB_CORE_NAME       ?=$(core)
OBB_VERBOSE         ?=$(if $(verbose),$(verbose),true)
OBB_BUILD_TYPE      ?=$(build_type)
OBB_SEPARATE        ?=$(separate)
OBB_CM3             ?=$(if $(cm3),$(cm3),true)
OBB_JOBS_N          ?=$(if $(jobs),$(jobs),40)
OBB_JOBS            ?= -j $(OBB_JOBS_N)
OBB_BRANCH_NAME     ?= $(branch)
OBB_ACORE_MODULE    ?=$(acore-module)
OBB_BUILD_ACTION    ?=$(build_action)
OBB_LPHY_RTT        ?=$(lphy_rtt)
OBB_TRANS_MATCH     ?=$(if $(trans_match),$(trans_match),MatchFile)
RELEASE_PACKAGE_NAME ?=$(if $(pack),$(pack),$(product))
HW_PRODUCT          ?=hi3630

# core name
OBB_CORE_NAME       :=$(if $(strip $(OBB_CORE_NAME)),$(OBB_CORE_NAME),all)

# verbose flags
ifeq ($(OBB_VERBOSE),true)
Q :=
V := -v 
else
Q :=@
V :=
endif

# Make sure OBB_PRODUCT_NAME is correct.
ifeq ($(OBB_PRODUCT_NAME),)
$(warning ************************************************************)
$(warning The OBB_PRODUCT_NAME is blank. $(OBB_PRODUCT_NAME))
$(warning ************************************************************)
$(error "Please use product=<product name>")
endif

# product path define
OBB_PRODUCT_DEFINE_FILE := $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/define
OBB_PRODUCT_CONFIG_DIR := $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config
OBB_PRODUCT_DELIVERY_DIR := $(BALONG_TOPDIR)/build/delivery/$(OBB_PRODUCT_NAME)
OBB_SCRIPTS_DIR := $(BALONG_TOPDIR)/build/scripts
OBB_ANDROID_DIR :=$(BALONG_TOPDIR)/modem/system/android/android_4.2_r1

#include $(OBB_SCRIPTS_DIR)/main.env

# include product define macros file
include $(OBB_PRODUCT_CONFIG_DIR)/balong_product_config.mk

# drv test code control
ifeq ($(CFG_ENABLE_TEST_CODE),YES)
CONFIG_ENABLE_TEST_CODE := y
endif
ifeq ($(HUTAF_HLT_COV),true)
CONFIG_ENABLE_TEST_CODE := y
endif

# gu feature_config dir
OBB_CHIPSET_DIR := $(BALONG_TOPDIR)
OBB_PRODUCT_GU_CONFIG_DIR := $(BALONG_TOPDIR)/$(CFG_GU_FEATURE_CONFIG_PATH)

# build type parameter
ifeq ($(strip $(OBB_BUILD_TYPE)),)
OBB_BUILD_TYPE :=$(CFG_BUILD_TYPE)
endif

# build acore component
ifeq ($(strip $(OBB_ACORE_MODULE)),)
OBB_ACORE_MODULE :=balong
endif

#OBB_BUILD_ACTION values: cc lint coverity fortify ...
ifeq ($(strip $(OBB_BUILD_ACTION)),)
OBB_BUILD_ACTION :=cc
endif

#ssh server config
OBB_SERVER_ADDR			?=$(build_server)
OBB_USERNAME			?=$(build_user)

#sync config
OBB_SYNC_SOURCE_DIR      := $(BALONG_TOPDIR)
OBB_SYNC_DESTINATION_DIR := /cygdrive/e/$(OBB_USERNAME)/$(OBB_PRODUCT_NAME)

# config ccache
ifneq ($(USE_CCACHE),)
CCACHE :=$(BALONG_TOPDIR)/build/tools/ccache/ccache
endif

#linux distcc
#ifneq ($(distcc-acore),true)
#export DISTCC_KERNEL:=
#export DISTCC_ANDROID:=
#endif

#acore build-in config
OBB_CFG_BUILDIN :=m
ifeq ($(strip $(CFG_BOOT_OPTI_BUILDIN)),YES)
OBB_CFG_BUILDIN :=y
endif

# export base vars for passing parameters to other makefiles
export BALONG_TOPDIR OBB_PRODUCT_NAME OBB_CORE_NAME OBB_VERBOSE OBB_DISTCC OBB_BUILD_TYPE OBB_ANDROID_DIR CONFIG_ENABLE_TEST_CODE
