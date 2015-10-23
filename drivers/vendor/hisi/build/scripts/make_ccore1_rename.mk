include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#*****************************************************************************
# complier config
#*****************************************************************************
AR     :=ldarm 
ARFLAG :=-r -o

OBJCOPY 				:= objcopyarm
NM 						:= nm
GREP 					:= grep
SED 					:= sed
DUMP 					:= objdumparm

#*********************************************** 
# obc define
#*********************************************** 
OBC_LOCAL_CORE_NAME		:=ccore
OBC_LOCAL_MOUDLE_NAME	:=modemcore1_union
LIB_LIBRARY_FILE       :=$(OBB_PRODUCT_DELIVERY_DIR)/lib/$(OBC_LOCAL_MOUDLE_NAME)

ifeq ($(strip $(OBB_SEPARATE)),true)
CFG_CCORE_MODEM1_COMPONENTS := bsp_ccore1.o
endif

OBC_LOCAL_LIB_DIR		:=
OBC_LOCAL_LIB_FILE		:=$(addprefix $(OBB_PRODUCT_DELIVERY_DIR)/lib/,$(CFG_CCORE_MODEM1_COMPONENTS))

.PHONY: all

all:$(LIB_LIBRARY_FILE).o
$(LIB_LIBRARY_FILE).o:$(OBC_LOCAL_LIB_FILE)
	$(Q)$(AR) $(ARFLAG)  $(LIB_LIBRARY_FILE).o $(OBC_LOCAL_LIB_FILE)  
	$(NM) -g $(LIB_LIBRARY_FILE).o | $(GREP) -v " U " | $(SED) -e s/".* [^U] "// -e s/.*/"\0 I1_\0"/ > $(LIB_LIBRARY_FILE).cpy 
	$(OBJCOPY) --redefine-syms=$(LIB_LIBRARY_FILE).cpy  $(LIB_LIBRARY_FILE).o
	
