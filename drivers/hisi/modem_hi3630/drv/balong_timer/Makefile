-include $(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config/balong_product_config.mk
ifeq ($(strip $(CFG_OS_ANDROID_USE_K3V3_KERNEL)),YES)
	ifeq ($(strip $(CFG_CONFIG_MODULE_TIMER)),YES)
	obj-$(CONFIG_HARD_TIMER)	+= hardtimer_k3.o
	obj-$(CONFIG_SOFT_TIMER)	+= softtimer_balong.o
	obj-$(CONFIG_ENABLE_TEST_CODE)	+= hardtimer_k3_test.o
	obj-$(CONFIG_ENABLE_TEST_CODE)     += softtimer_balong_test.o
	endif
else
ifeq ($(strip $(CFG_CONFIG_MODULE_TIMER)),YES)
obj-$(CONFIG_HARD_TIMER)	+= hardtimer_balong.o
obj-$(CONFIG_SOFT_TIMER)	+= softtimer_balong.o
obj-$(CONFIG_ENABLE_TEST_CODE)     += hardtimer_balong_test.o
obj-$(CONFIG_ENABLE_TEST_CODE)     += softtimer_balong_test.o
endif
endif
