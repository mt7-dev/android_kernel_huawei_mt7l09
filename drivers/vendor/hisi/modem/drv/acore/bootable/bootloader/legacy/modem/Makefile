CFLAGS  += -I$(PRJ_TOP_DIR)/drivers/modem \
		   -I$(PRJ_TOP_DIR)/include \
		   -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/config \
           -I$(BALONG_TOPDIR)/config/product/$(OBB_PRODUCT_NAME)/include_gu \
           -I$(BALONG_TOPDIR)/config/nvim/include/gu \
           -I$(BALONG_TOPDIR)/include/drv \
           -I$(BALONG_TOPDIR)/include/nv/tl/drv \
           -I$(BALONG_TOPDIR)/include/nv/tl/oam \
           -I$(BALONG_TOPDIR)/include/nv/tl/lps \
           -I$(BALONG_TOPDIR)/include/phy/lphy \
           -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM) \
           -I$(BALONG_TOPDIR)/platform/$(CFG_PLATFORM)/soc \
           -I$(BALONG_TOPDIR)/modem/drv/common/include
		   
LIB := lib_modem.a

COBJFILE	= modem.o nv_balong.o nv_xml_dec.o 

SOBJFILE	= balong_modem.l2cache.bin.o

all: $(LIB)
	export
$(LIB): $(SOBJFILE) $(COBJFILE)
	$(AR) $(ARFLAGS) $@ $^

$(COBJFILE):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@
$(SOBJFILE):%.o:%.S
	$(CC) -c $(ASMFLAGS) $< -o $@

clean:
	@rm -rf ./*.o *.a
