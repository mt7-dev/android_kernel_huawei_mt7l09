# Makefile - make file for onchiprom

SUBDIRS	:=  cipher \
			efuse \
			lib \
			secBoot \
			nand \
			usb3

ifeq ($(strip $(CFG_HSUART)),YES)
SUBDIRS += hsuart
endif

ifeq ($(strip $(CFG_SPI)),YES)
SUBDIRS += spi
endif

ifeq ($(strip $(CFG_EMMC)),YES)
SUBDIRS += emmc
endif

ifeq ($(strip $(CFG_HSIC)),YES)
SUBDIRS += hsic
endif

all: 
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir $@ ; done

clean:
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir clean $@ ; done