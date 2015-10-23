obj-$(CONFIG_BALONG_PCIE_RC)		+= pcie_dma_hal.o pcie_dma_drv.o
obj-$(CONFIG_BALONG_PCIE_EP)        += pcie_ep_test.o

ifeq ($(CONFIG_BALONG_PCIE_RC), y)
obj-$(CONFIG_ENABLE_TEST_CODE)         += pcie_rc_test.o
endif

ifeq ($(CONFIG_BALONG_PCIE_EP), y)
obj-$(CONFIG_ENABLE_TEST_CODE)         += pcie_ep_test.o
endif