# Makefile - make file for onchiprom

LIB := libSecBoot.a

COBJFILE	= secBoot.o OcrShare.o

SOBJFILE	= romInit.o

all: $(LIB) $(SOBJFILE)

$(LIB): $(COBJFILE) 
	$(AR) $(ARFLAGS) $@ $^

$(COBJFILE):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@
$(SOBJFILE):%.o:%.s
	$(CC) -c $(ASMFLAGS) $< -o $@

clean:
	@rm -rf ./*.o *.a