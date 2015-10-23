#*****************************************************************************
# History :
#*****************************************************************************
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

ifeq ($(HUTAF_HLT_COV),true)
include $(BALONG_TOPDIR)/build/tools/linux_avatar_64/HLLT_init_ccore.mk
endif

#*****************************************************************************
# input parameters
#*****************************************************************************
OBC_LOCAL_LIB_FILE		?=
OBC_LOCAL_LIB_DIR		?=

OBC_LOCAL_CORE_NAME		?=
OBC_LOCAL_MOUDLE_NAME	        ?=

OBC_LD_RAM_LOW_ADDR		?=
OBC_LD_RAM_HIGH_ADDR	        ?=
OBC_LD_VX_START_ADDR	        ?=$(OBC_LD_RAM_LOW_ADDR)

OBC_LD_DEFINES			:= -defsym _VX_DATA_ALIGN=0x1
OBC_LD_SCRIPT_FILE		?=

OBC_LOCAL_3RD_LIBS		?=

#*****************************************************************************
# complier config
#*****************************************************************************
TCLSH					:=$(WIND_HOME)/workbench-3.2/foundation/x86-linux2/bin/tclsh
TCL_SCRIPTS_PATH		        :=$(WIND_HOME)/vxworks-6.8/host/resource/hutils/tcl


CC					:= ccarm
VSB_CONFIG_FILE 		        := $(BALONG_TOPDIR)/build/libs/os/ccore/$(CFG_OS_LIB)/h/config/vsbConfig.h
CC_FLAGS				:= -c -fno-feature-proxy -fdollars-in-identifiers -O2 -DCPU=ARMARCH7 -DTOOL_FAMILY=diab -DTOOL=diab -D_WRS_KERNEL            -DARMEL -DPRJ_BUILD -D_WRS_KERNEL -D_VSB_CONFIG_FILE=\"$(VSB_CONFIG_FILE)\"

AR					:= ldarm
AR_FLAGS				:= -r

LD					:= $(HUTAF_HLT_WRAPPER) ldarm
LD_FLAGS				:= -X -N -EL -e sysInit -Ttext $(OBC_LD_VX_START_ADDR)
SIZE					?= vxsize

PACK_HEAD_TOOL        := $(BALONG_TOPDIR)/build/tools/utility/postlink/packHead/packHead.py

#********************************************************
# Dirs and Targets define
#********************************************************
PRJ_DEBUG_PATH         :=$(OBB_PRODUCT_DELIVERY_DIR)/tmp
LIB_LIBRARY_FILE       :=$(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)
IMAGE_TARGET_FILE      :=$(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)
PRJ_OBJ_PATH           :=$(OBB_PRODUCT_DELIVERY_DIR)/obj/

# pre link
PRELINK_PATH :=$(BALONG_TOPDIR)/build/libs/prelink
PRELINK_OBJ_PATH :=$(OBB_PRODUCT_DELIVERY_DIR)/obj/prelink_$(OBC_LOCAL_CORE_NAME)
PRELINK_LIB_TARGET ?=$(OBB_PRODUCT_DELIVERY_DIR)/lib/prelink_$(OBC_LOCAL_CORE_NAME).o
OS_LIB_PATH :=$(BALONG_TOPDIR)/build/libs/os/ccore/$(CFG_OS_LIB)/arm/ARMARCH7
OS_LIB_INC  := -L$(OS_LIB_PATH)/sfgnu_standard\
               -L$(OS_LIB_PATH)/sfgnu \
               -L$(OS_LIB_PATH)/sfcommon

INC_DIR     := -I$(BALONG_TOPDIR)/$(CFG_OS_PATH)/$(OBC_LOCAL_CORE_NAME) \
               -I$(OBB_PRODUCT_DELIVERY_DIR)/os/$(OBC_LOCAL_CORE_NAME) \
               -I$(PRELINK_PATH)/ \
               -I$(OBB_PRODUCT_CONFIG_DIR) \
			   -I$(OBB_PRODUCT_GU_CONFIG_DIR) \
			   -I$(OBB_CHIPSET_DIR)

IMAGE_FILE_DEBUG        :=$(PRJ_DEBUG_PATH)/$(OBC_LOCAL_MOUDLE_NAME)

# OS dependency Libs
ifeq ($(CFG_OS_LIB),VXWORKS.6.8.3.LIB.RELEASE)
ifeq ($(OBB_BUILD_TYPE),DEBUG)
LOCAL_OS_LIBS  := -lipappl-inet6 -lipcom-inet6 -lipcom_vxworks-inet6 -lipcrypto-inet6 -lipcrypto_apps-inet6 -lipcrypto_fips-inet6 -lipdhcpc6-inet6 -lipdhcps6-inet6 -lipike-inet6 -lipipsec2-inet6 -lipnet-inet6 -lipnet_krn-inet6 -lipripng-inet6 -lipssl-inet6 -liptcp-inet6 -lvxcompat-inet6 -lvxcoreip-inet6 -lvxmux-inet6 -lwrsnmp-inet6 -lstl -lcplus -laim -larch -lboot -lc_wr -lcci -lcommoncc -lcurl -ldcc -ldebug -ldevice -ldrv -ldshm -ldsi -lepdes -lerfLib -lfp -lgcc -lhwdb -los -lprocfs -lpxtrace -lrpc -lsnmp -lssi -ltffs -lusb -lusb2 -lvnode -lvxmp -lwdb -lwind -lwindview -lxbd -lspl
else
LOCAL_OS_LIBS  := -lipappl-inet6 -lipcom-inet6 -lipcom_vxworks-inet6 -lipcrypto-inet6 -lipcrypto_apps-inet6 -lipcrypto_fips-inet6 -lipdhcpc6-inet6 -lipdhcps6-inet6 -lipike-inet6 -lipipsec2-inet6 -lipnet-inet6 -lipnet_krn-inet6 -lipripng-inet6 -lipssl-inet6 -liptcp-inet6 -lvxcompat-inet6 -lvxcoreip-inet6 -lvxmux-inet6 -lwrsnmp-inet6 -lstl -lcplus -laim -larch -lboot -lc_wr -lcci -lcommoncc -lcurl -ldcc -ldebug -ldevice -ldrv -ldshm -ldsi -lepdes -lerfLib -lfp -lgcc -lhwdb -los -lprocfs -lpxtrace -lrpc -lsnmp -lssi -ltffs -lusb -lusb2 -lvnode -lvxmp -lwdb -lwind -lwindview -lxbd -lspl
endif
else
ifeq ($(OBB_BUILD_TYPE),DEBUG)
LOCAL_OS_LIBS  := -lstl -lcplus  -lvxTestV2 -laim -larch -lboot -lc_wr -lcommoncc -ldcc -ldebug -ldevice  -ldshm -ldsi -lepcommon -lepdes -lerfLib -lfp -lgcc -lhwdb -lipappl -lipcom -lipcom_vxworks -lipcrypto -lipcrypto_apps -lipcrypto_fips -lipnet -lipnet_krn -lipppp -lipradius -lipsntp -lipssl -liptcp -los -lprocfs -lpxtrace -lrpc -lsnmp -lssi -ltffs -lusb -lusb2 -lvnode -lvxbridge -lvxcompat -lvxcoreip -lvxmp -lvxmux -lvxsnmp -lwdb -lwind -lwindview -lwrsnmp -lxbd -lcplus  -lvxTestV2 -laim -larch -lboot -lc_wr -lcommoncc -ldcc -ldebug -ldevice -ldrv -ldshm -ldsi -lepcommon -lepdes -lerfLib -lfp -lgcc -lhwdb -lipappl -lipcom -lipcom_vxworks -lipcrypto -lipcrypto_apps -lipcrypto_fips -lipnet -lipnet_krn -lipppp -lipradius -lipsntp -lipssl -liptcp -los -lprocfs -lpxtrace -lrpc -lsnmp -lssi -ltffs -lusb -lusb2 -lvnode -lvxbridge -lvxcompat -lvxcoreip -lvxmp -lvxmux -lvxsnmp -lwdb -lwind -lwindview -lwrsnmp -lxbd -lspl
else
LOCAL_OS_LIBS  := -lstl -lcplus  -laim -larch -lboot -lc_wr -lcommoncc -ldcc -ldebug -ldevice -ldrv -ldshm -ldsi -lerfLib -lfp -lgcc -lhwdb -lipappl -lipcom -lipcom_vxworks -lipcrypto -lipnet -lipnet_krn -liptcp -los -lprocfs -lpxtrace -lrpc -lssi -ltffs -lusb -lusb2 -lvnode -lvxcompat -lvxcoreip -lvxmp -lvxmux -lwdb -lwind -lwindview -lxbd -lcplus  -laim -larch -lboot -lc_wr -lcommoncc -ldcc -ldebug -ldevice -ldrv -ldshm -ldsi -lerfLib -lfp -lgcc -lhwdb -lipappl -lipcom -lipcom_vxworks -lipcrypto -lipnet -lipnet_krn -liptcp -los -lprocfs -lpxtrace -lrpc -lssi -ltffs -lusb -lusb2 -lvnode -lvxcompat -lvxcoreip -lvxmp -lvxmux -lwdb -lwind -lwindview -lxbd -lspl
endif
endif

# LD Lib define
LIB_FILES       := $(foreach val,$(OBC_LOCAL_LIB_DIR),$(wildcard $(val)/*.o $(val)/*.a))
LOCAL_USER_LIBS := $(LIB_FILES) $(OBC_LOCAL_LIB_FILE)

FULL_LIB_PATH     := $(foreach lib_path, $(LOCAL_LIB_DIR), -L$(lib_path))

#*************************************************************************
# rules
#*************************************************************************
.PHONY: all pre_link do_link do_img post_link do_all_before
all: pre_link do_link do_img post_link do_all_before
	@echo - do [$@]

### Rules order
do_all_before :post_link
post_link :do_img
do_img : do_link
do_link : pre_link
pre_link :
### Rules order end

#
# pre_link
#
pre_link: $(PRELINK_LIB_TARGET)
	@echo do [$@]

$(PRELINK_LIB_TARGET): $(LOCAL_USER_LIBS)
	echo $(LOCAL_USER_LIBS)
	$(Q)mkdir -p $(PRELINK_OBJ_PATH)
ifeq ($(CFG_ENABLE_SHELL_SYM),YES)
	@echo [AR] $(PRELINK_OBJ_PATH)/SymImage.o
	$(Q)$(AR) $(AR_FLAGS) -o $(PRELINK_OBJ_PATH)/SymImage.o $(OS_LIB_INC) $(LOCAL_USER_LIBS) $(LOCAL_OS_LIBS)
	$(Q)cp -f $(V) $(PRELINK_PATH)/ctdt.c.dummy   $(PRELINK_OBJ_PATH)/ctdt.c
	$(Q)$(TCLSH) $(TCL_SCRIPTS_PATH)/makeSymTbl.tcl arm $(PRELINK_OBJ_PATH)/SymImage.o $(PRELINK_OBJ_PATH)/symTbl.c
else
	$(Q)cp -f $(PRELINK_PATH)/symTbl.c.dummy $(PRELINK_OBJ_PATH)/symTbl.c
	$(Q)cp -f $(PRELINK_PATH)/ctdt.c.dummy   $(PRELINK_OBJ_PATH)/ctdt.c
endif
	$(Q)cp -f $(PRELINK_PATH)/symbol.h $(PRELINK_OBJ_PATH)/symbol.h
	$(Q)chmod -R 777 $(PRELINK_OBJ_PATH)
	@echo [CC] $(PRELINK_OBJ_PATH)/cdtd.o
	$(Q)$(CC) $(CC_FLAGS) $(INC_DIR) -c $(PRELINK_OBJ_PATH)/ctdt.c -o $(PRELINK_OBJ_PATH)/ctdt.o
	@echo [CC] $(PRELINK_OBJ_PATH)/symTbl.c
	$(Q)$(CC) $(CC_FLAGS) $(INC_DIR) -c $(PRELINK_OBJ_PATH)/symTbl.c -o $(PRELINK_OBJ_PATH)/symTbl.o
	@echo [AR] $(PRELINK_LIB_TARGET)
	$(Q)$(AR) $(AR_FLAGS) -o $(PRELINK_LIB_TARGET) $(PRELINK_OBJ_PATH)/ctdt.o $(PRELINK_OBJ_PATH)/symTbl.o

# Image file (.elf) to generate
do_link:$(LIB_LIBRARY_FILE).elf
	@echo do [$@]

$(LIB_LIBRARY_FILE).elf :  $(OBC_LOCAL_3RD_LIBS) $(LOCAL_USER_LIBS) $(OBC_LD_SCRIPT_FILE) $(PRELINK_LIB_TARGET) | pre_link
	@echo [LD] $@
	$(Q)$(LD) $(LD_FLAGS) $(LOCAL_BOOT_OBJS) \
	$(OBC_LOCAL_3RD_LIBS) $(LOCAL_USER_LIBS) $(PRELINK_LIB_TARGET) $(OBC_LD_DEFINES) -T $(OBC_LD_SCRIPT_FILE) --start-group $(OS_LIB_INC) $(LOCAL_OS_LIBS) \
	--end-group -o $(LIB_LIBRARY_FILE).elf
#	$(Q)$(SIZE) -v $(OBC_LD_RAM_HIGH_ADDR) $(OBC_LD_RAM_LOW_ADDR) $(LIB_LIBRARY_FILE).elf

# Bin file (.bin)
post_link:$(IMAGE_TARGET_FILE).bin
	@echo do [$@]

$(IMAGE_TARGET_FILE).bin: $(LIB_LIBRARY_FILE).elf | do_link
	@echo [BIN] $@
	$(Q)objcopyarm -O binary --binary-without-bss $(LIB_LIBRARY_FILE).elf $(IMAGE_TARGET_FILE).bin
ifeq ($(strip $(CFG_ROM_COMPRESS)),YES)
	$(Q)$(WIND_HOME)/vxworks-6.8/host/x86-linux2/bin/deflate < $(IMAGE_TARGET_FILE).bin > $(IMAGE_TARGET_FILE).bin.gz
	$(Q)$(WIND_HOME)/vxworks-6.8/host/x86-linux2/bin/binToAsm $(IMAGE_TARGET_FILE).bin.gz > $(IMAGE_TARGET_FILE).bin.gz.s
	$(Q)$(CC) $(CC_FLAGS) $(IMAGE_TARGET_FILE).bin.gz.s -o $(IMAGE_TARGET_FILE).bin.gz.o
	$(Q)$(LD) -X -EL -e romInit -Ttext $(CFG_MCORE_TEXT_START_ADDR_COMPRESSED) $(OBB_PRODUCT_DELIVERY_DIR)/lib/romCompress.a $(IMAGE_TARGET_FILE).bin.gz.o --start-group $(OS_LIB_INC) $(LOCAL_OS_LIBS) \
	--end-group -o $(IMAGE_TARGET_FILE).gz.elf
	$(Q)rm -f $(IMAGE_TARGET_FILE).bin $(IMAGE_TARGET_FILE).bin.gz $(IMAGE_TARGET_FILE).bin.gz.s $(IMAGE_TARGET_FILE).bin.gz.o
	$(Q)objcopyarm -O binary --binary-without-bss $(LIB_LIBRARY_FILE).gz.elf $(IMAGE_TARGET_FILE).bin
endif

.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)-rm -f $(LIB_LIBRARY_FILE).elf $(IMAGE_TARGET_FILE).bin $(IMAGE_FILE_DEBUG).elf
	$(Q)-rm -f $(PRELINK_LIB_TARGET) $(PRELINK_OBJ_PATH)/ctdt.c $(PRELINK_OBJ_PATH)/symTbl.c \
							$(PRELINK_OBJ_PATH)/ctdt.o $(PRELINK_OBJ_PATH)/symTbl.o
	$(Q)-rm -rf $(PRELINK_OBJ_PATH)
	$(Q)-rm -rf $(OBC_LOCAL_CLEAN_DIRS)