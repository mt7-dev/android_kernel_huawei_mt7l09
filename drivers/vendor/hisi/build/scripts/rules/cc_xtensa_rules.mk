include $(BALONG_TOPDIR)/build/scripts/make_base.mk
#****************************************************************************
# input parameters
#****************************************************************************
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

#****************************************************************************
# compiler configs
#****************************************************************************
CC		:=$(subst \,/,$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RD-2011.1-win32/XtensaTools/bin/xt-xcc)
AS		:=$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RD-2011.1-win32/XtensaTools/bin/xt-xcc
AR		:=$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RD-2011.1-win32/XtensaTools/bin/xt-ar
LD		:=$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RD-2011.1-win32/XtensaTools/bin/xt-ld
SIZE	:=$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RD-2011.1-win32/XtensaTools/bin/xt-size

CC_FLAGS_DEPEND		:= 
CC_FLAGS			:= -c -g -O2 -mlongcalls -mtext-section-literals 
CC_FLAGS			+= $(PRODUCT_CFG_CFLAGS) $(SUBSYS_CFG_CFLAGS) $(OBC_USER_CC_FLAGS) $(CC_FLAGS_DEPEND)

AS_FLAGS			:= -c -g -O2 -mlongcalls 
AS_FLAGS			+= $(PRODUCT_CFG_CFLAGS) $(SUBSYS_CFG_CFLAGS) $(AS_FLAGS) $(OBC_USER_CC_FLAGS) 
         
AR_FLAGS			:=rv
LD_FLAGS 			=  -defsym __stack_start=0x3f79

#****************************************************************************
# Direcotry configuration
#****************************************************************************
LOCAL_OBJ_PATH		:= $(OBB_PRODUCT_DELIVERY_DIR)/obj/$(OBC_LOCAL_MOUDLE_NAME)
LOCAL_LIBRARY		:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).out
LOCAL_IMAGE_FILE	:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)

# INC
OBC_LOCAL_INC_DIR	+= $(OBB_PRODUCT_CONFIG_DIR) $(OBB_PRODUCT_GU_CONFIG_DIR) $(OBB_CHIPSET_DIR)
FULL_SRC_INC_PATH	:= ${foreach lib_inc_path, ${OBC_LOCAL_INC_DIR}, -I${lib_inc_path}}

# CC configuration 	
C_DIR_FILES			:= $(foreach dir ,$(OBC_LOCAL_SRC_DIR) ,$(wildcard $(dir)/*.c $(dir)/*.C))
FULL_C_SRCS			:= $(filter %.c,$(OBC_LOCAL_SRC_FILE)) $(subst .C,.c, $(C_DIR_FILES)) 
FULL_C_OBJS       := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(FULL_C_SRCS))
FULL_C_DEPENDS		:= $(subst .o,.d,$(FULL_C_OBJS))

# AS configuration 
AS_DIR_FILES		:= $(foreach dir,$(OBC_LOCAL_SRC_DIR),$(wildcard $(dir)/*.s $(dir)/*.S))
FULL_AS_SRCS		:= $(filter %.s,$(OBC_LOCAL_SRC_FILE)) $(filter %.S,$(OBC_LOCAL_SRC_FILE)) $(subst .s,.S, $(AS_DIR_FILES))
FULL_AS_OBJS        := $(patsubst $(BALONG_TOPDIR)/%.s,$(LOCAL_OBJ_PATH)/%.o, $(FULL_AS_SRCS)) $(patsubst $(BALONG_TOPDIR)/%.S,$(LOCAL_OBJ_PATH)/%.o, $(FULL_AS_SRCS))

OBJ_C_DIRS          := $(sort $(dir $(FULL_C_OBJS)))
OBJ_C_DIRS          := $(patsubst %/,%,$(OBJ_C_DIRS))
OBJ_AS_DIRS         := $(sort $(dir $(FULL_AS_OBJS)))
OBJ_AS_DIRS         := $(patsubst %/,%,$(OBJ_AS_DIRS))

CODE_SIZE_FILE			:= $(LOCAL_LIBRARY).code.size
OBJ_LIST_FILE			:= $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME).obj.list

# AR config
filter_objs = $(patsubst $(BALONG_TOPDIR)/%.o,$(LOCAL_OBJ_PATH)/%.o, $(patsubst %.C,%.o,$(patsubst %.c,%.o,$(patsubst %.S,%.o,$(patsubst %.s,%.o,$(1))))))

LIB_om :=$(LOCAL_OBJ_PATH)/om.a
OBJS_om :=$(call filter_objs,$(src_om))
LIB_comm :=$(LOCAL_OBJ_PATH)/comm.a 
OBJS_comm :=$(call filter_objs,$(src_comm))
LIB_codec_amr :=$(LOCAL_OBJ_PATH)/codec_amr.a 
OBJS_codec_amr :=$(call filter_objs,$(src_codec_amr))
LIB_codec_amrwb :=$(LOCAL_OBJ_PATH)/codec_amrwb.a 
OBJS_codec_amrwb :=$(call filter_objs,$(src_codec_amrwb))
LIB_codec_efr :=$(LOCAL_OBJ_PATH)/codec_efr.a 
OBJS_codec_efr :=$(call filter_objs,$(src_codec_efr))
LIB_drv:=$(LOCAL_OBJ_PATH)/drv.a 
OBJS_drv :=$(call filter_objs,$(src_drv))
LIB_zos :=$(LOCAL_OBJ_PATH)/zos.a
OBJS_zos :=$(call filter_objs,$(src_osa))
LIB_codec_fr :=$(LOCAL_OBJ_PATH)/codec_fr.a
OBJS_codec_fr :=$(call filter_objs,$(src_codec_fr))
LIB_codec_hr :=$(LOCAL_OBJ_PATH)/codec_hr.a
OBJS_codec_hr :=$(call filter_objs,$(src_codec_hr))
LIB_codec_main :=$(LOCAL_OBJ_PATH)/codec_main.a
OBJS_codec_main :=$(call filter_objs,$(src_codec_main))

AR_LIBS := $(LIB_drv) $(LIB_om) $(LIB_comm) $(LIB_codec_amr) $(LIB_codec_amrwb) $(LIB_codec_efr) $(LIB_codec_fr) $(LIB_codec_hr) $(LIB_codec_main) $(LIB_zos)

# LD
TARGET_LINK_SCRIPT :=$(LOCAL_OBJ_PATH)/$(notdir $(OBC_LINK_SCRIPT))
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

ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_C_DEPENDS)
endif

$(FULL_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c  | do_build_before
	@echo [CC] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
	$(Q)$(CC) $(CC_FLAGS)  $(FULL_SRC_INC_PATH) -MD -MF $(LOCAL_OBJ_PATH)/$*.d -c $< -o $@
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif

ifneq ($(strip $(FULL_AS_OBJS)),)
$(FULL_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.S | do_build_before
	@echo [AS] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
	$(Q)$(AS) $(AS_FLAGS) $(FULL_SRC_INC_PATH) -c $< -o $@
ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@cp $@  $(LOCAL_OBJ_PATH)/
endif
endif

# Lib AR or LD...
do_lib : $(LOCAL_LIBRARY)
	@echo do [$@]

$(LIB_drv):$(OBJS_drv) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_om):$(OBJS_om) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_comm):$(OBJS_comm) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_codec_amr):$(OBJS_codec_amr) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_codec_amrwb):$(OBJS_codec_amrwb) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_codec_efr):$(OBJS_codec_efr) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_codec_fr):$(OBJS_codec_fr) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_codec_hr):$(OBJS_codec_hr) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_codec_main):$(OBJS_codec_main) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^
$(LIB_zos):$(OBJS_zos) | do_lib_before
	@echo [AR] $@
	$(AR) $(AR_FLAGS) $@ $^

$(LOCAL_LIBRARY): $(AR_LIBS) $(OS_LIBS) $(OBC_LINK_SCRIPT) $(FULL_C_OBJS) $(FULL_AS_OBJS) | do_lib_before
	$(Q)echo [LD] $@
	$(Q)xt-genldscripts -b $(OBC_LINK_SCRIPT)
	$(CC) -mlsp="$(OBC_LINK_SCRIPT)" -o $(LOCAL_LIBRARY) -Wl,--start-group -Wl,$(AR_LIBS) -Wl,--end-group -Wl,-Map -Wl,$(basename $(LOCAL_LIBRARY)).map
	@echo $(OBC_LOCAL_MOUDLE_NAME)  Code Size Info: >> "$(CODE_SIZE_FILE)"
	@-$(SIZE) -t $(LOCAL_LIBRARY)               >> "$(CODE_SIZE_FILE)"

# image file BIN & NM
do_post_build : $(LOCAL_IMAGE_FILE).bin 
	@echo - do [$@]

$(LOCAL_IMAGE_FILE).bin : $(LOCAL_LIBRARY) | do_post_build_before
	@echo [BIN] $@
	$(OBB_SCRIPTS_DIR)/makebin $(LOCAL_IMAGE_FILE).out $(LOCAL_IMAGE_FILE).bin $(OBB_SCRIPTS_DIR)/makebin.ini
	
# Clean up 
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -rf $(LOCAL_OBJ_PATH)
	$(Q)-rm -f $(OBJ_LIST_FILE) $(CODE_SIZE_FILE) $(LOCAL_LIBRARY) \
				$(OBC_LOCAL_CLEAN_FILES) $(LOCAL_IMAGE_FILE).bin $(LOCAL_IMAGE_FILE).map $(LOCAL_IMAGE_FILE).out \
				$(TARGET_LINK_SCRIPT) $(AR_LIBS) 
ifneq ($(OBC_LOCAL_CLEAN_DIRS),)
	$(Q)-rm -rf $(OBC_LOCAL_CLEAN_DIRS)
endif

    






