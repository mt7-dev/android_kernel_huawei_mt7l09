# Below projects/packages with LOCAL_MODULEs will be used by
# PRODUCT_PACKAGES to build LOCAL_MODULEs that are tagged with
# optional tag, which will not be available on target unless
# explicitly list here. Where project corresponds to the vars here
# in CAPs.

#KERNEL_TESTS

PRODUCT_PACKAGES := \
     OpenWnn \
    PinyinIME \
    libWnnEngDic \
    libWnnJpnDic \
    libwnndict \
 
# Additional settings used in all AOSP builds
PRODUCT_PROPERTY_OVERRIDES := \
    ro.com.android.dateformat=MM-dd-yyyy \
    ro.config.ringtone=Ring_Synth_04.ogg \
    ro.config.notification_sound=pixiedust.ogg

# Put en_US first in the list, so make it default.
PRODUCT_LOCALES := en_US




$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_small.mk)

# Override - we don't want to use any inherited value
PRODUCT_MANUFACTURER := Hisilicon


