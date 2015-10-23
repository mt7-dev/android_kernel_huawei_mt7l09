# Makefile - make file for onchiprom

include mk.cfg

CROSS_COMPILE	?= /opt/4.5.1/bin/arm-linux-
CC		= $(CROSS_COMPILE)gcc
AR    	= $(CROSS_COMPILE)ar
AS		= $(CROSS_COMPILE)as
LINK	= $(CROSS_COMPILE)ld 
OBJCOPY = $(CROSS_COMPILE)objcopy 
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS	= -mcpu=cortex-m3 -mthumb -O2 -fno-builtin  -g -mlong-calls -Wall

CFLAGS	+= -DTEXT_ENTRY=$(CFG_TEXT_ENTRY) -DPLATFORM=$(CFG_PLATFORM)

ASMFLAGS = -mcpu=cortex-m3 -mthumb 
ASMFLAGS += -I$(PRJ_TOP_DIR)/include -I$(SRCDIR)/secBoot
ASMFLAGS += -D__ASSEMBLY__ -x assembler-with-cpp -DPLATFORM=$(CFG_PLATFORM)



#²âÊÔÓÃ
#CFLAGS	+= -DONCHIPROM_TEST
#CFLAGS	+= -DSPI_TEST
#CFLAGS	+= -DEMMC_DEBUG
#CFLAGS	+= -DSTART_TIME_TEST
CFG_CIPHER = CIPHER_HARDWARE
#CFG_CIPHER = CIPHER_SOFTWARE

ifeq ($(strip $(CFG_CIPHER)),CIPHER_HARDWARE)
CFLAGS		+=	-DCIPHER
endif

ifneq ($(strip $(CFG_PLATFORM)),PLATFORM_V7R2_PORTING)
ASMFLAGS	+=	-DCFG_A9_PLL
endif


CFLAGS	+= -I$(PRJ_TOP_DIR)/include \
		-I$(SRCDIR)/cipher \
		-I$(SRCDIR)/efuse \
		-I$(SRCDIR)/emmc \
		-I$(SRCDIR)/hsic \
		-I$(SRCDIR)/hsuart \
		-I$(SRCDIR)/lib \
		-I$(SRCDIR)/secBoot \
		-I$(SRCDIR)/spi \
		-I$(SRCDIR)/usb3

ARFLAGS = -r 

LIB_ALL =   $(SRCDIR)/secBoot/romInit.o \
			$(SRCDIR)/secBoot/libSecBoot.a \
			$(SRCDIR)/cipher/libcipher.a \
			$(SRCDIR)/efuse/libefuse.a \
			$(SRCDIR)/emmc/libemmc.a \
			$(SRCDIR)/hsic/libhsic.a \
			$(SRCDIR)/hsuart/libhsuart.a \
			$(SRCDIR)/spi/libspi.a \
			$(SRCDIR)/usb3/libusb3.a \
			$(SRCDIR)/lib/libLib.a
	

export CC AR AS LINK OBJCOPY OBJDUMP CFLAGS ASMFLAGS ARFLAGS OUTFILE SRCDIR PRJ_TOP_DIR CFG_PLATFORM CFG_CIPHER

all: $(OUTFILE).bin

$(OUTFILE).bin: $(OUTFILE).elf
	$(OBJCOPY) -O binary -S  $^ $@	
	mv $(OUTFILE).* ./bin

$(OUTFILE).elf: 
	make -C $(SRCDIR)
	$(LINK) -X -EL -N -Map $(OUTFILE).map -e Vectors -Ttext $(CFG_TEXT_ENTRY)  -o $@ --whole-archive $(LIB_ALL)
	@$(OBJDUMP) -d $(OUTFILE).elf > $(OUTFILE).asm
	@$(OBJDUMP) -x $(OUTFILE).elf > $(OUTFILE).sec

clean:
	@echo "make: removing objects of project"
	$(MAKE) -C $(SRCDIR) clean
	@rm -rf ./bin/*

cipher:
	make -C $(SRCDIR)/cipher

efuse:
	make -C $(SRCDIR)/efuse

emmc:
	make -C $(SRCDIR)/emmc

hsic:
	make -C $(SRCDIR)/hsic

hsuart:
	make -C $(SRCDIR)/hsuart
	
lib:
	make -C $(SRCDIR)/lib

sec:
	make -C $(SRCDIR)/secBoot

spi:
	make -C $(SRCDIR)/spi
	
usb:
	make -C $(SRCDIR)/usb3
	

ramInit:
	rm -rf $(SRCDIR)/test/ramInit/*.o
	make -C $(SRCDIR)/test/ramInit
	mv $(SRCDIR)/test/ramInit/ramInit.* ./bin
	
eda:
	$(CC) -c $(ASMFLAGS) $(SRCDIR)/test/eda.s -o eda.o
	$(LINK) -X -EL -N -Map eda.map -e eda -Ttext 0x2FFF4000  -o eda.elf eda.o 
	$(OBJDUMP) -d eda.elf > eda.asm
	$(OBJCOPY) -O binary eda.elf eda.bin  	
	mv eda.* ./bin

bl:
	$(CC) -c $(ASMFLAGS)  $(SRCDIR)/test/testBl.s -o testBl.o
	$(LINK) -X -EL -N -Map testBl.map -e testBl -Ttext 0x00000000  -o testBl.elf testBl.o 
	$(OBJCOPY) -O binary  testBl.elf testBl.bin
	mv testBl.* ./bin
		
bootrom:
	$(CC) -c $(ASMFLAGS) $(SRCDIR)/test/testLoader.s -o testLoader.o
	$(LINK) -X -EL -N -Map testLoader.map -e testLoader -Ttext 0x00000000  -o testLoader.elf testLoader.o 
	$(OBJCOPY) -O binary  testLoader.elf testLoader.bin
	mv testLoader.* ./bin
	
test:
	@echo "CFLAGS:$(CFLAGS)"
	@echo "LIB_ALL:$(LIB_ALL)"
	