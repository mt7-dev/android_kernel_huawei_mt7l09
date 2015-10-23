

obj-$(CONFIG_COMMON_CLK)	+= clk.o clk-fixed-rate.o clk-gate.o \
				   clk-mux.o clk-divider.o

obj-$(CONFIG_BALONG_CLK)+= clkdev.o clock_balong.o clock_balong_hi6930.o clock_balong_ops.o clock_balong_debug.o 

obj-$(CONFIG_BALONG_CLOCK)+=   clock.o

ifeq ($(CONFIG_BALONG_CLK),y)
obj-$(CONFIG_ENABLE_TEST_CODE)+= clock_balong_test.o
endif
