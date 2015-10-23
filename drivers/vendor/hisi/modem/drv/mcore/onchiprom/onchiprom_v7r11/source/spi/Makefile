# Makefile - make file for onchiprom

LIB := libspi.a

COBJFILE	= spi.o spiDev.o #testSpi.o

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