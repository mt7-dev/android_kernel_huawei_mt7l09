
#***********************************************************
# include the define at the top
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#*************************************************************************
# Compiler definition (ToDo: 后续需要将编译器路径加到进程环境变量中 main.env)
#*************************************************************************
M3_CROSS_COMPILE    ?= $(OBB_ANDROID_DIR)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
CC                  :=$(DISTCC) $(SOURCEANALYZER) $(M3_CROSS_COMPILE)gcc
AS                  :=$(SOURCEANALYZER) $(M3_CROSS_COMPILE)as
AR                  :=$(SOURCEANALYZER) $(M3_CROSS_COMPILE)ar
LD                  :=$(SOURCEANALYZER) $(M3_CROSS_COMPILE)ld
OBJDUMP             :=$(M3_CROSS_COMPILE)objdump
OBJCOPY             :=$(M3_CROSS_COMPILE)objcopy
SIZE                :=$(M3_CROSS_COMPILE)size
NM                  :=$(M3_CROSS_COMPILE)nm

PACK_HEAD_TOOL      :=$(BALONG_TOPDIR)/build/tools/utility/postlink/packHead/packHead.py

#*************************************************************************
# sytem and os-Specific LIB & INC Path
#*************************************************************************
CLIB_INC_PATH       :=
CMSIS_INC_PATH      :=$(CMSIS_RTOS_RTX_DIR)/INC $(CMSIS_RTOS_RTX_DIR)/SRC $(CMSIS_ORG_DIR)/CMSIS/Include
SYS_INC_PATH        :=

#*************************************************************************
# Compilter-Specific flags & Configuration
#*************************************************************************
USER_CC_DEFINES     ?=
USER_AS_DEFINES     ?=
USER_CC_FLAGS       ?=
USER_AS_FLAGS       ?=
USER_C_OPTIM_FLAGS  ?=
USER_AS_OPTIM_FLAGS ?=

ifeq ($(strip $(OBB_BUILD_TYPE)),RELEASE)
USER_C_OPTIM_FLAGS  ?=
USER_AS_OPTIM_FLAGS ?=
else                
USER_C_OPTIM_FLAGS  ?= -g
USER_AS_OPTIM_FLAGS ?=
endif 

# ToDo: LD 选项是否需要用户自定义?
LOCAL_CC_OPTION     :=$(USER_CC_DEFINES) $(USER_CC_FLAGS) $(USER_C_OPTIM_FLAGS)
LOCAL_AS_OPTION     :=$(USER_AS_DEFINES) $(USER_AS_FLAGS) $(USER_AS_OPTIM_FLAGS)
LOCAL_LD_OPTION     :=$(USER_LD_FLAGS)
LOCAL_CC            :=$(CC) $(LOCAL_CC_OPTION)
LOCAL_AS            :=$(CC) $(LOCAL_AS_OPTION)
LOCAL_LD            :=$(LD) $(LOCAL_LD_OPTION)

#***********************************************************
# Direcotry configuration to output compiling result
#***********************************************************
LOCAL_OBJ_PATH      := $(OBB_PRODUCT_DELIVERY_DIR)/obj/$(OBC_LOCAL_MOUDLE_NAME)
LOCAL_LIBRARY       := $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).elf
LOCAL_IMAGE_FILE    := $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)

LOCAL_SRC_DIR       := $(OBC_LOCAL_SRC_DIR)
LOCAL_INC_DIR       := $(OBB_PRODUCT_CONFIG_DIR)  $(OBC_LOCAL_INC_DIR) $(CFG_CHIPSET_PATH) $(SYS_INC_PATH) $(CLIB_INC_PATH)  $(CMSIS_INC_PATH)
FULL_SRC_INC_PATH   := $(foreach lib_inc_path, $(LOCAL_INC_DIR), -I$(lib_inc_path))

# CC configuration 
C_DIR_FILES         := $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.c $(dir)/*.C))
FULL_C_SRCS         := $(filter %.c,$(OBC_LOCAL_SRC_FILE)) $(subst .C,.c, $(C_DIR_FILES)) 
FULL_C_OBJS         := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(FULL_C_SRCS)))
FULL_C_DEPENDS      := $(patsubst %.o,%.d,$(FULL_C_OBJS))

# AS configuration 
AS_DIR_FILES        := $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.s $(dir)/*.S))
FULL_AS_SRCS        := $(filter %.S,$(OBC_LOCAL_SRC_FILE)) $(subst .S,.s, $(AS_DIR_FILES))
FULL_AS_OBJS        := $(patsubst $(BALONG_TOPDIR)/%.S,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(FULL_AS_SRCS)))
FULL_AS_DEPENDS     := $(patsubst %.o,%.d,$(FULL_AS_OBJS))

OBJ_C_DIRS          := $(sort $(dir $(FULL_C_OBJS) $(RTX_C_OBJS)))
OBJ_C_DIRS          := $(patsubst %/,%,$(OBJ_C_DIRS))
OBJ_AS_DIRS         := $(sort $(dir $(FULL_AS_OBJS) $(RTX_AS_OBJS)))
OBJ_AS_DIRS         := $(patsubst %/,%,$(OBJ_AS_DIRS))

CODE_SIZE_FILE      := $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME).code.size
OBJ_LIST_FILE       := $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME).obj.list

#***********************************************************
# rules
#***********************************************************
.PHONY: all do_pre_build_before do_pre_build do_build_before do_build do_lib_before do_lib do_post_build_before do_post_build do_final_check_before do_final_check do_all_before
		
all: do_pre_build_before do_pre_build do_build_before do_build do_lib_before do_lib do_post_build_before do_post_build do_final_check_before do_final_check do_all_before
	@echo - do [$@]

### Rules order
do_all_before : do_final_check 
do_final_check : do_final_check_before
do_final_check_before : do_post_build
do_post_build : do_post_build_before
do_post_build_before : do_lib
do_lib : do_lib_before
do_lib_before : do_build
do_build : do_build_before
do_build_before : do_pre_build
do_pre_build : do_pre_build_before

# Pre-Build 
do_pre_build:
	@echo -  do [$@]
	$(Q)mkdir -p $(LOCAL_OBJ_PATH)
ifneq ($(strip $(OBJ_C_DIRS)),)
	$(Q)mkdir -p $(OBJ_C_DIRS)
endif
ifneq ($(strip $(OBJ_AS_DIRS)),)
	$(Q)mkdir -p $(OBJ_AS_DIRS)
endif
# build CC & AS ...
do_build : $(FULL_C_OBJS)  $(FULL_AS_OBJS)
	@echo do [$@]

ifneq ($(strip $(FULL_C_OBJS)),)
ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_C_DEPENDS)
endif

$(FULL_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c | do_build_before
	@echo [CC] $@
	$(Q)$(LOCAL_CC) $(FULL_SRC_INC_PATH) -c $< -o $@ 

ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
endif 

ifneq ($(strip $(FULL_AS_OBJS)),)
ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_AS_DEPENDS)
endif

$(FULL_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.S | do_build_before
	@echo [AS] $@
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@ 
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
endif

# Lib AR or LD...
do_lib : $(LOCAL_LIBRARY)
	@echo do [$@]

$(LOCAL_LIBRARY): $(FULL_C_OBJS)  $(FULL_AS_OBJS) | do_lib_before
	$(Q)mkdir -p $(LOCAL_OBJ_PATH)
	$(Q)echo $(FULL_AS_OBJS) $(FULL_C_OBJS)  > $(OBJ_LIST_FILE)
ifneq ($(strip $(USER_LIB)),)
	@echo $(USER_LIB) >>$(OBJ_LIST_FILE)
endif
ifneq ($(strip $(OBC_LOCAL_USER_OBJ_DYN_FILE)),)
	@echo $(OBC_LOCAL_USER_OBJ_DYN_FILE) >>$(OBJ_LIST_FILE)
endif

	@echo [LD] $(LOCAL_LIBRARY)
	$(Q)$(LOCAL_LD) $(FULL_AS_OBJS) $(FULL_C_OBJS) $(USER_LIB) -Map=$(basename $@).map -o $@

# Post-Build 
do_post_build : $(LOCAL_IMAGE_FILE).bin 
	@echo - do [$@]

$(LOCAL_IMAGE_FILE).bin : $(LOCAL_LIBRARY) | do_post_build_before
	@echo [BIN] $@
	$(Q)$(OBJCOPY) -O binary -S $(LOCAL_LIBRARY) $(LOCAL_IMAGE_FILE).bin
	$(Q)$(OBJDUMP) -D $(LOCAL_LIBRARY) > $(LOCAL_IMAGE_FILE).asm
	-$(Q)$(SIZE) -t $(LOCAL_LIBRARY) > $(CODE_SIZE_FILE)

#***********************************************************
# Clean up 
#***********************************************************
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -rf $(LOCAL_OBJ_PATH)
	$(Q)-rm -f $(OBJ_LIST_FILE) $(CODE_SIZE_FILE)  $(basename $(LOCAL_LIBRARY)).* $(LOCAL_DEBUG_LIBRARY)
	$(Q)-rm -f $(LOCAL_IMAGE_FILE).*
	$(Q)-rm -f  $(LOCAL_SEPERATE_OBJ_FILES_TARGET) $(OBC_LOCAL_CLEAN_FILES)  
ifneq ($(OBC_LOCAL_CLEAN_DIRS),)
	$(Q)-rm -rf $(OBC_LOCAL_CLEAN_DIRS)
endif