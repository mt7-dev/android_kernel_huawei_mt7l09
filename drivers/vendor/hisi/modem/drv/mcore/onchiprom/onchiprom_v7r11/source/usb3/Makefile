# Makefile - make file for onchiprom

LIB := libusb3.a

COBJFILE	= usb3_drv.o usb3_pcd.o usb3_intr.o usb3_pcd_intr.o usb3_prot.o

SOBJFILE	= 

all: $(LIB)

$(LIB): $(SOBJFILE) $(COBJFILE) 
	$(AR) $(ARFLAGS) $@ $^

$(COBJFILE):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@
$(SOBJFILE):%.o:%.s
	$(CC) -c $(ASMFLAGS) $< -o $@

clean:
	@rm -rf ./*.o *.a