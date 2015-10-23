# Makefile - make file for onchiprom

SUBDIRS	:=  cipher \
			efuse \
			emmc \
			hsic \
			hsuart \
			lib \
			secBoot \
			spi \
			usb3

all: 
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir $@ ; done

clean:
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $$dir clean $@ ; done