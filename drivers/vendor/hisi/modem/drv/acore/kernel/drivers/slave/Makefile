#
# Makefile for the arasan sdio.
#
ccflags-$(CONFIG_SDIO_ARASAN_DEBUG)	:= -DDEBUG

obj-$(CONFIG_SDIO_ARASAN)		+= sdcc.o
sdcc-objs		:= sdio_slave_hal.o sdio_slave_osi.o sdio_slave_osr.o debugfs.o
