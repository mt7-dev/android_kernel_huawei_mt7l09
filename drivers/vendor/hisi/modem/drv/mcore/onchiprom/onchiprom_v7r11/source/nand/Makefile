# Makefile - make file for onchiprom

LIB := libnand.a

COBJFILE	= nand.o 

all: $(LIB) 

$(LIB): $(COBJFILE) 
	$(AR) $(ARFLAGS) $@ $^

$(COBJFILE):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	@rm -rf ./*.o *.a