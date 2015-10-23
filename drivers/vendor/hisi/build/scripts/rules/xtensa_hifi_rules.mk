include $(BALONG_TOPDIR)/build/scripts/make_base.mk
ifeq ($(FORTIFY),true)
ifeq ($(OBB_OS_TYPE),Linux)
SOURCEANALYZER := $(BALONG_TOPDIR)/llt/ci/tools/fortify_linux/bin/sourceanalyzer -b $(OBB_PRODUCT_NAME)
else
SOURCEANALYZER := $(BALONG_TOPDIR)/llt/ci/tools/fortify_windows/bin/sourceanalyzer.exe -b $(OBB_PRODUCT_NAME)
endif
endif
#****************************************************************************
# input parameters
#****************************************************************************
OBC_LOCAL_CORE_NAME          =$(XTENSA_CORE_HIFI)
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

#****************************************************************************
# compiler configs
#****************************************************************************

CC      := $(SOURCEANALYZER) $(CCACHE_BIN) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-xcc
AS      := $(SOURCEANALYZER) $(CCACHE_BIN) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-xcc
AR      :=$(SOURCEANALYZER) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-ar
LD      :=$(SOURCEANALYZER) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-ld
SIZE    :=$(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-size
XT_GENLD :=$(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-genldscripts

CC_FLAGS            := -c -g -O2 -mlongcalls -mtext-section-literals --xtensa-core=$(OBC_LOCAL_CORE_NAME) --xtensa-system=$(subst \,/,$(XTENSA_SYSTEM_HIFI))
CC_FLAGS            += $(OBC_USER_CC_FLAGS) $(OBC_USER_CC_DEFINES)

AS_FLAGS            := -c -g -O2 -mlongcalls --xtensa-core=$(OBC_LOCAL_CORE_NAME) --xtensa-system=$(subst \,/,$(XTENSA_SYSTEM_HIFI))
AS_FLAGS            += $(OBC_USER_CC_FLAGS) $(OBC_USER_CC_DEFINES)
         
AR_FLAGS            :=rv

#****************************************************************************
# Direcotry configuration
#****************************************************************************
LOCAL_OBJ_PATH		:= $(OBB_PRODUCT_DELIVERY_DIR)/obj/guhifi
LOCAL_LIBRARY		:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).a

# INC
OBC_LOCAL_INC_DIR   += $(OBB_PRODUCT_CONFIG_DIR) $(OBB_PRODUCT_GU_CONFIG_DIR) $(OBB_CHIPSET_DIR)
FULL_SRC_INC_PATH   := ${foreach lib_inc_path, ${OBC_LOCAL_INC_DIR}, -I${lib_inc_path}}

LOCAL_SRC_DIR := $(OBC_LOCAL_SRC_DIR)

# CC configuration 
C_DIR_FILES         := $(foreach dir, $(LOCAL_SRC_DIR), $(wildcard $(dir)/*.c $(dir)/*.C))
FULL_C_SRCS         := $(filter %.c,$(OBC_LOCAL_SRC_FILE)) $(subst .C,.c, $(C_DIR_FILES))
FULL_C_OBJS         := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(FULL_C_SRCS))
FULL_C_DEPENDS      := $(subst .o,.d,$(FULL_C_OBJS))

# AS configuration 
AS_DIR_FILES        := $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.s $(dir)/*.S))
FULL_AS_SRCS        := $(filter %.s,$(OBC_LOCAL_SRC_FILE)) $(filter %.S,$(OBC_LOCAL_SRC_FILE)) $(subst .s,.S, $(AS_DIR_FILES))
FULL_AS_OBJS        := $(patsubst $(BALONG_TOPDIR)/%.s,$(LOCAL_OBJ_PATH)/%.o, $(FULL_AS_SRCS)) $(patsubst $(BALONG_TOPDIR)/%.S,$(LOCAL_OBJ_PATH)/%.o, $(FULL_AS_SRCS))

OBJ_C_DIRS          := $(sort $(dir $(FULL_C_OBJS)))
OBJ_C_DIRS          := $(patsubst %/,%,$(OBJ_C_DIRS))
OBJ_AS_DIRS         := $(sort $(dir $(FULL_AS_OBJS)))
OBJ_AS_DIRS         := $(patsubst %/,%,$(OBJ_AS_DIRS))

OBJ_LIST_FILE       := $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME).obj.list

#****************************************************************************
# compile
#****************************************************************************
.PHONY: all do_pre_build_before do_pre_build do_build_before do_build do_lib_before do_lib do_post_build_before do_post_build 

all: do_pre_build_before do_pre_build do_build_before do_build do_lib_before do_lib do_post_build_before do_post_build 
	@echo - do [$@]

# Rules order
do_post_build : do_post_build_before
do_post_build_before : do_lib
do_lib : do_lib_before
do_lib_before : do_build
do_build : do_build_before
do_build_before : do_pre_build
do_pre_build : do_pre_build_before

# Pre build
do_pre_build:
	@echo -  do [$@]
	@mkdir -p $(LOCAL_OBJ_PATH)
ifneq ($(strip $(OBJ_C_DIRS)),)
	$(Q)mkdir -p $(OBJ_C_DIRS)
endif
ifneq ($(strip $(OBJ_AS_DIRS)),)
	$(Q)mkdir -p $(OBJ_AS_DIRS)
endif

# build CC & AS ...
do_build : $(FULL_C_OBJS) $(FULL_AS_OBJS)
	@echo do [$@]
	$(Q)echo $(FULL_AS_OBJS) $(FULL_C_OBJS)  > $(OBJ_LIST_FILE)

ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_C_DEPENDS)
endif

$(FULL_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c  | do_build_before
	@echo [CC] $@
	$(Q)$(CC) $(CC_FLAGS)  $(FULL_SRC_INC_PATH) -MD -MF $(LOCAL_OBJ_PATH)/$*.d -c $< -o $@
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif

ifneq ($(strip $(FULL_AS_OBJS)),)
$(FULL_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.S | do_build_before
	@echo [AS] $@
	$(Q)$(AS) $(AS_FLAGS) $(FULL_SRC_INC_PATH) -c $< -o $@
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif
endif

# Lib AR or LD...
do_lib : $(LOCAL_LIBRARY)
	@echo do [$@]
	
$(LOCAL_LIBRARY): $(FULL_C_OBJS)  $(FULL_AS_OBJS) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^

# Clean up 
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -rf $(LOCAL_OBJ_PATH)
	$(Q)-rm -f $(OBJ_LIST_FILE) $(LOCAL_LIBRARY)