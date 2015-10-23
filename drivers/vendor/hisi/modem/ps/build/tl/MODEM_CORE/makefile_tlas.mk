
#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#***********************************************************#
# moudle name & core define
#***********************************************************#
OBC_LOCAL_CORE_NAME		?=mcore
OBC_LOCAL_MOUDLE_NAME	?=tlas_ccore

#***********************************************************#
# compiler flags
#***********************************************************#
CC_USER_FLAGS   := -DM1_PORTING -DPEEK_DEBUG  -DUSE_ENODEB_2 -DPROCESS_TRACE -DVOS_OSA_CPU=0 -DBSP_CORE_MODEM
ifeq ($(strip $(CFG_FEATURE_UE_MODE_TDS)),FEATURE_ON)
CC_USER_FLAGS   += \
			-DSTACK_TD_EDGE_DM -DMEMO_BLOCK -DVERS_TD_ONLY_IP -DYNMIC_LOAD_DSP -DTDSCDMA_ONLY -DRRC_ASN_EXTEND -DBSP_CORE_MODEM
endif #end of CFG_FEATURE_UE_MODE_TDS
AS_USER_FLAGS   := 

#***********************************************************#
# Source File to compile
#***********************************************************#
PRJ_CONFIG_DIR  :=$(BALONG_TOPDIR)/config
PRJ_COMM_DIR    :=$(BALONG_TOPDIR)/include
PRJ_PLATFORM_DIR:=$(BALONG_TOPDIR)/platform
PS_DIR			:=$(BALONG_TOPDIR)/modem/ps
PS_COMM_DIR  	:=$(PS_DIR)/comm
PS_INC_DIR    	:=$(PS_DIR)/inc
PS_AS_SRC_DIR   :=$(PS_DIR)/as
PS_NAS_SRC_DIR  :=$(PS_DIR)/nas

PS_LTE_L2_SRC   := $(PS_AS_SRC_DIR)/tl/lte/l2/com/Src \
                   $(PS_AS_SRC_DIR)/tl/lte/l2/mac/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/l2/pdcp/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/l2/rlc/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/l2/rohc/src

PS_LTE_RRC_SRC  := $(PS_AS_SRC_DIR)/tl/lte/rrc/Cmm/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Csel/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Cselpc/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Cselbg/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Itf/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Main/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Rb/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Reest/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Rrc/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Sib/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Smc/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/rrc/Asn1/Src 

PS_TDS_COMM_SRC := $(PS_AS_SRC_DIR)/tl/tds/comm/hl1/source \
                   $(PS_AS_SRC_DIR)/tl/tds/comm/rtos_ps/source \
                   $(PS_AS_SRC_DIR)/tl/tds/comm/tgl/source \
                   $(PS_AS_SRC_DIR)/tl/tds/comm/tgl_ps/source \
                   $(PS_AS_SRC_DIR)/tl/tds/comm/tgl_ps_pf/source \
                   $(PS_AS_SRC_DIR)/tl/tds/Stub/Pcstub
				   
PS_TDS_L2_SRC   := $(PS_AS_SRC_DIR)/tl/tds/l2/Mac/Src \
                   $(PS_AS_SRC_DIR)/tl/tds/l2/pdc/source \
                   $(PS_AS_SRC_DIR)/tl/tds/l2/rlc/source \
                   $(PS_AS_SRC_DIR)/tl/tds/l2/com/source 
	
PS_TDS_RRC_SRC  := $(PS_AS_SRC_DIR)/tl/tds/rrc/acc/source \
                   $(PS_AS_SRC_DIR)/tl/tds/rrc/asn/source \
				   $(PS_AS_SRC_DIR)/tl/tds/rrc/bmc/source \
				   $(PS_AS_SRC_DIR)/tl/tds/rrc/dum/source \
				   $(PS_AS_SRC_DIR)/tl/tds/rrc/idl/source \
				   $(PS_AS_SRC_DIR)/tl/tds/rrc/rrc/source

PS_LT_COMM_SRC  := $(PS_AS_SRC_DIR)/tl/lte/comm/HPA/Src \
                   $(PS_AS_SRC_DIR)/tl/lte/comm/PSOM/Src \
				   $(PS_AS_SRC_DIR)/tl/lte/comm/Stub/Boardstub/Comm
			   
PS_TDS_SRC := $(PS_AS_SRC_DIR)/tl/tds/Prj

OBC_LOCAL_SRC_FILE := 
OBC_LOCAL_SRC_DIR  :=  $(PS_LTE_L2_SRC) $(PS_LTE_RRC_SRC) $(PS_LT_COMM_SRC) 

ifeq ($(strip $(CFG_FEATURE_UE_MODE_TDS)),FEATURE_ON)
OBC_LOCAL_SRC_DIR += $(PS_TDS_SRC)
endif #end of CFG_FEATURE_UE_MODE_TDS

OBC_LOCAL_INC_DIR :=\
		$(WIND_BASE)/target/h $(WIND_BASE)/target/h/wrn/coreip \
        $(PRJ_COMM_DIR)/drv \
        $(PRJ_COMM_DIR)/oam/comm \
		$(PRJ_COMM_DIR)/oam/lt/diag \
		$(PRJ_COMM_DIR)/oam/lt/msp \
		$(PRJ_COMM_DIR)/oam/gu/om \
		$(PRJ_COMM_DIR)/oam/gu/log \
		$(PRJ_COMM_DIR)/oam/gu/nvim\
        $(PRJ_COMM_DIR)/oam/comm/om \
		$(PRJ_COMM_DIR)/oam/comm/nvim \
		$(PRJ_COMM_DIR)/oam/comm/usimm \
		$(PRJ_COMM_DIR)/oam/comm/errno \
		$(PRJ_COMM_DIR)/oam/comm/osa\
		$(PRJ_COMM_DIR)/oam/comm/si\
		$(PRJ_COMM_DIR)/oam/usimm \
		$(PRJ_COMM_DIR)/nv/tl/lps \
		$(PRJ_COMM_DIR)/nv/tl/phy \
		$(PRJ_COMM_DIR)/nv/gu/gas \
		$(PRJ_COMM_DIR)/nv/gu/ttf \
		$(PRJ_COMM_DIR)/nv/gu/was \
		$(PRJ_COMM_DIR)/nv/gu/nas \
		$(PRJ_COMM_DIR)/phy/lphy \
		$(PRJ_COMM_DIR)/phy/lphy/om \
        $(PRJ_COMM_DIR)/ps/gups \
		$(PRJ_COMM_DIR)/ps/tlps \
		$(PRJ_COMM_DIR)/phy/wphy \
		$(PRJ_COMM_DIR)/med \
		$(PRJ_COMM_DIR)/ps/nas \
		$(PRJ_COMM_DIR)/taf \
		$(PRJ_COMM_DIR)/nv/gu/codec \
                $(PRJ_COMM_DIR)/nv/gu/oam \
		$(PRJ_COMM_DIR)/tools \
	    $(PS_COMM_DIR)/comm/CDS/Inc \
		$(PS_COMM_DIR)/comm/ND/NDCLIENT/Inc \
		$(PS_COMM_DIR)/comm/ND/IPCOMM/Inc \
		$(PS_COMM_DIR)/comm/LINK/Inc \
        $(PS_COMM_DIR)/comm/STL/Inc \
		$(PS_COMM_DIR)/comm/MEM/Inc \
		$(PS_COMM_DIR)/tl/lte/inc/Comm \
		$(PS_COMM_DIR)/tl/lte/inc/Hso \
		$(PS_COMM_DIR)/tl/lte/inc/Interface \
		$(PRJ_CONFIG_DIR)/osa \
		$(PRJ_CONFIG_DIR)/log \
		$(PRJ_CONFIG_DIR)/nvim/include/tl/LPHY \
		$(PRJ_CONFIG_DIR)/nvim/include/tl/tlps \
		$(PRJ_CONFIG_DIR)/nvim/include/tl/MSP \
		$(PRJ_CONFIG_DIR)/nvim/include/gu \
		$(PRJ_CONFIG_DIR)/product/include/$(OBB_PRODUCT_NAME) \
		$(PRJ_CONFIG_DIR)/product/define \
		$(PS_INC_DIR)/gu \
		$(PS_INC_DIR)/tl \
		$(PS_AS_SRC_DIR)/tl/lte/comm/HPA/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/comm/PSOM/Inc \
        $(PS_AS_SRC_DIR)/tl/lte/rrc/Rrc/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Csel/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Cmm/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Inc \
        $(PS_AS_SRC_DIR)/tl/lte/rrc/Main/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Smc/Inc \
        $(PS_AS_SRC_DIR)/tl/lte/rrc/Sib/Inc \
        $(PS_AS_SRC_DIR)/tl/lte/rrc/Reest/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Cselpc/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Cselbg/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Rb/Inc \
	    $(PS_AS_SRC_DIR)/tl/lte/rrc/Itf/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/rrc/Asn1/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/l2/mac/Inc \
        $(PS_AS_SRC_DIR)/tl/lte/l2/rlc/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/l2/rohc/inc \
		$(PS_AS_SRC_DIR)/tl/lte/l2/com/Inc \
		$(PS_AS_SRC_DIR)/tl/lte/l2/pdcp/Inc \
	    $(PS_AS_SRC_DIR)/tl/lte/l2/inc \
	    $(PS_NAS_SRC_DIR)/tl/lte/Inc \
	    $(PS_NAS_SRC_DIR)/tl/lte/COMM/INC \
        $(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/ATTACH/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/TAU/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/SECU/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/PUBU/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/GUTI/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/MRRC/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/PLMN/INC \
        $(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/IDEN/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/DETACH/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/EMM/SRC/SERVICE/INC \
        $(PS_NAS_SRC_DIR)/tl/lte/ESM/Inc \
		$(PS_NAS_SRC_DIR)/tl/lte/IP/Inc \
		$(PS_NAS_SRC_DIR)/tl/lte/IP/Src/IPM/Inc \
		$(PS_NAS_SRC_DIR)/tl/lte/MMC/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/PUBM/INC \
		$(PS_NAS_SRC_DIR)/tl/lte/RABM/Inc \
		$(PS_NAS_SRC_DIR)/tl/lte/TC/Inc \
		$(PS_NAS_SRC_DIR)/tl/lte/USIM/Inc \
		$(PS_NAS_SRC_DIR)/gu/src/Mmc/Inc \
		$(PS_NAS_SRC_DIR)/comm/mml/INC  \
		$(PS_AS_SRC_DIR)/tl/tds/comm/hl1/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/rtos_ps/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl_pa_pf/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl_ps/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl_ps/source \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl_ps_pa/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl_ps_pf/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/rtos_ps/include \
		$(PS_AS_SRC_DIR)/tl/tds/comm/rtos_ps/source \
		$(PS_AS_SRC_DIR)/tl/tds/comm/tgl/source \
		$(PS_AS_SRC_DIR)/tl/tds/comm/hl1/include  \
		$(PS_AS_SRC_DIR)/tl/tds/comm/hl1/source  \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/acc/include \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/acc/source \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/asn/source \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/asn/include \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/rrc/include \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/rrc/source \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/idl/include \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/idl/source \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/dum/source \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/dum/include \
		$(PS_AS_SRC_DIR)/tl/tds/rrc/bmc/include \
		$(PS_AS_SRC_DIR)/tl/tds/l2/pdc/include \
		$(PS_AS_SRC_DIR)/tl/tds/l2/pdc/source \
		$(PS_AS_SRC_DIR)/tl/tds/l2/Mac/Inc \
		$(PS_AS_SRC_DIR)/tl/tds/l2/Mac/Src \
		$(PS_AS_SRC_DIR)/tl/tds/l2/rlc/include \
		$(PS_AS_SRC_DIR)/tl/tds/l2/rlc/source \
		$(PS_AS_SRC_DIR)/tl/tds/l2/com/include \
		$(PS_AS_SRC_DIR)/tl/tds/Stub/Pcstub \
		$(PS_AS_SRC_DIR)/tl/tds/l2/com/source 
		
#***********************************************************
#include rules. must be droped at the bottom, OBB_BUILD_ACTION values: cc tqe lint fortify
#***********************************************************
include $(BALONG_TOPDIR)/build/scripts/rules/$(OBB_BUILD_ACTION)_vxworks6.8_rules.mk
