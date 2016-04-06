####################################################################################################
#
####################################################################################################
MSP_SRC_DIR				:=$(BALONG_SRC_PATH)/modem/oam
MSP_INC_DIR				:=$(MSP_SRC_DIR)/inc
PRJ_CONFIG_DIR          :=$(BALONG_SRC_PATH)/config
PRJ_INCLUDE_DIR         :=$(BALONG_SRC_PATH)/include
PRJ_PLATFORM_DIR        :=$(BALONG_SRC_PATH)/platform

####################################################################################################
#Directories for include files
####################################################################################################
vob_include_dirs :=   $(WIND_BASE)/target/h \
					  $(WIND_BASE)/target/h/wrn/coreip \
					  $(WIND_HOME)/vxworks-6.8/target/h \
					  $(WIND_HOME)/vxworks-6.8/target/h/types \
					  $(WIND_HOME)/vxworks-6.8/target/h/wrn/coreip \
					  $(WIND_HOME)/components/ip_net2-6.8/vxcompat/include \
					  $(PRJ_CONFIG_DIR)/product/include/$(PRODUCT_CFG_PRODUCT_FORM) \
					  $(PRJ_PLATFORM_DIR)/$(PRODUCT_CFG_PLATFORM_NEW) \
					  $(PRJ_CONFIG_DIR)\log \
					  $(PRJ_CONFIG_DIR)\nvim\include\gu \
					  $(PRJ_CONFIG_DIR)\nvim\data\comm \
					  $(PRJ_CONFIG_DIR)\osa \
					  $(PRJ_INCLUDE_DIR)\app \
					  $(PRJ_INCLUDE_DIR)\drv \
					  $(PRJ_INCLUDE_DIR)\nv\tl\drv \
					  $(PRJ_INCLUDE_DIR)\nv\tl\oam \
					  $(PRJ_INCLUDE_DIR)\med \
					  $(PRJ_INCLUDE_DIR)\oam\comm\om \
					  $(PRJ_INCLUDE_DIR)\oam\comm\osa \
					  $(PRJ_INCLUDE_DIR)\oam\comm\scm \
					  $(PRJ_INCLUDE_DIR)\oam\comm\si \
					  $(PRJ_INCLUDE_DIR)\oam\comm\socp \
					  $(PRJ_INCLUDE_DIR)\oam\comm\usimm \
					  $(PRJ_INCLUDE_DIR)\oam\comm\errno \
					  $(PRJ_INCLUDE_DIR)\oam\comm\dms \
					  $(PRJ_INCLUDE_DIR)\oam\comm\cpm \
					  $(PRJ_INCLUDE_DIR)\oam\comm\nvim \
                      $(PRJ_INCLUDE_DIR)\oam\comm\cbpa \
					  $(PRJ_INCLUDE_DIR)\oam\gu\log \
					  $(PRJ_INCLUDE_DIR)\oam\gu\om \
					  $(PRJ_INCLUDE_DIR)\oam\gu\nvim \
					  $(PRJ_INCLUDE_DIR)\oam\gu\hpa \
					  $(PRJ_INCLUDE_DIR)\oam\gu\log \
					  $(PRJ_INCLUDE_DIR)\oam\gu\om \
					  $(PRJ_INCLUDE_DIR)\nv\gu\oam \
					  $(PRJ_INCLUDE_DIR)\nv\gu\codec \
					  $(PRJ_INCLUDE_DIR)\nv\gu\drv \
					  $(PRJ_INCLUDE_DIR)\nv\gu\gas \
					  $(PRJ_INCLUDE_DIR)\nv\gu\nas \
					  $(PRJ_INCLUDE_DIR)\nv\gu\phy \
					  $(PRJ_INCLUDE_DIR)\nv\gu\ttf \
					  $(PRJ_INCLUDE_DIR)\nv\gu\was \
					  $(PRJ_INCLUDE_DIR)\nv\gu\tool \
					  $(PRJ_INCLUDE_DIR)\phy\wphy \
					  $(PRJ_INCLUDE_DIR)\ps\nas \
					  $(PRJ_INCLUDE_DIR)\gu \
					  $(PRJ_INCLUDE_DIR)\taf \
					  $(MSP_INC_DIR)\comm\acore\dms \
					  $(MSP_INC_DIR)\comm\ccore\om \
					  $(MSP_INC_DIR)\comm\ccore\usimm \
					  $(MSP_INC_DIR)\comm\ccore\xml \
					  $(MSP_INC_DIR)\comm\ccore\dh \
					  $(MSP_INC_DIR)\comm\comm\si \
					  $(MSP_INC_DIR)\comm\comm\nvim \
					  $(MSP_INC_DIR)\comm\ccore\cbpa \
					  $(MSP_INC_DIR)\gu\acore\om \
					  $(MSP_INC_DIR)\gu\ccore\hpa \
					  $(MSP_INC_DIR)\gu\ccore\nvim \
					  $(MSP_INC_DIR)\gu\ccore\om \
					  $(MSP_INC_DIR)\gu\ccore\pstool \
					  $(MSP_INC_DIR)\gu\ccore\sc \
					  $(MSP_INC_DIR)\gu\ccore\sleep \
					  $(MSP_INC_DIR)\gu\comm\om \


ifeq ($(CFG_FEATURE_MERGE_OM_CHAN),FEATURE_ON)
vob_include_dirs += \
					$(MSP_INC_DIR)\comm\comm\ppm
endif					  

ifneq ($(CFG_RAT_MODE),RAT_GU)
vob_include_dirs += \
					  $(PRJ_INCLUDE_DIR)\oam\lt\msp
endif					  
					  
					  