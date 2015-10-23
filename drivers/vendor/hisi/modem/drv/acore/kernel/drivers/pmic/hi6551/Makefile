#
# Makefile for pmic hi6551 drivers.
#

obj-y += pmu_hi6551.o
obj-y += irq_hi6551.o
obj-y += exc_hi6551.o
obj-y += common_hi6551.o
obj-y += debug_hi6551.o
obj-y += dr_hi6551.o

obj-$(CONFIG_HI6551_COUL) += coul_hi6551.o
#obj-y += coul_hi6551_test.o

obj-$(CONFIG_ENABLE_TEST_CODE) += pmu_hi6551_test.o