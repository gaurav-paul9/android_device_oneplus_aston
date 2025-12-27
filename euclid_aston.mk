#
# Copyright (C) 2021-2025 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit_only.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from aston device
$(call inherit-product, device/oneplus/aston/device.mk)

# Inherit some common euclid stuff.
$(call inherit-product, vendor/euclid/config/common_full_phone.mk)

# Euclid Stuff
#For GMS
WITH_GMS := true
TARGET_BUILD_DOTGALLERY := true
TARGET_BUILD_BCR := true
EUCLID_DEVICE := OnePlus_12R
EUCLID_PROCESSOR := Snapdragon_8_Gen_2

# Maintainer
EUCLID_BUILD_TYPE := UNOFFICIAL
EUCLID_MAINTAINER := Gaurav Paul

#Gapps
EUCLID_GAPPS := true
TARGET_SUPPORTS_GOOGLE_RECORDER := true
TARGET_INCLUDE_STOCK_ARCORE := true
TARGET_INCLUDE_LIVE_WALLPAPERS := true
TARGET_INCLUDE_PIXEL_LAUNCHER := true

#UDFPS
EXTRA_UDFPS_ANIMATIONS := true
TARGET_HAS_UDFPS := true

#Misc.
TARGET_SUPPORTS_TOUCHGESTURES := true

PRODUCT_NAME := lineage_aston
PRODUCT_DEVICE := aston
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_BRAND := OnePlus
PRODUCT_MODEL := CPH2585

PRODUCT_GMS_CLIENTID_BASE := android-oneplus

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="qssi-user 16 BP2A.250605.015 1763371040443 release-keys" \
    BuildFingerprint=OnePlus/CPH2585IN/OP5D35L1:16/TP1A.220905.001/U.R4T3.2d10abf-c190f3-c190f4:user/release-keys \
    DeviceName=OP5D35L1 \
    DeviceProduct=CPH2585 \
    SystemDevice=OP5D35L1 \
    SystemName=CPH2585
