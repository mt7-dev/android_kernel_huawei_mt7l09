# Makefile - make file for onchiprom


LIB := libcipher.a

COBJFILE	= md5.o

ifeq ($(strip $(CFG_CIPHER)),CIPHER_HARDWARE)
COBJFILE	+= cipher.o
else
COBJFILE	+= sha2.o
endif

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