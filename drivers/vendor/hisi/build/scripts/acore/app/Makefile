include $(BALONG_TOPDIR)/build/scripts/make_base.mk

obj-m = app.o
app-objs = LPS.o GUPS.o LMSP.o GUMSP.o

ANDROID_KERN_DIR		?=$(BALONG_TOPDIR)/ANDROID/android-2.6.35
#****************************************************************************
# rules
#****************************************************************************
.PHONY:all
all:
	$(MAKE) -C $(ANDROID_KERN_DIR) $(KERNEL_FLAGS) M=$(CURDIR) modules V=$(VERBOSE) $(OBB_JOBS) 

.PHONY:clean
clean:
	$(MAKE) -s -C $(ANDROID_KERN_DIR)  $(KERNEL_FLAGS) M=$(CURDIR) clean V=$(VERBOSE) 