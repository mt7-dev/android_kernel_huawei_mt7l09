
ccflags-$(CONFIG_PM_DEBUG)	:= -DDEBUG

obj-y				+= qos.o
obj-$(CONFIG_PM)		+= main.o
obj-$(CONFIG_VT_CONSOLE_SLEEP)	+= console.o
obj-$(CONFIG_FREEZER)		+= process.o
obj-$(CONFIG_SUSPEND)		+= suspend.o
obj-$(CONFIG_PM_TEST_SUSPEND)	+= suspend_test.o
obj-$(CONFIG_HIBERNATION)	+= hibernate.o snapshot.o swap.o user.o \
				   block_io.o
obj-$(CONFIG_PM_AUTOSLEEP)	+= autosleep.o
obj-$(CONFIG_WAKELOCK)		+= wakelock.o
obj-$(CONFIG_USER_WAKELOCK)	+= userwakelock.o
obj-$(CONFIG_SUSPEND_TIME)	+= suspend_time.o
obj-$(CONFIG_EARLYSUSPEND)	+= earlysuspend.o
obj-$(CONFIG_ENABLE_TEST_CODE)	+=  earlysuspend_test.o

obj-$(CONFIG_MAGIC_SYSRQ)	+= poweroff.o

obj-$(CONFIG_WAKELOCK_BALONG)		+= wakelock_debug.o

obj-$(CONFIG_PWC_MNTN_ACORE)		+= lowpower_mntn_balong.o
obj-$(CONFIG_ADP_PWC_MNTN_ACORE)    += adp_lowpower_mntn.o
# add for fb
obj-$(CONFIG_FB_EARLYSUSPEND)	+= fbearlysuspend.o

