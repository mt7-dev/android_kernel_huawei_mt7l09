# Makefile - make file for onchiprom

LIB := libemmc.a

COBJFILE	= emmcBoot.o emmcMain.o emmcNormal.o emmcIdmac.o emmcTest.o

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