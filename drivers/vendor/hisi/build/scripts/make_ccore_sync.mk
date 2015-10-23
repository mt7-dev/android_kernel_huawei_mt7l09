
# include files
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

#****************************************************************************
# input parameters
#****************************************************************************
OBC_CORE_NAME                :=ccore
OBC_MOUDLE_NAME              :=hso
OBC_SERVER_ADDR              ?=$(OBB_SERVER_ADDR)
OBC_USERNAME                 ?=$(OBB_USERNAME)
OBC_PASSWORD                 ?=
OBC_SERVER_SOURCE_DIR        ?=$(OBB_SYNC_SOURCE_DIR)

# server dirs
OBC_LINUX_SOURCE_DIR         ?=$(OBC_SERVER_SOURCE_DIR)
SYNC_LINUX_SERVER_HOST       :=
OBC_SHELL_FUNC_NAME          :=all

# client dirs
OBC_WINDOWS_DESTINATION_DIR  ?=$(OBB_SYNC_DESTINATION_DIR)
SYNC_WINDOWS_CLIENT_HOST     :=$(OBC_USERNAME)@$(OBC_SERVER_ADDR)

# sync dirs
OBB_SYNC_INCLUDE_DIR          :=build/ config/ tool/
OBC_SYNC_EXCLUDE_DIR          :=*.pyc *.git/ *.repo/ include/ llt/ modem/ platform 
OBB_SYNC_INCLUDE_DIR          :=$(addprefix --include=,$(OBB_SYNC_INCLUDE_DIR))
OBC_SYNC_EXCLUDE_DIR          :=$(addprefix --exclude=,$(OBC_SYNC_EXCLUDE_DIR))

# sync config
MY_SSH                       := ssh $(SYNC_WINDOWS_CLIENT_HOST)
ifeq ($(OBB_VERBOSE),true)
SYNC_SLIENT := -v
else
SYNC_SLIENT := -q
endif
MY_SYNC                      := rsync -e ssh -rt $(SYNC_SLIENT)

#****************************************************************************
# rules
#****************************************************************************
.PHONY: all do_pre_build do_build do_post_build
		
all: do_pre_build do_build do_post_build
	@echo - do [$@]

### Rules order
ifneq ($(MAKECMDGOALS),clean-win32)
$(MAKECMDGOALS) : do_post_build
do_post_build : do_build
do_build : do_pre_build
else
$(MAKECMDGOALS) : clean
endif

#pragma local
do_pre_build:
	@echo - do [$@]
	@echo "Sync src ... $(OBC_MOUDLE_NAME) Code on Server($(OBC_SERVER_ADDR))."
	$(Q)$(MY_SSH) "mkdir -p $(OBC_WINDOWS_DESTINATION_DIR)" 
	$(Q)$(MY_SYNC) -m --delete --chmod=ugo=rwX  $(OBB_SYNC_INCLUDE_DIR) $(OBC_SYNC_EXCLUDE_DIR) $(OBC_LINUX_SOURCE_DIR) $(SYNC_WINDOWS_CLIENT_HOST):$(OBC_WINDOWS_DESTINATION_DIR)

#pragma local
do_build:
	@echo - do [$@]
	@echo "Build ... on Server($(OBC_SERVER_ADDR))...parameters:$(SHELL_CMD_ARGS)"
	$(Q)$(MY_SSH) " cd $(OBC_WINDOWS_DESTINATION_DIR)/hisi/build/ && python obuild.py product=$(OBB_PRODUCT_NAME) distcc=$(OBB_DISTCC) verbose=$(OBB_VERBOSE) jobs=$(OBB_JOBS_N) $(MAKECMDGOALS)"

do_post_build:
	@echo - do [$@]
	@echo "Sync targets ..."
	mkdir -p $(OBB_PRODUCT_DELIVERY_DIR)/img
	$(Q)$(MY_SYNC) -p $(SYNC_WINDOWS_CLIENT_HOST):$(OBC_WINDOWS_DESTINATION_DIR)/hisi/build/delivery/$(OBB_PRODUCT_NAME)/img/HiStudio.exe  $(OBB_PRODUCT_DELIVERY_DIR)/img

$(MAKECMDGOALS):
	@echo do [$@]...

# Clean up 
.PHONY: clean
clean:
	@echo -  do [$@]
	$(Q)$(MY_SSH) "rm -rf $(OBC_WINDOWS_DESTINATION_DIR)"
	@echo "clean $(OBC_LOCAL_MOUDLE_NAME) finish."