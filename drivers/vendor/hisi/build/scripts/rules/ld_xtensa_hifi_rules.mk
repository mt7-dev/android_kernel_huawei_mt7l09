include $(BALONG_TOPDIR)/build/scripts/make_base.mk

ifneq ($(FORTIFY),)
ifeq ($(OBB_OS_TYPE),Linux)
SOURCEANALYZER := $(BALONG_TOPDIR)/llt/ci/tools/fortify_linux/bin/sourceanalyzer -b $(OBB_PRODUCT_NAME)
else
SOURCEANALYZER := $(BALONG_TOPDIR)/llt/ci/tools/fortify_windows/bin/sourceanalyzer.exe -b $(OBB_PRODUCT_NAME)
endif
endif
#****************************************************************************
# input parameters
#****************************************************************************
OBC_LOCAL_LIB_FILE           ?=

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
AS		:= $(SOURCEANALYZER) $(CCACHE_BIN) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-xcc
AR		:=$(SOURCEANALYZER) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-ar
LD		:=$(SOURCEANALYZER) $(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-ld
SIZE	:=$(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-size
XT_GENLD :=$(TENSILICA_TOOLS_HIFI)/XtensaTools/bin/xt-genldscripts

CC_FLAGS_DEPEND		:= 
CC_FLAGS			:= -c -g -O2 -mlongcalls -mtext-section-literals --xtensa-core=$(OBC_LOCAL_CORE_NAME) --xtensa-system=$(subst \,/,$(XTENSA_SYSTEM_HIFI))

AS_FLAGS			:= -c -g -O2 -mlongcalls --xtensa-core=$(OBC_LOCAL_CORE_NAME) --xtensa-system=$(subst \,/,$(XTENSA_SYSTEM_HIFI))
         
AR_FLAGS			:=rv
LD_FLAGS 			= -b $(CFG_HIFI_LINK_DIR_NAME) --xtensa-core=$(OBC_LOCAL_CORE_NAME) --xtensa-system=$(subst \,/,$(XTENSA_SYSTEM_HIFI))

#****************************************************************************
# Direcotry configuration
#****************************************************************************
LOCAL_LIBRARY		:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME).out
LOCAL_IMAGE_FILE	:= $(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)
LOCAL_OUT2BIN_TOOL	:= elf2bin

DELIVERY_LIB_DIR      := $(OBB_PRODUCT_DELIVERY_DIR)/lib

#tool for elf to bin
ifeq ($(OBB_OS_TYPE),Windows)
ELF2BIN_GUHIFI := $(BALONG_TOPDIR)/build/tools/utility/tools/elf2bin/elf2bin.exe
else
ELF2BIN_GUHIFI := $(BALONG_TOPDIR)/build/tools/utility/tools/elf2bin/elftobin
endif

ELF2BIN_GUHIFI_INI  := $(BALONG_TOPDIR)/build/tools/utility/tools/elf2bin/guhifi/elftobin.ini

CODE_SIZE_FILE			:= $(LOCAL_LIBRARY).code.size

#****************************************************************************
# compile
#****************************************************************************
.PHONY: all do_pre_link_before do_pre_link do_link_before do_link do_lib_before do_lib do_post_link_before do_post_link 
		
all: do_pre_link_before do_pre_link do_link_before do_link do_lib_before do_lib do_post_link_before do_post_link 
	@echo - do [$@]

# Rules order
do_post_link : do_post_link_before
do_post_link_before : do_link
do_link : do_link_before
do_link_before : do_pre_link
do_pre_link : do_pre_link_before

# Lib AR or LD...
do_link : $(LOCAL_LIBRARY)
	@echo do [$@]

$(LOCAL_LIBRARY):  | do_lib_before
	$(Q)echo [LD] $@
	$(XT_GENLD) $(LD_FLAGS)
	$(CC) $(CC_FLAGS) -mlsp="$(OBC_LD_SCRIPT_DIR)" -o $(LOCAL_LIBRARY) -Wl,--start-group -Wl,$(OBC_LOCAL_LIB_FILE) -Wl,--end-group -Wl,-Map -Wl,$(basename $(LOCAL_LIBRARY)).map
	@echo $(OBC_LOCAL_MOUDLE_NAME)  Code Size Info: >> "$(CODE_SIZE_FILE)"
	@-$(SIZE) -t $(LOCAL_LIBRARY)               >> "$(CODE_SIZE_FILE)"

# image file BIN & NM
do_post_link : $(LOCAL_IMAGE_FILE).bin 
	@echo - do [$@]

$(LOCAL_IMAGE_FILE).bin : $(LOCAL_LIBRARY) | do_post_link_before
	$(Q)cd $(BALONG_TOPDIR)/build/tools/utility/tools/elf2bin/guhifi && $(ELF2BIN_GUHIFI) $(DELIVERY_LIB_DIR)/guhifi_mcore.out $(DELIVERY_LIB_DIR)/guhifi_mcore.bin
	@echo [BIN] $@
	@echo OBB_SCRIPTS_DIR $(OBB_SCRIPTS_DIR)
	@echo LOCAL_IMAGE_FILE $(LOCAL_IMAGE_FILE)
	
# Clean up 
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -f $(CODE_SIZE_FILE) $(LOCAL_LIBRARY) \
				$(LOCAL_IMAGE_FILE).bin $(LOCAL_IMAGE_FILE).map $(LOCAL_IMAGE_FILE).out
	$(Q)-rm -rf $(OBC_LD_SCRIPT_DIR)/ldscripts

    






