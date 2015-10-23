#
# Makefile for the linux kernel.
#


obj-y		:= timer.o lm.o common.o


obj-$(CONFIG_HAVE_CPU_IDLE)+=  governor_balong.o
obj-$(CONFIG_HAVE_CPU_IDLE)+=  cpuidle_balong.o
obj-$(CONFIG_HAVE_CPU_IDLE)+=  sleep_balong.o


obj-$(CONFIG_ARCH_TEST) += board-test.o
obj-$(CONFIG_ARCH_P531) += board-p531.o io-p531.o
obj-$(CONFIG_ARCH_V7R2) += board-v7r2.o io-v7r2.o
obj-$(CONFIG_ARCH_V711) += board-v711.o io-v711.o

obj-$(CONFIG_BALONG_EASY_SHELL) += easy_shell.o

obj-$(CONFIG_BALONG_PCIE_RC) += pcie_rc.o
obj-$(CONFIG_BALONG_PCIE_EP) += pcie_ep.o
## PM ##
obj-$(CONFIG_BALONG_PM_SUSPEND) += pm.o gic_pm.o
obj-$(CONFIG_BALONG_PM_SUSPEND) += pm_asm_deepsleep.o

obj-$(HUTAF_HLT_LTCOV)	+= ltcov.o


KBUILD_CPPFLAGS +=	-Idrivers/modem/om/dump
