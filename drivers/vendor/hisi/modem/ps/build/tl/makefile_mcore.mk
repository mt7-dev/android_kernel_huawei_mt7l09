
#***********************************************************#
# include the define at the top
#***********************************************************#
include $(BALONG_TOPDIR)/build/scripts/make_base.mk

MOUDULES:= tlnas tlas 
	
# process
.PHONY: tlnas tlas
lps: $(MOUDULES)

tlnas:
	$(MAKE)  $(OBB_JOBS)  -f MODEM_CORE/makefile_tlnas.mk
tlas:
	$(MAKE)  $(OBB_JOBS)  -f MODEM_CORE/makefile_tlas.mk	

CLEAN_MOUDULES:= clean-tlnas clean-tlas

.PHONY: clean clean-tlnas clean-tlas

clean : clean-tlnas clean-tlas

clean-tlnas:
	$(MAKE)  $(OBB_JOBS)  -f MODEM_CORE/makefile_tlnas.mk clean
clean-tlas:
	$(MAKE)  $(OBB_JOBS)  -f MODEM_CORE/makefile_tlas.mk	clean

clean-lps: $(CLEAN_MOUDULES)
