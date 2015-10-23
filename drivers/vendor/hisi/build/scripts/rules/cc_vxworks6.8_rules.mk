include $(BALONG_TOPDIR)/build/scripts/make_base.mk

ifeq ($(HUTAF_HLT_COV),true)
include $(BALONG_TOPDIR)/build/tools/linux_avatar_64/HLLT_init_ccore.mk
endif

#***********************************************************
# input parameters
#***********************************************************
OBC_LOCAL_CORE_NAME          ?=

OBC_LOCAL_SRC_FILE           ?=
OBC_LOCAL_SRC_TEST_FILE      ?=
OBC_LOCAL_SRC_DIR            ?=
OBC_LOCAL_INC_DIR            ?=
OBC_LOCAL_LIB_DIR            ?=

OBC_LOCAL_USER_OBJ_DIR       ?=
OBC_LOCAL_USER_OBJ_FILE      ?=
OBC_LOCAL_USER_OBJ_DYN_FILE  ?=
OBC_LOCAL_ENBAL_LIB_O        ?=YES
OBC_LOCAL_SEPERATE_OBJ_FILES ?=

OBC_LOCAL_CLEAN_FILES        ?=
OBC_LOCAL_CLEAN_DIRS         ?=

CC_USER_FLAGS     ?=
AS_USER_FLAGS     ?=

ifeq ($(OBB_LPHY_RTT),true)
CC_USER_FLAGS                += -DDSP_RTT_VERSION
endif

ifneq ($(strip $(OBC_LOCAL_SEPERATE_OBJ_FILES)),)
OBC_LOCAL_ENBAL_LIB_O=NO
endif

# define core macro map, TODO:clear this kit
ifeq ($(strip $(OBC_LOCAL_CORE_NAME)),ccore)
    CC_USER_FLAGS += -DOBC_IMAGE_MODEM
    AS_USER_FLAGS += -DOBC_IMAGE_MODEM
endif
ifeq ($(strip $(OBC_LOCAL_CORE_NAME)),vxworks)
    CC_USER_FLAGS += -DOBC_IMAGE_MODEM
    AS_USER_FLAGS += -DOBC_IMAGE_MODEM
endif
ifeq ($(strip $(OBC_LOCAL_CORE_NAME)),bootrom)
    CC_USER_FLAGS += -DOBC_IMAGE_BOOTROM
    AS_USER_FLAGS += -DOBC_IMAGE_BOOTROM
endif
ifeq ($(strip $(OBC_LOCAL_CORE_NAME)),bootloader)
    CC_USER_FLAGS += -DOBC_IMAGE_BOOTLOADER
    AS_USER_FLAGS += -DOBC_IMAGE_BOOTLOADER
endif
ifeq ($(strip $(OBC_LOCAL_CORE_NAME)),acore)
    CC_USER_FLAGS += -DOBC_IMAGE_APP
    AS_USER_FLAGS += -DOBC_IMAGE_APP
endif

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
OS_LIB_PATH             :=$(BALONG_TOPDIR)/build/libs/os/ccore/$(CFG_OS_LIB)
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
CC = $(DISTCC) $(HUTAF_HLT_WRAPPER) $(SOURCEANALYZER) $(CCACHE) ccarm
AS = $(SOURCEANALYZER) $(CCACHE) asarm
LIBAR           ?= ararm

ifeq ($(strip $(OBB_BUILD_TYPE)),RELEASE)
OBC_C_OPTIM_OPTION    ?= -Os -g
OBC_CPP_OPTIM_OPTION  ?= -Os
OBC_AS_OPTIM_OPTION   ?= -Os
else 
OBC_C_OPTIM_OPTION    ?= -Os -g
OBC_CPP_OPTIM_OPTION  ?= -Os  
OBC_AS_OPTIM_OPTION   ?= 
endif
OBC_CC_OPTION := -c -t7 -msoft-float -fdollars-in-identifiers \
	-fno-builtin -fvolatile -fno-feature-proxy -fno-zero-initialized-in-bss \
	-fsigned-char -falign-arrays  -MD \
	-Wmissing-braces -Wnonnull  -Wcomment  \
	-Wchar-subscripts  -Wimplicit-int -Wmain -Wparentheses  -Wpointer-sign  \
	-Wreturn-type  -Wsequence-point  -Wsign-compare -Wstrict-aliasing    -Wswitch  \
	-Wtrigraphs  -Wuninitialized  -Wunknown-pragmas  -Wunused-label -Wunused-value  -Wunused-variable  \
	-Wvolatile-register-var
#	-Wimplicit-function-declaration -Wunused-function  
         
OBC_C_DEFINES := -D_WRS_KERNEL -DCPU_CORTEXA8 -DARMMMU=ARMMMU_CORTEXA8 \
	-DARMCACHE=ARMCACHE_CORTEXA8 -DIP_PORT_VXWORKS=68 -DINET -DARMEL
LOCAL_CC_FLAGS     := $(OBC_CC_OPTION) $(OBC_C_DEFINES) 

OBC_AS_OPTION   ?= -c -t7 -msoft-float -fdollars-in-identifiers -fno-builtin -fvolatile -fno-feature-proxy -fno-zero-initialized-in-bss -xassembler-with-cpp  -MD
OBC_AS_DEFINES  := -D_WRS_KERNEL -DCPU_CORTEXA8 -DARMMMU=ARMMMU_CORTEXA8 \
	-DARMCACHE=ARMCACHE_CORTEXA8 -DIP_PORT_VXWORKS=68 -DINET -DARMEL
LOCAL_AS_FLAGS  := $(OBC_AS_OPTION) $(OBC_AS_DEFINES) 


# LD & AR
LD       := $(SOURCEANALYZER) ldarm
LDFLAGS  :=-r -o

ifeq ($(OBC_LOCAL_ENBAL_LIB_O), YES)
AR     := $(SOURCEANALYZER) ldarm 
ARFLAG :=-r -o
LIB_EXT_NAME=.o
else 
AR     := $(SOURCEANALYZER) ararm
ARFLAG :=-qcs
LIB_EXT_NAME=.a
endif

SIZE =sizearm
OBJ_DUMP = objdumparm.exe -D 
#*************************************************************************
# OS System Include files
#*************************************************************************
SYS_OS_INC_DIR := $(WIND_HOME)/vxworks-6.8/target/h \
            $(WIND_HOME)/vxworks-6.8/target/h/types \
            $(WIND_HOME)/vxworks-6.8/target/h/wrn/coreip \
	    $(WIND_HOME)/vxworks-6.8/target/config/all \
            $(WIND_HOME)/components/ip_net2-6.8/vxcompat/include \
	    $(OBB_PRODUCT_DELIVERY_DIR)/os/$(OBC_LOCAL_CORE_NAME) \
	    $(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
		$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc

OS_LIB_TAG_PATH         :=$(OS_LIB_PATH)/tags/$(VX_CPU_FAMILY)/$(CPU)

#*************************************************************************
# Compiler-Specific Configuration
#*************************************************************************
LOCAL_CC := $(CC) $(OBC_CC_OPTION) $(OBC_C_OPTIM_OPTION) $(OBC_C_DEFINES) \
    -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL)   -D__VXWORKS__ \
    -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" \
    $(CC_USER_DEFINES)  $(CC_USER_FLAGS) 

#  π”√C±‡“Î∆˜±‡“Îª„±‡¥˙¬Î 
LOCAL_AS = $(CC) $(OBC_AS_OPTION) $(OBC_AS_OPTIM_OPTION) $(OBC_AS_DEFINES) \
    -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL)   -D__VXWORKS__ \
    -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\" \
    $(AS_USER_DEFINES) $(AS_USER_FLAGS)

#*************************************************************************
# Direcotry configuration to output compiling result
#*************************************************************************
LOCAL_OBJ_PATH       := $(OBB_PRODUCT_DELIVERY_DIR)/obj/$(OBC_LOCAL_MOUDLE_NAME)
LOCAL_LIBRARY        := $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME)
LOCAL_DEBUG_LIBRARY  := $(OB_ENV_PRJ_DEBUG_MODULE_FILE).o

LOCAL_USER_LIB_OBJ_PATH   :=$(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)
ifneq ($(strip $(OBC_LOCAL_SEPERATE_OBJ_FILES)),)
LOCAL_SEPERATE_OBJ_FILES_TARGET := $(addprefix $(LOCAL_USER_LIB_OBJ_PATH)/, $(OBC_LOCAL_SEPERATE_OBJ_FILES))
endif

LOCAL_SRC_DIR := $(OBC_LOCAL_SRC_DIR)
LOCAL_INC_DIR := $(SYS_OS_INC_DIR) $(OBC_LOCAL_INC_DIR) $(OBB_PRODUCT_CONFIG_DIR) $(OBB_PRODUCT_GU_CONFIG_DIR) $(OBB_CHIPSET_DIR)
LOCAL_LIB_DIR := $(OBC_LOCAL_LIB_DIR)

# CC configuration 
C_DIR_FILES  := $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.c $(dir)/*.C))
FULL_C_SRCS      := $(filter %.c,$(OBC_LOCAL_SRC_FILE)) $(subst .C,.c, $(C_DIR_FILES)) 
FULL_C_OBJS       := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(FULL_C_SRCS))
FULL_C_DEPENDS    := $(patsubst %.o,%.d,$(FULL_C_OBJS))

ROM_COMPRESS_C_SRCS      := $(filter %.c,$(OBC_LOCAL_SRC_ROM_COMPRESS_FILE)) $(subst .C,.c, $(C_DIR_FILES)) 
ROM_COMPRESS_C_OBJS       := $(patsubst $(BALONG_TOPDIR)/%.c,$(LOCAL_OBJ_PATH)/%.o, $(ROM_COMPRESS_C_SRCS))
ROM_COMPRESS_C_DEPENDS    := $(patsubst %.o,%.d,$(ROM_COMPRESS_C_OBJS))

FULL_SRC_INC_PATH      := $(foreach lib_inc_path, $(LOCAL_INC_DIR), -I$(subst \,/,$(lib_inc_path)))

# AS configuration 
AS_DIR_FILES  := $(foreach dir,$(LOCAL_SRC_DIR),$(wildcard $(dir)/*.s $(dir)/*.S))
#FULL_AS_SRCS      := $(filter %.s,$(OBC_LOCAL_SRC_FILE)) $(subst .S,.s, $(AS_DIR_FILES))
FULL_AS_LOWER_SUFFIX_SRCS := $(filter %.s,$(OBC_LOCAL_SRC_FILE) $(AS_DIR_FILES))
FULL_AS_UPPER_SUFFIX_SRCS := $(filter %.S,$(OBC_LOCAL_SRC_FILE) $(AS_DIR_FILES))
FULL_AS_LOWER_SUFFIX_OBJS := $(patsubst $(BALONG_TOPDIR)/%.s,$(LOCAL_OBJ_PATH)/%.o, $(FULL_AS_LOWER_SUFFIX_SRCS))
FULL_AS_UPPER_SUFFIX_OBJS := $(patsubst $(BALONG_TOPDIR)/%.S,$(LOCAL_OBJ_PATH)/%.o, $(FULL_AS_UPPER_SUFFIX_SRCS))
FULL_AS_OBJS              := $(FULL_AS_LOWER_SUFFIX_OBJS) $(FULL_AS_UPPER_SUFFIX_OBJS)
FULL_AS_DEPENDS           := $(patsubst %.o,%.d,$(FULL_AS_LOWER_SUFFIX_OBJS) $(FULL_AS_UPPER_SUFFIX_OBJS))

ROM_COMPRESS_AS_SRCS      := $(filter %.s,$(OBC_LOCAL_SRC_ROM_COMPRESS_FILE)) $(AS_DIR_FILES)
ROM_COMPRESS_AS_OBJS       := $(patsubst $(BALONG_TOPDIR)/%.s,$(LOCAL_OBJ_PATH)/%.o, $(ROM_COMPRESS_AS_SRCS))
ROM_COMPRESS_AS_DEPENDS    := $(patsubst %.o,%.d,$(ROM_COMPRESS_AS_OBJS))

# Obj to AR configuration.
OBJ_DIR_FILES           := $(foreach dir,$(OBC_LOCAL_USER_OBJ_DIR),$(wildcard $(dir)/*.o $(dir)/*.O))
ALL_USER_OBJS           := $(OBC_LOCAL_USER_OBJ_FILE) $(OBJ_DIR_FILES)

OBJ_C_DIRS              := $(sort $(dir $(FULL_C_OBJS)))
OBJ_C_DIRS              := $(patsubst %/,%,$(OBJ_C_DIRS))
OBJ_AS_DIRS             := $(sort $(dir $(FULL_AS_OBJS)))
OBJ_AS_DIRS             := $(patsubst %/,%,$(OBJ_AS_DIRS))

CODE_SIZE_FILE			:= $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME).code.size
OBJ_LIST_FILE			:= $(LOCAL_OBJ_PATH)/$(OBC_LOCAL_MOUDLE_NAME)$(LIB_EXT_NAME).obj.list



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
do_build : $(FULL_C_OBJS)  $(FULL_AS_LOWER_SUFFIX_OBJS) $(FULL_AS_UPPER_SUFFIX_OBJS)
	@echo do [$@]

ifneq ($(strip $(FULL_C_OBJS)),)
ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_C_DEPENDS)
endif

$(FULL_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c | do_build_before
	@echo [CC] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
#	$(Q)$(LOCAL_CC) -Xmake-dependency=5 -Xmake-dependency-savefile=$(basename $@).d $(FULL_SRC_INC_PATH) -c $< -o $@ 
ifeq ($(DISABLE_VXWORKS_DEPEND),true)
	$(Q)$(LOCAL_CC) $(FULL_SRC_INC_PATH) -c $< -o $@ 
else
	$(Q)$(LOCAL_CC) -MD -MF $(basename $@).d $(FULL_SRC_INC_PATH) -c $< -o $@ 
	$(Q)sed -e 's,\([A-Za-z]:\)\([^ /\\]\),\1/\2,g' -e 's,\([^ ]\)\\\([^\r\n]\),\1/\2,g' -i $(basename $@).d
endif

$(ROM_COMPRESS_C_OBJS): $(LOCAL_OBJ_PATH)/%.o : $(BALONG_TOPDIR)/%.c | do_build_before
	@echo [CC] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
#	$(Q)$(LOCAL_CC) -Xmake-dependency=5 -Xmake-dependency-savefile=$(basename $@).d $(FULL_SRC_INC_PATH) -c $< -o $@
ifeq ($(DISABLE_VXWORKS_DEPEND),true)
	$(Q)$(LOCAL_CC) $(FULL_SRC_INC_PATH) -c $< -o $@
else
	$(Q)$(LOCAL_CC) -MD -MF $(basename $@).d $(FULL_SRC_INC_PATH) -c $< -o $@
	$(Q)sed -e 's,\([A-Za-z]:\)\([^ /\\]\),\1/\2,g' -e 's,\([^ ]\)\\\([^\r\n]\),\1/\2,g' -i $(basename $@).d
endif

ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
endif 

ifneq ($(strip $(FULL_AS_LOWER_SUFFIX_OBJS) $(FULL_AS_UPPER_SUFFIX_OBJS)),)
ifneq ($(MAKECMDGOALS),clean)
sinclude $(FULL_AS_DEPENDS)
endif

$(FULL_AS_LOWER_SUFFIX_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.s | do_build_before
	@echo [AS] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
ifeq ($(DISABLE_VXWORKS_DEPEND),true)
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@ 
else
	$(Q)$(LOCAL_AS) -MD $(FULL_SRC_INC_PATH) -c $< -o $@
	$(Q)sed -e 's,\([A-Za-z]:\)\([^ /\\]\),\1/\2,g' -e 's,\([^ ]\)\\\([^\r\n]\),\1/\2,g' -i $(basename $@).d
endif

ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
$(FULL_AS_UPPER_SUFFIX_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.S | do_build_before
	@echo [AS] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
ifeq ($(DISABLE_VXWORKS_DEPEND),true)
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@ 
else
	$(Q)$(LOCAL_AS) -MD $(FULL_SRC_INC_PATH) -c $< -o $@
	$(Q)sed -e 's,\([A-Za-z]:\)\([^ /\\]\),\1/\2,g' -e 's,\([^ ]\)\\\([^\r\n]\),\1/\2,g' -i $(basename $@).d
endif

$(ROM_COMPRESS_AS_OBJS): $(LOCAL_OBJ_PATH)/%.o :$(BALONG_TOPDIR)/%.s | do_build_before
	@echo [AS] $@
#	$(Q)-mkdir -p $(patsubst %/,%,$(dir $@))
ifeq ($(DISABLE_VXWORKS_DEPEND),true)
	$(Q)$(LOCAL_AS) $(FULL_SRC_INC_PATH) -c $< -o $@
else
	$(Q)$(LOCAL_AS) -MD $(FULL_SRC_INC_PATH) -c $< -o $@
	$(Q)sed -e 's,\([A-Za-z]:\)\([^ /\\]\),\1/\2,g' -e 's,\([^ ]\)\\\([^\r\n]\),\1/\2,g' -i $(basename $@).d
endif

ifeq ($(CFG_FEATURE_OBJ_DUMP),YES)
	@echo [DUMP] $(LOCAL_OBJ_PATH)/$(notdir $@)
	$(Q)cp -f $@ $(LOCAL_OBJ_PATH)/
endif
endif

# Lib AR or LD...
do_lib : $(LOCAL_LIBRARY) romCompress.a
	@echo do [$@]

romCompress.a: $(ROM_COMPRESS_AS_OBJS)  $(ROM_COMPRESS_C_OBJS)
ifneq ($(strip $(OBC_LOCAL_SRC_ROM_COMPRESS_FILE)),)
	$(Q)echo $(ROM_COMPRESS_AS_OBJS) $(ROM_COMPRESS_C_OBJS) > $(OBB_PRODUCT_DELIVERY_DIR)/lib/romCompress.a.obj.list
	$(Q)$(AR) $(ARFLAG) $(OBB_PRODUCT_DELIVERY_DIR)/lib/romCompress.a  @$(OBB_PRODUCT_DELIVERY_DIR)/lib/romCompress.a.obj.list
endif

$(LOCAL_LIBRARY): $(FULL_C_OBJS)  $(FULL_AS_OBJS) | do_lib_before
	$(Q)mkdir -p $(LOCAL_OBJ_PATH)
	$(Q)echo $(FULL_AS_OBJS) $(FULL_C_OBJS)  > $(OBJ_LIST_FILE)
ifneq ($(strip $(ALL_USER_OBJS)),)
	@echo $(ALL_USER_OBJS) >>$(OBJ_LIST_FILE)
endif
ifneq ($(strip $(OBC_LOCAL_USER_OBJ_DYN_FILE)),)
	@echo $(OBC_LOCAL_USER_OBJ_DYN_FILE) >>$(OBJ_LIST_FILE)
endif
	@echo [AR] $(LOCAL_LIBRARY)
	$(Q)$(AR) $(ARFLAG)  $(LOCAL_LIBRARY)  @$(OBJ_LIST_FILE)

	@echo $(OBC_LOCAL_MOUDLE_NAME)  Code Size Info: >> "$(CODE_SIZE_FILE)"
	@-$(SIZE) -t $(LOCAL_LIBRARY)               >> "$(CODE_SIZE_FILE)"

ifneq ($(strip $(OB_ENV_PRJ_DEBUG_MODULE_FILE)),)
	@echo [AR] $(LOCAL_DEBUG_LIBRARY)
	$(Q)$(LD) $(LDFLAGS) $(LOCAL_DEBUG_LIBRARY)  @$(OBJ_LIST_FILE)
endif

# Post-Build 
do_post_build: $(LOCAL_SEPERATE_OBJ_FILES_TARGET)
	@echo -  do [$@]
	@echo Build $(OBC_LOCAL_MOUDLE_NAME) Complete!
	
ifneq ($(LOCAL_SEPERATE_OBJ_FILES_TARGET),)
$(LOCAL_SEPERATE_OBJ_FILES_TARGET) :  $(LOCAL_LIBRARY) | do_post_build_before
	@echo [AR -x] $@
	$(Q)mkdir -p $(LOCAL_USER_LIB_OBJ_PATH)
	$(Q)xchmod $(LOCAL_USER_LIB_OBJ_PATH)
	$(Q)$(LIBAR) -x $(LOCAL_LIBRARY) $(OBC_LOCAL_SEPERATE_OBJ_FILES)
	$(Q)$(LIBAR) -d $(LOCAL_LIBRARY) $(OBC_LOCAL_SEPERATE_OBJ_FILES)
	$(Q)mv -f -v $(OBC_LOCAL_SEPERATE_OBJ_FILES) $(LOCAL_USER_LIB_OBJ_PATH)
endif

#***********************************************************
# Clean up 
#***********************************************************
.PHONY: clean do_clean
clean: do_clean
	@echo -  do [$@]
	$(Q)-rm -rf $(LOCAL_OBJ_PATH)
	$(Q)-rm -f $(OBJ_LIST_FILE) $(CODE_SIZE_FILE) $(LOCAL_LIBRARY) $(LOCAL_DEBUG_LIBRARY)  
	$(Q)-rm -f  $(LOCAL_SEPERATE_OBJ_FILES_TARGET) $(OBC_LOCAL_CLEAN_FILES)  
ifneq ($(OBC_LOCAL_CLEAN_DIRS),)
	$(Q)-rm -rf $(OBC_LOCAL_CLEAN_DIRS)
endif


#***********************************************************
# pclint
#***********************************************************
.PHONY: $(maction) do_$(maction)

$(maction): do_$(maction)