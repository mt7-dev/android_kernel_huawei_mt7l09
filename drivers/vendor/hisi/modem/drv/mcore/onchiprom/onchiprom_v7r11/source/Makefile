# Makefile - make file for onchiprom

SUBDIRS	:=  cipher \
			efuse \
			emmc \
			hsuart \
			lib \
			secBoot \
			spi \
			nand \
			usb3

ifeq ($(strip $(CFG_HSIC)),YES)
SUBDIRS +=hsic
endif
all: 
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir $@ ; done

clean:
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir clean $@ ; done