ccflags-y += -Idrivers/media/huawei/camera/sensor
ccflags-y += -Idrivers/media/huawei/camera
ccflags-y += -Idrivers/media/huawei/camera/csi
ccflags-y += -Idrivers/media/huawei/camera/platform

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_FAKE_SENSOR),y) 
	obj-y += fake-sensor.o
endif

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_IMX135),y)
	obj-y += imx135.o
endif

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_IMX214),y)
	obj-y += imx214.o
endif

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_OV5648),y) 
	obj-y += ov5648.o
endif

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_IMX179_REAR),y)
	obj-y += imx179_rear.o
endif

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_IMX179_FRONT),y)
	obj-y += imx179_front.o
endif

ifeq ($(CONFIG_HUAWEI_CAMERA_USE_OV8865_REAR),y)
	obj-y += ov8865_rear.o
endif