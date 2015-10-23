include $(BALONG_TOPDIR)/build/scripts/make_base.mk
#***********************************************************
# input parameters
#***********************************************************
OBC_LOCAL_CORE_NAME          ?=
OBC_LOCAL_MOUDLE_NAME        ?=

OBC_LOCAL_SRC_FILE           ?=
OBC_LOCAL_SRC_DIR            ?=
OBC_LOCAL_INC_DIR            ?=
OBC_LOCAL_CLEAN_FILES        ?=
OBC_LOCAL_CLEAN_DIRS         ?=
OBC_LOCAL_USER_OBJS 		 ?=
DTCM_FLAGS					 ?=
ITCM_FLAGS					 ?=

CC_USER_FLAGS     			?=
AS_USER_FLAGS     			?=


#***********************************************************			   
# compiler config
#***********************************************************
TENSILICA_TOOLS             =$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RD-2012.5-linux
export PATH:=$(TENSILICA_TOOLS)/XtensaTools/bin:$(TENSILICA_TOOLS)/XtDevTools/tools/perl/bin/MSWin32-x86:$(TENSILICA_TOOLS)/XtensaTools/lib/iss:$(TENSILICA_TOOLS)/XtensaTools/Tools/bin:$(PATH)
CC			:= $(SOURCEANALYZER) $(CCACHE) $(TENSILICA_TOOLS)/XtensaTools/bin/xt-xcc 
AS			:= $(SOURCEANALYZER) $(CCACHE) $(TENSILICA_TOOLS)/XtensaTools/bin/xt-xcc 
AR			:= $(SOURCEANALYZER) $(TENSILICA_TOOLS)/XtensaTools/bin/xt-ar 
LD			:= $(SOURCEANALYZER) $(TENSILICA_TOOLS)/XtensaTools/bin/xt-xc++ 
SIZE		:= $(TENSILICA_TOOLS)/XtensaTools/bin/xt-size
ASC_TO_BIN_TOOL := $(BALONG_TOPDIR)/build/tools/utility/tools/asc_to_bin.exe

CC_FLAGS	:= -c -OPT:space_flix=1 -Os -fno-zero-initialized-in-bss -fcommon -fmessage-length=0 -ffunction-sections -DPROC_$(CFG_XTENSA_CORE) -DCONFIG_$(CFG_XTENSA_CORE) \
				-DTENSILICA_PLATFORM -DLPS_RTT --xtensa-system=$(subst \,/,$(CFG_XTENSA_SYSTEM)) --xtensa-core=$(CFG_XTENSA_CORE) --xtensa-params= 
AS_FLAGS	:= -c -Wa,--gdwarf-2 -DPROC_$(CFG_XTENSA_CORE) -DCONFIG_$(CFG_XTENSA_CORE) \
				-DTENSILICA_PLATFORM -DLPS_RTT --xtensa-system=$(subst \,/,$(CFG_XTENSA_SYSTEM)) --xtensa-core=$(CFG_XTENSA_CORE) 
AR_FLAGS	:= -qcsu 
LD_MAP_PATH	:= $(BALONG_TOPDIR)/$(LPHY_LD_MAP_PATH)
LD_FLAGS	:=  -Wl,-gc-sections --xtensa-system=$(subst \,/,$(CFG_XTENSA_SYSTEM)) --xtensa-core=$(CFG_XTENSA_CORE) --xtensa-params= \
				-mlsp=$(LD_MAP_PATH) $(COREFLAG_$(CFG_XTENSA_CORE)) 

export XTENSA_SYSTEM=$(CFG_XTENSA_SYSTEM)
export XTENSA_CORE=$(CFG_XTENSA_CORE)
#ifeq ($(OBB_BUILD_TYPE),DEBUG)
CC_FLAGS     += -g 
#endif


#***********************************************************
# Direcotry configuration to output compiling result
#***********************************************************
LOCAL_OBJ_PATH		:= $(OBB_PRODUCT_DELIVERY_DIR)/obj/$(OBC_LOCAL_MOUDLE_NAME)
LOCAL_LIBRARY		:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).elf

LOCAL_MOUDLE_ITCM_NAME	 :=$(join $(OBC_LOCAL_MOUDLE_NAME),_itcm.bin)
LOCAL_MOUDLE_DTCM_NAME   :=$(join $(OBC_LOCAL_MOUDLE_NAME),_dtcm.bin)
LOCAL_MOUDLE_ITCM_NAME   :=$(OBB_PRODUCT_DELIVERY_DIR)/lib/$(LOCAL_MOUDLE_ITCM_NAME)
LOCAL_MOUDLE_DTCM_NAME   :=$(OBB_PRODUCT_DELIVERY_DIR)/lib/$(LOCAL_MOUDLE_DTCM_NAME)
LOCAL_IMAGE_FILE    := $(LOCAL_MOUDLE_ITCM_NAME) $(LOCAL_MOUDLE_DTCM_NAME)
LOCAL_MAP_FILE      := $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).map
LOCAL_DUMP_FILE      := $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).dump

LOCAL_SRC_DIR		:= $(OBC_LOCAL_SRC_DIR)
LOCAL_INC_DIR		:= $(OBB_PRODUCT_CONFIG_DIR)  $(OBC_LOCAL_INC_DIR) $(CFG_CHIPSET_PATH)
FULL_SRC_INC_PATH   := $(foreach lib_inc_path, $(LOCAL_INC_DIR), -I$(lib_inc_path))

# CC configuration 
C_DIR_FILES  		:= $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.c $(dir)/*.C))
FULL_C_SRCS      	:= $(filter %.c,$(OBC_LOCAL_SRC_FILE)) $(subst .C,.c, $(C_DIR_FILES)) 
FULL_C_OBJS       	:= $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(FULL_C_SRCS)))
FULL_C_DEPENDS      := $(subst .o,.d,$(FULL_C_OBJS))

# AS configuration 
AS_DIR_FILES  		:= $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.s $(dir)/*.S))
FULL_AS_SRCS      	:= $(filter %.S,$(OBC_LOCAL_SRC_FILE)) $(subst .s,.S, $(AS_DIR_FILES))
FULL_AS_OBJS       	:= $(patsubst $(BALONG_TOPDIR)/%.S,$(LOCAL_OBJ_PATH)/%.o, $(subst \,/,$(FULL_AS_SRCS)))

CODE_SIZE_FILE		:= $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME).code.size
OBJ_LIST_FILE		:= $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME).obj.list

vpath %.h . $(LOCAL_SRC_DIR) 
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

# Pre build
do_pre_build:
	@echo -  do [$@]
	@export
	@echo cc_tensilica4.0.5_rules
ifeq ($(strip $(LD_MAP_PATH)),)
	@$(error "Please configure LD_MAP_PATH before compiling any PHY object codes")
endif
	@echo Tensilica lsp path is $(LD_MAP_PATH)
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

ifneq ($(strip $(FULL_C_OBJS)),)
ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_C_DEPENDS)
endif

$(FULL_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c | do_build_before
	@echo [CC] $@
	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))	
	$(Q)$(CC) $(CC_USER_FLAGS)  $(CC_FLAGS) $(FULL_SRC_INC_PATH) -MD -MF $(LOCAL_OBJ_PATH)/$*.d  -c $< -o $@ 
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
endif 

ifneq ($(strip $(FULL_AS_OBJS)),)
$(FULL_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.S | do_build_before
	@echo [AS] $@
	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
	$(Q)$(AS) $(AS_USER_FLAGS)  $(AS_FLAGS) $(FULL_SRC_INC_PATH)  --xtensa-params= "$<"  -o $@ 
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
endif 

# Lib AR or LD...
do_lib : $(LOCAL_LIBRARY)
	@echo do [$@]


$(LOCAL_LIBRARY): $(FULL_C_OBJS) $(FULL_AS_OBJS) | do_lib_before
	$(Q)mkdir -p $(LOCAL_OBJ_PATH)
	$(Q)echo $(FULL_C_OBJS)  > $(OBJ_LIST_FILE)
	$(Q)echo $(FULL_AS_OBJS) >> $(OBJ_LIST_FILE)

#	@echo [AR] $(LOCAL_LIBRARY)
#	$(Q)$(AR) $(AR_FLAGS)  $(LOCAL_LIBRARY)  $(FULL_C_OBJS)

	@echo [LD] $(LOCAL_LIBRARY)
ifneq ($(PRJ_LIB_NAME),)
	$(Q)$(LD) $(LD_FLAGS) -o $(LOCAL_LIBRARY) @$(OBJ_LIST_FILE) -L$(PRJ_LIB_PATH) -l$(PRJ_LIB_NAME)
else
	$(Q)$(LD) $(LD_FLAGS) -o $(LOCAL_LIBRARY) @$(OBJ_LIST_FILE) -L$(PRJ_LIB_PATH) 
endif

	@echo $(OBC_LOCAL_MOUDLE_NAME)  Code Size Info: >> "$(CODE_SIZE_FILE)"
	@-$(SIZE) -t $(LOCAL_LIBRARY)               >> "$(CODE_SIZE_FILE)"

# image file BIN & NM
do_post_build : $(LOCAL_IMAGE_FILE)
	@echo - do [$@]

$(LOCAL_IMAGE_FILE) : $(LOCAL_LIBRARY) | do_post_build_before
	$(Q)echo [BIN] $@
	$(Q)xt-nm --format=posix $(LOCAL_LIBRARY) > $(LOCAL_MAP_FILE)
	$(Q)xt-objdump -D $(LOCAL_LIBRARY)    > "$(LOCAL_DUMP_FILE)" && xt-objdump -h $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .debug_aranges $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .debug_abbrev $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .debug_pubnames $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .debug_info $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .debug_line $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .xtensa.info $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .xt.lit $(LOCAL_LIBRARY)
	#$(Q)xt-objcopy -R .xt.prop $(LOCAL_LIBRARY)
	$(Q)xt-objcopy -R .debug_aranges -R .debug_abbrev -R .debug_pubnames -R .debug_info -R .debug_line -R .xtensa.info -R .xt.lit -R .xt.prop -R .comment $(LOCAL_LIBRARY)
	$(Q)xt-dumpelf $(DTCM_FLAGS) $(LOCAL_LIBRARY) > $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)_dtcm.dram0.tmp
	wine $(ASC_TO_BIN_TOOL) $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)_dtcm.dram0.tmp $(LOCAL_MOUDLE_DTCM_NAME)
	$(Q)xt-dumpelf $(ITCM_FLAGS) $(LOCAL_LIBRARY) > $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)_itcm.iram0.tmp
	wine $(ASC_TO_BIN_TOOL) $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)_itcm.iram0.tmp $(LOCAL_MOUDLE_ITCM_NAME)


# Clean up 
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -rf $(LOCAL_OBJ_PATH)
	$(Q)-rm -f $(OBJ_LIST_FILE) $(CODE_SIZE_FILE) $(LOCAL_LIBRARY) \
				$(OBC_LOCAL_CLEAN_FILES) $(LOCAL_IMAGE_FILE) $(LOCAL_MAP_FILE) $(LOCAL_DUMP_FILE)
	$(Q)-rm -rf $(OBB_PRODUCT_DELIVERY_DIR)/lib/lphy*
ifneq ($(OBC_LOCAL_CLEAN_DIRS),)
	$(Q)-rm -rf $(OBC_LOCAL_CLEAN_DIRS)
endif
